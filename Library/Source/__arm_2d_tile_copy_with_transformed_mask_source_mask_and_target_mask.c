/*
 * Copyright (c) 2009-2025 Arm Limited. All rights reserved.
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
 * Title:        __arm_2d_tile_copy_with_transformed_mask_source_mask_and
 *               _target_mask.c
 * Description:  APIs for tile-copy-with-transformed-mask-source-mask-and
 *               -target-mask
 *
 * $Date:        06 Jan 2026
 * $Revision:    v1.0.2
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

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
#   pragma clang diagnostic ignored "-Wunused-variable"
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 174,177,188,68,513,144
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic ignored "-Wunused-variable"
#endif

/*============================ MACROS ========================================*/
#undef OP_CORE
#define OP_CORE this.use_as__arm_2d_op_core_t
#undef OPCODE
#define OPCODE this

#undef __CALIBFX
#define __CALIBFX 590

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask                                           *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_GRAY8:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __gray8_tile_copy_with_transformed_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
            pchExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
        pchExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_gray8_tile_copy_with_transformed_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __gray8_tile_copy_with_transformed_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchExtraSourceLine++);
        }

        pchTargetBase           += iTargetStride;
        pchExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_gray8_with_transformed_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
            pwExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
        pwExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_gray8_with_transformed_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pwExtraSourceLine++);
        }

        pchTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_gray8_sw_tile_copy_with_transformed_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_gray8_tile_copy_with_transformed_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_GRAY8,
    __arm_2d_gray8_sw_tile_copy_with_transformed_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_GRAY8),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask and Opacity                               *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_GRAY8:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __gray8_tile_copy_with_transformed_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
            pchExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
        pchExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __gray8_tile_copy_with_transformed_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchExtraSourceLine++,
                            chOpacity);
        }

        pchTargetBase           += iTargetStride;
        pchExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_gray8_with_transformed_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
            pwExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
        pwExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_gray8_with_transformed_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pchTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_gray8_sw_tile_copy_with_transformed_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_gray8_sw_tile_copy_with_transformed_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_GRAY8,
    __arm_2d_gray8_sw_tile_copy_with_transformed_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_GRAY8),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask and Target Mask                           *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_and_target_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_GRAY8:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __gray8_tile_copy_with_transformed_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
            pchExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
        pchExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __gray8_tile_copy_with_transformed_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchTargetMask,
                            pchExtraSourceLine++);
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pchExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_gray8_with_transformed_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
            pwExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
        pwExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_gray8_with_transformed_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchTargetMask,
                            pwExtraSourceLine++);
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_gray8_sw_tile_copy_with_transformed_mask_and_target_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_GRAY8,
    __arm_2d_gray8_sw_tile_copy_with_transformed_mask_and_target_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_GRAY8),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Target Mask and Opacity                  *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_target_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_GRAY8:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __gray8_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
            pchExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
        pchExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_gray8_tile_copy_with_transformed_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __gray8_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchTargetMask,
                            pchExtraSourceLine++,
                            chOpacity);
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pchExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_gray8_with_transformed_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
            pwExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
        pwExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_gray8_with_transformed_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchTargetMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_gray8_sw_tile_copy_with_transformed_mask_target_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_gray8_sw_tile_copy_with_transformed_mask_and_target_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_gray8_tile_copy_with_transformed_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_GRAY8,
    __arm_2d_gray8_sw_tile_copy_with_transformed_mask_target_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_GRAY8),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask and Source Mask                           *
 *---------------------------------------------------------------------------*/


