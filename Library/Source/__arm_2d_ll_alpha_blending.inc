
/*
 * Copyright (C) 2010-2021 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        __arm_2d_alpha_blending.inc
 * Description:  c code template for drawing pattern
 *
 * $Date:        07. Feb 2023
 * $Revision:    V.1.1.1
 *
 * -------------------------------------------------------------------- */


#ifndef __API_COLOUR
#   error You have to define __API_COLOUR before using this c template
#endif
#ifndef __API_INT_TYPE
#   error You have to define the __API_INT_TYPE before using this c template
#endif
#ifndef __API_PIXEL_BLENDING
#   error You have to define __API_PIXEL_BLENDING before using this c template
#endif

#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#define ____ARM_2D_FUNC(__NAME, __COLOUR)  __arm_2d_impl_##__COLOUR##_##__NAME
#define ___ARM_2D_FUNC(__NAME, __COLOUR)   ____ARM_2D_FUNC(__NAME, __COLOUR)
#define __ARM_2D_FUNC(__NAME)   ___ARM_2D_FUNC(__NAME, __API_COLOUR)

#ifndef __PATCH_ALPHA_BLENDING
__WEAK
void __ARM_2D_FUNC(tile_copy_opacity) (__API_INT_TYPE *__RESTRICT pSourceBase,
                                    int16_t iSourceStride,
                                    __API_INT_TYPE *__RESTRICT pTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif

    uint16_t        hwRatioCompl = 256 - hwRatio;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __API_PIXEL_BLENDING(pSourceBase++, pTargetBase++, hwRatioCompl);
        }

        pSourceBase += (iSourceStride - iWidth);
        pTargetBase += (iTargetStride - iWidth);
    }

}
#else
extern
void __ARM_2D_FUNC(tile_copy_opacity) (__API_INT_TYPE *__RESTRICT pSourceBase,
                                    int16_t iSourceStride,
                                    __API_INT_TYPE *__RESTRICT pTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwRatio);
#endif



#ifndef __PATCH_ALPHA_BLENDING_COLOUR_MASKING
__WEAK
void __ARM_2D_FUNC(tile_copy_colour_keying_opacity)(
                                                __API_INT_TYPE * __RESTRICT pSourceBase,
                                                int16_t         iSourceStride,
                                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                                int16_t         iTargetStride,
                                                arm_2d_size_t * __RESTRICT ptCopySize,
                                                uint_fast16_t hwRatio,
                                                __API_INT_TYPE   Colour)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif

    uint16_t        hwRatioCompl = 256 - hwRatio;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (*pSourceBase != Colour) {
                __API_PIXEL_BLENDING( pSourceBase, pTargetBase, hwRatioCompl);
            }
            pSourceBase++;
            pTargetBase++;
        }
        pSourceBase += (iSourceStride - iWidth);
        pTargetBase += (iTargetStride - iWidth);
    }
}
#else
extern
void __ARM_2D_FUNC(tile_copy_colour_keying_opacity)(
                                                __API_INT_TYPE * __RESTRICT pSourceBase,
                                                int16_t         iSourceStride,
                                                __API_INT_TYPE * __RESTRICT pTargetBase,
                                                int16_t         iTargetStride,
                                                arm_2d_size_t * __RESTRICT ptCopySize,
                                                uint_fast16_t hwRatio,
                                                __API_INT_TYPE   Colour);
#endif


#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#undef __API_COLOUR
#undef __API_INT_TYPE
#undef __API_PIXEL_BLENDING
#undef __PATCH_ALPHA_BLENDING
#undef __PATCH_ALPHA_BLENDING_COLOUR_MASKING

