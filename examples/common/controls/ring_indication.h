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

#ifndef __RING_INDICATION_H__
#define __RING_INDICATION_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__common.h"
#include "./meter_pointer.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#   pragma clang diagnostic ignored "-Wpadded"
#endif


/* OOC header, please DO NOT modify  */
#ifdef __RING_INDICATION_IMPLEMENT__
#   undef   __RING_INDICATION_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__RING_INDICATION_INHERIT__)
#   undef   __RING_INDICATION_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define RING_INDICATION_USER_DRAW(__FUNCTION_NAME)                              \
        void __FUNCTION_NAME(   void *pTarget,                                  \
                                ring_indication_t *ptRingIndicator,             \
                                const arm_2d_tile_t *ptTile,                    \
                                arm_2d_point_float_t *ptPivot,                  \
                                bool bIsNewFrame)

/*============================ TYPES =========================================*/

typedef struct ring_indication_t ring_indication_t;

typedef void ring_indication_user_draw_evt_handler_t(void *pTarget, 
                                                     ring_indication_t *ptRingIndicator,
                                                     const arm_2d_tile_t *ptTile,
                                                     arm_2d_point_float_t *ptPivot,
                                                     bool bIsNewFrame);

typedef struct ring_indication_user_draw_evt_t {
    ring_indication_user_draw_evt_handler_t *fnHandler;
    union {
        void *pTarget;
        uintptr_t nTarget;
    };
} ring_indication_user_draw_evt_t;

typedef struct ring_indication_cfg_t {
    arm_2d_scene_t *ptScene;
    __spin_zoom_widget_indication_t tIndication;

    struct {
        const arm_2d_tile_t *ptMask;
        float fAngleOffset;
        arm_2d_point_float_t tCentre;
    } QuarterSector;

    struct {
        const arm_2d_tile_t *ptTile;
        const arm_2d_tile_t *ptMask;
    } Foreground;

    struct {
        const arm_2d_tile_t *ptMask;
    } Background;

    spin_zoom_widget_mode_t *ptTransformMode;

    arm_2d_helper_pi_slider_cfg_t tPISliderCFG;

    arm_2d_helper_dirty_region_item_t *ptUserDirtyRegionItem;

    ring_indication_user_draw_evt_t evtUserDraw;

} ring_indication_cfg_t;

enum {
    RING_INDICATOR_START_FROM_Y_PLUS,
    RING_INDICATOR_START_FROM_X_MINUS,
    RING_INDICATOR_START_FROM_Y_MINUS,
    RING_INDICATOR_START_FROM_X_PLUS,
};



struct ring_indication_t {
ARM_PRIVATE(
    meter_pointer_t tSector;
    struct {
        const arm_2d_tile_t *ptTile;
        const arm_2d_tile_t *ptMask;
    } Foreground;

    struct {
        const arm_2d_tile_t *ptMask;
    } Background;

    float fSectorScale;
    float fLastAngle;

    int16_t iDiameter;
    uint16_t u2StartFrom                            : 2;
    uint16_t bNeedAddExtraRegion                    : 1;

    arm_2d_scene_t *ptScene;
    arm_2d_helper_dirty_region_item_t *ptUserDirtyRegionItem;

    ring_indication_user_draw_evt_t evtUserDraw;
)
};


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if defined(RTE_Acceleration_Arm_2D_Transform)
extern
ARM_NONNULL(1, 2)
void ring_indication_init( ring_indication_t *ptThis,
                          ring_indication_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void ring_indication_depose( ring_indication_t *ptThis);

extern
ARM_NONNULL(1)
void ring_indication_on_load( ring_indication_t *ptThis);

extern
ARM_NONNULL(1)
bool ring_indication_on_frame_start(ring_indication_t *ptThis, 
                                    int32_t nValue);

extern
ARM_NONNULL(1)
bool ring_indication_on_frame_start_f32(ring_indication_t *ptThis, 
                                        float fTargetValue);

extern
ARM_NONNULL(1)
void ring_indication_on_frame_complete( ring_indication_t *ptThis);

extern
ARM_NONNULL(1)
void ring_indication_show( ring_indication_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion, 
                            bool bIsNewFrame);

extern 
ARM_NONNULL(1)
arm_2d_size_t ring_indication_get_size(ring_indication_t *ptThis);

extern
ARM_NONNULL(1)
int32_t ring_indication_get_current_value(ring_indication_t *ptThis);

extern
ARM_NONNULL(1)
int32_t ring_indication_set_current_value(ring_indication_t *ptThis, int32_t nValue);

extern
ARM_NONNULL(1)
float ring_indication_get_current_value_f32(ring_indication_t *ptThis);

extern
ARM_NONNULL(1)
float ring_indication_set_current_value_f32(ring_indication_t *ptThis, float fValue);

#else

#define ring_indication_init(...)
#define ring_indication_depose(...)
#define ring_indication_on_load(...)
#define ring_indication_on_frame_start(...)         true
#define ring_indication_on_frame_start_f32(...)     true
#define ring_indication_on_frame_complete(...)
#define ring_indication_show(...)
#define ring_indication_get_size(...)               ((arm_2d_size_t){0})
#define ring_indication_get_current_value(...)      (0)
#define ring_indication_set_current_value(...)      (0)
#define ring_indication_get_current_value_f32(...)  (0)
#define ring_indication_set_current_value_f32(...)  (0)

#endif

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