/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_and_source_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_GRAY8:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __gray8_tile_copy_with_transformed_mask_and_source_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint8_t *pchExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
            pchExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
        pchExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_source_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __gray8_tile_copy_with_transformed_mask_and_source_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchExtraSourceMask,
                            pchExtraSourceLine++);
        }

        pchTargetBase           += iTargetStride;
        pchExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_gray8_with_transformed_mask_and_source_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
            pwExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
        pwExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_source_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_gray8_with_transformed_mask_and_source_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchExtraSourceMask,
                            pwExtraSourceLine++);
        }

        pchTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_gray8_sw_tile_copy_with_transformed_mask_and_source_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_source_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_source_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_GRAY8,
    __arm_2d_gray8_sw_tile_copy_with_transformed_mask_and_source_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_GRAY8),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Source Mask and Opacity                  *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_GRAY8:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __gray8_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint8_t *pchExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
            pchExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
        pchExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __gray8_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchExtraSourceMask,
                            pchExtraSourceLine++,
                            chOpacity);
        }

        pchTargetBase           += iTargetStride;
        pchExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
            pwExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
        pwExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchExtraSourceMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pchTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_gray8_sw_tile_copy_with_transformed_mask_source_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_gray8_sw_tile_copy_with_transformed_mask_and_source_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_GRAY8,
    __arm_2d_gray8_sw_tile_copy_with_transformed_mask_source_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_GRAY8),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Source Mask, and Target Mask             *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4,5)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_GRAY8:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint8_t *pchExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
            pchExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
        pchExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            pchExtraSourceLine++);
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pchExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
            pwExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
        pwExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            pwExtraSourceLine++);
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_gray8_sw_tile_copy_with_transformed_mask_source_mask_and_target_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_GRAY8,
    __arm_2d_gray8_sw_tile_copy_with_transformed_mask_source_mask_and_target_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_GRAY8),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Source Mask, Target Mask and Opacity     *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4,5)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_GRAY8:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint8_t *pchExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
            pchExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_GRAY8( 
        pchExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pchExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            pchExtraSourceLine++,
                            chOpacity);
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pchExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
            pwExtraSource, 
            pchTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8 (
        pwExtraSource, 
        pchTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t  *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pchTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pchTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_gray8_sw_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_gray8_sw_tile_copy_with_transformed_mask_source_mask_and_target_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_GRAY8,
    __arm_2d_gray8_sw_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_GRAY8),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask                                           *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_RGB565:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __rgb565_tile_copy_with_transformed_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint16_t *phwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
            phwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
        phwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_rgb565_tile_copy_with_transformed_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __rgb565_tile_copy_with_transformed_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            phwExtraSourceLine++);
        }

        phwTargetBase           += iTargetStride;
        phwExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_rgb565_with_transformed_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
            pwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
        pwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_rgb565_with_transformed_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pwExtraSourceLine++);
        }

        phwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_rgb565_sw_tile_copy_with_transformed_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_rgb565_tile_copy_with_transformed_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_RGB565,
    __arm_2d_rgb565_sw_tile_copy_with_transformed_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_RGB565),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask and Opacity                               *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_RGB565:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __rgb565_tile_copy_with_transformed_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint16_t *phwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
            phwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
        phwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __rgb565_tile_copy_with_transformed_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            phwExtraSourceLine++,
                            chOpacity);
        }

        phwTargetBase           += iTargetStride;
        phwExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
            pwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
        pwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        phwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_rgb565_sw_tile_copy_with_transformed_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_rgb565_sw_tile_copy_with_transformed_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_RGB565,
    __arm_2d_rgb565_sw_tile_copy_with_transformed_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_RGB565),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask and Target Mask                           *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_and_target_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_RGB565:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __rgb565_tile_copy_with_transformed_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchTargetMask,
                                            uint16_t *phwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
            phwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
        phwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __rgb565_tile_copy_with_transformed_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchTargetMask,
                            phwExtraSourceLine++);
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        phwExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
            pwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
        pwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchTargetMask,
                            pwExtraSourceLine++);
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_rgb565_sw_tile_copy_with_transformed_mask_and_target_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_RGB565,
    __arm_2d_rgb565_sw_tile_copy_with_transformed_mask_and_target_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_RGB565),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Target Mask and Opacity                  *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_target_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_RGB565:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __rgb565_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchTargetMask,
                                            uint16_t *phwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
            phwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
        phwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __rgb565_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchTargetMask,
                            phwExtraSourceLine++,
                            chOpacity);
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        phwExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_rgb565_with_transformed_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
            pwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
        pwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_rgb565_with_transformed_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchTargetMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_rgb565_sw_tile_copy_with_transformed_mask_target_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_rgb565_sw_tile_copy_with_transformed_mask_and_target_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_RGB565,
    __arm_2d_rgb565_sw_tile_copy_with_transformed_mask_target_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_RGB565),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask and Source Mask                           *
 *---------------------------------------------------------------------------*/


/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_and_source_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_RGB565:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __rgb565_tile_copy_with_transformed_mask_and_source_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint16_t *phwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
            phwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
        phwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_source_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __rgb565_tile_copy_with_transformed_mask_and_source_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchExtraSourceMask,
                            phwExtraSourceLine++);
        }

        phwTargetBase           += iTargetStride;
        phwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_source_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
            pwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
        pwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_source_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_source_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchExtraSourceMask,
                            pwExtraSourceLine++);
        }

        phwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_rgb565_sw_tile_copy_with_transformed_mask_and_source_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_source_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_source_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_RGB565,
    __arm_2d_rgb565_sw_tile_copy_with_transformed_mask_and_source_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_RGB565),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Source Mask and Opacity                  *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_RGB565:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint16_t *phwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
            phwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
        phwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchExtraSourceMask,
                            phwExtraSourceLine++,
                            chOpacity);
        }

        phwTargetBase           += iTargetStride;
        phwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
            pwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
        pwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchExtraSourceMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        phwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_rgb565_sw_tile_copy_with_transformed_mask_source_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_rgb565_sw_tile_copy_with_transformed_mask_and_source_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_RGB565,
    __arm_2d_rgb565_sw_tile_copy_with_transformed_mask_source_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_RGB565),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Source Mask, and Target Mask             *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4,5)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_RGB565:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint16_t *phwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
            phwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
        phwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            phwExtraSourceLine++);
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        phwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
            pwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
        pwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            pwExtraSourceLine++);
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_rgb565_sw_tile_copy_with_transformed_mask_source_mask_and_target_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_RGB565,
    __arm_2d_rgb565_sw_tile_copy_with_transformed_mask_source_mask_and_target_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_RGB565),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Source Mask, Target Mask and Opacity     *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4,5)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_RGB565:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint16_t *phwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
            phwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_RGB565( 
        phwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *phwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            phwExtraSourceLine++,
                            chOpacity);
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        phwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
            pwExtraSource, 
            phwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565 (
        pwExtraSource, 
        phwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t  *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                phwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            phwTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_rgb565_sw_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_rgb565_sw_tile_copy_with_transformed_mask_source_mask_and_target_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_RGB565,
    __arm_2d_rgb565_sw_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_RGB565),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask                                           *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_CCCN888:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __cccn888_tile_copy_with_transformed_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_cccn888_tile_copy_with_transformed_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __cccn888_tile_copy_with_transformed_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pwExtraSourceLine++);
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_cccn888_with_transformed_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_cccn888_with_transformed_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pwExtraSourceLine++);
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_cccn888_sw_tile_copy_with_transformed_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_CCCN888 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_cccn888_tile_copy_with_transformed_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_CCCN888,
    __arm_2d_cccn888_sw_tile_copy_with_transformed_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_CCCN888),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask and Opacity                               *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_CCCN888:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __cccn888_tile_copy_with_transformed_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __cccn888_tile_copy_with_transformed_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {

            if (false
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_cccn888_sw_tile_copy_with_transformed_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_CCCN888 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_cccn888_sw_tile_copy_with_transformed_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_CCCN888,
    __arm_2d_cccn888_sw_tile_copy_with_transformed_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_OPACITY_CCCN888),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask and Target Mask                           *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_and_target_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_CCCN888:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __cccn888_tile_copy_with_transformed_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __cccn888_tile_copy_with_transformed_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchTargetMask,
                            pwExtraSourceLine++);
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchTargetMask,
                            pwExtraSourceLine++);
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_cccn888_sw_tile_copy_with_transformed_mask_and_target_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_CCCN888 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_CCCN888,
    __arm_2d_cccn888_sw_tile_copy_with_transformed_mask_and_target_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK_CCCN888),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Target Mask and Opacity                  *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_target_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_CCCN888:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = NULL;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __cccn888_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __cccn888_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchTargetMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase      += iExtraSourceStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_cccn888_with_transformed_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_cccn888_with_transformed_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchTargetMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_cccn888_sw_tile_copy_with_transformed_mask_target_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_CCCN888 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_cccn888_sw_tile_copy_with_transformed_mask_and_target_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_CCCN888,
    __arm_2d_cccn888_sw_tile_copy_with_transformed_mask_target_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_TARGET_MASK_AND_OPACITY_CCCN888),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask and Source Mask                           *
 *---------------------------------------------------------------------------*/


/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_and_source_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_CCCN888:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __cccn888_tile_copy_with_transformed_mask_and_source_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_source_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __cccn888_tile_copy_with_transformed_mask_and_source_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchExtraSourceMask,
                            pwExtraSourceLine++);
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_source_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_source_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_source_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchExtraSourceMask,
                            pwExtraSourceLine++);
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_cccn888_sw_tile_copy_with_transformed_mask_and_source_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_CCCN888 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_source_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_source_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_CCCN888,
    __arm_2d_cccn888_sw_tile_copy_with_transformed_mask_and_source_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK_CCCN888),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Source Mask and Opacity                  *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_CCCN888:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = NULL;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchExtraSourceMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchExtraSourceMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_cccn888_sw_tile_copy_with_transformed_mask_source_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_CCCN888 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_cccn888_sw_tile_copy_with_transformed_mask_and_source_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_CCCN888,
    __arm_2d_cccn888_sw_tile_copy_with_transformed_mask_source_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_OPACITY_CCCN888),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Source Mask, and Target Mask             *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4,5)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_CCCN888:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            pwExtraSourceLine++);
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_target_mask_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = wTotalAlpha >> 8;

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_target_mask(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_target_mask_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            pwExtraSourceLine++);
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_cccn888_sw_tile_copy_with_transformed_mask_source_mask_and_target_mask(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_CCCN888 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        } else 
    #endif
        {
            __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_CCCN888,
    __arm_2d_cccn888_sw_tile_copy_with_transformed_mask_source_mask_and_target_mask);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK_CCCN888),
            .ptFillLike = NULL,
        },
    },
};



