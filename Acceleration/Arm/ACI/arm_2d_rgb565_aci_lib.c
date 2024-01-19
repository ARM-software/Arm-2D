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
 * Project:      Arm-2D RGB565 ACI Library
 * Title:        arm_2d_rgb565_aci_lib.c
 * Description:  Provides RGB565 ACI acceleration
 *
 * $Date:        17. Jan 2024
 * $Revision:    V 1.0.0
 *
 * Target Processor:  Cortex-M cores with Helium & RGB565 Vector CDE
 *
 * -------------------------------------------------------------------- */

#ifdef __ARM_FEATURE_CDE

#define __ARM_2D_IMPL__

#include "arm_2d_rgb565_aci_lib.h"


void __arm_2d_aci_init(void)
{
    uint32_t        coproPrivMask = 0;
    /* Enable Full access privileges for coprocessor IDs deduced from compiler exported bitmap mask __ARM_FEATURE_CDE_COPROC */
    /* This library requires +cdecp0 compilation, enabling bit 0 */

#define COPRO_FULL_ACCESS 0x3U

    for (uint32_t i = 0; i < 7; i++) {
        if (__ARM_FEATURE_CDE_COPROC & (1 << i))
            coproPrivMask |= (COPRO_FULL_ACCESS << (i * 2));
    }

    SCB->CPACR |= coproPrivMask;
    __ISB();
}


__OVERRIDE_WEAK
void __arm_2d_impl_rgb565_colour_filling_with_opacity(uint16_t *
                                                      __restrict pTargetBase,
                                                      int16_t iTargetStride,
                                                      arm_2d_size_t *
                                                      __restrict ptCopySize,
                                                      uint16_t Colour,
                                                      uint_fast16_t hwRatio)
{

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif
    uint16x8_t      vecColour = vdupq_n_u16(Colour);
    uint16x8_t      vecRatio = vdupq_n_u16((uint16_t)(hwRatio >> 1));
    int32_t         blkCnt;

    for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
        uint16_t       *phwTarget = pTargetBase;

        blkCnt = ptCopySize->iWidth;
        do {
            mve_pred16_t    p = vctp16q((uint32_t)blkCnt);

            vst1q_p_u16(phwTarget,
                        __arm_2d_cde_vrgb565_blend_m(
                            vld1q_z(phwTarget, p),
                            vecColour,
                            vecRatio, p),
                        p);

            phwTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        pTargetBase += iTargetStride;
    }
}
#endif
