#!/usr/bin/python
# -*- coding: utf-8 -*-

# *************************************************************************************************
#  Arm 2D project
#  @file        ttf2c.py
#  @brief       TrueTypeFont to C-array converter
#
# *************************************************************************************************
#
# * Copyright (C) 2025 ARM Limited or its affiliates. All rights reserved.
# *
# * SPDX-License-Identifier: Apache-2.0
# *
# * Licensed under the Apache License, Version 2.0 (the License); you may
# * not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# * www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an AS IS BASIS, WITHOUT
# * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# */


import os
import sys
import argparse
import freetype
import numpy as np
import math
import binascii
import tempfile

c_head_string="""

/* generated with ttf2c.py (v2.4.0) */

#include "arm_2d_helper.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wgnu-variable-sized-type-not-at-end"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress=1296,1,64,177
#endif

typedef struct {
    uint16_t      hwIndex;
    arm_2d_size_t tCharSize;
    int16_t       iAdvance;
    int16_t       iBearingX;
    int16_t       iBearingY;
    uint8_t       chCodeLength;
    uint8_t       chUTF8[4];
} const __ttf_char_descriptor_t;
"""


c_body_string="""


ARM_SECTION(\"arm2d.tile.c_tileUTF8{0}A{5}Mask\")
static const arm_2d_tile_t c_tileUTF8{0}A{5}Mask = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {4},
        }},
    }},
    .tInfo = {{
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {{
            .chScheme = ARM_2D_COLOUR_{5}BIT,
        }},
    }},
    .pchBuffer = (uint8_t *)c_bmpUTF8{0}A{5}Font,
}};

#define __UTF8_FONT_SIZE_{5}__



static
IMPL_FONT_GET_CHAR_DESCRIPTOR(__utf8_a{5}_font_get_char_descriptor)
{{
    assert(NULL != ptFont);
    assert(NULL != ptDescriptor);
    assert(NULL != pchCharCode);

    arm_2d_user_font_t *ptThis = (arm_2d_user_font_t *)ptFont;
    ARM_2D_UNUSED(ptThis);

    memset(ptDescriptor, 0, sizeof(arm_2d_char_descriptor_t));

    ptDescriptor->tileChar.ptParent = (arm_2d_tile_t *)&ptFont->tileFont;
    ptDescriptor->tileChar.tInfo.bDerivedResource = true;

    /* use the white space as the default char */
    __ttf_char_descriptor_t *ptUTF8Char =
        (__ttf_char_descriptor_t *)&c_tUTF8{0}LookUpTableA{5}[dimof(c_tUTF8{0}LookUpTableA{5})-1];

    arm_foreach(__ttf_char_descriptor_t, c_tUTF8{0}LookUpTableA{5}, ptChar) {{
        if (0 == strncmp(   (char *)pchCharCode,
                            (char *)ptChar->chUTF8,
                            ptChar->chCodeLength)) {{
            /* found the UTF8 char */
            ptUTF8Char = ptChar;
            break;
        }}
    }}

    ptDescriptor->chCodeLength = ptUTF8Char->chCodeLength;
    ptDescriptor->tileChar.tRegion.tSize = ptUTF8Char->tCharSize;
    ptDescriptor->tileChar.tRegion.tLocation.iY = (int16_t)ptUTF8Char->hwIndex;

    ptDescriptor->iAdvance = ptUTF8Char->iAdvance;
    ptDescriptor->iBearingX= ptUTF8Char->iBearingX;
    ptDescriptor->iBearingY= ptUTF8Char->iBearingY;

    return ptDescriptor;
}}

ARM_SECTION(\"arm2d.asset.FONT.ARM_2D_FONT_{0}_A{5}\")
const
struct {{
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
}} ARM_2D_FONT_{0}_A{5} = {{

    .use_as__arm_2d_user_font_t = {{
        .use_as__arm_2d_font_t = {{
            .tileFont = impl_child_tile(
                c_tileUTF8{0}A{5}Mask,
                0,          /* x offset */
                0,          /* y offset */
                {1},        /* width */
                {4}         /* height */
            ),
            .tCharSize = {{
                .iWidth = {1},
                .iHeight = {2},
            }},
            .nCount =  {3},                             //!< Character count
            .fnGetCharDescriptor = &__utf8_a{5}_font_get_char_descriptor,
            .fnDrawChar = &__arm_2d_lcd_text_default_a{5}_font_draw_char,
        }},
        .hwCount = 1,
        .hwDefaultCharIndex = 1, /* tBlank */
    }},

    .tUTF8Table = {{
        .hwCount = {3},
        .hwOffset = 0,
    }},
}};

#undef __UTF8_FONT_SIZE_{5}__
"""


