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
 * Title:        __arm_2d_fill_colour_with_alpha_gradient_and_mask.c
 * Description:  The source code of APIs for colour-filling-with-alpha-gradient
 *               -and-mask
 *
 * $Date:        15. Aug 2024
 * $Revision:    V.0.5.2
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
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144,64
#endif

/*============================ MACROS ========================================*/
#undef OP_CORE
#define OP_CORE this.use_as__arm_2d_op_src_t.use_as__arm_2d_op_core_t
#undef OPCODE
#define OPCODE this.use_as__arm_2d_op_src_t



/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#define __alpha_mix(__alpha1, __alpha2)                                         \
        (uint8_t)(  ((__alpha1) == 255)                                         \
                    ?   (__alpha2)                                              \
                    :   (   ((__alpha2) == 255)                                 \
                        ?   (__alpha1)                                          \
                        :   ((uint16_t)(__alpha1) * (uint16_t)(__alpha2) >> 8)))


/*---------------------------------------------------------------------------*
 * Colour Filling with 4 sample points Alpha Gradient and Mask               *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_4pts_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_4p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_gray8_t tColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_4p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_GRAY8;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.chColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;
    
    

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_4pts_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_4p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_gray8_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_4p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_GRAY8;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.chColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_gray8_fill_colour_with_4pts_alpha_gradient_and_mask(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint8_t chColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatioLeft, q16YRatioRight;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatioLeft = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                      / iHeight;

        q16YRatioRight = (((int32_t)(   tSamplePoints.chBottomRight 
                                    -   tSamplePoints.chTopRight)) << 16) 
                       / iHeight;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatioLeft;


        int32_t q16OpacityRight = (((int32_t)tSamplePoints.chTopRight) << 16) 
                               + (y + tOffset.iY) * q16YRatioRight;


        int32_t q16XRatio;
        /* calculate X Ratios */
        do {
            int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

            q16XRatio = (q16OpacityRight - q16OpacityLeft) / iWidth;
        } while(0);

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint8_t * __RESTRICT pchTargetLine = pchTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - ((nOpacity * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pchTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_gray8_fill_colour_with_4pts_alpha_gradient_and_chn_mask(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint8_t chColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatioLeft, q16YRatioRight;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatioLeft = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                      / iHeight;

        q16YRatioRight = (((int32_t)(   tSamplePoints.chBottomRight 
                                    -   tSamplePoints.chTopRight)) << 16) 
                       / iHeight;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatioLeft;


        int32_t q16OpacityRight = (((int32_t)tSamplePoints.chTopRight) << 16) 
                               + (y + tOffset.iY) * q16YRatioRight;


        int32_t q16XRatio;
        /* calculate X Ratios */
        do {
            int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

            q16XRatio = (q16OpacityRight - q16OpacityLeft) / iWidth;
        } while(0);

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint8_t * __RESTRICT pchTargetLine = pchTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - (nOpacity * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pchTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_4pts_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_4p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_gray8_fill_colour_with_4pts_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.chColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_gray8_fill_colour_with_4pts_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.chColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_GRAY8,
                __arm_2d_gray8_sw_colour_filling_with_4pts_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_GRAY8),
            .ptFillLike = NULL,
        },
    },
};

/*---------------------------------------------------------------------------*
 * Colour Filling with 3 sample points Alpha Gradient and Mask               *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_3pts_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_3p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_gray8_t tColour,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_3p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_GRAY8;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.chColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_3pts_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_3p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_gray8_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_3p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_GRAY8;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.chColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_gray8_fill_colour_with_3pts_alpha_gradient_and_mask(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint8_t chColour,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio, q16XRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iHeight;
    } while(0);

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chTopRight 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iWidth;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatio;

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint8_t * __RESTRICT pchTargetLine = pchTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = q16OpacityLeft + (x + tOffset.iX) * q16XRatio;
            nOpacity >>= 16;
            nOpacity = (nOpacity >= 255) * 255 + !(nOpacity >= 255) * nOpacity;
            nOpacity *= (nOpacity > 0);

            uint16_t hwAlpha = 256 - ((nOpacity * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pchTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_gray8_fill_colour_with_3pts_alpha_gradient_and_chn_mask(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint8_t chColour,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio, q16XRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iHeight;
    } while(0);

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chTopRight 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iWidth;
    } while(0);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatio;

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint8_t * __RESTRICT pchTargetLine = pchTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = q16OpacityLeft + (x + tOffset.iX) * q16XRatio;
            nOpacity >>= 16;
            nOpacity = (nOpacity >= 255) * 255 + !(nOpacity >= 255) * nOpacity;
            nOpacity *= (nOpacity > 0);

            uint16_t hwAlpha = 256 - (nOpacity * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pchTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_3pts_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_3p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_gray8_fill_colour_with_3pts_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.chColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_gray8_fill_colour_with_3pts_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.chColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_GRAY8,
                __arm_2d_gray8_sw_colour_filling_with_3pts_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_GRAY8),
            .ptFillLike = NULL,
        },
    },
};

/*---------------------------------------------------------------------------*
 * Colour Filling with Horizontal Alpha Gradient and Mask                    *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_horizontal_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_gray8_t tColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_GRAY8;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.chColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_horizontal_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_gray8_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_GRAY8;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.chColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_gray8_fill_colour_with_horizontal_alpha_gradient_and_mask(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint8_t chColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16XRatio;

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chRight 
                               -   tSamplePoints.chLeft)) << 16) 
                  / iWidth;
    } while(0);
    int32_t q16OpacityLeft = (int32_t)(tSamplePoints.chLeft) << 16;


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint8_t * __RESTRICT pchTargetLine = pchTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - ((nOpacity * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pchTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_gray8_fill_colour_with_horizontal_alpha_gradient_and_chn_mask(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint8_t chColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16XRatio;

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chRight 
                               -   tSamplePoints.chLeft)) << 16) 
                  / iWidth;
    } while(0);
    int32_t q16OpacityLeft = (int32_t)(tSamplePoints.chLeft) << 16;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint8_t * __RESTRICT pchTargetLine = pchTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - (nOpacity * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pchTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_horizontal_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_gray8_fill_colour_with_horizontal_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.chColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_gray8_fill_colour_with_horizontal_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.chColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_GRAY8,
                __arm_2d_gray8_sw_colour_filling_with_horizontal_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_GRAY8),
            .ptFillLike = NULL,
        },
    },
};


/*---------------------------------------------------------------------------*
 * Colour Filling with Vertical Alpha Gradient and Mask                      *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_vertical_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_gray8_t tColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_GRAY8;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.chColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_vertical_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_gray8_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_GRAY8;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.chColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_gray8_fill_colour_with_vertical_alpha_gradient_and_mask(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint8_t chColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottom 
                               -   tSamplePoints.chTop)) << 16) 
                  / iHeight;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTop) << 16) 
                               + (y + tOffset.iY) * q16YRatio;
        q16OpacityLeft >>= 16;

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint8_t * __RESTRICT pchTargetLine = pchTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */

            uint16_t hwAlpha = 256 - ((q16OpacityLeft * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pchTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_gray8_fill_colour_with_vertical_alpha_gradient_and_chn_mask(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint8_t chColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottom 
                               -   tSamplePoints.chTop)) << 16) 
                  / iHeight;
    } while(0);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTop) << 16) 
                               + (y + tOffset.iY) * q16YRatio;
        q16OpacityLeft >>= 16;

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint8_t * __RESTRICT pchTargetLine = pchTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            uint16_t hwAlpha = 256 - (q16OpacityLeft * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pchTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_vertical_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_gray8_fill_colour_with_vertical_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.chColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_gray8_fill_colour_with_vertical_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.chColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_GRAY8,
                __arm_2d_gray8_sw_colour_filling_with_vertical_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_GRAY8),
            .ptFillLike = NULL,
        },
    },
};


/*---------------------------------------------------------------------------*
 * Colour Filling with 4 sample points Alpha Gradient and Mask               *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_4pts_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_4p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_rgb565_t tColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_4p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_RGB565;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.hwColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;
    
    

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_4pts_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_4p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_rgb565_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_4p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_RGB565;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.hwColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_4pts_alpha_gradient_and_mask(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint16_t hwColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatioLeft, q16YRatioRight;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatioLeft = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                      / iHeight;

        q16YRatioRight = (((int32_t)(   tSamplePoints.chBottomRight 
                                    -   tSamplePoints.chTopRight)) << 16) 
                       / iHeight;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatioLeft;


        int32_t q16OpacityRight = (((int32_t)tSamplePoints.chTopRight) << 16) 
                               + (y + tOffset.iY) * q16YRatioRight;


        int32_t q16XRatio;
        /* calculate X Ratios */
        do {
            int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

            q16XRatio = (q16OpacityRight - q16OpacityLeft) / iWidth;
        } while(0);

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint16_t * __RESTRICT phwTargetLine = phwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - ((nOpacity * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        phwTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_4pts_alpha_gradient_and_chn_mask(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint16_t hwColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatioLeft, q16YRatioRight;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatioLeft = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                      / iHeight;

        q16YRatioRight = (((int32_t)(   tSamplePoints.chBottomRight 
                                    -   tSamplePoints.chTopRight)) << 16) 
                       / iHeight;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatioLeft;


        int32_t q16OpacityRight = (((int32_t)tSamplePoints.chTopRight) << 16) 
                               + (y + tOffset.iY) * q16YRatioRight;


        int32_t q16XRatio;
        /* calculate X Ratios */
        do {
            int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

            q16XRatio = (q16OpacityRight - q16OpacityLeft) / iWidth;
        } while(0);

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint16_t * __RESTRICT phwTargetLine = phwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - (nOpacity * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        phwTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_4pts_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_4p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_rgb565_fill_colour_with_4pts_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.hwColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_rgb565_fill_colour_with_4pts_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.hwColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_RGB565,
                __arm_2d_rgb565_sw_colour_filling_with_4pts_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_RGB565),
            .ptFillLike = NULL,
        },
    },
};

/*---------------------------------------------------------------------------*
 * Colour Filling with 3 sample points Alpha Gradient and Mask               *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_3pts_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_3p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_rgb565_t tColour,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_3p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_RGB565;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.hwColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_3pts_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_3p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_rgb565_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_3p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_RGB565;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.hwColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_3pts_alpha_gradient_and_mask(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint16_t hwColour,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio, q16XRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iHeight;
    } while(0);

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chTopRight 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iWidth;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatio;

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint16_t * __RESTRICT phwTargetLine = phwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = q16OpacityLeft + (x + tOffset.iX) * q16XRatio;
            nOpacity >>= 16;
            nOpacity = (nOpacity >= 255) * 255 + !(nOpacity >= 255) * nOpacity;
            nOpacity *= (nOpacity > 0);

            uint16_t hwAlpha = 256 - ((nOpacity * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        phwTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_3pts_alpha_gradient_and_chn_mask(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint16_t hwColour,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio, q16XRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iHeight;
    } while(0);

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chTopRight 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iWidth;
    } while(0);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatio;

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint16_t * __RESTRICT phwTargetLine = phwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = q16OpacityLeft + (x + tOffset.iX) * q16XRatio;
            nOpacity >>= 16;
            nOpacity = (nOpacity >= 255) * 255 + !(nOpacity >= 255) * nOpacity;
            nOpacity *= (nOpacity > 0);

            uint16_t hwAlpha = 256 - (nOpacity * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        phwTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_3pts_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_3p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_rgb565_fill_colour_with_3pts_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.hwColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_rgb565_fill_colour_with_3pts_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.hwColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_RGB565,
                __arm_2d_rgb565_sw_colour_filling_with_3pts_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_RGB565),
            .ptFillLike = NULL,
        },
    },
};

/*---------------------------------------------------------------------------*
 * Colour Filling with Horizontal Alpha Gradient and Mask                    *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_horizontal_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_rgb565_t tColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_RGB565;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.hwColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_horizontal_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_rgb565_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_RGB565;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.hwColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_horizontal_alpha_gradient_and_mask(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint16_t hwColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16XRatio;

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chRight 
                               -   tSamplePoints.chLeft)) << 16) 
                  / iWidth;
    } while(0);
    int32_t q16OpacityLeft = (int32_t)(tSamplePoints.chLeft) << 16;


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint16_t * __RESTRICT phwTargetLine = phwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - ((nOpacity * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        phwTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_horizontal_alpha_gradient_and_chn_mask(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint16_t hwColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16XRatio;

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chRight 
                               -   tSamplePoints.chLeft)) << 16) 
                  / iWidth;
    } while(0);
    int32_t q16OpacityLeft = (int32_t)(tSamplePoints.chLeft) << 16;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint16_t * __RESTRICT phwTargetLine = phwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - (nOpacity * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        phwTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_horizontal_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_rgb565_fill_colour_with_horizontal_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.hwColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_rgb565_fill_colour_with_horizontal_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.hwColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_RGB565,
                __arm_2d_rgb565_sw_colour_filling_with_horizontal_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_RGB565),
            .ptFillLike = NULL,
        },
    },
};


/*---------------------------------------------------------------------------*
 * Colour Filling with Vertical Alpha Gradient and Mask                      *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_vertical_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_rgb565_t tColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_RGB565;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.hwColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_vertical_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_rgb565_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_RGB565;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.hwColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_vertical_alpha_gradient_and_mask(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint16_t hwColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottom 
                               -   tSamplePoints.chTop)) << 16) 
                  / iHeight;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTop) << 16) 
                               + (y + tOffset.iY) * q16YRatio;
        q16OpacityLeft >>= 16;

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint16_t * __RESTRICT phwTargetLine = phwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */

            uint16_t hwAlpha = 256 - ((q16OpacityLeft * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        phwTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_vertical_alpha_gradient_and_chn_mask(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint16_t hwColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottom 
                               -   tSamplePoints.chTop)) << 16) 
                  / iHeight;
    } while(0);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTop) << 16) 
                               + (y + tOffset.iY) * q16YRatio;
        q16OpacityLeft >>= 16;

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint16_t * __RESTRICT phwTargetLine = phwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            uint16_t hwAlpha = 256 - (q16OpacityLeft * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        phwTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_vertical_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_rgb565_fill_colour_with_vertical_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.hwColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_rgb565_fill_colour_with_vertical_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.hwColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_RGB565,
                __arm_2d_rgb565_sw_colour_filling_with_vertical_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_RGB565),
            .ptFillLike = NULL,
        },
    },
};


/*---------------------------------------------------------------------------*
 * Colour Filling with 4 sample points Alpha Gradient and Mask               *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_4pts_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_4p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_cccn888_t tColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_4p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_CCCN888;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.wColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;
    
    

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_4pts_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_4p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_cccn888_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_4p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_CCCN888;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.wColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_4pts_alpha_gradient_and_mask(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint32_t wColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatioLeft, q16YRatioRight;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatioLeft = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                      / iHeight;

        q16YRatioRight = (((int32_t)(   tSamplePoints.chBottomRight 
                                    -   tSamplePoints.chTopRight)) << 16) 
                       / iHeight;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatioLeft;


        int32_t q16OpacityRight = (((int32_t)tSamplePoints.chTopRight) << 16) 
                               + (y + tOffset.iY) * q16YRatioRight;


        int32_t q16XRatio;
        /* calculate X Ratios */
        do {
            int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

            q16XRatio = (q16OpacityRight - q16OpacityLeft) / iWidth;
        } while(0);

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint32_t * __RESTRICT pwTargetLine = pwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - ((nOpacity * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pwTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_4pts_alpha_gradient_and_chn_mask(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint32_t wColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatioLeft, q16YRatioRight;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatioLeft = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                      / iHeight;

        q16YRatioRight = (((int32_t)(   tSamplePoints.chBottomRight 
                                    -   tSamplePoints.chTopRight)) << 16) 
                       / iHeight;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatioLeft;


        int32_t q16OpacityRight = (((int32_t)tSamplePoints.chTopRight) << 16) 
                               + (y + tOffset.iY) * q16YRatioRight;


        int32_t q16XRatio;
        /* calculate X Ratios */
        do {
            int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

            q16XRatio = (q16OpacityRight - q16OpacityLeft) / iWidth;
        } while(0);

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint32_t * __RESTRICT pwTargetLine = pwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - (nOpacity * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pwTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_4pts_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_4p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_cccn888_fill_colour_with_4pts_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.wColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_cccn888_fill_colour_with_4pts_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.wColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_CCCN888,
                __arm_2d_cccn888_sw_colour_filling_with_4pts_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_4PTS_ALPHA_GRADIENT_AND_MASK_CCCN888),
            .ptFillLike = NULL,
        },
    },
};

