#!/usr/bin/python
# -*- coding: utf-8 -*-

# *************************************************************************************************
#  Arm 2D project
#  @file        img2c.py
#  @brief       image to C-array converter
#
# *************************************************************************************************
#
# * Copyright (C) 2010-2022 ARM Limited or its affiliates. All rights reserved.
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

import sys
from PIL import Image
import numpy as np
import time;
import argparse
import os



hdr="""
/* Generated on {0} from {1} */
/* Re-sized : {2} */
/* Rotated : {3} deg */



#include "arm_2d.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wcast-qual"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress=1296
#endif

"""

tailDataGRAY8="""

extern const arm_2d_tile_t c_tile{0}GRAY8;
__attribute__((section(\"arm2d.tile.c_tile{0}GRAY8\")))
const arm_2d_tile_t c_tile{0}GRAY8 = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {2},
        }},
    }},
    .tInfo = {{
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {{
            .chScheme = ARM_2D_COLOUR_GRAY8,
        }},
    }},
    {3}c_bmp{0}GRAY8,
}};

"""

tailDataRGB565="""

extern const arm_2d_tile_t c_tile{0}RGB565;
__attribute__((section(\"arm2d.tile.c_tile{0}RGB565\")))
const arm_2d_tile_t c_tile{0}RGB565 = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {2},
        }},
    }},
    .tInfo = {{
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {{
            .chScheme = ARM_2D_COLOUR_RGB565,
        }},
    }},
    {3}c_bmp{0}RGB565,
}};

"""

tailDataRGB888="""

extern const arm_2d_tile_t c_tile{0}CCCN888;

__attribute__((section(\"arm2d.tile.c_tile{0}CCCN888\")))
const arm_2d_tile_t c_tile{0}CCCN888 = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {2},
        }},
    }},
    .tInfo = {{
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {{
            .chScheme = ARM_2D_COLOUR_RGB888,
        }},
    }},
    {3}c_bmp{0}CCCN888,
}};

"""

tailDataRGBA8888="""

extern const arm_2d_tile_t c_tile{0}CCCA8888;

__attribute__((section(\"arm2d.tile.c_tile{0}CCCA8888\")))
const arm_2d_tile_t c_tile{0}CCCA8888 = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {2},
        }},
    }},
    .tInfo = {{
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {{
            .chScheme = ARM_2D_COLOUR_BGRA8888,
        }},
    }},
    {3}c_bmp{0}CCCA8888,
}};

"""

tailAlpha="""


extern const arm_2d_tile_t c_tile{0}Mask;

__attribute__((section(\"arm2d.tile.c_tile{0}Mask\")))
const arm_2d_tile_t c_tile{0}Mask = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {2},
        }},
    }},
    .tInfo = {{
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {{
            .chScheme = ARM_2D_COLOUR_8BIT,
        }},
    }},
    .pchBuffer = (uint8_t *)c_bmp{0}Alpha,
}};
"""

tail2BitAlpha="""


extern const arm_2d_tile_t c_tile{0}A2Mask;

__attribute__((section(\"arm2d.tile.c_tile{0}A2Mask\")))
const arm_2d_tile_t c_tile{0}A2Mask = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {2},
        }},
    }},
    .tInfo = {{
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {{
            .chScheme = ARM_2D_COLOUR_MASK_A2,
        }},
    }},
    .pchBuffer = (uint8_t *)c_bmp{0}A2Alpha,
}};
"""

tail4BitAlpha="""


extern const arm_2d_tile_t c_tile{0}A4Mask;

__attribute__((section(\"arm2d.tile.c_tile{0}A4Mask\")))
const arm_2d_tile_t c_tile{0}A4Mask = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {2},
        }},
    }},
    .tInfo = {{
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {{
            .chScheme = ARM_2D_COLOUR_MASK_A4,
        }},
    }},
    .pchBuffer = (uint8_t *)c_bmp{0}A4Alpha,
}};
"""


tailAlpha2="""


extern const arm_2d_tile_t c_tile{0}Mask2;

__attribute__((section(\"arm2d.tile.c_tile{0}Mask2\")))
const arm_2d_tile_t c_tile{0}Mask2 = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {2},
        }},
    }},
    .tInfo = {{
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {{
            .chScheme = ARM_2D_CHANNEL_8in32,
        }},
    }},
    .nAddress = ((intptr_t)c_bmp{0}CCCA8888) + 3,
}};
"""


tail="""

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_warning=1296
#endif

"""

def main(argv):

    parser = argparse.ArgumentParser(description='image to C array converter (v1.2.0)')

    parser.add_argument('-i', nargs='?', type = str,  required=True, help="Input file (png, bmp, etc..)")
    parser.add_argument('-o', nargs='?', type = str,  required=False, help="output C file containing RGB56/RGB888/Gray8 and alpha values arrays")

    parser.add_argument('--name', nargs='?',type = str, required=True, help="A specified array name")
    parser.add_argument('--format', nargs='?',type = str, default="all", help="RGB Format (rgb565, rgb32, gray8, all)")
    parser.add_argument('--dim', nargs=2,type = int, help="Resize the image with the given width and height")
    parser.add_argument('--rot', nargs='?',type = float, default=0.0, help="Rotate the image with the given angle in degrees")
    parser.add_argument('--a2', action='store_true', help="Generate 2bit alpha-mask")
    parser.add_argument('--a4', action='store_true', help="Generate 4bit alpha-mask")

    args = parser.parse_args()

    if args.i == None or args.i == "" :
        parser.print_help()
        exit(1)
    inputfile = args.i;
    basename = os.path.basename(inputfile).split('.')[0];


    outputfile = args.o
    if outputfile == None or outputfile == "":
        outputfile = basename + ".c"

    arr_name = args.name
    if arr_name == None or arr_name == "":
        arr_name = basename

    if args.format != 'rgb565' and \
        args.format != 'rgb32' and \
        args.format != 'gray8' and \
        args.format != 'all':
        parser.print_help()
        exit(1)

    try:
        image = Image.open(inputfile)
    except FileNotFoundError:
        print("Cannot open image file %s" % (inputfile))
        sys.exit(2)


    # rotation
    if args.rot != 0.0:
        image = image.rotate(args.rot)


    # resizing
    resized = False
    if args.dim != None:
        image = image.resize((args.dim[0], args.dim[1]))
        resized = True


    mode = image.mode

    # Modes supported by Pillow

    # 1 (1-bit pixels, black and white, stored with one pixel per byte), the value is in 0-1.
    # L (8-bit pixels, black and white), the value is in 0-255.
    # P (8-bit pixels, mapped to any other mode using a color palette), the value is in 0-255.
    # RGB (3×8-bit pixels, true color), the value is in [0-255, 0-255, 0-255].
    # RGBA (4×8-bit pixels, true color with transparency mask), the value is in [0-255, 0-255, 0-255, 0-255]
    # CMYK (4×8-bit pixels, color separation)
    # YCbCr (3×8-bit pixels, color video format)
    # LAB (3×8-bit pixels, the L*a*b color space)
    # HSV (3×8-bit pixels, Hue, Saturation, Value color space)
    # I (32-bit signed integer pixels)
    # F (32-bit floating point pixels)
    # LA (L with alpha)
    # PA (P with alpha)
    # RGBX (true color with padding)
    # RGBa (true color with premultiplied alpha)
    # La (L with premultiplied alpha)
    # I;16 (16-bit unsigned integer pixels)
    # I;16L (16-bit little endian unsigned integer pixels)
    # I;16B (16-bit big endian unsigned integer pixels)
    # I;16N (16-bit native endian unsigned integer pixels)
    # BGR;15 (15-bit reversed true colour)
    # BGR;16 (16-bit reversed true colour)
    # BGR;24 (24-bit reversed true colour)
    # BGR;32 (32-bit reversed true colour)

    # handle {P, LA, RGB, RGBA} for now
    if mode == 'P' or mode == 'LA':
        image = image.convert('RGBA')
        mode = 'RGBA'
    if mode == 'L':
        image = image.convert('RGB')
        mode = 'RGB'
    (row, col) = image.size
    data = np.asarray(image)

    # C Array format width
    WIDTH_ALPHA = 16
    WIDTH_GRAY8 = 32
    WIDTH_RGB565 = 16
    WIDTH_RGB32 = 16

    with open(outputfile,"w") as o:

        # insert header
        print(hdr.format(time.asctime( time.localtime(time.time())), argv[0], resized, args.rot), file=o)

        if mode == "RGBA":
            print('__attribute__((aligned(4), section(\"arm2d.asset.c_bmp%sAlpha\")))' % (arr_name), file=o)
            # alpha channel array available
            print('static const uint8_t c_bmp%sAlpha[%d*%d] = {' % (arr_name, row, col),file=o)
            cnt = 0
            for eachRow in data:
                lineWidth=0
                print("/* -%d- */" % (cnt), file=o)
                for eachPix in eachRow:
                    alpha = eachPix[3]
                    if lineWidth % WIDTH_ALPHA == (WIDTH_ALPHA - 1):
                        print("0x%02x," %(alpha) ,file=o)
                    else:
                        print("0x%02x" %(alpha), end =", ",file=o)
                    lineWidth+=1
                cnt+=1
                print('',file=o)
            print('};', file=o)

            # 2-bit Alpha channel
            if args.a2 or args.format == 'all':

                def RevBitPairPerByte(byteArr):
                    return ((byteArr & 0x03) << 6) |  ((byteArr & 0xc0) >> 6) | ((byteArr & 0x30) >> 2 ) | ((byteArr & 0x0c) << 2)


                print('__attribute__((aligned(4), section(\"arm2d.asset.c_bmp%sA2Alpha\")))' % (arr_name), file=o)
                print('static const uint8_t c_bmp%sA2Alpha[%d*%d] = {' % (arr_name, (row+3)/4, col),file=o)
                cnt = 0
                alpha = data[...,3].astype(np.uint8)
                for eachRow in alpha:
                    lineWidth=0
                    print("/* -%d- */" % (cnt), file=o)

                    bitsArr = np.unpackbits(eachRow.astype(np.uint8))

                    # generate indexes for MSB bit pair every byte
                    idx = np.arange(0, np.size(bitsArr), 8)
                    idx=np.reshape(np.column_stack((idx+0, idx+1)), (1,-1))

                    # extraction + endianness conversion
                    packedBytes = RevBitPairPerByte(np.packbits(bitsArr[idx]))

                    for elt in packedBytes:
                        if lineWidth % WIDTH_ALPHA == (WIDTH_ALPHA-1):
                            print("0x%02x," %(elt) ,file=o)
                        else:
                            print("0x%02x" %(elt), end =", ",file=o)
                        lineWidth+=1
                    cnt+=1
                    print('',file=o)
                print('};', file=o)

            # 4-bit Alpha channel
            if args.a4 or args.format == 'all':

                def RevBitQuadPerByte(byteArr):
                    return ((byteArr & 0x0f) << 4) |  ((byteArr & 0xf0) >> 4)


                print('__attribute__((aligned(4), section(\"arm2d.asset.c_bmp%sA4Alpha\")))' % (arr_name), file=o)
                print('static const uint8_t c_bmp%sA4Alpha[%d*%d] = {' % (arr_name, (row+1)/2, col),file=o)
                cnt = 0
                alpha = data[...,3].astype(np.uint8)
                for eachRow in alpha:
                    lineWidth=0
                    print("/* -%d- */" % (cnt), file=o)

                    bitsArr = np.unpackbits(eachRow.astype(np.uint8))

                    # generate indexes for MSB bit quadruplet every byte
                    idx = np.arange(0, np.size(bitsArr), 8)
                    idx=np.reshape(np.column_stack(
                            (np.column_stack((idx+0, idx+1)), np.column_stack((idx+2, idx+3)))),
                            (1,-1)),

                    # extraction + endianness conversion
                    packedBytes = RevBitQuadPerByte(np.packbits(bitsArr[idx]))

                    for elt in packedBytes:
                        if lineWidth % WIDTH_ALPHA == (WIDTH_ALPHA - 1):
                            print("0x%02x," %(elt) ,file=o)
                        else:
                            print("0x%02x" %(elt), end =", ",file=o)
                        lineWidth+=1
                    cnt+=1
                    print('',file=o)
                print('};', file=o)


        # Gray8 channel array
        if args.format == 'gray8' or args.format == 'all':

            R = (data[...,0]).astype(np.uint16)
            G = (data[...,1]).astype(np.uint16)
            B = (data[...,2]).astype(np.uint16)
            # merge
            RGB = np.rint((R + G + B)/3).astype(np.uint8)

            print('',file=o)
            print('__attribute__((section(\"arm2d.asset.c_bmp%sGRAY8\")))' % (arr_name), file=o)
            print('const uint8_t c_bmp%sGRAY8[%d*%d] = {' % (arr_name, row, col), file=o)
            cnt = 0
            for eachRow in RGB:
                lineWidth=0
                print("/* -%d- */" % (cnt), file=o)
                for eachPix in eachRow:
                    if lineWidth % WIDTH_GRAY8 == (WIDTH_GRAY8 - 1):
                        print("0x%02x," %(eachPix) ,file=o)
                    else:
                        print("0x%02x" %(eachPix), end =", ", file=o)
                    lineWidth+=1
                print('',file=o)
                cnt+=1
            print('};', file=o)
            buffStr='pchBuffer'
            typStr='uint8_t'

        # RGB565 channel array
        if args.format == 'rgb565' or args.format == 'all':
            R = (data[...,0]>>3).astype(np.uint16) << 11
            G = (data[...,1]>>2).astype(np.uint16) << 5
            B = (data[...,2]>>3).astype(np.uint16)
            # merge
            RGB = R | G | B

            print('',file=o)
            print('__attribute__((section(\"arm2d.asset.c_bmp%sRGB565\")))' % (arr_name), file=o)
            print('const uint16_t c_bmp%sRGB565[%d*%d] = {' % (arr_name, row, col), file=o)
            cnt = 0
            for eachRow in RGB:
                lineWidth=0
                print("/* -%d- */" % (cnt), file=o)
                for eachPix in eachRow:
                    if lineWidth % WIDTH_RGB565 == (WIDTH_RGB565 - 1):
                        print("0x%04x," %(eachPix) ,file=o)
                    else:
                        print("0x%04x" %(eachPix), end =", ", file=o)
                    lineWidth+=1
                print('',file=o)
                cnt+=1
            print('};', file=o)
            buffStr='phwBuffer'
            typStr='uint16_t'



        if args.format == 'rgb32' or args.format == 'all':
            R = data[...,0].astype(np.uint32) << 16
            G = data[...,1].astype(np.uint32) << 8
            B = data[...,2].astype(np.uint32)
            if mode == "RGBA":
                A = data[...,3].astype(np.uint32) << 24
            else:
                # alpha chanel forced to 0xFF
                A = 0xff << 24
            # merge
            RGB = R | G | B | A

            print('',file=o)

            if mode == "RGBA":
                print('__attribute__((section(\"arm2d.asset.c_bmp%sCCCA8888\")))' % (arr_name), file=o)
                print('const uint32_t c_bmp%sCCCA8888[%d*%d] = {' % (arr_name, row, col), file=o)
            else:
                print('__attribute__((section(\"arm2d.asset.c_bmp%sCCCN888\")))' % (arr_name), file=o)
                print('const uint32_t c_bmp%sCCCN888[%d*%d]= {' % (arr_name, row, col), file=o)

            cnt = 0
            for eachRow in RGB:
                lineWidth=0
                print("/* -%d- */" % (cnt), file=o)
                for eachPix in eachRow:
                    if lineWidth % WIDTH_RGB32 == (WIDTH_RGB32 - 1):
                        print("0x%08x," %(eachPix) ,file=o)
                    else:
                        print("0x%08x" %(eachPix), end =", ", file=o)
                    lineWidth+=1
                print('',file=o)
                cnt+=1
            print('};', file=o)
            buffStr='pwBuffer'
            typStr='uint32_t'

        # insert tail
        if args.format == 'gray8' or args.format == 'all':
            buffStr='pchBuffer'
            typStr='uint8_t'
            print(tailDataGRAY8.format(arr_name, str(row), str(col), "."+buffStr+" = ("+typStr+"*)"), file=o)

        if args.format == 'rgb565' or args.format == 'all':
            buffStr='phwBuffer'
            typStr='uint16_t'
            print(tailDataRGB565.format(arr_name, str(row), str(col), "."+buffStr+" = ("+typStr+"*)"), file=o)

        if args.format == 'rgb32' or args.format == 'all':
            buffStr='pwBuffer'
            typStr='uint32_t'
            if mode == "RGBA":
                print(tailDataRGBA8888.format(arr_name, str(row), str(col), "."+buffStr+" = ("+typStr+"*)"), file=o)
                print(tailAlpha2.format(arr_name, str(row), str(col)), file=o)
            else :
                print(tailDataRGB888.format(arr_name, str(row), str(col), "."+buffStr+" = ("+typStr+"*)"), file=o)


        if mode == "RGBA":
            print(tailAlpha.format(arr_name, str(row), str(col)), file=o)

            if args.a2 or args.format == 'all':
                print(tail2BitAlpha.format(arr_name, str(row), str(col)), file=o)

            if args.a4 or args.format == 'all':
                print(tail4BitAlpha.format(arr_name, str(row), str(col)), file=o)


        print(tail.format(arr_name, str(row), str(col)), file=o)

if __name__ == '__main__':
    main(sys.argv[1:])