c_body_string_ascii="""


ARM_SECTION(\"arm2d.tile.c_tileUTF8{0}A{5}Mask\")
static const arm_2d_tile_t c_tileUTF8{0}A{5}Mask = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {4},
        }},
    }},
    .tInfo = {{
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {{
            .chScheme = ARM_2D_COLOUR_{5}BIT,
        }},
    }},
    .pchBuffer = (uint8_t *)c_bmpUTF8{0}A{5}Font,
}};

#define __UTF8_FONT_SIZE_{5}__



static
IMPL_FONT_GET_CHAR_DESCRIPTOR(__utf8_a{5}_font_get_char_descriptor)
{{
    assert(NULL != ptFont);
    assert(NULL != ptDescriptor);
    assert(NULL != pchCharCode);

    arm_2d_user_font_t *ptThis = (arm_2d_user_font_t *)ptFont;
    ARM_2D_UNUSED(ptThis);

    memset(ptDescriptor, 0, sizeof(arm_2d_char_descriptor_t));

    ptDescriptor->tileChar.ptParent = (arm_2d_tile_t *)&ptFont->tileFont;
    ptDescriptor->tileChar.tInfo.bDerivedResource = true;

    /* use the white space as the default char */
    __ttf_char_descriptor_t *ptUTF8Char = NULL;
        
    if (pchCharCode[0] > 0x20 && pchCharCode[0] <= 0x7e) {{
        ptUTF8Char = (__ttf_char_descriptor_t *)
            &c_tUTF8{0}LookUpTableA{5}[ 
                pchCharCode[0] - c_tUTF8{0}LookUpTableA{5}[0].chUTF8[0]];
    }} else {{
        /* use the white space as the default char */
        ptUTF8Char = (__ttf_char_descriptor_t *)
            &c_tUTF8{0}LookUpTableA{5}[dimof(c_tUTF8{0}LookUpTableA{5})-1];
    }}

    ptDescriptor->chCodeLength = ptUTF8Char->chCodeLength;
    ptDescriptor->tileChar.tRegion.tSize = ptUTF8Char->tCharSize;
    ptDescriptor->tileChar.tRegion.tLocation.iY = (int16_t)ptUTF8Char->hwIndex;

    ptDescriptor->iAdvance = ptUTF8Char->iAdvance;
    ptDescriptor->iBearingX= ptUTF8Char->iBearingX;
    ptDescriptor->iBearingY= ptUTF8Char->iBearingY;

    return ptDescriptor;
}}

ARM_SECTION(\"arm2d.asset.FONT.ARM_2D_FONT_{0}_A{5}\")
const
struct {{
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
}} ARM_2D_FONT_{0}_A{5} = {{

    .use_as__arm_2d_user_font_t = {{
        .use_as__arm_2d_font_t = {{
            .tileFont = impl_child_tile(
                c_tileUTF8{0}A{5}Mask,
                0,          /* x offset */
                0,          /* y offset */
                {1},        /* width */
                {4}         /* height */
            ),
            .tCharSize = {{
                .iWidth = {1},
                .iHeight = {2},
            }},
            .nCount =  {3},                             //!< Character count
            .fnGetCharDescriptor = &__utf8_a{5}_font_get_char_descriptor,
            .fnDrawChar = &__arm_2d_lcd_text_default_a{5}_font_draw_char,
        }},
        .hwCount = 1,
        .hwDefaultCharIndex = 1, /* tBlank */
    }},

    .tUTF8Table = {{
        .hwCount = {3},
        .hwOffset = 0,
    }},
}};

#undef __UTF8_FONT_SIZE_{5}__
"""


c_tail_string="""

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_warning=1296
#endif
"""

