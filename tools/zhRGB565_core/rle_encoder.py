"""
RLE (Run-Length Encoding) 编码器
纯RLE编码实现
"""

import numpy as np
from typing import Tuple, Optional

# 编码阈值
RLE_THRESHOLD = 3  # RLE编码最小连续像素数


def rgb565_get_r(color: int) -> int:
    """提取RGB565的红色分量(5位)"""
    return (color >> 11) & 0x1F


def rgb565_get_g(color: int) -> int:
    """提取RGB565的绿色分量(6位)"""
    return (color >> 5) & 0x3F


def rgb565_get_b(color: int) -> int:
    """提取RGB565的蓝色分量(5位)"""
    return color & 0x1F


def find_encode_flag(img: np.ndarray) -> Tuple[int, int, int, bool]:
    """
    查找最佳编码标志位
    
    优先级:
    1. 从未使用的高字节值(XX00格式)
    2. 高字节的所有像素都至少3个连续相同
    3. 使用次数最少的高字节
    
    Args:
        img: RGB565图像数据数组
        
    Returns:
        (encode_flag, encode_flag_cs, encode_flag_mode, is_perfect)
        encode_flag: 标志值(高字节<<8)
        encode_flag_cs: 相关信息(最小连续次数或出现次数)
        encode_flag_mode: 模式(0=未使用, 1=RLE完美, 2=最少使用)
        is_perfect: 是否找到完美标志
    """
    pixel_count = len(img)
    
    if pixel_count == 0:
        return 0xFF00, 0, 0, True
    
    # 统计高字节使用情况
    used_map = [0] * 32
    count_map = [0] * 256
    
    for pixel in img:
        hi_byte = (pixel >> 8) & 0xFF
        used_map[hi_byte // 8] |= (1 << (hi_byte % 8))
        count_map[hi_byte] += 1
    
    # 优先级1: 从未使用的高字节
    for hi_byte in range(256):
        if (used_map[hi_byte // 8] & (1 << (hi_byte % 8))) == 0:
            return (hi_byte << 8), 0, 0, True
    
    # 优先级2: 完美RLE的高字节
    for hi_byte in range(256):
        if count_map[hi_byte] == 0:
            continue
        
        is_perfect = True
        min_continuous = 0xFFFF
        
        i = 0
        while i < pixel_count:
            # 跳过不是这个高字节的像素
            while i < pixel_count and ((img[i] >> 8) & 0xFF) != hi_byte:
                i += 1
            
            if i >= pixel_count:
                break
            
            # 检查连续段
            current_value = img[i]
            continuous_count = 1
            j = i + 1
            
            while j < pixel_count and img[j] == current_value:
                continuous_count += 1
                j += 1
            
            # 检查是否满足条件(至少3个连续相同)
            if continuous_count < RLE_THRESHOLD:
                is_perfect = False
                break
            
            if continuous_count < min_continuous:
                min_continuous = continuous_count
            
            i = j
        
        if is_perfect and min_continuous != 0xFFFF:
            # 找一个具体的像素值作为标志
            concrete_value = hi_byte << 8
            for k in range(pixel_count):
                if ((img[k] >> 8) & 0xFF) == hi_byte:
                    concrete_value = img[k]
                    break
            
            return (concrete_value & 0xFF00), min_continuous, 1, True
    
    # 优先级3: 最少使用的高字节
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
    检查从start位置开始的RLE长度
    
    Args:
        pixels: 像素数组
        start: 起始位置
        end: 结束位置(不包含)
        
    Returns:
        连续相同像素的数量
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
    纯RLE编码函数
    
    Args:
        input_data: 输入RGB565数据，长度为width*height
        width: 图像宽度
        height: 图像高度
        
    Returns:
        (output_data, output_size, compression_ratio)
        output_data: 编码后的数据数组
        output_size: 编码后数据大小(以uint16_t为单位)
        compression_ratio: 压缩率(百分比，越小越好)
    """
    pixel_count = width * height
    
    if width == 0 or height == 0 or pixel_count == 0:
        return None, 0, 0.0
    
    # 查找编码标志
    encode_flag, encode_flag_cs, encode_flag_mode, flag_ok = find_encode_flag(input_data)
    
    # 预估最大输出大小
    max_output_size = 10 + height + (pixel_count * 2)
    output = np.zeros(max_output_size, dtype=np.uint32)  # 使用32位避免溢出
    
    # 分配行偏移数组(使用32位计算)
    row_offsets = np.zeros(height + 1, dtype=np.uint32)
    
    # 编码数据缓冲区
    encoded_data = np.zeros(max_output_size, dtype=np.uint32)
    encoded_index = 0
    
    # 逐行遍历
    for y in range(height):
        row_offsets[y] = encoded_index
        row_start = y * width
        row = input_data[row_start:row_start + width]
        
        col = 0
        while col < width:
            # 检查RLE长度
            rle_len = check_rle_length(row, col, width)
            
            if rle_len >= RLE_THRESHOLD:
                color = int(row[col])
                
                if rle_len >= 128:
                    # 长编码: flag, color, count
                    encoded_data[encoded_index] = encode_flag
                    encoded_index += 1
                    encoded_data[encoded_index] = color
                    encoded_index += 1
                    encoded_data[encoded_index] = rle_len
                    encoded_index += 1
                else:
                    # 短编码: flag + count, color
                    encoded_data[encoded_index] = encode_flag + rle_len
                    encoded_index += 1
                    encoded_data[encoded_index] = color
                    encoded_index += 1
                
                col += rle_len
            else:
                # 处理与标志位重复的像素
                color_tmp = int(row[col])
                
                if (color_tmp & 0xFF00) == encode_flag:
                    # 像素与标志码冲突，使用RLE短编码存储单个像素
                    encoded_data[encoded_index] = encode_flag + 1
                    encoded_index += 1
                    encoded_data[encoded_index] = color_tmp
                    encoded_index += 1
                else:
                    # 直接存储原始像素
                    encoded_data[encoded_index] = color_tmp
                    encoded_index += 1
                
                col += 1
    
    row_offsets[height] = encoded_index
    
    # 计算升阶表
    upgrade = []
    for i in range(height - 1):
        tmp0 = row_offsets[i]
        tmp1 = row_offsets[i + 1]
        if tmp0 > tmp1:
            upgrade.append(i + 1)
    
    upgrade_len = len(upgrade)
    
    # 计算行表起点坐标和编码数据起点坐标
    row_offset_addr = 6 + upgrade_len
    encode_data_addr = row_offset_addr + height + 1
    
    # 填充头部
    output[0] = width
    output[1] = height
    output[2] = encode_flag
    output[3] = upgrade_len
    output[4] = row_offset_addr
    output[5] = encode_data_addr
    
    idx = 6
    
    # 写入升阶表
    if upgrade_len > 0:
        for val in upgrade:
            output[idx] = val
            idx += 1
    
    # 写入行偏移表
    for i in range(height + 1):
        output[idx] = row_offsets[i]
        idx += 1
    
    # 写入编码数据
    for i in range(encoded_index):
        output[idx] = encoded_data[i]
        idx += 1
    
    # 计算压缩率
    original_size = pixel_count * 2  # 原始数据大小(字节)
    compressed_size = idx * 2        # 压缩后大小(字节)
    compression_ratio = (compressed_size / original_size) * 100.0
    
    # 转换为uint16数组返回
    result = output[:idx].astype(np.uint16)
    
    return result, idx, compression_ratio


def generate_c_array(output_data: np.ndarray, output_size: int, 
                     width: int, height: int, compression_ratio: float,
                     src_path: str = "", array_name: str = "img") -> str:
    """
    生成C语言数组格式的字符串
    
    Args:
        output_data: 编码后的数据
        output_size: 数据大小
        width: 图像宽度
        height: 图像高度
        compression_ratio: 压缩率
        src_path: 源文件路径
        array_name: 数组名称
        
    Returns:
        C语言代码字符串
    """
    import os
    
    # 获取基础文件名
    base_name = os.path.splitext(os.path.basename(src_path))[0] if src_path else "image"
    
    lines = []
    lines.append("// Compressed RGB565 data")
    lines.append(f"// Source file: {src_path}")
    lines.append(f"// Original size: {width} x {height} = {width * height} pixels")
    lines.append(f"// Compression ratio: {compression_ratio:.2f}%")
    lines.append("")
    lines.append(f"const uint16_t _{base_name}_zhRGB565_Data[{output_size}] = {{")
    lines.append("")
    
    # 写入头部信息
    lines.append("    /* width, height, encode_flag, level_up_table_len, row_offset_addr, data_addr */")
    lines.append(f"    {output_data[0]}, {output_data[1]}, 0x{output_data[2]:04X}, {output_data[3]}, {output_data[4]}, {output_data[5]},")
    lines.append("")
    
    idx = 6
    upgrade_len = output_data[3]
    row_offset_addr = output_data[4]
    
    # 写入升阶表
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
    
    # 写入行偏移表
    lines.append(f"    /* Row offset table ({height} rows total) */")
    line = "    "
    for i in range(height + 1):
        line += f"{output_data[idx]}"
        idx += 1
        line += ", "  # 所有元素后面都加逗号（包括最后一个）
        if (i + 1) % 16 == 0:
            lines.append(line.rstrip())
            line = "    "
    if line.strip():
        lines.append(line.rstrip())
    lines.append("")
    
    # 写入编码数据
    lines.append("    /* Encoded data */")
    
    # 按行格式化输出
    hhcnt = 0
    line_base = 0
    next_line = 1
    current_line = 0
    
    lines.append(f"    /* 0 */")
    line = "    "
    
    while idx < output_size:
        if next_line == height:
            # 最后一行，全部输出
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
    # 测试代码
    # 创建一个简单的测试图像
    test_data = np.array([
        0xF800, 0xF800, 0xF800, 0x07E0, 0x07E0, 0x001F, 0x001F, 0x001F,
        0xF800, 0xF800, 0xF800, 0x07E0, 0x07E0, 0x001F, 0x001F, 0x001F,
    ], dtype=np.uint16)
    
    width, height = 8, 2
    
    result, size, ratio = encode_rgb565_rle_only(test_data, width, height)
    if result is not None:
        print(f"编码成功!")
        print(f"输出大小: {size} uint16")
        print(f"压缩率: {ratio:.2f}%")
        print("\n生成的C数组:")
        c_code = generate_c_array(result, size, width, height, ratio, "test.bmp")
        print(c_code)
