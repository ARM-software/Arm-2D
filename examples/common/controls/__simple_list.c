/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
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

#define __SIMPLE_LIST_IMPLEMENT__
#define __ARM_2D_HELPER_LIST_VIEW_INHERIT__

#include "./arm_2d_example_controls.h"
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
enum {
    SIMPLE_LIST_DIRTY_REGION_START = 0,
    SIMPLE_LIST_DIRTY_REGION_REDRAW_DONE,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

 
IMPL_ON_DRAW_EVT(__arm_2d_simple_list_draw_background)
{
    ARM_2D_UNUSED(bIsNewFrame);
    
    __simple_list_t *ptThis = (__simple_list_t *)pTarget;

    arm_2d_canvas(ptTile, __canvas) {
        if (!this.tSimpleListCFG.bIgnoreBackground) {
            arm_2d_fill_colour_with_opacity(
                                ptTile, 
                                &__canvas, 
                                (__arm_2d_color_t) {this.tSimpleListCFG.tBackgroundColour},
                                this.tSimpleListCFG.chOpacity);
        }
    }
    ARM_2D_OP_WAIT_ASYNC();
    
    return arm_fsm_rt_cpl;
}

static
void __arm_2d_simple_list_draw_scrolling_bar_horizontal(
                                                __simple_list_t *ptThis, 
                                                const arm_2d_tile_t *ptTile, 
                                                arm_2d_region_t *ptCanvas, 
                                                bool bIsNewFrame)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);
    assert(NULL != ptCanvas);

    uint16_t hwListCount = __simple_list_get_list_item_count(ptThis);
    uint16_t hwSelectedID = __simple_list_get_selected_item_id(ptThis);

    arm_2d_canvas(ptTile, __list_cover) {

        if (this.tSimpleListCFG.bUseMonochromeMode) {

            arm_2d_dock_horizontal(*ptCanvas, 1, 4, 4) {

                /* draw selection indicator */
                if (bIsNewFrame) {
                    int16_t iBarHeight = __horizontal_region.tSize.iHeight;
                    q16_t q16Step = div_n_q16(reinterpret_q16_s16(iBarHeight), hwListCount);

                    int16_t iIndicatorHeight = reinterpret_s16_q16(q16Step);
                    iIndicatorHeight = MAX(2, iIndicatorHeight);

                    arm_2d_region_t tIndicatorRegion = {
                        .tSize = {
                            .iHeight = iIndicatorHeight,
                            .iWidth = 3,
                        },
                        .tLocation = {
                            .iX = __horizontal_region.tLocation.iX - 1,
                            .iY = __horizontal_region.tLocation.iY
                                + reinterpret_s16_q16(mul_n_q16(q16Step, hwSelectedID)),
                        },
                    };

                    this.tIndicatorRegion = tIndicatorRegion;
                }

                if (this.chScrollingBarOpacity > 0) {
                    /* draw bar */
                    arm_2d_fill_colour( ptTile, 
                                        &__horizontal_region, 
                                        (COLOUR_INT)this.tSimpleListCFG.ScrollingBar.wColour);

                    arm_2d_fill_colour( ptTile, 
                                        &this.tIndicatorRegion, 
                                        (COLOUR_INT)this.tSimpleListCFG.ScrollingBar.wColour);
                }

                if (this.bRedrawTheScrollingBar) {
                    this.bRedrawTheScrollingBar = false;
                    arm_2d_helper_dirty_region_update_item( &this.tDirtyRegionItem[1],
                                                            (arm_2d_tile_t *)ptTile,
                                                            &__list_cover,
                                                            ptCanvas);
                } else if (this.bSelectionChanged) {
                    arm_2d_helper_dirty_region_update_item( &this.tDirtyRegionItem[1],
                                                            (arm_2d_tile_t *)ptTile,
                                                            ptCanvas,
                                                            &this.tIndicatorRegion);
                }
            }

        } else {

            uint8_t chScrollingBarOpacity = arm_2d_helper_alpha_mix(128, this.chScrollingBarOpacity);

            arm_2d_dock_horizontal(*ptCanvas, 2, 4, 4) {
                arm_2d_fill_colour_with_opacity(ptTile, 
                                                &__horizontal_region, 
                                                (__arm_2d_color_t){this.tSimpleListCFG.ScrollingBar.wColour}, 
                                                chScrollingBarOpacity);

                /* draw selection indicator */
                if (bIsNewFrame) {
                    int16_t iBarHeight = __horizontal_region.tSize.iHeight;
                    q16_t q16Step = div_n_q16(reinterpret_q16_s16(iBarHeight), hwListCount);

                    int16_t iIndicatorHeight = reinterpret_s16_q16(q16Step);
                    iIndicatorHeight = MAX(2, iIndicatorHeight);

                    arm_2d_region_t tIndicatorRegion = {
                        .tSize = {
                            .iHeight = iIndicatorHeight,
                            .iWidth = 2,
                        },
                        .tLocation = {
                            .iX = __horizontal_region.tLocation.iX,
                            .iY = __horizontal_region.tLocation.iY
                                + reinterpret_s16_q16(mul_n_q16(q16Step, hwSelectedID)),
                        },
                    };

                    this.tIndicatorRegion = tIndicatorRegion;
                }

                arm_2d_fill_colour_with_opacity(ptTile, 
                                                &this.tIndicatorRegion, 
                                                (__arm_2d_color_t){this.tSimpleListCFG.ScrollingBar.wColour}, 
                                                chScrollingBarOpacity);

                if (this.bRedrawTheScrollingBar) {
                    this.bRedrawTheScrollingBar = false;
                    arm_2d_helper_dirty_region_update_item( 
                                                &this.tDirtyRegionItem[1],
                                                (arm_2d_tile_t *)ptTile,
                                                &__horizontal_region,
                                                ptCanvas);
                } else if (this.bSelectionChanged) {
                    arm_2d_helper_dirty_region_update_item( 
                                                &this.tDirtyRegionItem[1],
                                                (arm_2d_tile_t *)ptTile,
                                                &__horizontal_region,
                                                &this.tIndicatorRegion);
                }

            }
        }

    }
}

static 
IMPL_PFB_ON_DRAW(__arm_2d_simple_list_draw_cover_vertical)
{
    ARM_2D_UNUSED(bIsNewFrame);
    
    __simple_list_t *ptThis = (__simple_list_t *)pTarget;

    uint16_t hwListCount = __simple_list_get_list_item_count(ptThis);
    uint16_t hwSelectedID = __simple_list_get_selected_item_id(ptThis);

    arm_2d_canvas(ptTile, __list_cover) {

        if (this.tSimpleListCFG.bPlaceScrollingBarOnTopOrLeft) {
            arm_2d_dock_left(__list_cover, 3) {
                __arm_2d_simple_list_draw_scrolling_bar_horizontal( 
                                                                ptThis,
                                                                ptTile,
                                                                &__left_region,
                                                                bIsNewFrame);
            }
        } else {
            arm_2d_dock_right(__list_cover, 3) {
                __arm_2d_simple_list_draw_scrolling_bar_horizontal( 
                                                                ptThis,
                                                                ptTile,
                                                                &__right_region,
                                                                bIsNewFrame); 
            }
        }
    }

    ARM_2D_OP_WAIT_ASYNC();
    
    return arm_fsm_rt_cpl;
}


static
void __arm_2d_simple_list_draw_scrolling_bar_vertical(
                                                __simple_list_t *ptThis, 
                                                const arm_2d_tile_t *ptTile, 
                                                arm_2d_region_t *ptCanvas, 
                                                bool bIsNewFrame)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);
    assert(NULL != ptCanvas);

    uint16_t hwListCount = __simple_list_get_list_item_count(ptThis);
    uint16_t hwSelectedID = __simple_list_get_selected_item_id(ptThis);

    arm_2d_canvas(ptTile, __list_cover) {
        if (this.tSimpleListCFG.bUseMonochromeMode) {

            arm_2d_dock_vertical(*ptCanvas, 1, (ptCanvas->tSize.iWidth >> 2)) {

                /* draw selection indicator */
                if (bIsNewFrame) {
                    int16_t iBarWidth = __vertical_region.tSize.iWidth;
                    q16_t q16Step = div_n_q16(reinterpret_q16_s16(iBarWidth), hwListCount);

                    int16_t iIndicatorWidth = reinterpret_s16_q16(q16Step);
                    iIndicatorWidth = MAX(2, iIndicatorWidth);

                    arm_2d_region_t tIndicatorRegion = {
                        .tSize = {
                            .iHeight = 3,
                            .iWidth = iIndicatorWidth,
                        },
                        .tLocation = {
                            .iY = __vertical_region.tLocation.iY - 1,
                            .iX = __vertical_region.tLocation.iX
                                + reinterpret_s16_q16(mul_n_q16(q16Step, hwSelectedID)),
                        },
                    };

                    this.tIndicatorRegion = tIndicatorRegion;
                }

                if (this.chScrollingBarOpacity > 0) {
                    /* draw bar */
                    arm_2d_fill_colour( ptTile, 
                                        &__vertical_region, 
                                        (COLOUR_INT)this.tSimpleListCFG.ScrollingBar.wColour);

                    arm_2d_fill_colour( ptTile, 
                                        &this.tIndicatorRegion, 
                                        (COLOUR_INT)this.tSimpleListCFG.ScrollingBar.wColour);
                }

                if (this.bRedrawTheScrollingBar) {
                    this.bRedrawTheScrollingBar = false;
                    arm_2d_helper_dirty_region_update_item( &this.tDirtyRegionItem[1],
                                                            (arm_2d_tile_t *)ptTile,
                                                            &__list_cover,
                                                            ptCanvas);
                } else if (this.bSelectionChanged) {
                    arm_2d_helper_dirty_region_update_item( &this.tDirtyRegionItem[1],
                                                            (arm_2d_tile_t *)ptTile,
                                                            ptCanvas,
                                                            &this.tIndicatorRegion);
                }
            }

        } else {

            uint8_t chScrollingBarOpacity = arm_2d_helper_alpha_mix(128, this.chScrollingBarOpacity);

            arm_2d_dock_vertical(*ptCanvas, 2, (ptCanvas->tSize.iWidth >> 2)) {
                arm_2d_fill_colour_with_opacity(ptTile, 
                                                &__vertical_region, 
                                                (__arm_2d_color_t){this.tSimpleListCFG.ScrollingBar.wColour}, 
                                                chScrollingBarOpacity);

                /* draw selection indicator */
                if (bIsNewFrame) {
                    int16_t iBarWidth = __vertical_region.tSize.iWidth;
                    q16_t q16Step = div_n_q16(reinterpret_q16_s16(iBarWidth), hwListCount);

                    int16_t iIndicatorWidth = reinterpret_s16_q16(q16Step);
                    iIndicatorWidth = MAX(2, iIndicatorWidth);

                    arm_2d_region_t tIndicatorRegion = {
                        .tSize = {
                            .iHeight = 2,
                            .iWidth = iIndicatorWidth,
                        },
                        .tLocation = {
                            .iY = __vertical_region.tLocation.iY - 1,
                            .iX = __vertical_region.tLocation.iX
                                + reinterpret_s16_q16(mul_n_q16(q16Step, hwSelectedID)),
                        },
                    };

                    this.tIndicatorRegion = tIndicatorRegion;
                }

                arm_2d_fill_colour_with_opacity(ptTile, 
                                                &this.tIndicatorRegion, 
                                                (__arm_2d_color_t){this.tSimpleListCFG.ScrollingBar.wColour}, 
                                                chScrollingBarOpacity);

                if (this.bRedrawTheScrollingBar) {
                    this.bRedrawTheScrollingBar = false;
                    arm_2d_helper_dirty_region_update_item( 
                                                &this.tDirtyRegionItem[1],
                                                (arm_2d_tile_t *)ptTile,
                                                &__vertical_region,
                                                ptCanvas);
                } else if (this.bSelectionChanged) {
                    arm_2d_helper_dirty_region_update_item( 
                                                &this.tDirtyRegionItem[1],
                                                (arm_2d_tile_t *)ptTile,
                                                &__vertical_region,
                                                &this.tIndicatorRegion);
                }

            }
        }
    }
        
}

static 
IMPL_PFB_ON_DRAW(__arm_2d_simple_list_draw_cover_horizontal)
{
    ARM_2D_UNUSED(bIsNewFrame);
    
    __simple_list_t *ptThis = (__simple_list_t *)pTarget;

    arm_2d_canvas(ptTile, __list_cover) {

        if (this.tSimpleListCFG.bPlaceScrollingBarOnTopOrLeft) {
            arm_2d_dock_top(__list_cover, 3) {
                __arm_2d_simple_list_draw_scrolling_bar_vertical(ptThis,
                                                                ptTile,
                                                                &__top_region,
                                                                bIsNewFrame);
            }
        } else {
            arm_2d_dock_bottom(__list_cover, 3) {
                __arm_2d_simple_list_draw_scrolling_bar_vertical(ptThis,
                                                                ptTile,
                                                                &__bottom_region,
                                                                bIsNewFrame);
            }
        }
    }

    ARM_2D_OP_WAIT_ASYNC();
    
    return arm_fsm_rt_cpl;
}


#if defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__
#define __va_list    va_list
#endif


static arm_2d_list_item_t *__arm_2d_simple_list_iterator(
                                        __arm_2d_list_core_t *ptListView,
                                        arm_2d_list_iterator_dir_t tDirection,
                                        uint_fast16_t hwID
                                    )
{
    __simple_list_t *ptThis = (__simple_list_t *)ptListView;
    int32_t hwIterationIndex;

    if (this.use_as____arm_2d_list_core_t.tCFG.bDisableRingMode) {
        uint16_t hwItemCount = this.use_as____arm_2d_list_core_t.tCFG.hwItemCount;
        switch (tDirection) {
            default:
            case __ARM_2D_LIST_GET_ITEM_WITH_ID_WITHOUT_MOVE_POINTER:
                if (hwID >= hwItemCount) {
                    /* out of range */
                    return NULL;
                }
                hwIterationIndex = hwID;
                break;

            case __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER:
                if (hwID >= hwItemCount) {
                    /* out of range */
                    return NULL;
                }

                this.hwIterationIndex = hwID;
                hwIterationIndex = this.hwIterationIndex;
                break;

            case __ARM_2D_LIST_GET_PREVIOUS:
                if (this.hwIterationIndex) {
                    this.hwIterationIndex--;
                } else {
                    /* out of range */
                    return NULL;
                }
                hwIterationIndex = this.hwIterationIndex;
                break;

            case __ARM_2D_LIST_GET_NEXT:
                if ((this.hwIterationIndex + 1) >= hwItemCount) {
                    /* out of range */
                    return NULL;
                }
                this.hwIterationIndex++;
                hwIterationIndex = this.hwIterationIndex;
                break;

            case __ARM_2D_LIST_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER:
                hwIterationIndex = 0;
                break;

            case __ARM_2D_LIST_GET_FIRST_ITEM:
                this.hwIterationIndex = 0;
                hwIterationIndex = this.hwIterationIndex;
                break;

            case __ARM_2D_LIST_GET_CURRENT:
                hwIterationIndex = this.hwIterationIndex;
                break;

            case __ARM_2D_LIST_GET_LAST_ITEM_WITHOUT_MOVE_POINTER:
                hwIterationIndex = hwItemCount - 1;
                break;

            case __ARM_2D_LIST_GET_LAST_ITEM:
                this.hwIterationIndex = hwItemCount - 1;
                hwIterationIndex = this.hwIterationIndex;
                break;
        }
    } else {
        switch (tDirection) {
            default:
            case __ARM_2D_LIST_GET_ITEM_WITH_ID_WITHOUT_MOVE_POINTER:
                hwIterationIndex = hwID;
                hwIterationIndex %= this.tSimpleListCFG.hwCount;
                break;

            case __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER:
                this.hwIterationIndex = hwID;
                this.hwIterationIndex %= this.tSimpleListCFG.hwCount;
                hwIterationIndex = this.hwIterationIndex;
                break;

            case __ARM_2D_LIST_GET_PREVIOUS:
                if (this.hwIterationIndex) {
                    this.hwIterationIndex--;
                } else {
                    this.hwIterationIndex = this.tSimpleListCFG.hwCount - 1;
                }
                hwIterationIndex = this.hwIterationIndex;
                break;

            case __ARM_2D_LIST_GET_NEXT:
                this.hwIterationIndex++;
                this.hwIterationIndex %= this.tSimpleListCFG.hwCount;
                
                hwIterationIndex = this.hwIterationIndex;
                break;

            case __ARM_2D_LIST_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER:
                hwIterationIndex = 0;
                break;

            case __ARM_2D_LIST_GET_FIRST_ITEM:
                this.hwIterationIndex = 0;
                hwIterationIndex = this.hwIterationIndex;
                break;

            case __ARM_2D_LIST_GET_CURRENT:
                hwIterationIndex = this.hwIterationIndex;
                break;

            case __ARM_2D_LIST_GET_LAST_ITEM_WITHOUT_MOVE_POINTER:
                hwIterationIndex = this.tSimpleListCFG.hwCount - 1;
                break;

            case __ARM_2D_LIST_GET_LAST_ITEM:
                this.hwIterationIndex = this.tSimpleListCFG.hwCount - 1;
                hwIterationIndex = this.hwIterationIndex;
                break;
        }
    }

    /* validate item size */
    if (this.tTempItem.tSize.iHeight <= 0) {
        this.tTempItem.tSize.iHeight = this.tSimpleListCFG.ptFont->tCharSize.iHeight;
    }
    if (this.tTempItem.tSize.iWidth <= 0) {
        this.tTempItem.tSize.iWidth 
            = this.use_as____arm_2d_list_core_t
                .Runtime.tileList.tRegion.tSize.iWidth;
    }

    hwIterationIndex %= this.tSimpleListCFG.hwCount;

    /* update item id : pretend that this is a different list core item */
    this.tTempItem.hwID = (uint16_t)hwIterationIndex;

    /* target scene */
    this.tTempItem.pTarget = (uintptr_t)this.tSimpleListCFG.ptTargetScene;

    return &this.tTempItem;
}


ARM_NONNULL(1,2)
arm_2d_err_t __simple_list_init(__simple_list_t *ptThis, 
                                __simple_list_cfg_t *ptCFG,
                                arm_2d_i_list_region_calculator_t *ptCalculator)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(ptCFG->hwCount > 0);

    int16_t iItemHeight = ptCFG->chPreviousPadding + ptCFG->chNextPadding;

    if (0 == ptCFG->tItemSize.iHeight) {
        if (NULL != ptCFG->ptFont) {
            iItemHeight += ptCFG->ptFont->tCharSize.iHeight;
        } else {
            iItemHeight += 8;
        }
    } else {
        iItemHeight += ptCFG->tItemSize.iHeight;
    }

    this.tSimpleListCFG = *ptCFG;

    /* validation */
    if (!this.tSimpleListCFG.hwCount) {
        this.tSimpleListCFG.hwCount = 1;      /* at least one item */
    }

    /* call base class contructor */
    do {
        __arm_2d_list_core_cfg_t tCFG = {
            .fnIterator = &__arm_2d_simple_list_iterator,
            
            /* vertical list, centre aligned style */
            .ptCalculator = ptCalculator,
            //.fnCalculator = &ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_FIXED_SIZED_ITEM_NO_STATUS_CHECK_VERTICAL,
            .fnOnDrawListBackground = &__arm_2d_simple_list_draw_background,
            .hwSwitchingPeriodInMs = ptCFG->hwSwitchingPeriodInMs,
            .hwItemCount = this.tSimpleListCFG.hwCount,
            .nTotalLength = this.tSimpleListCFG.hwCount * iItemHeight,
            .tListSize = ptCFG->tListSize,
            .bDisableRingMode = ptCFG->bDisableRingMode,
            
            .ptPISliderCFG = ptCFG->ptPISliderCFG,
            .bUsePISliderForSelectionIndicator = (ptCFG->ptPISliderCFG != NULL),
        };

        /* you can override the default implementations */
        if (NULL != ptCFG->fnOnDrawListBackground) {
            tCFG.fnOnDrawListBackground = ptCFG->fnOnDrawListBackground;
        }
        if (NULL != ptCFG->fnOnDrawListCover) {
            tCFG.fnOnDrawListCover = ptCFG->fnOnDrawListCover;
        } else if (ptCalculator->tDirection == ARM_2D_LIST_HORIZONTAL) {
            tCFG.fnOnDrawListCover = &__arm_2d_simple_list_draw_cover_horizontal;
        } else {
            tCFG.fnOnDrawListCover = &__arm_2d_simple_list_draw_cover_vertical;
        }
    
        if (NULL != ptCFG->fnOnDrawListItemBackground) {
            tCFG.fnOnDrawListItemBackground = ptCFG->fnOnDrawListItemBackground;
        }

        __arm_2d_list_core_init(&this.use_as____arm_2d_list_core_t, &tCFG);
    } while(0);

    
    
    if (NULL == this.tSimpleListCFG.ptFont) {
        this.tSimpleListCFG.ptFont = (arm_2d_font_t *)&ARM_2D_FONT_6x8;
    }
    
    
    if (0 == this.tSimpleListCFG.chOpacity) {
        this.tSimpleListCFG.chOpacity = 255;
    }
    
    /* update temp item */
    memset(&this.tTempItem, 0, sizeof(this.tTempItem));
    
    this.tTempItem.bIsEnabled = true;
    this.tTempItem.bIsVisible = true;
    this.tTempItem.u4Alignment = ARM_2D_ALIGN_CENTRE;
    
    this.tTempItem.Padding.chPrevious = ptCFG->chPreviousPadding;
    this.tTempItem.Padding.chNext = ptCFG->chNextPadding;
    this.tTempItem.tSize = ptCFG->tItemSize;

    if (NULL != ptCFG->fnOnDrawListItem ) {
        this.tTempItem.fnOnDrawItem = ptCFG->fnOnDrawListItem;
    }

    if (!(    (this.tSimpleListCFG.bUseDirtyRegion) 
        &&  (NULL != this.tSimpleListCFG.ptTargetScene))) {

        this.tSimpleListCFG.bUseDirtyRegion = false;
    }

    this.bIsOnLoad = true;
    this.bRedrawCurrentItem = true;

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1,2)
void __simple_list_set_draw_list_item_handler(
                                    __simple_list_t *ptThis,
                                    arm_2d_draw_list_item_handler_t *fnHandler)
{
    assert(NULL != ptThis);
    assert(NULL != fnHandler);

    this.tTempItem.fnOnDrawItem = fnHandler;
}

ARM_NONNULL(1,2)
void __simple_list_set_draw_list_background_handler(
                                    __simple_list_t *ptThis,
                                    arm_2d_helper_draw_handler_t *fnHandler)
{
    assert(NULL != ptThis);
    assert(NULL != fnHandler);

    this.use_as____arm_2d_list_core_t.tCFG.fnOnDrawListBackground = fnHandler;
}

ARM_NONNULL(1,2)
void __simple_list_set_draw_list_cover_handler(
                                    __simple_list_t *ptThis,
                                    arm_2d_helper_draw_handler_t *fnHandler)
{
    assert(NULL != ptThis);
    assert(NULL != fnHandler);

    this.use_as____arm_2d_list_core_t.tCFG.fnOnDrawListCover = fnHandler;
}


ARM_NONNULL(1)
void __simple_list_depose(__simple_list_t *ptThis)
{
    assert(NULL != ptThis);

    if (    (this.tSimpleListCFG.bUseDirtyRegion) 
        &&  (NULL != this.tSimpleListCFG.ptTargetScene)) {
        
        arm_2d_helper_dirty_region_remove_items(
                        &this.tSimpleListCFG.ptTargetScene->tDirtyRegionHelper,
                        this.tDirtyRegionItem,
                        dimof(this.tDirtyRegionItem));
    }
}

ARM_NONNULL(1)
void __simple_list_on_frame_start(__simple_list_t *ptThis)
{
    assert(NULL != ptThis);

    if (this.tSimpleListCFG.bUseDirtyRegion) {
        
        if (this.bIsOnLoad) {
            this.bIsOnLoad = false;

            arm_2d_helper_dirty_region_add_items(
                        &this.tSimpleListCFG.ptTargetScene->tDirtyRegionHelper,
                        this.tDirtyRegionItem,
                        dimof(this.tDirtyRegionItem));
            
            arm_2d_helper_dirty_region_item_force_to_use_minimal_enclosure(
                                                        &this.tDirtyRegionItem[0],
                                                        true);
        }
    }

    this.bSelectionChanged = __arm_2d_list_core_is_list_moving(
                                            &this.use_as____arm_2d_list_core_t);

    do {
        int64_t lCurrent = arm_2d_helper_get_system_timestamp();
        if (this.bSelectionChanged) {
            /* reset timer */
            this.lTimestamp = lCurrent;

            this.chScrollingBarOpacity = 255;
            this.bRedrawTheScrollingBar = true;

        } else if ((this.tSimpleListCFG.chScrollingBarAutoDisappearTimeX100Ms > 0)
            &&  (this.chScrollingBarOpacity > 0)) {

            int32_t nTimeoutMS 
                = this.tSimpleListCFG.chScrollingBarAutoDisappearTimeX100Ms * 100ul;
            int32_t lElapsedMS = arm_2d_helper_convert_ticks_to_ms(
                                                lCurrent - this.lTimestamp);

            if (lElapsedMS > nTimeoutMS) {
                if (this.tSimpleListCFG.bUseMonochromeMode) {
                    this.chScrollingBarOpacity = 0;
                    this.bRedrawTheScrollingBar = true;
                } else {
                    int32_t nOpacity = 255 - ((int32_t)(lElapsedMS - nTimeoutMS) >> 1);

                    if (nOpacity > 0) {
                        this.chScrollingBarOpacity = (uint8_t)nOpacity;
                        this.bRedrawTheScrollingBar = true;
                    } else {
                        this.chScrollingBarOpacity = 0;
                        this.bRedrawTheScrollingBar = true;
                    }
                }
            }
        }
    } while(0);
}

ARM_NONNULL(1,2)
arm_fsm_rt_t __simple_list_show(  __simple_list_t *ptThis,
                                const arm_2d_tile_t *ptTile, 
                                const arm_2d_region_t *ptRegion, 
                                bool bIsNewFrame)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);

    arm_fsm_rt_t tResult = arm_fsm_rt_on_going;

    if (!arm_2d_helper_pfb_is_region_active(ptTile, ptRegion, true)) {
        return arm_fsm_rt_cpl;
    }

    __arm_2d_list_core_t *ptList = &this.use_as____arm_2d_list_core_t;

    tResult =  __arm_2d_list_core_show(ptList,
                            ptTile,
                            ptRegion,
                            bIsNewFrame);

    if (bIsNewFrame) {
        /* process the dynamic dirty region */
        if (this.tSimpleListCFG.bUseDirtyRegion) {

            bool bRedrawList = false;
            bool bRedrawCurrentItem = false;

            arm_irq_safe {
                bRedrawList = this.bRedrawListReq;
                bRedrawCurrentItem = this.bRedrawCurrentItem;

                this.bRedrawListReq = false;
                this.bRedrawCurrentItem = false;
            }
            
            if (    __arm_2d_list_core_need_redraw(ptList, true)
                ||  bRedrawList
                ||  bRedrawCurrentItem) {

                arm_2d_tile_t *ptTargetTile 
                            = __arm_2d_list_core_get_inner_tile(
                                                &this.use_as____arm_2d_list_core_t);

                arm_2d_region_t tRedrawRegion = {
                    .tSize = ptTargetTile->tRegion.tSize,
                };

                if (    (   !__arm_2d_list_core_is_list_scrolling(&this.use_as____arm_2d_list_core_t)
                        ||  bRedrawCurrentItem)
                  &&    !bRedrawList) {
                    __arm_2d_list_core_get_selection_region(
                        &this.use_as____arm_2d_list_core_t, 
                        &tRedrawRegion);
                }

                arm_2d_helper_dirty_region_update_item( &this.tDirtyRegionItem[0],
                                                        (arm_2d_tile_t *)ptTargetTile,
                                                        NULL,
                                                        &tRedrawRegion);
                
            }

        }
    }

    return tResult;
    
}

ARM_NONNULL(1,2,4)
int __simple_list_item_printf(  __simple_list_t *ptThis, 
                                const arm_2d_region_t *ptRegion,
                                arm_2d_align_t tAlignment, 
                                const char *format, 
                                ...)
{
    assert(NULL != ptThis);
    assert(NULL != ptRegion);
    assert(NULL != format);

    int real_size;
    static char s_chBuffer[__LCD_PRINTF_CFG_TEXT_BUFFER_SIZE__ + 1];
    __va_list ap;
    va_start(ap, format);
        real_size = vsnprintf(s_chBuffer, sizeof(s_chBuffer)-1, format, ap);
    va_end(ap);
    real_size = MIN(sizeof(s_chBuffer)-1, real_size);
    s_chBuffer[real_size] = '\0';

    arm_lcd_text_set_draw_region((arm_2d_region_t *)ptRegion);
    arm_lcd_puts_label(s_chBuffer, tAlignment);
    arm_lcd_text_set_draw_region(NULL);

    return real_size;
}

ARM_NONNULL(1)
void __simple_list_move_selection(__simple_list_t *ptThis,
                                int16_t iSteps,
                                int32_t nFinishInMs)
{
    __arm_2d_list_core_move_request(&this.use_as____arm_2d_list_core_t,
                                    iSteps,
                                    nFinishInMs);
}

ARM_NONNULL(1)
uint16_t __simple_list_get_selected_item_id(__simple_list_t *ptThis)
{
    return __arm_2d_list_core_get_selected_item_id(
                                            &this.use_as____arm_2d_list_core_t);
}

ARM_NONNULL(1)
uint16_t __simple_list_get_list_item_count(__simple_list_t *ptThis)
{
    assert(NULL != ptThis);
    return this.tSimpleListCFG.hwCount;
}

ARM_NONNULL(1)
void __simple_list_request_redraw_list(__simple_list_t *ptThis) 
{
    assert(NULL != ptThis);
    arm_irq_safe {
        this.bRedrawListReq = true;
    }
}

ARM_NONNULL(1)
void __simple_list_request_redraw_selected_item(__simple_list_t *ptThis)
{
    assert(NULL != ptThis);
    arm_irq_safe {
        this.bRedrawCurrentItem = true;
    }
}



#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
