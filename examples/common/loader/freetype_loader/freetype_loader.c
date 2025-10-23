/*
 * Copyright (c) 2009-2025 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*============================ INCLUDES ======================================*/
#define __FREETYPE_LOADER_IMPLEMENT__

#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "freetype_loader.h"
#include <assert.h>
#include <string.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif

/*============================ MACROS ========================================*/

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


void __arm_2d_freetype_test(arm_2d_tile_t *ptTile, 
                            arm_2d_region_t *ptRegion, 
                            const char *pchFontFilePath, 
                            const uint8_t *pchUTF8)
{
    FT_Library lib; FT_Face face;
    FT_Init_FreeType(&lib);

    //run_os_command("ls .");
    FT_New_Face(lib, pchFontFilePath, 0, &face);

    FT_Select_Charmap(face, FT_ENCODING_UNICODE);

    FT_Set_Pixel_Sizes(face, 0, 32);

    uint32_t cp = arm_2d_helper_utf8_to_unicode(pchUTF8);
    FT_UInt glyph = FT_Get_Char_Index(face, cp);

    FT_Int32 flags = FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT;

    FT_Load_Glyph(face, glyph, flags);
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

    FT_Bitmap* bm = &face->glyph->bitmap;
    if (bm->pixel_mode != FT_PIXEL_MODE_GRAY || bm->num_grays != 256) {
        assert(false);
    }

    arm_2d_tile_t tGlyphMask = {
        .tRegion = {
            .tSize = {
                .iWidth = bm->width,
                .iHeight = bm->rows,
            },
        },
        .tInfo = {
            .bIsRoot = true,
            .bHasEnforcedColour = true,
            .tColourInfo = {
                .chScheme = ARM_2D_COLOUR_8BIT,
            },
        },
        .pchBuffer = (uint8_t *)bm->buffer,
    };

    arm_2d_fill_colour_with_mask(   ptTile, 
                                    ptRegion, 
                                    &tGlyphMask, 
                                    (__arm_2d_color_t){GLCD_COLOR_BLACK});

    FT_Done_Face(face);
    FT_Done_FreeType(lib);


}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
