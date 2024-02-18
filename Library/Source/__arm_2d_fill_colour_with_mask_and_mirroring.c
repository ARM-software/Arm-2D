/*
 * Copyright (C) 2010-2024 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        __arm_2d_fill_colour_with_mask_and_mirroring.c
 * Description:  APIs for colour-filling-with-mask-and-mirroring
 *
 * $Date:        18. Feb 2024
 * $Revision:    V.1.6.2
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
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-compare"
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
/*============================ INCLUDES ======================================*/
/*============================ IMPLEMENTATION ================================*/

/*
 * Target Colour: RGB565
 */
/* Low Level Interface */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_MASK_AND_X_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_mask_and_x_mirror);

/* OPCODE */
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_X_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_MASK_AND_X_MIRROR_RGB565),
            .ptFillLike = NULL, 
        },
    },
};

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_mask_and_x_mirror(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, 
                                __ARM_2D_MASK_ALLOW_A8
                        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                            |   __ARM_2D_MASK_ALLOW_8in32
                        #endif
                            )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_X_MIRROR_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = ARM_2D_CP_MODE_X_MIRROR;
    this.hwColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

__WEAK
void __arm_2d_impl_rgb565_colour_filling_mask_x_mirror(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchAlpha,
                            int16_t iAlphaStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint16_t Colour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchSourceMaskCur = pchAlpha;
        pchSourceMaskCur += ptCopySize->iWidth - 1;

        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint16_t hwAlpha = 256 - (*pchSourceMaskCur--);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif
            __ARM_2D_PIXEL_BLENDING_RGB565(&Colour, phwTarget++, hwAlpha);
        }

        pchAlpha += iAlphaStride;
        phwTarget += (iTargetStride - iWidth);
    }
}

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
__WEAK
void __arm_2d_impl_rgb565_colour_filling_channel_mask_x_mirror(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwAlpha,
                            int16_t iAlphaStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint16_t Colour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwSourceMaskCur = pwAlpha;
        pwSourceMaskCur += ptCopySize->iWidth - 1;

        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint16_t hwAlpha = 256 - *(uint8_t *)(pwSourceMaskCur--);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif
            __ARM_2D_PIXEL_BLENDING_RGB565(&Colour, phwTarget++, hwAlpha);
        }

        pwAlpha += iAlphaStride;
        phwTarget += (iTargetStride - iWidth);
    }
}
#endif

/* default software adaptor for low-level interface */
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_rgb565_colour_filling_channel_mask_x_mirror(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour);
    #endif
    } else {
        __arm_2d_impl_rgb565_colour_filling_mask_x_mirror(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour);
    }
    return arm_fsm_rt_cpl;
}

#ifdef   __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */