#!/usr/bin/python
# -*- coding: utf-8 -*-

# *************************************************************************************************
#  Arm 2D project
#  @file        img2c.py
#  @brief       image to C-array converter
#
# *************************************************************************************************
#
# * Copyright (C) 2010-2021 ARM Limited or its affiliates. All rights reserved.
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

    parser = argparse.ArgumentParser(description='image to C array converter')

    parser.add_argument('-i', nargs='?', required=True, type = str,  default="", help="Input file (png, bmp, etc..)")
    parser.add_argument('-o', nargs='?', type = str,  default="", help="output C file containing RGB56/RGB888 and alpha values arrays")

    parser.add_argument('--name', nargs='?',type = str, default="", help="A specified array name")
    parser.add_argument('--format', nargs='?',type = str, default="rgb565", help="RGB Format (rgb565, rgb32)")
    parser.add_argument('--dim', nargs=2,type = int, help="Resize the image with the given width and height")
    parser.add_argument('--rot', nargs='?',type = float, default=0.0, help="Rotate the image with the given angle in degrees")

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

    if args.format != 'rgb565' and args.format != 'rgb32':
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


    # palettised
    mode = image.mode
    if mode == 'P':
        image = image.convert('RGBA')
        mode = 'RGBA'
    # greyscale
    if mode == 'L':
        image = image.convert('RGB')
        mode = 'RGB'

    (row, col) = image.size
    data = np.asarray(image)

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
                    if lineWidth % 16 == 15:
                        print("0x%02x," %(alpha) ,file=o)
                    else:
                        print("0x%02x" %(alpha), end =", ",file=o)
                    lineWidth+=1
                cnt+=1
                print('',file=o)
            print('};', file=o)


        # RGB565 channel array
        if args.format == 'rgb565':
            R = (data[...,0]>>3).astype(np.uint16) << 11
            G = (data[...,1]>>2).astype(np.uint16) << 5
            B = (data[...,2]>>3).astype(np.uint16)
            # merge
            RGB = R | G | B

            print('',file=o)
            print('__attribute__((section(\"arm2d.asset.c_bmp%sRGB565\")))' % (arr_name), file=o)
            print('static const uint16_t c_bmp%sRGB565[%d*%d] = {' % (arr_name, row, col), file=o)
            cnt = 0
            for eachRow in RGB:
                lineWidth=0
                print("/* -%d- */" % (cnt), file=o)
                for eachPix in eachRow:
                    if lineWidth % 16 == 15:
                        print("0x%04x," %(eachPix) ,file=o)
                    else:
                        print("0x%04x" %(eachPix), end =", ", file=o)
                    lineWidth+=1
                print('',file=o)
                cnt+=1
            print('};', file=o)
            buffStr='phwBuffer'
            typStr='uint16_t'

        elif args.format == 'rgb32':
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
                print('static const uint32_t c_bmp%sCCCA8888[%d*%d] = {' % (arr_name, row, col), file=o)
            else:
                print('__attribute__((section(\"arm2d.asset.c_bmp%sCCCN888\")))' % (arr_name), file=o)
                print('static const uint32_t c_bmp%sCCCN888[%d*%d]= {' % (arr_name, row, col), file=o)
                
            cnt = 0
            for eachRow in RGB:
                lineWidth=0
                print("/* -%d- */" % (cnt), file=o)
                for eachPix in eachRow:
                    if lineWidth % 16 == 15:
                        print("0x%08x," %(eachPix) ,file=o)
                    else:
                        print("0x%08x" %(eachPix), end =", ", file=o)
                    lineWidth+=1
                print('',file=o)
                cnt+=1
            print('};', file=o)
            buffStr='pwBuffer'
            typStr='uint32_t'
        else:
            print('unknown format')
            exit(1)



        # insert tail
        
        if args.format == 'rgb565':
            print(tailDataRGB565.format(arr_name, str(row), str(col), "."+buffStr+" = ("+typStr+"*)"), file=o)
            if mode == "RGBA":
                print(tailAlpha.format(arr_name, str(row), str(col)), file=o)
        
        if args.format == 'rgb32':
            if mode == "RGBA":
                print(tailDataRGBA8888.format(arr_name, str(row), str(col), "."+buffStr+" = ("+typStr+"*)"), file=o)
                print(tailAlpha.format(arr_name, str(row), str(col)), file=o)
                print(tailAlpha2.format(arr_name, str(row), str(col)), file=o)
            else :
                print(tailDataRGB888.format(arr_name, str(row), str(col), "."+buffStr+" = ("+typStr+"*)"), file=o)
                
            

        print(tail.format(arr_name, str(row), str(col)), file=o)

if __name__ == '__main__':
    main(sys.argv[1:])