def generate_glyphs_data(input_file, text, pixel_size, font_bit_size, font_index):
    face = freetype.Face(input_file, index=font_index)
    face.set_pixel_sizes(0, pixel_size)

    glyphs_data = []
    current_index = 0

    width_max = 0
    height_max = 0

    for char in set(text):
        # Get the glyph bitmap
        face.load_char(char)
        bitmap = face.glyph.bitmap
        utf8_encoding = char.encode('utf-8')

        if len(bitmap.buffer) == 0:
            continue
        if list(utf8_encoding) == [0xef, 0xbb, 0xbf]:
            continue

        width_max = max(bitmap.width, width_max)
        height_max = max(bitmap.rows, height_max)
    
    width_max += 2
    height_max += 2

    for char in sorted(set(text)):
        face.load_char(char)
        bitmap = face.glyph.bitmap
        utf8_encoding = char.encode('utf-8')

        if len(bitmap.buffer) == 0:
            continue
        if list(utf8_encoding) == [0xef, 0xbb, 0xbf]:
            continue

        advance_width = math.ceil(face.glyph.advance.x / 64.0)
        bearing_x = face.glyph.bitmap_left
        bearing_y = face.glyph.bitmap_top
        width = bitmap.width
        height = bitmap.rows
        pitch = bitmap.pitch

        if bitmap.pixel_mode == freetype.FT_PIXEL_MODE_MONO:
            buffer = np.frombuffer(bytes(bitmap.buffer), dtype=np.uint8)
            bits_per_row = pitch * 8
            bitmap_array = np.unpackbits(buffer).reshape((height, bits_per_row))[:, :width]
            bitmap_array = bitmap_array.astype(np.uint8) * 255

        elif bitmap.pixel_mode == freetype.FT_PIXEL_MODE_GRAY2:
            buffer = np.frombuffer(bytes(bitmap.buffer), dtype=np.uint8)
            bits_per_row = pitch * 4
            bitmap_array = np.unpackbits(buffer).reshape((height, bits_per_row))[:, :width * 2]
            bitmap_array = (bitmap_array.reshape((height, width * 2)) * 85).astype(np.uint8)

        elif bitmap.pixel_mode == freetype.FT_PIXEL_MODE_GRAY4:
            buffer = np.frombuffer(bytes(bitmap.buffer), dtype=np.uint8)
            bits_per_row = pitch * 2
            bitmap_array = np.unpackbits(buffer).reshape((height, bits_per_row))[:, :width * 4]
            bitmap_array = (bitmap_array.reshape((height, width * 2)) * 17).astype(np.uint8)

        elif bitmap.pixel_mode == freetype.FT_PIXEL_MODE_GRAY:
            bitmap_array = np.array(bitmap.buffer, dtype=np.uint8).reshape((height, width))

        else:
            raise ValueError(f"Unsupported pixel mode: {bitmap.pixel_mode}")
            continue

        if width < width_max:
            if (width_max - width) > 1:
                padding = ((0, 0), (1, width_max - width - 1))
            else: 
                padding = ((0, 0), (0, width_max - width))
            bitmap_array = np.pad(bitmap_array, padding, 'constant')
        
        padding = ((1, 1), (0, 0))
        bitmap_array = np.pad(bitmap_array, padding, 'constant')

        height += 2

        char_index_advance = len(bitmap_array.flatten());

        if font_bit_size == 4:
            def RevBitQuadPerByte(byteArr):
                return ((byteArr & 0x0f) << 4) |  ((byteArr & 0xf0) >> 4)

            # temporary array with 2x reduced width & pad
            (r, c) = np.shape(bitmap_array)
            tmp = np.empty((0, int((c+1)/2)), dtype=np.uint8)

            for cur in bitmap_array:
                bitsArr = np.unpackbits(cur.astype(np.uint8))
                # generate indexes for MSB bit quadruplet every byte
                idx = np.arange(0, np.size(bitsArr), 8)
                idx = np.reshape(np.column_stack(
                    (np.column_stack((idx+0, idx+1)), np.column_stack((idx+2, idx+3)))),
                    (1,-1)),

                # extraction + endianness conversion appended in temp array
                # packbits is taking care of padding
                tmp = np.vstack([tmp, RevBitQuadPerByte(np.packbits(bitsArr[idx]))])

            bitmap_array = tmp

        elif font_bit_size == 2:
            def RevBitPairPerByte(byteArr):
                return ((byteArr & 0x03) << 6) |  ((byteArr & 0xc0) >> 6) | ((byteArr & 0x30) >> 2 ) | ((byteArr & 0x0c) << 2)

            # temporary array with 4x reduced width & pad
            (r, c) = np.shape(bitmap_array)
            tmp = np.empty((0, int((c+3)/4)), dtype=np.uint8)

            for cur in bitmap_array:
                bitsArr = np.unpackbits(cur.astype(np.uint8))

                # generate indexes for MSB bit pair every byte
                idx = np.arange(0, np.size(bitsArr), 8)
                idx = np.reshape(np.column_stack((idx+0, idx+1)), (1,-1))

                # extraction + endianness conversion appended in temp array
                tmp = np.vstack([tmp, RevBitPairPerByte(np.packbits(bitsArr[idx]))])

            bitmap_array = tmp

        elif font_bit_size == 1:
            def RevBitPerByte(byteArr):
                return ((byteArr & 0x01) << 7) | ((byteArr & 0x80) >> 7) | \
                       ((byteArr & 0x02) << 5) | ((byteArr & 0x40) >> 5) | \
                       ((byteArr & 0x04) << 3) | ((byteArr & 0x20) >> 3) | \
                       ((byteArr & 0x08) << 1) | ((byteArr & 0x10) >> 1)

            # temporary array with 8x reduced width & pad
            (r, c) = np.shape(bitmap_array)
            tmp = np.empty((0, int((c+7)/8)), dtype=np.uint8)

            for cur in bitmap_array:
                bitsArr = np.unpackbits(cur.astype(np.uint8))

                # generate indexes for MSB bit every byte
                idx = np.arange(0, np.size(bitsArr), 8)

                # extraction + endianness conversion
                tmp = np.vstack([tmp, RevBitPerByte(np.packbits(bitsArr[idx]))])

            bitmap_array = tmp

        char_mask_array = bitmap_array.flatten()

        glyphs_data.append((char, char_mask_array, width + 2, height, current_index, advance_width, bearing_x, bearing_y, utf8_encoding))

        current_index += char_index_advance

    return glyphs_data, width_max, height_max


