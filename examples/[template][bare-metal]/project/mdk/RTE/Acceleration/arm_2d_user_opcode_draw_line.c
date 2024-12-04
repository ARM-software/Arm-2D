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

/*============================ INCLUDES ======================================*/
#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"

#include "arm_2d_user_opcode_draw_line.h"

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
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144
#endif

/*============================ MACROS ========================================*/
#undef OP_CORE
#define OP_CORE this.use_as__arm_2d_op_t.use_as__arm_2d_op_core_t
#undef OPCODE
#define OPCODE this.use_as__arm_2d_op_t

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    __ARM_2D_OP_IDX_USER_DRAW_LINE = __ARM_2D_OP_IDX_USER_OP_START,

};


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern
void __arm_2d_impl_rgb565_user_draw_line(
                            arm_2d_user_draw_line_descriptor_t *ptThis,
                            uint16_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static
arm_2d_err_t arm_2dp_rgb565_user_draw_line_prepare(
                            arm_2d_user_draw_line_descriptor_t *ptThis,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_user_draw_line_api_params_t *ptParams)
{
    assert(NULL != ptThis);
    assert(NULL != ptParams);

    /* ensure we increase the Y*/
    if (ptParams->tStart.iY > ptParams->tEnd.iY) {

        arm_2d_location_t tStart = ptParams->tEnd;
        arm_2d_location_t tEnd = ptParams->tStart;

        this.tParams = *ptParams;

        this.tParams.tStart = tStart;
        this.tParams.tEnd = tEnd;

    } else {
        this.tParams = *ptParams;
    }

    q16_t q16DeltaX = reinterpret_q16_s16( this.tParams.tEnd.iX - this.tParams.tStart.iX + 1 );
    q16_t q16DeltaY = reinterpret_q16_s16( this.tParams.tEnd.iY - this.tParams.tStart.iY + 1 );

    // update brush
    float fBrushWidth = ptParams->fBrushWidth;
    if (fabsf(fBrushWidth) < 0.1f) {
        fBrushWidth = 0.1f;
    }

    this.q16BrushWidth = reinterpret_q16_f32(fBrushWidth);

    float fK, fSqrtKKp1, fSqrtKKp1_divK;
    float fYend;
    arm_2d_size_t tEndPointFix = {0};

    if (0 == q16DeltaY) {
        this.bHorizontalLine = true;
    }
    if (0 == q16DeltaX) {
        this.bVerticalLine = true;
    }

    if (this.bHorizontalLine && this.bVerticalLine ) {
        return ARM_2D_ERR_INVALID_PARAM;
    } else if (this.bHorizontalLine) {
        fK = 0.0f;
        this.q16K = 0;
        this.q16dX = __INT32_MAX__;
        this.q16L = this.q16BrushWidth;
        this.q16_SqrtKKp1_divK = __INT32_MAX__;
        this.q16abs_Kp_1divK_ = __INT32_MAX__;
        this.q16Xend = 0;
        this.q16Yend = 0;

        tEndPointFix.iHeight = (fBrushWidth / 2.0f);
        this.q16PixelCompensate = 0;

    } else if (this.bVerticalLine) {

        this.q16K = __INT32_MAX__;
        this.q16dX = 0;
        fSqrtKKp1_divK = 1.0f;
        this.q16Xend = 0;
        this.q16Yend = 0;

        // L = brush * sqrt(k*k + 1) / k
        this.q16L = this.q16BrushWidth;

        this.q16_SqrtKKp1_divK = reinterpret_q16_s16(1);
        this.q16abs_Kp_1divK_ = 0;

        tEndPointFix.iWidth = (fBrushWidth / 2.0f);
        this.q16PixelCompensate = 0;

    } else {
        fK = (float)q16DeltaY / (float)q16DeltaX;
        this.q16abs_Kp_1divK_ = reinterpret_q16_f32(fabsf(fK + 1.0f/fK));

        this.q16K = reinterpret_q16_f32(fK);
        this.q16dX = reinterpret_q16_f32(1.0f / fK);

        // param = sqrt(k*k + 1) / k
        if (ARM_MATH_SUCCESS != arm_sqrt_f32(fK * fK + 1.0f, &fSqrtKKp1)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        fSqrtKKp1_divK = fabsf(fSqrtKKp1 / fK);

        // Xend = brush / [ 2k * sqrt(k*k + 1) ]
        this.q16Xend = reinterpret_q16_f32(fBrushWidth / (2.0f * fK * fSqrtKKp1));

        // Yend = brush / fSqrtKKp1;
        fYend = fBrushWidth / fSqrtKKp1;
        this.q16Yend = reinterpret_q16_f32(fYend);

        // L = brush * sqrt(k*k + 1) / k
        this.q16L = reinterpret_q16_f32(fBrushWidth * fSqrtKKp1_divK);

        this.q16_SqrtKKp1_divK = reinterpret_q16_f32(fSqrtKKp1_divK);

    
        q16_t q16Pixel1divK = div_q16(reinterpret_q16_s16(1), this.q161divK);
        q16_t q16PixelK= div_q16(reinterpret_q16_s16(1), this.q16K);

        q16Pixel1divK = ABS(q16Pixel1divK);
        q16PixelK = ABS(q16PixelK);

        this.q16PixelCompensate = abs_q16(q16Pixel1divK - q16PixelK);

        if (!this.tParams.bHorizontallyChoppedEndpoints) {
            tEndPointFix.iHeight = reinterpret_s16_q16(
                    div_n_q16(this.q16Yend, 2) 
                +   reinterpret_q16_f32(0.9f)
                );
            tEndPointFix.iWidth = reinterpret_s16_q16(
                    abs_q16(div_n_q16(mul_q16(this.q16K, this.q16Yend), 2)) 
                +   reinterpret_q16_f32(0.9f)
                );

        }
    }
    
    

    /* clip the line with the given region */
    do {
        arm_2d_region_t tTargetRegion = {0};
        if (NULL == ptRegion) {
            tTargetRegion.tSize = ptTarget->tRegion.tSize;
        } else {
            tTargetRegion = *ptRegion;
        }

        /* clip Y axis */
        this.tParams.tStart.iY = MAX(this.tParams.tStart.iY, tTargetRegion.tLocation.iY);
        this.tParams.tEnd.iY = MIN(this.tParams.tEnd.iY, (tTargetRegion.tLocation.iY + tTargetRegion.tSize.iHeight - 1));

        /* clip X axis */
        if (fK > 0) {
            this.tParams.tStart.iX = MAX(this.tParams.tStart.iX, tTargetRegion.tLocation.iX);
            this.tParams.tEnd.iX = MIN(this.tParams.tEnd.iX, (tTargetRegion.tLocation.iX + tTargetRegion.tSize.iWidth - 1));
        } else {
            this.tParams.tEnd.iX = MAX(this.tParams.tEnd.iX, tTargetRegion.tLocation.iX);
            this.tParams.tStart.iX = MIN(this.tParams.tStart.iX, (tTargetRegion.tLocation.iX + tTargetRegion.tSize.iWidth - 1));
        }

        /* calculate compensation for drawing terminals*/
        do {
            arm_2d_region_t tStartRegion = {
                .tSize = {1,1},
                .tLocation = this.tParams.tStart,
            };

            arm_2d_region_t tEndRegion = {
                .tSize = {1,1},
                .tLocation = this.tParams.tEnd,
            };

            if (!this.tParams.bHorizontallyChoppedEndpoints) {
                if (this.tParams.tStart.iX < this.tParams.tEnd.iX) {
                    tStartRegion.tLocation.iX -= tEndPointFix.iWidth;
                    tStartRegion.tLocation.iY -= tEndPointFix.iHeight;

                    tEndRegion.tLocation.iX += tEndPointFix.iWidth;
                    tEndRegion.tLocation.iY += tEndPointFix.iHeight;
                } else {
                    tEndRegion.tLocation.iX -= tEndPointFix.iWidth;
                    tEndRegion.tLocation.iY += tEndPointFix.iHeight;

                    tStartRegion.tLocation.iX += tEndPointFix.iWidth;
                    tStartRegion.tLocation.iY -= tEndPointFix.iHeight;
                }
            }

            arm_2d_region_get_minimal_enclosure(&tStartRegion, &tEndRegion, &this.tDrawRegion);
            OPCODE.Target.ptRegion = &this.tDrawRegion;
        } while(0);

    } while(0);

    return ARM_2D_ERR_NONE;
}


/*
 * the Frontend API
 */

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_user_draw_line(  
                            arm_2d_user_draw_line_descriptor_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_user_draw_line_api_params_t *ptParams,
                            uint8_t chOpacity)
{

    assert(NULL != ptTarget);
    assert(NULL != ptParams);

    ARM_2D_IMPL(arm_2d_user_draw_line_descriptor_t, ptOP);

    switch(arm_2d_target_tile_is_new_frame(ptTarget)) {
        case ARM_2D_RT_FALSE:
            
            break;
        case ARM_2D_RT_TRUE:
            do {
                if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
                    return (arm_fsm_rt_t)ARM_2D_ERR_BUSY;
                }

                arm_2d_err_t ret = arm_2dp_rgb565_user_draw_line_prepare(   ptThis, 
                                                                            ptTarget, 
                                                                            ptRegion, 
                                                                            ptParams);

                if (ARM_2D_ERR_NONE != ret) {
                    return (arm_fsm_rt_t)ret;
                }

                this.chOpacity = chOpacity;

            } while(0);
            break;
        case ARM_2D_ERR_INVALID_PARAM:
        default:
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    OP_CORE.ptOp = &ARM_2D_OP_USER_DRAW_LINE;
    OPCODE.Target.ptTile = ptTarget;

    /* this is updated in prepare function */
    //OPCODE.Target.ptRegion = ptRegion;
    this.tParams = *ptParams;
    

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_rgb565_sw_user_draw_line( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_user_draw_line_descriptor_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    arm_2d_region_t tTargetRegion = {0};
    
    if (NULL == ((arm_2d_op_t *)ptThis)->Target.ptRegion) {
        tTargetRegion.tSize = ((arm_2d_op_t *)ptThis)->Target.ptTile->tRegion.tSize;
    } else {
        tTargetRegion = *(((arm_2d_op_t *)ptThis)->Target.ptRegion);
    }

    tTargetRegion.tLocation 
        = arm_2d_get_absolute_location( ((arm_2d_op_t *)ptThis)->Target.ptTile,
                                        tTargetRegion.tLocation,
                                        true);

    
    __arm_2d_impl_rgb565_user_draw_line(ptThis,
                                        ptTask->Param.tTileProcess.pBuffer,
                                        ptTask->Param.tTileProcess.iStride,
                                        &(ptTask->Param.tTileProcess.tValidRegionInVirtualScreen),
                                        &tTargetRegion);


    return arm_fsm_rt_cpl;
}


__STATIC_INLINE void __fill_line_stride(int16_t iXStart,
                                        int16_t iYStart,
                                        int16_t iScanY,
                                        q16_t q16StrideStartX, 
                                        q16_t q16KLeftWing,
                                        q16_t q16KRightWing,
                                        uint16_t *phwTargetStride,
                                        arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                                        int32_t q8LOrigin,
                                        uint8_t chOpacity,
                                        uint16_t hwColour)
{

    if (q8LOrigin <= 0) {
        return ;
    }

    /* the first point of the stride */
    arm_2d_location_t tPoint = {
        .iY = iScanY + iYStart,
        .iX = reinterpret_s16_q16(q16StrideStartX),
    };
    int16_t iXOffset = tPoint.iX - iXStart;
    bool bDrawWings = false;

    /* handle the left wing */
    do {
        int32_t q8AlphaLeft = MIN(256, q8LOrigin);
        arm_2d_location_t tLeftWingPoint = tPoint;
        int16_t iLeftWingXOffset = iXOffset;

        int_fast16_t n = 1;
        do {
            q16_t q16DeltaY = mul_n_q16(q16KLeftWing, n);
            int32_t hwAlpha = q8AlphaLeft - (q16DeltaY >> 8);
            if (hwAlpha <= 0) {
                /* finished */
                break;
            }

            if (arm_2d_is_point_inside_region(ptValidRegionOnVirtualScreen, &tLeftWingPoint)) {
                uint16_t hwTrans =  256 - (hwAlpha * chOpacity >> 8);
                __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, &phwTargetStride[iLeftWingXOffset], hwTrans);
            }

            iLeftWingXOffset--;
            tLeftWingPoint.iX--;
            n++;
        } while(true);

        if (n > 1) {
            bDrawWings = true;
        }
    } while(0);


    /* handle the solid body */
    do {
        uint16_t hwAlpha = (256 - ((q16StrideStartX >> 8) & 0xFF));
        int32_t q8L = q8LOrigin;
        hwAlpha = MIN(hwAlpha, q8L);

        bool bFoundPointsInsideRegion = false;
        while(q8L) {

            if (arm_2d_is_point_inside_region(ptValidRegionOnVirtualScreen, &tPoint)) {
                uint16_t hwTrans =  256 - (hwAlpha * chOpacity >> 8);
                __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, &phwTargetStride[iXOffset], hwTrans);
                bFoundPointsInsideRegion = true;
            } else if (bFoundPointsInsideRegion) {
                break;
            }

            tPoint.iX++;
            iXOffset++;

            q8L -= hwAlpha; /* subtract previous alpha */
            if (q8L >= 256){
                hwAlpha = 256;
            } else {
                hwAlpha = q8L;
            }
        }
    } while(0);

    /* handle the right wing */
    do {
        int32_t q8AlphaRight = MIN(256, q8LOrigin);
        arm_2d_location_t tRightWingPoint = tPoint;
        tRightWingPoint.iX--;
        int16_t iRightWingXOffset = iXOffset - 1;

        int_fast16_t n = 1;
        do {
            q16_t q16DeltaY = mul_n_q16(q16KRightWing, n);
            int32_t hwAlpha = q8AlphaRight - (q16DeltaY >> 8);
            if (hwAlpha <= 0) {
                /* finished */
                break;
            }

            if (arm_2d_is_point_inside_region(ptValidRegionOnVirtualScreen, &tRightWingPoint)) {
                uint16_t hwTrans =  256 - (hwAlpha * chOpacity >> 8);
                __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, &phwTargetStride[iRightWingXOffset], hwTrans);
            }

            iRightWingXOffset++;
            tRightWingPoint.iX++;
            n++;
        } while(true);
    } while(0);
}

