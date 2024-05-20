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
 * Title:        __arm_2d_filter_iir_blur.c
 * Description:  APIs for IIR Blur
 *
 * $Date:        10. May 2024
 * $Revision:    V.1.1.0
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
#undef OP_CORE
#define OP_CORE this.use_as__arm_2d_op_t.use_as__arm_2d_op_core_t

#define OPCODE this.use_as__arm_2d_op_t



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
arm_fsm_rt_t arm_2dp_gray8_filter_iir_blur(  
                            arm_2d_filter_iir_blur_descriptor_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            uint8_t chBlurDegree)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_filter_iir_blur_descriptor_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    OP_CORE.ptOp = &ARM_2D_OP_FILTER_IIR_BLUR_GRAY8;
    
    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    this.chBlurDegree = chBlurDegree;

    if (NULL != (void *)this.tScratchMemory.pBuffer) {
        arm_2d_size_t tSize;
        if (NULL != ptRegion) {
            tSize = ptRegion->tSize;
        } else {
            tSize = ptTarget->tRegion.tSize;
        }

        size_t tSizeInByte = (tSize.iHeight + tSize.iWidth) * (sizeof(__arm_2d_iir_blur_acc_gray8_t));
        if (this.tScratchMemory.u24SizeInByte < tSizeInByte) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INSUFFICIENT_RESOURCE;
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

/* default low level implementation */
__WEAK
void __arm_2d_impl_gray8_filter_iir_blur(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint8_t chBlurDegree,
                            arm_2d_scratch_mem_t *ptScratchMemory)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;
  
    int16_t iY, iX;
    uint16_t hwRatio = 256 - chBlurDegree;
    __arm_2d_iir_blur_acc_gray8_t tAcc;
    __arm_2d_iir_blur_acc_gray8_t *ptStatusH = NULL;
    __arm_2d_iir_blur_acc_gray8_t *ptStatusV = NULL;
    uint8_t *pchChannel = NULL;

    if (NULL != (void *)(ptScratchMemory->pBuffer)) {
        ptStatusH = (__arm_2d_iir_blur_acc_gray8_t *)ptScratchMemory->pBuffer;
        ptStatusV = ptStatusH + ptTargetRegionOnVirtualScreen->tSize.iWidth;
    }

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
    
    uint8_t *pchPixel = pchTarget;

    if (NULL != ptStatusV) {
        /* rows direct path */
        ptStatusV += tOffset.iY;
    }

    for (iY = 0; iY < iHeight; iY++) {   

        if (NULL != ptStatusV && tOffset.iX > 0) {
            /* recover the previous statues */
            tAcc = *ptStatusV;
        } else {
            
            pchChannel = pchPixel;
            tAcc.hwC = *pchChannel;
    
        }

        pchChannel = pchPixel;

        for (iX = 0; iX < iWidth; iX++) {
            tAcc.hwC += ((*pchChannel) - tAcc.hwC) * hwRatio >> 8;  *pchChannel++ = tAcc.hwC;
            
        }

        if (NULL != ptStatusV) {
            /* save the last pixel */
            *ptStatusV++ = tAcc;
        }

        pchPixel +=iTargetStride;
          
    }

#if defined(__ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__)                    \
 && __ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__
    /* rows reverse path */
    
    pchPixel = &(pchTarget[(iWidth-1) + (iHeight-1)*iTargetStride]);
    
    for (iY = iHeight-1; iY > 0; iY--) {   
        
        /* initialize the accumulator */
        do {
            
            pchChannel = pchPixel;
            tAcc.hwC = *pchChannel;
    
        } while(0);

        pchChannel = pchPixel;

        for (iX = 0; iX < iWidth; iX++) {   
            tAcc.hwC += ((*pchChannel) - tAcc.hwC) * hwRatio >> 8;  *pchChannel++ = tAcc.hwC;
            pchChannel -= 2;
        }
        
        pchPixel -=iTargetStride;
    }
#endif 


    pchPixel = pchTarget;

    if (NULL != ptStatusH) {
        ptStatusH += tOffset.iX;
    }

    /* columns direct path */
    for (iX = 0; iX < iWidth; iX++) {     

        if (NULL != ptStatusH && tOffset.iY > 0) {
            /* recover the previous statues */
            tAcc = *ptStatusH;
        } else {
            
            pchChannel = pchPixel;
            tAcc.hwC = *pchChannel;
    
        }

        pchChannel = pchPixel;

        for (iY = 0; iY < iHeight; iY++) {
            tAcc.hwC += ((*pchChannel) - tAcc.hwC) * hwRatio >> 8;  *pchChannel++ = tAcc.hwC;
            pchChannel += iTargetStride - 1;
        }

        pchPixel++;

        if (NULL != ptStatusH) {
            /* save the last pixel */
            *ptStatusH++ = tAcc;
        }
    }

#if defined(__ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__)                    \
 && __ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__

    pchPixel = &(pchTarget[iWidth-1 + (iHeight-1)*iTargetStride]);

    /* columns reverse path */
    for (iX = iWidth-1; iX > 0; iX--)
    {   
        /* initialize the accumulator */
        do {
            
            pchChannel = pchPixel;
            tAcc.hwC = *pchChannel;
    
        } while(0);

        pchChannel = pchPixel;

        for (iY = 0; iY < iHeight; iY++) {   
            tAcc.hwC += ((*pchChannel) - tAcc.hwC) * hwRatio >> 8;  *pchChannel++ = tAcc.hwC;
            pchChannel -= 1 + iTargetStride;
        }

        pchPixel--;
    }
#endif  
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_gray8_sw_filter_iir_blur( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_filter_iir_blur_descriptor_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    arm_2d_region_t tTargetRegion = *this.use_as__arm_2d_op_t.Target.ptRegion;

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( this.use_as__arm_2d_op_t.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    __arm_2d_impl_gray8_filter_iir_blur( 
                        ptTask->Param.tTileProcess.pBuffer,
                        ptTask->Param.tTileProcess.iStride,
                        &(ptTask->Param.tTileProcess.tValidRegionInVirtualScreen),
                        &tTargetRegion,
                        this.chBlurDegree,
                        &this.tScratchMemory);

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILTER_IIR_BLUR_GRAY8,
                __arm_2d_gray8_sw_filter_iir_blur);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILTER_IIR_BLUR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILETER_IIR_BLUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_FILTER_IIR_BLUR_GRAY8),
        },
    },
};

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_filter_iir_blur(  
                            arm_2d_filter_iir_blur_descriptor_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            uint8_t chBlurDegree)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_filter_iir_blur_descriptor_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    OP_CORE.ptOp = &ARM_2D_OP_FILTER_IIR_BLUR_RGB565;
    
    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    this.chBlurDegree = chBlurDegree;

    if (NULL != (void *)this.tScratchMemory.pBuffer) {
        arm_2d_size_t tSize;
        if (NULL != ptRegion) {
            tSize = ptRegion->tSize;
        } else {
            tSize = ptTarget->tRegion.tSize;
        }

        size_t tSizeInByte = (tSize.iHeight + tSize.iWidth) * (sizeof(__arm_2d_iir_blur_acc_rgb565_t));
        if (this.tScratchMemory.u24SizeInByte < tSizeInByte) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INSUFFICIENT_RESOURCE;
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_filter_iir_blur(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint8_t chBlurDegree,
                            arm_2d_scratch_mem_t *ptScratchMemory)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;
  
    int16_t iY, iX;
    uint16_t hwRatio = 256 - chBlurDegree;
    __arm_2d_iir_blur_acc_rgb565_t tAcc;
    __arm_2d_iir_blur_acc_rgb565_t *ptStatusH = NULL;
    __arm_2d_iir_blur_acc_rgb565_t *ptStatusV = NULL;
    __arm_2d_color_fast_rgb_t tPixel;

    if (NULL != (void *)(ptScratchMemory->pBuffer)) {
        ptStatusH = (__arm_2d_iir_blur_acc_rgb565_t *)ptScratchMemory->pBuffer;
        ptStatusV = ptStatusH + ptTargetRegionOnVirtualScreen->tSize.iWidth;
    }

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
    
    uint16_t *phwPixel = phwTarget;

    if (NULL != ptStatusV) {
        /* rows direct path */
        ptStatusV += tOffset.iY;
    }

    for (iY = 0; iY < iHeight; iY++) {   

        if (NULL != ptStatusV && tOffset.iX > 0) {
            /* recover the previous statues */
            tAcc = *ptStatusV;
        } else {
            
            __arm_2d_rgb565_unpack(*phwPixel, &tPixel);
            tAcc = *(__arm_2d_iir_blur_acc_rgb565_t *)&tPixel;
    
        }

        uint16_t *phwTargetPixel = phwPixel;

        for (iX = 0; iX < iWidth; iX++) {
            
            __arm_2d_rgb565_unpack(*phwTargetPixel, &tPixel);

            tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;  tPixel.B = tAcc.hwB; 
            tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;  tPixel.G = tAcc.hwG;
            tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;  tPixel.R = tAcc.hwR;

            *phwTargetPixel = __arm_2d_rgb565_pack(&tPixel);
    
            phwTargetPixel++;
        }

        if (NULL != ptStatusV) {
            /* save the last pixel */
            *ptStatusV++ = tAcc;
        }

        phwPixel +=iTargetStride;
          
    }

#if defined(__ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__)                    \
 && __ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__
    /* rows reverse path */
    
    phwPixel = &(phwTarget[(iWidth-1) + (iHeight-1)*iTargetStride]);
    
    for (iY = iHeight-1; iY > 0; iY--) {   
        
        /* initialize the accumulator */
        do {
            
            __arm_2d_rgb565_unpack(*phwPixel, &tPixel);
            tAcc = *(__arm_2d_iir_blur_acc_rgb565_t *)&tPixel;
    
        } while(0);

        uint16_t *phwTargetPixel = phwPixel;

        for (iX = 0; iX < iWidth; iX++) {   
            
            __arm_2d_rgb565_unpack(*phwTargetPixel, &tPixel);

            tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;  tPixel.B = tAcc.hwB; 
            tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;  tPixel.G = tAcc.hwG;
            tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;  tPixel.R = tAcc.hwR;

            *phwTargetPixel = __arm_2d_rgb565_pack(&tPixel);
    
            phwTargetPixel--;
        }
        
        phwPixel -=iTargetStride;
    }
#endif 


    phwPixel = phwTarget;

    if (NULL != ptStatusH) {
        ptStatusH += tOffset.iX;
    }

    /* columns direct path */
    for (iX = 0; iX < iWidth; iX++) {     

        if (NULL != ptStatusH && tOffset.iY > 0) {
            /* recover the previous statues */
            tAcc = *ptStatusH;
        } else {
            
            __arm_2d_rgb565_unpack(*phwPixel, &tPixel);
            tAcc = *(__arm_2d_iir_blur_acc_rgb565_t *)&tPixel;
    
        }

        uint16_t *phwTargetPixel = phwPixel;

        for (iY = 0; iY < iHeight; iY++) {
            
            __arm_2d_rgb565_unpack(*phwTargetPixel, &tPixel);

            tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;  tPixel.B = tAcc.hwB; 
            tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;  tPixel.G = tAcc.hwG;
            tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;  tPixel.R = tAcc.hwR;

            *phwTargetPixel = __arm_2d_rgb565_pack(&tPixel);
    
            phwTargetPixel += iTargetStride;
        }

        phwPixel++;

        if (NULL != ptStatusH) {
            /* save the last pixel */
            *ptStatusH++ = tAcc;
        }
    }

#if defined(__ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__)                    \
 && __ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__

    phwPixel = &(phwTarget[iWidth-1 + (iHeight-1)*iTargetStride]);

    /* columns reverse path */
    for (iX = iWidth-1; iX > 0; iX--)
    {   
        /* initialize the accumulator */
        do {
            
            __arm_2d_rgb565_unpack(*phwPixel, &tPixel);
            tAcc = *(__arm_2d_iir_blur_acc_rgb565_t *)&tPixel;
    
        } while(0);

        uint16_t *phwTargetPixel = phwPixel;

        for (iY = 0; iY < iHeight; iY++) {   
            
            __arm_2d_rgb565_unpack(*phwTargetPixel, &tPixel);

            tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;  tPixel.B = tAcc.hwB; 
            tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;  tPixel.G = tAcc.hwG;
            tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;  tPixel.R = tAcc.hwR;

            *phwTargetPixel = __arm_2d_rgb565_pack(&tPixel);
    
            phwTargetPixel -= iTargetStride;
        }

        phwPixel--;
    }
#endif  
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_rgb565_sw_filter_iir_blur( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_filter_iir_blur_descriptor_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    arm_2d_region_t tTargetRegion = *this.use_as__arm_2d_op_t.Target.ptRegion;

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( this.use_as__arm_2d_op_t.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    __arm_2d_impl_rgb565_filter_iir_blur( 
                        ptTask->Param.tTileProcess.pBuffer,
                        ptTask->Param.tTileProcess.iStride,
                        &(ptTask->Param.tTileProcess.tValidRegionInVirtualScreen),
                        &tTargetRegion,
                        this.chBlurDegree,
                        &this.tScratchMemory);

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILTER_IIR_BLUR_RGB565,
                __arm_2d_rgb565_sw_filter_iir_blur);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILTER_IIR_BLUR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILETER_IIR_BLUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_FILTER_IIR_BLUR_RGB565),
        },
    },
};

