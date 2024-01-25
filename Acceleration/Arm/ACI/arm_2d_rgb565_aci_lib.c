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


#if COMPARE_CDE_TEST
#include <stdio.h>

#define DUMP_Q15_VEC(vec)   printf("%s { %d %d %d %d %d %d %d %d }\n",#vec, vec[0], vec[1], vec[2], vec[3], vec[4], vec[5], vec[6], vec[7])

int COMPARE_Q15_VEC(uint16x8_t vec1, uint16x8_t vec2)
{
    for (int i = 0; i < 8; i++) {
        if (vec1[i] != vec2[i]) {
            return 0;
        }
    }
    return 1;
}
#endif

static void __arm_2d_cde_rgb565_init(void)
{
#define COPRO_FULL_ACCESS 0x3U

    SCB->CPACR |= COPRO_FULL_ACCESS << (ARM_2D_RGB565_ACI_LIB_COPRO_ID * 2);
    __ISB();
}


__OVERRIDE_WEAK
    void __arm_2d_impl_rgb565_colour_filling_with_opacity(uint16_t *
                                                          __restrict pTargetBase,
                                                          int16_t iTargetStride,
                                                          arm_2d_size_t *
                                                          __restrict ptCopySize,
                                                          uint16_t Colour, uint_fast16_t hwRatio)
{

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif
    uint16x8_t      vecColour = vdupq_n_u16(Colour);
    uint16x8_t      vecRatio = vdupq_n_u16((uint16_t) (hwRatio >> 0));
    int32_t         blkCnt;

#if COMPARE_CDE_TEST
    uint16_t        ratio1x8 = hwRatio * 8;
    uint16_t        ratio1x4 = hwRatio * 4;
    uint16_t        ratio2x8 = (256 - hwRatio) * 8;
    uint16_t        ratio2x4 = (256 - hwRatio) * 4;

    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecMaskGpck = vdupq_n_u16(0x00fc);
    uint16x8_t      vecIn;
    uint16x8_t      vecColorR, vecColorB, vecColorG;

    /* unpack color & scale */
    vecIn = vdupq_n_u16(Colour);
    vecColorR = (vecIn & vecMaskR) * ratio1x8;

    vecColorB = (vecIn >> 11) * ratio1x8;

    vecColorG = vecIn >> 5;
    vecColorG = (vecColorG & vecMaskG) * ratio1x4;
#endif

    for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
        uint16_t       *phwTarget = pTargetBase;

        blkCnt = ptCopySize->iWidth;
        do {
            mve_pred16_t    p = vctp16q((uint32_t) blkCnt);

#if COMPARE_CDE_TEST
            uint16x8_t      vecR0, vecB0, vecG0;
            uint16x8_t      vecR1, vecB1, vecG1;

            /* unpack stream */
            vecIn = vld1q(phwTarget);
            vecR1 = vecIn & vecMaskR;

            vecB1 = vecIn >> 11;

            vecG1 = vecIn >> 5;
            vecG1 = vecG1 & vecMaskG;


            /* merge */
            vecR0 = vecColorR + vecR1 * ratio2x8;
            vecR0 = vecR0 >> 8;

            vecG0 = vecColorG + vecG1 * ratio2x4;
            vecG0 = vecG0 >> 8;

            vecB0 = vecColorB + vecB1 * ratio2x8;
            vecB0 = vecB0 >> 8;

            /* pack */
            uint16x8_t      vOut = vecR0 >> 3 | vmulq((vecG0 & vecMaskGpck), 8)
                | vmulq((vecB0 & vecMaskBpck), 256);


            uint16x8_t      cde = __arm_2d_cde_vrgb565_blend(vecColour,
                                                             vecIn,

                                                             vecRatio);

            if (COMPARE_Q15_VEC(cde, vOut) == 0) {
                printf("\nhwRatio %d\n", hwRatio);
                DUMP_Q15_VEC(vecIn);
                DUMP_Q15_VEC(vecColour);
                DUMP_Q15_VEC(cde);
                DUMP_Q15_VEC(vOut);
            }
#endif
            vst1q_p_u16(phwTarget,
                        __arm_2d_cde_vrgb565_blend_m(vld1q_z(phwTarget, p),
                                                     vecColour, vecRatio, p), p);

            phwTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        pTargetBase += iTargetStride;
    }
}
#endif

void __arm_2d_aci_init(void)
{
    if (__ARM_FEATURE_CDE_COPROC & (1 << ARM_2D_RGB565_ACI_LIB_COPRO_ID))
        __arm_2d_cde_rgb565_init();
}
