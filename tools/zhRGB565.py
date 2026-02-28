"""zhRGB565 Encoder Library
Combines RLE and RLE+Differential encoding implementations
Strictly follows C implementation data types and behavior
"""

import numpy as np
from typing import Tuple, Optional

# Encoding thresholds
RLE_THRESHOLD = 3  # Minimum consecutive pixels for RLE encoding
DIFF_THRESHOLD = 7  # Minimum pixels for differential encoding


def rgb565_get_r_u8(color: np.uint16) -> np.uint8:
    """Extract RGB565 red component (5 bits) - matches C uint8_t"""
    return np.uint8((color >> 11) & 0x1F)


def rgb565_get_g_u8(color: np.uint16) -> np.uint8:
    """Extract RGB565 green component (6 bits) - matches C uint8_t"""
    return np.uint8((color >> 5) & 0x3F)


def rgb565_get_b_u8(color: np.uint16) -> np.uint8:
    """Extract RGB565 blue component (5 bits) - matches C uint8_t"""
    return np.uint8(color & 0x1F)


def rgb332_val(r: np.uint8, g: np.uint8, b: np.uint8) -> np.uint8:
    """Pack RGB components into RGB332 format - matches C uint8_t"""
    return np.uint8(((r & 0x07) << 5) | ((g & 0x07) << 2) | (b & 0x03))


def pack_u8_to_u16(high: np.uint8, low: np.uint8) -> np.uint16:
    """Pack two uint8 values into one uint16 - matches C uint16_t"""
    return np.uint16((np.uint16(high) << 8) | np.uint16(low))


def can_compress_diff(diff: np.uint16) -> bool:
    """
    Check if difference can be compressed into one byte - matches C implementation exactly
    
    Condition: R component <=7, G component <=7, B component <=3
    """
    r = rgb565_get_r_u8(diff)
    g = rgb565_get_g_u8(diff)
    b = rgb565_get_b_u8(diff)
    
    return (r <= np.uint8(7)) and (g <= np.uint8(7)) and (b <= np.uint8(3))


def compress_diff_to_byte(diff: np.uint16) -> np.uint8:
    """Compress difference into one byte (RGB332 format) - matches C uint8_t"""
    r = rgb565_get_r_u8(diff)
    g = rgb565_get_g_u8(diff)
    b = rgb565_get_b_u8(diff)
    
    return rgb332_val(r, g, b)


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


def calculate_diff_length(data: np.ndarray, start: np.uint32, end: np.uint32, output: np.ndarray) -> np.uint16:
    """
    Calculate the number of pixels that meet differential encoding criteria - strictly matches C implementation
    
    Args:
        data: Pixel data array
        start: Start position (uint32_t in C)
        end: End position (exclusive, uint32_t in C)
        output: Output difference data buffer
        
    Returns:
        Compressible difference pixel count (uint16_t in C)
    """
    if end - start < DIFF_THRESHOLD:
        return np.uint16(0)
    
    idx = np.uint16(0)  # CRITICAL: Must be uint16_t like C
    idx_tmp = np.uint16(0)
    tmp = np.zeros(2, dtype=np.uint8)
    
    # CRITICAL: C uses uint16_t for loop variable, not uint32_t
    i = np.uint16(start)
    diff_count = np.uint16(1)
    rle_cnt = np.uint16(1)
    
    while i < end - np.uint16(1):
        current_pixel = np.uint16(data[int(i)])
        next_pixel = np.uint16(data[int(i) + 1])
        diff = np.uint16(current_pixel ^ next_pixel)
        
        # Meet differential encoding conditions
        if can_compress_diff(diff):
            diff_count += np.uint16(1)
            
            tmp[int(idx_tmp)] = compress_diff_to_byte(diff)
            idx_tmp += np.uint16(1)
            
            if idx_tmp == np.uint16(2):
                if int(idx) >= len(output):
                    # Prevent buffer overflow - match C behavior
                    break
                output[int(idx)] = pack_u8_to_u16(tmp[0], tmp[1])
                idx += np.uint16(1)
                idx_tmp = np.uint16(0)
                if idx == np.uint16(31):  # Actual encoded source data 31*2 + 1
                    break
            
            if diff == np.uint16(0):
                # diff=0 means 2 identical pixels
                rle_cnt += np.uint16(1)
                # Handle uint16_t overflow - if rle_cnt reaches max, treat as exceeding threshold
                if rle_cnt == np.uint16(0):  # Overflow occurred
                    diff_count -= np.uint16(RLE_THRESHOLD)
                    break
                if rle_cnt > np.uint16(RLE_THRESHOLD):  # CRITICAL: Revert back to >
                    # Consecutive pixels exceed 3+1, exit for RLE processing
                    diff_count -= np.uint16(RLE_THRESHOLD)
                    break
            else:
                rle_cnt = np.uint16(0)
            
            # CRITICAL: C uses i++ which keeps it as uint16_t
            i += np.uint16(1)
        else:
            break
    
    # CRITICAL: C doesn't have this logic - only adjust for even count
    # Handle remaining differences - CRITICAL: Try removing this adjustment
    # if idx_tmp == np.uint16(1):
    #     # Odd number of differences, reduce by one
    #     diff_count -= np.uint16(1)
    
    # Number of pixels meeting differential encoding must be odd and not zero
    if diff_count % np.uint16(2) == np.uint16(0) and diff_count != np.uint16(0):
        diff_count -= np.uint16(1)
    
    if diff_count >= np.uint16(DIFF_THRESHOLD):
        return diff_count
    else:
        return np.uint16(0)


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


