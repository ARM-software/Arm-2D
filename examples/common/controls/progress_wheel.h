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

#if 0
#define __progress_wheel_init0(__this_ptr, __diameter, __colour)                \
            do {                                                                \
                progress_wheel_cfg_t tCFG = {                                   \
                    .iWheelDiameter = (__diameter),                             \
                    .tWheelColour = (COLOUR_INT)(__colour),                     \
                };                                                              \
                __progress_wheel_init((__this_ptr), &tCFG);                     \
            } while(0)

#define __progress_wheel_init1(__this_ptr, __diameter, __colour, __dot_colour)  \
            do {                                                                \
                progress_wheel_cfg_t tCFG = {                                   \
                    .iWheelDiameter = (__diameter),                             \
                    .tWheelColour = (COLOUR_INT)(__colour),                     \
                    .tDotColour = (COLOUR_INT)(__dot_colour),                   \
                };                                                              \
                __progress_wheel_init((__this_ptr), &tCFG);                     \
            } while(0)

#define __progress_wheel_init2( __this_ptr,                                     \
                                __diameter,                                     \
                                __colour,                                       \
                                __dot_colour,                                   \
                                __dirty_region_list_pptr)                       \
            do {                                                                \
                progress_wheel_cfg_t tCFG = {                                   \
                    .iWheelDiameter = (__diameter),                             \
                    .tWheelColour = (COLOUR_INT)(__colour),                     \
                    .tDotColour = (COLOUR_INT)(__dot_colour),                   \
                    .ppList = (__dirty_region_list_pptr),                       \
                };                                                              \
                __progress_wheel_init((__this_ptr), &tCFG);                     \
            } while(0)

#define __progress_wheel_init3( __this_ptr,                                     \
                                __diameter,                                     \
                                __colour,                                       \
                                __dot_colour,                                   \
                                __arc_mask_ptr,                                 \
                                __dirty_region_list_pptr)                       \
            do {                                                                \
                progress_wheel_cfg_t tCFG = {                                   \
                    .ptileArcMask = (__arc_mask_ptr),                           \
                    .iWheelDiameter = (__diameter),                             \
                    .tWheelColour = (COLOUR_INT)(__colour),                     \
                    .tDotColour = (COLOUR_INT)(__dot_colour),                   \
                    .ppList = (__dirty_region_list_pptr),                       \
                };                                                              \
                __progress_wheel_init((__this_ptr), &tCFG);                     \
            } while(0)

#define __progress_wheel_init4( __this_ptr,                                     \
                                __diameter,                                     \
                                __colour,                                       \
                                __dot_colour,                                   \
                                __arc_mask_ptr,                                 \
                                __dot_mask_ptr,                                 \
                                __dirty_region_list_pptr)                       \
            do {                                                                \
                progress_wheel_cfg_t tCFG = {                                   \
                    .ptileArcMask = (__arc_mask_ptr),                           \
                    .ptileDotMask = (__dot_mask_ptr),                           \
                    .iWheelDiameter = (__diameter),                             \
                    .tWheelColour = (COLOUR_INT)(__colour),                     \
                    .tDotColour = (COLOUR_INT)(__dot_colour),                   \
                    .ppList = (__dirty_region_list_pptr),                       \
                };                                                              \
                __progress_wheel_init((__this_ptr), &tCFG);                     \
            } while(0)

#define progress_wheel_init(__this_ptr, __diameter, __colour, ...)              \
            ARM_CONNECT2(   __progress_wheel_init,                              \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))((__this_ptr),       \
                                                            (__diameter),       \
                                                            (__colour),         \
                                                            ##__VA_ARGS__)
#endif

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

    arm_2d_helper_transform_t tTransHelper;
    
    arm_2d_region_list_item_t tDirtyRegion;
    uint8_t chState;
    int8_t  chLastQuadrant;
    int16_t iLastProgress;
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
void progress_wheel_depose(progress_wheel_t *ptThis);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif



#endif
