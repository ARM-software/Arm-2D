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
 * Title:        __arm_2d_tile_fill_with_opacity.c
 * Description:  APIs for tile fill with opacity only
 *
 * $Date:        08. September 2025
 * $Revision:    V.1.3.1
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
#   pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144,1,64
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

__WEAK
void __arm_2d_impl_gray8_tile_fill_with_opacity(
                        uint8_t * __RESTRICT pchSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint8_t *__RESTRICT pchTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    hwOpacity += (hwOpacity == 255);
    uint16_t hwTransparency = 256 - hwOpacity;
    hwTransparency -= (hwTransparency == 1);

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {
    
        //! reset source
        uint8_t *__RESTRICT pchSource = pchSourceBase;  

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint8_t *__RESTRICT pchTarget = pchTargetBase;     
            
            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                uint8_t *__RESTRICT pchSrc = pchSource;

                for (int_fast16_t x = 0; x < wLength; x++) {
                
                    __ARM_2D_PIXEL_BLENDING_GRAY8(pchSrc++, pchTarget++, hwTransparency);
                }

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);
            
            /*---------------- Height Loop End----------------*/
            pchSource += iSourceStride;
            pchTargetBase += iTargetStride;
        
            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_gray8_with_opacity(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint8_t *__RESTRICT pchTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    hwOpacity += (hwOpacity == 255);
    uint16_t hwTransparency = 256 - hwOpacity;
    hwTransparency -= (hwTransparency == 1);

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {
    
        //! reset source
        uint32_t *__RESTRICT pwSource = pwSourceBase;  

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint8_t *__RESTRICT pchTarget = pchTargetBase;     
            
            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                uint32_t *__RESTRICT pwSrc = pwSource;

                for (int_fast16_t x = 0; x < wLength; x++) {
                
                    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(pwSrc++, pchTarget++, hwTransparency);
                }

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);
            
            /*---------------- Height Loop End----------------*/
            pwSource += iSourceStride;
            pchTargetBase += iTargetStride;
        
            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_gray8(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint8_t *__RESTRICT pchTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {
    
        //! reset source
        uint32_t *__RESTRICT pwSource = pwSourceBase;  

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint8_t *__RESTRICT pchTarget = pchTargetBase;     
            
            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                uint32_t *__RESTRICT pwSrc = pwSource;

                for (int_fast16_t x = 0; x < wLength; x++) {
                
                    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(pwSrc++, pchTarget++, 0);
                }

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);
            
            /*---------------- Height Loop End----------------*/
            pwSource += iSourceStride;
            pchTargetBase += iTargetStride;
        
            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}



ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_gray8_tile_fill_with_opacity_only(
                                        arm_2d_op_src_opc_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_src_opc_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_GRAY8:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_FILL_WITH_OPACITY_ONLY_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = ARM_2D_CP_MODE_FILL;
    this.chOpacity = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_opacity_only( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_src_opc_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    if (255 == this.chOpacity) {
        return __arm_2d_c8bit_sw_tile_fill(ptTask);
    }
#endif

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(this.Source.ptTile, NULL, NULL);
    assert(NULL != ptSourceRoot);
    if (ARM_2D_COLOUR_CCCA8888 == ptSourceRoot->tInfo.tColourInfo.chScheme) {

        __arm_2d_impl_ccca8888_tile_fill_to_gray8_with_opacity(
            ptTask->Param.tFill.tSource.pBuffer,
            ptTask->Param.tFill.tSource.iStride,
            &ptTask->Param.tFill.tSource.tValidRegion.tSize,

            ptTask->Param.tFill.tTarget.pBuffer,
            ptTask->Param.tFill.tTarget.iStride,
            &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
            this.chOpacity);

        return arm_fsm_rt_cpl;
    }
#endif

    __arm_2d_impl_gray8_tile_fill_with_opacity(
            ptTask->Param.tFill.tSource.pBuffer,
            ptTask->Param.tFill.tSource.iStride,
            &ptTask->Param.tFill.tSource.tValidRegion.tSize,

            ptTask->Param.tFill.tTarget.pBuffer,
            ptTask->Param.tFill.tTarget.iStride,
            &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
        this.chOpacity);

    return arm_fsm_rt_cpl;
}


__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_OPACITY_ONLY_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_opacity_only);

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_OPACITY_ONLY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour = true,
#endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_OPACITY_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_OPACITY_ONLY_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_OPACITY_ONLY_GRAY8),
        },
    },
};

