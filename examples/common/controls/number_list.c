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

#define __NUMBER_LIST_IMPLEMENT__
#define __ARM_2D_HELPER_LIST_VIEW_INHERIT__

#include "./arm_extra_controls.h"
#include "./number_list.h"

#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

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
#   pragma clang diagnostic ignored "-Wformat-nonliteral"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif


#undef this
#define this        (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


static 
IMPL_ON_DRAW_EVT(__arm_2d_number_list_draw_background)
{
    ARM_2D_UNUSED(bIsNewFrame);
    
    number_list_t *ptThis = (number_list_t *)pTarget;


    if (!this.tNumListCFG.bIgnoreBackground) {
        arm_2d_fill_colour_with_opacity(
                            ptTile, 
                            NULL, 
                            (__arm_2d_color_t) {this.tNumListCFG.tBackgroundColour},
                            this.tNumListCFG.chOpacity);
    }
    
    arm_2d_op_wait_async(NULL);
    
    return arm_fsm_rt_cpl;
}


#if defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__
#define __va_list    va_list

#endif



static 
int __printf(number_list_t *ptThis, const arm_2d_region_t *ptRegion, const char *format, ...)
{
    int real_size;
    static char s_chBuffer[__LCD_PRINTF_CFG_TEXT_BUFFER_SIZE__ + 1];
    __va_list ap;
    va_start(ap, format);
        real_size = vsnprintf(s_chBuffer, sizeof(s_chBuffer)-1, format, ap);
    va_end(ap);
    real_size = MIN(sizeof(s_chBuffer)-1, real_size);
    s_chBuffer[real_size] = '\0';
    
    int16_t iWidth = this.tNumListCFG.ptFont->tCharSize.iWidth;
    int16_t iHeight = this.tNumListCFG.ptFont->tCharSize.iHeight;

    arm_2d_align_centre( *ptRegion, (int16_t)real_size * iWidth, iHeight) {
        arm_lcd_text_set_draw_region(&__centre_region);
        arm_lcd_puts(s_chBuffer);
        arm_lcd_text_set_draw_region(NULL);
    }

    return real_size;
}

static 
arm_fsm_rt_t __arm_2d_number_list_draw_list_core_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    number_list_t *ptThis = (number_list_t *)ptItem->ptListView;
    ARM_2D_UNUSED(ptItem);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    arm_lcd_text_set_font(this.tNumListCFG.ptFont);

    arm_lcd_text_set_colour(this.tNumListCFG.tFontColour, this.tNumListCFG.tBackgroundColour);
    arm_lcd_text_set_display_mode(ARM_2D_DRW_PATN_MODE_COPY);
    
    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
    
    arm_2d_canvas(ptTile, __top_container) {
        /* print numbers */
        __printf(ptThis,
                 &__top_container,
                 this.tNumListCFG.pchFormatString,
                 this.tNumListCFG.nStart + ptItem->hwID * this.tNumListCFG.iDelta);
    }
    arm_lcd_text_set_target_framebuffer(NULL);
    
    return arm_fsm_rt_cpl;
}

static arm_2d_list_item_t *__arm_2d_number_list_iterator(
                                        __arm_2d_list_core_t *ptListView,
                                        arm_2d_list_iterator_dir_t tDirection,
                                        uint_fast16_t hwID
                                    )
{
    number_list_t *ptThis = (number_list_t *)ptListView;
    int32_t nIterationIndex;
    switch (tDirection) {
        default:
        case __ARM_2D_LIST_GET_ITEM_WITH_ID_WITHOUT_MOVE_POINTER:
            nIterationIndex = hwID;
            nIterationIndex %= this.tNumListCFG.hwCount;
            break;

        case __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER:
            this.nIterationIndex = hwID;
            this.nIterationIndex %= this.tNumListCFG.hwCount;
            nIterationIndex = this.nIterationIndex;
            break;

        case __ARM_2D_LIST_GET_PREVIOUS:
            if (this.nIterationIndex) {
                this.nIterationIndex--;
            } else {
                this.nIterationIndex = this.tNumListCFG.hwCount - 1;
            }
            nIterationIndex = this.nIterationIndex;
            break;

        case __ARM_2D_LIST_GET_NEXT:
            this.nIterationIndex++;
            this.nIterationIndex %= this.tNumListCFG.hwCount;
            
            nIterationIndex = this.nIterationIndex;
            break;

        case __ARM_2D_LIST_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER:
            nIterationIndex = 0;
            break;

        case __ARM_2D_LIST_GET_FIRST_ITEM:
            this.nIterationIndex = 0;
            nIterationIndex = this.nIterationIndex;
            break;

        case __ARM_2D_LIST_GET_CURRENT:
            nIterationIndex = this.nIterationIndex;
            break;

        case __ARM_2D_LIST_GET_LAST_ITEM_WITHOUT_MOVE_POINTER:
            nIterationIndex = this.tNumListCFG.hwCount - 1;
            break;

        case __ARM_2D_LIST_GET_LAST_ITEM:
            this.nIterationIndex = this.tNumListCFG.hwCount - 1;
            nIterationIndex = this.nIterationIndex;
            break;
    }

    /* validate item size */
    if (this.tTempItem.tSize.iHeight <= 0) {
        this.tTempItem.tSize.iHeight = this.tNumListCFG.ptFont->tCharSize.iHeight;
    }
    if (this.tTempItem.tSize.iWidth <= 0) {
        this.tTempItem.tSize.iWidth 
            = this.use_as____arm_2d_list_core_t
                .Runtime.tileList.tRegion.tSize.iWidth;
    }

    nIterationIndex %= this.tNumListCFG.hwCount;

    /* update item id : pretend that this is a different list core item */
    this.tTempItem.hwID = (uint16_t)nIterationIndex;

    return &this.tTempItem;
}


ARM_NONNULL(1,2)
void number_list_init(  number_list_t *ptThis, 
                        number_list_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(ptCFG->hwCount > 0);

    static const char c_chDefaultFormatString[] = {"%d"};


    int16_t iItemHeight = ptCFG->chPrviousePadding + ptCFG->chNextPadding;

    if (NULL != ptCFG->ptFont) {
        iItemHeight += ptCFG->ptFont->tCharSize.iHeight;
    } else {
        iItemHeight += 8;
    }

    /* call base class contructor */
    do {
        __arm_2d_list_core_cfg_t tCFG = {
            .fnIterator = &__arm_2d_number_list_iterator,
            
            /* vertical list, centre aligned style */
            .fnCalculator = &ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_FIXED_SIZED_ITEM_NO_STATUS_CHECK_VERTICAL,
            .fnOnDrawListBackground = &__arm_2d_number_list_draw_background,
            //.fnOnDrawListItemBackground =       &__arm_2d_number_list_draw_list_core_item_background,
            .hwSwitchingPeriodInMs = ptCFG->hwSwitchingPeriodInMs,
            .hwItemCount = ptCFG->hwCount,
            .nTotalLength = ptCFG->hwCount * iItemHeight,
            .tListSize = ptCFG->tListSize,
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

    this.tNumListCFG = *ptCFG;
    
    if (NULL == this.tNumListCFG.ptFont) {
        this.tNumListCFG.ptFont = (arm_2d_font_t *)&ARM_2D_FONT_6x8;
    }
    
    /* validation */
    if (!this.tNumListCFG.hwCount) {
        this.tNumListCFG.hwCount = 1;      /* at least one item */
    }

    if (NULL == this.tNumListCFG.pchFormatString) {
        /* use the default format string */
        this.tNumListCFG.pchFormatString = c_chDefaultFormatString;
    }
    
    if (0 == this.tNumListCFG.chOpacity) {
        this.tNumListCFG.chOpacity = 255;
    }
    
    /* update temp item */
    memset(&this.tTempItem, 0, sizeof(this.tTempItem));
    
    this.tTempItem.bIsEnabled = true;
    this.tTempItem.bIsVisible = true;
    this.tTempItem.u4Alignment = ARM_2D_ALIGN_CENTRE;
    
    this.tTempItem.Padding.chPrevious = ptCFG->chPrviousePadding;
    this.tTempItem.Padding.chNext = ptCFG->chNextPadding;
    this.tTempItem.tSize = ptCFG->tItemSize;

    this.tTempItem.fnOnDrawItem = &__arm_2d_number_list_draw_list_core_item;
    
    /* request updating StartOffset */
    //this.use_as____arm_2d_list_core_t.CalMidAligned.bListHeightChanged = true;
}

ARM_NONNULL(1,2)
arm_fsm_rt_t number_list_show(  number_list_t *ptThis,
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
void numer_list_move_selection( number_list_t *ptThis,
                                        int16_t iSteps,
                                        int32_t nFinishInMs)
{
    __arm_2d_list_core_move_request( &this.use_as____arm_2d_list_core_t,
                                             iSteps,
                                             nFinishInMs);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
