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
#define __LMSK_LOADER_IMPLEMENT__
#include "lmsk_loader.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB) && defined(RTE_Acceleration_Arm_2D_Extra_Loader)

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
arm_2d_err_t __lmsk_loader_decoder_init(arm_generic_loader_t *ptObj);

ARM_NONNULL(1, 2, 3)
static
arm_2d_err_t __lmsk_loader_draw(  arm_generic_loader_t *ptObj,
                                    arm_2d_region_t *ptROI,
                                    uint8_t *pchBuffer,
                                    uint32_t iTargetStrideInByte,
                                    uint_fast8_t chBitsPerPixel);

static
bool __lmsk_loader_seek(uintptr_t pTarget, int32_t offset);

static
size_t __lmsk_loader_read ( intptr_t pTarget,      
                            uint8_t *pchBuffer,
                            size_t tLength);
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t lmsk_loader_init(lmsk_loader_t *ptThis,
                                lmsk_loader_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    memset(ptThis, 0, sizeof(lmsk_loader_t));


    arm_2d_err_t tResult = ARM_2D_ERR_NONE;

    do {
    #if __ARM_LMSK_USE_LOADER_IO__
        if (NULL == ptCFG->ImageIO.ptIO) {
            this.use_as__arm_generic_loader_t.bErrorDetected = true;
            tResult = ARM_2D_ERR_IO_ERROR;
            break;
        }
    #else
        if (NULL == ptCFG->pchLMSKSource) {
            this.use_as__arm_generic_loader_t.bErrorDetected = true;
            tResult = ARM_2D_ERR_IO_ERROR;
            break;
        }
    #endif

        arm_generic_loader_cfg_t tCFG = {
            .bUseHeapForVRES = ptCFG->bUseHeapForVRES,
            .tColourInfo.chScheme = ARM_2D_COLOUR_MASK_A8,
        
        #if __ARM_LMSK_USE_LOADER_IO__
            .ImageIO = {
                .ptIO = ptCFG->ImageIO.ptIO,
                .pTarget = ptCFG->ImageIO.pTarget,
            },
        #endif

            .UserDecoder = {
                .fnDecoderInit = &__lmsk_loader_decoder_init,
                .fnDecode = &__lmsk_loader_draw,
            },

            .ptScene = ptCFG->ptScene,
        };

        tResult = arm_generic_loader_init(  &this.use_as__arm_generic_loader_t,
                                            &tCFG);

        if (tResult < 0) {
            break;
        }

        if ((0 == this.tTile.tRegion.tSize.iWidth)
         || (0 == this.tTile.tRegion.tSize.iHeight)) {
            tResult = ARM_2D_ERR_INVALID_PARAM;
            break;
        }

    } while(0);

    return tResult;

}

ARM_NONNULL(1)
void lmsk_loader_depose( lmsk_loader_t *ptThis)
{
    assert(NULL != ptThis);

    arm_generic_loader_depose(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void lmsk_loader_on_load( lmsk_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    arm_generic_loader_on_load(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void lmsk_loader_on_frame_start( lmsk_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    arm_generic_loader_on_frame_start(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void lmsk_loader_on_frame_complete( lmsk_loader_t *ptThis)
{
    assert(NULL != ptThis);

    arm_generic_loader_on_frame_complete(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
static
arm_2d_err_t __lmsk_loader_decoder_init(arm_generic_loader_t *ptObj)
{
    assert(NULL != ptObj);

    lmsk_loader_t *ptThis = (lmsk_loader_t *)ptObj;

    arm_lmsk_decoder_cfg_t tCFG = {
        .IO = {
            .fnSeek = &__lmsk_loader_seek,
            .fnRead = &__lmsk_loader_read,
            .pTarget = (uintptr_t)ptObj,
        },
    };

    if (0 != arm_lmsk_decoder_init(&this.tDecoder, &tCFG)) {
        return ARM_2D_ERR_IO_ERROR;
    }

    this.tTile.tRegion.tSize.iWidth = this.tDecoder.tSetting.iWidth;
    this.tTile.tRegion.tSize.iHeight = this.tDecoder.tSetting.iHeight;

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1, 2, 3)
static
arm_2d_err_t __lmsk_loader_draw(arm_generic_loader_t *ptObj,
                                arm_2d_region_t *ptROI,
                                uint8_t *pchBuffer,
                                uint32_t wTargetStrideInByte,
                                uint_fast8_t chBitsPerPixel)
{
    assert(NULL != ptObj);
    lmsk_loader_t *ptThis = (lmsk_loader_t *)ptObj;
    ARM_2D_UNUSED(ptThis);

    int_fast16_t iXLimit = ptROI->tSize.iWidth + ptROI->tLocation.iX; 
    int_fast16_t iYLimit = ptROI->tSize.iHeight + ptROI->tLocation.iY; 

    /* decode */
    if (0 != arm_lmsk_decode(   &this.tDecoder, 
                                ptROI->tLocation.iX,
                                ptROI->tLocation.iY,
                                ptROI->tSize.iWidth,
                                ptROI->tSize.iHeight,
                                pchBuffer, 
                                wTargetStrideInByte)) {
        return ARM_2D_ERR_IO_ERROR;
    }

    return ARM_2D_ERR_NONE;
}

static
bool __lmsk_loader_seek(uintptr_t pTarget, int32_t nOffset)
{
    lmsk_loader_t *ptThis = (lmsk_loader_t *)pTarget;

#if __ARM_LMSK_USE_LOADER_IO__
    return arm_generic_loader_io_seek(  &this.use_as__arm_generic_loader_t, 
                                        nOffset, 
                                        SEEK_SET);
#else
    this.nPosition = nOffset;
    return true;
#endif
}

static
size_t __lmsk_loader_read ( intptr_t pTarget,      
                            uint8_t *pchBuffer,
                            size_t tLength)
{
    lmsk_loader_t *ptThis = (lmsk_loader_t *)pTarget;

#if __ARM_LMSK_USE_LOADER_IO__
    return arm_generic_loader_io_read(&this.use_as__arm_generic_loader_t, pchBuffer, tLength);
#else
    memcpy(pchBuffer, &this.pchLMSKSource[this.nPosition], tLength); 
    
    return tLength;
#endif
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif
