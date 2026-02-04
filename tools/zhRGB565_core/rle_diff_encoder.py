"""
RLE + Differential Encoding Mixed Encoder
Combines RLE and differential encoding for better compression of gradient areas
"""

import numpy as np
from typing import Tuple, Optional
from zhRGB565_core.rle_encoder import find_encode_flag, check_rle_length, RLE_THRESHOLD, generate_c_array

# Differential encoding threshold
DIFF_THRESHOLD = 7  # Minimum pixels for differential encoding


def rgb565_get_r(color: int) -> int:
    """Extract RGB565 red component (5 bits)"""
    return (color >> 11) & 0x1F


def rgb565_get_g(color: int) -> int:
    """Extract RGB565 green component (6 bits)"""
    return (color >> 5) & 0x3F


def rgb565_get_b(color: int) -> int:
    """Extract RGB565 blue component (5 bits)"""
    return color & 0x1F


def rgb332_val(r: int, g: int, b: int) -> int:
    """Pack RGB components into RGB332 format"""
    return ((r & 0x07) << 5) | ((g & 0x07) << 2) | (b & 0x03)


def pack_u8_to_u16(high: int, low: int) -> int:
    """Pack two uint8 values into one uint16"""
    return ((high & 0xFF) << 8) | (low & 0xFF)


def can_compress_diff(diff: int) -> bool:
    """
    Check if difference can be compressed into one byte
    
    Condition: R component <=7, G component <=7, B component <=3
    """
    r = rgb565_get_r(diff)
    g = rgb565_get_g(diff)
    b = rgb565_get_b(diff)
    
    return (r <= 7) and (g <= 7) and (b <= 3)


def compress_diff_to_byte(diff: int) -> int:
    """Compress difference into one byte (RGB332 format)"""
    r = rgb565_get_r(diff)
    g = rgb565_get_g(diff)
    b = rgb565_get_b(diff)
    
    return rgb332_val(r, g, b)


def calculate_diff_length(data: np.ndarray, start: int, end: int, output: np.ndarray) -> int:
    """
    Calculate the number of pixels that meet differential encoding criteria
    
    Args:
        data: Pixel data array
        start: Start position
        end: End position (exclusive)
        output: Output difference data buffer
        
    Returns:
        Compressible difference pixel count (odd number, because 2 differences merge into 1 uint16)
    """
    if end - start < DIFF_THRESHOLD:
        return 0
    
    idx = 0
    idx_tmp = 0
    tmp = [0, 0]
    
    i = start
    diff_count = 1
    rle_cnt = 1
    
    while i < end - 1:
        current_pixel = int(data[i])
        next_pixel = int(data[i + 1])
        diff = current_pixel ^ next_pixel
        
        # Meet differential encoding conditions
        if can_compress_diff(diff):
            diff_count += 1
            
            tmp[idx_tmp] = compress_diff_to_byte(diff)
            idx_tmp += 1
            
            if idx_tmp == 2:
                output[idx] = pack_u8_to_u16(tmp[0], tmp[1])
                idx += 1
                idx_tmp = 0
                if idx == 31:  # Actual encoded source data 31*2 + 1
                    break
            
            if diff == 0:
                # diff=0 means 2 identical pixels
                rle_cnt += 1
                if rle_cnt > RLE_THRESHOLD:
                    # Consecutive pixels exceed 3+1, exit for RLE processing
                    diff_count -= RLE_THRESHOLD
                    break
            else:
                rle_cnt = 0
            
            i += 1
        else:
            break
    
    # Handle remaining differences
    if idx_tmp == 1:
        # Odd number of differences, reduce by one
        diff_count -= 1
    
    # Number of pixels meeting differential encoding must be odd and not zero
    if diff_count % 2 == 0 and diff_count != 0:
        diff_count -= 1
    
    if diff_count >= DIFF_THRESHOLD:
        return diff_count
    else:
        return 0


