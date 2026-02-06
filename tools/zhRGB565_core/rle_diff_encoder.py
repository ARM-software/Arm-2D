"""
RLE + Differential Encoding Mixed Encoder
Combines RLE and differential encoding for better compression of gradient areas - Strictly follows C implementation data types
"""

import numpy as np
from typing import Tuple, Optional
from zhRGB565_core.rle_encoder import find_encode_flag, check_rle_length, RLE_THRESHOLD, generate_c_array

# Differential encoding threshold
DIFF_THRESHOLD = 7  # Minimum pixels for differential encoding


def rgb565_get_r(color: np.uint16) -> np.uint8:
    """Extract RGB565 red component (5 bits) - matches C uint8_t"""
    return np.uint8((color >> 11) & 0x1F)


def rgb565_get_g(color: np.uint16) -> np.uint8:
    """Extract RGB565 green component (6 bits) - matches C uint8_t"""
    return np.uint8((color >> 5) & 0x3F)


def rgb565_get_b(color: np.uint16) -> np.uint8:
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
    r = rgb565_get_r(diff)
    g = rgb565_get_g(diff)
    b = rgb565_get_b(diff)
    
    return (r <= np.uint8(7)) and (g <= np.uint8(7)) and (b <= np.uint8(3))


def compress_diff_to_byte(diff: np.uint16) -> np.uint8:
    """Compress difference into one byte (RGB332 format) - matches C uint8_t"""
    r = rgb565_get_r(diff)
    g = rgb565_get_g(diff)
    b = rgb565_get_b(diff)
    
    return rgb332_val(r, g, b)


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


if __name__ == "__main__":
    # Test code
    # Create a test image with gradients
    test_data = np.array([
        0xF800, 0xF801, 0xF802, 0xF803, 0xF804, 0xF805, 0xF806, 0xF807,
        0x07E0, 0x07E0, 0x07E0, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
    ], dtype=np.uint16)
    
    width = np.uint16(8)
    height = np.uint16(2)
    
    result, size, ratio = encode_rgb565_rle_diff(test_data, width, height)
    if result is not None:
        print(f"Encoding successful!")
        print(f"Output size: {size} uint16")
        print(f"Compression ratio: {ratio:.2f}%")
        print("\nGenerated C array:")
        c_code = generate_c_array(result, size, width, height, ratio, "test.bmp")
        print(c_code)