/*
 * the Frontend API
 */

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_filter_iir_blur(  
                            arm_2d_filter_iir_blur_descriptor_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            uint8_t chBlurDegree)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_filter_iir_blur_descriptor_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    OP_CORE.ptOp = &ARM_2D_OP_FILTER_IIR_BLUR_CCCN888;
    
    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    this.chBlurDegree = chBlurDegree;

    if (NULL != (void *)this.tScratchMemory.pBuffer) {
        arm_2d_size_t tSize;
        if (NULL != ptRegion) {
            tSize = ptRegion->tSize;
        } else {
            tSize = ptTarget->tRegion.tSize;
        }

        size_t tSizeInByte = (tSize.iHeight + tSize.iWidth) * (sizeof(__arm_2d_iir_blur_acc_cccn888_t));
        if (this.tScratchMemory.u24SizeInByte < tSizeInByte) {
            return (arm_fsm_rt_t)ARM_2D_ERR_INSUFFICIENT_RESOURCE;
        }
    }

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

/* default low level implementation */
__WEAK
void __arm_2d_impl_cccn888_filter_iir_blur(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                            uint8_t chBlurDegree,
                            arm_2d_scratch_mem_t *ptScratchMemory)
{
    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;
  
    int16_t iY, iX;
    uint16_t hwRatio = 256 - chBlurDegree;
    __arm_2d_iir_blur_acc_cccn888_t tAcc;
    __arm_2d_iir_blur_acc_cccn888_t *ptStatusH = NULL;
    __arm_2d_iir_blur_acc_cccn888_t *ptStatusV = NULL;
    uint8_t *pchChannel = NULL;

    if (NULL != (void *)(ptScratchMemory->pBuffer)) {
        ptStatusH = (__arm_2d_iir_blur_acc_cccn888_t *)ptScratchMemory->pBuffer;
        ptStatusV = ptStatusH + ptTargetRegionOnVirtualScreen->tSize.iWidth;
    }

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
    
    uint32_t *pwPixel = pwTarget;

    if (NULL != ptStatusV) {
        /* rows direct path */
        ptStatusV += tOffset.iY;
    }

    for (iY = 0; iY < iHeight; iY++) {   

        if (NULL != ptStatusV && tOffset.iX > 0) {
            /* recover the previous statues */
            tAcc = *ptStatusV;
        } else {
            
            pchChannel = (uint8_t *)pwPixel;
            tAcc.hwB = *pchChannel++;
            tAcc.hwG = *pchChannel++;
            tAcc.hwR = *pchChannel++;
    
        }

        pchChannel = (uint8_t *)pwPixel;

        for (iX = 0; iX < iWidth; iX++) {
            
            tAcc.hwB += ((*pchChannel) - tAcc.hwB) * hwRatio >> 8;  *pchChannel++ = tAcc.hwB; 
            tAcc.hwG += ((*pchChannel) - tAcc.hwG) * hwRatio >> 8;  *pchChannel++ = tAcc.hwG;
            tAcc.hwR += ((*pchChannel) - tAcc.hwR) * hwRatio >> 8;  *pchChannel++ = tAcc.hwR;
    
            pchChannel++;
        }

        if (NULL != ptStatusV) {
            /* save the last pixel */
            *ptStatusV++ = tAcc;
        }

        pwPixel +=iTargetStride;
          
    }

#if defined(__ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__)                    \
 && __ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__
    /* rows reverse path */
    
    pwPixel = &(pwTarget[(iWidth-1) + (iHeight-1)*iTargetStride]);
    
    for (iY = iHeight-1; iY > 0; iY--) {   
        
        /* initialize the accumulator */
        do {
            
            pchChannel = (uint8_t *)pwPixel;
            tAcc.hwB = *pchChannel++;
            tAcc.hwG = *pchChannel++;
            tAcc.hwR = *pchChannel++;
    
        } while(0);

        pchChannel = (uint8_t *)pwPixel;

        for (iX = 0; iX < iWidth; iX++) {   
            
            tAcc.hwB += ((*pchChannel) - tAcc.hwB) * hwRatio >> 8;  *pchChannel++ = tAcc.hwB; 
            tAcc.hwG += ((*pchChannel) - tAcc.hwG) * hwRatio >> 8;  *pchChannel++ = tAcc.hwG;
            tAcc.hwR += ((*pchChannel) - tAcc.hwR) * hwRatio >> 8;  *pchChannel++ = tAcc.hwR;
    
            pchChannel -= 7;
        }
        
        pwPixel -=iTargetStride;
    }
#endif 


    pwPixel = pwTarget;

    if (NULL != ptStatusH) {
        ptStatusH += tOffset.iX;
    }

    /* columns direct path */
    for (iX = 0; iX < iWidth; iX++) {     

        if (NULL != ptStatusH && tOffset.iY > 0) {
            /* recover the previous statues */
            tAcc = *ptStatusH;
        } else {
            
            pchChannel = (uint8_t *)pwPixel;
            tAcc.hwB = *pchChannel++;
            tAcc.hwG = *pchChannel++;
            tAcc.hwR = *pchChannel++;
    
        }

        pchChannel = (uint8_t *)pwPixel;

        for (iY = 0; iY < iHeight; iY++) {
            
            tAcc.hwB += ((*pchChannel) - tAcc.hwB) * hwRatio >> 8;  *pchChannel++ = tAcc.hwB; 
            tAcc.hwG += ((*pchChannel) - tAcc.hwG) * hwRatio >> 8;  *pchChannel++ = tAcc.hwG;
            tAcc.hwR += ((*pchChannel) - tAcc.hwR) * hwRatio >> 8;  *pchChannel++ = tAcc.hwR;
    
            pchChannel += (iTargetStride * sizeof(uint32_t)) - 3;
        }

        pwPixel++;

        if (NULL != ptStatusH) {
            /* save the last pixel */
            *ptStatusH++ = tAcc;
        }
    }

#if defined(__ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__)                    \
 && __ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__

    pwPixel = &(pwTarget[iWidth-1 + (iHeight-1)*iTargetStride]);

    /* columns reverse path */
    for (iX = iWidth-1; iX > 0; iX--)
    {   
        /* initialize the accumulator */
        do {
            
            pchChannel = (uint8_t *)pwPixel;
            tAcc.hwB = *pchChannel++;
            tAcc.hwG = *pchChannel++;
            tAcc.hwR = *pchChannel++;
    
        } while(0);

        pchChannel = (uint8_t *)pwPixel;

        for (iY = 0; iY < iHeight; iY++) {   
            
            tAcc.hwB += ((*pchChannel) - tAcc.hwB) * hwRatio >> 8;  *pchChannel++ = tAcc.hwB; 
            tAcc.hwG += ((*pchChannel) - tAcc.hwG) * hwRatio >> 8;  *pchChannel++ = tAcc.hwG;
            tAcc.hwR += ((*pchChannel) - tAcc.hwR) * hwRatio >> 8;  *pchChannel++ = tAcc.hwR;
    
            pchChannel -= 3 + (iTargetStride * sizeof(uint32_t));
        }

        pwPixel--;
    }
#endif  
}

/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_cccn888_sw_filter_iir_blur( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_filter_iir_blur_descriptor_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    arm_2d_region_t tTargetRegion = *this.use_as__arm_2d_op_t.Target.ptRegion;

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( this.use_as__arm_2d_op_t.Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    __arm_2d_impl_cccn888_filter_iir_blur( 
                        ptTask->Param.tTileProcess.pBuffer,
                        ptTask->Param.tTileProcess.iStride,
                        &(ptTask->Param.tTileProcess.tValidRegionInVirtualScreen),
                        &tTargetRegion,
                        this.chBlurDegree,
                        &this.tScratchMemory);

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILTER_IIR_BLUR_CCCN888,
                __arm_2d_cccn888_sw_filter_iir_blur);      /* Default SW Implementation */


/*
 * OPCODE
 */

const __arm_2d_op_info_t ARM_2D_OP_FILTER_IIR_BLUR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILETER_IIR_BLUR,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_FILTER_IIR_BLUR_CCCN888),
        },
    },
};

#ifdef   __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */
