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
#define __CIRCLE_MASK_GENERATOR_IMPLEMENT__
#include "circle_mask_generator.h"

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
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
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
arm_2d_err_t __circle_mask_generator_decoder_init(arm_generic_loader_t *ptObj);

ARM_NONNULL(1, 2, 3)
static
arm_2d_err_t __circle_mask_generator_decode(arm_generic_loader_t *ptObj,
                                 arm_2d_region_t *ptROI,
                                 uint8_t *pchBuffer,
                                 uint32_t iTargetStrideInByte,
                                 uint_fast8_t chBitsPerPixel);
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t circle_mask_generator_init(circle_mask_generator_t *ptThis,
                                        circle_mask_generator_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    
    memset(ptThis, 0, sizeof(circle_mask_generator_t));

    this.iRadius = ptCFG->iRadius;
    this.bAntiAlias = ptCFG->bAntiAlias;

    arm_2d_err_t tResult = ARM_2D_ERR_NONE;
    do {
        arm_generic_loader_cfg_t tCFG = {
            .bUseHeapForVRES = ptCFG->bUseHeapForVRES,
            .tColourInfo.chScheme = ARM_2D_COLOUR_MASK_A8,

            .UserDecoder = {
                .fnDecoderInit = &__circle_mask_generator_decoder_init,
                .fnDecode = &__circle_mask_generator_decode,
            },

            .ptScene = ptCFG->ptScene,
        };

        tResult = arm_generic_loader_init(  &this.use_as__arm_generic_loader_t,
                                            &tCFG);

        if (tResult < 0) {
            break;
        }

        this.tTile.tRegion.tSize = ptCFG->tBoxSize;
        if ((0 == this.tTile.tRegion.tSize.iWidth)
         || (0 == this.tTile.tRegion.tSize.iHeight)) {
            tResult = ARM_2D_ERR_INVALID_PARAM;
            break;
        }

        /* update pivot */
        if (NULL == ptCFG->ptPivot) {
            this.tPivot.iX = (this.tTile.tRegion.tSize.iWidth - 1) >> 1;
            this.tPivot.iY = (this.tTile.tRegion.tSize.iHeight - 1) >> 1;
        } else {
            this.tPivot = *ptCFG->ptPivot;
        }

    } while(0);

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
void circle_mask_generator_depose( circle_mask_generator_t *ptThis)
{
    assert(NULL != ptThis);

    arm_generic_loader_depose(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void circle_mask_generator_on_load( circle_mask_generator_t *ptThis)
{
    assert(NULL != ptThis);
    
    arm_generic_loader_on_load(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void circle_mask_generator_on_frame_start( circle_mask_generator_t *ptThis)
{
    assert(NULL != ptThis);
    
    arm_generic_loader_on_frame_start(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void circle_mask_generator_on_frame_complete( circle_mask_generator_t *ptThis)
{
    assert(NULL != ptThis);
    
    arm_generic_loader_on_frame_complete(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
arm_2d_err_t circle_mask_generator_set_radius(  circle_mask_generator_t *ptThis, 
                                                int16_t iRadius)
{
    assert(NULL != ptThis);

    if (iRadius > 0) {
        this.iRadius = iRadius;
    } else {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
int16_t circle_mask_generator_get_radius(  circle_mask_generator_t *ptThis)
{
    assert(NULL != ptThis);
    return this.iRadius;
}

ARM_NONNULL(1)
static
arm_2d_err_t __circle_mask_generator_decoder_init(arm_generic_loader_t *ptObj)
{
    assert(NULL != ptObj);

    circle_mask_generator_t *ptThis = (circle_mask_generator_t *)ptObj;
    ARM_2D_UNUSED(ptThis);

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1, 2, 3)
static
arm_2d_err_t __circle_mask_generator_decode(arm_generic_loader_t *ptObj,
                                 arm_2d_region_t *ptROI,
                                 uint8_t *pchBuffer,
                                 uint32_t iTargetStrideInByte,
                                 uint_fast8_t chBitsPerPixel)
{
    ARM_2D_UNUSED(chBitsPerPixel);

    assert(NULL != ptObj);
    circle_mask_generator_t *ptThis = (circle_mask_generator_t *)ptObj;

    int_fast16_t iXLimit = ptROI->tSize.iWidth + ptROI->tLocation.iX; 
    int_fast16_t iYLimit = ptROI->tSize.iHeight + ptROI->tLocation.iY; 

    int16_t iRadius = this.iRadius;

    uint32_t wRadius2 = (uint32_t)iRadius * (uint32_t)iRadius;
    uint32_t wRadiusBorder2 = (uint32_t)(iRadius + 1) * (uint32_t)(iRadius + 1);

    q16_t q161MagicRatio 
        = reinterpret_q16_s16( 
            (int16_t)(wRadiusBorder2 - wRadius2));

    for (int_fast16_t iY = ptROI->tLocation.iY; iY < iYLimit; iY++) {

        uint8_t *pchTargetLine = pchBuffer;
        int16_t iYOffset = iY - this.tPivot.iY;

        bool bFindFirstInnerPoint = false;
        uint16_t hwInnerPoints = 0;

        int_fast16_t iX = ptROI->tLocation.iX;

    /* disable it for Cortex-M0/M0+ */
    #if !(defined(__ARM_ARCH) && __ARM_ARCH_PROFILE == 'M' && (__ARM_ARCH_ISA_THUMB < 2))
        /* calculate the left most point */
        if (ABS(iYOffset) < iRadius) {

            uint32_t wDistance2 = wRadius2
                                - (uint32_t)iYOffset * (uint32_t)iYOffset;
            
            float fXDelta;
            arm_sqrt_f32((float)wDistance2, &fXDelta);

            /* NOTE: Here 4 is a magic number for improving the margin 
             * smoothness, please do NOT change it
             */
            int16_t iLeftMostX = this.tPivot.iX - fXDelta - 4;  
            int16_t iXAdvance = iLeftMostX - iX;
            if (iX < iLeftMostX && iLeftMostX < iXLimit) {
                iX = iLeftMostX;
                memset(pchTargetLine, 0x00, iXAdvance);
                pchTargetLine += iXAdvance;
            }
        } while(0);
    #endif

        for (; iX < iXLimit; iX++) {

            if (bFindFirstInnerPoint) {
                /* draw inner points */
                if (--hwInnerPoints) {
                    *pchTargetLine++ = 0xFF;
                    continue;
                } else {
                    bFindFirstInnerPoint = false;
                }
            }
            
            {
                int16_t iXOffset = iX - this.tPivot.iX;
                
                /* calculate the distance */
                uint32_t wDistance2 = (uint32_t)iXOffset * (uint32_t)iXOffset + (uint32_t)iYOffset * (uint32_t)iYOffset;
                
                if (wDistance2 >= wRadiusBorder2) {
                    *pchTargetLine++ = 0x00;
                    continue;
                } else if (wDistance2 <= wRadius2) {
                    if (iXOffset < 0) {
                        bFindFirstInnerPoint = true;
                        hwInnerPoints = ABS(iXOffset) * 2;
                    }
                    /* fill colour with opacity */
                    *pchTargetLine++ = 0xFF;
                    continue;
                } else if (!this.bAntiAlias) {
                    *pchTargetLine++ = 0x00;
                    continue;
                }

                q16_t q16Delta = reinterpret_q16_s16(wDistance2 - wRadius2);

                q16Delta = div_q16(q16Delta, q161MagicRatio);

                *pchTargetLine++ = 255 - ((q16Delta & 0xFF00) >> 8);
            }
        }

        pchBuffer += iTargetStrideInByte;
    }

    return ARM_2D_ERR_NONE;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif