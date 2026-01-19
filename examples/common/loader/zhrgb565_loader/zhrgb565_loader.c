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
#define __GENERIC_LOADER_INHERIT__
#define __ZHRGB565_LOADER_IMPLEMENT__
#include "zhrgb565_loader.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB) && defined(RTE_Acceleration_Arm_2D_Extra_Loader)

#include <assert.h>
#include <string.h>

#include "./zhrgb565/zhRGB565_Decoder/zhRGB565_Decoder.h"
#include "arm_zhrgb565_cfg.h"

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
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
ARM_NONNULL(1)
static
arm_2d_err_t __zhrgb565_loader_decoder_init(arm_generic_loader_t *ptObj);

ARM_NONNULL(1, 2, 3)
static
arm_2d_err_t __zhrgb565_loader_draw(  arm_generic_loader_t *ptObj,
                                    arm_2d_region_t *ptROI,
                                    uint8_t *pchBuffer,
                                    uint32_t iTargetStrideInByte,
                                    uint_fast8_t chBitsPerPixel);

extern
ARM_NONNULL(1)
arm_2d_size_t zhRGB565_get_image_size(arm_generic_loader_t *ptLoader);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t zhrgb565_loader_init(zhrgb565_loader_t *ptThis,
                                zhrgb565_loader_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    memset(ptThis, 0, sizeof(zhrgb565_loader_t));

    arm_2d_err_t tResult = ARM_2D_ERR_NONE;

    do {
        if (NULL == ptCFG->ImageIO.ptIO) {
            this.use_as__arm_generic_loader_t.bErrorDetected = true;
            tResult = ARM_2D_ERR_IO_ERROR;
            break;
        }

        arm_generic_loader_cfg_t tCFG = {
            .bUseHeapForVRES = ptCFG->bUseHeapForVRES,
            .tColourInfo.chScheme = ARM_2D_COLOUR,
            .bBlendWithBG = false,
            .ImageIO = {
                .ptIO = ptCFG->ImageIO.ptIO,
                .pTarget = ptCFG->ImageIO.pTarget,
            },

            .UserDecoder = {
                .fnDecoderInit = &__zhrgb565_loader_decoder_init,
                .fnDecode = &__zhrgb565_loader_draw,
            },

            .ptScene = ptCFG->ptScene,
        };

        tResult = arm_generic_loader_init(  &this.use_as__arm_generic_loader_t,
                                            &tCFG);

        if (tResult < 0) {
            break;
        }

        //this.tTile.tRegion.tSize = zhRGB565_get_image_size(&this.use_as__arm_generic_loader_t);
        if ((0 == this.tTile.tRegion.tSize.iWidth)
         || (0 == this.tTile.tRegion.tSize.iHeight)) {
            tResult = ARM_2D_ERR_INVALID_PARAM;
            break;
        }

    } while(0);

    return tResult;

}

ARM_NONNULL(1)
void zhrgb565_loader_depose( zhrgb565_loader_t *ptThis)
{
    assert(NULL != ptThis);

    arm_generic_loader_depose(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void zhrgb565_loader_on_load( zhrgb565_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    arm_generic_loader_on_load(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void zhrgb565_loader_on_frame_start( zhrgb565_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    arm_generic_loader_on_frame_start(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void zhrgb565_loader_on_frame_complete( zhrgb565_loader_t *ptThis)
{
    assert(NULL != ptThis);

    arm_generic_loader_on_frame_complete(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
static
arm_2d_err_t __zhrgb565_loader_decoder_init(arm_generic_loader_t *ptObj)
{
    assert(NULL != ptObj);

    zhrgb565_loader_t *ptThis = (zhrgb565_loader_t *)ptObj;
    ARM_2D_UNUSED(ptThis);

    if (!this.use_as__arm_generic_loader_t.bInitialized) {
        this.tTile.tRegion.tSize = zhRGB565_get_image_size(ptObj);
    }

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1, 2, 3)
static
arm_2d_err_t __zhrgb565_loader_draw(arm_generic_loader_t *ptObj,
                                    arm_2d_region_t *ptROI,
                                    uint8_t *pchBuffer,
                                    uint32_t iTargetStrideInByte,
                                    uint_fast8_t chBitsPerPixel)
{
    assert(NULL != ptObj);
    zhrgb565_loader_t *ptThis = (zhrgb565_loader_t *)ptObj;
    ARM_2D_UNUSED(ptThis);

    int_fast16_t iXLimit = ptROI->tSize.iWidth + ptROI->tLocation.iX; 
    int_fast16_t iYLimit = ptROI->tSize.iHeight + ptROI->tLocation.iY; 

    uint_fast8_t chBytesPerPixel = chBitsPerPixel >> 3;
    assert(0 == (chBitsPerPixel & 0x07));
    assert(chBytesPerPixel != 3);

    int16_t iTargetStride = iTargetStrideInByte / chBytesPerPixel;
    

    zhRGB565_decompress_for_arm2d(  ptROI->tLocation.iX,
                                    ptROI->tLocation.iY,
                                    ptROI->tSize.iWidth,
                                    ptROI->tSize.iHeight,
                                #if __ARM_2D_ZHRGB565_USE_LOADER_IO__
                                    (const uint16_t *)ptObj,
                                #else
                                    (const uint16_t *)arm_generic_loader_io_get_position(ptObj),
                                #endif
                                    (uint16_t *)pchBuffer,
                                    iTargetStride);

    return ARM_2D_ERR_NONE;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif
