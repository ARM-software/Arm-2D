"""
RLE (Run-Length Encoding) Encoder
Pure RLE encoding implementation - Strictly follows C implementation data types
"""

import numpy as np
from typing import Tuple, Optional

# Encoding threshold
RLE_THRESHOLD = 3  # Minimum consecutive pixels for RLE encoding


def rgb565_get_r(color: int) -> int:
    """Extract RGB565 red component (5 bits)"""
    return (color >> 11) & 0x1F


def rgb565_get_g(color: int) -> int:
    """Extract RGB565 green component (6 bits)"""
    return (color >> 5) & 0x3F


def rgb565_get_b(color: int) -> int:
    """Extract RGB565 blue component (5 bits)"""
    return color & 0x1F


def find_encode_flag(img: np.ndarray, pixel_count: np.uint64) -> Tuple[np.uint16, np.uint16, np.uint16, bool]:
    """
    Find optimal encoding flag - strictly matches C implementation data types
    
    Priority:
    1. Unused high byte values (XX00 format)
    2. High bytes where all pixels have at least 3 consecutive identical values
    3. Least used high bytes
    
    Args:
        img: RGB565 image data array
        pixel_count: Number of pixels (uint64_t in C)
        
    Returns:
        (encode_flag, encode_flag_cs, encode_flag_mode, is_perfect)
        encode_flag: Flag value (high byte << 8)
        encode_flag_cs: Related info (minimum consecutive count or occurrence count)
        encode_flag_mode: Mode (0=unused, 1=RLE perfect, 2=least used)
        is_perfect: Whether perfect flag was found
    """
    if pixel_count == 0:
        return np.uint16(0xFF00), np.uint16(0), np.uint16(0), True
    
    # ========== Step 1: Count basic information ==========
    # Match C implementation: uint8_t used_map[32] = {0};
    used_map = np.zeros(32, dtype=np.uint8)
    # Match C implementation: uint16_t count_map[256] = {0};
    # Use uint32_t to prevent overflow with large images
    count_map = np.zeros(256, dtype=np.uint32)
    
    for i in range(int(pixel_count)):
        pixel = int(img[i])
        hi_byte = (pixel >> 8) & 0xFF
        used_map[hi_byte // 8] |= np.uint8(1 << (hi_byte % 8))
        count_map[hi_byte] += np.uint32(1)  # Use uint32_t to prevent overflow
    
    # ========== Step 2: Priority 1 - Unused high bytes ==========
    for hi_byte in range(256):
        if (int(used_map[hi_byte // 8]) & (1 << (hi_byte % 8))) == 0:
            return np.uint16(hi_byte << 8), np.uint16(0), np.uint16(0), True
    
    # ========== Step 3: Priority 2 - Perfect RLE high bytes ==========
    for hi_byte in range(256):
        if count_map[hi_byte] == 0:
            continue
        
        is_perfect = True
        min_continuous = np.uint16(0xFFFF)
        
        i = np.uint64(0)
        while i < pixel_count:
            # Skip pixels that don't have this high byte
            while i < pixel_count and ((int(img[int(i)]) >> 8) & 0xFF) != hi_byte:
                i += np.uint64(1)
            
            if i >= pixel_count:
                break
            
            # Check continuous segments
            current_value = np.uint16(img[int(i)])
            continuous_count = np.uint16(1)
            j = i + np.uint64(1)
            
            while j < pixel_count and np.uint16(img[int(j)]) == current_value:
                continuous_count += np.uint16(1)
                j += np.uint64(1)
            
            # Check if condition is met (at least 3 consecutive identical)
            if continuous_count < RLE_THRESHOLD:
                is_perfect = False
                break
            
            if continuous_count < min_continuous:
                min_continuous = continuous_count
            
            i = j
        
        if is_perfect and min_continuous != np.uint16(0xFFFF):
            # Find a specific pixel value as flag
            concrete_value = np.uint16(hi_byte << 8)
            for k in range(int(pixel_count)):
                if ((int(img[k]) >> 8) & 0xFF) == hi_byte:
                    concrete_value = np.uint16(img[k])
                    break
            
            return np.uint16(concrete_value & 0xFF00), min_continuous, np.uint16(1), True
    
    # ========== Step 4: Priority 3 - Least used high bytes ==========
    min_hi_byte = np.uint16(0)
    min_count = np.uint16(0xFFFF)
    
    for hi_byte in range(256):
        if count_map[hi_byte] > 0:
            if count_map[hi_byte] < min_count:
                min_count = count_map[hi_byte]
                min_hi_byte = np.uint16(hi_byte)
            elif count_map[hi_byte] == min_count:
                if hi_byte < min_hi_byte:
                    min_hi_byte = np.uint16(hi_byte)
    
    if min_count == np.uint16(0xFFFF):
        return np.uint16(0xFF00), np.uint16(0), np.uint16(2), False
    
    # Important: In least used mode, return XX00 format
    return np.uint16(min_hi_byte << 8), min_count, np.uint16(2), False


def check_rle_length(pixels: np.ndarray, start: np.uint32, end: np.uint32) -> np.uint32:
    """
    Check RLE length starting from position - strictly matches C implementation data types
    
    Args:
        pixels: Pixel array
        start: Start position (uint32_t in C)
        end: End position (exclusive, uint32_t in C)
        
    Returns:
        Number of consecutive identical pixels (uint32_t)
    """
    if start >= end:
        return np.uint32(0)
    
    first_color = np.uint16(pixels[int(start)])
    length = np.uint32(1)
    
    for i in range(int(start) + 1, int(end)):
        if np.uint16(pixels[i]) == first_color:
            length += np.uint32(1)
            if length == np.uint32(0xFFFF):
                break
        else:
            break
    
    return length


def encode_rgb565_rle_only(input_data: np.ndarray, width: np.uint16, height: np.uint16) -> Tuple[Optional[np.ndarray], np.uint32, float]:
    """
    Pure RLE encoding function - strictly matches C implementation data types
    
    Args:
        input_data: Input RGB565 data, length is width*height
        width: Image width (uint16_t in C)
        height: Image height (uint16_t in C)
        
    Returns:
        (output_data, output_size, compression_ratio)
        output_data: Encoded data array
        output_size: Encoded data size (uint32_t in C)
        compression_ratio: Compression ratio (percentage, smaller is better)
    """
    pixel_count = np.uint64(width) * np.uint64(height)
    
    if width == 0 or height == 0 or pixel_count == 0:
        return None, np.uint32(0), 0.0
    
    # Find encoding flag - pass pixel_count as uint64_t
    encode_flag, encode_flag_cs, encode_flag_mode, flag_ok = find_encode_flag(input_data, pixel_count)
    
    # Estimate maximum output size - match C calculation exactly (uint64_t)
    max_output_size = np.uint64(10) + np.uint64(height) + (pixel_count * np.uint64(2))
    output = np.zeros(int(max_output_size), dtype=np.uint32)  # Use 32-bit to avoid overflow during construction
    
    # Allocate row offset array (uint32_t like C)
    row_offsets = np.zeros(height + 1, dtype=np.uint32)
    
    # Encoding data buffer (uint32_t for index calculations)
    encoded_data = np.zeros(int(max_output_size), dtype=np.uint32)
    encoded_index = np.uint32(0)
    
    # Traverse row by row (uint16_t y in C)
    for y in range(int(height)):
        row_offsets[y] = encoded_index
        row_start = y * int(width)
        row = input_data[row_start:row_start + int(width)]
        
        col = np.uint32(0)
        while col < width:
            # Check RLE length (uint32_t in C)
            rle_len = check_rle_length(row, col, width)
            
            if rle_len >= RLE_THRESHOLD:
                color = np.uint16(row[int(col)])
                
                if rle_len >= np.uint32(128):
                    # Long encoding: flag, color, count (match C exactly)
                    encoded_data[int(encoded_index)] = encode_flag
                    encoded_index += np.uint32(1)
                    encoded_data[int(encoded_index)] = color
                    encoded_index += np.uint32(1)
                    encoded_data[int(encoded_index)] = rle_len
                    encoded_index += np.uint32(1)
                else:
                    # Short encoding: flag + count, color (match C exactly)
                    encoded_data[int(encoded_index)] = np.uint16(encode_flag + rle_len)
                    encoded_index += np.uint32(1)
                    encoded_data[int(encoded_index)] = color
                    encoded_index += np.uint32(1)
                
                col += rle_len
            else:
                # Handle pixels that conflict with flag
                color_tmp = np.uint16(row[int(col)])
                
                if (color_tmp & np.uint16(0xFF00)) == encode_flag:
                    # Pixel conflicts with flag code, use RLE short encoding to store single pixel
                    encoded_data[int(encoded_index)] = np.uint16(encode_flag + 1)
                    encoded_index += np.uint32(1)
                    encoded_data[int(encoded_index)] = color_tmp
                    encoded_index += np.uint32(1)
                else:
                    # Store original pixel directly
                    encoded_data[int(encoded_index)] = color_tmp
                    encoded_index += np.uint32(1)
                
                col += np.uint32(1)
    
    row_offsets[height] = encoded_index
    
    # Calculate upgrade table - match C logic exactly (uint16_t upgrade[500])
    upgrade = np.zeros(500, dtype=np.uint16)
    upgrade_len = np.uint16(0)
    for i in range(int(height) - 1):
        tmp0 = row_offsets[i]
        tmp1 = row_offsets[i + 1]
        if tmp0 > tmp1:
            upgrade[int(upgrade_len)] = np.uint16(i + 1)
            upgrade_len += np.uint16(1)
    
    # Calculate row table start coordinate and encoding data start coordinate - match C exactly
    row_offset_addr = np.uint16(6) + upgrade_len
    encode_data_addr = row_offset_addr + np.uint16(height) + np.uint16(1)
    
    # Fill header - match C structure exactly
    output[0] = width
    output[1] = height
    output[2] = encode_flag
    output[3] = upgrade_len
    output[4] = row_offset_addr
    output[5] = encode_data_addr
    
    idx = np.uint32(6)
    
    # Write upgrade table (uint16_t values)
    if upgrade_len > 0:
        for i in range(int(upgrade_len)):
            output[int(idx)] = upgrade[i]
            idx += np.uint32(1)
    
    # Write row offset table (uint32_t values)
    for i in range(int(height) + 1):
        output[int(idx)] = row_offsets[i]
        idx += np.uint32(1)
    
    # Write encoding data
    for i in range(int(encoded_index)):
        output[int(idx)] = encoded_data[i]
        idx += np.uint32(1)
    
    # Calculate compression ratio - match C calculation exactly
    original_size = float(pixel_count * np.uint64(2))  # Original data size (bytes)
    compressed_size = float(idx * np.uint32(2))       # Compressed size (bytes)
    compression_ratio = (compressed_size / original_size) * 100.0
    
    # Convert to uint16 array for return (like C output)
    result = output[:int(idx)].astype(np.uint16)
    
    return result, idx, compression_ratio


def generate_c_array(output_data: np.ndarray, output_size: np.uint32, 
                     width: np.uint16, height: np.uint16, compression_ratio: float,
                     src_path: str = "", array_name: str = "img") -> str:
    """
    Generate C language array format string - matches C implementation
    
    Args:
        output_data: Encoded data
        output_size: Data size (uint32_t in C)
        width: Image width (uint16_t in C)
        height: Image height (uint16_t in C)
        compression_ratio: Compression ratio
        src_path: Source file path
        array_name: Array name
        
    Returns:
        C language code string
    """
    import os
    
    # Get base filename
    base_name = os.path.splitext(os.path.basename(src_path))[0] if src_path else "image"
    
    lines = []
    lines.append("// Compressed RGB565 data")
    lines.append(f"// Source file: {src_path}")
    lines.append(f"// Original size: {width} x {height} = {width * height} pixels")
    lines.append(f"// Compression ratio: {compression_ratio:.2f}%")
    lines.append("")
    lines.append(f"const uint16_t _{base_name}_zhRGB565_Data[{output_size}] = {{")
    lines.append("")
    
    # Write header information
    lines.append("    /* width, height, encode_flag, level_up_table_len, row_offset_addr, data_addr */")
    lines.append(f"    {output_data[0]}, {output_data[1]}, 0x{output_data[2]:04X}, {output_data[3]}, {output_data[4]}, {output_data[5]},")
    lines.append("")
    
    idx = np.uint32(6)
    upgrade_len = np.uint16(output_data[3])
    row_offset_addr = np.uint16(output_data[4])
    
    # Write upgrade table
    if upgrade_len > 0:
        lines.append("    /* level_up table */")
        line = "    "
        count = 0
        for i in range(int(upgrade_len)):
            line += f"{output_data[int(idx)]}, "
            idx += np.uint32(1)
            count += 1
            if count % 16 == 0:
                lines.append(line.rstrip())
                line = "    "
        if line.strip():
            lines.append(line.rstrip())
        lines.append("")
    else:
        lines.append("    /* NO level_up table */")
        lines.append("")
    
    # Write row offset table
    lines.append(f"    /* Row offset table ({height} rows total) */")
    line = "    "
    for i in range(int(height) + 1):
        line += f"{output_data[int(idx)]}"
        idx += np.uint32(1)
        line += ", "  # Add comma after all elements (including last)
        if (i + 1) % 16 == 0:
            lines.append(line.rstrip())
            line = "    "
    if line.strip():
        lines.append(line.rstrip())
    lines.append("")
    
    # Write encoding data
    lines.append("    /* Encoded data */")
    
    # Format output by lines - match C logic exactly
    hhcnt = np.uint32(0)
    line_base = np.uint32(0)
    next_line = np.uint32(1)
    current_line = np.uint32(0)
    
    lines.append(f"    /* 0 */")
    line = "    "
    
    while idx < output_size:
        if next_line == height:
            # Last line, output all
            while idx < output_size:
                line += f"0x{int(output_data[int(idx)]):04X}, "
                idx += np.uint32(1)
                hhcnt += np.uint32(1)
                if hhcnt % 16 == 0:
                    lines.append(line.rstrip())
                    line = "    "
        else:
            current_line_pos = np.uint32(output_data[int(row_offset_addr) + int(current_line)]) + line_base
            next_line_pos = np.uint32(output_data[int(row_offset_addr) + int(next_line)]) + line_base
            
            if current_line_pos > next_line_pos:
                line_base += np.uint32(65536)
                next_line_pos += np.uint32(65536)
            
            for j in range(int(current_line_pos), int(next_line_pos)):
                if idx >= output_size:
                    break
                line += f"0x{int(output_data[int(idx)]):04X}, "
                idx += np.uint32(1)
                hhcnt += np.uint32(1)
                if hhcnt % 16 == 0 and j != int(next_line_pos) - 1:
                    lines.append(line.rstrip())
                    line = "    "
            
            current_line = next_line
            next_line += np.uint32(1)
            hhcnt = np.uint32(0)
        
        if idx >= output_size:
            break
        
        if line.strip():
            lines.append(line.rstrip())
        lines.append(f"    /* {current_line} */")
        line = "    "
    
    if line.strip():
        lines.append(line.rstrip())
    
    lines.append("};")
    lines.append("")
    
    return "\n".join(lines)


if __name__ == "__main__":
    # Test code
    # Create a simple test image
    test_data = np.array([
        0xF800, 0xF800, 0xF800, 0x07E0, 0x07E0, 0x001F, 0x001F, 0x001F,
        0xF800, 0xF800, 0xF800, 0x07E0, 0x07E0, 0x001F, 0x001F, 0x001F,
    ], dtype=np.uint16)
    
    width = np.uint16(8)
    height = np.uint16(2)
    
    result, size, ratio = encode_rgb565_rle_only(test_data, width, height)
    if result is not None:
        print(f"Encoding successful!")
        print(f"Output size: {size} uint16")
        print(f"Compression ratio: {ratio:.2f}%")
        print("\nGenerated C array:")
        c_code = generate_c_array(result, size, width, height, ratio, "test.bmp")
        print(c_code)