#include "arm_2d_helper.h"

/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_user_draw_line(
                                    arm_2d_user_draw_line_descriptor_t *ptThis,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                                    arm_2d_region_t *ptTargetRegionOnVirtualScreen)
{
    uint8_t chOpacity = this.chOpacity;
    uint16_t *__RESTRICT phwTarget = phwTargetBase;
    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };
    ARM_2D_UNUSED(tOffset);
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

    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;

    arm_2d_location_t tStart = 
        arm_2d_get_absolute_location(   this.use_as__arm_2d_op_t.Target.ptTile,
                                        this.tParams.tStart,
                                        true);
    
    arm_2d_location_t tEnd = 
        arm_2d_get_absolute_location(   this.use_as__arm_2d_op_t.Target.ptTile,
                                        this.tParams.tEnd,
                                        true);

    /* iXStart is used to calculate the pixel index in stride, no need to update */
    int16_t iXStart = ptValidRegionOnVirtualScreen->tLocation.iX;

    /* we know the line will always inside the target region
     * iYStart is the starting point for the horizontal scanning 
     */
    int16_t iYStart = ptValidRegionOnVirtualScreen->tLocation.iY;
    
    int16_t iNormalBodyYEnd = 0, iNormalBodyYStart = 0;
    int16_t iYEnd = 0;

    if (!this.tParams.bHorizontallyChoppedEndpoints) {
        /* chop the start terminal */
        do {
            iNormalBodyYStart = tStart.iY + reinterpret_s16_q16(div_n_q16(this.q16Yend, 2) + 0x8000);

            if (iYStart >= iNormalBodyYStart) {
                break;
            }
            int16_t iYStartEndPoint = tStart.iY - reinterpret_s16_q16(div_n_q16(this.q16Yend, 2) + 0x8000);
            if (iYStartEndPoint > iYStart) {
                int16_t iYOffset = iYStartEndPoint - iYStart;
                iHeight -= iYOffset;
                iYStart = iYStartEndPoint;

                if (iHeight <= 0) {
                    break;
                }

                phwTarget += iYOffset * iTargetStride;
            }

            int16_t iStartEndpointHeight = iNormalBodyYStart - iYStart - 1;

            /* draw the start endpoint*/
            do {
                q16_t q16XStart = mul_n_q16(this.q16dX, (iYStart - tStart.iY)) + reinterpret_q16_s16(tStart.iX);
                q16_t q16XOffset = div_n_q16(this.q16L, 2);

                uint16_t hwColour = this.tParams.hwColour;

                int16_t iYOffset = iYStart - iYStartEndPoint;

                for (int_fast16_t iY = 0; iY < iStartEndpointHeight; iY++) {

                    q16_t q16Stride = mul_q16(reinterpret_q16_s16(iY + iYOffset), this.q16abs_Kp_1divK_);
                
                    int32_t q8StrideTotalAlpha = q16Stride >> 8;
                    q8StrideTotalAlpha = MAX(q8StrideTotalAlpha, 128);

                    q16_t q16StrideStartX = q16XStart + q16XOffset - q16Stride;
                #if 1
                    __fill_line_stride( iXStart, 
                                        iYStart, 
                                        iY, 
                                        q16StrideStartX, 
                                        this.q161divK,
                                        this.q16K,
                                        phwTarget, 
                                        ptValidRegionOnVirtualScreen, 
                                        q8StrideTotalAlpha, 
                                        chOpacity, 
                                        hwColour);
                #endif
                    q16XStart += this.q16dX;
                    phwTarget += iTargetStride;
                }
            } while(0);

            /* update height */
            iHeight -= iStartEndpointHeight;

            iYStart = iNormalBodyYStart;
        } while(0);

        /* chop the end terminal */
        do {
            iNormalBodyYEnd = tEnd.iY - reinterpret_s16_q16(div_n_q16(this.q16Yend, 2) + 0x8000);
            iYEnd = iYStart + iHeight - 1;
            if (iYEnd > iNormalBodyYEnd) {
            #if 0
                int16_t iStopEndpointHeight = iYEnd - iNormalBodyYEnd;
                /* update height */
                iHeight -= iStopEndpointHeight;
            #else
                iHeight = tEnd.iY - tStart.iY + 1 - reinterpret_s16_q16(this.q16Yend + 0x8000);
            #endif
            }
        } while(0);
    }


    /* draw the normal part*/
    if (iHeight > 0) {
        q16_t q16XStart = mul_n_q16(this.q16dX, (iYStart - tStart.iY)) + reinterpret_q16_s16(tStart.iX);
        q16_t q16XOffset = div_n_q16(this.q16L - this.q161PixelL, 2) - this.q16PixelCompensate;

        int32_t q8StrideTotalAlpha = (this.q16L -(this.q161PixelL + this.q16PixelCompensate)) >> 8;
        q8StrideTotalAlpha = MAX(q8StrideTotalAlpha, 255);

        uint16_t hwColour = this.tParams.hwColour;


        for (int_fast16_t iY = 0; iY < iHeight; iY++) {

            q16_t q16StrideStartX = q16XStart - q16XOffset;
            __fill_line_stride( iXStart, 
                                iYStart, 
                                iY, 
                                q16StrideStartX,
                                this.q16K,
                                this.q16K,
                                phwTarget, 
                                ptValidRegionOnVirtualScreen, 
                                q8StrideTotalAlpha,
                                chOpacity, 
                                hwColour);
            q16XStart += this.q16dX;
            phwTarget += iTargetStride;
        }

        iYStart += iHeight;
    }

    if (!this.tParams.bHorizontallyChoppedEndpoints) {
        /* draw the stop endpoint*/
        if (iYEnd > iNormalBodyYEnd) {
            q16_t q16XStart = mul_n_q16(this.q16dX, (iYStart - tStart.iY)) + reinterpret_q16_s16(tStart.iX);
            q16_t q16XOffset = div_n_q16(this.q16L - this.q161PixelL, 2) - this.q16PixelCompensate;
            
            uint16_t hwColour = this.tParams.hwColour;

            int16_t iYOffset = iYStart - iNormalBodyYEnd;
            int16_t iStopEndpointHeight = iYEnd - iNormalBodyYEnd + 1;
            iStopEndpointHeight = MIN(iStopEndpointHeight, ptValidRegionOnVirtualScreen->tSize.iHeight);

            for (int_fast16_t iY = 0; iY < iStopEndpointHeight; iY++) {
                
                q16_t q16Stride = mul_q16( (this.q16Yend - reinterpret_q16_s16(iY + iYOffset)), this.q16abs_Kp_1divK_);
                int32_t q8StrideTotalAlpha = (q16Stride - this.q161PixelL) >> 8;
                //q8StrideTotalAlpha = MAX(q8StrideTotalAlpha, 128);

                q16_t q16StrideStartX = q16XStart - q16XOffset;

                __fill_line_stride( iXStart, 
                                    iYStart, 
                                    iY, 
                                    q16StrideStartX,
                                    this.q16K,
                                    this.q161divK,
                                    phwTarget, 
                                    ptValidRegionOnVirtualScreen, 
                                    q8StrideTotalAlpha,
                                    chOpacity, 
                                    hwColour);

                q16XStart += this.q16dX;
                phwTarget += iTargetStride;
            }
        }
    }

}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_USER_DRAW_LINE_RGB565,
                __arm_2d_rgb565_sw_user_draw_line);      /* Default SW Implementation */


/*
 * OPCODE
 */
const __arm_2d_op_info_t ARM_2D_OP_USER_DRAW_LINE = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_USER_DRAW_LINE,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_USER_DRAW_LINE_RGB565),
        },
    },
};



#ifdef   __cplusplus
}
#endif

