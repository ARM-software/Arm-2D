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

#if defined(__STDC_VERSION__) && __STDC_VERSION__ > 199901L
#define spin_zoom_widget_show(  __THIS_PTR,                                     \
                                __TARGET_TILE_PTR,                              \
                                __REGION_PTR,                                   \
                                __PIVOT_PTR,                                    \
                                __OPACITY)                                      \
    _Generic((__PIVOT_PTR),                                                     \
        const arm_2d_location_t *   : spin_zoom_widget_show_with_normal_pivot,  \
        arm_2d_location_t *         : spin_zoom_widget_show_with_normal_pivot,  \
        intptr_t                    : spin_zoom_widget_show_with_fp_pivot,      \
        void *                      : spin_zoom_widget_show_with_fp_pivot,      \
        const void *                : spin_zoom_widget_show_with_fp_pivot,      \
        const arm_2d_point_float_t *: spin_zoom_widget_show_with_fp_pivot,      \
        arm_2d_point_float_t *      : spin_zoom_widget_show_with_fp_pivot       \
    )(  (__THIS_PTR),                                                           \
        (__TARGET_TILE_PTR),                                                    \
        (__REGION_PTR),                                                         \
        (__PIVOT_PTR),                                                          \
        (__OPACITY))

#else
#   define spin_zoom_widget_show    spin_zoom_widget_show_with_normal_pivot
#endif
/*============================ TYPES =========================================*/


typedef struct spin_zoom_widget_t spin_zoom_widget_t;

typedef const struct spin_zoom_widget_mode_t {

    arm_fsm_rt_t (*fnTransform)(spin_zoom_widget_t *ptThis, 
                                const arm_2d_tile_t *ptTile,
                                const arm_2d_region_t *ptRegion,
                                const arm_2d_point_float_t *ptPivot,
                                uint8_t chOpacity);

} spin_zoom_widget_mode_t;

typedef struct spin_zoom_widget_cfg_t {
    arm_2d_scene_t *ptScene;

    struct {
        const arm_2d_tile_t     *ptMask;
        const arm_2d_tile_t     *ptSource;
        union {
            arm_2d_point_float_t    tCentreFloat;
            arm_2d_location_t       tCentre;
        };
        union {
            COLOUR_INT     tColourForKeying;
            COLOUR_INT     tColourToFill;
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

        struct {
            float fAngle;
            union {
                float fScale;
                float fScaleX;
            };
            float fScaleY;
        } Step;
    } Indicator;

    uint32_t bUseFloatPointInCentre : 1;
    uint32_t                        : 31;

} spin_zoom_widget_cfg_t;

struct spin_zoom_widget_t {


ARM_PROTECTED(
    spin_zoom_widget_cfg_t tCFG;
)

ARM_PRIVATE(
    union {
        arm_2d_op_fill_cl_msk_opa_trans_t   tFillColourTransform;
        arm_2d_op_trans_msk_opa_t           tTileTransform;
        arm_2d_op_trans_opa_t               tTile;
    } OPCODE;

    arm_2d_helper_dirty_region_transform_t  tHelper;
)
    
};

/*============================ GLOBAL VARIABLES ==============================*/
extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_FILL_COLOUR;

extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_TILE_WITH_MASK;

extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_TILE_ONLY;

extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_TILE_WITH_COLOUR_KEYING;

/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void spin_zoom_widget_init( spin_zoom_widget_t *ptThis,
                            spin_zoom_widget_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void spin_zoom_widget_depose(spin_zoom_widget_t *ptThis);

extern
ARM_NONNULL(1)
void spin_zoom_widget_on_load(spin_zoom_widget_t *ptThis);

extern
ARM_NONNULL(1)
void spin_zoom_widget_on_frame_start(   spin_zoom_widget_t *ptThis, 
                                        int32_t nValue, 
                                        float fScale);

extern
ARM_NONNULL(1)
void spin_zoom_widget_on_frame_start_f32(spin_zoom_widget_t *ptThis, 
                                         float fValue, 
                                         float fScale);

extern
ARM_NONNULL(1)
float spin_zoom_widget_get_current_angle(spin_zoom_widget_t *ptThis);

extern
ARM_NONNULL(1)
void spin_zoom_widget_on_frame_complete( spin_zoom_widget_t *ptThis);

extern
ARM_NONNULL(1,2)
void spin_zoom_widget_show_with_normal_pivot(   spin_zoom_widget_t *ptThis,
                                                const arm_2d_tile_t *ptTile,
                                                const arm_2d_region_t *ptRegion,
                                                const arm_2d_location_t *ptPivot,
                                                uint8_t chOpacity);

extern
ARM_NONNULL(1,2)
void spin_zoom_widget_show_with_fp_pivot(   spin_zoom_widget_t *ptThis,
                                            const arm_2d_tile_t *ptTile,
                                            const arm_2d_region_t *ptRegion,
                                            const arm_2d_point_float_t *ptPivot,
                                            uint8_t chOpacity);
extern
ARM_NONNULL(1)
void spin_zoom_widget_set_colour( spin_zoom_widget_t *ptThis, COLOUR_INT tColour);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
