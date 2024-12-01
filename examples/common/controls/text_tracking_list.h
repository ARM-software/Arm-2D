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

#ifndef __TEXT_TRACKING_LIST_H__
#define __TEXT_TRACKING_LIST_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "text_list.h"

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
#ifdef __TEXT_TRACKING_LIST_IMPLEMENT__
#   undef   __TEXT_TRACKING_LIST_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__TEXT_TRACKING_LIST_INHERIT__)
#   undef   __TEXT_TRACKING_LIST_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"


/*============================ MACROFIED FUNCTIONS ===========================*/
#define text_tracking_list_init(__TEXT_TRACKING_LIST_PTR, __CFG_PTR, ...)       \
            __text_tracking_list_init(                                          \
                (__TEXT_TRACKING_LIST_PTR),                                     \
                (__CFG_PTR),                                                    \
                (&ARM_2D_LIST_CALCULATOR_NORMAL_FIXED_SIZED_ITEM_NO_STATUS_CHECK_VERTICAL, ##__VA_ARGS__))


/*============================ TYPES =========================================*/

typedef char * __disp_string_t ;

typedef struct text_tracking_list_setting_t {

    uint8_t bUsePIMode              : 1;

    /*! \note Auto Size mode only works with text left alignment */
    uint8_t bIndicatorAutoSize      : 1;

    uint8_t bShowScrollingBar       : 1;
    uint8_t bUseMonochromeMode      : 1;
    uint8_t                         : 4;

    /*! \note  0 means always-on. The unit is 100ms */
    uint8_t chScrollingBarAutoDisappearTimeX100Ms;

    union {
        uint8_t chValue;
        uint16_t hwValue;
        uint32_t wValue;
        COLOUR_INT_TYPE tValue;
    } IndicatorColour;

    union {
        uint8_t chValue;
        uint16_t hwValue;
        uint32_t wValue;
        COLOUR_INT_TYPE tValue;
    } ScrollingBar;

} text_tracking_list_setting_t;

typedef struct text_tracking_list_cfg_t {
    implement(text_list_cfg_t);

    union {
        /* please ensure the anonymous structure is identical to tSettings */
        text_tracking_list_setting_t tSettings;
        struct {
            uint8_t bUsePIMode              : 1;

            /*! \note Auto Size mode only works with text left alignment */
            uint8_t bIndicatorAutoSize      : 1;

            uint8_t bShowScrollingBar       : 1;
            uint8_t bUseMonochromeMode      : 1;  
            uint8_t                         : 4;       

            /*! \note  0 means always-on */
            uint8_t chScrollingBarAutoDisappearTimeX100Ms;

            union {
                uint8_t chValue;
                uint16_t hwValue;
                uint32_t wValue;
                COLOUR_INT_TYPE tValue;
            } IndicatorColour;

            union {
                uint8_t chValue;
                uint16_t hwValue;
                uint32_t wValue;
                COLOUR_INT_TYPE tValue;
            } ScrollingBar;
        };
    };

} text_tracking_list_cfg_t;

typedef struct text_tracking_list_t {
    implement(text_list_t);
ARM_PRIVATE(
    int64_t lTimestamp[2];

    int16_t iStringWidth;
    uint8_t bIsOnLoad                : 1;
    uint8_t bRedrawTheScrollingBar   : 1;
    uint8_t bSelectionChanged        : 1;
    uint8_t                          : 5;
    uint8_t chScrollingBarOpacity;


    arm_2d_size_t tLastStringSize;
    arm_2d_region_t tIndicatorRegion;

    arm_2d_helper_dirty_region_item_t tDirtyRegionItem;

    text_tracking_list_setting_t tSettings;

    
)

} text_tracking_list_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1,2,3)
arm_2d_err_t __text_tracking_list_init(  text_tracking_list_t *ptThis, 
                                text_tracking_list_cfg_t *ptCFG,
                                arm_2d_i_list_region_calculator_t *ptCalculator);

extern
ARM_NONNULL(1)
void text_tracking_list_depose(text_tracking_list_t *ptThis);

extern
ARM_NONNULL(1,2)
arm_fsm_rt_t text_tracking_list_show(  text_tracking_list_t *ptThis,
                                const arm_2d_tile_t *ptTile, 
                                const arm_2d_region_t *ptRegion, 
                                bool bIsNewFrame);

extern
ARM_NONNULL(1)
void text_tracking_list_move_selection( text_tracking_list_t *ptThis,
                                        int16_t iSteps,
                                        int32_t nFinishInMs);

extern 
ARM_NONNULL(1)
void text_tracking_list_on_frame_start(text_tracking_list_t *ptThis);


extern
ARM_NONNULL(1)
uint16_t text_tracking_list_get_selected_item_id(text_tracking_list_t *ptThis);

extern
ARM_NONNULL(1)
__disp_string_t text_tracking_list_get_item_string(text_tracking_list_t *ptThis, uint_fast16_t hwItemID);

extern
ARM_NONNULL(1)
uint16_t text_tracking_list_get_list_item_count(text_tracking_list_t *ptThis);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
