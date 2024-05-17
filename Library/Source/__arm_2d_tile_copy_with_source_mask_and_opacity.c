/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        __arm_2d_tile_copy_with_mask_and_opacity.c
 * Description:  APIs tile copy with mask and opacity
 *
 * $Date:        30. April 2024
 * $Revision:    V.0.5.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */


/*============================ INCLUDES ======================================*/

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wempty-translation-unit"
#endif

#ifdef __ARM_2D_COMPILATION_UNIT
#undef __ARM_2D_COMPILATION_UNIT

#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wextra-semi-stmt"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wswitch-enum"
#   pragma clang diagnostic ignored "-Wswitch"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    hwRatio += (hwRatio == 255);

    uint16_t hwRatioCompl = 256 - hwRatio;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(  pwSourceLine++, 
                                                        pchTargetLine++,
                                                        hwRatioCompl);

        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }

}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*pchSourceMaskLine++) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(  pwSourceLine++, 
                                                        pchTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_gray8_tile_copy_with_src_mask_and_opacity(
                                    uint8_t *__RESTRICT pchSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *__RESTRICT pchSourceLine = pchSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*pchSourceMaskLine++) >> 8);

            __ARM_2D_PIXEL_BLENDING_GRAY8(  pchSourceLine++, 
                                            pchTargetLine++,
                                            hwTransparency);
        }

        pchSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_gray8_tile_copy_with_src_chn_mask_and_opacity(
                                    uint8_t *__RESTRICT pchSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *__RESTRICT pchSourceLine = pchSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*(uint8_t *)(pwSourceMaskLine++)) >> 8);

            __ARM_2D_PIXEL_BLENDING_GRAY8(  pchSourceLine++, 
                                                        pchTargetLine++,
                                                        hwTransparency);
        }

        pchSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_chn_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*(uint8_t *)(pwSourceMaskLine++)) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(  pwSourceLine++, 
                                                        pchTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*pchSourceMaskLine++);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(  pwSourceLine++, 
                                                        pchTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_chn_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*(uint8_t *)(pwSourceMaskLine++));

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(  pwSourceLine++, 
                                                        pchTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_source_mask_and_opacity_only( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_src_msk_opc_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    //! this should not happen
    assert(NULL != this.Mask.ptSourceSide);

    if (ptTask->Param.tCopyMask.tSrcMask.bInvalid) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (255 == this.chOpacity) {
        return __arm_2d_gray8_sw_tile_copy_with_src_mask_only(ptTask);
    }

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(this.Source.ptTile, NULL, NULL);
    assert(NULL != ptSourceRoot);
    if (ARM_2D_COLOUR_CCCA8888 == ptSourceRoot->tInfo.tColourInfo.chScheme) {

    
        if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme) {
        
        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
            __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_chn_mask_and_opacity(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,

                &ptTask->Param.tCopy.tCopySize,
                this.chOpacity);
        #else
            return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
        #endif

        } else {
            __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_mask_and_opacity(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,

                &ptTask->Param.tCopy.tCopySize,
                this.chOpacity);
        }

        return arm_fsm_rt_cpl;
    }
#endif

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme) {
    
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__

        __arm_2d_impl_gray8_tile_copy_with_src_chn_mask_and_opacity(
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.iStride,

            ptTask->Param.tCopyMask.tSrcMask.pBuffer,
            ptTask->Param.tCopyMask.tSrcMask.iStride,
            &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.iStride,
                
            &ptTask->Param.tCopy.tCopySize,
            this.chOpacity);
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    } else {
        assert(     ARM_2D_COLOUR_8BIT 
              ==    ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme);

        __arm_2d_impl_gray8_tile_copy_with_src_mask_and_opacity(
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.iStride,

            ptTask->Param.tCopyMask.tSrcMask.pBuffer,
            ptTask->Param.tCopyMask.tSrcMask.iStride,
            &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.iStride,
                
            &ptTask->Param.tCopy.tCopySize,
            this.chOpacity);

    }

    return arm_fsm_rt_cpl;
}


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_OPACITY_ONLY_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_source_mask_and_opacity_only);


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_OPACITY_ONLY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour = true,
#endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_OPACITY_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_OPACITY_ONLY_GRAY8),
        },
    },
};

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    hwRatio += (hwRatio == 255);

    uint16_t hwRatioCompl = 256 - hwRatio;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(  pwSourceLine++, 
                                                        phwTargetLine++,
                                                        hwRatioCompl);

        }

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    }

}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*pchSourceMaskLine++) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(  pwSourceLine++, 
                                                        phwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_rgb565_tile_copy_with_src_mask_and_opacity(
                                    uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *__RESTRICT phwSourceLine = phwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*pchSourceMaskLine++) >> 8);

            __ARM_2D_PIXEL_BLENDING_RGB565(  phwSourceLine++, 
                                            phwTargetLine++,
                                            hwTransparency);
        }

        phwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_rgb565_tile_copy_with_src_chn_mask_and_opacity(
                                    uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *__RESTRICT phwSourceLine = phwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*(uint8_t *)(pwSourceMaskLine++)) >> 8);

            __ARM_2D_PIXEL_BLENDING_RGB565(  phwSourceLine++, 
                                                        phwTargetLine++,
                                                        hwTransparency);
        }

        phwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_chn_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*(uint8_t *)(pwSourceMaskLine++)) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(  pwSourceLine++, 
                                                        phwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*pchSourceMaskLine++);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(  pwSourceLine++, 
                                                        phwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_chn_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*(uint8_t *)(pwSourceMaskLine++));

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(  pwSourceLine++, 
                                                        phwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_source_mask_and_opacity_only( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_src_msk_opc_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    //! this should not happen
    assert(NULL != this.Mask.ptSourceSide);

    if (ptTask->Param.tCopyMask.tSrcMask.bInvalid) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (255 == this.chOpacity) {
        return __arm_2d_rgb565_sw_tile_copy_with_src_mask_only(ptTask);
    }

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(this.Source.ptTile, NULL, NULL);
    assert(NULL != ptSourceRoot);
    if (ARM_2D_COLOUR_CCCA8888 == ptSourceRoot->tInfo.tColourInfo.chScheme) {

    
        if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme) {
        
        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
            __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_chn_mask_and_opacity(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,

                &ptTask->Param.tCopy.tCopySize,
                this.chOpacity);
        #else
            return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
        #endif

        } else {
            __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_mask_and_opacity(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,

                &ptTask->Param.tCopy.tCopySize,
                this.chOpacity);
        }

        return arm_fsm_rt_cpl;
    }
#endif

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme) {
    
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__

        __arm_2d_impl_rgb565_tile_copy_with_src_chn_mask_and_opacity(
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.iStride,

            ptTask->Param.tCopyMask.tSrcMask.pBuffer,
            ptTask->Param.tCopyMask.tSrcMask.iStride,
            &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.iStride,
                
            &ptTask->Param.tCopy.tCopySize,
            this.chOpacity);
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    } else {
        assert(     ARM_2D_COLOUR_8BIT 
              ==    ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme);

        __arm_2d_impl_rgb565_tile_copy_with_src_mask_and_opacity(
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.iStride,

            ptTask->Param.tCopyMask.tSrcMask.pBuffer,
            ptTask->Param.tCopyMask.tSrcMask.iStride,
            &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.iStride,
                
            &ptTask->Param.tCopy.tCopySize,
            this.chOpacity);

    }

    return arm_fsm_rt_cpl;
}


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_OPACITY_ONLY_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_source_mask_and_opacity_only);


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_OPACITY_ONLY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour = true,
#endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_OPACITY_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_OPACITY_ONLY_RGB565),
        },
    },
};

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    hwRatio += (hwRatio == 255);

    uint16_t hwRatioCompl = 256 - hwRatio;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(  pwSourceLine++, 
                                                        pwTargetLine++,
                                                        hwRatioCompl);

        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;
    }

}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*pchSourceMaskLine++) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(  pwSourceLine++, 
                                                        pwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_cccn888_tile_copy_with_src_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*pchSourceMaskLine++) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCN888(  pwSourceLine++, 
                                            pwTargetLine++,
                                            hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_cccn888_tile_copy_with_src_chn_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*(uint8_t *)(pwSourceMaskLine++)) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCN888(  pwSourceLine++, 
                                                        pwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_chn_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*(uint8_t *)(pwSourceMaskLine++)) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(  pwSourceLine++, 
                                                        pwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*pchSourceMaskLine++);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(  pwSourceLine++, 
                                                        pwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_chn_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*(uint8_t *)(pwSourceMaskLine++));

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(  pwSourceLine++, 
                                                        pwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_source_mask_and_opacity_only( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_src_msk_opc_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    //! this should not happen
    assert(NULL != this.Mask.ptSourceSide);

    if (ptTask->Param.tCopyMask.tSrcMask.bInvalid) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (255 == this.chOpacity) {
        return __arm_2d_cccn888_sw_tile_copy_with_src_mask_only(ptTask);
    }

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(this.Source.ptTile, NULL, NULL);
    assert(NULL != ptSourceRoot);
    if (ARM_2D_COLOUR_CCCA8888 == ptSourceRoot->tInfo.tColourInfo.chScheme) {

    
        if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme) {
        
        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
            __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_chn_mask_and_opacity(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,

                &ptTask->Param.tCopy.tCopySize,
                this.chOpacity);
        #else
            return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
        #endif

        } else {
            __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_mask_and_opacity(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,

                &ptTask->Param.tCopy.tCopySize,
                this.chOpacity);
        }

        return arm_fsm_rt_cpl;
    }
#endif

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme) {
    
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__

        __arm_2d_impl_cccn888_tile_copy_with_src_chn_mask_and_opacity(
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.iStride,

            ptTask->Param.tCopyMask.tSrcMask.pBuffer,
            ptTask->Param.tCopyMask.tSrcMask.iStride,
            &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.iStride,
                
            &ptTask->Param.tCopy.tCopySize,
            this.chOpacity);
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    } else {
        assert(     ARM_2D_COLOUR_8BIT 
              ==    ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme);

        __arm_2d_impl_cccn888_tile_copy_with_src_mask_and_opacity(
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tSource.iStride,

            ptTask->Param.tCopyMask.tSrcMask.pBuffer,
            ptTask->Param.tCopyMask.tSrcMask.iStride,
            &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
            ptTask->Param.tCopyMask
                .use_as____arm_2d_param_copy_t.tTarget.iStride,
                
            &ptTask->Param.tCopy.tCopySize,
            this.chOpacity);

    }

    return arm_fsm_rt_cpl;
}


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_OPACITY_ONLY_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_source_mask_and_opacity_only);


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_OPACITY_ONLY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour = true,
#endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_OPACITY_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_OPACITY_ONLY_CCCN888),
        },
    },
};



#ifdef   __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */