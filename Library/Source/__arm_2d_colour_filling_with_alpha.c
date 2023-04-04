/*
 * Copyright (C) 2010-2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        __arm_2d_colour_filling_with_alpha.c
 * Description:  APIs for colour-filling operations related to alpha channel
 *               and masks
 *
 * $Date:        4. April 2023
 * $Revision:    V.1.0.0
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
//static
//const uint8_t c_chA2MaskTable[4]  = {0, 85, 170, 255};

//static
//const uint8_t c_chA4MaskTable[16] = {0, 17, 34,  51, 68, 85, 102, 119,
//                                     136, 153, 170, 187, 204, 221, 238, 255};

/*============================ IMPLEMENTATION ================================*/




/*----------------------------------------------------------------------------*
 * Accelerable Low Level APIs                                                 *
 *----------------------------------------------------------------------------*/
/*! adding support with c code template */

#define __API_COLOUR        gray8
#define __API_INT_TYPE      uint8_t
#define __API_PIXEL_BLENDING            __ARM_2D_PIXEL_BLENDING_GRAY8
            
#include "__arm_2d_ll_colour_filling_with_alpha.inc"


#define __API_COLOUR        rgb565
#define __API_INT_TYPE      uint16_t
#define __API_PIXEL_BLENDING            __ARM_2D_PIXEL_BLENDING_RGB565

#include "__arm_2d_ll_colour_filling_with_alpha.inc"


/*! adding support with c code template */
#define __API_COLOUR        cccn888
#define __API_INT_TYPE      uint32_t
#define __API_PIXEL_BLENDING            __ARM_2D_PIXEL_BLENDING_CCCN888

#include "__arm_2d_ll_colour_filling_with_alpha.inc"


/*----------------------------------------------------------------------------*
 * Fill a specified region with a given colour and transparency ratio (0~255) *
 *----------------------------------------------------------------------------*/
 
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                arm_2d_color_gray8_t tColour,
                                                uint_fast8_t chRatio)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_OPACITY_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.chColour = tColour.tValue;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
} 

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                arm_2d_color_rgb565_t tColour,
                                                uint_fast8_t chRatio)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_OPACITY_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.hwColour = tColour.tValue;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}
 
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_opacity( 
                                                arm_2d_op_fill_cl_opc_t *ptOP,
                                                const arm_2d_tile_t *ptTarget,
                                                const arm_2d_region_t *ptRegion,
                                                arm_2d_color_cccn888_t tColour,
                                                uint_fast8_t chRatio)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_OPACITY_RGB888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.wColour = tColour.tValue;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);

    if (255 == this.chRatio) {
        __arm_2d_impl_c8bit_colour_filling(
                            ptTask->Param.tTileProcess.pBuffer,
                            ptTask->Param.tTileProcess.iStride,
                            &(ptTask->Param.tTileProcess.tValidRegion.tSize),
                            this.chColour );
    } else {
        __arm_2d_impl_gray8_colour_filling_with_opacity(  
                                ptTask->Param.tTileProcess.pBuffer,
                                ptTask->Param.tTileProcess.iStride,
                                &(ptTask->Param.tTileProcess.tValidRegion.tSize),
                                this.chColour,
                                this.chRatio);
    }
    return arm_fsm_rt_cpl;
}
 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    if (255 == this.chRatio) {
        __arm_2d_impl_rgb16_colour_filling(
                            ptTask->Param.tTileProcess.pBuffer,
                            ptTask->Param.tTileProcess.iStride,
                            &(ptTask->Param.tTileProcess.tValidRegion.tSize),
                            this.hwColour );
    } else {
        __arm_2d_impl_rgb565_colour_filling_with_opacity(  
                        ptTask->Param.tTileProcess.pBuffer,
                        ptTask->Param.tTileProcess.iStride,
                        &(ptTask->Param.tTileProcess.tValidRegion.tSize),
                        this.hwColour,
                        this.chRatio);
    }
    
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_opc_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chRatio) {
        __arm_2d_impl_rgb32_colour_filling(
                            ptTask->Param.tTileProcess.pBuffer,
                            ptTask->Param.tTileProcess.iStride,
                            &(ptTask->Param.tTileProcess.tValidRegion.tSize),
                            this.wColour );
    } else {
        __arm_2d_impl_cccn888_colour_filling_with_opacity(  
                        ptTask->Param.tTileProcess.pBuffer,
                        ptTask->Param.tTileProcess.iStride,
                        &(ptTask->Param.tTileProcess.tValidRegion.tSize),
                        this.wColour,
                        this.chRatio);
    }
    
    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour and an 8bit alpha mask                   *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_gray8_t tColour)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);
    
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(   ptAlpha, 
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

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_MASK_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.chColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_mask(
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
    if (!__arm_2d_valid_mask(   ptAlpha, 
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

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_MASK_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.hwColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_cccn888_t tColour)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(   ptAlpha, 
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

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_MASK_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.wColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_gray8_colour_filling_channel_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.chColour);
    #endif
    
    } else {
        __arm_2d_impl_gray8_colour_filling_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.chColour);
    }
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_rgb565_colour_filling_channel_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour);
    #endif
    } else {
        __arm_2d_impl_rgb565_colour_filling_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour);
    }
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_cccn888_colour_filling_channel_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.wColour);
    #endif
    } else {
        __arm_2d_impl_cccn888_colour_filling_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        &(ptTask->Param.tCopy.tCopySize),
                        this.wColour);
    }

    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour and an 2bit alpha mask                   *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_a2_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_gray8_t tColour)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);
    
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A2 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.chColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_a2_mask(
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
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A2 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.hwColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_a2_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_cccn888_t tColour)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

    //! valid alpha mask tile
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A2 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.wColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_a2_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_impl_gray8_colour_filling_a2_mask(
                    ptTask->Param.tCopy.tTarget.pBuffer,
                    ptTask->Param.tCopy.tTarget.iStride,
                    ptTask->Param.tCopy.tSource.pBuffer,                        //!< alpha tile
                    ptTask->Param.tCopy.tSource.iStride,                        //!< alpha tile
                    ptTask->Param.tCopy.tSource.nOffset,
                    &(ptTask->Param.tCopy.tCopySize),
                    this.chColour);

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_a2_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_impl_rgb565_colour_filling_a2_mask(
                    ptTask->Param.tCopy.tTarget.pBuffer,
                    ptTask->Param.tCopy.tTarget.iStride,
                    ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                    ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                    ptTask->Param.tCopy.tSource.nOffset,
                    &(ptTask->Param.tCopy.tCopySize),
                    this.hwColour);

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_a2_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_impl_cccn888_colour_filling_a2_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.wColour);

    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour and a 4bit alpha mask                   *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_a4_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_gray8_t tColour)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);
    
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A4 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.chColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_a4_mask(
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
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A4 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.hwColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_a4_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_cccn888_t tColour)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A4 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.wColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_a4_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_impl_gray8_colour_filling_a4_mask(
                    ptTask->Param.tCopy.tTarget.pBuffer,
                    ptTask->Param.tCopy.tTarget.iStride,
                    ptTask->Param.tCopy.tSource.pBuffer,                        //!< alpha tile
                    ptTask->Param.tCopy.tSource.iStride,                        //!< alpha tile
                    ptTask->Param.tCopy.tSource.nOffset,
                    &(ptTask->Param.tCopy.tCopySize),
                    this.chColour);

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_a4_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_impl_rgb565_colour_filling_a4_mask(
                    ptTask->Param.tCopy.tTarget.pBuffer,
                    ptTask->Param.tCopy.tTarget.iStride,
                    ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                    ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                    ptTask->Param.tCopy.tSource.nOffset,
                    &(ptTask->Param.tCopy.tCopySize),
                    this.hwColour);

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_a4_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __arm_2d_impl_cccn888_colour_filling_a4_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                    //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.wColour);

    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour, an a2 mask and a specified opacity      *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_a2_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_gray8_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A2 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_AND_OPACITY_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.chColour = tColour.tValue;
    this.chRatio = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_a2_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A2 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_AND_OPACITY_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.hwColour = tColour.tValue;
    this.chRatio = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_a2_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_cccn888_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A2 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_AND_OPACITY_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.wColour = tColour.tValue;
    this.chRatio = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_a2_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chRatio) {
        __arm_2d_impl_gray8_colour_filling_a2_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.chColour);
    } else {
        __arm_2d_impl_gray8_colour_filling_a2_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.chColour,
                        this.chRatio);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_a2_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chRatio) {
        __arm_2d_impl_rgb565_colour_filling_a2_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour);
    } else {
        __arm_2d_impl_rgb565_colour_filling_a2_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour,
                        this.chRatio);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_a2_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chRatio) {
        __arm_2d_impl_cccn888_colour_filling_a2_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.wColour);
    } else {
        __arm_2d_impl_cccn888_colour_filling_a2_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.wColour,
                        this.chRatio);
    }

    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour, an a4 mask and a specified opacity      *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_a4_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_gray8_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A4 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_AND_OPACITY_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.chColour = tColour.tValue;
    this.chRatio = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_a4_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A4 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_AND_OPACITY_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.hwColour = tColour.tValue;
    this.chRatio = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_a4_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_cccn888_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptAlpha, __ARM_2D_MASK_ALLOW_A4 )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_AND_OPACITY_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.wColour = tColour.tValue;
    this.chRatio = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_a4_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chRatio) {
        __arm_2d_impl_gray8_colour_filling_a4_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.chColour);
    } else {
        __arm_2d_impl_gray8_colour_filling_a4_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.chColour,
                        this.chRatio);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_a4_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chRatio) {
        __arm_2d_impl_rgb565_colour_filling_a4_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour);
    } else {
        __arm_2d_impl_rgb565_colour_filling_a4_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.hwColour,
                        this.chRatio);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_a4_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chRatio) {
        __arm_2d_impl_cccn888_colour_filling_a4_mask(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.wColour);
    } else {
        __arm_2d_impl_cccn888_colour_filling_a4_mask_opacity(
                        ptTask->Param.tCopy.tTarget.pBuffer,
                        ptTask->Param.tCopy.tTarget.iStride,
                        ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                        ptTask->Param.tCopy.tSource.nOffset,
                        &(ptTask->Param.tCopy.tCopySize),
                        this.wColour,
                        this.chRatio);
    }

    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Fill tile with a specified colour, an alpha mask and a specified opacity   *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_gray8_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

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

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_OPACITY_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.chColour = tColour.tValue;
    this.chRatio = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

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

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_OPACITY_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.hwColour = tColour.tValue;
    this.chRatio = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_mask_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_cccn888_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptAlpha);

    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptOP);

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

    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_OPACITY_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Mask.ptTile = ptAlpha;
    this.wMode = 0;
    this.wColour = tColour.tValue;
    this.chRatio = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
    
        if (255 == this.chRatio) {
            __arm_2d_impl_gray8_colour_filling_channel_mask(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.chColour);
        } else {
            __arm_2d_impl_gray8_colour_filling_channel_mask_opacity(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.chColour,
                            this.chRatio);
        }
    #endif
    } else {
        if (255 == this.chRatio) {
            __arm_2d_impl_gray8_colour_filling_mask(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.chColour);
        } else {
            __arm_2d_impl_gray8_colour_filling_mask_opacity(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.chColour,
                            this.chRatio);
        }
    }
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        if (255 == this.chRatio) {
            __arm_2d_impl_rgb565_colour_filling_channel_mask(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.hwColour);
        } else {
            __arm_2d_impl_rgb565_colour_filling_channel_mask_opacity(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.hwColour,
                            this.chRatio);
        }
    #endif
    } else {
        if (255 == this.chRatio) {
            __arm_2d_impl_rgb565_colour_filling_mask(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.hwColour);
        } else {
            __arm_2d_impl_rgb565_colour_filling_mask_opacity(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.hwColour,
                            this.chRatio);
        }
    }
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_fill_cl_msk_opc_t, ptTask->ptOP)
    //assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        if (255 == this.chRatio) {
            __arm_2d_impl_cccn888_colour_filling_channel_mask(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.wColour);
        } else {
            __arm_2d_impl_cccn888_colour_filling_channel_mask_opacity(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.wColour,
                            this.chRatio);
        }
    #endif
    } else {
        if (255 == this.chRatio) {
            __arm_2d_impl_cccn888_colour_filling_mask(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.wColour);
        } else {
            __arm_2d_impl_cccn888_colour_filling_mask_opacity(
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tCopySize),
                            this.wColour,
                            this.chRatio);
        }
    }

    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Low Level IO Interfaces                                                    *
 *----------------------------------------------------------------------------*/

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_GRAY8, 
                __arm_2d_gray8_sw_colour_filling_with_a2_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_a2_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_RGB888, 
                __arm_2d_cccn888_sw_colour_filling_with_a2_mask);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_GRAY8, 
                __arm_2d_gray8_sw_colour_filling_with_a4_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_a4_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_RGB888, 
                __arm_2d_cccn888_sw_colour_filling_with_a4_mask);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_GRAY8, 
                __arm_2d_gray8_sw_colour_filling_with_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_CCCN888, 
                __arm_2d_cccn888_sw_colour_filling_with_mask);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_AND_OPACITY_GRAY8, 
                __arm_2d_gray8_sw_colour_filling_with_a2_mask_and_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_AND_OPACITY_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_a2_mask_and_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_AND_OPACITY_CCCN888, 
                __arm_2d_cccn888_sw_colour_filling_with_a2_mask_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_AND_OPACITY_GRAY8, 
                __arm_2d_gray8_sw_colour_filling_with_a4_mask_and_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_AND_OPACITY_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_a4_mask_and_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_AND_OPACITY_CCCN888, 
                __arm_2d_cccn888_sw_colour_filling_with_a4_mask_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_AND_OPACITY_GRAY8, 
                __arm_2d_gray8_sw_colour_filling_with_mask_and_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_AND_OPACITY_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_mask_and_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_AND_OPACITY_CCCN888, 
                __arm_2d_cccn888_sw_colour_filling_with_mask_and_opacity);


