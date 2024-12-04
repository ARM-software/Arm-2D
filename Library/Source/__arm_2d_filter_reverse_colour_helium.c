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
* Title:        __arm_2d_filter_reverse_colour_helium.c
* Description:  Source code for reverse colour helium implementation
*
* $Date:        4. Dec 2024
* $Revision:    V.1.0.0
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

#include <arm_mve.h>

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
void __arm_2d_impl_gray8_reverse_colour(uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize)
{
    uint8x16_t vFF = vdupq_n_u8(__UINT8_MAX__);
    int16_t iHeight = ptCopySize->iHeight;
    int16_t iWidth = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *__RESTRICT pchTargetLine = pchTarget;
        
        int16_t iStrideWidth = iWidth;

        do {
            mve_pred16_t vp = vctp8q(iStrideWidth);

            uint8x16_t vTarget = vld1q_z_u8(pchTargetLine, vp);

            vTarget ^= vFF;

            vst1q_p_u8(pchTargetLine, vTarget, vp);
            pchTargetLine += (128 / sizeof(uint8_t));
            iStrideWidth -= (128 / sizeof(uint8_t));
        } while(iStrideWidth > 0);

        pchTarget += iTargetStride;
    }
}

__OVERRIDE_WEAK
void __arm_2d_impl_rgb565_reverse_colour(   uint16_t *__RESTRICT phwTarget,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *__RESTRICT ptCopySize)
{
    uint16x8_t vFFFF = vdupq_n_u16(__UINT16_MAX__);
    int16_t iHeight = ptCopySize->iHeight;
    int16_t iWidth = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *__RESTRICT phwTargetLine = phwTarget;
        
        int16_t iStrideWidth = iWidth;

        do {
            mve_pred16_t vp = vctp16q(iStrideWidth);

            uint16x8_t vTarget = vld1q_z_u16(phwTargetLine, vp);

            vTarget ^= vFFFF;

            vst1q_p_u16(phwTargetLine, vTarget, vp);
            phwTargetLine += (128 / sizeof(uint16_t));
            iStrideWidth -= (128 / sizeof(uint16_t));
        } while(iStrideWidth > 0);

        phwTarget += iTargetStride;
    }
}

__OVERRIDE_WEAK
void __arm_2d_impl_cccn888_reverse_colour(  uint32_t *__RESTRICT pwTarget,
                                            int16_t iTargetStride,
                                            arm_2d_size_t *__RESTRICT ptCopySize)
{
    uint32x4_t v00FFFFFF = vdupq_n_u32(0x00FFFFFF);
    int16_t iHeight = ptCopySize->iHeight;
    int16_t iWidth = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwTargetLine = pwTarget;
        
        int16_t iStrideWidth = iWidth;

        do {
            mve_pred16_t vp = vctp32q(iStrideWidth);

            uint32x4_t vTarget = vld1q_z_u32(pwTargetLine, vp);

            vTarget ^= v00FFFFFF;

            vst1q_p_u32(pwTargetLine, vTarget, vp);
            pwTargetLine += (128 / sizeof(uint32_t));
            iStrideWidth -= (128 / sizeof(uint32_t));
        } while(iStrideWidth > 0);

        pwTarget += iTargetStride;
    }
}


#ifdef   __cplusplus
}
#endif

#endif                          /* __ARM_2D_COMPILATION_UNIT */