/*---------------------------------------------------------------------------*
 * Tile Copy with Transformed Mask, Source Mask, Target Mask and Opacity     *
 *---------------------------------------------------------------------------*/

/*
 * the Frontend API
 */
ARM_NONNULL(2,3,4,5)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);
    assert(NULL != ptSourceMask);
    assert(NULL != ptTransMask);
    assert(NULL != ptTargetMask);

    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_CCCN888:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif
    //! valid source mask tile
    if (!__arm_2d_valid_mask(ptSourceMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    do {
        /*! \note the extra mask tile should be bigger than or equals to the  
         *!       extra source tile.
         */
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_width_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        if (ARM_2D_CMP_SMALLER == arm_2d_tile_height_compare(ptSourceMask, ptSource, true)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    } while(0);

    //! valid mask to be transformed tile
    if (!__arm_2d_valid_mask(ptTransMask,   __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptTargetMask,  __ARM_2D_MASK_ALLOW_A8 
//#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
//                                    |   __ARM_2D_MASK_ALLOW_8in32
//#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!ARM_2D_OP_WAIT_ASYNC(ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptTransMask;
    this.Mask.ptOriginSide = NULL;
    this.Mask.ptTargetSide = ptTargetMask;
    this.wMode = 0;

    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    this.ExtraSource.ptTile = ptSource;
    this.ExtraSource.ptMask = ptSourceMask;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

/* 
 * default low level implementation 
 */

__STATIC_INLINE
void __cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_OPA_CCCN888( 
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

/* 
 * default low level implementation for implicit ccca8888 conversion
 */

__STATIC_INLINE
void __ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_target_mask_and_opacity_process_point( 
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity)
{

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint8_t *pchAlphaSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if !__ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#else
    union {
        uint32_t wValue;
        uint8_t chPixel[4];
    } PixelAlpha;

#undef chPixelAlpha0
#undef chPixelAlpha1
#undef chPixelAlpha2
#undef chPixelAlpha3

#define chPixelAlpha0   PixelAlpha.chPixel[0]
#define chPixelAlpha1   PixelAlpha.chPixel[1]
#define chPixelAlpha2   PixelAlpha.chPixel[2]
#define chPixelAlpha3   PixelAlpha.chPixel[3]

    chPixelAlpha0 = *pchAlphaSample++;
    chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigStride;
    chPixelAlpha3 = *pchAlphaSample--;
    chPixelAlpha2 = *pchAlphaSample;

    if (0 == PixelAlpha.wValue) {
        return ;
    }
#endif

    uint16_t hwAlpha1 = (uint32_t)((uint32_t)hwAlphaX * (256 - (uint32_t)hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;

    wTotalAlpha += (chPixelAlpha0 == 255) * ((uint32_t)hwAlpha0 << 8) 
                + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    
    wTotalAlpha += (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);

    wTotalAlpha += (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);

    wTotalAlpha += (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);

    if (wTotalAlpha) {
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                         + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        uint8_t chTargetAlpha;
        ARM_2D_UNUSED(chTargetAlpha);
        /* apply extra source mask */
        chTargetAlpha = *pchExtraSourceMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
        /* apply target mask */
        chTargetAlpha = *pchTargetMask;
        hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
                + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

        hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
        __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
            pwExtraSource, 
            pwTarget, 
            hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint16_t hwAlpha = pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == hwAlpha) {
        return ;
    }
#endif
    hwAlpha =  (hwAlpha == 255) * hwOpacity
                + !(hwAlpha == 255) * (hwAlpha * hwOpacity >> 8);

    uint8_t chTargetAlpha;
    ARM_2D_UNUSED(chTargetAlpha);
    /* apply extra source mask */
    chTargetAlpha = *pchExtraSourceMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));
    /* apply target mask */
    chTargetAlpha = *pchTargetMask;
    hwAlpha = (chTargetAlpha == 0xFF) * hwAlpha
            + (!(chTargetAlpha == 0xFF) * (chTargetAlpha * hwAlpha >> 8));

    hwAlpha = 256 - hwAlpha;

        /* blend the pixel */
    __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888 (
        pwExtraSource, 
        pwTarget, 
        hwAlpha);
#endif
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_target_mask_and_opacity(
                                    __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t chOpacity)
{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t  *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    chOpacity += (chOpacity == 255);

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        
        for (int_fast16_t x = 0; x < iWidth; x++) {
            uint8_t *pchTargetMask = pchTargetMaskLine++;
            uint8_t *pchExtraSourceMask = pchExtraSourceMaskLine++;

            if (false
                ||  (*pchTargetMask == 0)
                ||  (*pchExtraSourceMask == 0)
            ) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            arm_2d_point_fx_t tPointFast;

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) 
                    - ptOriginValidRegion->tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y)
                    - ptOriginValidRegion->tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptOriginValidRegion->tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptOriginValidRegion->tSize.iHeight - 1))) {
                *pwExtraSourceLine++;
                pwTargetLine++;
                continue;
            }

            __ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_target_mask_and_opacity_process_point (
                            &tPointFast,
                            ptOriginValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pwTargetLine++,
                            pchTargetMask,
                            pchExtraSourceMask,
                            pwExtraSourceLine++,
                            chOpacity);
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * The backend entry
 */
arm_fsm_rt_t 
__arm_2d_cccn888_sw_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(
    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_CCCN888 == OP_CORE.ptOp->Info.Colour.chScheme);
    if (0 == this.chOpacity) {
        return arm_fsm_rt_cpl;
    }

#if __ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__
    else if (255 == this.chOpacity) {
        return __arm_2d_cccn888_sw_tile_copy_with_transformed_mask_source_mask_and_target_mask(   
                ptTask);
    }
#endif

    ARM_TYPE_CONVERT(ptTask->Param
                        .tCopyOrigMaskExtra
                            .use_as____arm_2d_param_copy_orig_msk_t
                                .use_as____arm_2d_param_copy_orig_t
                                    .tOrigin
                                        .pBuffer, intptr_t)
        -= ptTask->Param
                .tCopyOrigMaskExtra
                    .use_as____arm_2d_param_copy_orig_msk_t
                        .use_as____arm_2d_param_copy_orig_t
                            .tOrigin
                                .nOffset;
    arm_2d_err_t tErr;
    ARM_2D_UNUSED(tErr);
    tErr = __arm_mask_validate(NULL, 
                                            NULL, 
                                            this.Target.ptTile, 
                                            this.Mask.ptTargetSide, 
                                            0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    /* NOTE: Since the size of the extra mask is no less than the size of the extra source 
     *       (this is ensured by the pipeline), we can just valid the extra mask here.
     */
    tErr = __arm_mask_validate( NULL, 
                                NULL, 
                                this.Target.ptTile, 
                                this.ExtraSource.ptMask, 
                                0 );
    if (tErr != ARM_2D_ERR_NONE) {
        return (arm_fsm_rt_t)tErr;
    }
    
    __arm_2d_param_copy_orig_msk_t *ptParamCopyOrigMask 
        = &ptTask->Param
            .tCopyOrigMaskExtra
                .use_as____arm_2d_param_copy_orig_msk_t;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptParamCopyOrigMask->use_as____arm_2d_param_copy_orig_t
                    .tOrigin
                        .tColour
                            .chScheme);
    
    bIsMaskChannel8In32 =   bIsMaskChannel8In32 
                        &&  (   ARM_2D_CHANNEL_8in32 
                            ==  ptParamCopyOrigMask->tDesMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    } else 
#endif
    {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
        arm_2d_tile_t *ptExtraSourceRoot = arm_2d_tile_get_root(this.ExtraSource.ptTile, NULL, NULL);
        assert(NULL != ptExtraSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptExtraSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        } else 
    #endif
        {
            __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(   
                &(ptTask->Param.tCopyOrigMaskExtra),
                &this.tTransform,
                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  
    __ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_CCCN888,
    __arm_2d_cccn888_sw_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity);   /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t 
ARM_2D_OP_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme               = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasTarget             = true,
            .bHasTargetMask         = true,
            .bHasOrigin             = true,
            .bHasExtraSource        = true,
            .bHasExtraSourceMask    = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex = __ARM_2D_OP_IDX_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t, tTransform),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_TARGET_MASK_AND_OPACITY_CCCN888),
            .ptFillLike = NULL,
        },
    },
};

#ifdef   __cplusplus
}
#endif

#endif