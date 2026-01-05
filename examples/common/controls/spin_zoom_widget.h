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

#if defined(RTE_Acceleration_Arm_2D_Transform)
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
#else
#   define spin_zoom_widget_show(...)
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

typedef struct __spin_zoom_widget_indication_t {
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
} __spin_zoom_widget_indication_t;

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

    struct {
        const arm_2d_tile_t     *ptMask;
    } Target;

    struct {
        const arm_2d_tile_t     *ptTile;
        const arm_2d_tile_t     *ptMask;
    } Extra;

    spin_zoom_widget_mode_t *ptTransformMode;

    __spin_zoom_widget_indication_t Indicator;

    uint32_t bUseFloatPointInCentre : 1;
    uint32_t bValueSaturation       : 1;
    uint32_t                        : 30;

} spin_zoom_widget_cfg_t;

struct spin_zoom_widget_t {

ARM_PRIVATE(
    union {
        arm_2d_op_fill_cl_msk_opa_trans_t                   tFillColourTransform;
        arm_2d_op_fill_cl_trans_msk_des_msk_opa_t           tFillColourTransformTargetMask;
        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t   tTileCopyWithTransformedMask;
        arm_2d_op_trans_msk_opa_t                           tTileTransform;
        arm_2d_op_trans_opa_t                               tTile;
    } OPCODE;

    struct {
        spin_zoom_widget_mode_t *ptTransformMode;
        float fAngleOffset;
    } Request;
)

ARM_PUBLIC(
    arm_2d_helper_dirty_region_transform_t  tHelper;
)

ARM_PROTECTED(
    spin_zoom_widget_cfg_t tCFG;
)
    
};

/*============================ GLOBAL VARIABLES ==============================*/

#if defined(RTE_Acceleration_Arm_2D_Transform)
/*!
 * \brief colour filling with a transformed mask
 */
extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_FILL_COLOUR;

/*!
 * \brief colour filling with a transformed mask and a target mask
 */
extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_FILL_COLOUR_WITH_TARGET_MASK;

/*!
 * \brief tile-transformation with a source mask
 */
extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_TILE_WITH_MASK;

/*!
 * \brief tile-transformation
 */
extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_TILE_ONLY;

/*!
 * \brief tile-transformation with colour keying
 */
extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_TILE_WITH_COLOUR_KEYING;

/*!
 * \brief tile-copy with a transformed mask, a source mask and a target mask
 */
extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK;

/*!
 * \brief tile-copy with a transformed mask and a source mask
 */
extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK;

/*!
 * \brief tile-copy with a transformed mask and a target mask
 */
extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK;

/*!
 * \brief tile-copy with a transformed mask
 */
extern
spin_zoom_widget_mode_t SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK;
#else
extern
spin_zoom_widget_mode_t __SPIN_ZOOM_MODE_NULL;

#define SPIN_ZOOM_MODE_FILL_COLOUR                              __SPIN_ZOOM_MODE_NULL

#define SPIN_ZOOM_MODE_FILL_COLOUR_WITH_TARGET_MASK             __SPIN_ZOOM_MODE_NULL

#define SPIN_ZOOM_MODE_TILE_WITH_MASK                           __SPIN_ZOOM_MODE_NULL

#define SPIN_ZOOM_MODE_TILE_ONLY                                __SPIN_ZOOM_MODE_NULL

#define SPIN_ZOOM_MODE_TILE_WITH_COLOUR_KEYING                  __SPIN_ZOOM_MODE_NULL

#define SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK_SOURCE_MASK_AND_TARGET_MASK\
                                                                __SPIN_ZOOM_MODE_NULL

#define SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK            \
                                                                __SPIN_ZOOM_MODE_NULL

#define SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK            \
                                                                __SPIN_ZOOM_MODE_NULL

#define SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK    __SPIN_ZOOM_MODE_NULL
#endif

/*============================ PROTOTYPES ====================================*/

#if defined(RTE_Acceleration_Arm_2D_Transform)
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
void spin_zoom_widget_on_frame_start_xy(spin_zoom_widget_t *ptThis, 
                                        int32_t nValue, 
                                        float fScaleX, 
                                        float fScaleY);

extern
ARM_NONNULL(1)
void spin_zoom_widget_on_frame_start_f32(spin_zoom_widget_t *ptThis, 
                                         float fValue, 
                                         float fScale);

extern
ARM_NONNULL(1)
void spin_zoom_widget_on_frame_start_xy_f32(spin_zoom_widget_t *ptThis, 
                                            float fValue, 
                                            float fScaleX, 
                                            float fScaleY);

extern
ARM_NONNULL(1)
float spin_zoom_valid_value(spin_zoom_widget_t *ptThis, float fValue);

/*!
 * \brief get the (nominal) angle (without the user configured offset)
 */
extern
ARM_NONNULL(1)
float spin_zoom_widget_get_current_angle(spin_zoom_widget_t *ptThis);

/*!
 * \brief get the actual angle
 */
extern
ARM_NONNULL(1)
float spin_zoom_widget_get_actual_angle(spin_zoom_widget_t *ptThis);

extern
ARM_NONNULL(1)
void spin_zoom_widget_on_frame_complete( spin_zoom_widget_t *ptThis);

ARM_NONNULL(1)
void spin_zoom_widget_set_angle_offset(
                                    spin_zoom_widget_t *ptThis, 
                                    float fAngleOffset);

extern
ARM_NONNULL(1,2)
void spin_zoom_widget_update_transform_mode(
                                    spin_zoom_widget_t *ptThis, 
                                    spin_zoom_widget_mode_t *ptTransformMode);

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

extern
ARM_NONNULL(1)
void spin_zoom_widget_set_source(   spin_zoom_widget_t  *ptThis, 
                                    const arm_2d_tile_t *ptSource,
                                    const arm_2d_tile_t *ptMask,
                                    arm_2d_location_t   tCentre);

extern
ARM_NONNULL(1)
void spin_zoom_widget_set_source_f32(   spin_zoom_widget_t *ptThis, 
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptMask,
                                        arm_2d_point_float_t tCentre);
#else

#   define spin_zoom_widget_init(...)
#   define spin_zoom_widget_depose(...)
#   define spin_zoom_widget_on_load(...);
#   define spin_zoom_widget_on_frame_start(...)
#   define spin_zoom_widget_on_frame_start_xy(...)
#   define spin_zoom_widget_on_frame_start_f32(...)
#   define spin_zoom_widget_on_frame_start_xy_f32(...)
#   define spin_zoom_widget_get_current_angle(...)          (0)
#   define spin_zoom_widget_get_actual_angle(...)           (0)
#   define spin_zoom_widget_on_frame_complete(...)
#   define spin_zoom_widget_update_transform_mode(...)
#   define spin_zoom_widget_show_with_normal_pivot(...)
#   define spin_zoom_widget_show_with_fp_pivot(...)
#   define spin_zoom_widget_set_colour(...)
#   define spin_zoom_widget_set_source(...)
#   define spin_zoom_widget_set_source_f32(...)
#   define spin_zoom_valid_value(...)                       (0)
#endif

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
