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

#ifndef __NUMBER_LIST_H__
#define __NUMBER_LIST_H__

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
#   undef   __NUMBER_LIST_IMPLEMENT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct number_list_cfg_t {
    const char *pchFormatString;
    int32_t nStart;
    int16_t iDelta;
    uint16_t hwCount;
    COLOUR_INT tFontColour;
    COLOUR_INT tBackgroundColour;
    arm_2d_size_t tItemSize;
    arm_2d_size_t tListSize;

    int8_t chPrviousePadding;
    int8_t chNextPadding;
    int16_t hwSwitchingPeriodInMs;
    bool bIgnoreBackground;
    uint8_t chOpacity;

    arm_2d_draw_list_item_handler_t *fnOnDrawListItemBackground;                /*!< the On-Draw-List-Core-Item-Background event handler */
    arm_2d_helper_draw_handler_t    *fnOnDrawListBackground;                    /*!< the On-Draw-List-Core-Background event handler */
    arm_2d_helper_draw_handler_t    *fnOnDrawListCover;                         /*!< the On-Draw-List-Core-Cover event handler */
    arm_2d_font_t                   *ptFont;                                    /*!< user specified font */
} number_list_cfg_t;


typedef struct number_list_t {
    implement(__arm_2d_list_core_t);

ARM_PRIVATE(
    number_list_cfg_t tNumListCFG;
    arm_2d_list_item_t tTempItem;
    
    int32_t nSelection;
    int32_t nIterationIndex;
)

} number_list_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern arm_2d_user_font_t ARM_2D_FONT_A2_DIGITS_ONLY;
extern arm_2d_user_font_t ARM_2D_FONT_A4_DIGITS_ONLY;
extern arm_2d_user_font_t ARM_2D_FONT_A8_DIGITS_ONLY;

/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1,2)
void number_list_init(  number_list_t *ptThis, 
                        number_list_cfg_t *ptCFG);



extern
ARM_NONNULL(1,2)
arm_fsm_rt_t number_list_show(  number_list_t *ptThis,
                                const arm_2d_tile_t *ptTile, 
                                const arm_2d_region_t *ptRegion, 
                                bool bIsNewFrame);

extern
ARM_NONNULL(1)
arm_2d_err_t numer_list_move_selection( number_list_t *ptThis,
                                        int16_t iSteps,
                                        int32_t nFinishInMs);
#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