/*---------------------------------------------------------------------------*
 * Colour Filling with 3 sample points Alpha Gradient and Mask               *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_3pts_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_3p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_cccn888_t tColour,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_3p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_CCCN888;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.wColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_3pts_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_3p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_cccn888_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_3p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_CCCN888;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.wColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_3pts_alpha_gradient_and_mask(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint32_t wColour,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio, q16XRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iHeight;
    } while(0);

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chTopRight 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iWidth;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatio;

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint32_t * __RESTRICT pwTargetLine = pwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = q16OpacityLeft + (x + tOffset.iX) * q16XRatio;
            nOpacity >>= 16;
            nOpacity = (nOpacity >= 255) * 255 + !(nOpacity >= 255) * nOpacity;
            nOpacity *= (nOpacity > 0);

            uint16_t hwAlpha = 256 - ((nOpacity * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pwTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_3pts_alpha_gradient_and_chn_mask(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint32_t wColour,
                            arm_2d_alpha_samples_3pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio, q16XRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottomLeft 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iHeight;
    } while(0);

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chTopRight 
                                   -   tSamplePoints.chTopLeft)) << 16) 
                  / iWidth;
    } while(0);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatio;

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint32_t * __RESTRICT pwTargetLine = pwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = q16OpacityLeft + (x + tOffset.iX) * q16XRatio;
            nOpacity >>= 16;
            nOpacity = (nOpacity >= 255) * 255 + !(nOpacity >= 255) * nOpacity;
            nOpacity *= (nOpacity > 0);

            uint16_t hwAlpha = 256 - (nOpacity * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pwTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_3pts_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_3p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_cccn888_fill_colour_with_3pts_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.wColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_cccn888_fill_colour_with_3pts_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.wColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_CCCN888,
                __arm_2d_cccn888_sw_colour_filling_with_3pts_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_3PTS_ALPHA_GRADIENT_AND_MASK_CCCN888),
            .ptFillLike = NULL,
        },
    },
};

/*---------------------------------------------------------------------------*
 * Colour Filling with Horizontal Alpha Gradient and Mask                    *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_horizontal_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_cccn888_t tColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_CCCN888;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.wColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_horizontal_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_cccn888_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_CCCN888;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.wColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_horizontal_alpha_gradient_and_mask(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint32_t wColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16XRatio;

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chRight 
                               -   tSamplePoints.chLeft)) << 16) 
                  / iWidth;
    } while(0);
    int32_t q16OpacityLeft = (int32_t)(tSamplePoints.chLeft) << 16;


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint32_t * __RESTRICT pwTargetLine = pwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - ((nOpacity * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pwTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_horizontal_alpha_gradient_and_chn_mask(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint32_t wColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16XRatio;

    /* calculate X Ratios */
    do {
        int16_t iWidth = ptTargetRegionOnVirtualScreen->tSize.iWidth;

        q16XRatio = (((int32_t)(   tSamplePoints.chRight 
                               -   tSamplePoints.chLeft)) << 16) 
                  / iWidth;
    } while(0);
    int32_t q16OpacityLeft = (int32_t)(tSamplePoints.chLeft) << 16;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint32_t * __RESTRICT pwTargetLine = pwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            int32_t nOpacity = (q16OpacityLeft + (x + tOffset.iX) * q16XRatio) >> 16;

            uint16_t hwAlpha = 256 - (nOpacity * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pwTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_horizontal_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_cccn888_fill_colour_with_horizontal_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.wColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_cccn888_fill_colour_with_horizontal_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.wColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_CCCN888,
                __arm_2d_cccn888_sw_colour_filling_with_horizontal_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_HORIZONTAL_ALPHA_GRADIENT_AND_MASK_CCCN888),
            .ptFillLike = NULL,
        },
    },
};


