#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
LMSK Encoder (Python) - based on LMSK spec + reference C encoder.

Features:
1) Input: image path (PNG/BMP/JPG/...)
2) If image has alpha: extract alpha channel -> compress to .lmsk
3) If image has no alpha: convert to grayscale (8-bit) -> compress to .lmsk
4) Optional output path; if omitted -> same directory, same stem + .lmsk
5) Raw mode off by default; enable via --raw
6) Disable gradient algorithm via --no-gradient
7) Set gradient tolerant (0~3) via --gradient-tolerant
"""

import argparse
import struct
import zlib
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Sequence, Tuple

try:
    from PIL import Image
except ImportError:
    raise SystemExit("Missing dependency: Pillow. Install with: pip install pillow")


# -----------------------------------------------------------------------------
# Constants (aligned with __lmsk_common.h)
# -----------------------------------------------------------------------------
ARM_LMSK_VERSION_MAJOR = 1
ARM_LMSK_VERSION_MINOR = 2
ARM_LMSK_VERSION = ((ARM_LMSK_VERSION_MAJOR & 0xF) << 4) | (ARM_LMSK_VERSION_MINOR & 0xF)

TAG_U8_ALPHA = 0xFD
TAG_U8_GRADIENT = 0xF9

END_MARK = 0xDEADBEEF

FLOOR_SIZE = 1 << 16  # tagSetBits=0 => 65536


# -----------------------------------------------------------------------------
# Helpers: header packing (16 bytes)
# -----------------------------------------------------------------------------
def pack_header(width: int,
                height: int,
                alpha_bits: int,
                raw: bool,
                floor_count: int,
                tagset_bits: int = 0) -> bytes:
    """
    Pack arm_lmsk_header_t (16 bytes).
    Layout based on spec and __lmsk_common.h:
        chName[5] = "LMSK\0"
        Version    = (major<<4)|minor
        iWidth     = int16
        iHeight    = int16
        settingBits= u3AlphaMSBCount:3 | bRaw:1 | u2TagSetBits:2 | reserved:2
        chFloorCount= uint8
        reserved32 = uint32
    """
    if not (1 <= alpha_bits <= 8):
        raise ValueError("alpha_bits must be 1..8")
    if not (-32768 <= width <= 32767 and -32768 <= height <= 32767):
        raise ValueError("width/height must fit int16")

    u3AlphaMSBCount = (alpha_bits - 1) & 0x7
    setting_byte = (u3AlphaMSBCount
                    | ((1 if raw else 0) << 3)
                    | ((tagset_bits & 0x3) << 4))

    return (b"LMSK\0"
            + struct.pack("<BhhBBI",
                          ARM_LMSK_VERSION,
                          width,
                          height,
                          setting_byte,
                          floor_count & 0xFF,
                          0))


# -----------------------------------------------------------------------------
# Delta helper (matches __arm_lmsk_get_delta)
# -----------------------------------------------------------------------------
def get_delta(prev: int, curr: int, alpha_bits: int) -> int:
    """
    Compute delta in MSB domain, choosing the smaller absolute value with wrap-around,
    matching reference C encoder __arm_lmsk_get_delta().
    """
    shift = 8 - alpha_bits
    prev_m = prev >> shift
    curr_m = curr >> shift
    delta0 = curr_m - prev_m
    mod = 1 << alpha_bits

    if delta0 < 0:
        delta1 = (curr_m + mod) - prev_m
        return delta0 if abs(delta0) < abs(delta1) else delta1
    elif delta0 > 0:
        delta1 = (prev_m + mod) - curr_m
        return delta0 if abs(delta0) < abs(delta1) else -delta1
    else:
        return 0


# -----------------------------------------------------------------------------
# Encoding result model
# -----------------------------------------------------------------------------
@dataclass
class EncodeResult:
    hit: bool
    raw_size: int
    enc: bytes
    new_prev: int
    check_palette: bool
    name: str


# -----------------------------------------------------------------------------
# Palette helper (mimics reference encoder strategy with sentinel 0 after slot0)
# -----------------------------------------------------------------------------
def palette_find_or_insert(palette: List[int], alpha: int) -> Optional[int]:
    """
    Reference encoder strategy:
    - search for exact alpha
    - stop early if palette[i]==0 and i>0 (sentinel = first free)
    - if not found and free exists -> insert and return index
    """
    idx = 0
    for idx in range(32):
        if palette[idx] == alpha:
            return idx
        if palette[idx] == 0 and idx > 0:
            break

    if idx < 32:
        palette[idx] = alpha
        return idx
    return None


# -----------------------------------------------------------------------------
# Try functions: ALPHA / DELTA_LARGE / DELTA_SMALL / REPEAT / GRADIENT
# -----------------------------------------------------------------------------
def try_alpha(source: Sequence[int], size_left: int, prev: int,
              alpha_bits: int, palette: List[int], tolerant: int, actual_prev: int) -> EncodeResult:
    val = source[0]
    return EncodeResult(True, 1, bytes([TAG_U8_ALPHA, val]), val, True, "ALPHA")


def try_delta_large(source: Sequence[int], size_left: int, prev: int,
                    alpha_bits: int, palette: List[int], tolerant: int, actual_prev: int) -> EncodeResult:
    d = get_delta(prev, source[0], alpha_bits)
    if -32 <= d <= 31:
        b = ((d & 0x3F) << 2) | 0x03  # tag bits 0b11
        shift = 8 - alpha_bits
        new_prev = (source[0] >> shift) << shift
        return EncodeResult(True, 1, bytes([b]), new_prev, False, "DELTA_LARGE")
    return EncodeResult(False, 0, b"", prev, False, "DELTA_LARGE")


def try_delta_small(source: Sequence[int], size_left: int, prev: int,
                    alpha_bits: int, palette: List[int], tolerant: int, actual_prev: int) -> EncodeResult:
    if size_left < 2:
        return EncodeResult(False, 0, b"", prev, False, "DELTA_SMALL")

    d0 = get_delta(prev, source[0], alpha_bits)
    if not (-4 <= d0 <= 3):
        return EncodeResult(False, 0, b"", prev, False, "DELTA_SMALL")

    d1 = get_delta(source[0], source[1], alpha_bits)
    if not (-4 <= d1 <= 3):
        return EncodeResult(False, 0, b"", prev, False, "DELTA_SMALL")

    # tag bits 0b10, d0 in bits[4:2], d1 in bits[7:5]
    b = 0x02 | ((d0 & 0x7) << 2) | ((d1 & 0x7) << 5)
    shift = 8 - alpha_bits
    new_prev = (source[1] >> shift) << shift
    return EncodeResult(True, 2, bytes([b]), new_prev, False, "DELTA_SMALL")


def try_repeat_prev(source: Sequence[int], size_left: int, prev: int,
                    alpha_bits: int, palette: List[int], tolerant: int, actual_prev: int) -> EncodeResult:
    shift = 8 - alpha_bits
    prev_m = prev >> shift

    count = 0
    for i in range(size_left):
        if (source[i] >> shift) != prev_m:
            break
        count += 1

    if count == 0:
        return EncodeResult(False, 0, b"", prev, False, "REPEAT")

    if count <= 62:
        # tag bits 0b01; u6Repeat = count-1
        b = 0x01 | ((count - 1) << 2)
        return EncodeResult(True, count, bytes([b]), prev, False, "REPEAT")

    # Reference encoder uses GRADIENT tag for extra-long repeat (>62)
    enc = struct.pack("<BBh", TAG_U8_GRADIENT, prev, count - 1)
    return EncodeResult(True, count, enc, prev, False, "REPEAT_GRADIENT")


def try_gradient(source: Sequence[int], size_left: int, prev: int,
                 alpha_bits: int, palette: List[int], tolerant: int, actual_prev: int) -> EncodeResult:
    """
    Ported from __arm_lmsk_try_gradient_tag (C).
    Note: This is the "gradient detection algorithm"; can be disabled by --no-gradient.
    """
    if size_left < 4:
        return EncodeResult(False, 0, b"", prev, False, "GRADIENT")

    # initial
    iGradientSize = 1
    iPrevious = prev

    iCurrent = source[0]
    idx = 1
    left = size_left - 1

    chToAlpha = iCurrent
    iDeltaPrevious = iCurrent - iPrevious

    iPrevious = iCurrent

    step_len_current = 2
    step_len_prev = 0
    first_step_len = True

    prev_step_to = 0
    prev_step_size = 0
    step_count = 0
    bNewStep = False

    while left > 0:
        iCurrent = source[idx]
        idx += 1
        left -= 1

        iDelta = iCurrent - iPrevious
        iDeltaChange = iDelta - iDeltaPrevious

        if abs(iDeltaChange) > 1:
            break

        if iDeltaChange == 0 or bNewStep:
            step_len_current += 1
            bNewStep = False
        else:
            if first_step_len:
                first_step_len = False
            else:
                if abs(step_len_current - step_len_prev) > tolerant:
                    break

            prev_step_to = iPrevious
            prev_step_size = iGradientSize
            step_count += 1

            step_len_prev = step_len_current
            step_len_current = 1
            bNewStep = True

        # wrong direction
        if iDelta < 0 < iDeltaPrevious:
            break
        if iDelta > 0 > iDeltaPrevious:
            break

        iDeltaPrevious = iDelta
        chToAlpha = iCurrent
        iPrevious = iCurrent
        iGradientSize += 1

    # resume to previous step if needed
    if step_count > 0:
        chToAlpha = prev_step_to
        iGradientSize = prev_step_size

    if iGradientSize > 4:
        # case 1: prev equals actual previous pixel -> gradient tag directly
        if prev == actual_prev:
            steps = iGradientSize - 1  # count = pixels-1
            enc = struct.pack("<BBh", TAG_U8_GRADIENT, chToAlpha, steps)
            return EncodeResult(True, iGradientSize, enc, chToAlpha, False, "GRADIENT")

        # case 2: prev mismatch but long enough -> insert INDEX/ALPHA + gradient
        if iGradientSize > 6:
            start_pixel = source[0]  # actual first pixel of gradient segment

            # Search/insert to palette (side effect allowed, matches C encoder)
            idx_pal = palette_find_or_insert(palette, start_pixel)

            raw_size = iGradientSize
            grad_pixels = iGradientSize - 1
            steps = grad_pixels - 1  # = iGradientSize - 2
            grad_enc = struct.pack("<BBh", TAG_U8_GRADIENT, chToAlpha, steps)

            if idx_pal is not None:
                index_byte = (idx_pal & 0x1F) << 2  # tag bits 00 + index
                enc = bytes([index_byte]) + grad_enc
                return EncodeResult(True, raw_size, enc, chToAlpha, False, "INDEX+GRADIENT")

            # If palette full, fall back to ALPHA_TAG + gradient
            enc = bytes([TAG_U8_ALPHA, start_pixel]) + grad_enc
            return EncodeResult(True, raw_size, enc, chToAlpha, False, "ALPHA+GRADIENT")

    return EncodeResult(False, 0, b"", prev, False, "GRADIENT")


# -----------------------------------------------------------------------------
# Encode one scanline (matches __arm_lmsk_encode_line)
# -----------------------------------------------------------------------------
def encode_line(row: bytes,
                alpha_bits: int,
                palette: List[int],
                no_gradient: bool,
                tolerant: int) -> bytes:
    width = len(row)
    out = bytearray()

    # first pixel raw
    out.append(row[0])
    prev = row[0]

    pos = 1
    left = width - 1

    # order aligned with C: DELTA_LARGE, REPEAT, DELTA_SMALL, GRADIENT, ALPHA
    algs = [try_delta_large, try_repeat_prev, try_delta_small, try_gradient, try_alpha]

    while left > 0:
        source = row[pos:]
        actual_prev = row[pos - 1]  # pchSourceBase[-1] equivalent

        best: Optional[EncodeResult] = None
        best_rate = -1.0

        for fn in algs:
            if fn is try_gradient and no_gradient:
                continue

            res = fn(source, left, prev, alpha_bits, palette, tolerant, actual_prev)
            if not res.hit:
                continue

            rate = res.raw_size / len(res.enc)
            if (best is None) or (rate > best_rate) or (rate == best_rate and res.raw_size > best.raw_size):
                best = res
                best_rate = rate

        if best is None:
            raise RuntimeError("No encoding decision hit (unexpected).")

        # palette substitution only for ALPHA + raw_size==1 (matches C)
        if best.check_palette and best.raw_size == 1:
            idx_pal = palette_find_or_insert(palette, best.new_prev)
            if idx_pal is not None:
                index_byte = (idx_pal & 0x1F) << 2
                best = EncodeResult(True, 1, bytes([index_byte]), best.new_prev, False, "INDEX")

        out.extend(best.enc)
        pos += best.raw_size
        left -= best.raw_size
        prev = best.new_prev

    return bytes(out)


# -----------------------------------------------------------------------------
# Whole-image encoding with row dedup + floor handling (matches lmsk_encoder.c)
# -----------------------------------------------------------------------------
@dataclass
class LineOut:
    source: bytes
    encoded: bytes
    position: int
    crc32: int


def encode_lmsk(mask: bytes,
                width: int,
                height: int,
                raw: bool = False,
                no_gradient: bool = False,
                tolerant: int = 1,
                alpha_bits: int = 8) -> bytes:
    if raw:
        # raw mode: header + raw pixels + end mark (no palette/tables)
        header = pack_header(width, height, alpha_bits=1, raw=True, floor_count=0)
        return header + mask + struct.pack("<I", END_MARK)

    palette = [0] * 32
    crc_map: Dict[int, List[LineOut]] = {}

    references = [0] * height
    data_section = bytearray()
    position = 0

    for y in range(height):
        row = mask[y * width:(y + 1) * width]
        crc = zlib.crc32(row) & 0xFFFFFFFF

        found: Optional[LineOut] = None
        candidates = crc_map.get(crc)
        if candidates:
            # search from tail (matches C behavior)
            for line in reversed(candidates):
                if line.source == row:
                    found = line
                    break

        if found is not None:
            current_floor = position >> 16
            target_floor = found.position >> 16

            if current_floor != target_floor:
                # clone into current floor (matches C)
                new_line = LineOut(found.source, found.encoded, position, crc)
                crc_map.setdefault(crc, []).append(new_line)
                references[y] = position
                data_section.extend(found.encoded)
                position += len(found.encoded)
            else:
                references[y] = found.position
        else:
            encoded = encode_line(row, alpha_bits, palette, no_gradient, tolerant)
            new_line = LineOut(row, encoded, position, crc)
            crc_map.setdefault(crc, []).append(new_line)
            references[y] = position
            data_section.extend(encoded)
            position += len(encoded)

    # build floor table + line index table
    floor_level = 0
    floor_entries: List[int] = []
    index_table: List[int] = [0] * height

    for y in range(height):
        ref = references[y]
        if ref - floor_level < 0:
            raise RuntimeError("Reference before current floor level (should not happen).")

        if ref - floor_level >= FLOOR_SIZE:
            floor_level += FLOOR_SIZE
            floor_entries.append(y)

        index_table[y] = ref - floor_level
        if not (0 <= index_table[y] <= 0xFFFF):
            raise RuntimeError("Line index overflow (must fit uint16).")

    floor_count = len(floor_entries)
    if floor_count > 255:
        raise RuntimeError("Too large: floor_count > 255 (data section >= 16MB).")

    header = pack_header(width, height, alpha_bits, raw=False, floor_count=floor_count)

    palette_bytes = bytes(palette)
    floor_bytes = b"".join(struct.pack("<H", v) for v in floor_entries) if floor_count else b""
    index_bytes = b"".join(struct.pack("<H", v) for v in index_table)
    end_mark = struct.pack("<I", END_MARK)

    return header + palette_bytes + floor_bytes + index_bytes + bytes(data_section) + end_mark


# -----------------------------------------------------------------------------
# Image loading: alpha extraction or grayscale conversion
# -----------------------------------------------------------------------------
def load_mask_from_image(path: Path) -> Tuple[bytes, int, int, bool]:
    img = Image.open(path)

    # Detect alpha presence robustly
    has_alpha = ("A" in img.mode) or (img.mode == "P" and "transparency" in img.info)

    if has_alpha:
        img = img.convert("RGBA")
        alpha = img.getchannel("A")
        mask = alpha.tobytes()
    else:
        img = img.convert("L")
        mask = img.tobytes()

    return mask, img.width, img.height, has_alpha


# -----------------------------------------------------------------------------
# CLI
# -----------------------------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(
        description="Encode image alpha (or grayscale) into LMSK (*.lmsk)."
    )
    parser.add_argument("image", help="Input image path (PNG/BMP/JPG/...).")
    parser.add_argument("-o", "--output", default=None,
                        help="Output .lmsk full path. If omitted, use input stem + .lmsk in same dir.")
    parser.add_argument("--raw", action="store_true",
                        help="Enable RAW mode (no compression, no palette/tables). Default: off.")
    parser.add_argument("--no-gradient", action="store_true",
                        help="Disable gradient detection algorithm. (Repeat>62 still uses GRADIENT tag like C encoder.)")
    parser.add_argument("--gradient-tolerant", type=int, default=1, choices=[0, 1, 2, 3],
                        help="Gradient tolerant (0~3). Default: 1.")
    parser.add_argument("--alpha-bits", type=int, default=8, choices=[1, 2, 3, 4, 5, 6, 7, 8],
                        help="Alpha Bits (1~8). Default: 8.")
    args = parser.parse_args()

    in_path = Path(args.image)
    if not in_path.exists():
        raise SystemExit(f"Input file not found: {in_path}")

    out_path = Path(args.output) if args.output else in_path.with_suffix(".lmsk")
    if out_path.suffix.lower() != ".lmsk":
        out_path = out_path.with_suffix(".lmsk")

    mask, w, h, has_alpha = load_mask_from_image(in_path)

    # For your current requirements, keep alpha_bits=8 (true lossless for 8-bit alpha/grayscale)
    alpha_bits = args.alpha_bits

    blob = encode_lmsk(mask,
                       width=w,
                       height=h,
                       raw=args.raw,
                       no_gradient=args.no_gradient,
                       tolerant=args.gradient_tolerant,
                       alpha_bits=alpha_bits)

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_bytes(blob)

    kind = "Alpha" if has_alpha else "Grayscale"
    mode = "RAW" if args.raw else "Compressed"
    print(f"[OK] {kind} -> LMSK ({mode})")
    print(f"     Input : {in_path}")
    print(f"     Output: {out_path}")
    print(f"     Size  : {len(blob)} bytes  ({w}x{h})")


if __name__ == "__main__":
    main()
