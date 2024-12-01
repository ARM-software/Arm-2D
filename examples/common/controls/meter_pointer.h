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

#ifndef __METER_POINTER_H__
#define __METER_POINTER_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__common.h"

#include "spin_zoom_widget.h"

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
#ifdef __METER_POINTER_IMPLEMENT__
#   undef   __METER_POINTER_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__METER_POINTER_INHERIT__)
#   undef   __METER_POINTER_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


typedef struct meter_pointer_cfg_t {
    implement_ex(spin_zoom_widget_cfg_t, tSpinZoom);

    struct {
        bool bIsSourceHorizontal;
        int16_t iRadius;
    } Pointer;

    arm_2d_helper_pi_slider_cfg_t tPISliderCFG;
    int32_t nPISliderStartPosition;

    

} meter_pointer_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct meter_pointer_t meter_pointer_t;

struct meter_pointer_t {
    implement(spin_zoom_widget_t);

ARM_PRIVATE(
    arm_2d_helper_pi_slider_cfg_t   tPISliderCFG; 
    arm_2d_helper_pi_slider_t       tPISlider;
    float                           fCurrentValue;
)
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
arm_2d_err_t meter_pointer_init(meter_pointer_t *ptThis,
                                meter_pointer_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void meter_pointer_depose( meter_pointer_t *ptThis);

extern
ARM_NONNULL(1)
void meter_pointer_on_load( meter_pointer_t *ptThis);

extern
ARM_NONNULL(1)
bool meter_pointer_on_frame_start(  meter_pointer_t *ptThis, 
                                    int32_t nTargetValue, 
                                    float fScale);

extern
ARM_NONNULL(1)
bool meter_pointer_on_frame_start_f32(  meter_pointer_t *ptThis, 
                                        float fTargetValue,
                                        float fScale);

extern
ARM_NONNULL(1)
void meter_pointer_on_frame_complete( meter_pointer_t *ptThis);

extern
ARM_NONNULL(1, 2)
void meter_pointer_show(meter_pointer_t *ptThis,
                        const arm_2d_tile_t *ptTile,
                        const arm_2d_region_t *ptRegion,
                        const arm_2d_location_t *ptPivot,
                        uint8_t chOpacity);

extern
ARM_NONNULL(1)
void meter_pointer_set_colour(  meter_pointer_t *ptThis, 
                                COLOUR_INT tColour);

extern
ARM_NONNULL(1)
int32_t meter_pointer_get_current_value(meter_pointer_t *ptThis);

extern
ARM_NONNULL(1)
int32_t meter_pointer_set_current_value(meter_pointer_t *ptThis, int32_t nValue);

extern
ARM_NONNULL(1)
float meter_pointer_get_current_value_f32(meter_pointer_t *ptThis);

extern
ARM_NONNULL(1)
float meter_pointer_set_current_value_f32(meter_pointer_t *ptThis, float fValue);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
