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
* Title:        __arm_2d_filter_iir_blur_helium.c
* Description:  APIs for IIR Blur
*
* $Date:        27. April 2025
* $Revision:    V.2.0.1
*
* Target Processor:  Cortex-M cores with helium
*
* -------------------------------------------------------------------- */


/*============================ INCLUDES ======================================*/

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wempty-translation-unit"
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
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#pragma diag_suppress 174,177,188,68,513,144
#elif defined(__IS_COMPILER_GCC__)
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

/*============================ MACROS ========================================*/

/* /!\ Scratch memory dimenensions must be mutiple of for Helium */
#define ROUND_UP_8(x) (((x) + 7) & ~7)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_gray8_filter_iir_blur) (
                    uint8_t * __RESTRICT pchTarget,
                    int16_t iTargetStride,
                    arm_2d_region_t * __RESTRICT ptValidRegionOnVirtualScreen,
                    arm_2d_region_t * ptTargetRegionOnVirtualScreen,
                    uint8_t chBlurDegree, 
                    arm_2d_filter_iir_blur_descriptor_t *ptThis)
{
    arm_2d_scratch_mem_t *ptScratchMemory = &this.tScratchMemory;
    int_fast16_t    iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t    iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;

    if (0 == chBlurDegree) {
        return ;
    }

    int32_t         iY, iX;
    /* pre-scaled ratio to take into account doubling + high-part extraction of vqdmulhq */
    int16_t        hwRatio = (256 - chBlurDegree) << 7;

    __arm_2d_iir_blur_acc_gray8_t *ptStatusH = NULL;
    __arm_2d_iir_blur_acc_gray8_t *ptStatusV = NULL;
    int16x8_t       vacc;

#if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
    if (NULL != (void *) (ptScratchMemory->pBuffer)) {
        ptStatusH = (__arm_2d_iir_blur_acc_gray8_t *) ptScratchMemory->pBuffer;
        ptStatusV = ptStatusH + ROUND_UP_8(ptTargetRegionOnVirtualScreen->tSize.iWidth);
    }
#endif

    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

#if defined(__ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__)                    \
 && __ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__
    const bool bAllowReversePath = (1 == arm_2d_is_region_inside_target(
                                                ptTargetRegionOnVirtualScreen,
                                                ptValidRegionOnVirtualScreen));
#else
    const bool bAllowReversePath = false;
#endif

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
    if (this.bForwardHorizontal) {
        uint8_t *pchPixel = pchTarget;

    #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
        if (NULL != ptStatusV) {
            /* rows direct path */
            ptStatusV += tOffset.iY;
        }
    #endif

        uint16x8_t vstride = vidupq_n_u16(0, 1);

        vstride = vstride * iTargetStride;

        for (iY = 0; iY < iHeight / 8; iY++) {
            uint16x8_t      voffs = vstride;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV && tOffset.iX > 0) {
                /* recover the previous accumulators */
                vacc = vld1q((uint16_t *) ptStatusV);
            } else 
        #endif
            {
                vacc = vldrbq_gather_offset_u16(pchPixel, voffs);
            }

            for (iX = 0; iX < iWidth; iX++) {
                uint16x8_t      in = vldrbq_gather_offset_u16(pchPixel, voffs);
                int16x8_t       vdiff = vsubq_s16(in, vacc);

                vacc = vaddq_s16(vacc, vqdmulhq(vdiff, hwRatio));

                vstrbq_scatter_offset_u16(pchPixel, voffs, vacc);
                voffs += 1;
            }

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV) {
                /* save the last pixels */
                vst1q((int16_t *) ptStatusV, vacc);
                ptStatusV += 8;
            }
        #endif

            pchPixel += (iTargetStride * 8);
        }

        /* residue for non-multiple of 8 height dimension */
        if (iHeight & 7) {
            uint16x8_t      voffs = vstride;
            mve_pred16_t    tailPred = vctp16q(iHeight & 7);

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV && tOffset.iX > 0) {
                /* recover the previous accumulators */
                vacc = vld1q((uint16_t *) ptStatusV);
            } else 
        #endif
            {
                vacc = vldrbq_gather_offset_u16(pchPixel, voffs);
            }

            for (iX = 0; iX < iWidth; iX++) {
                uint16x8_t      in = vldrbq_gather_offset_u16(pchPixel, voffs);
                int16x8_t       vdiff = vsubq_s16(in, vacc);
                vacc = vaddq_s16(vacc, vqdmulhq(vdiff, hwRatio));

                vstrbq_scatter_offset_p_u16(pchPixel, voffs, vacc, tailPred);
                voffs += 1;
            }

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV) {
                /* save the last pixels */
                vst1q((int16_t *) ptStatusV, vacc);
            }
        #endif
        }
    }

    /* rows reverse path */
    if (this.bReverseHorizontal && bAllowReversePath) {

        uint8_t *pchPixel = &(pchTarget[(iHeight - 1 - 7)*iTargetStride]);

        uint16x8_t vstride = vidupq_n_u16(1, 1);
        vstride = vstride * iTargetStride;
        vstride -= 1;

        for (iY = 0; iY < iHeight / 8; iY++) {
            uint16x8_t voffs = vstride;

            vacc = vldrbq_gather_offset_u16(pchPixel, voffs);

            for (iX = 0; iX < iWidth; iX++) {
                uint16x8_t      in = vldrbq_gather_offset_u16(pchPixel, voffs);
                int16x8_t       vdiff = vsubq_s16(in, vacc);

                vacc = vaddq_s16(vacc, vqdmulhq(vdiff, hwRatio));

                vstrbq_scatter_offset_u16(pchPixel, voffs, vacc);
                voffs -= 1;
            }

            pchPixel -= (iTargetStride * 8);
        }

        /* residue for non-multiple of 8 height dimension */
        if (iHeight & 7) {
            int16x8_t voffs = vstride;
            mve_pred16_t tailPred = vctp16q(iHeight & 7);

            vacc = vldrbq_gather_offset_z_u16(pchPixel, voffs, tailPred);

            for (iX = 0; iX < iWidth; iX++) {
                uint16x8_t in = vldrbq_gather_offset_z_u16(pchPixel, voffs, tailPred);
                int16x8_t vdiff = vsubq_s16(in, vacc);
                vacc = vaddq_s16(vacc, vqdmulhq(vdiff, hwRatio));

                vstrbq_scatter_offset_p_u16(pchPixel, voffs, vacc, tailPred);
                voffs -= 1;
            }

        }
    }

    if (this.bForwardVertical) {
        uint8_t *pchPixel = pchTarget;

    #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
        if (NULL != ptStatusH) {
            ptStatusH += tOffset.iX;
        }
    #endif

        /* columns direct path */
        for (iX = 0; iX < iWidth / 8; iX++) {
            uint8_t *pchChannel = pchPixel;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH && tOffset.iY > 0) {
                /* recover the previous accumulators */ ;
                vacc = vld1q((uint16_t *) ptStatusH);
            } else
        #endif
            {
                vacc = vldrbq_u16(pchChannel);
            }

            for (iY = 0; iY < iHeight; iY++) {
                uint16x8_t      in = vldrbq_u16(pchChannel);
                int16x8_t       vdiff = vsubq_s16(in, vacc);
                vacc = vaddq_s16(vacc, vqdmulhq(vdiff, hwRatio));

                vstrbq_u16(pchChannel, vacc);
                pchChannel += iTargetStride;
            }

            pchPixel += 8;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH) {
                /* save the last pixels */
                vst1q((int16_t *) ptStatusH, vacc);
                ptStatusH += 8;
            }
        #endif
        }

        /* residue for non-multiple of 8 columns dimension */
        if (iWidth & 7) {
            mve_pred16_t    tailPred = vctp16q(iWidth & 7);
            uint8_t        *pchChannel = pchPixel;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH && tOffset.iY > 0) {
                /* recover the previous accumulators */ ;
                vacc = vld1q((uint16_t *) ptStatusH);
            } else 
        #endif
            {
                vacc = vldrbq_u16(pchChannel);
            }

            for (iY = 0; iY < iHeight; iY++) {
                uint16x8_t      in = vldrbq_u16(pchChannel);
                int16x8_t       vdiff = vsubq_s16(in, vacc);
                vacc = vaddq_s16(vacc, vqdmulhq(vdiff, hwRatio));

                vstrbq_p_u16(pchChannel, vacc, tailPred);
                pchChannel += iTargetStride;
            }
        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH) {
                /* save the last pixels */
                vst1q((int16_t *) ptStatusH, vacc);
            }
        #endif
        }
    }

    if (this.bReverseVertical && bAllowReversePath) {
        uint8_t *pchPixel = &(pchTarget[iWidth-1 - 7 + (iHeight-1)*iTargetStride]);

        /* columns direct path */
        for (iX = 0; iX < iWidth / 8; iX++) {
            uint8_t *pchChannel = pchPixel;

            vacc = vldrbq_u16(pchChannel);

            for (iY = 0; iY < iHeight; iY++) {
                uint16x8_t in = vldrbq_u16(pchChannel);
                int16x8_t vdiff = vsubq_s16(in, vacc);
                vacc = vaddq_s16(vacc, vqdmulhq(vdiff, hwRatio));

                vstrbq_u16(pchChannel, vacc);
                pchChannel -= iTargetStride;
            }

            pchPixel -= 8;
        }

        /* residue for non-multiple of 8 columns dimension */
        if (iWidth & 7) {
            mve_pred16_t tailPred = vctp16q(iWidth & 7);
            uint8_t *pchChannel = pchPixel;

            vacc = vldrbq_z_u16(pchChannel, tailPred);

            for (iY = 0; iY < iHeight; iY++) {
                uint16x8_t in = vldrbq_z_u16(pchChannel, tailPred);
                int16x8_t vdiff = vsubq_s16(in, vacc);
                vacc = vaddq_s16(vacc, vqdmulhq(vdiff, hwRatio));

                vstrbq_p_u16(pchChannel, vacc, tailPred);
                pchChannel -= iTargetStride;
            }
        }
    }
}

