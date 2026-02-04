"""
zhRGB565 Core Compression Library

This module provides RGB565 image compression functionality using:
1. RLE (Run-Length Encoding) - Pure RLE encoding for simple compression
2. RLE+Diff - Mixed encoding combining RLE and differential encoding for better compression of gradient areas
"""

# Import all functions from RLE encoder module
from .rle_encoder import (
    # RGB565 color component extraction functions
    rgb565_get_r,
    rgb565_get_g, 
    rgb565_get_b,
    
    # RLE encoding utility functions
    find_encode_flag,
    check_rle_length,
    
    # Main RLE encoding function
    encode_rgb565_rle_only,
    
    # C array generation function
    generate_c_array
)

# Import all functions from RLE+Diff encoder module
from .rle_diff_encoder import (
    # RGB565 color component extraction functions
    rgb565_get_r as rgb565_get_r_diff,
    rgb565_get_g as rgb565_get_g_diff,
    rgb565_get_b as rgb565_get_b_diff,
    
    # Utility functions
    rgb332_val,
    pack_u8_to_u16,
    can_compress_diff,
    compress_diff_to_byte,
    calculate_diff_length,
    
    # Main RLE+Diff encoding function
    encode_rgb565_rle_diff
)

# Define what should be imported when using "from zhRGB565_core import *"
__all__ = [
    # RLE encoder functions
    'rgb565_get_r',
    'rgb565_get_g',
    'rgb565_get_b',
    'find_encode_flag',
    'check_rle_length',
    'encode_rgb565_rle_only',
    'generate_c_array',
    
    # RLE+Diff encoder functions
    'rgb565_get_r_diff',
    'rgb565_get_g_diff',
    'rgb565_get_b_diff',
    'rgb332_val',
    'pack_u8_to_u16',
    'can_compress_diff',
    'compress_diff_to_byte',
    'calculate_diff_length',
    'encode_rgb565_rle_diff'
]

# Version information
__version__ = "1.0.0"
__author__ = "zhRGB565 Project"