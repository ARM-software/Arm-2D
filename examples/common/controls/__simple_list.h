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

#ifndef __SIMPLE_LIST_H__
#define __SIMPLE_LIST_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "arm_2d_helper_list.h"

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
#ifdef __SIMPLE_LIST_IMPLEMENT__
#   undef   __SIMPLE_LIST_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__SIMPLE_LIST_INHERIT__)
#   undef   __SIMPLE_LIST_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"

/* Opps! typos..., please use the correct spelling. 
 * The typos will be removed in the future!
 */
#define numer_list_move_selection number_list_move_selection
#define chPrviousePadding   chPreviousPadding  

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __simple_list_cfg_t {
    uint16_t hwCount;

    bool bIgnoreBackground;
    uint8_t chOpacity;

    COLOUR_INT tFontColour;
    COLOUR_INT tBackgroundColour;

    arm_2d_size_t tItemSize;
    arm_2d_size_t tListSize;

    int8_t chPreviousPadding;
    int8_t chNextPadding;
    int16_t hwSwitchingPeriodInMs;
    

    arm_2d_draw_list_item_handler_t *fnOnDrawListItem;                          /*!< the On-Draw-List-Core-Item event handler */
    arm_2d_draw_list_item_handler_t *fnOnDrawListItemBackground;                /*!< the On-Draw-List-Core-Item-Background event handler */
    arm_2d_helper_draw_handler_t    *fnOnDrawListBackground;                    /*!< the On-Draw-List-Core-Background event handler */
    arm_2d_helper_draw_handler_t    *fnOnDrawListCover;                         /*!< the On-Draw-List-Core-Cover event handler */
    arm_2d_font_t                   *ptFont;                                    /*!< user specified font */
    arm_2d_align_t                  tTextAlignment;                             /*!< text alignment in each item*/

    bool bUseDirtyRegion;
    arm_2d_scene_t *ptTargetScene;
} __simple_list_cfg_t;

typedef struct __simple_list_t {
    implement(__arm_2d_list_core_t);

ARM_PROTECTED(
    __simple_list_cfg_t tSimpleListCFG;
)

ARM_PRIVATE(
    arm_2d_list_item_t tTempItem;
    
    int32_t nIterationIndex;

    arm_2d_region_list_item_t tDirtyRegion;
)

} __simple_list_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/* only derived class can see the following methods */
ARM_PROTECTED_METHOD(

    extern
    ARM_NONNULL(1,2)
    arm_2d_err_t __simple_list_init(  __simple_list_t *ptThis, 
                                    __simple_list_cfg_t *ptCFG);

    extern
    ARM_NONNULL(1,2)
    void __simple_list_set_draw_list_item_handler(
                                        __simple_list_t *ptThis,
                                        arm_2d_draw_list_item_handler_t *fnHandler);

    extern
    ARM_NONNULL(1)
    void __simple_list_depose(__simple_list_t *ptThis);

    extern
    ARM_NONNULL(1,2)
    arm_fsm_rt_t __simple_list_show(  __simple_list_t *ptThis,
                                    const arm_2d_tile_t *ptTile,
                                    const arm_2d_region_t *ptRegion, 
                                    bool bIsNewFrame);
    extern
    ARM_NONNULL(1,2,4)
    int __simple_list_item_printf(  __simple_list_t *ptThis, 
                                    const arm_2d_region_t *ptRegion,
                                    arm_2d_align_t tAlignment,
                                    const char *format, 
                                    ...);

    extern
    ARM_NONNULL(1)
    void __simple_list_move_selection(__simple_list_t *ptThis,
                                    int16_t iSteps,
                                    int32_t nFinishInMs);

    extern 
    ARM_NONNULL(1)
    void __simple_list_on_frame_start(__simple_list_t *ptThis);


    extern
    ARM_NONNULL(1)
    uint16_t __simple_list_get_selected_item_id(__simple_list_t *ptThis);

)

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
