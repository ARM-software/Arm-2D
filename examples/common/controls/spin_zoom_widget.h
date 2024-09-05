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

#ifndef __SPIN_ZOOM_WIDGET_H__
#define __SPIN_ZOOM_WIDGET_H__

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
#ifdef __SPIN_ZOOM_WIDGET_IMPLEMENT__
#   undef   __SPIN_ZOOM_WIDGET_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__SPIN_ZOOM_WIDGET_INHERIT__)
#   undef   __SPIN_ZOOM_WIDGET_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


typedef struct spin_zoom_widget_t spin_zoom_widget_t;

typedef const struct spin_zoom_widget_mode_t {

    arm_fsm_rt_t (*fnTransform)(spin_zoom_widget_t *ptThis, 
                                const arm_2d_tile_t *ptTile,
                                const arm_2d_region_t *ptRegion,
                                const arm_2d_location_t *ptPivot,
                                uint8_t chOpacity);

} spin_zoom_widget_mode_t;

typedef struct spin_zoom_widget_cfg_t {
    arm_2d_scene_t *ptScene;

    struct {
        const arm_2d_tile_t     *ptMask;
        const arm_2d_tile_t     *ptSource;
        arm_2d_location_t       tCentre;
        union {
            COLOUR_INT_TYPE     tColourForKeying;
            COLOUR_INT_TYPE     tColourToFill;
        };
    } Source;

    spin_zoom_widget_mode_t *ptTransformMode;

    struct {
        struct {
            float fAngleInDegree;
            int32_t nValue;
        } LowerLimit;

        struct {
            float fAngleInDegree;
            int32_t nValue;
        } UpperLimit;
    } Indicator;

    arm_2d_helper_pi_slider_cfg_t tPISliderCFG;

} spin_zoom_widget_cfg_t;

struct spin_zoom_widget_t {

ARM_PRIVATE(

    spin_zoom_widget_cfg_t tCFG;

    union {
        arm_2d_op_fill_cl_msk_opa_trans_t   tFillColourTransform;
        arm_2d_op_trans_msk_opa_t           tTileTransform;
    } OPCODE;

    arm_2d_helper_dirty_region_transform_t  tHelper;
    arm_2d_helper_pi_slider_t               tPISlider;
)
    
};

/*============================ GLOBAL VARIABLES ==============================*/
extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_FILL_COLOUR;

/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void spin_zoom_widget_init( spin_zoom_widget_t *ptThis,
                          spin_zoom_widget_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void spin_zoom_widget_depose( spin_zoom_widget_t *ptThis);

extern
ARM_NONNULL(1)
void spin_zoom_widget_on_load( spin_zoom_widget_t *ptThis);

extern
ARM_NONNULL(1)
void spin_zoom_widget_on_frame_start( spin_zoom_widget_t *ptThis, int32_t nValue, float fScale);

extern
ARM_NONNULL(1,2,4)
void spin_zoom_widget_show( spin_zoom_widget_t *ptThis,
                            const arm_2d_tile_t *ptTile,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_location_t *ptPivot,
                            uint8_t chOpacity);


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
