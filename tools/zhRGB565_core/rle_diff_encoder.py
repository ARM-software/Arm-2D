"""
RLE + 差值编码 混合编码器
结合了RLE和差值编码，对渐变区域有更好的压缩效果
"""

import numpy as np
from typing import Tuple, Optional
from zhRGB565_core.rle_encoder import find_encode_flag, check_rle_length, RLE_THRESHOLD, generate_c_array

# 差值编码阈值
DIFF_THRESHOLD = 7  # 差值编码最小像素数


def rgb565_get_r(color: int) -> int:
    """提取RGB565的红色分量(5位)"""
    return (color >> 11) & 0x1F


def rgb565_get_g(color: int) -> int:
    """提取RGB565的绿色分量(6位)"""
    return (color >> 5) & 0x3F


def rgb565_get_b(color: int) -> int:
    """提取RGB565的蓝色分量(5位)"""
    return color & 0x1F


def rgb332_val(r: int, g: int, b: int) -> int:
    """将RGB分量打包为RGB332格式"""
    return ((r & 0x07) << 5) | ((g & 0x07) << 2) | (b & 0x03)


def pack_u8_to_u16(high: int, low: int) -> int:
    """将两个uint8打包为一个uint16"""
    return ((high & 0xFF) << 8) | (low & 0xFF)


def can_compress_diff(diff: int) -> bool:
    """
    检查差值是否可以压缩为一个字节
    
    条件: R分量<=7, G分量<=7, B分量<=3
    """
    r = rgb565_get_r(diff)
    g = rgb565_get_g(diff)
    b = rgb565_get_b(diff)
    
    return (r <= 7) and (g <= 7) and (b <= 3)


def compress_diff_to_byte(diff: int) -> int:
    """将差值压缩为一个字节(RGB332格式)"""
    r = rgb565_get_r(diff)
    g = rgb565_get_g(diff)
    b = rgb565_get_b(diff)
    
    return rgb332_val(r, g, b)


def calculate_diff_length(data: np.ndarray, start: int, end: int, output: np.ndarray) -> int:
    """
    计算满足差值编码的像素数
    
    Args:
        data: 像素数据数组
        start: 起始位置
        end: 结束位置(不包含)
        output: 输出差值数据缓冲区
        
    Returns:
        可压缩的差值像素数(奇数，因为2个差值合并为1个uint16)
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
        
        # 满足差值编码条件
        if can_compress_diff(diff):
            diff_count += 1
            
            tmp[idx_tmp] = compress_diff_to_byte(diff)
            idx_tmp += 1
            
            if idx_tmp == 2:
                output[idx] = pack_u8_to_u16(tmp[0], tmp[1])
                idx += 1
                idx_tmp = 0
                if idx == 31:  # 实际编码源数据 31*2 + 1
                    break
            
            if diff == 0:
                # 差值=0，说明有2个相同的像素
                rle_cnt += 1
                if rle_cnt > RLE_THRESHOLD:
                    # 连续像素超过3+1个，退出让RLE处理
                    diff_count -= RLE_THRESHOLD
                    break
            else:
                rle_cnt = 0
            
            i += 1
        else:
            break
    
    # 处理剩余的差值
    if idx_tmp == 1:
        # 奇数个差值，减少一个
        diff_count -= 1
    
    # 满足差值编码的像素数必须是奇数且不为0
    if diff_count % 2 == 0 and diff_count != 0:
        diff_count -= 1
    
    if diff_count >= DIFF_THRESHOLD:
        return diff_count
    else:
        return 0


def encode_rgb565_rle_diff(input_data: np.ndarray, width: int, height: int) -> Tuple[Optional[np.ndarray], int, float]:
    """
    RLE+差值混合编码函数
    
    Args:
        input_data: 输入RGB565数据，长度为width*height
        width: 图像宽度
        height: 图像高度
        
    Returns:
        (output_data, output_size, compression_ratio)
    """
    pixel_count = width * height
    
    if width == 0 or height == 0 or pixel_count == 0:
        return None, 0, 0.0
    
    # 查找编码标志
    encode_flag, encode_flag_cs, encode_flag_mode, flag_ok = find_encode_flag(input_data)
    
    # 预估最大输出大小
    max_output_size = 6 + height + (pixel_count * 2)
    output = np.zeros(max_output_size, dtype=np.uint32)
    
    # 分配行偏移数组
    row_offsets = np.zeros(height + 1, dtype=np.uint32)
    
    # 差值编码数据缓冲区
    encoded_diff_data = np.zeros(64, dtype=np.uint16)
    
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
            # 尝试差值编码
            diff_len = calculate_diff_length(row, col, width, encoded_diff_data)
            
            if diff_len >= DIFF_THRESHOLD:
                base_color = int(row[col])
                
                if diff_len >= 128:
                    # 长编码
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
                    # 短编码
                    encoded_data[encoded_index] = encode_flag + 0x80 + (diff_len // 2)
                    encoded_index += 1
                    encoded_data[encoded_index] = base_color
                    encoded_index += 1
                    
                    for i in range(diff_len // 2):
                        encoded_data[encoded_index] = encoded_diff_data[i]
                        encoded_index += 1
                
                col += diff_len
            else:
                # 尝试RLE编码
                rle_len = check_rle_length(row, col, width)
                
                if rle_len >= RLE_THRESHOLD:
                    color = int(row[col])
                    
                    if rle_len >= 128:
                        # 长编码
                        encoded_data[encoded_index] = encode_flag
                        encoded_index += 1
                        encoded_data[encoded_index] = color
                        encoded_index += 1
                        encoded_data[encoded_index] = rle_len
                        encoded_index += 1
                    else:
                        # 短编码
                        encoded_data[encoded_index] = encode_flag + rle_len
                        encoded_index += 1
                        encoded_data[encoded_index] = color
                        encoded_index += 1
                    
                    col += rle_len
                else:
                    # 直接存储原始像素
                    color_tmp = int(row[col])
                    
                    if (color_tmp & 0xFF00) == encode_flag:
                        # 像素与标志码冲突
                        encoded_data[encoded_index] = encode_flag + 1
                        encoded_index += 1
                        encoded_data[encoded_index] = color_tmp
                        encoded_index += 1
                    else:
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
    original_size = pixel_count * 2
    compressed_size = idx * 2
    compression_ratio = (compressed_size / original_size) * 100.0
    
    # 转换为uint16数组返回
    result = output[:idx].astype(np.uint16)
    
    return result, idx, compression_ratio


if __name__ == "__main__":
    # 测试代码
    # 创建一个包含渐变的测试图像
    test_data = np.array([
        0xF800, 0xF801, 0xF802, 0xF803, 0xF804, 0xF805, 0xF806, 0xF807,
        0x07E0, 0x07E0, 0x07E0, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
    ], dtype=np.uint16)
    
    width, height = 8, 2
    
    result, size, ratio = encode_rgb565_rle_diff(test_data, width, height)
    if result is not None:
        print(f"编码成功!")
        print(f"输出大小: {size} uint16")
        print(f"压缩率: {ratio:.2f}%")
        print("\n生成的C数组:")
        c_code = generate_c_array(result, size, width, height, ratio, "test.bmp")
        print(c_code)
