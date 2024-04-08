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
 * Title:        __arm-2d_conversion_helium.c
 * Description:  APIs for colour format conversion with Helium acceleration
 *
 * $Date:        08. Aug 2022
 * $Revision:    V.0.2.0
 *
 * Target Processor:  Cortex-M cores with Helium
 *
 * -------------------------------------------------------------------- */

#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"

#ifdef __ARM_2D_COMPILATION_UNIT
#undef __ARM_2D_COMPILATION_UNIT

#if defined(__ARM_2D_HAS_HELIUM__) && __ARM_2D_HAS_HELIUM__

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
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
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wvector-conversion"
#   pragma clang diagnostic ignored "-Wundef"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#endif


/*============================ INCLUDES ======================================*/

#include "__arm_2d_utils_helium.h"
#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/




__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_cccn888_to_rgb565)(uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    int32_t         blkCnt;
    uint32x4_t      maskR = vdupq_n_u32(0x001f);
    uint32x4_t      maskG = vdupq_n_u32(0x07e0);
    uint32x4_t      maskB = vdupq_n_u32(0xf800);

    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        const uint32_t *pSource = pwSourceBase;
        uint16_t       *pTarget = phwTargetBase;

        blkCnt = ptCopySize->iWidth;
#ifdef USE_MVE_INTRINSICS
        do {
            mve_pred16_t    tailPred = vctp32q(blkCnt);

            /* load a vector of 4 cccn888 pixels */
            uint32x4_t      vecIn = vld1q_z(pSource, tailPred);
            /* extract individual channels and place them according */
            /* to their relative position inside rgb565 */
            uint32x4_t      vecR = (vecIn >> 3) & maskR;
            uint32x4_t      vecG = (vecIn >> 5) & maskG;
            uint32x4_t      vecB = (vecIn >> 8) & maskB;
            /* merge */
            uint32x4_t      vOut = vecR | vecG | vecB;

            /* store a vector of 4 rgb565 pixels */
            vstrhq_p_u32(pTarget, vOut, tailPred);

            pSource += 4;
            pTarget += 4;
            blkCnt -= 4;
        }
        while (blkCnt > 0);
#else
    /* constants allowing to replace right shifts with fixed-point multiplications */
    /* enabling more beat overlap oportunities and reduce stalls */
    const int32_t inv_2pow3 = 1 << (31-3); /*1/2^3 in Q.31 */
    const int32_t inv_2pow5 = 1 << (31-5); /*1/2^5 in Q.31 */
    const int32_t inv_2pow8 = 1 << (31-8); /*1/2^8 in Q.31 */

    __asm volatile(
        "   wlstp.32                lr, %[loopCnt], 1f                  \n"
        /* precompute for allowing filling stalls in the inner loop  */
        /* use vqdmulh to replace shifts to allow overlap with 'AND' */

        /* load a vector of 4 cccn888 pixels */
        "   vldrw.u32               q0, [%[pSource]], #16               \n"
        /* mimic right shift by 3 */
        "   vqdmulh.s32             q1, q0, %[inv_2pow3]                \n"

        ".p2align 2                                                     \n"
        "2:                                                             \n"
        "   vand                    q1, q1, %q[maskR]                   \n"
        /* mimic right shift by 5 */
        "   vqdmulh.s32             q2, q0, %[inv_2pow5]                \n"
        "   vand                    q2, q2, %q[maskG]                   \n"
        /* mimic right shift by 8 */
        "   vqdmulh.s32             q3, q0, %[inv_2pow8]                \n"
        /* accumulate R & G */
        "   vorr                    q2, q1, q2                          \n"
        /* load next vector of 4 cccn888 pixels */
        "   vldrw.u32               q0, [%[pSource]], #16               \n"
        "   vand                    q3, q3, %q[maskB]                   \n"
        /* mimic right shift by 3 */
        "   vqdmulh.s32             q1, q0, %[inv_2pow3]                \n"
        /* accumulate B */
        "   vorr                    q2, q2, q3                          \n"
        /* store a vector of 4 rgb565 pixels */
        "   vstrh.32                q2, [%[pTarget]], #8                \n"
        "   letp                    lr, 2b                              \n"
        "1:                                                             \n"

        : [pSource] "+r"(pSource), [pTarget] "+l" (pTarget)
        : [loopCnt] "r"(blkCnt), [inv_2pow3] "r" (inv_2pow3),
          [inv_2pow5] "r" (inv_2pow5),  [inv_2pow8] "r" (inv_2pow8),
          [maskR] "t" (maskR),[maskG] "t" (maskG),[maskB] "t" (maskB)
        : "q0", "q1", "q2", "q3", "memory", "r14" );
#endif

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    }
}



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_to_rgb565)(uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
#ifdef USE_MVE_INTRINSICS
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint8_t  *__RESTRICT pSource = (const uint8_t *) pwSourceBase;
        uint16_t       *__RESTRICT phwTarget = phwTargetBase;
        int32_t         blkCnt = ptCopySize->iWidth;

        do {
            uint16x8_t      R, G, B;
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint8x16x2_t    vdeintr2 = vld2q_u8(pSource);

            uint16x8_t      vSrcOpa = vmovltq(vdeintr2.val[1]);
            uint16x8_t      vSrcG = vmovlbq(vdeintr2.val[1]);
            uint16x8_t      vSrcR = vmovltq(vdeintr2.val[0]);
            uint16x8_t      vSrcB = vmovlbq(vdeintr2.val[0]);

            uint16x8_t      vtrgt = vld1q(phwTarget);

            __arm_2d_rgb565_unpack_single_vec(vtrgt, &R, &G, &B);

            uint16x8_t      vTrans = 256 - vSrcOpa;

            vSrcR = (vmulq(vSrcR, vSrcOpa) + vmulq(R, vTrans)) >> 8;
            vSrcG = (vmulq(vSrcG, vSrcOpa) + vmulq(G, vTrans)) >> 8;
            vSrcB = (vmulq(vSrcB, vSrcOpa) + vmulq(B, vTrans)) >> 8;

            vst1q_p(phwTarget,
                __arm_2d_rgb565_pack_single_vec(vSrcR, vSrcG, vSrcB), tailPred);

            pSource += 32;
            phwTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    }
#else
    uint16x8_t      scratch[2];
    vst1q((uint16_t*)&scratch[1], vdupq_n_u16(0xf800));
    vst1q((uint16_t*)&scratch[0], vdupq_n_u16(0x7e0));

    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {


        const uint8_t  *__RESTRICT pSource = (const uint8_t *) pwSourceBase;
        uint16_t       *__RESTRICT phwTarget = phwTargetBase;

        register unsigned loopCnt  __asm("lr");
        loopCnt = ptCopySize->iWidth;

    __asm volatile(
        ".p2align 2                                         \n"

        // pipelining
        "   vldrh.u16       q4, [%[phwTarget]]              \n"
        "   vmov.i16        q0, #0xf8                       \n"
        "   vld20.8         {q5,q6}, [%[pSource]]           \n"
        "   wlstp.16        lr, %[loopCnt], 1f              \n"
        "   vld21.8         {q5,q6}, [%[pSource]]!          \n"


        "2:                                                 \n"
        "   vrev16.8        q7, q6                          \n"
        "   vmullb.u8       q3, q5, q7                      \n"
        // Opacity
        "   vmovlt.u8       q2, q6                          \n"
        "   vmov.i16        q1, #0x100                      \n"
        // Transparency
        "   vsub.i16        q2, q1, q2                      \n"
        // vmullt combines vmovlt + vmul
        "   vmullt.u8       q1, q5, q6                      \n"
        "   vshr.u16        q5, q4, #0x3                    \n"
        "   vmullb.u8       q7, q6, q7                      \n"
        "   vshr.u16        q6, q4, #0x8                    \n"
        "   vmul.i16        q4, q4,  %[eight]               \n"
        "   vand q6,        q6, q0                          \n"
        "   vmul.i16        q6, q2, q6                      \n"
        "   vand            q4, q4, q0                      \n"
        "   vmul.i16        q4, q2, q4                      \n"
        "   vadd.i16        q6, q6, q1                      \n"
        "   vmov.i16        q1, #0xfc                       \n"
        "   vand            q5, q5, q1                      \n"
        "   vmul.i16        q2, q2, q5                      \n"
        "   vadd.i16        q1, q4, q3                      \n"
        // vector of 0xf800
        "   vldrw.u32       q4, [%[scratch], #16]           \n"
        "   vand            q3, q6, q4                      \n"
        "   vld20.8         {q5,q6}, [%[pSource]]           \n"
        "   vshr.u16        q1, q1, #0xb                    \n"
        "   vldrh.u16       q4, [%[phwTarget], #16]         \n"
        "   vadd.i16        q2, q2, q7                      \n"
        "   vqdmulh.s16     q2, q2, %[inv2pow5]             \n"
        "   vorr            q1, q3, q1                      \n"
        // vector of 0x7e0
        "   vldrw.u32       q7, [%[scratch]]                \n"
        "   vand            q2, q2, q7                      \n"
        "   vld21.8         {q5,q6}, [%[pSource]]!          \n"
        "   vorr            q1, q1, q2                      \n"
        "   vstrh.u16       q1, [%[phwTarget]] , #16        \n"
        "   letp            lr, 2b                          \n"
        "1:                                                 \n"

        : [phwTarget] "+r" (phwTarget),
          [pSource] "+r" (pSource), [loopCnt] "+r"(loopCnt)
        : [scratch] "r" (scratch), [eight] "r" (8),
          [inv2pow5] "r" ( 1 << (15 - 5)) //  /* 1/(2^5) in Q.15 */
        : "q0", "q1", "q2", "q3",
          "q4", "q5", "q6", "q7",
          "memory" );

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    }
    #endif
}

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_to_gray8) (   uint32_t *__RESTRICT pwSourceBase,
                                        int16_t iSourceStride,
                                        uint8_t *__RESTRICT pchTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize)
{
#ifdef USE_MVE_INTRINSICS
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint8_t  *__RESTRICT pSource = (const uint8_t *) pwSourceBase;
        uint8_t *__RESTRICT pchTarget = pchTargetBase;
        int32_t         blkCnt = ptCopySize->iWidth;

        do {

            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint8x16x2_t    vdeintr2 = vld2q_u8(pSource);

            uint16x8_t      vSrcOpa = vmovltq(vdeintr2.val[1]);
            uint16x8_t      vSrcG = vmovlbq(vdeintr2.val[1]);
            uint16x8_t      vSrcR = vmovltq(vdeintr2.val[0]);
            uint16x8_t      vSrcB = vmovlbq(vdeintr2.val[0]);

            uint16x8_t      vtrgt = vldrbq_u16(pchTarget);
            uint16x8_t      vTrans = 256 - vSrcOpa;

            uint16x8_t vAvg = vSrcG + vSrcR + vSrcB;

            vAvg = vAvg / 3;

            vAvg = (vmulq(vAvg, vSrcOpa) + vmulq(vtrgt, vTrans)) >> 8;

            vstrbq_p_u16(pchTarget, vAvg, tailPred);

            pSource += 32;
            pchTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }

#else

    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint8_t  *__RESTRICT pSource = (const uint8_t *) pwSourceBase;
        uint8_t *__RESTRICT pchTarget = pchTargetBase;

        register unsigned loopCnt  __asm("lr");
        loopCnt = ptCopySize->iWidth;

    __asm volatile(
        ".p2align 2                                         \n"

        // pipelining
        "   vld20.8         {q2,q3}, [%[pSource]]           \n"
        "   vdup.16         q1, %[one_third]                \n"
        "   vld21.8         {q2,q3}, [%[pSource]]!          \n"
        "   vmov.i16        q0, #0x100                      \n"
        "   wlstp.16        lr, %[loopCnt], 1f              \n"

        "2:                                                 \n"
        // Opacity
        "   vmovlt.u8       q4, q3                          \n"
        "   vmovlb.u8       q3, q3                          \n"
        "   vmovlt.u8       q5, q2                          \n"
        "   vmovlb.u8       q2, q2                          \n"
        // averaging
        "   vadd.i16        q6, q3, q5                      \n"
        "   vadd.i16        q6, q6, q2                      \n"
        "   vmulh.u16       q6, q6, q1                      \n"
        // Transparency
        "   vsub.i16        q7, q0, q4                      \n"
        "   vld20.8         {q2,q3}, [%[pSource]]           \n"
        "   vshr.u16        q6, q6, #0x1                    \n"
        "   vmul.i16        q6, q6, q4                      \n"
        "   vldrb.u16       q4, [%[pchTarget]]              \n"
        "   vmul.i16        q7, q7, q4                      \n"
        "   vld21.8         {q2,q3}, [%[pSource]]!          \n"
        "   vadd.i16        q6, q6, q7                      \n"
        // 8-bit right shift
        "   vqdmulh.s16     q6, q6, %[inv2pow8]             \n"
        "   vstrb.u16       q6, [%[pchTarget]] , #8         \n"
        "   letp            lr, 2b                          \n"
        "1:                                                 \n"

        : [pchTarget] "+r" (pchTarget),
          [pSource] "+r" (pSource), [loopCnt] "+r"(loopCnt)
        : [one_third] "r" (0xaaab),
          [inv2pow8] "r" ( 1 << (15 - 8)) //  /* 1/(2^8) in Q.15 */
        : "q0", "q1", "q2", "q3",
          "q4", "q5", "q6", "q7",
          "memory" );

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }
    #endif
}



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_to_cccn888)( uint32_t *__RESTRICT pwSourceBase,
                                        int16_t iSourceStride,
                                        uint32_t *__RESTRICT pwTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* offset to replicate the opacity accross the 4 channels */
    uint16x8_t offset = {3, 3, 3, 3, 7, 7, 7, 7};
#ifdef USE_MVE_INTRINSICS

    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {


        const uint8_t *__RESTRICT pwSource = (const uint8_t *)pwSourceBase;
        uint8_t       *__RESTRICT pwTarget = (uint8_t *)pwTargetBase;
        int32_t         blkCnt = ptCopySize->iWidth;

        do {

            mve_pred16_t    tailPred = vctp64q(blkCnt);

            uint16x8_t vSrc = vldrbq_u16(pwSource);
            uint16x8_t vTrg = vldrbq_u16(pwTarget);

            /*
              replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
              and leaves target 0 unchanged
              vSrcOpa = | opa0 | opa0 | opa0 |  0  | opa1 | opa1 | opa1 |  0  |
              */
            uint16x8_t vSrcOpa = vldrbq_gather_offset_z_u16(pwSource,  offset, 0x3f3f);

            uint16x8_t      vTrans = 256 - vSrcOpa;

            vTrg = (vTrg *vTrans + vSrc*vSrcOpa) >> 8;

            vstrbq_p_u16(pwTarget, vTrg, tailPred);

            pwSource += 8;
            pwTarget += 8;
            blkCnt -= 2;
        }
        while (blkCnt > 0);
        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;
    }
#else

    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint8_t *__RESTRICT pwSource = (const uint8_t *)pwSourceBase;
        uint8_t       *__RESTRICT pwTarget = (uint8_t *)pwTargetBase;


        register unsigned loopCnt  __asm("lr");
        loopCnt = ptCopySize->iWidth;

      __asm volatile(
            ".p2align 2                                         \n"

            "   vmsr            p0, %[repl_pred_msk]            \n"

            "   vpst                                            \n"
            "   vldrbt.u16      q5, [%[pwSource], %q[offset]]   \n"
            "   vmov.i16        q0, #0x100                      \n"
            "   vldrb.u16       q4, [%[pwSource]], #8           \n"

            // 2 x 32-bit pixel / loop
            "   wlstp.64        lr, %[loopCnt], 1f              \n"

            "2:                                                 \n"
            "   vmul.i16        q4, q5, q4                      \n"
            "   vldrb.u16       q2, [%[pwTarget]]               \n"
            // transparency
            "   vsub.i16        q3, q0, q5                      \n"
            "   vmul.i16        q3, q3, q2                      \n"
            // Opacity
            "   vpst                                            \n"
            "   vldrbt.u16      q5, [%[pwSource], %q[offset]]   \n"
            "   vadd.i16        q3, q3, q4                      \n"
            "   vldrb.u16       q4, [%[pwSource]], #8           \n"
            "   vshr.u16        q3, q3, #0x8                    \n"

            "   vstrb.16        q3, [%[pwTarget]], #8           \n"
            "   letp            lr, 2b                          \n"
            "1:                                                 \n"
            : [pwTarget] "+r" (pwTarget),
              [pwSource] "+r" (pwSource), [loopCnt] "+r"(loopCnt)
            : [repl_pred_msk] "r" (0x3f3f), [offset] "t" (offset)
            : "q0", "q1", "q2", "q3",
              "q4", "q5",
              "memory" );

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;
    }
