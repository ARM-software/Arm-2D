/*
 * Copyright (C) 2010-2023 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        arm-2d_transform_helium.c
 * Description:  Acceleration extensions using Helium.
 *
 * $Date:        28. April 2025
 * $Revision:    V.2.0.0
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




#include "__arm_2d_math_helium.h"
#include "__arm_2d_utils_helium.h"
#ifdef   __cplusplus
extern "C" {
#endif

static
mve_pred16_t arm_2d_is_point_vec_inside_region_s16(const arm_2d_region_t * ptRegion,
                                               const arm_2d_point_s16x8_t * ptPoint)
{
    mve_pred16_t    p0 = vcmpgeq(ptPoint->X, ptRegion->tLocation.iX);
    p0 = vcmpgeq_m(ptPoint->Y, ptRegion->tLocation.iY, p0);
    p0 = vcmpltq_m(ptPoint->X, ptRegion->tLocation.iX + ptRegion->tSize.iWidth, p0);
    p0 = vcmpltq_m(ptPoint->Y, ptRegion->tLocation.iY + ptRegion->tSize.iHeight, p0);

    return p0;
}

static
mve_pred16_t arm_2d_is_point_vec_inside_region_s16_safe(const arm_2d_region_t * ptRegion,
                                               const arm_2d_point_s16x8_t * ptPoint)
{
    mve_pred16_t    p0 = vcmpgeq(ptPoint->X, ptRegion->tLocation.iX);
    p0 = vcmpgeq_m(ptPoint->Y, ptRegion->tLocation.iY, p0);
    p0 = vcmpltq_m(ptPoint->X, ptRegion->tLocation.iX + ptRegion->tSize.iWidth - 1, p0);
    p0 = vcmpltq_m(ptPoint->Y, ptRegion->tLocation.iY + ptRegion->tSize.iHeight - 1, p0);

    return p0;
}

static
mve_pred16_t arm_2d_is_point_vec_inside_region_s32(const arm_2d_region_t * ptRegion,
                                               const arm_2d_point_s32x4_t * ptPoint)
{
    mve_pred16_t    p0 = vcmpgeq_n_s32(ptPoint->X, ptRegion->tLocation.iX);
    p0 = vcmpgeq_m_n_s32(ptPoint->Y, ptRegion->tLocation.iY, p0);
    p0 = vcmpltq_m_n_s32(ptPoint->X, ptRegion->tLocation.iX + ptRegion->tSize.iWidth, p0);
    p0 = vcmpltq_m_n_s32(ptPoint->Y, ptRegion->tLocation.iY + ptRegion->tSize.iHeight, p0);

    return p0;
}


/**
  @brief         return 3 vector of 16-bit channels (8-bit widened) taken from a memory reference
  @param[in]     pMem           pointer to packed 8-bit channel
  @param[out]    R              vector of 16-bit widened R channel
  @param[out]    G              vector of 16-bit widened G channel
  @param[out]    B              vector of 16-bit widened B channel
 */
void __arm_2d_unpack_rgb888_from_mem(const uint8_t * pMem, uint16x8_t * R, uint16x8_t * G,
                                     uint16x8_t * B)
{
    uint16x8_t      sg = vidupq_n_u16(0, 4);

    *B = vldrbq_gather_offset_u16(pMem, sg);
    *G = vldrbq_gather_offset_u16(pMem + 1, sg);
    *R = vldrbq_gather_offset_u16(pMem + 2, sg);
}

/**
  @brief         interleave 3 x 16-bit widened vectors into 8-bit memory reference
                 (4th channel untouched)
  @param[in]     pMem           pointer to packed 8-bit channel
  @param[in]     R              vector of 16-bit widened R channel
  @param[in]     G              vector of 16-bit widened G channel
  @param[in]     B              vector of 16-bit widened B channel
 */
void __arm_2d_pack_rgb888_to_mem(uint8_t * pMem, uint16x8_t R, uint16x8_t G, uint16x8_t B)
{
    uint16x8_t      sg = vidupq_n_u16(0, 4);

    vstrbq_scatter_offset_u16(pMem,     sg, vminq(B, vdupq_n_u16(255)));
    vstrbq_scatter_offset_u16(pMem + 1, sg, vminq(G, vdupq_n_u16(255)));
    vstrbq_scatter_offset_u16(pMem + 2, sg, vminq(R, vdupq_n_u16(255)));
    //vstrbq_scatter_offset_u16(pMem + 3, sg, vdupq_n_u16(0));
}



/* selector for 16-bit vector gather load */
/* FAR version is been used when offsets involved are above 65535 16-bit words */
#define FAR_OFFSET   _FAR
#define NEAR_OFFSET


/**
  unpack vectors of 8-bit widened pixels read from a input 2D coordinates if fits inside the region of
  interest or alternative target pixel if content matches color mask
  (vector of packed pixels & region of interest name implicit and fixed to respectively
  vTarget and ptOrigValidRegion)
  Update global predictor tracking region fit & color mask comparison.
 */
#define __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(/* inputs */                                           \
                                                      vecX, vecY, pOrigin, ptOrigValidRegion,       \
                                                      iOrigStride, predTail,                        \
                                                      /* outputs */                                 \
                                                      vPixVal, predGlb)                             \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                       \
        /* set vector predicate if point is inside the region */                                    \
        mve_pred16_t    p =                                                                         \
            arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);                      \
        predGlb |= p;                                                                               \
        /* prepare vector of point offsets */                                                       \
        int16_t         correctionOffset = vminvq_s16(INT16_MAX, vPoint.Y) - 1;                     \
        uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;            \
                                                                                                    \
        /* base pointer update to compensate offset */                                              \
        uint8_t       *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);               \
        /* retrieve all point values */                                                             \
        vPixVal =                                                                                   \
            vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);


#define __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR(/* inputs */                                       \
                                                      vecX, vecY, pOrigin, ptOrigValidRegion,       \
                                                      iOrigStride, predTail,                        \
                                                      /* outputs */                                 \
                                                      vPixVal, predGlb)                             \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vecX, vecY, pOrigin, ptOrigValidRegion,                \
                                    iOrigStride, predTail, vPixVal, predGlb);                       \
                                                                                                    \
        /* combine 2 predicates set to true if point is in the region & values */                   \
        /*  different from color mask */                                                            \
        vPixVal = vpselq_u16(vPixVal, vTarget, p);
/**
  unpack vectors of 8-bit widened pixels read from a input 2D coordinates if fits inside the region of
  interest or alternative target pixel if content matches color mask
  (vector of packed pixels & region of interest name implicit and fixed to respectively
  vTarget and ptOrigValidRegion)
  Update global predictor tracking region fit & color mask comparison.
 */
#define __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(/* inputs */                                \
                                                      vecX, vecY, pOrigin, ptOrigValidRegion,       \
                                                      iOrigStride,                                  \
                                                      /* outputs */                                 \
                                                      vPixVal)                                      \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                       \
        /* prepare vector of point offsets */                                                       \
        int16_t         correctionOffset = vminvq_s16(INT16_MAX, vPoint.Y) - 1;                     \
        uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;            \
                                                                                                    \
        /* base pointer update to compensate offset */                                              \
        uint8_t       *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);               \
        /* retrieve all point values */                                                             \
        vPixVal = vldrbq_gather_offset_u16(pOriginCorrected, ptOffs);

#define __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR_INSIDE_SRC                                        \
                                                    __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC


#define __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(/* inputs */                              \
                                                      vecX, vecY, pOrigin, ptOrigValidRegion,       \
                                                      iOrigStride, MaskColour, vTarget, predTail,   \
                                                      /* outputs */                                 \
                                                      vPixVal, predGlb)                             \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vecX, vecY, pOrigin, ptOrigValidRegion,                \
                                    iOrigStride, predTail, vPixVal, predGlb);                       \
                                                                                                    \
        /* combine 2 predicates set to true if point is in the region & values */                   \
        /*  different from color mask */                                                            \
        p = vcmpneq_m_n_u16(vPixVal, MaskColour, p);                                                \
        predGlb |= p;                                                                               \
        vPixVal = vpselq_u16(vPixVal, vTarget, p);


#define __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR_INSIDE_SRC(/* inputs */                   \
                                                      vecX, vecY, pOrigin, ptOrigValidRegion,       \
                                                      iOrigStride, MaskColour, vTarget,             \
                                                      /* outputs */                                 \
                                                      vPixVal)                                      \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(vecX, vecY, pOrigin, ptOrigValidRegion,     \
                                    iOrigStride, vPixVal);                                          \
                                                                                                    \
        /* combine 2 predicates set to true if point is in the region & values */                   \
        /*  different from color mask */                                                            \
        vPixVal = vpselq_u16(vPixVal, vTarget, vcmpneq_n_u16(vPixVal, MaskColour));


/**
  unpack vectors of pixels read from a input 2D coordinates if fits inside the region of
  interest or alternative target pixel if content matches color mask
  (vector of packed pixels & region of interest name implicit and fixed to respectively
  vTarget and ptOrigValidRegion)
  Update global predictor tracking region fit & color mask comparison.
 */

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK(/* inputs */                                          \
                                                    vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,   \
                                                    predTail,                                              \
                                                    /* outputs */                                          \
                                                    ptVal, predGlb)                                        \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        /* set vector predicate if point is inside the region */                                           \
        mve_pred16_t    p =                                                                                \
            arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);                             \
                                                                                                           \
        /* prepare vector of point offsets */                                                              \
        uint16x8_t      ptOffs = vPoint.X + vPoint.Y * iOrigStride;                                        \
         /* retrieve all point values */                                                                   \
        ptVal =                                                                                            \
            vldrhq_gather_shifted_offset_z_u16(pOrigin, ptOffs, predTail & p);                             \
        predGlb |= p;                                                                                      \
        ptVal = vpselq_u16(ptVal, vTarget, p);

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_INSIDE_SRC(/* inputs */                               \
                                                    vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,   \
                                                    /* outputs */                                          \
                                                    ptVal)                                                 \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        /* set vector predicate if point is inside the region */                                           \
                                                                                                           \
        /* prepare vector of point offsets */                                                              \
        uint16x8_t      ptOffs = vPoint.X + vPoint.Y * iOrigStride;                                        \
         /* retrieve all point values */                                                                   \
        ptVal = vldrhq_gather_shifted_offset_u16(pOrigin, ptOffs);


#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK(/* inputs */                                     \
                                                    vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,   \
                                                    MaskColour, vTarget, predTail,                         \
                                                    /* outputs */                                          \
                                                    ptVal, predGlb)                                        \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        /* set vector predicate if point is inside the region */                                           \
        mve_pred16_t    p =                                                                                \
            arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);                             \
                                                                                                           \
        /* prepare vector of point offsets */                                                              \
        uint16x8_t      ptOffs = vPoint.X + vPoint.Y * iOrigStride;                                        \
         /* retrieve all point values */                                                                   \
        ptVal =                                                                                            \
            vldrhq_gather_shifted_offset_z_u16(pOrigin, ptOffs, predTail & p);                             \
                                                                                                           \
        /* combine 2 predicates set to true if point is in the region & values different from color mask */\
        p = vcmpneq_m_n_u16(ptVal, MaskColour, p);                                                         \
        predGlb |= p;                                                                                      \
        ptVal = vpselq_u16(ptVal, vTarget, p);

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_INSIDE_SRC(/* inputs */                          \
                                                    vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,   \
                                                    MaskColour, vTarget,                                   \
                                                    /* outputs */                                          \
                                                    ptVal)                                                 \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_INSIDE_SRC(vecX, vecY, pOrigin, ptOrigValidRegion,    \
                           iOrigStride, ptVal);                                                            \
                                                                                                           \
        /* combine 2 predicates set to true if point is in the region & values different from color mask */\
        ptVal = vpselq_u16(ptVal, vTarget, vcmpneq_n_u16(ptVal, MaskColour));

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT(/* inputs */                                                 \
                                              vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                              predTail,                                                    \
                                              /* outputs */                                                \
                                              vPixValR, vPixValG, vPixValB, predGlb)                       \
        uint16x8_t      ptVal;                                                                             \
                                                                                                           \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK(vecX, vecY, pOrigin, ptOrigValidRegion,               \
                                                        iOrigStride, predTail, ptVal, predGlb)             \
        /* expand channels */                                                                              \
        __arm_2d_rgb565_unpack_single_vec(ptVal, &vPixValR, &vPixValG, &vPixValB);

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_INSIDE_SRC(/* inputs */                                      \
                                              vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                              /* outputs */                                                \
                                              vPixValR, vPixValG, vPixValB)                                \
        uint16x8_t      ptVal;                                                                             \
                                                                                                           \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_INSIDE_SRC(vecX, vecY, pOrigin, ptOrigValidRegion,    \
                                                                iOrigStride, ptVal)                        \
        /* expand channels */                                                                              \
        __arm_2d_rgb565_unpack_single_vec(ptVal, &vPixValR, &vPixValG, &vPixValB);

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR(/* inputs */                                        \
                                              vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                              MaskColour, vTarget, predTail,                               \
                                              /* outputs */                                                \
                                              vPixValR, vPixValG, vPixValB, predGlb)                       \
        uint16x8_t      ptVal;                                                                             \
                                                                                                           \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK(vecX, vecY, pOrigin, ptOrigValidRegion,          \
                                       iOrigStride, MaskColour, vTarget, predTail, ptVal, predGlb)         \
        /* expand channels */                                                                              \
        __arm_2d_rgb565_unpack_single_vec(ptVal, &vPixValR, &vPixValG, &vPixValB);

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR_INSIDE_SRC(/* inputs */                             \
                                              vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                              MaskColour, vTarget,                                         \
                                              /* outputs */                                                \
                                              vPixValR, vPixValG, vPixValB)                                \
        uint16x8_t      ptVal;                                                                             \
                                                                                                           \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_INSIDE_SRC(                                      \
                                        vecX, vecY, pOrigin, ptOrigValidRegion,                            \
                                        iOrigStride, MaskColour, vTarget, ptVal)                           \
        /* expand channels */                                                                              \
        __arm_2d_rgb565_unpack_single_vec(ptVal, &vPixValR, &vPixValG, &vPixValB);