def utf8_to_c_array(utf8_bytes):
    return '{' + ', '.join([f'0x{byte:02x}' for byte in utf8_bytes]) + '}'

def write_c_code(glyphs_data, output_file, name, char_max_width, char_max_height, font_bit_size, ascii_mode):

    with open(output_file, "a") as f:

        print("ARM_SECTION(\"arm2d.asset.FONT.c_bmpUTF8{0}A{1}Font\")\nconst uint8_t c_bmpUTF8{0}A{1}Font[] = {{\n"
                .format(name, font_bit_size), 
                file=f)

        for char, data, width, height, index, advance_width, bearing_x, bearing_y, utf8_encoding in glyphs_data:
            utf8_c_array = utf8_to_c_array(utf8_encoding)
            f.write(f"\n    // Glyph for character {utf8_c_array}\n")
            hex_str = binascii.hexlify(data).decode()

            for i in range(0, len(hex_str), char_max_width*2):
                line = hex_str[i:i+char_max_width*2]
                spaced_line = ' '.join(f"0x{line[j:j+2]}," for j in range(0, len(line), 2))
                f.write("    ");
                f.write(spaced_line)
                f.write("\n");

        f.write("    // a white space\n    ");
        f.write("0x00, " * (char_max_width * char_max_height))
        f.write("\n};\n\n")

        print("ARM_SECTION(\"arm2d.asset.FONT.c_tUTF8{0}LookUpTableA{1}\")\n const __ttf_char_descriptor_t c_tUTF8{0}LookUpTableA{1}[{2}] = {{\n"
                .format(name, font_bit_size, len(glyphs_data) + 1), file=f)

        last_index = 0;
        last_advance = 0;
        last_height = 0;
        for char, data, width, height, index, advance_width, bearing_x, bearing_y, utf8_encoding in glyphs_data:
            utf8_c_array = utf8_to_c_array(utf8_encoding)
            last_index = index
            last_advance = advance_width
            last_height = height;
            f.write(f"    {{ {round(index / char_max_width)}, {{ {width}, {height}, }}, {advance_width}, {bearing_x}, {bearing_y}, {len(utf8_encoding)}, {utf8_c_array} }},\n")

        last_index += char_max_width * last_height
        f.write(f"    {{ {round(last_index / char_max_width)}, {{ {char_max_width}, {char_max_height}, }}, {round(char_max_width / 2)}, {0}, {char_max_height}, 1, {{0x20}} }},\n")

        last_index += char_max_width * char_max_height

        f.write("};\n")

        if ascii_mode == True:
            print(c_body_string_ascii.format( name,
                                        char_max_width,
                                        char_max_height,
                                        len(glyphs_data),
                                        round(last_index / char_max_width)+1,
                                        font_bit_size), file=f)
        else:
            print(c_body_string.format( name,
                                        char_max_width,
                                        char_max_height,
                                        len(glyphs_data),
                                        round(last_index / char_max_width)+1,
                                        font_bit_size), file=f)


