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

#ifndef __FREETYPE_LOADER_H__
#define __FREETYPE_LOADER_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d_helper.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

/*============================ MACROS ========================================*/

/* OOC header, please DO NOT modify  */
#ifdef __FREETYPE_LOADER_IMPLEMENT__
#   undef   __FREETYPE_LOADER_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__FREETYPE_LOADER_INHERIT__)
#   undef   __FREETYPE_LOADER_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct arm_freetype_loader_t {
    implement(arm_2d_font_t);

//ARM_PRIVATE(
    uint16_t bFontSourceStoredInMemory  : 1;
    uint16_t bValid                     : 1;
    uint16_t                            : 1;
    int16_t  s5ErrCode                  : 5;
    uint16_t                            : 8;
    int16_t  iFontPixelSize;

    union {
        struct {
            const char *pchFontPath;
        } FileIO;

        struct {
            void * pBuffer;
            size_t tSize;
        } Memory;
    }FontSource;

    FT_Library  tLibrary; 
    FT_Face     tFace;

    struct {
        uint8_t *pchBuffer;
        arm_2d_size_t tSize;
        arm_2d_tile_t tGlyph;
    } A8Mask;
    
//)

} arm_freetype_loader_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1,2)
arm_2d_font_t *arm_freetype_loader_file_io_init(arm_freetype_loader_t *ptThis,
                                                const char *pchPath,
                                                int16_t iIndex,
                                                int16_t iFontPixelSize);

extern
void arm_freetype_loader_depose(arm_freetype_loader_t *ptThis);

extern
ARM_NONNULL(1)
arm_2d_err_t arm_freetype_get_error(arm_freetype_loader_t *ptThis);


extern
void __arm_2d_freetype_test(arm_2d_tile_t *ptTile, 
                            arm_2d_region_t *ptRegion, 
                            const char *pchFontFilePath, 
                            const uint8_t *pchUTF8);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