#endif
}

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb565_to_cccn888)(uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    int32_t         blkCnt;
    uint32x4_t      maskRB = vdupq_n_u32(0xf8);
    uint32x4_t      maskG = vdupq_n_u32(0xfc00);


    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint16_t *__RESTRICT phwSource = phwSourceBase;
        uint32_t       *__RESTRICT pwTarget = pwTargetBase;

        blkCnt = ptCopySize->iWidth;
#ifdef USE_MVE_INTRINSICS
        do {
            mve_pred16_t    tailPred = vctp32q(blkCnt);

            /* load a vector of 4 rgb565 pixels */
            uint32x4_t      vecIn = vldrhq_z_u32(phwSource, tailPred);
            /* extract individual channels and place them according position */
            uint32x4_t      vecR = (vecIn << 3) & maskRB;
            uint32x4_t      vecG = (vecIn << 5) & maskG;
            uint32x4_t      vecB = ((vecIn >> 8) & maskRB) << 16;
            /* merge and set n channel to 0xff */
            uint32x4_t      vOut = 0xff000000 | vecR | vecG | vecB;

            /* store a vector of 4 cccn888 pixels */
            vst1q_p(pwTarget, vOut, tailPred);

            phwSource += 4;
            pwTarget += 4;
            blkCnt -= 4;
        }
        while (blkCnt > 0);

#else
    __asm volatile(
        "   wlstp.32                lr, %[loopCnt], 1f                  \n"
        /* preload & precompute for allowing filling stalls in the inner loop  */
        /* use vqdmulh & vmul to replace shifts to allow overlap with 'AND'    */

        /* load a vector of 4 rgb565 pixels */
        "   vldrh.u32               q0, [%[pSource]], #8                \n"
        /* mimic left shift by 3 */
        "   vmul.u32                q1, q0, %[two_pow3]                 \n"
        ".p2align 2                                                     \n"
        "2:                                                             \n"
        /* mimic left shift by 5 */
        "   vmul.u32                q2, q0, %[two_pow5]                 \n"
        "   vand                    q1, q1, %q[maskRB]                  \n"
        /* mimic right shift by 8 */
        "   vqdmulh.s32             q3, q0, %[inv_2pow8]                \n"
        "   vand                    q2, q2, %q[maskG]                   \n"
        /* accumulate G & R, use vmla instead of vorr for best overlap */
        "   vmla.u32                q2, q1, %[one]                      \n"
        "   vand                    q3, q3, %q[maskRB]                  \n"
        /* accumulate B + left shift by 16 */
        "   vmla.u32                q2, q3, %[two_pow16]                \n"
        /* load next vector of 4 rgb565 pixels */
        "   vldrh.u32               q0, [%[pSource]], #8                \n"
        /* merge and set n channel to 0xff */
        "   vorr.i32                q2, #0xff000000                     \n"
        /* mimic left shift by 3 */
        "   vmul.u32                q1, q0, %[two_pow3]                 \n"
        /* store a vector of 4 cccn888 pixels */
        "   vstrw.32                q2, [%[pTarget]], #16               \n"
        "   letp                    lr, 2b                              \n"
        "1:                                                             \n"

        : [pSource] "+r"(phwSource), [pTarget] "+r" (pwTarget)
        : [loopCnt] "r"(blkCnt),[two_pow3] "r" (1<<3), [two_pow5] "r" (1<<5),
          [two_pow16] "r" (1<<16),[inv_2pow8] "r" (1 << (31-8)),
          [maskRB] "t" (maskRB),[maskG] "t" (maskG), [one] "r" (1)
        : "q0", "q1", "q2", "q3", "memory", "r14" );
#endif

        phwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;
    }
}

