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
#include "__arm_2d_example_opcodes_common.h"

#include "arm_2d_user_opcode_draw_line.h"
#include "arm_2d_helper.h"

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

    q16_t q16DeltaX = reinterpret_q16_s16( this.tParams.tEnd.iX - this.tParams.tStart.iX);
    q16_t q16DeltaY = reinterpret_q16_s16( this.tParams.tEnd.iY - this.tParams.tStart.iY);


    float fK;

    bool bHorizontalLine = false;
    bool bVerticalLine = false;

    if (0 == q16DeltaY) {
        bHorizontalLine = true;
    }
    if (0 == q16DeltaX) {
        bVerticalLine = true;
    }

    if (bHorizontalLine && bVerticalLine ) {
        return ARM_2D_ERR_INVALID_PARAM;
    } else if (bHorizontalLine) {
        fK = 0.0f;
        this.q16K = 0;
        this.bUseYAdvance = false;
        //this.q16dX = __INT32_MAX__;
    } else if (bVerticalLine) {
        //this.q16K = __INT32_MAX__;
        fK = (float)__INT32_MAX__;
        this.q16dX = 0;
        this.bUseYAdvance = true;
    } else {
        fK = (float)q16DeltaY / (float)q16DeltaX;

        if (abs_q16(fK) >= reinterpret_q16_f32(1)) {
            this.bUseYAdvance = true;
            this.q16dX = reinterpret_q16_f32(1.0f / fK);
        } else {
            this.bUseYAdvance = false;
            this.q16K = reinterpret_q16_f32(fK);
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
                            arm_2d_color_rgb565_t tColour,
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
                this.hwColour = tColour.tValue;

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


__STATIC_INLINE 
void __draw_point(int16_t iXOffset, uint16_t *phwTarget, uint16_t hwColour, uint8_t chOpacity)
{
    uint16_t hwTransparency = 256 - chOpacity;
    hwTransparency -= (hwTransparency == 1);

    __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, &phwTarget[iXOffset], hwTransparency);
}


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
#if 0
    arm_2d_location_t tEnd = 
        arm_2d_get_absolute_location(   this.use_as__arm_2d_op_t.Target.ptTile,
                                        this.tParams.tEnd,
                                        true);
#endif

    /* iXStart is used to calculate the pixel index in stride, no need to update */
    int16_t iXStart = ptValidRegionOnVirtualScreen->tLocation.iX;

    /* we know the line will always inside the target region
     * iYStart is the starting point for the horizontal scanning 
     */
    int16_t iYStart = ptValidRegionOnVirtualScreen->tLocation.iY;
    uint16_t hwColour = this.hwColour;

    if (this.bUseYAdvance) {
        q16_t q16XStart = mul_n_q16(this.q161divK, (iYStart - tStart.iY)) + reinterpret_q16_s16(tStart.iX);
        arm_2d_location_t tDrawPoint = {
            .iY = iYStart,
        };

        for (int_fast16_t iY = 0; iY < iHeight; iY++) {

            /* The Left Point */

            tDrawPoint.iX = reinterpret_s16_q16(q16XStart);
            uint8_t u8Offset = (q16XStart & 0xFF00) >> 8;

            if (arm_2d_is_point_inside_region(ptValidRegionOnVirtualScreen, &tDrawPoint)) {
                uint8_t chPointOpacity = arm_2d_helper_alpha_mix(0xFF - u8Offset, chOpacity);
                __draw_point(tDrawPoint.iX - iXStart, phwTarget, hwColour, chPointOpacity);
            }

            /* The right Point */
            tDrawPoint.iX++;
            if (arm_2d_is_point_inside_region(ptValidRegionOnVirtualScreen, &tDrawPoint)) {
                
                uint8_t chPointOpacity = arm_2d_helper_alpha_mix(u8Offset, chOpacity);
                __draw_point(tDrawPoint.iX - iXStart, phwTarget, hwColour, chPointOpacity);
            }

            tDrawPoint.iY++;

            q16XStart += this.q16dX;
            phwTarget += iTargetStride;
        }
    } else {

        q16_t q16YStart = mul_n_q16(this.q16K, (iXStart - tStart.iX)) + reinterpret_q16_s16(tStart.iY);
        arm_2d_location_t tDrawPoint = {
            .iX = iXStart,
        };

        for (int_fast16_t iX = 0; iX < iWidth; iX++) {

            /* The Left Point */

            tDrawPoint.iY = reinterpret_s16_q16(q16YStart);
            uint8_t u8Offset = (q16YStart & 0xFF00) >> 8;

            if (arm_2d_is_point_inside_region(ptValidRegionOnVirtualScreen, &tDrawPoint)) {
                uint8_t chPointOpacity = arm_2d_helper_alpha_mix(0xFF - u8Offset, chOpacity);

                
                __draw_point(   0, 
                                phwTarget + (tDrawPoint.iY - iYStart) * iTargetStride, 
                                hwColour, 
                                chPointOpacity);
            }

            /* The right Point */
            tDrawPoint.iY++;
            if (arm_2d_is_point_inside_region(ptValidRegionOnVirtualScreen, &tDrawPoint)) {
                
                uint8_t chPointOpacity = arm_2d_helper_alpha_mix(u8Offset, chOpacity);

                __draw_point(   0, 
                                phwTarget + (tDrawPoint.iY - iYStart) * iTargetStride, 
                                hwColour, 
                                chPointOpacity);
            }

            tDrawPoint.iX++;

            q16YStart += this.q16dY;
            phwTarget ++;
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

