/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
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

/*============================ INCLUDES ======================================*/
#define __ARM_2D_HELPER_LIST_VIEW_INHERIT__
#define __NUMBER_LIST_IMPLEMENT__

#include "./list_view.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include <assert.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


static 
IMPL_PFB_ON_DRAW(__arm_2d_list_view_draw_background)
{
    ARM_2D_UNUSED(bIsNewFrame);
    
    list_view_t *ptThis = (list_view_t *)pTarget;


    if (!this.tListViewCFG.bIgnoreBackground) {
        arm_2d_fill_colour_with_opacity(
                            ptTile, 
                            NULL, 
                            (__arm_2d_color_t) {this.tListViewCFG.tBackgroundColour},
                            this.tListViewCFG.chOpacity);
    }
    
    arm_2d_op_wait_async(NULL);
    
    return arm_fsm_rt_cpl;
}


ARM_NONNULL(1,2)
void list_view_init(list_view_t *ptThis, 
                    list_view_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(NULL != ptCFG->fnIterator);

    this.tListViewCFG = *ptCFG;

    /* validation */
    if (0 == this.tListViewCFG.chOpacity) {
        this.tListViewCFG.chOpacity = 255;
    }

    /* call base class contructor */
    do {
        __arm_2d_list_core_cfg_t tCFG = {
            .fnIterator = ptCFG->fnIterator,
            
            /* vertical list, centre aligned style */
            .fnCalculator = ptCFG->fnCalculator,
            .fnOnDrawListBackground = &__arm_2d_list_view_draw_background,
            //.fnOnDrawListItemBackground =       &__arm_2d_number_list_draw_list_core_item_background,
            .hwSwitchingPeriodInMs = ptCFG->hwSwitchingPeriodInMs,
            .hwItemCount = ptCFG->hwCount,
            .hwItemSizeInByte = ptCFG->hwItemSizeInByte,
            .nTotalLength = 0,
            .tListSize = ptCFG->tListSize,
            .ptItems = ptCFG->ptItems,
        };

        /* you can override the default implementations */
        if (NULL != ptCFG->fnOnDrawListBackground) {
            tCFG.fnOnDrawListBackground = ptCFG->fnOnDrawListBackground;
        }
        if (NULL != ptCFG->fnOnDrawListCover) {
            tCFG.fnOnDrawListCover = ptCFG->fnOnDrawListCover;
        }
        if (NULL != ptCFG->fnOnDrawListItemBackground) {
            tCFG.fnOnDrawListItemBackground = ptCFG->fnOnDrawListItemBackground;
        }

        __arm_2d_list_core_init(&this.use_as____arm_2d_list_core_t, &tCFG);
    } while(0);

    /* request updating StartOffset */
    //this.use_as____arm_2d_list_core_t.CalMidAligned.bListHeightChanged = true;
}



ARM_NONNULL(1,2)
arm_fsm_rt_t list_view_show(list_view_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion, 
                            bool bIsNewFrame)
{

    return __arm_2d_list_core_show( &this.use_as____arm_2d_list_core_t,
                                    ptTile,
                                    ptRegion,
                                    bIsNewFrame);
}


ARM_NONNULL(1)
void list_view_move_selection(  list_view_t *ptThis,
                                        int16_t iSteps,
                                        int32_t nFinishInMs)
{
    __arm_2d_list_core_move_request(&this.use_as____arm_2d_list_core_t,
                                             iSteps,
                                             nFinishInMs);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
