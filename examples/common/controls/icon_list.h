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

#ifndef __ICON_LIST_H__
#define __ICON_LIST_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "arm_2d_helper_list.h"
#include "__simple_list.h"

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
#ifdef __ICON_LIST_IMPLEMENT__
#   undef   __ICON_LIST_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__ICON_LIST_INHERIT__)
#   undef   __ICON_LIST_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"


/*============================ MACROFIED FUNCTIONS ===========================*/

#define icon_list_init(__ICON_LIST_PTR, __CFG_PTR, ...)                         \
            __icon_list_init(   (__ICON_LIST_PTR),                              \
                                (__CFG_PTR),                                    \
                                (&ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_FIXED_SIZED_ITEM_NO_STATUS_CHECK_HORIZONTAL, ##__VA_ARGS__))

/*============================ TYPES =========================================*/

typedef struct icon_list_cfg_t {

    implement(__simple_list_cfg_t);

    struct {
        const arm_2d_tile_t **pptileMasks;
        uint16_t hwCount;
    } Icon;

} icon_list_cfg_t;

typedef struct icon_list_t {
    implement(__simple_list_t);

    struct {
        const arm_2d_tile_t **pptileMasks;
        uint16_t hwCount;
    } IconListCFG;
} icon_list_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1, 2, 3)
arm_2d_err_t __icon_list_init(
                            icon_list_t *ptThis, 
                            icon_list_cfg_t *ptCFG,
                            arm_2d_i_list_region_calculator_t *ptCalculator);

extern
ARM_NONNULL(1)
void icon_list_depose(icon_list_t *ptThis);

extern
ARM_NONNULL(1,2)
arm_fsm_rt_t icon_list_show(icon_list_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion, 
                            bool bIsNewFrame);

extern
ARM_NONNULL(1)
void icon_list_move_selection( icon_list_t *ptThis,
                                        int16_t iSteps,
                                        int32_t nFinishInMs);

extern 
ARM_NONNULL(1)
void icon_list_on_frame_start(icon_list_t *ptThis);


extern
ARM_NONNULL(1)
uint16_t icon_list_get_selected_item_id(icon_list_t *ptThis);

extern
ARM_NONNULL(1)
const arm_2d_tile_t * icon_list_get_item_icon(icon_list_t *ptThis, uint_fast16_t hwItemID);

extern
ARM_NONNULL(1)
uint16_t icon_list_get_list_item_count(icon_list_t *ptThis);

extern
ARM_NONNULL(1)
void icon_list_request_redraw_list(icon_list_t *ptThis);

extern
ARM_NONNULL(1)
void icon_list_request_redraw_selected_item(icon_list_t *ptThis);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
