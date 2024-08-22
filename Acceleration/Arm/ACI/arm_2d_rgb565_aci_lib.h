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
 * Title:        arm_2d_rgb565_aci_lib.h
 * Description:  Provides RGB565 ACI acceleration
 *
 * $Date:        22 Aug 2024
 * $Revision:    V.1.1.0
 *
 * Target Processor:  Cortex-M cores with Helium & RGB565 Vector CDE
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_RGB565_ACI_LIB_H__
#define __ARM_2D_RGB565_ACI_LIB_H__

/* The following library uses CDE coprocessor 0 */
#define ARM_2D_RGB565_ACI_LIB_COPRO_ID  0

#ifdef __ARM_FEATURE_CDE
/*
    // Mnemonic                  | Class          | Copro |  Opcode        | CDERTLID  | Comment
    //---------------------------|----------------|-------|----------------|-----------|----------
    // VRGB16UNPB<T>  Qd, Qm     | VCX2  (vector) |   0   |  7'b<T>000001  |   2/3/5   | Unpacking of Blue unpack
    // VRGB16UNPB<T>A Qd, Qm     | VCX2A (vector) |   0   |  7'b<T>000001  |   2/3/5   | Unpacking of Blue unpack with accumulate
    // VRGB16UNPG<T>  Qd, Qm     | VCX2  (vector) |   0   |  7'b<T>000010  |   2/3/5   | Unpacking of green unpack
    // VRGB16UNPG<T>A Qd, Qm     | VCX2A (vector) |   0   |  7'b<T>000010  |   2/3/5   | Unpacking of green unpack with accumulate
    // VRGB16UNPR<T>  Qd, Qm     | VCX2  (vector) |   0   |  7'b<T>000100  |   2/3/5   | Unpacking of Red unpack
    // VRGB16UNPR<T>A Qd, Qm     | VCX2A (vector) |   0   |  7'b<T>000100  |   2/3/5   | Unpacking of Red unpack with accumulate
    // VRGB16PR<T>A   Qd, Qm     | VCX2A (vector) |   0   |  7'b<T>001000  |    2/5    | Packing of red channel
    // VRGB16PG<T>A   Qd, Qm     | VCX2A (vector) |   0   |  7'b<T>010000  |    2/5    | Packing of green channel
    // VRGB16PB<T>A   Qd, Qm     | VCX2A (vector) |   0   |  7'b<T>100000  |    2/5    | Packing of blue channel
    // VRGB16P<T>A    Qd, Qn, Qm | VCX3A (vector) |   0   |  4'b<T>000     |    3/5    | Packing of all channels
    // VRGB16MIX      Qd, Qn, Qm | VCX3A (vector) |   0   |  4'b0001       |    4/5    | Unpacking, multiplying and packing of all channels
*/

#if (__ARM_FEATURE_CDE_COPROC & (0x1 << ARM_2D_RGB565_ACI_LIB_COPRO_ID) ) == 0
#error "__ARM_FEATURE_CDE_COPROC does not have correct feature bits set"
#endif
#define __ARM_2D_IMPL__
#include "__arm_2d_impl.h"
#include <RTE_Components.h>
#include CMSIS_device_header
#include <arm_mve.h>
#include <arm_cde.h>

__STATIC_FORCEINLINE
uint16x8_t vblda7q_rgb565(  uint16x8_t In1Out, 
                            uint16x8_t In2, 
                            uint16x8_t vu7TransRatio)
{
    return __arm_vcx3qa(ARM_2D_RGB565_ACI_LIB_COPRO_ID, In1Out, In2, vu7TransRatio, 1);
}