def encode_rgb565_rle_diff(input_data: np.ndarray, width: np.uint16, height: np.uint16) -> Tuple[Optional[np.ndarray], np.uint32, float]:
    """
    RLE+Differential mixed encoding function - strictly matches C implementation data types
    
    Args:
        input_data: Input RGB565 data, length is width*height
        width: Image width (uint16_t in C)
        height: Image height (uint16_t in C)
        
    Returns:
        (output_data, output_size, compression_ratio)
    """
    pixel_count = np.uint64(width) * np.uint64(height)
    
    if width == np.uint16(0) or height == np.uint16(0) or pixel_count == np.uint64(0):
        return None, np.uint32(0), 0.0
    
    # Find encoding flag - match C call exactly (uint64_t pixel_count)
    encode_flag, encode_flag_cs, encode_flag_mode, flag_ok = find_encode_flag(input_data, pixel_count)
    
    # Estimate maximum output size - match C calculation exactly (uint64_t)
    max_output_size = np.uint64(6) + np.uint64(height) + (pixel_count * np.uint64(2))
    output = np.zeros(int(max_output_size), dtype=np.uint32)  # Use 32-bit to avoid overflow
    
    # Allocate row offset array - use uint32_t like C
    row_offsets = np.zeros(int(height) + 1, dtype=np.uint32)
    
    # Differential encoding data buffer - match C: encoded_diff_data = (uint16_t*)malloc((size_t)65536 * 2 * sizeof(uint16_t))
    encoded_diff_data = np.zeros(65536 * 2, dtype=np.uint16)
    
    # Encoding data buffer - use uint32_t for index calculations (encoded_index = uint32_t)
    encoded_data = np.zeros(int(max_output_size), dtype=np.uint32)
    encoded_index = np.uint32(0)
    
    # Traverse row by row - match C logic exactly (uint16_t y)
    for y in range(int(height)):
        row_offsets[y] = encoded_index
        row_start = y * int(width)
        row = input_data[row_start:row_start + int(width)]
        
        col = np.uint32(0)
        while col < width:
            # Try differential encoding first (like C) - uint32_t parameters
            diff_len = calculate_diff_length(row, col, width, encoded_diff_data)
            
            if diff_len >= np.uint16(DIFF_THRESHOLD):
                base_color = np.uint16(row[int(col)])
                
                if diff_len >= np.uint16(128):
                    # Long encoding - match C exactly
                    encoded_data[int(encoded_index)] = encode_flag
                    encoded_index += np.uint32(1)
                    encoded_data[int(encoded_index)] = base_color
                    encoded_index += np.uint32(1)
                    encoded_data[int(encoded_index)] = np.uint16(0x8000 + (diff_len // np.uint16(2)))
                    encoded_index += np.uint32(1)
                    
                    for i in range(int(diff_len) // 2):
                        encoded_data[int(encoded_index)] = encoded_diff_data[i]
                        encoded_index += np.uint32(1)
                else:
                    # Short encoding - match C exactly
                    encoded_data[int(encoded_index)] = np.uint16(encode_flag + np.uint16(0x80) + (diff_len // np.uint16(2)))
                    encoded_index += np.uint32(1)
                    encoded_data[int(encoded_index)] = base_color
                    encoded_index += np.uint32(1)
                    
                    for i in range(int(diff_len) // 2):
                        encoded_data[int(encoded_index)] = encoded_diff_data[i]
                        encoded_index += np.uint32(1)
                
                col += diff_len
            else:
                # Try RLE encoding (like C) - uint32_t parameters
                rle_len = check_rle_length(row, col, width)
                
                if rle_len >= np.uint32(RLE_THRESHOLD):
                    color = np.uint16(row[int(col)])
                    
                    if rle_len >= np.uint32(128):
                        # Long encoding - match C exactly
                        encoded_data[int(encoded_index)] = encode_flag
                        encoded_index += np.uint32(1)
                        encoded_data[int(encoded_index)] = color
                        encoded_index += np.uint32(1)
                        encoded_data[int(encoded_index)] = rle_len
                        encoded_index += np.uint32(1)
                    else:
                        # Short encoding - match C exactly
                        encoded_data[int(encoded_index)] = np.uint16(encode_flag + rle_len)
                        encoded_index += np.uint32(1)
                        encoded_data[int(encoded_index)] = color
                        encoded_index += np.uint32(1)
                    
                    col += rle_len
                else:
                    # Store original pixel directly - match C logic exactly
                    color_tmp = np.uint16(row[int(col)])
                    
                    if (color_tmp & np.uint16(0xFF00)) == encode_flag:
                        # Pixel conflicts with flag code - match C exactly
                        encoded_data[int(encoded_index)] = np.uint16(encode_flag + np.uint16(1))
                        encoded_index += np.uint32(1)
                        encoded_data[int(encoded_index)] = row[int(col)]  # CRITICAL: C uses row[col], not color_tmp
                        encoded_index += np.uint32(1)
                    else:
                        # Store original pixel directly
                        encoded_data[int(encoded_index)] = color_tmp
                        encoded_index += np.uint32(1)
                    
                    col += np.uint32(1)
    
    row_offsets[int(height)] = encoded_index
    
    # Calculate upgrade table - match C logic exactly (uint16_t upgrade[500])
    upgrade = np.zeros(500, dtype=np.uint16)
    upgrade_len = np.uint16(0)
    for i in range(int(height) - 1):
        # CRITICAL: C casts to uint16_t for comparison
        tmp0 = np.uint16(row_offsets[i])
        tmp1 = np.uint16(row_offsets[i + 1])
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
    
    # Write row offset table (uint16_t values - CRITICAL: C casts to uint16_t)
    for i in range(int(height) + 1):
        output[int(idx)] = np.uint16(row_offsets[i])
        idx += np.uint32(1)
    
    # Write encoding data
    for i in range(int(encoded_index)):
        output[int(idx)] = encoded_data[i]
        idx += np.uint32(1)
    
    # Calculate compression ratio - match C calculation exactly
    original_size = float(pixel_count * np.uint64(2))
    compressed_size = float(idx * np.uint32(2))
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
    
    print("Testing RLE only encoding:")
    result, size, ratio = encode_rgb565_rle_only(test_data, width, height)
    if result is not None:
        print(f"Encoding successful!")
        print(f"Output size: {size} uint16")
        print(f"Compression ratio: {ratio:.2f}%")
        print("\nGenerated C array:")
        c_code = generate_c_array(result, size, width, height, ratio, "test.bmp")
        print(c_code)
    
    print("\n" + "="*50 + "\n")
    
    # Test RLE+Diff encoding
    print("Testing RLE+Diff encoding:")
    result, size, ratio = encode_rgb565_rle_diff(test_data, width, height)
    if result is not None:
        print(f"Encoding successful!")
        print(f"Output size: {size} uint16")
        print(f"Compression ratio: {ratio:.2f}%")
        print("\nGenerated C array:")
        c_code = generate_c_array(result, size, width, height, ratio, "test.bmp")
        print(c_code)