__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_cccn888_to_gray8)(uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    int32_t         blkCnt;

#define ONE_THIRD_Q8 85

#ifdef USE_MVE_INTRINSICS

    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
        const uint32_t *pSource = pwSourceBase;
        uint8_t        *pTarget = pchTargetBase;

        blkCnt = ptCopySize->iWidth;
        uint8x16_t      vone_third = vdupq_n_u8(ONE_THIRD_Q8);

        do {
            mve_pred16_t    tailPred = vctp8q(blkCnt);

            /* load and de-interleave channels from 16 cccn888 pixels */
            uint8x16x4_t    vecIn = vld4q_u8((const uint8_t *) pSource);

            /* Average R,G,B chans : scale by 1/3 and sum */
            vecIn.val[0] = vrmulhq(vecIn.val[0], vone_third);
            vecIn.val[0] = vecIn.val[0] + vrmulhq(vecIn.val[1], vone_third);
            vecIn.val[0] = vecIn.val[0] + vrmulhq(vecIn.val[2], vone_third);

            /* store a vector of 16 gray8 pixels */
            vstrbq_p_u8(pTarget, vecIn.val[0], tailPred);

            pSource += 16;
            pTarget += 16;
            blkCnt -= 16;
        }
        while (blkCnt > 0);

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }

#else
    if (ptCopySize->iWidth >= 32) {
        /* use unrolled core loop for image with larger widths */
        /* because of helium sequence optimization opportunities */

        /* residual size < 32 */
        int             tailCnt = ptCopySize->iWidth & 0x1f;

        for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
            const uint32_t *pSource = pwSourceBase;
            uint8_t        *pTarget = pchTargetBase;

            blkCnt = ptCopySize->iWidth;

            /* de-interleave RGBA channels */
            /* Core is unrolled to enable overlap between load and scale/accumulate operations */
            /* handles block of 32-pixels line */
            __asm volatile(
                 "   vld40.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vld41.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vld42.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vld43.8         {q0, q1, q2, q3}, [%[pSource]]!                      \n"
                 ".p2align 2                                                              \n"
                 "   wls             lr, %[loopCnt], 1f                                   \n"
                 "2:                                                                      \n"
                 /* overwrite unused Alpha channel  */
                 "   vmov.i8         q3, %[one_third]                                     \n"
                 /* de-interleave next 16 pixels block */
                 /* load operations interleaved with scale of previously loaded channels */
                 "   vld40.8         {q4, q5, q6, q7}, [%[pSource]]                       \n"
                 "   vrmulh.u8       q0, q0, q3                                           \n"
                 "   vld41.8         {q4, q5, q6, q7}, [%[pSource]]                       \n"
                 "   vrmulh.u8       q1, q1, q3                                           \n"
                 "   vld42.8         {q4, q5, q6, q7}, [%[pSource]]                       \n"
                 "   vadd.i8         q0, q0, q1                                           \n"
                 "   vrmulh.u8       q2, q2, q3                                           \n"
                 "   vld43.8         {q4, q5, q6, q7}, [%[pSource]]!                      \n"
                 "   vadd.i8         q0, q0, q2                                           \n"
                 /* store 16 Gray8 pixels */
                 "   vstrb.8         q0, [%[pTarget]], #16                                \n"
                 "   vmov.i8         q7, %[one_third]                                     \n"
                 "   vrmulh.u8       q4, q4, q7                                           \n"
                 /* de-interleave next 16 pixels block */
                 "   vld40.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vrmulh.u8       q5, q5, q7                                           \n"
                 "   vld41.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vadd.i8         q4, q4, q5                                           \n"
                 "   vld42.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vrmulh.u8       q6, q6, q7                                           \n"
                 "   vld43.8         {q0, q1, q2, q3}, [%[pSource]]!                      \n"
                 "   vadd.i8         q4, q4, q6                                           \n"
                 /* store 16 Gray8 pixels */
                 "   vstrb.8         q4, [%[pTarget]], #16                                \n"
                 "   le              lr, 2b                                               \n"
                 "1:                                                                      \n"

                 /* tail handling for [0-31] pixels residual */
                 /* 1/3 vector constant is in q7 and q0-q3 are already loaded */
                 "   wlstp.8         lr, %[tailCnt], 1f                                   \n"
                 "2:                                                                      \n"
                 "   vrmulh.u8       q0, q0, q7                                           \n"
                 "   vrmulh.u8       q1, q1, q7                                           \n"
                 "   vrmulh.u8       q2, q2, q7                                           \n"
                 "   vadd.i8         q0, q0, q1                                           \n"
                 "   vadd.i8         q0, q0, q2                                           \n"
                 "   vstrb.8         q0, [%[pTarget]], #16                                \n"
                 "   vld40.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vld41.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vld42.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vld43.8         {q0, q1, q2, q3}, [%[pSource]]!                      \n"
                 "   letp            lr, 2b                                               \n"
                 "1:                                                                      \n"
                 :[pSource] "+r"(pSource),[pTarget] "+r"(pTarget)
                 :[loopCnt] "r"  (blkCnt / 32),[one_third] "i"(ONE_THIRD_Q8),
                  [tailCnt] "r"(tailCnt)
                 :"q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "memory", "r14");

            pwSourceBase += iSourceStride;
            pchTargetBase += iTargetStride;
        }
    } else {
        /* iWidth lower than 32 */
        for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {
            const uint32_t *pSource = pwSourceBase;
            uint8_t        *pTarget = pchTargetBase;

            blkCnt = ptCopySize->iWidth;

            __asm volatile(
                 "   vmov.i8         q4, %[one_third]                                     \n"
                 ".p2align 2                                                              \n"
                 "   wlstp.8         lr, %[loopCnt], 1f                                   \n"
                 "2:                                                                      \n"
                 /* de-interleave RGBA channels */
                 "   vld40.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vld41.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vld42.8         {q0, q1, q2, q3}, [%[pSource]]                       \n"
                 "   vld43.8         {q0, q1, q2, q3}, [%[pSource]]!                      \n"
                 /* scale and sum R,G,B chan */
                 "   vrmulh.u8       q0, q0, q4                                           \n"
                 "   vrmulh.u8       q1, q1, q4                                           \n"
                 "   vrmulh.u8       q2, q2, q4                                           \n"
                 "   vadd.i8         q0, q0, q1                                           \n"
                 "   vadd.i8         q0, q0, q2                                           \n"
                 /* store 16 Gray8 pixels */
                 "   vstrb.8         q0, [%[pTarget]], #16                                \n"
                 "   letp            lr, 2b                                               \n"
                 "1:                                                                      \n"
                 :[pSource] "+r"(pSource),[pTarget] "+r"(pTarget)
                 :[loopCnt] "r"  (blkCnt),[one_third] "i"(ONE_THIRD_Q8)
                 :"q0", "q1", "q2", "q3",
                 "q4", "memory", "r14");

            pwSourceBase += iSourceStride;
            pchTargetBase += iTargetStride;
        }
    }
