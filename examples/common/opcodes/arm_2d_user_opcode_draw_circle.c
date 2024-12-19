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

#include "arm_2d_user_opcode_draw_circle.h"
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
void __arm_2d_impl_rgb565_user_draw_circle(
                            arm_2d_user_draw_circle_descriptor_t *ptThis,
                            uint16_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


/*
 * the Frontend API
 */

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_user_draw_circle(  
                            arm_2d_user_draw_circle_descriptor_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_user_draw_circle_api_params_t *ptParams,
                            arm_2d_color_rgb565_t tColour,
                            uint8_t chOpacity)
{

    assert(NULL != ptTarget);
    assert(NULL != ptParams);

    ARM_2D_IMPL(arm_2d_user_draw_circle_descriptor_t, ptOP);

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_USER_DRAW_CIRCLE;
    OPCODE.Target.ptTile = ptTarget;
    OPCODE.Target.ptRegion = ptRegion;

    /* this is updated in prepare function */
    this.tParams = *ptParams;
    this.chOpacity = chOpacity;
    this.tForeground.hwColour = tColour.tValue;

    arm_2d_region_t tTargetRegion = {0};
    if (NULL == ptRegion) {
        tTargetRegion.tSize = ptTarget->tRegion.tSize;
        ptRegion = &tTargetRegion;
    }

    /* calculate the pivot */
    do {
        if (this.tParams.ptPivot) {
            this.tPivot = *this.tParams.ptPivot;
        } else {
            this.tPivot.iX = ptRegion->tLocation.iX + (ptRegion->tSize.iWidth >> 1);
            this.tPivot.iY = ptRegion->tLocation.iY + (ptRegion->tSize.iHeight >> 1);
        }

        this.tDrawRegion.tSize.iHeight = this.tParams.iRadius * 2 + 2;
        this.tDrawRegion.tSize.iWidth = this.tDrawRegion.tSize.iHeight + 2;

        this.tDrawRegion.tLocation.iX = this.tPivot.iX - this.tParams.iRadius - 1;
        this.tDrawRegion.tLocation.iY = this.tPivot.iY - this.tParams.iRadius - 1;

        if (!arm_2d_region_intersect(ptRegion, &this.tDrawRegion, &this.tDrawRegion)) {
            /* nothing to draw */
            return __arm_2d_op_depose((arm_2d_op_core_t *)ptThis, arm_fsm_rt_cpl);
        }

        OPCODE.Target.ptRegion = &this.tDrawRegion;

        this.tPivot = arm_2d_get_absolute_location(ptTarget, this.tPivot, true );
    } while(0);
    
    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_rgb565_sw_user_draw_circle( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_user_draw_circle_descriptor_t, ptTask->ptOP);

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

    __arm_2d_impl_rgb565_user_draw_circle(ptThis,
                                        ptTask->Param.tTileProcess.pBuffer,
                                        ptTask->Param.tTileProcess.iStride,
                                        &(ptTask->Param.tTileProcess.tValidRegionInVirtualScreen),
                                        &tTargetRegion);


    return arm_fsm_rt_cpl;
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_user_draw_circle(
                                    arm_2d_user_draw_circle_descriptor_t *ptThis,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                                    arm_2d_region_t *ptTargetRegionOnVirtualScreen)
{
    uint8_t chOpacity = this.chOpacity;
    uint16_t *__RESTRICT phwTarget = phwTargetBase;
    uint16_t hwColour = this.tForeground.hwColour;

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

    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth + ptValidRegionOnVirtualScreen->tLocation.iX;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight + ptValidRegionOnVirtualScreen->tLocation.iY;

    uint32_t wRadius2 = (uint32_t)this.tParams.iRadius * (uint32_t)this.tParams.iRadius;
    uint32_t wRadiusBorder2 = (uint32_t)(this.tParams.iRadius + 1) * (uint32_t)(this.tParams.iRadius + 1);
    q16_t q16Radius = reinterpret_q16_s16(this.tParams.iRadius);

    for (int_fast16_t iY = ptValidRegionOnVirtualScreen->tLocation.iY; iY < iHeight; iY++) {

        uint16_t *phwTargetLine = phwTarget;
        int16_t iYOffset = iY - this.tPivot.iY;
        iYOffset = ABS(iYOffset);

        for (int_fast16_t iX = ptValidRegionOnVirtualScreen->tLocation.iX; iX < iWidth; iX++) {

            int16_t iXOffset = iX - this.tPivot.iX;
            iXOffset = ABS(iXOffset);
            
            /* calculate the distance */
            uint32_t wDistance2 = (uint32_t)iXOffset * (uint32_t)iXOffset + (uint32_t)iYOffset * (uint32_t)iYOffset;
            
            if (wDistance2 >= wRadiusBorder2) {
                phwTargetLine++;
                continue;
            } else if (wDistance2 <= wRadius2) {
                /* fill colour with opacity */
                __ARM_2D_PIXEL_BLENDING_OPA_RGB565(&hwColour, phwTargetLine++, chOpacity);
                continue;
            } else if (!this.tParams.bAntiAlias) {
                phwTargetLine++;
                continue;
            }

            /* anti alias */
            float fDistance;
            arm_sqrt_f32((float)wDistance2, &fDistance);
            q16_t q16Fraction = reinterpret_q16_f32(fDistance);

            /* get the residual */
            q16Fraction -= q16Radius;

            uint16_t hwOpacity = (q16Fraction & 0xFF00) >> 8;
            uint8_t chPointOpacity = arm_2d_helper_alpha_mix(0xFF - hwOpacity, chOpacity);
            
            __ARM_2D_PIXEL_BLENDING_OPA_RGB565(&hwColour, phwTargetLine++, chPointOpacity);
        }

        phwTarget += iTargetStride;
    }

}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_USER_DRAW_CIRCLE_RGB565,
                __arm_2d_rgb565_sw_user_draw_circle);      /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t ARM_2D_OP_USER_DRAW_CIRCLE = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_USER_DRAW_CIRCLE,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_USER_DRAW_CIRCLE_RGB565),
        },
    },
};



#ifdef   __cplusplus
}
#endif