const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A2_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A2_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A2_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_RGB888),
            .ptFillLike = NULL, 
        },
    },
};  

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A4_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A4_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A4_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_RGB888),
            .ptFillLike = NULL, 
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_CCCN888),
            .ptFillLike = NULL, 
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_REPEAT_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_REPEAT,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_GRAY8),
            //.ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_AND_REPEAT_GRAY8),, 
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_REPEAT_RGB565= {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_REPEAT,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_RGB565),
            //.ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_AND_REPEAT_RGB565),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_REPEAT_CCCN888= {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_REPEAT,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_CCCN888),
            //.ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_AND_REPEAT_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A2_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_AND_OPACITY_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A2_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_AND_OPACITY_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A2_MASK_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A2_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A2_MASK_AND_OPACITY_CCCN888),
            .ptFillLike = NULL, 
        },
    },
}; 

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A4_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_AND_OPACITY_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A4_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_AND_OPACITY_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_A4_MASK_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A4_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_A4_MASK_AND_OPACITY_CCCN888),
            .ptFillLike = NULL, 
        },
    },
};  

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_AND_OPACITY_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_AND_OPACITY_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MASK_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_MASK_AND_OPACITY_CCCN888),
            .ptFillLike = NULL, 
        },
    },
}; 


#ifdef   __cplusplus
}
#endif


#endif