/*---------------------------------------------------------------------------*
 * Colour Filling with Vertical Alpha Gradient and Mask                      *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_vertical_alpha_gradient_and_mask(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_cccn888_t tColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_CCCN888;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.wColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_vertical_alpha_gradient_mask_and_opacity(  
                            arm_2d_fill_cl_2p_al_grd_msk_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_tile_t *ptMask,
                            arm_2d_color_cccn888_t tColour,
                            uint8_t chOpacity,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask, 
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

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_CCCN888;

    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    OPCODE.Source.ptTile = ptMask;
    OPCODE.wMode = 0;

    this.wColour = tColour.tValue;
    this.tSamplePoints = tSamplePoints;

    /* pre-process opacity */
    if (chOpacity < 255) {
        arm_foreach(uint8_t, this.tSamplePoints.chAlpha, pchAlpha) {
            *pchAlpha = __alpha_mix(*pchAlpha, chOpacity);
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_vertical_alpha_gradient_and_mask(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint32_t wColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottom 
                               -   tSamplePoints.chTop)) << 16) 
                  / iHeight;
    } while(0);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTop) << 16) 
                               + (y + tOffset.iY) * q16YRatio;
        q16OpacityLeft >>= 16;

        uint8_t *__RESTRICT pchMaskLine = pchMask;

        uint32_t * __RESTRICT pwTargetLine = pwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */

            uint16_t hwAlpha = 256 - ((q16OpacityLeft * (*pchMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pwTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}

__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_vertical_alpha_gradient_and_chn_mask(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint32_t *__RESTRICT pwMask,
                            int16_t iMaskStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint32_t wColour,
                            arm_2d_alpha_samples_2pts_t tSamplePoints)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;


    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |     
         |       |                                           |          |
         |       |                                           |          |     
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+     
     
         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    
    int32_t q16YRatio;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatio = (((int32_t)(   tSamplePoints.chBottom 
                               -   tSamplePoints.chTop)) << 16) 
                  / iHeight;
    } while(0);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTop) << 16) 
                               + (y + tOffset.iY) * q16YRatio;
        q16OpacityLeft >>= 16;

        uint32_t *__RESTRICT pwMaskLine = pwMask;

        uint32_t * __RESTRICT pwTargetLine = pwTarget;
        for (int_fast16_t x = 0; x < iWidth; x++) {

            /* calclulate opacity */
            uint16_t hwAlpha = 256 - (q16OpacityLeft * (*(uint8_t *)(pwMaskLine++)) >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            hwAlpha -= (hwAlpha == 1);
#endif

            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pwTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_vertical_alpha_gradient_and_mask( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_fill_cl_2p_al_grd_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    arm_2d_region_t tTargetRegion = {0};

    if (NULL == OPCODE.Target.ptRegion) {
        tTargetRegion.tSize = OPCODE.Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *OPCODE.Target.ptRegion;
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( OPCODE.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else

        __arm_2d_impl_cccn888_fill_colour_with_vertical_alpha_gradient_and_chn_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.wColour,
                            this.tSamplePoints);
    #endif
    } else {
        __arm_2d_impl_cccn888_fill_colour_with_vertical_alpha_gradient_and_mask( 
                            ptTask->Param.tCopy.tTarget.pBuffer,
                            ptTask->Param.tCopy.tTarget.iStride,
                            ptTask->Param.tCopy.tSource.pBuffer,                //!< alpha tile
                            ptTask->Param.tCopy.tSource.iStride,                //!< alpha tile
                            &(ptTask->Param.tCopy.tTarget.tValidRegionInVirtualScreen),
                            &tTargetRegion,
                            this.wColour,
                            this.tSamplePoints);
    }
    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_CCCN888,
                __arm_2d_cccn888_sw_colour_filling_with_vertical_alpha_gradient_and_mask);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_ALPHA_GRADIENT_AND_MASK_CCCN888),
            .ptFillLike = NULL,
        },
    },
};

#ifdef   __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */