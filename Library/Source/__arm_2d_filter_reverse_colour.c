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
 * Title:        __arm_2d_filter_reverse_colour.c
 * Description:  the source code of the colour-reversing
 *
 * $Date:        29. Nov 2024
 * $Revision:    V.1.0.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

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
#undef OP_CORE
#define OP_CORE this.use_as__arm_2d_op_core_t
#undef OPCODE
#define OPCODE this

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_filter_reverse_colour(  
                                                arm_2d_op_fill_cl_msk_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_t, ptOP);

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILTER_REVERSE_COLOUR_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */

__WEAK
void __arm_2d_impl_gray8_reverse_colour(uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        uint8_t *__RESTRICT pchTargetLine = pchTarget;
        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++){
            *pchTargetLine++ ^= __UINT8_MAX__;
        }
        pchTarget += iTargetStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_gray8_sw_filter_reverse_colour( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_impl_gray8_reverse_colour(
                    ptTask->Param.tTileProcess.pBuffer,
                    ptTask->Param.tTileProcess.iStride,
                    &(ptTask->Param.tTileProcess.tValidRegion.tSize));

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILTER_REVERSE_COLOUR_GRAY8,
                __arm_2d_gray8_sw_filter_reverse_colour); /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILTER_REVERSE_COLOUR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILTER_REVERSE_COLOUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_FILTER_REVERSE_COLOUR_GRAY8),
        },
    },
};

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_filter_reverse_colour(  
                                                arm_2d_op_fill_cl_msk_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_t, ptOP);

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILTER_REVERSE_COLOUR_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */

__WEAK
void __arm_2d_impl_rgb565_reverse_colour(   uint16_t *__RESTRICT phwTarget,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        uint16_t *__RESTRICT phwTargetLine = phwTarget;
        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++){
            *phwTargetLine++ ^= __UINT16_MAX__;
        }
        phwTarget += iTargetStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_rgb565_sw_filter_reverse_colour( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_impl_rgb565_reverse_colour(
                    ptTask->Param.tTileProcess.pBuffer,
                    ptTask->Param.tTileProcess.iStride,
                    &(ptTask->Param.tTileProcess.tValidRegion.tSize));

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILTER_REVERSE_COLOUR_RGB565,
                __arm_2d_rgb565_sw_filter_reverse_colour); /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILTER_REVERSE_COLOUR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILTER_REVERSE_COLOUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_FILTER_REVERSE_COLOUR_RGB565),
        },
    },
};

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_filter_reverse_colour(  
                                                arm_2d_op_fill_cl_msk_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_t, ptOP);

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILTER_REVERSE_COLOUR_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */

__WEAK
void __arm_2d_impl_cccn888_reverse_colour(   uint32_t *__RESTRICT pwTarget,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        uint32_t *__RESTRICT pwTargetLine = pwTarget;
        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++){
            *pwTargetLine++ ^= 0x00FFFFFF;
        }
        pwTarget += iTargetStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_cccn888_sw_filter_reverse_colour( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_impl_cccn888_reverse_colour(
                    ptTask->Param.tTileProcess.pBuffer,
                    ptTask->Param.tTileProcess.iStride,
                    &(ptTask->Param.tTileProcess.tValidRegion.tSize));

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILTER_REVERSE_COLOUR_CCCN888,
                __arm_2d_cccn888_sw_filter_reverse_colour); /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILTER_REVERSE_COLOUR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILTER_REVERSE_COLOUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_FILTER_REVERSE_COLOUR_CCCN888),
        },
    },
};

#ifdef   __cplusplus
}
#endif

#endif
