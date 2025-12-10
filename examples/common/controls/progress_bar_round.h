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

#ifndef __PROGRESS_BAR_ROUND_H__
#define __PROGRESS_BAR_ROUND_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__common.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/*============================ MACROS ========================================*/

/* OOC header, please DO NOT modify  */
#ifdef __PROGRESS_BAR_ROUND_IMPLEMENT__
#   undef   __PROGRESS_BAR_ROUND_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__PROGRESS_BAR_ROUND_INHERIT__)
#   undef   __PROGRESS_BAR_ROUND_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


typedef struct progress_bar_round_cfg_t {
    arm_2d_scene_t *ptScene;

    struct {
        int16_t iMin;
        int16_t iMax;
    } ValueRange;

    const arm_2d_tile_t *ptCircleMask;

} progress_bar_round_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct progress_bar_round_t progress_bar_round_t;

struct progress_bar_round_t {

ARM_PRIVATE(
    progress_bar_round_cfg_t tCFG;

    arm_2d_helper_dirty_region_item_t tDirtyRegionItem;
    q16_t q16Ratio;
    int16_t iProgress;
    bool bFullyRedraw;

)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void progress_bar_round_init( progress_bar_round_t *ptThis,
                          progress_bar_round_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void progress_bar_round_depose( progress_bar_round_t *ptThis);

extern
ARM_NONNULL(1)
void progress_bar_round_on_load( progress_bar_round_t *ptThis);

extern
ARM_NONNULL(1)
void progress_bar_round_on_frame_start( progress_bar_round_t *ptThis);

extern
ARM_NONNULL(1)
void progress_bar_round_on_frame_complete( progress_bar_round_t *ptThis);

extern
ARM_NONNULL(1)
void progress_bar_round_set_circle_mask(progress_bar_round_t *ptThis, 
                                        const arm_2d_tile_t *ptCircleMask);
extern
ARM_NONNULL(1, 2)
arm_2d_region_t progress_bar_round_show(   
                                progress_bar_round_t *ptThis,
                                const arm_2d_tile_t *ptTile, 
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tBackgroundColour,
                                COLOUR_INT tBarColour,
                                int16_t iProgress,
                                uint8_t chOpacity);

extern
ARM_NONNULL(1, 2)
arm_2d_region_t  progress_bar_round_show2(   
                                progress_bar_round_t *ptThis,
                                const arm_2d_tile_t *ptTile, 
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tBackgroundColour,
                                COLOUR_INT tBarColour,
                                int16_t iProgress,
                                uint8_t chOpacity);

extern
ARM_NONNULL(1, 2)
arm_2d_region_t  progress_bar_round_show3(   
                                progress_bar_round_t *ptThis,
                                const arm_2d_tile_t *ptTile, 
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tBackgroundColour,
                                COLOUR_INT tBarColour,
                                int16_t iProgress,
                                uint8_t chOpacity);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
