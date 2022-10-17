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

#ifndef __LIST_VIEW_H__
#define __LIST_VIEW_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "arm_2d_helper_list.h"
#include "arm_extra_lcd_printf.h"
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
#ifdef __NUMBER_LIST_IMPLEMENT__
#   undef   __NUMBER_LIST_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__NUMBER_LIST_INHERIT__)
#   undef   __NUMBER_LIST_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct list_view_cfg_t {

    arm_2d_size_t tListSize;

    arm_2d_draw_list_item_handler_t     *fnOnDrawListItemBackground;            /*!< the On-Draw-List-Core-Item-Background event handler */
    arm_2d_helper_draw_handler_t        *fnOnDrawListBackground;                /*!< the On-Draw-List-Core-Background event handler */
    arm_2d_helper_draw_handler_t        *fnOnDrawListCover;                     /*!< the On-Draw-List-Core-Cover event handler */

    __arm_2d_list_item_iterator         *fnIterator;
    __arm_2d_list_region_calculator_t   *fnCalculator;
    arm_2d_list_item_t *ptItems;
    uint16_t hwCount;
    uint16_t hwItemSizeInByte;

    int16_t hwSwitchingPeriodInMs;
    bool bIgnoreBackground;
    uint8_t chOpacity;
    COLOUR_INT tBackgroundColour;
} list_view_cfg_t;


typedef struct list_view_t {
    implement(__arm_2d_list_core_t);

ARM_PRIVATE(
    list_view_cfg_t tListViewCFG;
    
    int32_t nSelection;
    int32_t nIterationIndex;
)

} list_view_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1,2)
void list_view_init(list_view_t *ptThis, 
                    list_view_cfg_t *ptCFG);



extern
ARM_NONNULL(1,2)
arm_fsm_rt_t list_view_show(list_view_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion, 
                            bool bIsNewFrame);

extern
ARM_NONNULL(1)
arm_2d_err_t list_view_move_selection(  list_view_t *ptThis,
                                        int16_t iSteps,
                                        int32_t nFinishInMs);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