__STATIC_FORCEINLINE
uint16x8_t vblda7q_m_rgb565(uint16x8_t In1Out, 
                        uint16x8_t In2, 
                        uint16x8_t vu7TransRatio,
                        mve_pred16_t p)
{
    return __arm_vcx3qa_m(  ARM_2D_RGB565_ACI_LIB_COPRO_ID, 
                            In1Out, 
                            In2, 
                            vu7TransRatio, 
                            1, 
                            p);
}

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_blendq(  uint16x8_t vec1, 
                                        uint16x8_t vec2, 
                                        uint16x8_t vratio)
{
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    mve_pred16_t p = vcmpneq_n_u16(vratio, 256);
    /* ACI RGB565 Mix uses a 7-bit ratio */
    return __arm_vcx3qa_m(ARM_2D_RGB565_ACI_LIB_COPRO_ID, vec1, vec2,
            vqshrnbq_m_n_s16((int8x16_t)vratio, vratio, 1, p), 1, p);
#else
    return __arm_vcx3qa(ARM_2D_RGB565_ACI_LIB_COPRO_ID, vec1, vec2,
        vqshrnbq_n_s16((int8x16_t)vratio, vratio, 1), 1);
#endif
}


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_blendq_m(uint16x8_t vec1, 
                                        uint16x8_t vec2,
                                        uint16x8_t vratio,
                                        mve_pred16_t p)
{
    /* ACI RGB565 Mix uses a 7-bit ratio */
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    p &= vcmpneq_n_u16(vratio, 256);
#endif
    return __arm_vcx3qa_m(ARM_2D_RGB565_ACI_LIB_COPRO_ID, vec1, vec2,
            vqshrnbq_m_n_s16((int8x16_t)vratio, vratio, 1, p), 1, p);
}


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_red_unpckbq(uint16x8_t in)
{
    return __arm_vcx2q(ARM_2D_RGB565_ACI_LIB_COPRO_ID, in, 0b0000100);
}

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_red_unpckbq_m(uint16x8_t inactive, uint16x8_t in, mve_pred16_t p)
{
    return __arm_vcx2q_m(ARM_2D_RGB565_ACI_LIB_COPRO_ID, inactive, in, 0b0000100, p);
}

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_red_unpcktq(uint16x8_t in)
{
    return __arm_vcx2q(ARM_2D_RGB565_ACI_LIB_COPRO_ID, in, 0b1000100);
}

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_red_unpcktq_m(uint16x8_t inactive, uint16x8_t in, mve_pred16_t p)
{
    return __arm_vcx2q_m(ARM_2D_RGB565_ACI_LIB_COPRO_ID, inactive, in, 0b1000100, p);
}


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_green_unpckbq(uint16x8_t in)
{
    return __arm_vcx2q(ARM_2D_RGB565_ACI_LIB_COPRO_ID, in, 0b0000010);
}

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_green_unpckbq_m(uint16x8_t inactive, uint16x8_t in, mve_pred16_t p)
{
    return __arm_vcx2q_m(ARM_2D_RGB565_ACI_LIB_COPRO_ID, inactive, in, 0b0000010, p);
}

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_green_unpcktq(uint16x8_t in)
{
    return __arm_vcx2q(ARM_2D_RGB565_ACI_LIB_COPRO_ID, in, 0b1000010);
}

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_blue_unpcktq_m(uint16x8_t inactive, uint16x8_t in, mve_pred16_t p)
{
    return __arm_vcx2q_m(ARM_2D_RGB565_ACI_LIB_COPRO_ID, inactive, in, 0b1000001, p);
}

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_blue_unpckbq(uint16x8_t in)
{
    return __arm_vcx2q(ARM_2D_RGB565_ACI_LIB_COPRO_ID, in, 0b0000001);
}

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_blue_unpckbq_m(uint16x8_t inactive, uint16x8_t in, mve_pred16_t p)
{
    return __arm_vcx2q_m(ARM_2D_RGB565_ACI_LIB_COPRO_ID, inactive, in, 0b000001, p);
}

__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_blue_unpcktq(uint16x8_t in)
{
    return __arm_vcx2q(ARM_2D_RGB565_ACI_LIB_COPRO_ID, in, 0b1000001);
}


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_packbq(uint16x8_t R, uint16x8_t G,        uint16x8_t B)
{
    return __arm_vcx3qa(ARM_2D_RGB565_ACI_LIB_COPRO_ID, B, G, R, 0);
}


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_packbq_m(uint16x8_t R, uint16x8_t G,
                                                               uint16x8_t B,
                                                               mve_pred16_t p)
{
    return __arm_vcx3qa_m(ARM_2D_RGB565_ACI_LIB_COPRO_ID, B, G, R, 0, p);
}


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_packtq(uint16x8_t R, uint16x8_t G,        uint16x8_t B)
{
    return __arm_vcx3qa(ARM_2D_RGB565_ACI_LIB_COPRO_ID, B, G, R, 0b1000);
}


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_cde_rgb565_packtq_m(uint16x8_t R, uint16x8_t G,
                                                               uint16x8_t B,
                                                               mve_pred16_t p)
{
    return __arm_vcx3qa_m(ARM_2D_RGB565_ACI_LIB_COPRO_ID, B, G, R, 0b1000, p);
}



#define VRGB16_UNPACK_RED_ACC_BOT(acc, in)      __arm_vcx2qa(0, acc, in, 0b0000001)
#define VRGB16_UNPACK_RED_ACC_TOP(acc, in)      __arm_vcx2qa(0, acc, in, 0b1000001)

#define VRGB16_UNPACK_GREEN_ACC_BOT(acc, in)    __arm_vcx2qa(0, acc, in, 0b0000010)
#define VRGB16_UNPACK_GREEN_ACC_TOP(acc, in)    __arm_vcx2qa(0, acc, in, 0b1000010)

#define VRGB16_UNPACK_BLUE_ACC_BOT(acc, in)     __arm_vcx2qa(0, acc, in, 0b0000100)
#define VRGB16_UNPACK_BLUE_ACC_TOP(acc, in)     __arm_vcx2qa(0, acc, in, 0b1000100)

#define VRGB16_PACK_RED_ACC_BOT(acc, in)        __arm_vcx2qa(0, acc, in, 0b0001000)
#define VRGB16_PACK_RED_ACC_TOP(acc, in)        __arm_vcx2qa(0, acc, in, 0b1001000)

#define VRGB16_PACK_GREEN_ACC_BOT(acc, in)      __arm_vcx2qa(0, acc, in, 0b0010000)
#define VRGB16_PACK_GREEN_ACC_TOP(acc, in)      __arm_vcx2qa(0, acc, in, 0b1010000)

#define VRGB16_PACK_BLUE_ACC_BOT(acc, in)       __arm_vcx2qa(0, acc, in, 0b0100000)
#define VRGB16_PACK_BLUE_ACC_TOP(acc, in)       __arm_vcx2qa(0, acc, in, 0b1100000)

void __arm_2d_aci_init(void);

#endif                          /* __ARM_FEATURE_CDE  */
#endif                          /* __ARM_2D_RGB565_ACI_LIB_H__ */