/**
  Same as above but use offset compensation during gather load.
  unpack vectors of pixels read from a input 2D coordinates if fits inside the region of
  interest or alternative target pixel if content matches color mask
  (vector of packed pixels & region of interest name implicit and fixed to respectively
  vTarget and ptOrigValidRegion)
  Update global predictor tracking region fit & color mask comparison.
 */

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_FAR(                                                  \
                                                /* inputs */                                               \
                                                vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,       \
                                                predTail,                                                  \
                                                /* outputs */                                              \
                                                ptVal, predGlb)                                            \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        /* set vector predicate if point is inside the region */                                           \
        mve_pred16_t    p =                                                                                \
            arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);                             \
        /* prepare vector of point offsets */                                                              \
        int16_t         correctionOffset = vminvq_s16(INT16_MAX, vPoint.Y) - 1;                            \
        uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;                   \
                                                                                                           \
        /* base pointer update to compensate offset */                                                     \
        uint16_t       *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);                     \
        /* retrieve all point values */                                                                    \
        ptVal =                                                                                            \
            vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);                    \
        predGlb |= p;                                                                                      \
        ptVal = vpselq_u16(ptVal, vTarget, p);

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_FAR_INSIDE_SRC(                                       \
                                                /* inputs */                                               \
                                                vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,       \
                                                /* outputs */                                              \
                                                ptVal)                                                     \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        /* prepare vector of point offsets */                                                              \
        int16_t         correctionOffset = vminvq_s16(INT16_MAX, vPoint.Y) - 1;                            \
        uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;                   \
                                                                                                           \
        /* base pointer update to compensate offset */                                                     \
        uint16_t       *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);                     \
        /* retrieve all point values */                                                                    \
        ptVal = vldrhq_gather_shifted_offset_u16(pOriginCorrected, ptOffs);


#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_FAR(                                             \
                                                /* inputs */                                               \
                                                vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,       \
                                                MaskColour, vTarget, predTail,                             \
                                                /* outputs */                                              \
                                                ptVal, predGlb)                                            \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        /* set vector predicate if point is inside the region */                                           \
        mve_pred16_t    p =                                                                                \
            arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);                             \
        /* prepare vector of point offsets */                                                              \
        int16_t         correctionOffset = vminvq_s16(INT16_MAX, vPoint.Y) - 1;                            \
        uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;                   \
                                                                                                           \
        /* base pointer update to compensate offset */                                                     \
        uint16_t       *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);                     \
        /* retrieve all point values */                                                                    \
        ptVal =                                                                                            \
            vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);                    \
                                                                                                           \
        /* combine 2 predicates set to true if point is in the region & values different from color mask */\
        p = vcmpneq_m_n_u16(ptVal, MaskColour, p);                                                         \
        predGlb |= p;                                                                                      \
        ptVal = vpselq_u16(ptVal, vTarget, p);

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_FAR_INSIDE_SRC(                                  \
                                                /* inputs */                                               \
                                                vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,       \
                                                MaskColour, vTarget,                                       \
                                                /* outputs */                                              \
                                                ptVal)                                                     \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        /* prepare vector of point offsets */                                                              \
        int16_t         correctionOffset = vminvq_s16(INT16_MAX, vPoint.Y) - 1;                            \
        uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;                   \
                                                                                                           \
        /* base pointer update to compensate offset */                                                     \
        uint16_t       *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);                     \
        /* retrieve all point values */                                                                    \
        ptVal = vldrhq_gather_shifted_offset_u16(pOriginCorrected, ptOffs);                                \
                                                                                                           \
        /* set to true if point values different from color mask */                                        \
        ptVal = vpselq_u16(ptVal, vTarget, vcmpneq_n_u16(ptVal, MaskColour));


#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_FAR(/* inputs */                                             \
                                                  vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,     \
                                                  predTail,                                                \
                                                  /* outputs */                                            \
                                                  vPixValR, vPixValG, vPixValB, predGlb)                   \
        uint16x8_t      ptVal;                                                                             \
                                                                                                           \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_FAR(vecX, vecY, pOrigin, ptOrigValidRegion,           \
                                        iOrigStride, predTail, ptVal, predGlb);                            \
                                                                                                           \
        /* expand channels */                                                                              \
        __arm_2d_rgb565_unpack_single_vec(ptVal, &vPixValR, &vPixValG, &vPixValB);

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_FAR_INSIDE_SRC(/* inputs */                                  \
                                                  vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,     \
                                                  /* outputs */                                            \
                                                  vPixValR, vPixValG, vPixValB)                            \
        uint16x8_t      ptVal;                                                                             \
                                                                                                           \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_FAR_INSIDE_SRC(vecX, vecY, pOrigin, ptOrigValidRegion,\
                                        iOrigStride, ptVal);                                               \
                                                                                                           \
        /* expand channels */                                                                              \
        __arm_2d_rgb565_unpack_single_vec(ptVal, &vPixValR, &vPixValG, &vPixValB);

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR_FAR(/* inputs */                                    \
                                                  vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,     \
                                                  MaskColour, vTarget, predTail,                           \
                                                  /* outputs */                                            \
                                                  vPixValR, vPixValG, vPixValB, predGlb)                   \
        uint16x8_t      ptVal;                                                                             \
                                                                                                           \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_FAR(vecX, vecY, pOrigin, ptOrigValidRegion,      \
                                        iOrigStride, MaskColour, vTarget, predTail, ptVal, predGlb);       \
                                                                                                           \
        /* expand channels */                                                                              \
        __arm_2d_rgb565_unpack_single_vec(ptVal, &vPixValR, &vPixValG, &vPixValB);

#define __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR_FAR_INSIDE_SRC(/* inputs */                         \
                                                  vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,     \
                                                  MaskColour, vTarget,                                     \
                                                  /* outputs */                                            \
                                                  vPixValR, vPixValG, vPixValB)                            \
        uint16x8_t      ptVal;                                                                             \
                                                                                                           \
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_FAR_INSIDE_SRC(                                  \
                                                        vecX, vecY, pOrigin, ptOrigValidRegion,            \
                                                        iOrigStride, MaskColour, vTarget, ptVal);          \
                                                                                                           \
        /* expand channels */                                                                              \
        __arm_2d_rgb565_unpack_single_vec(ptVal, &vPixValR, &vPixValG, &vPixValB);

/**
  unpack vectors of 32-bit pixels read from a input 2D coordinates if fits inside the region of
  interest or alternative target pixel if content matches color mask
  16-bit vector processed in 2 parts because of 32-bit requirements, so handles 8 x 32-bit vectors
  (vectors of packed pixels & region of interest name implicit and fixed to respectively
  vTargetLo, vectorHi and ptOrigValidRegion)
  Update 2 global predictors tracking region fit & color mask comparison for 1st and 2nd half.
 */

#define __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK(/* inputs */ \
                                                    vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,   \
                                                    predTailLo, predTailHi,                                \
                                                    /* outputs */                                          \
                                                    pointLo, pointHi, predGlbLo, predGlbHi)                \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        arm_2d_point_s32x4_t tPointLo, tPointHi;                                                           \
        ARM_ALIGN(8) uint32_t scratch32[32];                                                               \
        int16_t        *pscratch16 = (int16_t *) scratch32;                                                \
                                                                                                           \
        /* split 16-bit point vector into 2 x 32-bit vectors */                                            \
        vst1q(pscratch16, vPoint.X);                                                                       \
        tPointLo.X = vldrhq_s32(pscratch16);                                                               \
        tPointHi.X = vldrhq_s32(pscratch16 + 4);                                                           \
                                                                                                           \
        vst1q(pscratch16, vPoint.Y);                                                                       \
        tPointLo.Y = vldrhq_s32(pscratch16);                                                               \
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);                                                           \
                                                                                                           \
        /* 1st half */                                                                                     \
                                                                                                           \
        /* set vector predicate if point is inside the region */                                           \
        mve_pred16_t    p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);           \
        /* prepare vector of point offsets */                                                              \
        uint32x4_t      ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;                                    \
                                                                                                           \
        /* retrieve all point values */                                                                    \
        pointLo = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLo & p);                     \
        predGlbLo |= p;                                                                                    \
        pointLo = vpselq_u32(pointLo, vTargetLo, p);                                                       \
                                                                                                           \
        /* 2nd half */                                                                                     \
                                                                                                           \
        /* set vector predicate if point is inside the region */                                           \
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);                           \
        /* prepare vector of point offsets */                                                              \
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;                                                    \
                                                                                                           \
        /* retrieve all point values */                                                                    \
        pointHi = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHi & p);                     \
        predGlbHi |= p;                                                                                    \
        pointHi = vpselq_u32(pointHi, vTargetHi, p);

#define __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK_INSIDE_SRC(/* inputs */                               \
                                                    vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,   \
                                                    /* outputs */                                          \
                                                    pointLo, pointHi)                                      \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        arm_2d_point_s32x4_t tPointLo, tPointHi;                                                           \
        ARM_ALIGN(8) uint32_t scratch32[32];                                                               \
        int16_t        *pscratch16 = (int16_t *) scratch32;                                                \
                                                                                                           \
        /* split 16-bit point vector into 2 x 32-bit vectors */                                            \
        vst1q(pscratch16, vPoint.X);                                                                       \
        tPointLo.X = vldrhq_s32(pscratch16);                                                               \
        tPointHi.X = vldrhq_s32(pscratch16 + 4);                                                           \
                                                                                                           \
        vst1q(pscratch16, vPoint.Y);                                                                       \
        tPointLo.Y = vldrhq_s32(pscratch16);                                                               \
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);                                                           \
                                                                                                           \
        /* 1st half */                                                                                     \
                                                                                                           \
        /* prepare vector of point offsets */                                                              \
        uint32x4_t      ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;                                    \
                                                                                                           \
        /* retrieve all point values */                                                                    \
        pointLo = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);                                       \
                                                                                                           \
        /* 2nd half */                                                                                     \
                                                                                                           \
        /* prepare vector of point offsets */                                                              \
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;                                                    \
                                                                                                           \
        /* retrieve all point values */                                                                    \
        pointHi = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);

#define __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_CLR(/* inputs */                                 \
                                                    vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,   \
                                                    MaskColour, vTargetLo, vTargetHi,                      \
                                                    predTailLo, predTailHi,                                \
                                                    /* outputs */                                          \
                                                    pointLo, pointHi, predGlbLo, predGlbHi)                \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        arm_2d_point_s32x4_t tPointLo, tPointHi;                                                           \
        ARM_ALIGN(8) uint32_t scratch32[32];                                                               \
        int16_t        *pscratch16 = (int16_t *) scratch32;                                                \
                                                                                                           \
        /* split 16-bit point vector into 2 x 32-bit vectors */                                            \
        vst1q(pscratch16, vPoint.X);                                                                       \
        tPointLo.X = vldrhq_s32(pscratch16);                                                               \
        tPointHi.X = vldrhq_s32(pscratch16 + 4);                                                           \
                                                                                                           \
        vst1q(pscratch16, vPoint.Y);                                                                       \
        tPointLo.Y = vldrhq_s32(pscratch16);                                                               \
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);                                                           \
                                                                                                           \
        /* 1st half */                                                                                     \
                                                                                                           \
        /* set vector predicate if point is inside the region */                                           \
        mve_pred16_t    p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);           \
        /* prepare vector of point offsets */                                                              \
        uint32x4_t      ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;                                    \
                                                                                                           \
        /* retrieve all point values */                                                                    \
        pointLo = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLo & p);                     \
                                                                                                           \
        /* combine 2 predicates set to true if point is in the region & values different from color mask */\
        p = vcmpneq_m_n_u32(pointLo, MaskColour, p);                                                       \
        predGlbLo |= p;                                                                                    \
        pointLo = vpselq_u32(pointLo, vTargetLo, p);                                                       \
                                                                                                           \
                                                                                                           \
        /* 2nd half */                                                                                     \
                                                                                                           \
        /* set vector predicate if point is inside the region */                                           \
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);                           \
        /* prepare vector of point offsets */                                                              \
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;                                                    \
                                                                                                           \
        /* retrieve all point values */                                                                    \
        pointHi = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHi & p);                     \
                                                                                                           \
        /* combine 2 predicates set to true if point is in the region & values different from color mask */\
        p = vcmpneq_m_n_u32(pointHi, MaskColour, p);                                                       \
        predGlbHi |= p;                                                                                    \
        pointHi = vpselq_u32(pointHi, vTargetHi, p);

#define __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_CLR_INSIDE_SRC(/* inputs */                      \
                                                    vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,   \
                                                    MaskColour, vTargetLo, vTargetHi,                      \
                                                    /* outputs */                                          \
                                                    pointLo, pointHi)                                      \
        arm_2d_point_s16x8_t vPoint = {.X = vecX,.Y = vecY };                                              \
        arm_2d_point_s32x4_t tPointLo, tPointHi;                                                           \
        ARM_ALIGN(8) uint32_t scratch32[32];                                                               \
        int16_t        *pscratch16 = (int16_t *) scratch32;                                                \
                                                                                                           \
        /* split 16-bit point vector into 2 x 32-bit vectors */                                            \
        vst1q(pscratch16, vPoint.X);                                                                       \
        tPointLo.X = vldrhq_s32(pscratch16);                                                               \
        tPointHi.X = vldrhq_s32(pscratch16 + 4);                                                           \
                                                                                                           \
        vst1q(pscratch16, vPoint.Y);                                                                       \
        tPointLo.Y = vldrhq_s32(pscratch16);                                                               \
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);                                                           \
                                                                                                           \
        /* 1st half */                                                                                     \
                                                                                                           \
        /* prepare vector of point offsets */                                                              \
        uint32x4_t      ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;                                    \
                                                                                                           \
        /* retrieve all point values */                                                                    \
        pointLo = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);                                       \
                                                                                                           \
        /* predicates set to true if point values different from color mask */                             \
        pointLo = vpselq_u32(pointLo, vTargetLo, vcmpneq_n_u32(pointLo, MaskColour));                      \
                                                                                                           \
                                                                                                           \
        /* 2nd half */                                                                                     \
                                                                                                           \
        /* prepare vector of point offsets */                                                              \
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;                                                    \
                                                                                                           \
        /* retrieve all point values */                                                                    \
        pointHi = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);                                       \
                                                                                                           \
        /* predicates set to true if point values different from color mask */                             \
        pointHi = vpselq_u32(pointHi, vTargetHi, vcmpneq_n_u32(pointHi, MaskColour));


#define __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(/* inputs */                                                 \
                                                vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,       \
                                                predTailLo, predTailHi,                                    \
                                              /* outputs */                                                \
                                                 vPixValR, vPixValG, vPixValB, predGlbLo, predGlbHi )      \
                                                                                                           \
        uint32x4_t      pointLo, pointHi;                                                                  \
                                                                                                           \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK(vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,  \
                 predTailLo, predTailHi,                                                                   \
                                                   pointLo, pointHi, predGlbLo, predGlbHi)                 \
                                                                                                           \
        /* expand channels */                                                                              \
        vst1q(scratch32, pointLo);                                                                         \
        vst1q(scratch32 + 4, pointHi);                                                                     \
                                                                                                           \
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &vPixValR, &vPixValG, &vPixValB);

#define __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_INSIDE_SRC(/* inputs */                                      \
                                                vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,       \
                                              /* outputs */                                                \
                                                 vPixValR, vPixValG, vPixValB )                            \
                                                                                                           \
        uint32x4_t      pointLo, pointHi;                                                                  \
                                                                                                           \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK_INSIDE_SRC(                                           \
                                                vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,       \
                                                pointLo, pointHi)                                          \
                                                                                                           \
        /* expand channels */                                                                              \
        vst1q(scratch32, pointLo);                                                                         \
        vst1q(scratch32 + 4, pointHi);                                                                     \
                                                                                                           \
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &vPixValR, &vPixValG, &vPixValB);

#define __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR(/* inputs */                                        \
                                                vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,       \
                                                MaskColour, vTargetLo, vTargetHi, predTailLo, predTailHi,  \
                                              /* outputs */                                                \
                                                 vPixValR, vPixValG, vPixValB, predGlbLo, predGlbHi )      \
                                                                                                           \
        uint32x4_t      pointLo, pointHi;                                                                  \
                                                                                                           \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_CLR(vecX, vecY, pOrigin, ptOrigValidRegion,      \
                                  iOrigStride, MaskColour, vTargetLo, vTargetHi, predTailLo, predTailHi,   \
                                                   pointLo, pointHi, predGlbLo, predGlbHi)                 \
                                                                                                           \
        /* expand channels */                                                                              \
        vst1q(scratch32, pointLo);                                                                         \
        vst1q(scratch32 + 4, pointHi);                                                                     \
                                                                                                           \
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &vPixValR, &vPixValG, &vPixValB);

#define __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR_INSIDE_SRC(/* inputs */                             \
                                                vecX, vecY, pOrigin, ptOrigValidRegion, iOrigStride,       \
                                                MaskColour, vTargetLo, vTargetHi,                          \
                                              /* outputs */                                                \
                                                 vPixValR, vPixValG, vPixValB)                             \
                                                                                                           \
        uint32x4_t      pointLo, pointHi;                                                                  \
                                                                                                           \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_CLR_INSIDE_SRC(                                  \
                                  vecX, vecY, pOrigin, ptOrigValidRegion,                                  \
                                  iOrigStride, MaskColour, vTargetLo, vTargetHi,                           \
                                  pointLo, pointHi)                                                        \
                                                                                                           \
        /* expand channels */                                                                              \
        vst1q(scratch32, pointLo);                                                                         \
        vst1q(scratch32 + 4, pointHi);                                                                     \
                                                                                                           \
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &vPixValR, &vPixValG, &vPixValB);


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__

#define __ARM2D_AVG_NEIGHBR_GRAY8_PIX(/* inputs */ \
                                      ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                      predTail,                                                           \
                                      /* outputs */                                                       \
                                      predGlb, vAvgPixel)                                                 \
                                                                                                          \
    uint16x8_t      ptVal8;                                                                               \
    /* combination of Bottom / Top & Left / Right areas contributions */                                  \
    __typeof__ (vAvgPixel)    vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                         \
                                                                                                          \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                 \
                                                                                                          \
    /*                                                                                                    \
     * accumulate / average over the 4 neigbouring pixels                                                 \
     */                                                                                                   \
                                                                                                          \
    /* Bottom Left averaging */                                                                           \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,                    \
                                    iOrigStride, predTail, ptVal8, predGlb);                              \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC(vAvgPixel, ptVal8, vAreaBL);                                              \
    }                                                                                                     \
                                                                                                          \
    /* Bottom Right averaging */                                                                          \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR(vaddq_n_s16(vXi, 1), vYi, pOrigin,                       \
                ptOrigValidRegion, iOrigStride, predTail, ptVal8, predGlb);                               \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaBR);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Top Left averaging */                                                                              \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR(vXi, vaddq_n_s16(vYi, 1), pOrigin,                       \
                 ptOrigValidRegion, iOrigStride, predTail, ptVal8, predGlb);                              \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaTL);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Top Right averaging */                                                                             \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),                \
            pOrigin, ptOrigValidRegion, iOrigStride, predTail, ptVal8, predGlb);                          \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaTR);                                          \
    }

#define __ARM2D_AVG_NEIGHBR_GRAY8_PIX_INSIDE_SRC(/* inputs */                                             \
                                      ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                      /* outputs */                                                       \
                                      vAvgPixel)                                                          \
                                                                                                          \
    uint16x8_t      ptVal8;                                                                               \
    /* combination of Bottom / Top & Left / Right areas contributions */                                  \
    __typeof__ (vAvgPixel)    vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                         \
                                                                                                          \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                 \
                                                                                                          \
    /*                                                                                                    \
     * accumulate / average over the 4 neigbouring pixels                                                 \
     */                                                                                                   \
                                                                                                          \
    /* Bottom Left averaging */                                                                           \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR_INSIDE_SRC(vXi, vYi, pOrigin, ptOrigValidRegion,         \
                                    iOrigStride, ptVal8);                                                 \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC(vAvgPixel, ptVal8, vAreaBL);                                              \
    }                                                                                                     \
                                                                                                          \
    /* Bottom Right averaging */                                                                          \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR_INSIDE_SRC(vaddq_n_s16(vXi, 1), vYi, pOrigin,            \
                ptOrigValidRegion, iOrigStride, ptVal8);                                                  \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaBR);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Top Left averaging */                                                                              \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR_INSIDE_SRC(vXi, vaddq_n_s16(vYi, 1), pOrigin,            \
                 ptOrigValidRegion, iOrigStride, ptVal8);                                                 \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaTL);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Top Right averaging */                                                                             \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR_INSIDE_SRC(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),     \
            pOrigin, ptOrigValidRegion, iOrigStride, ptVal8);                                             \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaTR);                                          \
    }

#define __ARM2D_AVG_NEIGHBR_GRAY8_PIX_MASK_CLR(/* inputs */ \
                                      ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                      MaskColour, vTarget, predTail,                                      \
                                      /* outputs */                                                       \
                                      predGlb, vAvgPixel)                                                 \
                                                                                                          \
    uint16x8_t      ptVal8;                                                                               \
    /* combination of Bottom / Top & Left / Right areas contributions */                                  \
    __typeof__ (vAvgPixel)    vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                         \
                                                                                                          \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                 \
                                                                                                          \
    /*                                                                                                    \
     * accumulate / average over the 4 neigbouring pixels                                                 \
     */                                                                                                   \
                                                                                                          \
    /* Bottom Left averaging */                                                                           \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,           \
                                    iOrigStride, MaskColour, vTarget, predTail, ptVal8, predGlb);         \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC(vAvgPixel, ptVal8, vAreaBL);                                              \
    }                                                                                                     \
                                                                                                          \
    /* Bottom Right averaging */                                                                          \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vaddq_n_s16(vXi, 1), vYi, pOrigin,              \
                ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, ptVal8, predGlb);          \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaBR);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Top Left averaging */                                                                              \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vXi, vaddq_n_s16(vYi, 1), pOrigin,              \
                 ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, ptVal8, predGlb);         \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaTL);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Top Right averaging */                                                                             \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),       \
            pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, ptVal8, predGlb);     \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaTR);                                          \
    }

#define __ARM2D_AVG_NEIGHBR_GRAY8_PIX_MASK_CLR_INSIDE_SRC(/* inputs */                                    \
                                      ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                      MaskColour, vTarget,                                                \
                                      /* outputs */                                                       \
                                      vAvgPixel)                                                          \
                                                                                                          \
    uint16x8_t      ptVal8;                                                                               \
    /* combination of Bottom / Top & Left / Right areas contributions */                                  \
    __typeof__ (vAvgPixel)    vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                         \
                                                                                                          \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                 \
                                                                                                          \
    /*                                                                                                    \
     * accumulate / average over the 4 neigbouring pixels                                                 \
     */                                                                                                   \
                                                                                                          \
    /* Bottom Left averaging */                                                                           \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR_INSIDE_SRC(vXi, vYi, pOrigin, ptOrigValidRegion,\
                                    iOrigStride, MaskColour, vTarget, ptVal8);                            \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC(vAvgPixel, ptVal8, vAreaBL);                                              \
    }                                                                                                     \
                                                                                                          \
    /* Bottom Right averaging */                                                                          \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR_INSIDE_SRC(vaddq_n_s16(vXi, 1), vYi, pOrigin,   \
                ptOrigValidRegion, iOrigStride, MaskColour, vTarget, ptVal8);                             \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaBR);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Top Left averaging */                                                                              \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR_INSIDE_SRC(vXi, vaddq_n_s16(vYi, 1), pOrigin,   \
                 ptOrigValidRegion, iOrigStride, MaskColour, vTarget, ptVal8);                            \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaTL);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Top Right averaging */                                                                             \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR_INSIDE_SRC(                                     \
                        vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),                                         \
                        pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, ptVal8);            \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPixel, ptVal8, vAreaTR);                                          \
    }

#define __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(/* inputs */ \
                                      ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                      predTail,                                                           \
                                      /* outputs */                                                       \
                                      predGlb, vAvgPixel)                                                 \
                                                                                                          \
    uint16x8_t      ptVal8;                                                                               \
    /* combination of Bottom / Top & Left / Right areas contributions */                                  \
    __typeof__ (vAvgPixel)    vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                         \
                                                                                                          \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                 \
                                                                                                          \
    /*                                                                                                    \
     * accumulate / average over the 4 neigbouring pixels                                                 \
     */                                                                                                   \
                                                                                                          \
    /* Bottom Left averaging */                                                                           \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pOrigin, ptOrigValidRegion,                        \
                                    iOrigStride, predTail, ptVal8, predGlb);                              \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_OPA(vAvgPixel, ptVal8, vAreaBL);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Bottom Right averaging */                                                                          \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vaddq_n_s16(vXi, 1), vYi, pOrigin,                           \
                ptOrigValidRegion, iOrigStride, predTail, ptVal8, predGlb);                               \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_OPA_ACC(vAvgPixel, ptVal8, vAreaBR);                                      \
    }                                                                                                     \
                                                                                                          \
    /* Top Left averaging */                                                                              \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vaddq_n_s16(vYi, 1), pOrigin,                           \
                 ptOrigValidRegion, iOrigStride, predTail, ptVal8, predGlb);                              \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_OPA_ACC(vAvgPixel, ptVal8, vAreaTL);                                      \
    }                                                                                                     \
                                                                                                          \
    /* Top Right averaging */                                                                             \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),                    \
            pOrigin, ptOrigValidRegion, iOrigStride, predTail, ptVal8, predGlb);                          \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_OPA_ACC(vAvgPixel, ptVal8, vAreaTR);                                      \
    }

#define __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(/* inputs */                                    \
                                      ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,         \
                                      /* outputs */                                                       \
                                      vAvgPixel)                                                          \
                                                                                                          \
    uint16x8_t      ptVal8;                                                                               \
    /* combination of Bottom / Top & Left / Right areas contributions */                                  \
    __typeof__ (vAvgPixel)    vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                         \
                                                                                                          \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                 \
                                                                                                          \
    /*                                                                                                    \
     * accumulate / average over the 4 neigbouring pixels                                                 \
     */                                                                                                   \
                                                                                                          \
    /* Bottom Left averaging */                                                                           \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(vXi, vYi, pOrigin, ptOrigValidRegion,             \
                                    iOrigStride, ptVal8);                                                 \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_OPA(vAvgPixel, ptVal8, vAreaBL);                                          \
    }                                                                                                     \
                                                                                                          \
    /* Bottom Right averaging */                                                                          \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(vaddq_n_s16(vXi, 1), vYi, pOrigin,                \
                ptOrigValidRegion, iOrigStride, ptVal8);                                                  \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_OPA_ACC(vAvgPixel, ptVal8, vAreaBR);                                      \
    }                                                                                                     \
                                                                                                          \
    /* Top Left averaging */                                                                              \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(vXi, vaddq_n_s16(vYi, 1), pOrigin,                \
                 ptOrigValidRegion, iOrigStride, ptVal8);                                                 \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_OPA_ACC(vAvgPixel, ptVal8, vAreaTL);                                      \
    }                                                                                                     \
                                                                                                          \
    /* Top Right averaging */                                                                             \
    {                                                                                                     \
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),         \
            pOrigin, ptOrigValidRegion, iOrigStride, ptVal8);                                             \
                                                                                                          \
        __ARM_2D_SCALE_GRAY8VEC_OPA_ACC(vAvgPixel, ptVal8, vAreaTR);                                      \
    }


#define __ARM2D_AVG_NEIGHBR_RGB565_PIX(/* inputs */                                                        \
                             far, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,              \
                             predTail,                                                                     \
                             /* outputs */                                                                 \
                             predGlb, vAvgPixelR, vAvgPixelG, vAvgPixelB)                                  \
                                                                                                           \
    uint16x8_t      R, G, B;                                                                               \
    __typeof__ (vAvgPixelR)     vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                        \
                                                                                                           \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                  \
                                                                                                           \
                                                                                                           \
    /*                                                                                                     \
     * accumulate / average over the 4 neigbouring pixels                                                  \
     */                                                                                                    \
                                                                                                           \
    /* Bottom Left averaging */                                                                            \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far)(vXi, vYi, pOrigin, ptOrigValidRegion,     \
        iOrigStride, predTail, R, G, B, predGlb);                                                          \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBL);                       \
    }                                                                                                      \
                                                                                                           \
    /* Bottom Right averaging */                                                                           \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far)(vaddq_n_s16(vXi, 1), vYi,                 \
             pOrigin, ptOrigValidRegion, iOrigStride, predTail, R, G, B, predGlb);                         \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBR);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Left averaging */                                                                               \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far)(vXi, vaddq_n_s16(vYi, 1),                 \
                 pOrigin, ptOrigValidRegion, iOrigStride, predTail, R, G, B, predGlb);                     \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTL);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Right averaging */                                                                              \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far)(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1), \
                 pOrigin, ptOrigValidRegion, iOrigStride, predTail, R, G, B, predGlb);                     \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTR);                   \
    }


#define __ARM2D_AVG_NEIGHBR_RGB565_PIX_INSIDE_SRC(/* inputs */                                             \
                             far, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,              \
                             /* outputs */                                                                 \
                             vAvgPixelR, vAvgPixelG, vAvgPixelB)                                           \
                                                                                                           \
    uint16x8_t      R, G, B;                                                                               \
    __typeof__ (vAvgPixelR)     vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                        \
                                                                                                           \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                  \
                                                                                                           \
                                                                                                           \
    /*                                                                                                     \
     * accumulate / average over the 4 neigbouring pixels                                                  \
     */                                                                                                    \
                                                                                                           \
    /* Bottom Left averaging */                                                                            \
    {                                                                                                      \
        ARM_CONNECT3(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far, _INSIDE_SRC)(                             \
            vXi, vYi, pOrigin, ptOrigValidRegion,                                                          \
            iOrigStride, R, G, B);                                                                         \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBL);                       \
    }                                                                                                      \
                                                                                                           \
    /* Bottom Right averaging */                                                                           \
    {                                                                                                      \
        ARM_CONNECT3(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far, _INSIDE_SRC)(vaddq_n_s16(vXi, 1), vYi,    \
             pOrigin, ptOrigValidRegion, iOrigStride, R, G, B);                                            \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBR);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Left averaging */                                                                               \
    {                                                                                                      \
        ARM_CONNECT3(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far, _INSIDE_SRC)(vXi, vaddq_n_s16(vYi, 1),    \
                 pOrigin, ptOrigValidRegion, iOrigStride, R, G, B);                                        \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTL);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Right averaging */                                                                              \
    {                                                                                                      \
        ARM_CONNECT3(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT, far, _INSIDE_SRC)(                             \
                    vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),                                              \
                    pOrigin, ptOrigValidRegion, iOrigStride, R, G, B);                                     \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTR);                   \
    }

#define __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_CLR(/* inputs */                                               \
                             far, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,              \
                             MaskColour, vTarget, predTail,                                                \
                             /* outputs */                                                                 \
                             predGlb, vAvgPixelR, vAvgPixelG, vAvgPixelB)                                  \
                                                                                                           \
    uint16x8_t      R, G, B;                                                                               \
    __typeof__ (vAvgPixelR)     vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                        \
                                                                                                           \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                  \
                                                                                                           \
                                                                                                           \
    /*                                                                                                     \
     * accumulate / average over the 4 neigbouring pixels                                                  \
     */                                                                                                    \
                                                                                                           \
    /* Bottom Left averaging */                                                                            \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR, far)(vXi, vYi, pOrigin,               \
            ptOrigValidRegion,iOrigStride, MaskColour,vTarget, predTail, R, G, B, predGlb);                \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBL);                       \
    }                                                                                                      \
                                                                                                           \
    /* Bottom Right averaging */                                                                           \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR, far)(vaddq_n_s16(vXi, 1), vYi,        \
             pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, R, G, B, predGlb);    \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBR);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Left averaging */                                                                               \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR, far)(vXi, vaddq_n_s16(vYi, 1),        \
                 pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, R, G, B, predGlb);\
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTL);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Right averaging */                                                                              \
    {                                                                                                      \
        ARM_CONNECT2(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR, far)(                                 \
                                                       vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),           \
                 pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, predTail, R, G, B, predGlb);\
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTR);                   \
    }

#define __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_CLR_INSIDE_SRC(/* inputs */                                    \
                             far, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,              \
                             MaskColour, vTarget,                                                          \
                             /* outputs */                                                                 \
                             vAvgPixelR, vAvgPixelG, vAvgPixelB)                                           \
                                                                                                           \
    uint16x8_t      R, G, B;                                                                               \
    __typeof__ (vAvgPixelR)     vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                        \
                                                                                                           \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                  \
                                                                                                           \
                                                                                                           \
    /*                                                                                                     \
     * accumulate / average over the 4 neigbouring pixels                                                  \
     */                                                                                                    \
                                                                                                           \
    /* Bottom Left averaging */                                                                            \
    {                                                                                                      \
        ARM_CONNECT3(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR, far, _INSIDE_SRC)(vXi, vYi, pOrigin,  \
            ptOrigValidRegion,iOrigStride, MaskColour,vTarget, R, G, B);                                   \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBL);                       \
    }                                                                                                      \
                                                                                                           \
    /* Bottom Right averaging */                                                                           \
    {                                                                                                      \
        ARM_CONNECT3(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR, far, _INSIDE_SRC)(                    \
             vaddq_n_s16(vXi, 1), vYi,                                                                     \
             pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, R, G, B);                       \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBR);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Left averaging */                                                                               \
    {                                                                                                      \
        ARM_CONNECT3(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR, far, _INSIDE_SRC)(                    \
                 vXi, vaddq_n_s16(vYi, 1),                                                                 \
                 pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, R, G, B);                   \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTL);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Right averaging */                                                                              \
    {                                                                                                      \
        ARM_CONNECT3(__ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR, far, _INSIDE_SRC)(                    \
                                                       vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1),           \
                 pOrigin, ptOrigValidRegion, iOrigStride, MaskColour, vTarget, R, G, B);                   \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTR);                   \
    }


#define __ARM2D_AVG_NEIGHBR_CCCN888_PIX(/* inputs */                                                       \
                                        ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,        \
                                        predTailLow, predTailHigh,                                         \
                                        /* outputs */                                                      \
                                        predGlbLo, predGlbHi,                                              \
                                        vAvgPixelR, vAvgPixelG, vAvgPixelB)                                \
                                                                                                           \
    uint16x8_t      R, G, B;                                                                               \
    __typeof__ (vAvgPixelR)     vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                        \
                                                                                                           \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                  \
                                                                                                           \
    /*                                                                                                     \
     * accumulate / average over the 4 neigbouring pixels                                                  \
     */                                                                                                    \
                                                                                                           \
    /* Bottom Left averaging */                                                                            \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,           \
            predTailLow, predTailHigh, R, G, B, predGlbLo, predGlbHi);                                     \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBL);                       \
    }                                                                                                      \
                                                                                                           \
    /* Bottom Right averaging */                                                                           \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(vaddq_n_s16(vXi, 1), vYi, pOrigin, ptOrigValidRegion,        \
                                                 iOrigStride, predTailLow, predTailHigh,                   \
                                                                        R, G, B, predGlbLo, predGlbHi);    \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBR);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Left averaging */                                                                               \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(vXi, vaddq_n_s16(vYi, 1), pOrigin, ptOrigValidRegion,        \
                                                                 iOrigStride, predTailLow, predTailHigh,   \
                                                                        R, G, B, predGlbLo, predGlbHi);    \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTL);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Right averaging */                                                                              \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1), pOrigin,           \
                                         ptOrigValidRegion, iOrigStride,                                   \
                                       predTailLow, predTailHigh, R, G, B, predGlbLo, predGlbHi);          \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTR);                   \
    }

#define __ARM2D_AVG_NEIGHBR_CCCN888_PIX_INSIDE_SRC(/* inputs */                                            \
                                        ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,        \
                                        /* outputs */                                                      \
                                        vAvgPixelR, vAvgPixelG, vAvgPixelB)                                \
                                                                                                           \
    uint16x8_t      R, G, B;                                                                               \
    __typeof__ (vAvgPixelR)     vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                        \
                                                                                                           \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                  \
                                                                                                           \
    /*                                                                                                     \
     * accumulate / average over the 4 neigbouring pixels                                                  \
     */                                                                                                    \
                                                                                                           \
    /* Bottom Left averaging */                                                                            \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_INSIDE_SRC(vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,\
            R, G, B);                                                                                      \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBL);                       \
    }                                                                                                      \
                                                                                                           \
    /* Bottom Right averaging */                                                                           \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_INSIDE_SRC(                                                  \
                                                vaddq_n_s16(vXi, 1), vYi, pOrigin, ptOrigValidRegion,      \
                                                iOrigStride, R, G, B);                                     \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBR);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Left averaging */                                                                               \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_INSIDE_SRC(                                                  \
                                                vXi, vaddq_n_s16(vYi, 1), pOrigin, ptOrigValidRegion,      \
                                                iOrigStride, R, G, B);                                     \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTL);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Right averaging */                                                                              \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_INSIDE_SRC(vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1), pOrigin,\
                                    ptOrigValidRegion, iOrigStride, R, G, B);                              \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTR);                   \
    }

#define __ARM2D_AVG_NEIGHBR_CCCN888_PIX_MASK_CLR(/* inputs */                                              \
                                        ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,        \
                                        MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh,       \
                                        /* outputs */                                                      \
                                        predGlbLo, predGlbHi,                                              \
                                        vAvgPixelR, vAvgPixelG, vAvgPixelB)                                \
                                                                                                           \
    uint16x8_t      R, G, B;                                                                               \
    __typeof__ (vAvgPixelR)     vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                        \
                                                                                                           \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                  \
                                                                                                           \
    /*                                                                                                     \
     * accumulate / average over the 4 neigbouring pixels                                                  \
     */                                                                                                    \
                                                                                                           \
    /* Bottom Left averaging */                                                                            \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR(vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,  \
            MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh, R, G, B, predGlbLo, predGlbHi);   \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBL);                       \
    }                                                                                                      \
                                                                                                           \
    /* Bottom Right averaging */                                                                           \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR(                                                    \
                                                  vaddq_n_s16(vXi, 1), vYi, pOrigin, ptOrigValidRegion,    \
                               iOrigStride, MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh,   \
                                                                        R, G, B, predGlbLo, predGlbHi);    \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBR);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Left averaging */                                                                               \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR(                                                    \
                                              vXi, vaddq_n_s16(vYi, 1), pOrigin, ptOrigValidRegion,        \
                               iOrigStride, MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh,   \
                                                                        R, G, B, predGlbLo, predGlbHi);    \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTL);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Right averaging */                                                                              \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR(                                                    \
                                              vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1), pOrigin,           \
                                         ptOrigValidRegion, iOrigStride, MaskColour, vTargetLo, vTargetHi, \
                                       predTailLow, predTailHigh, R, G, B, predGlbLo, predGlbHi);          \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTR);                   \
    }

#define __ARM2D_AVG_NEIGHBR_CCCN888_PIX_MASK_CLR_INSIDE_SRC(/* inputs */                                   \
                                        ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,        \
                                        MaskColour, vTargetLo, vTargetHi,                                  \
                                        /* outputs */                                                      \
                                        vAvgPixelR, vAvgPixelG, vAvgPixelB)                                \
                                                                                                           \
    uint16x8_t      R, G, B;                                                                               \
    __typeof__ (vAvgPixelR)     vAreaTR, vAreaTL, vAreaBR, vAreaBL;                                        \
                                                                                                           \
    __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint, vAreaTR, vAreaTL, vAreaBR, vAreaBL);                  \
                                                                                                           \
    /*                                                                                                     \
     * accumulate / average over the 4 neigbouring pixels                                                  \
     */                                                                                                    \
                                                                                                           \
    /* Bottom Left averaging */                                                                            \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR_INSIDE_SRC(                                         \
                                vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,                         \
                                MaskColour, vTargetLo, vTargetHi, R, G, B);                                \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBL);                       \
    }                                                                                                      \
                                                                                                           \
    /* Bottom Right averaging */                                                                           \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR_INSIDE_SRC(                                         \
                        vaddq_n_s16(vXi, 1), vYi, pOrigin, ptOrigValidRegion,                              \
                        iOrigStride, MaskColour, vTargetLo, vTargetHi, R, G, B);                           \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaBR);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Left averaging */                                                                               \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR_INSIDE_SRC(                                         \
                        vXi, vaddq_n_s16(vYi, 1), pOrigin, ptOrigValidRegion,                              \
                        iOrigStride, MaskColour, vTargetLo, vTargetHi, R, G, B);                           \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTL);                   \
    }                                                                                                      \
                                                                                                           \
    /* Top Right averaging */                                                                              \
    {                                                                                                      \
        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR_INSIDE_SRC(                                         \
                        vaddq_n_s16(vXi, 1), vaddq_n_s16(vYi, 1), pOrigin,                                 \
                        ptOrigValidRegion, iOrigStride, MaskColour, vTargetLo, vTargetHi,                  \
                        R, G, B);                                                                          \
                                                                                                           \
        __ARM_2D_SCALE_RGBVEC_ACC(vAvgPixelR, vAvgPixelG, vAvgPixelB, R, G, B, vAreaTR);                   \
    }

#endif

#define __ARM_2D_8BIT_RGB_MIX(/* inputs / outputs */                                                       \
                            vR_InOut, vG_InOut, vB_InOut,                                                  \
                            /* inputs */                                                                   \
                            C1,                                                                            \
                            vR2, vG2, vB2, C2)                                                             \
    vR_InOut = vqaddq(vR_InOut * C1, vR2 * C2);                                                            \
    vR_InOut = vR_InOut >> 8;                                                                              \
                                                                                                           \
    vG_InOut = vqaddq(vG_InOut * C1, vG2 * C2);                                                            \
    vG_InOut = vG_InOut >> 8;                                                                              \
                                                                                                           \
    vB_InOut = vqaddq(vB_InOut * C1, vB2 * C2);                                                            \
    vB_InOut = vB_InOut >> 8;


/**
  Alpha blending of a packed vector with 3 vectors of single R, G & B channels
 */


#define __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(/* inputs */                                                   \
                                            hwOpacity, vPackedTarget, vAvgR, vAvgG, vAvgB,                 \
                                            /* output */                                                   \
                                            vBlended)                                                      \
    uint16x8_t      vTargetR, vTargetG, vTargetB;                                                          \
                                                                                                           \
    __arm_2d_rgb565_unpack_single_vec(vTarget, &vTargetR, &vTargetG, &vTargetB);                           \
                                                                                                           \
    uint16_t        transp = 256 - hwOpacity;                                                              \
    /* merge */                                                                                            \
    __ARM_2D_8BIT_RGB_MIX(vAvgR, vAvgG, vAvgB, hwOpacity,                                                  \
                                    vTargetR, vTargetG, vTargetB, transp);                                 \
                                                                                                           \
    vBlended = __arm_2d_rgb565_pack_single_vec(vAvgR, vAvgG, vAvgB);



/* extra calibration removed in fixed-point code since offset is lower than Q9.6 representation */

#define ONE_BY_2PI_Q31      341782637.0f
#define ONE_Q16             UINT16_MAX
#define ARSHIFT(x, shift)   (shift > 0 ? x >> shift : x << (-shift))
#define reinterpret_q16_s16(x)           ((x) << 16)
#define GET_Q6INT(x)        ((x) >> 6)
#define SET_Q6INT(x)        ((x) << 6)

/**
  Scale Gray8 channel
 */
#define __ARM_2D_SCALE_GRAY8VEC(/* output */                                            \
                                vAvgPix,                                                \
                                /* inputs */                                            \
                                vPtVal, vAreaScal)                                      \
        vAvgPix = vrmulhq_u16(vAreaScal, vPtVal);

/**
  Scale Gray8 channel with accumulation
 */
#define __ARM_2D_SCALE_GRAY8VEC_ACC(/* input / output */                                \
                                vAvgPix,                                                \
                                /* inputs */                                            \
                                vPtVal, vAreaScal)                                      \
        vAvgPix = vqaddq(vAvgPix, vrmulhq_u16(vAreaScal, vPtVal));



#define __ARM_2D_SCALE_GRAY8VEC_OPA(/* output */                                        \
                                vAvgPix,                                                \
                                /* inputs */                                            \
                                vPtVal, vAreaScal)                                      \
    __ARM_2D_SCALE_GRAY8VEC(vAvgPix, vPtVal, vAreaScal);

#define __ARM_2D_SCALE_GRAY8VEC_OPA_ACC(/* output */                                    \
                                vAvgPix,                                                \
                                /* inputs */                                            \
                                vPtVal, vAreaScal)                                      \
    __ARM_2D_SCALE_GRAY8VEC_ACC(vAvgPix, vPtVal, vAreaScal);

/**
  Scale R, G & B channels
 */
#define __ARM_2D_SCALE_RGBVEC(/* outputs */                                             \
                                vAvgPixelR, vAvgPixelG, vAvgPixelB,                     \
                              /* inputs */                                              \
                                R, G, B, vScal)                                         \
        vAvgPixelR = vrmulhq_u16(vScal, R);                                             \
        vAvgPixelG = vrmulhq_u16(vScal, G);                                             \
        vAvgPixelB = vrmulhq_u16(vScal, B);

/**
  Scale R, G & B channels with accumulation
 */
#define __ARM_2D_SCALE_RGBVEC_ACC(/* inputs / outputs */                                \
                                   vAvgPixelR, vAvgPixelG, vAvgPixelB,                  \
                                  /* inputs */                                          \
                                   R, G, B, vScal)                                      \
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vScal, R));                         \
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vScal, G));                         \
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vScal, B));



#define __ARM_2D_GET_POINT_COORD(point)     GET_Q6INT(point)
#define __ARM_2D_CONVERT_TO_PIX_TYP(x)      (x)

#define PIX_VEC_TYP                         uint16x8_t
#define ARM_2D_POINT_VEC                    arm_2d_point_s16x8_t
#define AREA_UNITY                          vdupq_n_u16(ONE_Q16)


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__


#define __ARM2D_GET_NEIGHBR_PIX_AREAS(vXi, vYi, ptPoint,                                \
                                           vAreaTR, vAreaTL, vAreaBR, vAreaBL)          \
    int16x8_t       vOne = vdupq_n_s16(SET_Q6INT(1));                                   \
                                                                                        \
    /*vXi = vsubq_m_n_s16(vXi, vXi, 1, vcmpltq_n_s16(ptPoint->X, 0));  */               \
    /*vYi = vsubq_m_n_s16(vYi, vYi, 1, vcmpltq_n_s16(ptPoint->Y, 0));  */               \
                                                                                        \
    int16x8_t       vWX = ptPoint->X - SET_Q6INT(vXi);                                  \
    int16x8_t       vWY = ptPoint->Y - SET_Q6INT(vYi);                                  \
                                                                                        \
    /* combination of Bottom / Top & Left / Right areas contributions */                \
    vAreaTR = vmulq_u16(vWX, vWY);                                                      \
    vAreaTL = vmulq_u16((vOne - vWX), vWY);                                             \
    vAreaBR = vmulq_u16(vWX, (vOne - vWY));                                             \
    vAreaBL = vmulq_u16((vOne - vWX), (vOne - vWY));                                    \
                                                                                        \
    /* Q16 conversion */                                                                \
    vAreaTR = vqshlq_n_u16(vAreaTR, 4);                                                 \
    vAreaTL = vqshlq_n_u16(vAreaTL, 4);                                                 \
    vAreaBR = vqshlq_n_u16(vAreaBR, 4);                                                 \
    vAreaBL = vqshlq_n_u16(vAreaBL, 4);


#endif


bool __arm_2d_transform_regression(arm_2d_size_t * __RESTRICT ptCopySize,
                                            arm_2d_location_t * pSrcPoint,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY,
                                            arm_2d_location_t * tOffset,
                                            arm_2d_point_float_t * center,
                                            int32_t             iOrigStride,
                                            arm_2d_rot_linear_regr_t regrCoefs[]
    )
{
    bool                gatherLoadIdxOverflow = false;
#if 0
    int_fast16_t        iHeight = ptCopySize->iHeight;
    int_fast16_t        iWidth = ptCopySize->iWidth;
    q31_t               invHeightFx = iHeight > 1 ? INT32_MAX / (iHeight - 1) : INT32_MAX;
    int32_t             AngleFx = lroundf(fAngle * ONE_BY_2PI_Q31);
    int32_t             ScaleFx = (int32_t)((float)fScaleX * (float)reinterpret_q16_s16(1));
    q31_t               cosAngleFx = MULTFX(arm_cos_q31(AngleFx), ScaleFx);
    q31_t               sinAngleFx = MULTFX(arm_sin_q31(AngleFx), ScaleFx);
    arm_2d_point_fx_t   tPointCornerFx[2][2];
    arm_2d_point_fx_t   centerQ16;
    arm_2d_point_fx_t   srcPointQ16;
    arm_2d_point_fx_t   tOffsetQ16;
    arm_2d_point_fx_t   tmp;
    int32_t             iXQ16, iYQ16;

    /* Q16 conversion */
    centerQ16.X = reinterpret_q16_f32(center->fX);
    centerQ16.Y = reinterpret_q16_f32(center->fY);

    srcPointQ16.X = reinterpret_q16_s16(pSrcPoint->iX);
    srcPointQ16.Y = reinterpret_q16_s16(pSrcPoint->iY);

    tOffsetQ16.X = reinterpret_q16_s16(tOffset->iX);
    tOffsetQ16.Y = reinterpret_q16_s16(tOffset->iY);


    /* (0,0) corner */
    tmp.X = srcPointQ16.X + 0 + tOffsetQ16.X;
    tmp.Y = srcPointQ16.Y + 0 + tOffsetQ16.Y;

    iXQ16 = tmp.X - centerQ16.X;
    iYQ16 = tmp.Y - centerQ16.Y;

    tPointCornerFx[0][0].Y =
        __QDADD(__QDADD(centerQ16.Y, mul_q16(iYQ16, cosAngleFx)),
                mul_q16(iXQ16, sinAngleFx));
    tPointCornerFx[0][0].X =
        __QDSUB(__QDADD(centerQ16.X, mul_q16(iXQ16, cosAngleFx)),
                mul_q16(iYQ16, sinAngleFx));


    /* ((iWidth - 1),0) corner */
    tmp.X = srcPointQ16.X + 0 + tOffsetQ16.X + reinterpret_q16_s16(iWidth - 1);
    iXQ16 = tmp.X - centerQ16.X;

    tPointCornerFx[1][0].Y =
        __QDADD(__QDADD(centerQ16.Y, mul_q16(iYQ16, cosAngleFx)),
                mul_q16(iXQ16, sinAngleFx));
    tPointCornerFx[1][0].X =
        __QDSUB(__QDADD(centerQ16.X, mul_q16(iXQ16, cosAngleFx)),
                mul_q16(iYQ16, sinAngleFx));


    /* ((iWidth - 1),(iHeight - 1)) corner */
    tmp.Y = srcPointQ16.Y + tOffsetQ16.Y + reinterpret_q16_s16(iHeight - 1);
    iYQ16 = tmp.Y - centerQ16.Y;

    tPointCornerFx[1][1].Y =
        __QDADD(__QDADD(centerQ16.Y, mul_q16(iYQ16, cosAngleFx)),
                mul_q16(iXQ16, sinAngleFx));
    tPointCornerFx[1][1].X =
        __QDSUB(__QDADD(centerQ16.X, mul_q16(iXQ16, cosAngleFx)),
                mul_q16(iYQ16, sinAngleFx));


    /* (0,(iHeight - 1)) corner */
    tmp.X = srcPointQ16.X + 0 + tOffsetQ16.X;
    iXQ16 = tmp.X - centerQ16.X;

    tPointCornerFx[0][1].Y =
        __QDADD(__QDADD(centerQ16.Y, mul_q16(iYQ16, cosAngleFx)),
                mul_q16(iXQ16, sinAngleFx));
    tPointCornerFx[0][1].X =
        __QDSUB(__QDADD(centerQ16.X, mul_q16(iXQ16, cosAngleFx)),
                mul_q16(iYQ16, sinAngleFx));
#else
    int_fast16_t        iHeight = ptCopySize->iHeight;
    int_fast16_t        iWidth = ptCopySize->iWidth;
    q31_t               invHeightFx;

    if (1 == iHeight) {
        invHeightFx = 0x7fffffff;
    } else {
        invHeightFx = 0x7fffffff / (iHeight - 1);
    }

    int32_t             AngleFx = ARM_2D_LROUNDF(fAngle * ONE_BY_2PI_Q31);
    int32_t             ScaleXFx = reinterpret_q16_f32(fScaleX);
    int32_t             ScaleYFx = reinterpret_q16_f32(fScaleY);

    q16_t               cosAngleFx = reinterpret_q16_q31(arm_cos_q31(AngleFx));
    q16_t               sinAngleFx = reinterpret_q16_q31(arm_sin_q31(AngleFx));

    arm_2d_point_fx_t   tPointCornerFx[2][2];
    arm_2d_point_fx_t   centerQ16;
    arm_2d_point_fx_t   srcPointQ16;
    arm_2d_point_fx_t   tOffsetQ16;
    arm_2d_point_fx_t   tmp;
    arm_2d_point_q16_t  tPoint;  


    /* Q16 conversion */
    centerQ16.q16X = reinterpret_q16_f32(center->fX);
    centerQ16.q16Y = reinterpret_q16_f32(center->fY);

    srcPointQ16.q16X = reinterpret_q16_s16(pSrcPoint->iX);
    srcPointQ16.q16Y = reinterpret_q16_s16(pSrcPoint->iY);

    tOffsetQ16.q16X = reinterpret_q16_s16(tOffset->iX);
    tOffsetQ16.q16Y = reinterpret_q16_s16(tOffset->iY);


    /* (0,0) corner */
    tmp.q16X = srcPointQ16.q16X + 0 + tOffsetQ16.q16X;
    tmp.q16Y = srcPointQ16.q16Y + 0 + tOffsetQ16.q16Y;

    tPoint.q16X = tmp.q16X - centerQ16.q16X;
    tPoint.q16Y = tmp.q16Y - centerQ16.q16Y;

#define __PT_TRANSFORM(__PT) \
    do {                                                                            \
        /* rotation first, then scaling */                                          \
        __PT.q16Y =qadd_q16(   centerQ16.q16Y,                                      \
                            mul_q16(qadd_q16(   mul_q16(tPoint.q16Y, cosAngleFx),   \
                                                mul_q16(tPoint.q16X, sinAngleFx)),  \
                                    ScaleYFx));                                     \
        __PT.q16X =qadd_q16(   centerQ16.q16X,                                      \
                            mul_q16(qsub_q16(   mul_q16(tPoint.q16X, cosAngleFx),   \
                                                mul_q16(tPoint.q16Y, sinAngleFx)),  \
                                    ScaleXFx));                                     \
    } while(0)

    __PT_TRANSFORM(tPointCornerFx[0][0]);

    /* ((iWidth - 1),0) corner */
    tmp.q16X = srcPointQ16.q16X + 0 + tOffsetQ16.q16X + reinterpret_q16_s16(iWidth - 1);
    tPoint.q16X = tmp.q16X - centerQ16.q16X;


    __PT_TRANSFORM(tPointCornerFx[1][0]);

    /* ((iWidth - 1),(iHeight - 1)) corner */
    tmp.q16Y = srcPointQ16.q16Y + tOffsetQ16.q16Y + reinterpret_q16_s16(iHeight - 1);
    tPoint.q16Y = tmp.q16Y - centerQ16.q16Y;

    __PT_TRANSFORM(tPointCornerFx[1][1]);

    /* (0,(iHeight - 1)) corner */
    tmp.q16X = srcPointQ16.q16X + 0 + tOffsetQ16.q16X;
    tPoint.q16X = tmp.q16X - centerQ16.q16X;

    __PT_TRANSFORM(tPointCornerFx[0][1]);
#endif

    /*
       Check whether rotated index offsets could exceed 16-bit limits
       used in subsequent gather loads
       This will occur for parts of large images (e.g. 320*200)
       To avoid unconditional penalties for small/medium images,
       returns a speculative overflow allowing to handle large offsets.
    */
    int32_t maxY = MAX(MAX
                        (MAX(tPointCornerFx[0][0].q16Y, tPointCornerFx[0][1].q16Y),
                            tPointCornerFx[1][0].q16Y),
                                tPointCornerFx[1][1].q16Y);

    if(MULTFX(reinterpret_q16_s16(iOrigStride), maxY) > UINT16_MAX)
        gatherLoadIdxOverflow = true;


    /* regression */
    int32_t           slopeXFx, slopeYFx;

    /* interpolation in Y direction for 1st elements column */
    slopeXFx = MULTFX((tPointCornerFx[0][1].q16X - tPointCornerFx[0][0].q16X), invHeightFx);
    slopeYFx = MULTFX((tPointCornerFx[0][1].q16Y - tPointCornerFx[0][0].q16Y), invHeightFx);

    regrCoefs[0].slopeY = slopeYFx * 2;
    regrCoefs[0].slopeX = slopeXFx * 2;
    regrCoefs[0].interceptY = tPointCornerFx[0][0].q16Y;
    regrCoefs[0].interceptX = tPointCornerFx[0][0].q16X;


    /* interpolation in Y direction for the last elements column */
    slopeXFx = MULTFX((tPointCornerFx[1][1].q16X - tPointCornerFx[1][0].q16X), invHeightFx);
    slopeYFx = MULTFX((tPointCornerFx[1][1].q16Y - tPointCornerFx[1][0].q16Y), invHeightFx);

    regrCoefs[1].slopeY = slopeYFx* 2;
    regrCoefs[1].slopeX = slopeXFx* 2;
    regrCoefs[1].interceptY = tPointCornerFx[1][0].q16Y;
    regrCoefs[1].interceptX = tPointCornerFx[1][0].q16X;

    return gatherLoadIdxOverflow;
}


static
void __MVE_WRAPPER( __arm_2d_impl_gray8_get_pixel_colour)(ARM_2D_POINT_VEC * ptPoint,
                                          arm_2d_region_t * ptOrigValidRegion,
                                          uint8_t * pOrigin,
                                          int16_t iOrigStride,
                                          uint8_t * pTarget, uint8_t MaskColour, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_u16(pTarget);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;
    /* accumulated pixel vectors */
    uint16x8_t      vDstPixel;


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_MASK_CLR(ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                      MaskColour, vTarget, predTail, predGlb, vAvgPixel);

        vDstPixel = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
        /* saturate to 8-bit */
        vDstPixel = vminq(vDstPixel, vdupq_n_u16(255));
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,
                                                          iOrigStride, MaskColour, vTarget, predTail,
                                                          vDstPixel, predGlb);
    }
#endif

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    vstrbq_p_u16(pTarget, vTarget, predTail);
}

static
void __MVE_WRAPPER( __arm_2d_impl_gray8_get_pixel_colour_inside_src)(
                        ARM_2D_POINT_VEC * ptPoint,
                        arm_2d_region_t * ptOrigValidRegion,
                        uint8_t * pOrigin,
                        int16_t iOrigStride,
                        uint8_t * pTarget, uint8_t MaskColour)
{
    uint16x8_t      vTarget = vldrbq_u16(pTarget);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;
    /* accumulated pixel vectors */
    uint16x8_t      vDstPixel;


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_MASK_CLR_INSIDE_SRC(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    MaskColour, vTarget, vAvgPixel);

        vDstPixel = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
        /* saturate to 8-bit */
        vDstPixel = vminq(vDstPixel, vdupq_n_u16(255));
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR_INSIDE_SRC(
                                    vXi, vYi, pOrigin, ptOrigValidRegion,
                                    iOrigStride, MaskColour, vTarget,
                                    vDstPixel);
    }
#endif

    vstrbq_u16(pTarget, vDstPixel);
}


static
void __MVE_WRAPPER( __arm_2d_impl_gray8_transform_only_get_pixel_colour)(
                                          ARM_2D_POINT_VEC * ptPoint,
                                          arm_2d_region_t * ptOrigValidRegion,
                                          uint8_t * pOrigin,
                                          int16_t iOrigStride,
                                          uint8_t * pTarget, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_u16(pTarget);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;
    /* accumulated pixel vectors */
    uint16x8_t      vDstPixel;


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;


        __ARM2D_AVG_NEIGHBR_GRAY8_PIX(ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                       predTail, predGlb, vAvgPixel);

        vDstPixel = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

        /* saturate to 8-bit */
        vDstPixel = vminq(vDstPixel, vdupq_n_u16(255));
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pOrigin, ptOrigValidRegion,
                                                          iOrigStride, predTail,
                                                          vDstPixel, predGlb);
    }
#endif

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    vstrbq_p_u16(pTarget, vTarget, predTail);
}

static
void __MVE_WRAPPER(
                __arm_2d_impl_gray8_transform_only_get_pixel_colour_inside_src)(
                                          ARM_2D_POINT_VEC * ptPoint,
                                          arm_2d_region_t * ptOrigValidRegion,
                                          uint8_t * pOrigin,
                                          int16_t iOrigStride,
                                          uint8_t * pTarget)
{
    uint16x8_t      vTarget = vldrbq_u16(pTarget);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;
    /* accumulated pixel vectors */
    uint16x8_t      vDstPixel;


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;


        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_INSIDE_SRC(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    vAvgPixel);

        vDstPixel = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

        /* saturate to 8-bit */
        vDstPixel = vminq(vDstPixel, vdupq_n_u16(255));
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pOrigin, ptOrigValidRegion,
                                        iOrigStride, vDstPixel);
    }
#endif

    /* select between target pixel, averaged pixed */
    //vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    vstrbq_u16(pTarget, vDstPixel);
}

static
void __MVE_WRAPPER( __arm_2d_impl_gray8_get_pixel_colour_with_alpha)(ARM_2D_POINT_VEC * ptPoint,
                                                     arm_2d_region_t * ptOrigValidRegion,
                                                     uint8_t * pOrigin,
                                                     int16_t iOrigStride,
                                                     uint8_t * pTarget,
                                                     uint8_t MaskColour,
                                                     uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_u16(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_MASK_CLR(ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                      MaskColour, vTarget, predTail, predGlb, vAvgPixel);

        vDstPixel = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,
                                                          iOrigStride, MaskColour, vTarget, predTail,
                                                          vDstPixel, predGlb);
    }
#endif

    /* blending */
    uint16_t        hwTransparency = 256 - hwOpacity;
    uint16x8_t      vBlended =
        vqaddq(vDstPixel * (uint16_t) hwOpacity, vTarget * hwTransparency) >> 8;

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pTarget, vTarget, predTail);
}

static
void __MVE_WRAPPER( __arm_2d_impl_gray8_get_pixel_colour_with_alpha_inside_src)(
                                         ARM_2D_POINT_VEC * ptPoint,
                                         arm_2d_region_t * ptOrigValidRegion,
                                         uint8_t * pOrigin,
                                         int16_t iOrigStride,
                                         uint8_t * pTarget,
                                         uint8_t MaskColour,
                                         uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vldrbq_u16(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_MASK_CLR_INSIDE_SRC(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    MaskColour, vTarget, vAvgPixel);

        vDstPixel = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_MASK_CLR_FAR_INSIDE_SRC(
                        vXi, vYi, pOrigin, ptOrigValidRegion,
                        iOrigStride, MaskColour, vTarget, vDstPixel);
    }
#endif

    /* blending */
    uint16_t        hwTransparency = 256 - hwOpacity;
    uint16x8_t      vBlended =
        vqaddq(vDstPixel * (uint16_t) hwOpacity, vTarget * hwTransparency) >> 8;

    vstrbq_u16(pTarget, vBlended);
}


static
void __MVE_WRAPPER( 
                __arm_2d_impl_gray8_transform_only_get_pixel_colour_with_alpha)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint8_t * pTarget,
                                        uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_u16(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX(ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                                       predTail, predGlb, vAvgPixel);

        vDstPixel = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR(vXi, vYi, pOrigin, ptOrigValidRegion,
                                                          iOrigStride, predTail,
                                                          vDstPixel, predGlb);
    }
#endif

    /* blending */
    uint16_t        hwTransparency = 256 - hwOpacity;
    uint16x8_t      vBlended =
        vqaddq(vDstPixel * (uint16_t) hwOpacity, vTarget * hwTransparency) >> 8;

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pTarget, vTarget, predTail);
}


static
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_transform_only_get_pixel_colour_with_alpha_inside_src)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint8_t * pTarget,
                                        uint_fast16_t hwOpacity)
{

    uint16x8_t      vTarget = vldrbq_u16(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_INSIDE_SRC(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    vAvgPixel);

        vDstPixel = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_FAR_INSIDE_SRC(
                                        vXi, vYi, pOrigin, ptOrigValidRegion,
                                        iOrigStride, vDstPixel);
    }
#endif

    /* blending */
    uint16_t        hwTransparency = 256 - hwOpacity;
    uint16x8_t      vBlended =
        vqaddq(vDstPixel * (uint16_t) hwOpacity, vTarget * hwTransparency) >> 8;

    vstrbq_u16(pTarget, vBlended);
}


static
void __MVE_WRAPPER( __arm_2d_impl_gray8_get_alpha_with_opacity )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint8_t * pTarget,
                                        uint8_t MaskColour,
                                        uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_u16(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

//        vHwPixelAlpha = vpselq(vdupq_n_u16(hwOpacity),
//                               vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
//                               vcmphiq_n_u16(vHwPixelAlpha, 255));
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

//        vHwPixelAlpha = vpselq(vdupq_n_u16(hwOpacity),
//                               vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
//                               vcmpeqq_n_u16(vHwPixelAlpha, 255));
    }
#endif

    vHwPixelAlpha = vpselq(vdupq_n_u16(hwOpacity),
                           vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
                           vcmphiq_n_u16(vHwPixelAlpha, 255));

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t      vBlended =
        vrshrq_n_u16(vqaddq(vHwPixelAlpha * vdupq_n_u16(MaskColour), vTarget * vhwTransparency),8);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pTarget, vTarget, predTail);
}


static
void __MVE_WRAPPER( __arm_2d_impl_gray8_get_alpha_with_opacity_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                     arm_2d_region_t * ptOrigValidRegion,
                                                     uint8_t * pOrigin,
                                                     int16_t iOrigStride,
                                                     uint8_t * pTarget,
                                                     uint8_t MaskColour,
                                                     uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vldrbq_u16(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif

    vHwPixelAlpha = vpselq(vdupq_n_u16(hwOpacity),
                           vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
                           vcmphiq_n_u16(vHwPixelAlpha, 255));

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t      vBlended =
        vrshrq_n_u16(   vqaddq(vHwPixelAlpha * vdupq_n_u16(MaskColour), 
                        vTarget * vhwTransparency),
                        8);

    vstrbq_u16(pTarget, vBlended);
}



static
void __MVE_WRAPPER( __arm_2d_impl_rgb565_get_alpha_with_opacity )(ARM_2D_POINT_VEC * ptPoint,
                                                     arm_2d_region_t * ptOrigValidRegion,
                                                     uint8_t * pOrigin,
                                                     int16_t iOrigStride,
                                                     uint16_t * pTarget,
                                                     uint16_t MaskColour,
                                                     uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q_u16(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

//        vHwPixelAlpha = vpselq(vdupq_n_u16(hwOpacity),
//                               vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
//                               vcmphiq_n_u16(vHwPixelAlpha, 255));
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

//        vHwPixelAlpha = vpselq(vdupq_n_u16(hwOpacity),
//                               vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
//                               vcmpeqq_n_u16(vHwPixelAlpha, 255));
    }
#endif

    vHwPixelAlpha = vpselq(vdupq_n_u16(hwOpacity),
                           vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
                           vcmphiq_n_u16(vHwPixelAlpha, 255));

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t      vBlended;
    __arm_2d_color_fast_rgb_t tSrcPix;

    __arm_2d_rgb565_unpack(*(&MaskColour), &tSrcPix);

    vBlended = __arm_2d_rgb565_blending_single_vec_with_scal(vTarget, &tSrcPix, vhwTransparency);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(pTarget, vTarget, predTail);
}


static
void __MVE_WRAPPER( __arm_2d_impl_rgb565_get_alpha_with_opacity_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * pTarget,
                                        uint16_t MaskColour,
                                        uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vld1q_u16(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif

    vHwPixelAlpha = 
        vpselq(vdupq_n_u16(hwOpacity),
               vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
               vcmphiq_n_u16(vHwPixelAlpha, 255));

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t      vBlended;
    __arm_2d_color_fast_rgb_t tSrcPix;

    __arm_2d_rgb565_unpack(*(&MaskColour), &tSrcPix);

    vBlended = __arm_2d_rgb565_blending_single_vec_with_scal(vTarget, 
                                                             &tSrcPix, 
                                                             vhwTransparency);

    vst1q(pTarget, vBlended);
}


static
void __MVE_WRAPPER( __arm_2d_impl_cccn888_get_alpha_with_opacity )(ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t * pTarget,
                                                    uint32_t MaskColour,
                                                    uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif

    vHwPixelAlpha = vpselq(vdupq_n_u16(hwOpacity),
                           vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
                           vcmphiq_n_u16(vHwPixelAlpha, 255));

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    arm_2d_color_bgra8888_t tSrcPix;
    tSrcPix.tValue = MaskColour;
    uint16x8_t      vTargetR, vTargetG, vTargetB;
    uint16x8_t      vBlendedR, vBlendedG, vBlendedB;

    /* get vectors of 8 x R, G, B pix */
    __arm_2d_unpack_rgb888_from_mem((const uint8_t *) pTarget, &vTargetR, &vTargetG, &vTargetB);

    /* merge vector with expanded Mask colour */
    vBlendedR = vqaddq(vTargetR * vhwTransparency, vmulq_n_u16(vHwPixelAlpha, tSrcPix.u8R));
    vBlendedR = vBlendedR >> 8;

    vBlendedG = vqaddq(vTargetG * vhwTransparency, vmulq_n_u16(vHwPixelAlpha, tSrcPix.u8G));
    vBlendedG = vBlendedG >> 8;

    vBlendedB = vqaddq(vTargetB * vhwTransparency, vmulq_n_u16(vHwPixelAlpha, tSrcPix.u8B));
    vBlendedB = vBlendedB >> 8;

    /* select between target pixel, averaged pixed */
    vTargetR = vpselq_u16(vBlendedR, vTargetR, predGlb);
    vTargetG = vpselq_u16(vBlendedG, vTargetG, predGlb);
    vTargetB = vpselq_u16(vBlendedB, vTargetB, predGlb);

    /* pack */
    uint16x8_t      sg = vidupq_n_u16(0, 4);
    vstrbq_scatter_offset_p_u16((uint8_t *) pTarget, sg,
                                vminq(vTargetB, vdupq_n_u16(255)), predTail);
    vstrbq_scatter_offset_p_u16((uint8_t *) pTarget + 1, sg,
                                vminq(vTargetG, vdupq_n_u16(255)), predTail);
    vstrbq_scatter_offset_p_u16((uint8_t *) pTarget + 2, sg,
                                vminq(vTargetR, vdupq_n_u16(255)), predTail);

}

static
void __MVE_WRAPPER( __arm_2d_impl_cccn888_get_alpha_with_opacity_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t * pTarget,
                                                    uint32_t MaskColour,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif

    vHwPixelAlpha = vpselq(vdupq_n_u16(hwOpacity),
                           vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
                           vcmphiq_n_u16(vHwPixelAlpha, 255));

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    arm_2d_color_bgra8888_t tSrcPix;
    tSrcPix.tValue = MaskColour;
    uint16x8_t      vTargetR, vTargetG, vTargetB;
    uint16x8_t      vBlendedR, vBlendedG, vBlendedB;

    /* get vectors of 8 x R, G, B pix */
    __arm_2d_unpack_rgb888_from_mem((const uint8_t *) pTarget, 
                                    &vTargetR, 
                                    &vTargetG, 
                                    &vTargetB);

    /* merge vector with expanded Mask colour */
    vBlendedR = vqaddq(vTargetR * vhwTransparency, 
                        vmulq_n_u16(vHwPixelAlpha, tSrcPix.u8R));
    vBlendedR = vBlendedR >> 8;

    vBlendedG = vqaddq(vTargetG * vhwTransparency, 
                        vmulq_n_u16(vHwPixelAlpha, tSrcPix.u8G));
    vBlendedG = vBlendedG >> 8;

    vBlendedB = vqaddq(vTargetB * vhwTransparency, 
                        vmulq_n_u16(vHwPixelAlpha, tSrcPix.u8B));
    vBlendedB = vBlendedB >> 8;

    /* pack */
    uint16x8_t      sg = vidupq_n_u16(0, 4);
    vstrbq_scatter_offset_u16((uint8_t *) pTarget, sg,
                                vminq(vBlendedB, vdupq_n_u16(255)));
    vstrbq_scatter_offset_u16((uint8_t *) pTarget + 1, sg,
                                vminq(vBlendedG, vdupq_n_u16(255)));
    vstrbq_scatter_offset_u16((uint8_t *) pTarget + 2, sg,
                                vminq(vBlendedR, vdupq_n_u16(255)));

}

static
void __MVE_WRAPPER( __arm_2d_impl_rgb565_get_pixel_colour)(
                        ARM_2D_POINT_VEC * ptPoint,
                        arm_2d_region_t * ptOrigValidRegion,
                        uint16_t * pOrigin,
                        int16_t iOrigStride,
                        uint16_t * pTarget, uint16_t MaskColour, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) && __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_CLR(
                NEAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                iOrigStride, MaskColour, vTarget, predTail, predGlb, vAvgPixelR,
                vAvgPixelG, vAvgPixelB);

        /* pack */
        vDstPixel = __arm_2d_rgb565_pack_single_vec(
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK(
                            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                            MaskColour, vTarget, predTail, vDstPixel, predGlb );
    }
#endif

    vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    /* update target pixels */
    vst1q_p(pTarget, vTarget, predTail);
}


static
void __MVE_WRAPPER( __arm_2d_impl_rgb565_get_pixel_colour_inside_src)(
                        ARM_2D_POINT_VEC * ptPoint,
                        arm_2d_region_t * ptOrigValidRegion,
                        uint16_t * pOrigin,
                        int16_t iOrigStride,
                        uint16_t * pTarget, 
                        uint16_t MaskColour)
{
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) && __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_CLR_INSIDE_SRC(
                NEAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                iOrigStride, MaskColour, vTarget, vAvgPixelR,
                vAvgPixelG, vAvgPixelB);

        /* pack */
        vDstPixel = __arm_2d_rgb565_pack_single_vec(
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_INSIDE_SRC(
                            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                            MaskColour, vTarget, vDstPixel );
    }
#endif

    //vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    /* update target pixels */
    vst1q(pTarget, vDstPixel);
}

