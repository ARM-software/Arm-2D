/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
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

#ifndef __PROGRESS_WHEEL_H__
#define __PROGRESS_WHEEL_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "__common.h"


#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/* OOC header, please DO NOT modify  */
#ifdef __PROGRESS_WHEEL_IMPLEMENT__
#   undef   __PROGRESS_WHEEL_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__PROGRESS_WHEEL_INHERIT__)
#   undef   __PROGRESS_WHEEL_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct progress_wheel_cfg_t {
    const arm_2d_tile_t *ptileArcMask;
    const arm_2d_tile_t *ptileDotMask;
    int16_t iWheelDiameter;
    COLOUR_INT tWheelColour;
    COLOUR_INT tDotColour;
    bool bUseDirtyRegions;
} progress_wheel_cfg_t;


typedef struct progress_wheel_t {

/* private members */
ARM_PRIVATE(
    progress_wheel_cfg_t tCFG;
    arm_2d_scene_t *ptTargetScene;

    float fScale;
    float fAngle;
    arm_2d_op_fill_cl_msk_opa_trans_t tOP[7];

    arm_2d_helper_dirty_region_transform_t tTransHelper;
    
    arm_2d_region_list_item_t tDirtyRegion;
    //arm_2d_region_t tLastCurveRegion;
    
    int8_t  chLastQuadrant;
    int16_t iLastProgress;
    int16_t iProgress;
    int16_t iNewProgress;
)

} progress_wheel_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void progress_wheel_init( progress_wheel_t *ptThis,
                          arm_2d_scene_t *ptTargetScene,
                          const progress_wheel_cfg_t *ptCFG);

extern
ARM_NONNULL(1)
void progress_wheel_set_diameter(progress_wheel_t *ptThis, 
                                int16_t iDiameter);

extern
ARM_NONNULL(1)
void progress_wheel_on_frame_start(progress_wheel_t *ptThis);

extern
ARM_NONNULL(1,2)
void progress_wheel_show(   progress_wheel_t *ptThis,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            int16_t iProgress,
                            uint8_t chOpacity,
                            bool bIsNewFrame);

extern
ARM_NONNULL(1)
void progress_wheel_on_load(progress_wheel_t *ptThis);

extern
ARM_NONNULL(1)
void progress_wheel_depose(progress_wheel_t *ptThis);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif



#endif
