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

#include "arm_2d_user_opcode_<user opcode template>.h"
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

enum {
    __ARM_2D_OP_IDX_USER_<USER OPCODE TEMPLATE> = __ARM_2D_OP_IDX_USER_OP_START,

};


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern
void __arm_2d_impl_rgb565_user_<user opcode template>(
                            arm_2d_user_<user opcode template>_descriptor_t *ptThis,
                            uint16_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                            arm_2d_region_t *ptTargetRegionOnVirtualScreen);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static
arm_2d_err_t arm_2dp_rgb565_user_<user opcode template>_prepare(
                            arm_2d_user_<user opcode template>_descriptor_t *ptThis,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_user_<user opcode template>_api_params_t *ptParams)
{
    assert(NULL != ptThis);
    assert(NULL != ptParams);

    /* some preparation work */

    return ARM_2D_ERR_NONE;
}


/*
 * the Frontend API
 */

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_user_<user opcode template>(  
                            arm_2d_user_<user opcode template>_descriptor_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_user_<user opcode template>_api_params_t *ptParams,
                            arm_2d_color_rgb565_t tColour,
                            uint8_t chOpacity)
{

    assert(NULL != ptTarget);
    assert(NULL != ptParams);

    ARM_2D_IMPL(arm_2d_user_<user opcode template>_descriptor_t, ptOP);

    switch(arm_2d_target_tile_is_new_frame(ptTarget)) {
        case ARM_2D_RT_FALSE:
            
            break;
        case ARM_2D_RT_TRUE:
            do {
                if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
                    return (arm_fsm_rt_t)ARM_2D_ERR_BUSY;
                }

                arm_2d_err_t ret = arm_2dp_rgb565_user_<user opcode template>_prepare(   ptThis, 
                                                                            ptTarget, 
                                                                            ptRegion, 
                                                                            ptParams);

                if (ARM_2D_ERR_NONE != ret) {
                    return (arm_fsm_rt_t)ret;
                }

                /* update parameters */
                this.chOpacity = chOpacity;
                this.tForeground.hwColour = tColour.tValue;

            } while(0);
            break;
        case ARM_2D_ERR_INVALID_PARAM:
        default:
            return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    OP_CORE.ptOp = &ARM_2D_OP_USER_<USER OPCODE TEMPLATE>;
    OPCODE.Target.ptTile = ptTarget;

    /* this is updated in prepare function */
    this.tParams = *ptParams;
    
    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/*
 * The backend entry
 */
arm_fsm_rt_t __arm_2d_rgb565_sw_user_<user opcode template>( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_user_<user opcode template>_descriptor_t, ptTask->ptOP);

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

    __arm_2d_impl_rgb565_user_<user opcode template>(ptThis,
                                        ptTask->Param.tTileProcess.pBuffer,
                                        ptTask->Param.tTileProcess.iStride,
                                        &(ptTask->Param.tTileProcess.tValidRegionInVirtualScreen),
                                        &tTargetRegion);


    return arm_fsm_rt_cpl;
}


/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_user_<user opcode template>(
                                    arm_2d_user_<user opcode template>_descriptor_t *ptThis,
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

    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;

    for (int_fast16_t iY = 0; iY < iHeight; iY++) {

        uint32_t *pwTargetLine = pwTarget;

        for (int_fast16_t iX = 0; iX < iWidth; iX++) {

            /* a simple example */
            __ARM_2D_PIXEL_BLENDING_OPA_RGB565(&hwColour, pwTargetLine++, chOpacity);

        }

        pwTarget += iTargetStride;
    }

}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_USER_<USER OPCODE TEMPLATE>_RGB565,
                __arm_2d_rgb565_sw_user_<user opcode template>);      /* Default SW Implementation */


/*
 * OPCODE
 */
const __arm_2d_op_info_t ARM_2D_OP_USER_<USER OPCODE TEMPLATE> = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_USER_<USER OPCODE TEMPLATE>,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_USER_<USER OPCODE TEMPLATE>_RGB565),
        },
    },
};



#ifdef   __cplusplus
}
#endif