__WEAK
void __arm_2d_impl_rgb565_tile_fill_with_opacity(
                        uint16_t * __RESTRICT phwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint16_t *__RESTRICT phwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    hwOpacity += (hwOpacity == 255);
    uint16_t hwTransparency = 256 - hwOpacity;
    hwTransparency -= (hwTransparency == 1);

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {
    
        //! reset source
        uint16_t *__RESTRICT phwSource = phwSourceBase;  

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint16_t *__RESTRICT phwTarget = phwTargetBase;     
            
            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                uint16_t *__RESTRICT phwSrc = phwSource;

                for (int_fast16_t x = 0; x < wLength; x++) {
                
                    __ARM_2D_PIXEL_BLENDING_RGB565(phwSrc++, phwTarget++, hwTransparency);
                }

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);
            
            /*---------------- Height Loop End----------------*/
            phwSource += iSourceStride;
            phwTargetBase += iTargetStride;
        
            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_rgb565_with_opacity(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint16_t *__RESTRICT phwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    hwOpacity += (hwOpacity == 255);
    uint16_t hwTransparency = 256 - hwOpacity;
    hwTransparency -= (hwTransparency == 1);

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {
    
        //! reset source
        uint32_t *__RESTRICT pwSource = pwSourceBase;  

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint16_t *__RESTRICT phwTarget = phwTargetBase;     
            
            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                uint32_t *__RESTRICT pwSrc = pwSource;

                for (int_fast16_t x = 0; x < wLength; x++) {
                
                    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(pwSrc++, phwTarget++, hwTransparency);
                }

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);
            
            /*---------------- Height Loop End----------------*/
            pwSource += iSourceStride;
            phwTargetBase += iTargetStride;
        
            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_rgb565(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint16_t *__RESTRICT phwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {
    
        //! reset source
        uint32_t *__RESTRICT pwSource = pwSourceBase;  

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint16_t *__RESTRICT phwTarget = phwTargetBase;     
            
            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                uint32_t *__RESTRICT pwSrc = pwSource;

                for (int_fast16_t x = 0; x < wLength; x++) {
                
                    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(pwSrc++, phwTarget++, 0);
                }

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);
            
            /*---------------- Height Loop End----------------*/
            pwSource += iSourceStride;
            phwTargetBase += iTargetStride;
        
            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}



ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb565_tile_fill_with_opacity_only(
                                        arm_2d_op_src_opc_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_src_opc_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_RGB565:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_FILL_WITH_OPACITY_ONLY_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = ARM_2D_CP_MODE_FILL;
    this.chOpacity = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_opacity_only( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_src_opc_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    if (255 == this.chOpacity) {
        return __arm_2d_rgb16_sw_tile_fill(ptTask);
    }
#endif

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(this.Source.ptTile, NULL, NULL);
    assert(NULL != ptSourceRoot);
    if (ARM_2D_COLOUR_CCCA8888 == ptSourceRoot->tInfo.tColourInfo.chScheme) {

        __arm_2d_impl_ccca8888_tile_fill_to_rgb565_with_opacity(
            ptTask->Param.tFill.tSource.pBuffer,
            ptTask->Param.tFill.tSource.iStride,
            &ptTask->Param.tFill.tSource.tValidRegion.tSize,

            ptTask->Param.tFill.tTarget.pBuffer,
            ptTask->Param.tFill.tTarget.iStride,
            &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
            this.chOpacity);

        return arm_fsm_rt_cpl;
    }
#endif

    __arm_2d_impl_rgb565_tile_fill_with_opacity(
            ptTask->Param.tFill.tSource.pBuffer,
            ptTask->Param.tFill.tSource.iStride,
            &ptTask->Param.tFill.tSource.tValidRegion.tSize,

            ptTask->Param.tFill.tTarget.pBuffer,
            ptTask->Param.tFill.tTarget.iStride,
            &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
        this.chOpacity);

    return arm_fsm_rt_cpl;
}


__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_OPACITY_ONLY_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_opacity_only);

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_OPACITY_ONLY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour = true,
#endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_OPACITY_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_OPACITY_ONLY_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_OPACITY_ONLY_RGB565),
        },
    },
};