static
void __MVE_WRAPPER( __arm_2d_impl_rgb565_transform_only_get_pixel_colour)(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint16_t * pOrigin,
                                            int16_t iOrigStride,
                                            uint16_t * pTarget, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX(
                    NEAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                    iOrigStride, predTail, predGlb, vAvgPixelR,
                    vAvgPixelG, vAvgPixelB);

        /* pack */
        vDstPixel = __arm_2d_rgb565_pack_single_vec(
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK(
                            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                            predTail, vDstPixel, predGlb );
    }
#endif
    vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    /* update target pixels */
    vst1q_p(pTarget, vTarget, predTail);
}

static
void __MVE_WRAPPER( 
            __arm_2d_impl_rgb565_transform_only_get_pixel_colour_inside_src)(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint16_t * pOrigin,
                                            int16_t iOrigStride,
                                            uint16_t * pTarget)
{
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_INSIDE_SRC(
                    NEAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                    iOrigStride, vAvgPixelR, vAvgPixelG, vAvgPixelB);

        /* pack */
        vDstPixel = __arm_2d_rgb565_pack_single_vec(
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_INSIDE_SRC(
                            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                            vDstPixel );
    }
#endif
    //vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    /* update target pixels */
    vst1q(pTarget, vDstPixel);
}


static
void __MVE_WRAPPER( __arm_2d_impl_rgb565_get_pixel_colour_offs_compensated)(
                        ARM_2D_POINT_VEC * ptPoint,
                        arm_2d_region_t * ptOrigValidRegion,
                        uint16_t * pOrigin,
                        int16_t iOrigStride,
                        uint16_t * pTarget, uint16_t MaskColour, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_CLR(
                FAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                iOrigStride, MaskColour, vTarget, predTail, predGlb, vAvgPixelR,
                vAvgPixelG, vAvgPixelB);

        /* pack */
        vDstPixel = __arm_2d_rgb565_pack_single_vec(
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_FAR(
                            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                            MaskColour, vTarget, predTail, vDstPixel, predGlb);
    }
#endif

    vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    /* update target pixels */
    vst1q_p(pTarget, vTarget, predTail);
}

static
void __MVE_WRAPPER( 
            __arm_2d_impl_rgb565_get_pixel_colour_offs_compensated_inside_src)(
                        ARM_2D_POINT_VEC * ptPoint,
                        arm_2d_region_t * ptOrigValidRegion,
                        uint16_t * pOrigin,
                        int16_t iOrigStride,
                        uint16_t * pTarget, uint16_t MaskColour)
{
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_CLR_INSIDE_SRC(
                FAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                iOrigStride, MaskColour, vTarget, vAvgPixelR,
                vAvgPixelG, vAvgPixelB);

        /* pack */
        vDstPixel = __arm_2d_rgb565_pack_single_vec(
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_FAR_INSIDE_SRC(
                            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                            MaskColour, vTarget, vDstPixel);
    }
#endif

    /* update target pixels */
    vst1q(pTarget, vDstPixel);
}

static
void __MVE_WRAPPER(
        __arm_2d_impl_rgb565_transform_only_get_pixel_colour_offs_compensated)(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint16_t * pOrigin,
                                            int16_t iOrigStride,
                                            uint16_t * pTarget, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX(
                    FAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                    iOrigStride, predTail, predGlb, vAvgPixelR,
                    vAvgPixelG, vAvgPixelB);

        /* pack */
        vDstPixel = __arm_2d_rgb565_pack_single_vec(
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_FAR(
                            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                            predTail, vDstPixel, predGlb);
    }
#endif

    vTarget = vpselq_u16(vDstPixel, vTarget, predGlb);

    /* update target pixels */
    vst1q_p(pTarget, vTarget, predTail);
}


static
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_transform_only_get_pixel_colour_offs_compensated_inside_src)(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint16_t * pOrigin,
                                            int16_t iOrigStride,
                                            uint16_t * pTarget)
{
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vDstPixel;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_INSIDE_SRC(
                    FAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                    iOrigStride, vAvgPixelR,
                    vAvgPixelG, vAvgPixelB);

        /* pack */
        vDstPixel = __arm_2d_rgb565_pack_single_vec(
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_NOUNPK_FAR_INSIDE_SRC(
                            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                            vDstPixel);
    }
#endif

    /* update target pixels */
    vst1q(pTarget, vDstPixel);
}

static
void __MVE_WRAPPER( __arm_2d_impl_rgb565_get_pixel_colour_with_alpha)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * pTarget,
                                        uint16_t MaskColour,
                                        uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_CLR(
                NEAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                iOrigStride, MaskColour, vTarget, predTail, predGlb, vAvgPixelR,
                vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR(
                vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                MaskColour, vTarget, predTail, vAvgR, vAvgG, vAvgB, predGlb );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(
                (uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB, vBlended);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(pTarget, vTarget, predTail);
}

static
void __MVE_WRAPPER( __arm_2d_impl_rgb565_get_pixel_colour_with_alpha_inside_src)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * pTarget,
                                        uint16_t MaskColour,
                                        uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_CLR_INSIDE_SRC(
                NEAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                iOrigStride, MaskColour, vTarget, vAvgPixelR,
                vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR_INSIDE_SRC(
                vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                MaskColour, vTarget, vAvgR, vAvgG, vAvgB );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(
                (uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB, vBlended);

    vst1q(pTarget, vBlended);
}

static
void __MVE_WRAPPER( 
        __arm_2d_impl_rgb565_transform_only_get_pixel_colour_with_alpha)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * pTarget,
                                        uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX(
                    NEAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                    iOrigStride, predTail, predGlb, vAvgPixelR,
                    vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT(
                            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                            predTail, vAvgR, vAvgG, vAvgB, predGlb );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC((uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB,
                                        vBlended);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(pTarget, vTarget, predTail);
}

static
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_transform_only_get_pixel_colour_with_alpha_inside_src)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * pTarget,
                                        uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_INSIDE_SRC(
                    NEAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                    iOrigStride, vAvgPixelR, vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_INSIDE_SRC(
                            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                            vAvgR, vAvgG, vAvgB );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(
                (uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB, vBlended);

    vst1q(pTarget, vBlended);
}

static
void __MVE_WRAPPER( 
            __arm_2d_impl_rgb565_get_pixel_colour_with_alpha_offs_compensated)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * pTarget,
                                        uint16_t MaskColour,
                                        uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_CLR(
            FAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
            iOrigStride, MaskColour, vTarget, predTail, predGlb, vAvgPixelR,
            vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR_FAR(
                vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                MaskColour, vTarget, predTail, vAvgR, vAvgG, vAvgB, predGlb );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(
                (uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB, vBlended);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(pTarget, vTarget, predTail);
}

static
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_get_pixel_colour_with_alpha_offs_compensated_inside_src)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * pTarget,
                                        uint16_t MaskColour,
                                        uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_MASK_CLR_INSIDE_SRC(
            FAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
            iOrigStride, MaskColour, vTarget, vAvgPixelR,
            vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_MASK_CLR_FAR_INSIDE_SRC(
                vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                MaskColour, vTarget, vAvgR, vAvgG, vAvgB);
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(
                (uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB, vBlended);

    vst1q(pTarget, vBlended);
}

static
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_transform_only_get_pixel_colour_with_alpha_offs_compensated)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * pTarget,
                                        uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX(
                    FAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                    iOrigStride, predTail, predGlb, vAvgPixelR,
                    vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_FAR(
                        vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                        predTail, vAvgR, vAvgG, vAvgB, predGlb );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(
                (uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB, vBlended);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(pTarget, vTarget, predTail);
}

static
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_transform_only_get_pixel_colour_with_alpha_offs_compensated_inside_src)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * pTarget,
                                        uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vld1q(pTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_INSIDE_SRC(
                    FAR_OFFSET, ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion,
                    iOrigStride, vAvgPixelR,
                    vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_FAR_INSIDE_SRC(
                        vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                        vAvgR, vAvgG, vAvgB );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(
                (uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB, vBlended);

    vst1q(pTarget, vBlended);
}


static
void __MVE_WRAPPER( __arm_2d_impl_cccn888_get_pixel_colour)(
                        ARM_2D_POINT_VEC * ptPoint,
                        arm_2d_region_t * ptOrigValidRegion,
                        uint32_t * pOrigin,
                        int16_t iOrigStride,
                        uint32_t * pTarget, uint32_t MaskColour, int16_t elts)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    uint32x4_t      vTargetLo = vld1q(pTarget);
    uint32x4_t      vTargetHi = vld1q(pTarget + 4);
    mve_pred16_t    predTailLow = vctp32q(elts);
    mve_pred16_t    predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;

    /* predicate accumulators */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlbLo = 0, predGlbHi = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;


        __ARM2D_AVG_NEIGHBR_CCCN888_PIX_MASK_CLR(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh,
                    predGlbLo, predGlbHi,
                    vAvgPixelR, vAvgPixelG, vAvgPixelB);

        {
            ARM_ALIGN(8)    uint32_t scratch32[32];
            /* pack */
            __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, 
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG), 
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));

            uint32x4_t      TempPixel = vld1q(scratch32);

            /* select between target pixel, averaged pixed */
            TempPixel = vpselq_u32(TempPixel, vTargetLo, predGlbLo);

            vst1q_p(pTarget, TempPixel, predTailLow);

            TempPixel = vld1q(scratch32 + 4);

            /* select between target pixel, averaged pixed */
            TempPixel = vpselq_u32(TempPixel, vTargetHi, predGlbHi);

            vst1q_p(pTarget + 4, TempPixel, predTailHigh);
        }
    }
#else
    {
        uint32x4_t      pointLo, pointHi;

        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_CLR(
                vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh, 
                pointLo, pointHi, predGlbLo, predGlbHi);

        uint32x4_t vPixel = vpselq_u32(pointLo, vTargetLo, predGlbLo);

        vst1q_p(pTarget, vPixel, predTailLow);

        vPixel = vpselq_u32(pointHi, vTargetHi, predGlbHi);

        vst1q_p(pTarget + 4, vPixel, predTailHigh);
    }
#endif
}

static
void __MVE_WRAPPER( __arm_2d_impl_cccn888_get_pixel_colour_inside_src)(
                        ARM_2D_POINT_VEC * ptPoint,
                        arm_2d_region_t * ptOrigValidRegion,
                        uint32_t * pOrigin,
                        int16_t iOrigStride,
                        uint32_t * pTarget, uint32_t MaskColour)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    uint32x4_t      vTargetLo = vld1q(pTarget);
    uint32x4_t      vTargetHi = vld1q(pTarget + 4);

    /* predicate accumulators */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlbLo = 0, predGlbHi = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;


        __ARM2D_AVG_NEIGHBR_CCCN888_PIX_MASK_CLR_INSIDE_SRC(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    MaskColour, vTargetLo, vTargetHi,
                    vAvgPixelR, vAvgPixelG, vAvgPixelB);

        {
            ARM_ALIGN(8)    uint32_t scratch32[32];
            /* pack */
            __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, 
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG), 
                                    __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));

            uint32x4_t      TempPixel = vld1q(scratch32);

            /* select between target pixel, averaged pixed */
            //TempPixel = vpselq_u32(TempPixel, vTargetLo, predGlbLo);

            vst1q(pTarget, TempPixel);

            TempPixel = vld1q(scratch32 + 4);

            /* select between target pixel, averaged pixed */
            //TempPixel = vpselq_u32(TempPixel, vTargetHi, predGlbHi);

            vst1q(pTarget + 4, TempPixel);
        }
    }
#else
    {
        uint32x4_t      pointLo, pointHi;

        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK_MASK_CLR_INSIDE_SRC(
                vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                MaskColour, vTargetLo, vTargetHi,
                pointLo, pointHi);

        //uint32x4_t vPixel = vpselq_u32(pointLo, vTargetLo, predGlbLo);

        vst1q(pTarget, pointLo);

        //vPixel = vpselq_u32(pointHi, vTargetHi, predGlbHi);

        vst1q(pTarget + 4, pointHi);
    }
#endif
}

static
void __MVE_WRAPPER( __arm_2d_impl_cccn888_transform_only_get_pixel_colour)(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint32_t * pOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pTarget, int16_t elts)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    uint32x4_t      vTargetLo = vld1q(pTarget);
    uint32x4_t      vTargetHi = vld1q(pTarget + 4);
    mve_pred16_t    predTailLow = vctp32q(elts);
    mve_pred16_t    predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;

    /* predicate accumulators */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlbLo = 0, predGlbHi = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) && __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;


        __ARM2D_AVG_NEIGHBR_CCCN888_PIX(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    predTailLow, predTailHigh,
                    predGlbLo, predGlbHi,
                    vAvgPixelR, vAvgPixelG, vAvgPixelB);

        {
            ARM_ALIGN(8) uint32_t scratch32[32];
            /* pack */
            __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, 
                                        __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                        __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG), 
                                        __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));

            uint32x4_t      TempPixel = vld1q(scratch32);

            /* select between target pixel, averaged pixed */
            TempPixel = vpselq_u32(TempPixel, vTargetLo, predGlbLo);

            vst1q_p(pTarget, TempPixel, predTailLow);

            TempPixel = vld1q(scratch32 + 4);

            /* select between target pixel, averaged pixed */
            TempPixel = vpselq_u32(TempPixel, vTargetHi, predGlbHi);

            vst1q_p(pTarget + 4, TempPixel, predTailHigh);
        }
    }
#else
    {
        uint32x4_t      pointLo, pointHi;

        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK(
            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
            predTailLow, predTailHigh, pointLo, pointHi, predGlbLo, predGlbHi);

        uint32x4_t vPixel = vpselq_u32(pointLo, vTargetLo, predGlbLo);

        vst1q_p(pTarget, vPixel, predTailLow);

        vPixel = vpselq_u32(pointHi, vTargetHi, predGlbHi);

        vst1q_p(pTarget + 4, vPixel, predTailHigh);
    }
#endif
}

static
void __MVE_WRAPPER( __arm_2d_impl_cccn888_transform_only_get_pixel_colour_inside_src)(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint32_t * pOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pTarget)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    uint32x4_t      vTargetLo = vld1q(pTarget);
    uint32x4_t      vTargetHi = vld1q(pTarget + 4);

    /* predicate accumulators */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlbLo = 0, predGlbHi = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) && __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;


        __ARM2D_AVG_NEIGHBR_CCCN888_PIX_INSIDE_SRC(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    vAvgPixelR, vAvgPixelG, vAvgPixelB);

        {
            ARM_ALIGN(8) uint32_t scratch32[32];
            /* pack */
            __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, 
                                        __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR),
                                        __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG), 
                                        __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB));

            uint32x4_t TempPixel = vld1q(scratch32);

            /* select between target pixel, averaged pixed */
            //TempPixel = vpselq_u32(TempPixel, vTargetLo, predGlbLo);

            vst1q(pTarget, TempPixel);

            TempPixel = vld1q(scratch32 + 4);

            /* select between target pixel, averaged pixed */
            //TempPixel = vpselq_u32(TempPixel, vTargetHi, predGlbHi);

            vst1q(pTarget + 4, TempPixel);
        }
    }
#else
    {
        uint32x4_t      pointLo, pointHi;

        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_NOUNPK_INSIDE_SRC(
            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
            pointLo, pointHi);

        //uint32x4_t vPixel = vpselq_u32(pointLo, vTargetLo, predGlbLo);

        vst1q(pTarget, pointLo);

        //vPixel = vpselq_u32(pointHi, vTargetHi, predGlbHi);

        vst1q(pTarget + 4, pointHi);
    }
#endif
}

static
void __MVE_WRAPPER( __arm_2d_impl_cccn888_get_pixel_colour_with_alpha)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint32_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint32_t * pTarget,
                                        uint32_t MaskColour,
                                        uint_fast16_t hwOpacity, int16_t elts)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    uint32x4_t      vTargetLo = vld1q(pTarget);
    uint32x4_t      vTargetHi = vld1q(pTarget + 4);
    mve_pred16_t    predTailLow = vctp32q(elts);
    mve_pred16_t    predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;
    uint16x8_t      vAvgR, vAvgG, vAvgB;


    /* predicate accumulators */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlbLo = 0, predGlbHi = 0;


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) && __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_CCCN888_PIX_MASK_CLR(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh,
                    predGlbLo, predGlbHi,
                    vAvgPixelR, vAvgPixelG,
                    vAvgPixelB);
        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        uint16x8_t      R, G, B;

        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR(
            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride, 
            MaskColour, vTargetLo, vTargetHi, predTailLow, predTailHigh, 
            R, G, B, predGlbLo, predGlbHi);

        vAvgR = R;
        vAvgG = G;
        vAvgB = B;
    }
#endif


    /* alpha blending */
    uint16x8_t      vTargetR, vTargetG, vTargetB;
    uint16_t        transp = 256 - hwOpacity;
    ARM_ALIGN(8)    uint32_t scratch32[32];

    __arm_2d_unpack_rgb888_from_mem((const uint8_t *) pTarget, 
                                    &vTargetR, 
                                    &vTargetG, 
                                    &vTargetB);

    /* merge */
    __ARM_2D_8BIT_RGB_MIX(  vAvgR, vAvgG, vAvgB, (uint16_t) hwOpacity,
                            vTargetR, vTargetG, vTargetB, transp);

    /* pack */
    __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, vAvgR, vAvgG, vAvgB);

    uint32x4_t      TempPixel = vld1q(scratch32);

    /* select between target pixel, averaged pixed */
    TempPixel = vpselq_u32(TempPixel, vTargetLo, predGlbLo);

    vst1q_p(pTarget, TempPixel, predTailLow);

    TempPixel = vld1q(scratch32 + 4);

    /* select between target pixel, averaged pixed */
    TempPixel = vpselq_u32(TempPixel, vTargetHi, predGlbHi);

    vst1q_p(pTarget + 4, TempPixel, predTailHigh);
}


static
void __MVE_WRAPPER( __arm_2d_impl_cccn888_get_pixel_colour_with_alpha_inside_src)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint32_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint32_t * pTarget,
                                        uint32_t MaskColour,
                                        uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    uint32x4_t      vTargetLo = vld1q(pTarget);
    uint32x4_t      vTargetHi = vld1q(pTarget + 4);

    uint16x8_t      vAvgR, vAvgG, vAvgB;


    /* predicate accumulators */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlbLo = 0, predGlbHi = 0;


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) && __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_CCCN888_PIX_MASK_CLR_INSIDE_SRC(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    MaskColour, vTargetLo, vTargetHi,
                    vAvgPixelR, vAvgPixelG,
                    vAvgPixelB);
        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        uint16x8_t      R, G, B;

        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_MASK_CLR_INSIDE_SRC(
            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride, 
            MaskColour, vTargetLo, vTargetHi,
            R, G, B);

        vAvgR = R;
        vAvgG = G;
        vAvgB = B;
    }
#endif


    /* alpha blending */
    uint16x8_t      vTargetR, vTargetG, vTargetB;
    uint16_t        transp = 256 - hwOpacity;
    ARM_ALIGN(8)    uint32_t scratch32[32];

    __arm_2d_unpack_rgb888_from_mem((const uint8_t *) pTarget, 
                                    &vTargetR, 
                                    &vTargetG, 
                                    &vTargetB);

    /* merge */
    __ARM_2D_8BIT_RGB_MIX(  vAvgR, vAvgG, vAvgB, (uint16_t) hwOpacity,
                            vTargetR, vTargetG, vTargetB, transp);

    /* pack */
    __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, vAvgR, vAvgG, vAvgB);

    uint32x4_t      TempPixel = vld1q(scratch32);

    vst1q(pTarget, TempPixel);

    TempPixel = vld1q(scratch32 + 4);

    vst1q(pTarget + 4, TempPixel);
}

static
void __MVE_WRAPPER(
    __arm_2d_impl_cccn888_transform_only_get_pixel_colour_with_alpha)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint32_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint32_t * pTarget,
                                        uint_fast16_t hwOpacity, int16_t elts)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    uint32x4_t      vTargetLo = vld1q(pTarget);
    uint32x4_t      vTargetHi = vld1q(pTarget + 4);
    mve_pred16_t    predTailLow = vctp32q(elts);
    mve_pred16_t    predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;
    uint16x8_t      vAvgR, vAvgG, vAvgB;


    /* predicate accumulators */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlbLo = 0, predGlbHi = 0;


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) && __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_CCCN888_PIX(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    predTailLow, predTailHigh,
                    predGlbLo, predGlbHi,
                    vAvgPixelR, vAvgPixelG,
                    vAvgPixelB);
        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        uint16x8_t      R, G, B;

        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT(
            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
            predTailLow, predTailHigh, R, G, B, predGlbLo, predGlbHi);

        vAvgR = R;
        vAvgG = G;
        vAvgB = B;
    }
#endif

    /* alpha blending */
    uint16x8_t      vTargetR, vTargetG, vTargetB;
    uint16_t        transp = 256 - hwOpacity;
    ARM_ALIGN(8)    uint32_t scratch32[32];

    __arm_2d_unpack_rgb888_from_mem((const uint8_t *) pTarget, 
                                    &vTargetR, 
                                    &vTargetG, 
                                    &vTargetB);

    /* merge */
    __ARM_2D_8BIT_RGB_MIX(  vAvgR, vAvgG, vAvgB, (uint16_t) hwOpacity,
                            vTargetR, vTargetG, vTargetB, transp);

    /* pack */
    __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, vAvgR, vAvgG, vAvgB);

    uint32x4_t      TempPixel = vld1q(scratch32);

    /* select between target pixel, averaged pixed */
    TempPixel = vpselq_u32(TempPixel, vTargetLo, predGlbLo);

    vst1q_p(pTarget, TempPixel, predTailLow);

    TempPixel = vld1q(scratch32 + 4);

    /* select between target pixel, averaged pixed */
    TempPixel = vpselq_u32(TempPixel, vTargetHi, predGlbHi);

    vst1q_p(pTarget + 4, TempPixel, predTailHigh);
}


static
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_transform_only_get_pixel_colour_with_alpha_inside_src)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint32_t * pOrigin,
                                        int16_t iOrigStride,
                                        uint32_t * pTarget,
                                        uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);

    uint32x4_t      vTargetLo = vld1q(pTarget);
    uint32x4_t      vTargetHi = vld1q(pTarget + 4);

    uint16x8_t      vAvgR, vAvgG, vAvgB;


    /* predicate accumulators */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlbLo = 0, predGlbHi = 0;


#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) && __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_CCCN888_PIX_INSIDE_SRC(
                    ptPoint, vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
                    vAvgPixelR, vAvgPixelG,
                    vAvgPixelB);
        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        uint16x8_t      R, G, B;

        __ARM_2D_RGB888_GET_RGBVEC_FROM_POINT_INSIDE_SRC(
            vXi, vYi, pOrigin, ptOrigValidRegion, iOrigStride,
            R, G, B);

        vAvgR = R;
        vAvgG = G;
        vAvgB = B;
    }
#endif

    /* alpha blending */
    uint16x8_t      vTargetR, vTargetG, vTargetB;
    uint16_t        transp = 256 - hwOpacity;
    ARM_ALIGN(8)    uint32_t scratch32[32];

    __arm_2d_unpack_rgb888_from_mem((const uint8_t *) pTarget, 
                                    &vTargetR, 
                                    &vTargetG, 
                                    &vTargetB);

    /* merge */
    __ARM_2D_8BIT_RGB_MIX(  vAvgR, vAvgG, vAvgB, (uint16_t) hwOpacity,
                            vTargetR, vTargetG, vTargetB, transp);

    /* pack */
    __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, vAvgR, vAvgG, vAvgB);

    uint32x4_t      TempPixel = vld1q(scratch32);

    vst1q(pTarget, TempPixel);

    TempPixel = vld1q(scratch32 + 4);

    vst1q(pTarget + 4, TempPixel);
}

/*
 * Src mask variants expansion
 *
 * - transform_with_src_chn_mask_and_opacity
 * - transform_with_src_mask_and_opacity
 * - transform_with_src_chn_mask
 * - transform_with_src_mask
 */


#define __API_INT_TYPE_BIT_NUM      8
#define __API_COLOUR                ARM_2D_M_COLOUR_GRAY8
#define __API_COLOUR_NAME           gray8


#include "__arm_2d_ll_transform_helium.inc"

#define __API_INT_TYPE_BIT_NUM      16
#define __API_COLOUR                ARM_2D_M_COLOUR_RGB565
#define __API_COLOUR_NAME           rgb565

#include "__arm_2d_ll_transform_helium.inc"

#define __API_INT_TYPE_BIT_NUM      32
#define __API_COLOUR                ARM_2D_M_COLOUR_CCCN888
#define __API_COLOUR_NAME           cccn888

#include "__arm_2d_ll_transform_helium.inc"


#ifdef   __cplusplus
}
#endif

#endif // __ARM_2D_HAS_HELIUM__

#endif // __ARM_2D_COMPILATION_UNIT