#if !defined(__ARM_2D_CFG_UNSAFE_NO_HELIUM_RGB565_PATCH_IN_IIR_BLUR__)
#   define __arm_2d_rgb565_unpack_single_vec    __arm_2d_rgb565_unpack_single_vec_comp
#endif


__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_rgb565_filter_iir_blur) (
                    uint16_t * __RESTRICT phwTarget,
                    int16_t iTargetStride,
                    arm_2d_region_t * __RESTRICT ptValidRegionOnVirtualScreen,
                    arm_2d_region_t * ptTargetRegionOnVirtualScreen,
                    uint8_t chBlurDegree, 
                    arm_2d_filter_iir_blur_descriptor_t *ptThis)
{
    arm_2d_scratch_mem_t *ptScratchMemory = &this.tScratchMemory;
    int_fast16_t    iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t    iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;

    if (0 == chBlurDegree) {
        return ;
    }

    int32_t         iY, iX;
    /* pre-scaled ratio to take into account doubling + high-part extraction of vqdmulhq */
    int16_t        hwRatio = (256 - chBlurDegree) << 7;

    __arm_2d_iir_blur_acc_rgb565_t *ptStatusH = NULL;
    __arm_2d_iir_blur_acc_rgb565_t *ptStatusV = NULL;

    int16_t       *pAccBase = NULL;
    int16x8_t      vaccR, vaccG, vaccB;

#if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
    if (NULL != (void *) (ptScratchMemory->pBuffer)) {
        ptStatusH = (__arm_2d_iir_blur_acc_rgb565_t *) ptScratchMemory->pBuffer;
        ptStatusV = ptStatusH + ROUND_UP_8(ptTargetRegionOnVirtualScreen->tSize.iWidth);
    }
#endif

    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

#if defined(__ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__)                    \
 && __ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__
    const bool bAllowReversePath = (1 == arm_2d_is_region_inside_target(
                                                ptTargetRegionOnVirtualScreen,
                                                ptValidRegionOnVirtualScreen));
#else
    const bool bAllowReversePath = false;
#endif
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

    /* left to right, top to down process */
    if (this.bForwardHorizontal) {
        uint16_t *phwPixel = phwTarget;

    #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
        if (NULL != ptStatusV) {
            /* rows direct path */
            ptStatusV += tOffset.iY;
        }
    #endif

        uint16x8_t      vstride = vidupq_n_u16(0, 1);
        vstride = vstride * iTargetStride;

        for (iY = 0; iY < iHeight / 8; iY++) {
            uint16x8_t      voffs = vstride;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV && tOffset.iX > 0) {
                /* recover the previous accumulators */
                pAccBase = (int16_t *) ptStatusV;
                vaccR = vld1q(pAccBase);
                vaccG = vld1q(pAccBase + 8);
                vaccB = vld1q(pAccBase + 16);
            } else 
        #endif
            {
                int16x8_t       vacc = vldrhq_gather_shifted_offset_u16(phwPixel, voffs);
                __arm_2d_rgb565_unpack_single_vec(vacc, (uint16x8_t *)&vaccR, (uint16x8_t *)&vaccG, (uint16x8_t *)&vaccB);
            }


            for (iX = 0; iX < iWidth; iX++) {
                uint16x8_t      in = vldrhq_gather_shifted_offset_u16(phwPixel, voffs);
                uint16x8_t      vR, vG, vB;

                __arm_2d_rgb565_unpack_single_vec(in, &vR, &vG, &vB);

                int16x8_t       vdiffR = vsubq_s16(vR, vaccR);
                int16x8_t       vdiffG = vsubq_s16(vG, vaccG);
                int16x8_t       vdiffB = vsubq_s16(vB, vaccB);

                vaccR += vqdmulhq(vdiffR, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccB += vqdmulhq(vdiffB, hwRatio);

                vstrhq_scatter_shifted_offset_u16(phwPixel, voffs, __arm_2d_rgb565_pack_single_vec(vaccR, vaccG, vaccB));
                voffs += 1;
            }

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV) {
                /* save the last pixels */
                pAccBase = (int16_t *) ptStatusV;
                vst1q(pAccBase, vaccR);
                vst1q(pAccBase + 8, vaccG);
                vst1q(pAccBase + 16, vaccB);
                ptStatusV += 8;
            }
        #endif

            phwPixel += (iTargetStride * 8);
        }

        if (iHeight & 7) {
            uint16x8_t      voffs = vstride;
            mve_pred16_t    tailPred = vctp16q(iHeight & 7);

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV && tOffset.iX > 0) {
                /* recover the previous values */
                pAccBase = (int16_t *) ptStatusV;
                vaccR = vld1q(pAccBase);
                vaccG = vld1q(pAccBase + 8);
                vaccB = vld1q(pAccBase + 16);

            } else 
        #endif
            {
                int16x8_t       vacc = vldrhq_gather_shifted_offset_u16(phwPixel, voffs);

                __arm_2d_rgb565_unpack_single_vec(vacc, (uint16x8_t *)&vaccR, (uint16x8_t *)&vaccG, (uint16x8_t *)&vaccB);
            }


            for (iX = 0; iX < iWidth; iX++) {
                uint16x8_t      in = vldrhq_gather_shifted_offset_u16(phwPixel, voffs);
                uint16x8_t      vR, vG, vB;

                __arm_2d_rgb565_unpack_single_vec(in, &vR, &vG, &vB);

                int16x8_t       vdiffR = vsubq_s16(vR, vaccR);
                int16x8_t       vdiffG = vsubq_s16(vG, vaccG);
                int16x8_t       vdiffB = vsubq_s16(vB, vaccB);

                vaccR += vqdmulhq(vdiffR, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccB += vqdmulhq(vdiffB, hwRatio);

                vstrhq_scatter_shifted_offset_p_u16(phwPixel, voffs, __arm_2d_rgb565_pack_single_vec(vaccR, vaccG, vaccB), tailPred);
                voffs += 1;

            }
        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV) {
                /* save the last pixels */
                pAccBase = (int16_t *) ptStatusV;
                vst1q(pAccBase, vaccR);
                vst1q(pAccBase + 8, vaccG);
                vst1q(pAccBase + 16, vaccB);
            }
        #endif
        }
    }

    /* right to left, down to top process */
    if (this.bReverseHorizontal && bAllowReversePath) {
        uint16_t *phwPixel = &(phwTarget[(iHeight - 1 - 7)*iTargetStride]);

        uint16x8_t vstride = vidupq_n_u16(1, 1);
        vstride = vstride * iTargetStride;
        vstride -= 1;

        for (iY = 0; iY < iHeight / 8; iY++) {
            uint16x8_t voffs = vstride;

            __arm_2d_rgb565_unpack_single_vec(  
                            vldrhq_gather_shifted_offset_u16(phwPixel, voffs), 
                            (uint16x8_t *)&vaccR, 
                            (uint16x8_t *)&vaccG, 
                            (uint16x8_t *)&vaccB);

            for (iX = 0; iX < iWidth; iX++) {
                uint16x8_t      in = vldrhq_gather_shifted_offset_u16(phwPixel, voffs);
                uint16x8_t      vR, vG, vB;

                __arm_2d_rgb565_unpack_single_vec(in, &vR, &vG, &vB);

                int16x8_t       vdiffR = vsubq_s16(vR, vaccR);
                int16x8_t       vdiffG = vsubq_s16(vG, vaccG);
                int16x8_t       vdiffB = vsubq_s16(vB, vaccB);

                vaccR += vqdmulhq(vdiffR, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccB += vqdmulhq(vdiffB, hwRatio);

                vstrhq_scatter_shifted_offset_u16(  
                        phwPixel, 
                        voffs, 
                        __arm_2d_rgb565_pack_single_vec(vaccR, vaccG, vaccB));
                voffs = voffs - 1;
            }

            phwPixel -= (iTargetStride * 8);
        }

        if (iHeight & 7) {
            uint16x8_t      voffs = vstride;
            mve_pred16_t    tailPred = vctp16q(iHeight & 7);

            __arm_2d_rgb565_unpack_single_vec(  
                            vldrhq_gather_shifted_offset_z_u16(phwPixel, voffs, tailPred), 
                            (uint16x8_t *)&vaccR, 
                            (uint16x8_t *)&vaccG, 
                            (uint16x8_t *)&vaccB);

            for (iX = 0; iX < iWidth; iX++) {
                uint16x8_t in = vldrhq_gather_shifted_offset_z_u16(phwPixel, voffs, tailPred);
                uint16x8_t vR, vG, vB;

                __arm_2d_rgb565_unpack_single_vec(in, &vR, &vG, &vB);

                int16x8_t vdiffR = vsubq_s16(vR, vaccR);
                int16x8_t vdiffG = vsubq_s16(vG, vaccG);
                int16x8_t vdiffB = vsubq_s16(vB, vaccB);

                vaccR += vqdmulhq(vdiffR, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccB += vqdmulhq(vdiffB, hwRatio);

                vstrhq_scatter_shifted_offset_p_u16(
                    phwPixel, 
                    voffs, 
                    __arm_2d_rgb565_pack_single_vec(vaccR, vaccG, vaccB), tailPred);

                voffs = voffs - 1;
            }
        }
    }

    /* top to down, left to right */
    if (this.bForwardVertical) {
        uint16_t *phwPixel = phwTarget;

    #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
        if (NULL != ptStatusH) {
            ptStatusH += tOffset.iX;
        }
    #endif

        /* columns direct path */
        for (iX = 0; iX < iWidth / 8; iX++) {
            uint16_t       *phwChannel = phwPixel;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH && tOffset.iY > 0) {

                /* recover the previous values */
                pAccBase = (int16_t *) ptStatusH;
                vaccR = vld1q(pAccBase);
                vaccG = vld1q(pAccBase + 8);
                vaccB = vld1q(pAccBase + 16);
            } else 
        #endif
            {
                int16x8_t       vacc = vldrhq_u16(phwChannel);
                __arm_2d_rgb565_unpack_single_vec(vacc, (uint16x8_t *)&vaccR, (uint16x8_t *)&vaccG, (uint16x8_t *)&vaccB);
            }

            for (iY = 0; iY < iHeight; iY++) {

                uint16x8_t      in = vldrhq_u16(phwChannel);
                uint16x8_t      vR, vG, vB;

                __arm_2d_rgb565_unpack_single_vec(in, &vR, &vG, &vB);

                int16x8_t       vdiffR = vsubq_s16(vR, vaccR);
                int16x8_t       vdiffG = vsubq_s16(vG, vaccG);
                int16x8_t       vdiffB = vsubq_s16(vB, vaccB);

                vaccR += vqdmulhq(vdiffR, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccB += vqdmulhq(vdiffB, hwRatio);

                vstrhq_u16(phwChannel, __arm_2d_rgb565_pack_single_vec(vaccR, vaccG, vaccB));
                phwChannel += iTargetStride;
            }

            phwPixel += 8;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH) {
                /* save the last pixels */
                pAccBase = (int16_t *) ptStatusH;
                vst1q(pAccBase, vaccR);
                vst1q(pAccBase + 8, vaccG);
                vst1q(pAccBase + 16, vaccB);
                ptStatusH += 8;
            }
        #endif
        }


        if (iWidth & 7) {
            mve_pred16_t    tailPred = vctp16q(iWidth & 7);
            uint16_t       *phwChannel = phwPixel;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH && tOffset.iY > 0) {
                /* recover the previous values */
                pAccBase = (int16_t *) ptStatusH;
                vaccR = vld1q(pAccBase);
                vaccG = vld1q(pAccBase + 8);
                vaccB = vld1q(pAccBase + 16);
            } else 
        #endif
            {
                int16x8_t       vacc = vldrhq_u16(phwChannel);
                __arm_2d_rgb565_unpack_single_vec(vacc, (uint16x8_t *)&vaccR, (uint16x8_t *)&vaccG, (uint16x8_t *)&vaccB);
            }

            for (iY = 0; iY < iHeight; iY++) {
                uint16x8_t      in = vldrhq_u16(phwChannel);
                uint16x8_t      vR, vG, vB;

                __arm_2d_rgb565_unpack_single_vec(in, &vR, &vG, &vB);

                int16x8_t       vdiffR = vsubq_s16(vR, vaccR);
                int16x8_t       vdiffG = vsubq_s16(vG, vaccG);
                int16x8_t       vdiffB = vsubq_s16(vB, vaccB);

                vaccR += vqdmulhq(vdiffR, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccB += vqdmulhq(vdiffB, hwRatio);

                vstrhq_p_u16(phwChannel, __arm_2d_rgb565_pack_single_vec(vaccR, vaccG, vaccB), tailPred);
                phwChannel += iTargetStride;
            }
        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH) {
                /* save the last pixels */
                pAccBase = (int16_t *) ptStatusH;
                vst1q(pAccBase, vaccR);
                vst1q(pAccBase + 8, vaccG);
                vst1q(pAccBase + 16, vaccB);
            }
        #endif
        }
    }

    /* down to top, right to left */
    if (this.bReverseVertical && bAllowReversePath) {
        uint16_t *phwPixel = &(phwTarget[iWidth - 1 - 7 + (iHeight-1)*iTargetStride]);

        /* columns direct path */
        for (iX = 0; iX < iWidth / 8; iX++) {
            uint16_t  *phwChannel = phwPixel;

            __arm_2d_rgb565_unpack_single_vec(  vldrhq_u16(phwChannel), 
                                                (uint16x8_t *)&vaccR, 
                                                (uint16x8_t *)&vaccG, 
                                                (uint16x8_t *)&vaccB);

            for (iY = 0; iY < iHeight; iY++) {

                uint16x8_t      in = vldrhq_u16(phwChannel);
                uint16x8_t      vR, vG, vB;

                __arm_2d_rgb565_unpack_single_vec(in, &vR, &vG, &vB);

                int16x8_t       vdiffR = vsubq_s16(vR, vaccR);
                int16x8_t       vdiffG = vsubq_s16(vG, vaccG);
                int16x8_t       vdiffB = vsubq_s16(vB, vaccB);

                vaccR += vqdmulhq(vdiffR, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccB += vqdmulhq(vdiffB, hwRatio);

                vstrhq_u16( phwChannel, 
                            __arm_2d_rgb565_pack_single_vec(vaccR, 
                                                            vaccG, 
                                                            vaccB));

                phwChannel -= iTargetStride;
            }

            phwPixel -= 8;
        }


        if (iWidth & 7) {
            mve_pred16_t    tailPred = vctp16q(iWidth & 7);
            uint16_t       *phwChannel = phwPixel;

            __arm_2d_rgb565_unpack_single_vec(  vldrhq_z_u16(phwChannel, tailPred), 
                                                (uint16x8_t *)&vaccR, 
                                                (uint16x8_t *)&vaccG, 
                                                (uint16x8_t *)&vaccB);

            for (iY = 0; iY < iHeight; iY++) {
                uint16x8_t      in = vldrhq_z_u16(phwChannel, tailPred);
                uint16x8_t      vR, vG, vB;

                __arm_2d_rgb565_unpack_single_vec(in, &vR, &vG, &vB);

                int16x8_t       vdiffR = vsubq_s16(vR, vaccR);
                int16x8_t       vdiffG = vsubq_s16(vG, vaccG);
                int16x8_t       vdiffB = vsubq_s16(vB, vaccB);

                vaccR += vqdmulhq(vdiffR, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccB += vqdmulhq(vdiffB, hwRatio);

                vstrhq_p_u16(   phwChannel, 
                                __arm_2d_rgb565_pack_single_vec(vaccR, 
                                                                vaccG, 
                                                                vaccB), 
                                tailPred);

                phwChannel -= iTargetStride;
            }
        }
    }

}




__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_cccn888_filter_iir_blur) (
                    uint32_t * __RESTRICT pwTarget,
                    int16_t iTargetStride,
                    arm_2d_region_t * __RESTRICT ptValidRegionOnVirtualScreen,
                    arm_2d_region_t * ptTargetRegionOnVirtualScreen,
                    uint8_t chBlurDegree, 
                    arm_2d_filter_iir_blur_descriptor_t *ptThis)
{
    arm_2d_scratch_mem_t *ptScratchMemory = &this.tScratchMemory;
    int_fast16_t    iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t    iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;

    if (0 == chBlurDegree) {
        return ;
    }

    int32_t         iY, iX;
    /* pre-scaled ratio to take into account doubling + high-part extraction of vqdmulhq */
    int16_t         hwRatio = (256 - chBlurDegree) << 7;
    __arm_2d_iir_blur_acc_cccn888_t       *ptStatusH = NULL;
    __arm_2d_iir_blur_acc_cccn888_t       *ptStatusV = NULL;
    int16_t        *pAccBase = NULL;
    int16x8_t       vaccB, vaccG, vaccR;

#if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
    if (NULL != (void *) (ptScratchMemory->pBuffer)) {
        ptStatusH = (__arm_2d_iir_blur_acc_cccn888_t *) ptScratchMemory->pBuffer;
        ptStatusV = ptStatusH + ROUND_UP_8(ptTargetRegionOnVirtualScreen->tSize.iWidth);
    }
#endif

    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

#if defined(__ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__)                    \
 && __ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__
    const bool bAllowReversePath = (1 == arm_2d_is_region_inside_target(
                                                ptTargetRegionOnVirtualScreen,
                                                ptValidRegionOnVirtualScreen));
#else
    const bool bAllowReversePath = false;
#endif

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

    if (this.bForwardHorizontal) {
        uint32_t *pwPixel = pwTarget;

    #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
        if (NULL != ptStatusV) {
            /* rows direct path */
            ptStatusV += tOffset.iY;
        }
    #endif

        uint16x8_t vstride = vidupq_n_u16(0, 4);
        vstride = vstride * iTargetStride;

        for (iY = 0; iY < iHeight / 8; iY++) {
            uint8_t        *pchPixelB = (uint8_t *) pwPixel;
            uint8_t        *pchPixelG = pchPixelB + 1;
            uint8_t        *pchPixelR = pchPixelB + 2;
        
        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV && tOffset.iX > 0) {
                /* recover the previous accumulators */
                pAccBase = (int16_t *) ptStatusV;
                vaccB = vld1q(pAccBase);
                vaccG = vld1q(pAccBase + 8);
                vaccR = vld1q(pAccBase + 16);
            } else 
        #endif
            {
                vaccB = vldrbq_gather_offset_u16(pchPixelB, vstride);
                vaccG = vldrbq_gather_offset_u16(pchPixelG, vstride);
                vaccR = vldrbq_gather_offset_u16(pchPixelR, vstride);
            }


            for (iX = 0; iX < iWidth; iX++) {

                uint16x8_t      inB = vldrbq_gather_offset_u16(pchPixelB, vstride);
                uint16x8_t      inG = vldrbq_gather_offset_u16(pchPixelG, vstride);
                uint16x8_t      inR = vldrbq_gather_offset_u16(pchPixelR, vstride);

                int16x8_t       vdiffB = vsubq_s16(inB, vaccB);
                int16x8_t       vdiffG = vsubq_s16(inG, vaccG);
                int16x8_t       vdiffR = vsubq_s16(inR, vaccR);

                vaccB += vqdmulhq(vdiffB, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccR += vqdmulhq(vdiffR, hwRatio);

                vstrbq_scatter_offset_u16(pchPixelB, vstride, vaccB);
                vstrbq_scatter_offset_u16(pchPixelG, vstride, vaccG);
                vstrbq_scatter_offset_u16(pchPixelR, vstride, vaccR);

                pchPixelB += 4;
                pchPixelG += 4;
                pchPixelR += 4;
            }

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV) {
                pAccBase = (int16_t *) ptStatusV;
                vst1q(pAccBase, vaccB);
                vst1q(pAccBase + 8, vaccG);
                vst1q(pAccBase + 16, vaccR);
                ptStatusV += 8;
            }
        #endif

            pwPixel += (iTargetStride * 8);
        }

        if (iHeight & 7) {

            mve_pred16_t    tailPred = vctp16q(iHeight & 7);

            uint8_t        *pchPixelB = (uint8_t *) pwPixel;
            uint8_t        *pchPixelG = pchPixelB + 1;
            uint8_t        *pchPixelR = pchPixelB + 2;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV && tOffset.iX > 0) {
                /* recover the previous accumulators */
                pAccBase = (int16_t *) ptStatusV;
                vaccB = vld1q(pAccBase);
                vaccG = vld1q(pAccBase + 8);
                vaccR = vld1q(pAccBase + 16);
            } else 
        #endif
            {
                vaccB = vldrbq_gather_offset_u16(pchPixelB, vstride);
                vaccG = vldrbq_gather_offset_u16(pchPixelG, vstride);
                vaccR = vldrbq_gather_offset_u16(pchPixelR, vstride);
            }

            for (iX = 0; iX < iWidth; iX++) {

                uint16x8_t      inB = vldrbq_gather_offset_u16(pchPixelB, vstride);
                uint16x8_t      inG = vldrbq_gather_offset_u16(pchPixelG, vstride);
                uint16x8_t      inR = vldrbq_gather_offset_u16(pchPixelR, vstride);

                int16x8_t       vdiffB = vsubq_s16(inB, vaccB);
                int16x8_t       vdiffG = vsubq_s16(inG, vaccG);
                int16x8_t       vdiffR = vsubq_s16(inR, vaccR);

                vaccB += vqdmulhq(vdiffB, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccR += vqdmulhq(vdiffR, hwRatio);


                vstrbq_scatter_offset_p_u16(pchPixelB, vstride, vaccB, tailPred);
                vstrbq_scatter_offset_p_u16(pchPixelG, vstride, vaccG, tailPred);
                vstrbq_scatter_offset_p_u16(pchPixelR, vstride, vaccR, tailPred);

                pchPixelB += 4;
                pchPixelG += 4;
                pchPixelR += 4;
            }

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusV) {
                pAccBase = (int16_t *) ptStatusV;
                vst1q(pAccBase, vaccB);
                vst1q(pAccBase + 8, vaccG);
                vst1q(pAccBase + 16, vaccR);
            }
        #endif
        }
    }

    if (this.bReverseHorizontal && bAllowReversePath) {
        uint32_t *pwPixel = &(pwTarget[(iHeight - 1 - 7)*iTargetStride]);

        uint16x8_t vstride = vidupq_n_u16(4, 4);
        vstride = vstride * iTargetStride;
        vstride -= 4;

        for (iY = 0; iY < iHeight / 8; iY++) {
            uint8_t        *pchPixelB = (uint8_t *) pwPixel;
            uint8_t        *pchPixelG = pchPixelB + 1;
            uint8_t        *pchPixelR = pchPixelB + 2;

            uint16x8_t voff = vstride;

            vaccB = vldrbq_gather_offset_u16(pchPixelB, voff);
            vaccG = vldrbq_gather_offset_u16(pchPixelG, voff);
            vaccR = vldrbq_gather_offset_u16(pchPixelR, voff);
            

            for (iX = 0; iX < iWidth; iX++) {

                uint16x8_t      inB = vldrbq_gather_offset_u16(pchPixelB, voff);
                uint16x8_t      inG = vldrbq_gather_offset_u16(pchPixelG, voff);
                uint16x8_t      inR = vldrbq_gather_offset_u16(pchPixelR, voff);

                int16x8_t       vdiffB = vsubq_s16(inB, vaccB);
                int16x8_t       vdiffG = vsubq_s16(inG, vaccG);
                int16x8_t       vdiffR = vsubq_s16(inR, vaccR);

                vaccB += vqdmulhq(vdiffB, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccR += vqdmulhq(vdiffR, hwRatio);

                vstrbq_scatter_offset_u16(pchPixelB, voff, vaccB);
                vstrbq_scatter_offset_u16(pchPixelG, voff, vaccG);
                vstrbq_scatter_offset_u16(pchPixelR, voff, vaccR);

                voff -= 4;
            }

            pwPixel -= (iTargetStride * 8);
        }

        if (iHeight & 7) {

            mve_pred16_t    tailPred = vctp16q(iHeight & 7);

            uint8_t        *pchPixelB = (uint8_t *) pwPixel;
            uint8_t        *pchPixelG = pchPixelB + 1;
            uint8_t        *pchPixelR = pchPixelB + 2;
            
            uint16x8_t voff = vstride;

            vaccB = vldrbq_gather_offset_z_u16(pchPixelB, voff, tailPred);
            vaccG = vldrbq_gather_offset_z_u16(pchPixelG, voff, tailPred);
            vaccR = vldrbq_gather_offset_z_u16(pchPixelR, voff, tailPred);

            for (iX = 0; iX < iWidth; iX++) {

                uint16x8_t      inB = vldrbq_gather_offset_z_u16(pchPixelB, voff, tailPred);
                uint16x8_t      inG = vldrbq_gather_offset_z_u16(pchPixelG, voff, tailPred);
                uint16x8_t      inR = vldrbq_gather_offset_z_u16(pchPixelR, voff, tailPred);

                int16x8_t       vdiffB = vsubq_s16(inB, vaccB);
                int16x8_t       vdiffG = vsubq_s16(inG, vaccG);
                int16x8_t       vdiffR = vsubq_s16(inR, vaccR);

                vaccB += vqdmulhq(vdiffB, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccR += vqdmulhq(vdiffR, hwRatio);


                vstrbq_scatter_offset_p_u16(pchPixelB, voff, vaccB, tailPred);
                vstrbq_scatter_offset_p_u16(pchPixelG, voff, vaccG, tailPred);
                vstrbq_scatter_offset_p_u16(pchPixelR, voff, vaccR, tailPred);

                voff -= 1;
            }
        }
    }

    if (this.bForwardVertical) {
        uint32_t *pwPixel = pwTarget;

    #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
        if (NULL != ptStatusH) {
            ptStatusH += tOffset.iX;
        }
    #endif

        uint16x8_t vstride = vidupq_n_u16(0, 4);

        /* columns direct path */
        for (iX = 0; iX < iWidth / 8; iX++) {

            uint8_t        *pchPixelB = (uint8_t *) pwPixel;
            uint8_t        *pchPixelG = pchPixelB + 1;
            uint8_t        *pchPixelR = pchPixelB + 2;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH && tOffset.iY > 0) {
                /* recover the previous accumulators */
                pAccBase = (int16_t *) ptStatusH;
                vaccB = vld1q(pAccBase);
                vaccG = vld1q(pAccBase + 8);
                vaccR = vld1q(pAccBase + 16);
            } else 
        #endif
            {
                vaccB = vldrbq_gather_offset_u16(pchPixelB, vstride);
                vaccG = vldrbq_gather_offset_u16(pchPixelG, vstride);
                vaccR = vldrbq_gather_offset_u16(pchPixelR, vstride);
            }

            for (iY = 0; iY < iHeight; iY++) {

                uint16x8_t      inB = vldrbq_gather_offset_u16(pchPixelB, vstride);
                uint16x8_t      inG = vldrbq_gather_offset_u16(pchPixelG, vstride);
                uint16x8_t      inR = vldrbq_gather_offset_u16(pchPixelR, vstride);

                int16x8_t       vdiffB = vsubq_s16(inB, vaccB);
                int16x8_t       vdiffG = vsubq_s16(inG, vaccG);
                int16x8_t       vdiffR = vsubq_s16(inR, vaccR);

                vaccB += vqdmulhq(vdiffB, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccR += vqdmulhq(vdiffR, hwRatio);


                vstrbq_scatter_offset_u16(pchPixelB, vstride, vaccB);
                vstrbq_scatter_offset_u16(pchPixelG, vstride, vaccG);
                vstrbq_scatter_offset_u16(pchPixelR, vstride, vaccR);

                pchPixelB += 4 * iTargetStride;
                pchPixelG += 4 * iTargetStride;
                pchPixelR += 4 * iTargetStride;

            }

            pwPixel += 8;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH) {
                pAccBase = (int16_t *) ptStatusH;
                vst1q(pAccBase, vaccB);
                vst1q(pAccBase + 8, vaccG);
                vst1q(pAccBase + 16, vaccR);

                ptStatusH += 8;
            }
        #endif
        }

        if (iWidth & 7) {
            mve_pred16_t    tailPred = vctp16q(iWidth & 7);
            uint8_t        *pchPixelB = (uint8_t *) pwPixel;
            uint8_t        *pchPixelG = pchPixelB + 1;
            uint8_t        *pchPixelR = pchPixelB + 2;

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH && tOffset.iY > 0) {
                /* recover the previous accumulators */
                pAccBase = (int16_t *) ptStatusH;
                vaccB = vld1q(pAccBase);
                vaccG = vld1q(pAccBase + 8);
                vaccR = vld1q(pAccBase + 16);
            } else 
        #endif
            {
                vaccB = vldrbq_gather_offset_u16(pchPixelB, vstride);
                vaccG = vldrbq_gather_offset_u16(pchPixelG, vstride);
                vaccR = vldrbq_gather_offset_u16(pchPixelR, vstride);
            }

            for (iY = 0; iY < iHeight; iY++) {
                uint16x8_t      inB = vldrbq_gather_offset_u16(pchPixelB, vstride);
                uint16x8_t      inG = vldrbq_gather_offset_u16(pchPixelG, vstride);
                uint16x8_t      inR = vldrbq_gather_offset_u16(pchPixelR, vstride);

                int16x8_t       vdiffB = vsubq_s16(inB, vaccB);
                int16x8_t       vdiffG = vsubq_s16(inG, vaccG);
                int16x8_t       vdiffR = vsubq_s16(inR, vaccR);

                vaccB += vqdmulhq(vdiffB, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccR += vqdmulhq(vdiffR, hwRatio);


                vstrbq_scatter_offset_p_u16(pchPixelB, vstride, vaccB, tailPred);
                vstrbq_scatter_offset_p_u16(pchPixelG, vstride, vaccG, tailPred);
                vstrbq_scatter_offset_p_u16(pchPixelR, vstride, vaccR, tailPred);

                pchPixelB += 4 * iTargetStride;
                pchPixelG += 4 * iTargetStride;
                pchPixelR += 4 * iTargetStride;
            }

        #if !defined(__ARM_2D_CFG_UNSAFE_NO_PFB_SUPPORT_IN_IIR_BLUR_HELIUM__)
            if (NULL != ptStatusH) {
                pAccBase = (int16_t *) ptStatusH;
                vst1q(pAccBase, vaccB);
                vst1q(pAccBase + 8, vaccG);
                vst1q(pAccBase + 16, vaccR);

            }
        #endif
        }
    }

    if (this.bReverseVertical && bAllowReversePath) {
        uint32_t *pwPixel = &(pwTarget[iWidth - 1 - 7 + (iHeight-1)*iTargetStride]);

        uint16x8_t vstride = vidupq_n_u16(0, 4);

        /* columns direct path */
        for (iX = 0; iX < iWidth / 8; iX++) {

            uint8_t        *pchPixelB = (uint8_t *) pwPixel;
            uint8_t        *pchPixelG = pchPixelB + 1;
            uint8_t        *pchPixelR = pchPixelB + 2;

            vaccB = vldrbq_gather_offset_u16(pchPixelB, vstride);
            vaccG = vldrbq_gather_offset_u16(pchPixelG, vstride);
            vaccR = vldrbq_gather_offset_u16(pchPixelR, vstride);

            for (iY = 0; iY < iHeight; iY++) {

                uint16x8_t      inB = vldrbq_gather_offset_u16(pchPixelB, vstride);
                uint16x8_t      inG = vldrbq_gather_offset_u16(pchPixelG, vstride);
                uint16x8_t      inR = vldrbq_gather_offset_u16(pchPixelR, vstride);

                int16x8_t       vdiffB = vsubq_s16(inB, vaccB);
                int16x8_t       vdiffG = vsubq_s16(inG, vaccG);
                int16x8_t       vdiffR = vsubq_s16(inR, vaccR);

                vaccB += vqdmulhq(vdiffB, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccR += vqdmulhq(vdiffR, hwRatio);


                vstrbq_scatter_offset_u16(pchPixelB, vstride, vaccB);
                vstrbq_scatter_offset_u16(pchPixelG, vstride, vaccG);
                vstrbq_scatter_offset_u16(pchPixelR, vstride, vaccR);

                pchPixelB -= 4 * iTargetStride;
                pchPixelG -= 4 * iTargetStride;
                pchPixelR -= 4 * iTargetStride;

            }

            pwPixel -= 8;
        }

        if (iWidth & 7) {
            mve_pred16_t    tailPred = vctp16q(iWidth & 7);
            uint8_t        *pchPixelB = (uint8_t *) pwPixel;
            uint8_t        *pchPixelG = pchPixelB + 1;
            uint8_t        *pchPixelR = pchPixelB + 2;

            vaccB = vldrbq_gather_offset_z_u16(pchPixelB, vstride, tailPred);
            vaccG = vldrbq_gather_offset_z_u16(pchPixelG, vstride, tailPred);
            vaccR = vldrbq_gather_offset_z_u16(pchPixelR, vstride, tailPred);

            for (iY = 0; iY < iHeight; iY++) {
                uint16x8_t      inB = vldrbq_gather_offset_z_u16(pchPixelB, vstride, tailPred);
                uint16x8_t      inG = vldrbq_gather_offset_z_u16(pchPixelG, vstride, tailPred);
                uint16x8_t      inR = vldrbq_gather_offset_z_u16(pchPixelR, vstride, tailPred);

                int16x8_t       vdiffB = vsubq_s16(inB, vaccB);
                int16x8_t       vdiffG = vsubq_s16(inG, vaccG);
                int16x8_t       vdiffR = vsubq_s16(inR, vaccR);

                vaccB += vqdmulhq(vdiffB, hwRatio);
                vaccG += vqdmulhq(vdiffG, hwRatio);
                vaccR += vqdmulhq(vdiffR, hwRatio);

                vstrbq_scatter_offset_p_u16(pchPixelB, vstride, vaccB, tailPred);
                vstrbq_scatter_offset_p_u16(pchPixelG, vstride, vaccG, tailPred);
                vstrbq_scatter_offset_p_u16(pchPixelR, vstride, vaccR, tailPred);

                pchPixelB -= 4 * iTargetStride;
                pchPixelG -= 4 * iTargetStride;
                pchPixelR -= 4 * iTargetStride;
            }
        }
    }
}



#ifdef   __cplusplus
}
#endif

#undef __arm_2d_rgb565_unpack_single_vec
#endif                          /* __ARM_2D_COMPILATION_UNIT */
