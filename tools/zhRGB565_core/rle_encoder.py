"""
RLE (Run-Length Encoding) Encoder
Pure RLE encoding implementation
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


def find_encode_flag(img: np.ndarray) -> Tuple[int, int, int, bool]:
    """
    Find optimal encoding flag
    
    Priority:
    1. Unused high byte values (XX00 format)
    2. High bytes where all pixels have at least 3 consecutive identical values
    3. Least used high bytes
    
    Args:
        img: RGB565 image data array
        
    Returns:
        (encode_flag, encode_flag_cs, encode_flag_mode, is_perfect)
        encode_flag: Flag value (high byte << 8)
        encode_flag_cs: Related info (minimum consecutive count or occurrence count)
        encode_flag_mode: Mode (0=unused, 1=RLE perfect, 2=least used)
        is_perfect: Whether perfect flag was found
    """
    pixel_count = len(img)
    
    if pixel_count == 0:
        return 0xFF00, 0, 0, True
    
    # Count high byte usage
    used_map = [0] * 32
    count_map = [0] * 256
    
    for pixel in img:
        hi_byte = (pixel >> 8) & 0xFF
        used_map[hi_byte // 8] |= (1 << (hi_byte % 8))
        count_map[hi_byte] += 1
    
    # Priority 1: Unused high byte values
    for hi_byte in range(256):
        if (used_map[hi_byte // 8] & (1 << (hi_byte % 8))) == 0:
            return (hi_byte << 8), 0, 0, True
    
    # Priority 2: Perfect RLE high byte values
    for hi_byte in range(256):
        if count_map[hi_byte] == 0:
            continue
        
        is_perfect = True
        min_continuous = 0xFFFF
        
        i = 0
        while i < pixel_count:
            # Skip pixels that don't have this high byte
            while i < pixel_count and ((img[i] >> 8) & 0xFF) != hi_byte:
                i += 1
            
            if i >= pixel_count:
                break
            
            # Check continuous segments
            current_value = img[i]
            continuous_count = 1
            j = i + 1
            
            while j < pixel_count and img[j] == current_value:
                continuous_count += 1
                j += 1
            
            # Check if condition is met (at least 3 consecutive identical)
            if continuous_count < RLE_THRESHOLD:
                is_perfect = False
                break
            
            if continuous_count < min_continuous:
                min_continuous = continuous_count
            
            i = j
        
        if is_perfect and min_continuous != 0xFFFF:
            # Find a specific pixel value as flag
            concrete_value = hi_byte << 8
            for k in range(pixel_count):
                if ((img[k] >> 8) & 0xFF) == hi_byte:
                    concrete_value = img[k]
                    break
            
            return (concrete_value & 0xFF00), min_continuous, 1, True
    
    # Priority 3: Least used high byte values
    min_hi_byte = 0
    min_count = 0xFFFF
    
    for hi_byte in range(256):
        if count_map[hi_byte] > 0:
            if count_map[hi_byte] < min_count:
                min_count = count_map[hi_byte]
                min_hi_byte = hi_byte
            elif count_map[hi_byte] == min_count:
                if hi_byte < min_hi_byte:
                    min_hi_byte = hi_byte
    
    if min_count == 0xFFFF:
        return 0xFF00, 0, 2, False
    
    return (min_hi_byte << 8), min_count, 2, False


def check_rle_length(pixels: np.ndarray, start: int, end: int) -> int:
    """
    Check RLE length starting from position
    
    Args:
        pixels: Pixel array
        start: Start position
        end: End position (exclusive)
        
    Returns:
        Number of consecutive identical pixels
    """
    if start >= end:
        return 0
    
    first_color = int(pixels[start])
    length = 1
    
    for i in range(start + 1, end):
        if int(pixels[i]) == first_color:
            length += 1
            if length == 0xFFFF:
                break
        else:
            break
    
    return length


def encode_rgb565_rle_only(input_data: np.ndarray, width: int, height: int) -> Tuple[Optional[np.ndarray], int, float]:
    """
    Pure RLE encoding function
    
    Args:
        input_data: Input RGB565 data, length is width*height
        width: Image width
        height: Image height
        
    Returns:
        (output_data, output_size, compression_ratio)
        output_data: Encoded data array
        output_size: Encoded data size (in uint16_t units)
        compression_ratio: Compression ratio (percentage, smaller is better)
    """
    pixel_count = width * height
    
    if width == 0 or height == 0 or pixel_count == 0:
        return None, 0, 0.0
    
    # Find encoding flag
    encode_flag, encode_flag_cs, encode_flag_mode, flag_ok = find_encode_flag(input_data)
    
    # Estimate maximum output size
    max_output_size = 10 + height + (pixel_count * 2)
    output = np.zeros(max_output_size, dtype=np.uint32)  # Use 32-bit to avoid overflow
    
    # Allocate row offset array (using 32-bit calculation)
    row_offsets = np.zeros(height + 1, dtype=np.uint32)
    
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
            # Check RLE length
            rle_len = check_rle_length(row, col, width)
            
            if rle_len >= RLE_THRESHOLD:
                color = int(row[col])
                
                if rle_len >= 128:
                    # Long encoding: flag, color, count
                    encoded_data[encoded_index] = encode_flag
                    encoded_index += 1
                    encoded_data[encoded_index] = color
                    encoded_index += 1
                    encoded_data[encoded_index] = rle_len
                    encoded_index += 1
                else:
                    # Short encoding: flag + count, color
                    encoded_data[encoded_index] = encode_flag + rle_len
                    encoded_index += 1
                    encoded_data[encoded_index] = color
                    encoded_index += 1
                
                col += rle_len
            else:
                # Handle pixels that conflict with flag
                color_tmp = int(row[col])
                
                if (color_tmp & 0xFF00) == encode_flag:
                    # Pixel conflicts with flag code, use RLE short encoding to store single pixel
                    encoded_data[encoded_index] = encode_flag + 1
                    encoded_index += 1
                    encoded_data[encoded_index] = color_tmp
                    encoded_index += 1
                else:
                    # Store original pixel directly
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
    original_size = pixel_count * 2  # Original data size (bytes)
    compressed_size = idx * 2        # Compressed size (bytes)
    compression_ratio = (compressed_size / original_size) * 100.0
    
    # Convert to uint16 array for return
    result = output[:idx].astype(np.uint16)
    
    return result, idx, compression_ratio


def generate_c_array(output_data: np.ndarray, output_size: int, 
                     width: int, height: int, compression_ratio: float,
                     src_path: str = "", array_name: str = "img") -> str:
    """
    Generate C language array format string
    
    Args:
        output_data: Encoded data
        output_size: Data size
        width: Image width
        height: Image height
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
    
    idx = 6
    upgrade_len = output_data[3]
    row_offset_addr = output_data[4]
    
    # Write upgrade table
    if upgrade_len > 0:
        lines.append("    /* level_up table */")
        line = "    "
        count = 0
        for i in range(upgrade_len):
            line += f"{output_data[idx]}, "
            idx += 1
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
    for i in range(height + 1):
        line += f"{output_data[idx]}"
        idx += 1
        line += ", "  # Add comma after all elements (including last)
        if (i + 1) % 16 == 0:
            lines.append(line.rstrip())
            line = "    "
    if line.strip():
        lines.append(line.rstrip())
    lines.append("")
    
    # Write encoding data
    lines.append("    /* Encoded data */")
    
    # Format output by lines
    hhcnt = 0
    line_base = 0
    next_line = 1
    current_line = 0
    
    lines.append(f"    /* 0 */")
    line = "    "
    
    while idx < output_size:
        if next_line == height:
            # Last line, output all
            while idx < output_size:
                line += f"0x{int(output_data[idx]):04X}, "
                idx += 1
                hhcnt += 1
                if hhcnt % 16 == 0:
                    lines.append(line.rstrip())
                    line = "    "
        else:
            current_line_pos = output_data[row_offset_addr + current_line] + line_base
            next_line_pos = output_data[row_offset_addr + next_line] + line_base
            
            if current_line_pos > next_line_pos:
                line_base += 65536
                next_line_pos += 65536
            
            for j in range(current_line_pos, next_line_pos):
                if idx >= output_size:
                    break
                line += f"0x{int(output_data[idx]):04X}, "
                idx += 1
                hhcnt += 1
                if hhcnt % 16 == 0 and j != next_line_pos - 1:
                    lines.append(line.rstrip())
                    line = "    "
            
            current_line = next_line
            next_line += 1
            hhcnt = 0
        
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
    
    width, height = 8, 2
    
    result, size, ratio = encode_rgb565_rle_only(test_data, width, height)
    if result is not None:
        print(f"Encoding successful!")
        print(f"Output size: {size} uint16")
        print(f"Compression ratio: {ratio:.2f}%")
        print("\nGenerated C array:")
        c_code = generate_c_array(result, size, width, height, ratio, "test.bmp")
        print(c_code)