#endif
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_rgb565_to_gray8)( uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
#define ONE_THIRD_Q16 21845

    uint16x8_t      vone_third = vdupq_n_u16(ONE_THIRD_Q16);
    int32_t         blkCnt;

    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint16_t *__RESTRICT phwSource = phwSourceBase;
        uint8_t        *__RESTRICT pTarget = pchTargetBase;

        blkCnt = ptCopySize->iWidth;
#ifdef USE_MVE_INTRINSICS

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);
            uint16x8_t R,  G,  B, vecGray8;

            /* load a vector of 8 rgb565 pixels */
            uint16x8_t      vecIn = vld1q_z_u16(phwSource, tailPred);

            __arm_2d_rgb565_unpack_single_vec(vecIn, &R, &G, &B);

            /* sum R,G,B contributions */
            vecGray8 = R + G + B;
            /* average */
            vecGray8 = vrmulhq(vecGray8, vone_third);

            /* store a vector of 8 x gray8 pixels */
            vstrbq_p_u16(pTarget, vecGray8, tailPred);

            phwSource += 8;
            pTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

#else
    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);

    __asm volatile(
        /* preload pixel vector + mask R allowing more efficient pipelining */
        "   vldrh.u16       q0, [%[pSource]], #16                 \n"
        /* R ch */
        "   vand            q1, q0, %q[maskR]                     \n"


        ".p2align 2                                               \n"
        "   wlstp.16        lr, %[loopCnt], 1f                    \n"
        "2:                                                       \n"
        /* G ch */
        "   vshr.u16        q3, q0, #5                            \n"
        /* use vmul to replace left shift and allow overlap with vshr */
        "   vmul.u16        q1, q1, %[two_pow3]                   \n"
        /* B ch */
        "   vshr.u16        q2, q0, #11                           \n"
        "   vmul.u16        q2, q2, %[two_pow3]                   \n"
        "   vand            q0, q3, %q[maskG]                     \n"

        /* summing */
        "   vmla.u16        q2, q0, %[four]                       \n"
        "   vadd.i16        q2, q1, q2                            \n"

        /* load next RGB565 pixel vector */
        "   vldrh.u16       q0, [%[pSource]], #16                 \n"
        /* averaging */
        "   vrmulh.u16      q2, q2, %q[vone_third]                \n"
        /* R */
        "   vand            q1, q0, %q[maskR]                     \n"
        /* store 8 Gray8 pixels */
        "   vstrb.16        q2, [%[pTarget]], #8                  \n"

        "   letp            lr, 2b                                \n"
        "1:                                                       \n"

        : [pSource] "+r"(phwSource), [pTarget] "+l" (pTarget)
        : [loopCnt] "r"(blkCnt),[two_pow3] "r" (1<<3),
          [four] "r" (4),
          [maskR] "t" (vecMaskR),[maskG] "t" (vecMaskG),
          [vone_third] "t" (vone_third)
        : "q0", "q1", "q2", "q3", "memory", "r14" );

#endif

        phwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_gray8_to_rgb565)( uint8_t *__RESTRICT pchSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    int32_t         blkCnt;

    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint8_t *__RESTRICT pchSource = pchSourceBase;
        uint16_t      *__RESTRICT pTarget = phwTargetBase;

        blkCnt = ptCopySize->iWidth;
#ifdef USE_MVE_INTRINSICS

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);
            uint16x8_t      vecRgb565;

            /* load a vector of 8 gray pixels */
            uint16x8_t      vecGray8 = vldrbq_z_u16(pchSource, tailPred);

            /* replication in all RGB565 components */
            vecRgb565 =__arm_2d_rgb565_pack_single_vec(vecGray8, vecGray8, vecGray8);

            /* store a vector of 8 x RGB565 pixels */
            vst1q_p(pTarget, vecRgb565, tailPred);

            pchSource += 8;
            pTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

#else

    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecMaskGpck = vdupq_n_u16(0x00fc);

    __asm volatile(
        ".p2align 2                                                \n"

        /* preload pixel vector + mask G allowing more efficient pipelining */
        "   vldrb.u16       q0, [%[pSource]], #8                   \n"
        /* G ch */
        "   vand            q2, q0, %q[maskG]                      \n"

        "   wlstp.16        lr, %[loopCnt], 1f                     \n"
        "2:                                                        \n"
        /* R ch */
        "   vshr.u16        q1, q0, #3                             \n"
        /* use VMLA to mimick left shift combined with OR */
        "   vmla.u16        q1, q2, %[eight]                       \n"
        /* B ch */
        "   vand            q2, q0, %q[maskB]                      \n"
        "   vmla.u16        q1, q2, %[twofiftysix]                 \n"

        /* load next Gray pixel vector */
        "   vldrb.u16       q0, [%[pSource]], #8                   \n"
        /* G ch */
        "   vand            q2, q0, %q[maskG]                      \n"
        /* store 8 RGB565 pixels */
        "   vstrh.16        q1, [%[pTarget]], #16                  \n"
        "   letp            lr, 2b                                 \n"
        "1:                                                        \n"

        : [pSource] "+r"(pchSource), [pTarget] "+r" (pTarget)
        : [loopCnt] "r"(blkCnt),[twofiftysix] "r" (256),
          [eight] "r" (8),
          [maskG] "t" (vecMaskGpck),[maskB] "t" (vecMaskBpck)
        : "q0", "q1", "q2", "q3", "memory", "r14" );
#endif

        pchSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    }
}

__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_gray8_to_cccn888)(uint8_t *__RESTRICT pchSourceBase,
                                    int16_t iSourceStride,
                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    int32_t         blkCnt;


    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint8_t *__RESTRICT pchSource = pchSourceBase;
        uint32_t      *__RESTRICT pTarget = pwTargetBase;

        blkCnt = ptCopySize->iWidth;
#ifdef USE_MVE_INTRINSICS

        do {
            mve_pred16_t    tailPred = vctp32q(blkCnt);
            uint32x4_t      vecCcn888;

            /* load a vector of 4 gray8 pixels */
            uint32x4_t      vecGray8 = vldrbq_z_u32(pchSource, tailPred);

            /* byte duplication inside 32-bit elements */
            /* and set alpha chan to 0xff */
            vecCcn888 = (vecGray8 * 0x00010101) | 0xff000000;

            /* store a vector of 4 x ccc888 pixels */
            vst1q_p(pTarget, vecCcn888, tailPred);

            pchSource += 4;
            pTarget += 4;
            blkCnt -= 4;
        }
        while (blkCnt > 0);

#else
    int             tailCnt = ptCopySize->iWidth & 7;

    __asm volatile(
        ".p2align 2                                                \n"

        /* preload pixel vector allowing more efficient pipelining */
        "   vldrb.u32       q0, [%[pSource]], #4                   \n"

        "   wls             lr, %[loopCnt], 1f                     \n"
        "2:                                                        \n"

        "   vmul.i32        q0, q0, %[byte_dup_cst]                \n"
        "   vldrb.u32       q1, [%[pSource]], #4                   \n"
        "   vorr.i32        q0, #0xff000000                        \n"
        /* store 4 CCN888 Gray8 pixels */
        "   vstrw.32        q0, [%[pTarget]], #16                  \n"

        "   vmul.i32        q1, q1, %[byte_dup_cst]                \n"
        "   vldrb.u32       q0, [%[pSource]], #4                   \n"
        "   vorr.i32        q1, #0xff000000                        \n"
        /* store 4 CCN888 Gray8 pixels */
        "   vstrw.32        q1, [%[pTarget]], #16                  \n"

        "   le              lr, 2b                                 \n"
        "1:                                                        \n"

         /* tail handling for [0-7] pixel residual */
         /* q0 is already loaded */
        "   wlstp.32        lr, %[tailCnt], 1f                     \n"
        "2:                                                        \n"
        "   vmul.i32        q0, q0, %[byte_dup_cst]                \n"
        "   vorr.i32        q0, #0xff000000                        \n"
        /* store 4 CCN888 Gray8 pixels */
        "   vstrw.32        q0, [%[pTarget]], #16                  \n"
        "   vldrb.u32       q0, [%[pSource]], #4                   \n"
        "   letp            lr, 2b                                 \n"
        "1:                                                        \n"

        : [pSource] "+r"(pchSource), [pTarget] "+r" (pTarget)
        : [loopCnt] "r"(blkCnt/8),[byte_dup_cst] "r" (0x00010101),
          [tailCnt] "r" (tailCnt)
        : "q0", "memory", "r14" );
#endif

        pchSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;
    }
}


#ifdef   __cplusplus
}
#endif

#endif // __ARM_2D_HAS_HELIUM__

#endif // __ARM_2D_COMPILATION_UNIT
