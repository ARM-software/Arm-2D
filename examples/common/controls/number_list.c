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
IMPL_PFB_ON_DRAW(__arm_2d_number_list_draw_background)
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

//static 
//arm_fsm_rt_t __arm_2d_number_list_draw_list_view_item_background( 
//                                      arm_2d_list_view_item_t *ptThis,
//                                      const arm_2d_tile_t *ptTile,
//                                      bool bIsNewFrame,
//                                      arm_2d_list_view_item_param_t *ptParam)
//{
//    ARM_2D_UNUSED(ptThis);
//    ARM_2D_UNUSED(bIsNewFrame);
//    ARM_2D_UNUSED(ptTile);
//    ARM_2D_UNUSED(ptParam);
//    
//    return arm_fsm_rt_cpl;
//}



#if defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__
#define __va_list    va_list

#endif



static 
int __printf(const arm_2d_region_t *ptRegion, const char *format, ...)
{
    int real_size;
    static char s_chBuffer[__LCD_PRINTF_CFG_TEXT_BUFFER_SIZE__ + 1];
    __va_list ap;
    va_start(ap, format);
        real_size = vsnprintf(s_chBuffer, sizeof(s_chBuffer)-1, format, ap);
    va_end(ap);
    real_size = MIN(sizeof(s_chBuffer)-1, real_size);
    s_chBuffer[real_size] = '\0';

    arm_2d_align_centre( *ptRegion, (int16_t)real_size * 16, 24) {
        __centre_region.tLocation.iY += 2;  /* add an offset for 16x24 digits */
        
        arm_lcd_text_set_draw_region(&__centre_region);
        arm_lcd_puts(s_chBuffer);
        arm_lcd_text_set_draw_region(NULL);
    }

    return real_size;
}

static 
arm_fsm_rt_t __arm_2d_number_list_draw_list_view_item( 
                                      arm_2d_list_view_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_view_item_param_t *ptParam)
{
    number_list_t *ptThis = (number_list_t *)ptItem->ptListView;
    ARM_2D_UNUSED(ptItem);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);
    
    arm_lcd_text_set_font(&ARM_2D_FONT_16x24);
    arm_lcd_text_set_colour(this.tNumListCFG.tFontColour, this.tNumListCFG.tBackgroundColour);
    arm_lcd_text_set_display_mode(ARM_2D_DRW_PATN_MODE_COPY);
    
    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
    
    /* print numbers */
    __printf(&ptTile->tRegion,
             this.tNumListCFG.pchFormatString,
             this.tNumListCFG.nStart + ptItem->hwID * this.tNumListCFG.iDelta);
    
    arm_lcd_text_set_target_framebuffer(NULL);
    
    return arm_fsm_rt_cpl;
}

static arm_2d_list_view_item_t *__arm_2d_number_list_iterator(
                                        __arm_2d_list_view_t *ptListView,
                                        arm_2d_list_iterator_dir_t tDirection,
                                        uint_fast16_t hwID
                                    )
{
    number_list_t *ptThis = (number_list_t *)ptListView;
    int32_t nIterationIndex;
    switch (tDirection) {
        default:
        case __ARM_2D_LIST_VIEW_GET_ITEM_WITH_ID_WITHOUT_MOVE_POINTER:
            nIterationIndex = hwID;
            break;
        case __ARM_2D_LIST_VIEW_GET_ITEM_AND_MOVE_POINTER:
            this.nIterationIndex = hwID;
            nIterationIndex = this.nIterationIndex;
            break;
        case __ARM_2D_LIST_VIEW_GET_PREVIOUS:
            this.nIterationIndex--;
            nIterationIndex = this.nIterationIndex;
            break;
        case __ARM_2D_LIST_VIEW_GET_NEXT:
            this.nIterationIndex++;
            nIterationIndex = this.nIterationIndex;
            break;
        case __ARM_2D_LIST_VIEW_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER:
            nIterationIndex = 0;
            break;
        case __ARM_2D_LIST_VIEW_GET_FIRST_ITEM:
            this.nIterationIndex = 0;
            nIterationIndex = this.nIterationIndex;
            break;
        case __ARM_2D_LIST_VIEW_GET_CURRENT:
            nIterationIndex = this.nIterationIndex;
            break;
        case __ARM_2D_LIST_VIEW_GET_LAST_ITEM_WITHOUT_MOVE_POINTER:
            nIterationIndex = this.tNumListCFG.hwCount - 1;
            break;
        case __ARM_2D_LIST_VIEW_GET_LAST_ITEM:
            this.nIterationIndex = this.tNumListCFG.hwCount - 1;
            nIterationIndex = this.nIterationIndex;
            break;
    }

    /* validate item size */
    if (this.tTempItem.tSize.iHeight <= 0) {
        this.tTempItem.tSize.iHeight = 24;
    }
    if (this.tTempItem.tSize.iWidth <= 0) {
        this.tTempItem.tSize.iWidth 
            = this.use_as____arm_2d_list_view_t
                .Runtime.tileList.tRegion.tSize.iWidth;
    }

    nIterationIndex %= this.tNumListCFG.hwCount;

    /* update item id : pretend that this is a different list view item */
    this.tTempItem.hwID = (uint16_t)nIterationIndex;

    return &this.tTempItem;
}

static
__arm_2d_list_view_work_area_t *__arm_2d_number_list_region_calculator(
                                __arm_2d_list_view_t *ptThis,
                                __arm_2d_list_view_item_iterator *fnIterator,
                                int32_t nOffset
                            )
{

    arm_2d_list_view_item_t *ptItem = NULL;
    
ARM_PT_BEGIN(this.CalMidAligned.chState)

    /* update start-offset */
    if (this.CalMidAligned.bListHeightChanged) {
        this.CalMidAligned.bListHeightChanged = false;
        
         /* update the iStartOffset */
         ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER,
                        0));
        this.CalMidAligned.iStartOffset
            = (     this.Runtime.tileList.tRegion.tSize.iHeight 
                -   ptItem->tSize.iHeight) 
            >> 1;
        this.CalMidAligned.iStartOffset -= ptItem->Padding.chPrevious;
    }

    /* update total length and item count */
    if (    (0 == this.tCFG.nTotalLength)
        ||  (0 == this.tCFG.hwItemCount)) {

        uint16_t hwItemCount = 0; 
        uint32_t nTotalLength = 0;
        
        /* update the iStartOffset */
        ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM,
                        0));
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        uint16_t hwStartID = ptItem->hwID;
        
        do {
            hwItemCount++;
            nTotalLength += ptItem->tSize.iHeight 
                          + ptItem->Padding.chPrevious 
                          + ptItem->Padding.chNext;
            
            ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_NEXT,
                        0));
            if (NULL == ptItem) {
                break;
            }
            if (ptItem->hwID == hwStartID) {
                break;  /* reach the starting point again */
            }
        } while(true);
        
        this.tCFG.nTotalLength = nTotalLength;
        this.tCFG.hwItemCount = hwItemCount;
        
    } while(0);

    if (this.tCFG.nTotalLength) {
        nOffset = nOffset % this.tCFG.nTotalLength;
        nOffset -= this.tCFG.nTotalLength;
    }

    /* find the first and last visible items */
    do {
        /* move to the first item */
        ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM,
                        0));
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        /* move the nOffset to the top invisible area */
        do {
            int32_t nStartY = nOffset 
                            +   this.CalMidAligned.iStartOffset 
                            +   ptItem->Padding.chPrevious;
                    
            if (nStartY <= 0) {
                break;
            } 
            nOffset -= this.tCFG.nTotalLength;
        } while(true);
        
        /* get the inital offset */
        this.CalMidAligned.nOffset = nOffset;

        int32_t nTempOffset = nOffset;
        
        while(NULL != ptItem) {
            int32_t nY1 = nTempOffset 
                            +   this.CalMidAligned.iStartOffset 
                            +   ptItem->Padding.chPrevious;
            int32_t nY2 = nY1 + ptItem->tSize.iHeight - 1;
        
            if (nY1 >= 0 || nY2 >=0) {
                /* we find the top item */
                break;
            }
            
            /* update nTempOffset */
            nTempOffset += ptItem->tSize.iHeight 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;
            
            /* move to the next */
            ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_VIEW_GET_NEXT,
                            0));

            /* just in case the iterator won't support ring mode */
            if (NULL == ptItem) {
                ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM,
                        0));
            }
        }
        
        /* mark the first visible item on top */
        this.CalMidAligned.iTopVisiableOffset = (int16_t)nTempOffset;
        this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;

        /* mark the last visible item on bottom */
        do {
            this.CalMidAligned.iBottomVisibleOffset = (int16_t)nTempOffset;
            this.CalMidAligned.hwBottomVisibleItemID = ptItem->hwID;
            
            /* update nTempOffset */
            nTempOffset += ptItem->tSize.iHeight 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;

            /* move to the next */
            ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_VIEW_GET_NEXT,
                            0));

            /* just in case the iterator won't support ring mode */
            if (NULL == ptItem) {
                ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM,
                        0));
                /* this should not happen as we checked before */
                assert(NULL != ptItem);
            }
            
            int32_t nX1 = nTempOffset 
                            +   this.CalMidAligned.iStartOffset 
                            +   ptItem->Padding.chPrevious;
            
            if (nX1 >= this.Runtime.tileList.tRegion.tSize.iHeight) {
                /* this item is not visible */
                break;
            }
        } while(true);
    } while(0);


    /* start draw items */
    do {
        /* move to the top item */
        ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_ITEM_AND_MOVE_POINTER,
                        this.CalMidAligned.hwTopVisibleItemID));
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iTopVisiableOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )

        if (    this.CalMidAligned.hwTopVisibleItemID 
            ==  this.CalMidAligned.hwBottomVisibleItemID) {
            /* reach the centre item */
            break;
        }

        /* resume local context */
        ptItem = this.Runtime.tWorkingArea.ptItem;

        /* update top visiable item id and offset */
        do {
            this.CalMidAligned.iTopVisiableOffset += ptItem->tSize.iHeight 
                                                  + ptItem->Padding.chPrevious
                                                  + ptItem->Padding.chNext;
            
            /* move to the top item */
            ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_VIEW_GET_NEXT,
                            0));
            /* just in case the iterator won't support ring mode */
            if (NULL == ptItem) {
                ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM,
                        0));
                /* this should not happen as we checked before */
                assert(NULL != ptItem);
            }
            this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;
        } while(0);

        /* move to the bottom item */
        ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_ITEM_AND_MOVE_POINTER,
                        this.CalMidAligned.hwBottomVisibleItemID));
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iBottomVisibleOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )

        /* update bottom visiable item id and offset */
        do {
            /* move to the top item */
            ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_VIEW_GET_PREVIOUS,
                            0));
            /* just in case the iterator won't support ring mode */
            if (NULL == ptItem) {
                ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_LAST_ITEM,
                        0));
                /* this should not happen as we checked before */
                assert(NULL != ptItem);
            }

            this.CalMidAligned.hwBottomVisibleItemID = ptItem->hwID;
            
            this.CalMidAligned.iBottomVisibleOffset 
                -= ptItem->tSize.iHeight 
                 + ptItem->Padding.chPrevious
                 + ptItem->Padding.chNext;

        } while(0);
        


//        if (    this.CalMidAligned.hwTopVisibleItemID 
//            ==  this.CalMidAligned.hwBottomVisibleItemID) {
//            /* reach the centre item */
//            break;
//        }

    } while(true);

ARM_PT_END()

    return NULL;
}


ARM_NONNULL(1,2)
void number_list_init(  number_list_t *ptThis, 
                        number_list_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(ptCFG->hwCount > 0);
    static const char c_chDefaultFormatString[] = {"%d"};

    int16_t iItemHeight = 24 + ptCFG->chPrviousePadding + ptCFG->chNextPadding;

    /* call base class contructor */
    do {
        __arm_2d_list_view_cfg_t tCFG = {
            .fnIterator =                   &__arm_2d_number_list_iterator,
            .fnCalculator =                 &__arm_2d_number_list_region_calculator,
            .fnOnDrawListViewBackground =   &__arm_2d_number_list_draw_background,
            //.fnOnDrawItemBackground =       &__arm_2d_number_list_draw_list_view_item_background,
            .hwSwitchingPeriodInMs = ptCFG->hwSwitchingPeriodInMs,
            .hwItemCount = ptCFG->hwCount,
            .nTotalLength = ptCFG->hwCount * iItemHeight,
            .tListSize = ptCFG->tListSize,
        };

        /* you can override the default implementations */
        if (NULL != ptCFG->fnOnDrawListViewBackground) {
            tCFG.fnOnDrawListViewBackground = ptCFG->fnOnDrawListViewBackground;
        }
        if (NULL != ptCFG->fnOnDrawListViewCover) {
            tCFG.fnOnDrawListViewCover = ptCFG->fnOnDrawListViewCover;
        }
        if (NULL != ptCFG->fnOnDrawItemBackground) {
            tCFG.fnOnDrawItemBackground = ptCFG->fnOnDrawItemBackground;
        }

        __arm_2d_list_view_init(&this.use_as____arm_2d_list_view_t, &tCFG);
    } while(0);

    this.tNumListCFG = *ptCFG;
    
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

    this.tTempItem.fnOnDrawItem = &__arm_2d_number_list_draw_list_view_item;
    
    /* request updating StartOffset */
    this.use_as____arm_2d_list_view_t.CalMidAligned.bListHeightChanged = true;
}

ARM_NONNULL(1,2)
arm_fsm_rt_t number_list_show(  number_list_t *ptThis,
                        const arm_2d_tile_t *ptTile, 
                        const arm_2d_region_t *ptRegion, 
                        bool bIsNewFrame)
{

    return __arm_2d_list_view_show( &this.use_as____arm_2d_list_view_t,
                                    ptTile,
                                    ptRegion,
                                    bIsNewFrame);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