__WEAK
void __arm_2d_impl_cccn888_tile_fill_with_opacity(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t *__RESTRICT pwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    hwOpacity += (hwOpacity == 255);
    uint16_t hwTransparency = 256 - hwOpacity;
    hwTransparency -= (hwTransparency == 1);

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {
    
        //! reset source
        uint32_t *__RESTRICT pwSource = pwSourceBase;  

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint32_t *__RESTRICT pwTarget = pwTargetBase;     
            
            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                uint32_t *__RESTRICT pwSrc = pwSource;

                for (int_fast16_t x = 0; x < wLength; x++) {
                
                    __ARM_2D_PIXEL_BLENDING_CCCN888(pwSrc++, pwTarget++, hwTransparency);
                }

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);
            
            /*---------------- Height Loop End----------------*/
            pwSource += iSourceStride;
            pwTargetBase += iTargetStride;
        
            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_cccn888_with_opacity(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t *__RESTRICT pwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    hwOpacity += (hwOpacity == 255);
    uint16_t hwTransparency = 256 - hwOpacity;
    hwTransparency -= (hwTransparency == 1);

    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {
    
        //! reset source
        uint32_t *__RESTRICT pwSource = pwSourceBase;  

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint32_t *__RESTRICT pwTarget = pwTargetBase;     
            
            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                uint32_t *__RESTRICT pwSrc = pwSource;

                for (int_fast16_t x = 0; x < wLength; x++) {
                
                    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(pwSrc++, pwTarget++, hwTransparency);
                }

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);
            
            /*---------------- Height Loop End----------------*/
            pwSource += iSourceStride;
            pwTargetBase += iTargetStride;
        
            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_cccn888(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t *__RESTRICT pwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {
    
        //! reset source
        uint32_t *__RESTRICT pwSource = pwSourceBase;  

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {
            uint32_t *__RESTRICT pwTarget = pwTargetBase;     
            
            /*---------------- Height Loop Begin----------------*/
            uint_fast32_t   wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t   wLength = MIN(wLengthLeft, ptSourceSize->iWidth);
                /*---------------- Width Loop Begin----------------*/

                uint32_t *__RESTRICT pwSrc = pwSource;

                for (int_fast16_t x = 0; x < wLength; x++) {
                
                    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(pwSrc++, pwTarget++, 0);
                }

                /*---------------- Width Loop End----------------*/
                wLengthLeft -= wLength;
            } while (wLengthLeft);
            
            /*---------------- Height Loop End----------------*/
            pwSource += iSourceStride;
            pwTargetBase += iTargetStride;
        
            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}



ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_cccn888_tile_fill_with_opacity_only(
                                        arm_2d_op_src_opc_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_src_opc_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_CCCN888:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_FILL_WITH_OPACITY_ONLY_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = ARM_2D_CP_MODE_FILL;
    this.chOpacity = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_opacity_only( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_src_opc_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    if (255 == this.chOpacity) {
        return __arm_2d_rgb32_sw_tile_fill(ptTask);
    }
#endif

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(this.Source.ptTile, NULL, NULL);
    assert(NULL != ptSourceRoot);
    if (ARM_2D_COLOUR_CCCA8888 == ptSourceRoot->tInfo.tColourInfo.chScheme) {

        __arm_2d_impl_ccca8888_tile_fill_to_cccn888_with_opacity(
            ptTask->Param.tFill.tSource.pBuffer,
            ptTask->Param.tFill.tSource.iStride,
            &ptTask->Param.tFill.tSource.tValidRegion.tSize,

            ptTask->Param.tFill.tTarget.pBuffer,
            ptTask->Param.tFill.tTarget.iStride,
            &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
            this.chOpacity);

        return arm_fsm_rt_cpl;
    }
#endif

    __arm_2d_impl_cccn888_tile_fill_with_opacity(
            ptTask->Param.tFill.tSource.pBuffer,
            ptTask->Param.tFill.tSource.iStride,
            &ptTask->Param.tFill.tSource.tValidRegion.tSize,

            ptTask->Param.tFill.tTarget.pBuffer,
            ptTask->Param.tFill.tTarget.iStride,
            &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
        this.chOpacity);

    return arm_fsm_rt_cpl;
}


__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_OPACITY_ONLY_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_opacity_only);

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_OPACITY_ONLY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour = true,
#endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_OPACITY_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_OPACITY_ONLY_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_OPACITY_ONLY_CCCN888),
        },
    },
};

#ifdef   __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */