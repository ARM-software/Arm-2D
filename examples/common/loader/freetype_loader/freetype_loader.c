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

ARM_NONNULL(1,2)
static
arm_2d_err_t __arm_freetype_loader_allocate_memory_for_a8_mask( 
                                                arm_freetype_loader_t *ptThis,
                                                arm_2d_size_t *ptNewSize)
{
    assert(NULL != ptThis);
    assert(NULL != ptNewSize);

    if (    (ptNewSize->iWidth <= this.A8Mask.tSize.iWidth)
      &&    (ptNewSize->iHeight <= this.A8Mask.tSize.iHeight)) {
        return ARM_2D_ERR_NONE;
    }

    if (NULL != this.A8Mask.pchBuffer) {
        __arm_2d_free_scratch_memory(   ARM_2D_MEM_TYPE_FAST, 
                                        this.A8Mask.pchBuffer);

        this.A8Mask.pchBuffer = NULL;
    }
    this.A8Mask.pchBuffer = NULL;

    memset(&this.A8Mask.tSize, 0, sizeof(this.A8Mask.tSize));

    size_t tNewSize = (ptNewSize->iHeight + 2) 
                    * (ptNewSize->iWidth + 2);
    
    this.A8Mask.pchBuffer = __arm_2d_allocate_scratch_memory(   
                                                        tNewSize,
                                                        1,
                                                        ARM_2D_MEM_TYPE_FAST);
    if (NULL == this.A8Mask.pchBuffer) {
        return ARM_2D_ERR_INSUFFICIENT_RESOURCE;
    }

    this.A8Mask.tSize = *ptNewSize;

    return ARM_2D_ERR_NONE;
}

static
IMPL_FONT_GET_CHAR_DESCRIPTOR(__freetype_loader_get_char_descriptor)
{
    assert(NULL != ptFont);
    assert(NULL != ptDescriptor);
    assert(NULL != pchCharCode);
        
    arm_freetype_loader_t *ptThis = (arm_freetype_loader_t *)ptFont;
    memset(ptDescriptor, 0, sizeof(arm_2d_char_descriptor_t));

    do {
        if (!this.bValid) {
            break;
        }

        uint32_t wUnicode = arm_2d_helper_utf8_to_unicode(pchCharCode);
        FT_UInt tGlyph = FT_Get_Char_Index(this.tFace, wUnicode);
        if (0 == tGlyph) {
            /* encounter unsupported char, use white space to replace  */
            tGlyph = FT_Get_Char_Index(this.tFace, ' ');
            assert(0 != tGlyph);
        }

        if (FT_Err_Ok != FT_Load_Glyph( this.tFace, 
                                        tGlyph, 
                                        FT_LOAD_DEFAULT         | 
                                        FT_LOAD_FORCE_AUTOHINT  | 
                                        FT_LOAD_TARGET_LIGHT    )) {
            break;
        }

        if (FT_Err_Ok != FT_Render_Glyph(   this.tFace->glyph, 
                                            FT_RENDER_MODE_NORMAL)) {
            break;
        }

        FT_Bitmap * ptBitmap = &this.tFace->glyph->bitmap;
        assert(ptBitmap->pixel_mode == FT_PIXEL_MODE_GRAY);
        assert(ptBitmap->num_grays == 256);

        this.A8Mask.tGlyph.tRegion.tSize.iHeight = ptBitmap->rows;
        this.A8Mask.tGlyph.tRegion.tSize.iWidth = ptBitmap->width;
        this.A8Mask.tGlyph.pchBuffer = ptBitmap->buffer;

        arm_2d_err_t tResult = 
            __arm_freetype_loader_allocate_memory_for_a8_mask(  
                                                ptThis,
                                                &this.A8Mask.tGlyph.tRegion.tSize);
        if (tResult < 0) {
            this.s5ErrCode = tResult;
            break;
        }

        int16_t iA8Width = this.A8Mask.tGlyph.tRegion.tSize.iWidth + 2;
        int16_t iA8Height = this.A8Mask.tGlyph.tRegion.tSize.iHeight + 2;
        
        ptDescriptor->tileChar.tInfo = this.A8Mask.tGlyph.tInfo;
        ptDescriptor->tileChar.tRegion.tSize.iWidth = iA8Width;
        ptDescriptor->tileChar.tRegion.tSize.iHeight = iA8Height;
        ptDescriptor->tileChar.pchBuffer = this.A8Mask.pchBuffer;

        arm_2d_region_t tTargetRegion = {
            .tLocation = {1,1},
            .tSize = this.A8Mask.tGlyph.tRegion.tSize,
        };

        memset(this.A8Mask.pchBuffer, 0, iA8Width * iA8Height );
        arm_2d_sw_normal_root_tile_copy(&this.A8Mask.tGlyph,
                                        &ptDescriptor->tileChar,
                                        &tTargetRegion, 
                                        1);
        
        ptDescriptor->iBearingX = this.tFace->glyph->bitmap_left + 1;
        ptDescriptor->iBearingY = this.tFace->glyph->bitmap_top + 1;
        ptDescriptor->iAdvance = (this.tFace->glyph->advance.x >> 6) + 1;

        //ptDescriptor->chCodeLength = arm_2d_helper_get_utf8_byte_valid_length((uint8_t *)pchCharCode);

    } while(0);


    return ptDescriptor;
}

ARM_NONNULL(1,2)
arm_2d_font_t *arm_freetype_loader_file_io_init(arm_freetype_loader_t *ptThis,
                                                const char *pchPath,
                                                int16_t iIndex,
                                                int16_t iFontPixelSize)
{
    assert(NULL != pchPath);
    
    do {
        if (NULL == ptThis) {
            assert(NULL != ptThis);
            break;
        }

        memset(ptThis, 0, sizeof(arm_freetype_loader_t));

        if (iFontPixelSize <= 0) {
            iFontPixelSize = 32;
        }
        this.iFontPixelSize = iFontPixelSize;

        this.A8Mask.tGlyph = (arm_2d_tile_t) {
            .tInfo = {
                .bIsRoot = true,
                .bHasEnforcedColour = true,
                .tColourInfo = {
                    .chScheme = ARM_2D_COLOUR_8BIT,
                },
            },
        };

        this.s5ErrCode = 
            __arm_freetype_loader_allocate_memory_for_a8_mask(
                                            ptThis, 
                                            (arm_2d_size_t []){
                                            {iFontPixelSize, iFontPixelSize}});
        if (this.s5ErrCode < 0) {
            break;
        }

        FT_Init_FreeType(&this.tLibrary);

        if (FT_Err_Ok != FT_New_Face(   this.tLibrary, 
                                        pchPath, 
                                        iIndex, 
                                        &this.tFace)) {
            break;
        }

        if (FT_Err_Ok != FT_Select_Charmap(this.tFace, FT_ENCODING_UNICODE)) {
            break;
        }

        FT_Set_Pixel_Sizes(this.tFace, 0, this.iFontPixelSize);

        /* initialize font */
        arm_using(arm_2d_font_t *ptFont = &this.use_as__arm_2d_font_t) {
            ptFont->tCharSize   = this.A8Mask.tSize;
            ptFont->nCount      = -1;
            ptFont->fnDrawChar  = &__arm_2d_lcd_text_default_a8_font_draw_char;
            ptFont->fnGetCharDescriptor 
                                = &__freetype_loader_get_char_descriptor;
            ptFont->tileFont.pchBuffer = this.A8Mask.pchBuffer;
            ptFont->tileFont.tInfo.bIsRoot = true;
        };

        

        this.bValid = true;
        
        return &this.use_as__arm_2d_font_t;
    } while(0);

    if (NULL != ptThis) {
        FT_Done_Face(this.tFace);
        FT_Done_FreeType(this.tLibrary);
    }

    return NULL;
}

void arm_freetype_loader_depose(arm_freetype_loader_t *ptThis)
{
    if (NULL == ptThis) {
        return ;
    }

    FT_Done_Face(this.tFace);
    FT_Done_FreeType(this.tLibrary);

    __arm_2d_free_scratch_memory(   ARM_2D_MEM_TYPE_FAST, 
                                    this.A8Mask.pchBuffer);
    
    memset(ptThis, 0, sizeof(arm_freetype_loader_t));
}

ARM_NONNULL(1)
arm_2d_err_t arm_freetype_get_error(arm_freetype_loader_t *ptThis)
{
    if (NULL == ptThis) {
        assert(false);
        return ARM_2D_ERR_INVALID_PARAM;
    }

    return this.s5ErrCode;
}

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