def encode_rgb565_rle_diff(input_data: np.ndarray, width: int, height: int) -> Tuple[Optional[np.ndarray], int, float]:
    """
    RLE+Differential mixed encoding function
    
    Args:
        input_data: Input RGB565 data, length is width*height
        width: Image width
        height: Image height
        
    Returns:
        (output_data, output_size, compression_ratio)
    """
    pixel_count = width * height
    
    if width == 0 or height == 0 or pixel_count == 0:
        return None, 0, 0.0
    
    # Find encoding flag
    encode_flag, encode_flag_cs, encode_flag_mode, flag_ok = find_encode_flag(input_data)
    
    # Estimate maximum output size
    max_output_size = 6 + height + (pixel_count * 2)
    output = np.zeros(max_output_size, dtype=np.uint32)
    
    # Allocate row offset array
    row_offsets = np.zeros(height + 1, dtype=np.uint32)
    
    # Differential encoding data buffer
    encoded_diff_data = np.zeros(64, dtype=np.uint16)
    
    # Encoding data buffer
    encoded_data = np.zeros(max_output_size, dtype=np.uint32)
    encoded_index = 0
    
    # Traverse row by row
    for y in range(height):
        row_offsets[y] = encoded_index
        row_start = y * width
        row = input_data[row_start:row_start + width]
        
        col = 0
        while col < width:
            # Try differential encoding
            diff_len = calculate_diff_length(row, col, width, encoded_diff_data)
            
            if diff_len >= DIFF_THRESHOLD:
                base_color = int(row[col])
                
                if diff_len >= 128:
                    # Long encoding
                    encoded_data[encoded_index] = encode_flag
                    encoded_index += 1
                    encoded_data[encoded_index] = base_color
                    encoded_index += 1
                    encoded_data[encoded_index] = 0x8000 + (diff_len // 2)
                    encoded_index += 1
                    
                    for i in range(diff_len // 2):
                        encoded_data[encoded_index] = encoded_diff_data[i]
                        encoded_index += 1
                else:
                    # Short encoding
                    encoded_data[encoded_index] = encode_flag + 0x80 + (diff_len // 2)
                    encoded_index += 1
                    encoded_data[encoded_index] = base_color
                    encoded_index += 1
                    
                    for i in range(diff_len // 2):
                        encoded_data[encoded_index] = encoded_diff_data[i]
                        encoded_index += 1
                
                col += diff_len
            else:
                # Try RLE encoding
                rle_len = check_rle_length(row, col, width)
                
                if rle_len >= RLE_THRESHOLD:
                    color = int(row[col])
                    
                    if rle_len >= 128:
                        # Long encoding
                        encoded_data[encoded_index] = encode_flag
                        encoded_index += 1
                        encoded_data[encoded_index] = color
                        encoded_index += 1
                        encoded_data[encoded_index] = rle_len
                        encoded_index += 1
                    else:
                        # Short encoding
                        encoded_data[encoded_index] = encode_flag + rle_len
                        encoded_index += 1
                        encoded_data[encoded_index] = color
                        encoded_index += 1
                    
                    col += rle_len
                else:
                    # Store original pixel directly
                    color_tmp = int(row[col])
                    
                    if (color_tmp & 0xFF00) == encode_flag:
                        # Pixel conflicts with flag code
                        encoded_data[encoded_index] = encode_flag + 1
                        encoded_index += 1
                        encoded_data[encoded_index] = color_tmp
                        encoded_index += 1
                    else:
                        encoded_data[encoded_index] = color_tmp
                        encoded_index += 1
                    
                    col += 1
    
    row_offsets[height] = encoded_index
    
    # Calculate upgrade table
    upgrade = []
    for i in range(height - 1):
        tmp0 = row_offsets[i]
        tmp1 = row_offsets[i + 1]
        if tmp0 > tmp1:
            upgrade.append(i + 1)
    
    upgrade_len = len(upgrade)
    
    # Calculate row table start coordinate and encoding data start coordinate
    row_offset_addr = 6 + upgrade_len
    encode_data_addr = row_offset_addr + height + 1
    
    # Fill header
    output[0] = width
    output[1] = height
    output[2] = encode_flag
    output[3] = upgrade_len
    output[4] = row_offset_addr
    output[5] = encode_data_addr
    
    idx = 6
    
    # Write upgrade table
    if upgrade_len > 0:
        for val in upgrade:
            output[idx] = val
            idx += 1
    
    # Write row offset table
    for i in range(height + 1):
        output[idx] = row_offsets[i]
        idx += 1
    
    # Write encoding data
    for i in range(encoded_index):
        output[idx] = encoded_data[i]
        idx += 1
    
    # Calculate compression ratio
    original_size = pixel_count * 2
    compressed_size = idx * 2
    compression_ratio = (compressed_size / original_size) * 100.0
    
    # Convert to uint16 array for return
    result = output[:idx].astype(np.uint16)
    
    return result, idx, compression_ratio


if __name__ == "__main__":
    # Test code
    # Create a test image with gradients
    test_data = np.array([
        0xF800, 0xF801, 0xF802, 0xF803, 0xF804, 0xF805, 0xF806, 0xF807,
        0x07E0, 0x07E0, 0x07E0, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
    ], dtype=np.uint16)
    
    width, height = 8, 2
    
    result, size, ratio = encode_rgb565_rle_diff(test_data, width, height)
    if result is not None:
        print(f"Encoding successful!")
        print(f"Output size: {size} uint16")
        print(f"Compression ratio: {ratio:.2f}%")
        print("\nGenerated C array:")
        c_code = generate_c_array(result, size, width, height, ratio, "test.bmp")
        print(c_code)
