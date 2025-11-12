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

#ifndef __FOLDABLE_PANEL_H__
#define __FOLDABLE_PANEL_H__

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
#ifdef __FOLDABLE_PANEL_IMPLEMENT__
#   undef   __FOLDABLE_PANEL_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__FOLDABLE_PANEL_INHERIT__)
#   undef   __FOLDABLE_PANEL_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    FOLDABLE_PANEL_STATUS_FOLDED,
    FOLDABLE_PANEL_STATUS_UNFOLDING,
    FOLDABLE_PANEL_STATUS_UNFOLDED,
    FOLDABLE_PANEL_STATUS_FOLDING
} foldable_panel_status_t;

typedef struct foldable_panel_cfg_t {
    arm_2d_scene_t *ptScene;

    uint32_t bShowScanLines                 : 1;
    uint32_t bAlignTimeline                 : 1;
    uint32_t bUseDirtyRegions               : 1;
    uint32_t                                : 5;
    uint32_t u12VerticalFoldingTimeInMS     : 12;
    uint32_t u12HorizontalFoldingTimeInMS   : 12;
    COLOUR_TYPE_T tLineColour;
} foldable_panel_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct foldable_panel_t foldable_panel_t;

struct foldable_panel_t {

ARM_PRIVATE(

    foldable_panel_cfg_t tCFG;

    int64_t lTimestamp[2];
    arm_2d_tile_t tInnerPanel;
    arm_2d_tile_t tUserPanel;
    arm_2d_size_t tOuterPanelSize;
    arm_2d_size_t tInnerPanelSize;

    arm_2d_region_t tDrawRegion;

    uint8_t bInnerPanelSizeChanged  : 1;
    uint8_t bOutPanelSizeChanged    : 1;
    uint8_t u8Status                : 2;
    uint8_t                         : 2;
    uint8_t bUnfoldRequest          : 1;
    uint8_t bFoldRequest            : 1;

    uint8_t chPT;

    struct {
        arm_2d_helper_dirty_region_item_t tDirtyRegionItems[4];
        arm_2d_size_t tLastInnerPanelSize;
    } DirtyRegion;

)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void foldable_panel_init( foldable_panel_t *ptThis,
                          foldable_panel_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void foldable_panel_depose( foldable_panel_t *ptThis);

extern
ARM_NONNULL(1)
void foldable_panel_on_load( foldable_panel_t *ptThis);

extern
ARM_NONNULL(1)
void foldable_panel_on_frame_start( foldable_panel_t *ptThis);

extern
ARM_NONNULL(1)
void foldable_panel_on_frame_complete( foldable_panel_t *ptThis);

extern
ARM_NONNULL(1)
arm_2d_tile_t * foldable_panel_show(foldable_panel_t *ptThis,
                                    const arm_2d_tile_t *ptTile, 
                                    const arm_2d_region_t *ptRegion, 
                                    bool bIsNewFrame);

extern
ARM_NONNULL(1)
foldable_panel_status_t foldable_panel_status(foldable_panel_t *ptThis);

extern
ARM_NONNULL(1)
void foldable_panel_unfold(foldable_panel_t *ptThis);

extern
ARM_NONNULL(1)
void foldable_panel_fold(foldable_panel_t *ptThis);

extern
ARM_NONNULL(1)
arm_2d_region_t * foldable_panel_get_draw_region(foldable_panel_t *ptThis);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