def main():
    parser = argparse.ArgumentParser(description='TrueTypeFont to C array converter (v2.4.0)')
    parser.add_argument("-i", "--input",    type=str,   help="Path to the TTF file",            required=True)
    parser.add_argument("--index",          type=int,   help="The Font Index in a TTC file",    required=False,     default=0)
    parser.add_argument("-t", "--text",     type=str,   help="Path to the text file",           required=False)
    parser.add_argument("-n", "--name",     type=str,   help="The customized UTF8 font name",   required=False,     default="UTF8")
    parser.add_argument("-o", "--output",   type=str,   help="Path to the output C file",       required=True)
    parser.add_argument("-p", "--pixelsize",type=int,   help="Font size in pixels",             required=False,     default=32)
    parser.add_argument("-s", "--fontbitsize",type=int, help="font bit size (1,2,4,8)",         required=False,     default=0)

    if len(sys.argv)==1:
        parser.print_help(sys.stderr)
        sys.exit(1)

    args = parser.parse_args()

    if args.fontbitsize not in [1, 2, 4, 8, 0]:
        print(f'Invalid alpha size={args.fontbitsize}')
        sys.exit(1)

    ascii_mode = False

    if args.text == None or args.text == "" :
        ascii_mode = True
        fd, args.text = tempfile.mkstemp()
        with os.fdopen(fd, 'w') as temptextfile:
            print("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", file=temptextfile, end='')

    with open(args.output, "w") as outputfile:
        print(c_head_string, file=outputfile)

    if args.fontbitsize in [1, 2, 4, 8]:
        with open(args.text, 'r', encoding='utf-8') as f:
            text = f.read()

            glyphs_data, char_max_width, char_max_height = generate_glyphs_data(args.input, text, args.pixelsize, args.fontbitsize, args.index)
            write_c_code(glyphs_data, args.output, args.name, char_max_width, char_max_height, args.fontbitsize, ascii_mode)

    else:
        with open(args.text, 'r', encoding='utf-8') as f:
            text = f.read()

            glyphs_data, char_max_width, char_max_height = generate_glyphs_data(args.input, text, args.pixelsize, 1, args.index)
            write_c_code(glyphs_data, args.output, args.name, char_max_width, char_max_height, 1, ascii_mode)


        with open(args.text, 'r', encoding='utf-8') as f:
            text = f.read()

            glyphs_data, char_max_width, char_max_height = generate_glyphs_data(args.input, text, args.pixelsize, 2, args.index)
            write_c_code(glyphs_data, args.output, args.name, char_max_width, char_max_height, 2, ascii_mode)


        with open(args.text, 'r', encoding='utf-8') as f:
            text = f.read()

            glyphs_data, char_max_width, char_max_height = generate_glyphs_data(args.input, text, args.pixelsize, 4, args.index)
            write_c_code(glyphs_data, args.output, args.name, char_max_width, char_max_height, 4, ascii_mode)


        with open(args.text, 'r', encoding='utf-8') as f:
            text = f.read()

            glyphs_data, char_max_width, char_max_height = generate_glyphs_data(args.input, text, args.pixelsize, 8, args.index)
            write_c_code(glyphs_data, args.output, args.name, char_max_width, char_max_height, 8, ascii_mode)

    with open(args.output, "a") as outputfile:
        print(c_tail_string, file=outputfile)



if __name__ == '__main__':
    main()
