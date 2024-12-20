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

#define __USER_SCENE_FILTERS_IMPLEMENT__
#include "arm_2d_scene_filters.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include <stdlib.h>
#include <string.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion" 
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 64,177
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe177,Pe174
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#   pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileCMSISLogo          c_tileCMSISLogoGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;
/*============================ PROTOTYPES ====================================*/

static 
arm_fsm_rt_t __list_view_item_0_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam);

static 
arm_fsm_rt_t __list_view_item_1_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam);

static 
arm_fsm_rt_t __list_view_item_2_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam);

static 
arm_fsm_rt_t __list_view_item_3_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam);
/*============================ LOCAL VARIABLES ===============================*/

enum {
    LIST_ITEM_ID_REFERENCE,
    LIST_ITEM_ID_GLASS_BAR,
    LIST_ITEM_ID_REVERSE_COLOUR,
    LIST_ITEM_ID_IIR_BLUR,
};

static
arm_2d_list_item_t s_tListArray[] = {
    [LIST_ITEM_ID_REFERENCE] = {
        .hwID = LIST_ITEM_ID_REFERENCE,
        .bIsEnabled = true,
        .bIsVisible = true,
        .bIsReadOnly = true,    /* indicate this is a const item */

        .tSize = { 240, 70 },
        .fnOnDrawItem = &__list_view_item_0_draw_item,
    },

    [LIST_ITEM_ID_GLASS_BAR] = {
        .hwID = LIST_ITEM_ID_GLASS_BAR,
        .bIsEnabled = true,
        .bIsVisible = true,
        .bIsReadOnly = true,    /* indicate this is a const item */

        .tSize = { 240, 70 },
        .fnOnDrawItem = &__list_view_item_1_draw_item,
    },

    [LIST_ITEM_ID_REVERSE_COLOUR] = {
        .hwID = LIST_ITEM_ID_REVERSE_COLOUR,
        .bIsEnabled = true,
        .bIsVisible = true,
        .bIsReadOnly = true,    /* indicate this is a const item */

        .tSize = { 240, 70 },
        .fnOnDrawItem = &__list_view_item_2_draw_item,
    },

    [LIST_ITEM_ID_IIR_BLUR] = {
        .hwID = LIST_ITEM_ID_IIR_BLUR,
        .bIsEnabled = true,
        .bIsVisible = true,
        .bIsReadOnly = true,    /* indicate this is a const item */

        .tSize = { 240, 70 },
        .fnOnDrawItem = &__list_view_item_3_draw_item,
    },
};

/*============================ IMPLEMENTATION ================================*/

static 
arm_fsm_rt_t __list_view_item_0_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptItem->pTarget;
    ARM_2D_UNUSED(ptThis);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    arm_2d_canvas(ptTile, __list_item_canvas) {

        arm_2d_container(ptTile, __bar, &__list_item_canvas, 3, 3, 3, 3) {

            arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)&__bar);
            arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_6x8);
            arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);
            arm_lcd_text_set_opacity(255);
            arm_lcd_text_set_scale(1.0f);

            arm_2d_layout(__bar_canvas) {
                __item_line_dock_horizontal(64) {

                    arm_2d_fill_colour(&__bar, &__item_region, GLCD_COLOR_DARK_GREY);

                    arm_lcd_text_set_draw_region(&__item_region);
                    arm_lcd_printf_label(ARM_2D_ALIGN_CENTRE, "Reference");
                }

                __item_line_dock_horizontal() {
                    arm_2d_align_centre(__item_region, 
                                        c_tileCMSISLogo.tRegion.tSize) {
                        arm_2d_tile_copy_with_src_mask_only(
                                                    &c_tileCMSISLogo,
                                                    &c_tileCMSISLogoMask,
                                                    &__bar,
                                                    &__centre_region);
                    }
                }
            }

            arm_2d_draw_box(&__bar, &__bar_canvas, 1, GLCD_COLOR_DARK_GREY, 255);
        }

    }
    
    return arm_fsm_rt_cpl;
}

static 
arm_fsm_rt_t __list_view_item_1_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptItem->pTarget;
    ARM_2D_UNUSED(ptThis);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    arm_2d_canvas(ptTile, __list_item_canvas) {

        arm_2d_container(ptTile, __bar, &__list_item_canvas, 3, 3, 3, 3) {

            arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)&__bar);
            arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_6x8);
            arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);
            arm_lcd_text_set_opacity(255);
            arm_lcd_text_set_scale(1.0f);

            arm_2d_layout(__bar_canvas) {
                __item_line_dock_horizontal(64) {

                    arm_2d_fill_colour(&__bar, &__item_region, GLCD_COLOR_DARK_GREY);

                    arm_lcd_text_set_draw_region(&__item_region);
                    arm_lcd_printf_label(ARM_2D_ALIGN_CENTRE, "Glass Bar");
                }

                __item_line_dock_horizontal() {
                    arm_2d_align_centre(__item_region, 
                                        c_tileCMSISLogo.tRegion.tSize) {
                        arm_2d_tile_copy_with_src_mask_only(
                                                    &c_tileCMSISLogo,
                                                    &c_tileCMSISLogoMask,
                                                    &__bar,
                                                    &__centre_region);
                    }
                }
            }

            draw_glass_bar(&__bar, &__bar_canvas, 255, true);

            arm_2d_draw_box(&__bar, &__bar_canvas, 1, GLCD_COLOR_DARK_GREY, 255);
        }
    }
    
    return arm_fsm_rt_cpl;
}


static 
arm_fsm_rt_t __list_view_item_2_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptItem->pTarget;
    ARM_2D_UNUSED(ptThis);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    arm_2d_canvas(ptTile, __list_item_canvas) {

        arm_2d_container(ptTile, __bar, &__list_item_canvas, 3, 3, 3, 3) {

            arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)&__bar);
            arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_6x8);
            arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);
            arm_lcd_text_set_opacity(255);
            arm_lcd_text_set_scale(1.0f);

            arm_2d_layout(__bar_canvas) {
                __item_line_dock_horizontal(64) {

                    arm_2d_fill_colour(&__bar, &__item_region, GLCD_COLOR_DARK_GREY);

                    int16_t iLabelYOffset = (__item_region.tSize.iHeight - 16) >> 1;

                    arm_2d_dock(__item_region, 8, 8, iLabelYOffset, iLabelYOffset) {
                        arm_lcd_text_set_draw_region(&__dock_region);
                        arm_lcd_printf("Reverse Colour");
                    }
                }

                __item_line_dock_horizontal() {
                    arm_2d_align_centre(__item_region, 
                                        c_tileCMSISLogo.tRegion.tSize) {
                        arm_2d_tile_copy_with_src_mask_only(
                                                    &c_tileCMSISLogo,
                                                    &c_tileCMSISLogoMask,
                                                    &__bar,
                                                    &__centre_region);
                        
                        arm_2d_filter_reverse_colour(&__bar, &__centre_region);
                    }
                }
            }

            draw_glass_bar(&__bar, &__bar_canvas, 255, true);

            arm_2d_draw_box(&__bar, &__bar_canvas, 1, GLCD_COLOR_DARK_GREY, 255);
        }
    }
 
    return arm_fsm_rt_cpl;
}

static 
arm_fsm_rt_t __list_view_item_3_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptItem->pTarget;
    ARM_2D_UNUSED(ptThis);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    arm_2d_canvas(ptTile, __list_item_canvas) {

        arm_2d_container(ptTile, __bar, &__list_item_canvas, 3, 3, 3, 3) {

            arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)&__bar);
            arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_6x8);
            arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);
            arm_lcd_text_set_opacity(255);
            arm_lcd_text_set_scale(1.0f);

            arm_2d_layout(__bar_canvas) {
                __item_line_dock_horizontal(64) {

                    arm_2d_fill_colour(&__bar, &__item_region, GLCD_COLOR_DARK_GREY);

                    arm_lcd_text_set_draw_region(&__item_region);
                    arm_lcd_printf_label(ARM_2D_ALIGN_CENTRE, "IIR Blur");
                }

                __item_line_dock_horizontal() {
                    arm_2d_align_centre(__item_region, 
                                        c_tileCMSISLogo.tRegion.tSize) {
                        arm_2d_tile_copy_with_src_mask_only(
                                                    &c_tileCMSISLogo,
                                                    &c_tileCMSISLogoMask,
                                                    &__bar,
                                                    &__centre_region);
                    }

                    arm_2dp_filter_iir_blur(&this.tBlurOP,
                                            &__bar,
                                            &__item_region,
                                            this.chBlurDegree);

                    if (ptParam->bIsSelected 
                        && !__arm_2d_list_core_is_list_moving(
                                &this.tListView.use_as____arm_2d_list_core_t) ) {

                        arm_2d_helper_dirty_region_update_item(
                            &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
                            &__bar,
                            &__bar_canvas, 
                            &__item_region);
                    }        

                    ARM_2D_OP_WAIT_ASYNC(&this.tBlurOP);
                }
            }

            draw_glass_bar(&__bar, &__bar_canvas, 255, true);

            arm_2d_draw_box(&__bar, &__bar_canvas, 1, GLCD_COLOR_DARK_GREY, 255);
        }
    }

    return arm_fsm_rt_cpl;
}


static void __on_scene_filters_load(arm_2d_scene_t *ptScene)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __after_scene_filters_switching(arm_2d_scene_t *ptScene)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_filters_depose(arm_2d_scene_t *ptScene)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    ARM_2D_OP_DEPOSE(this.tBlurOP);

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene filters                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_filters_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_filters_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_filters_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    if (LIST_ITEM_ID_IIR_BLUR == list_view_get_selected_item_id(&this.tListView)) {
        if (arm_2d_helper_is_time_out(6000, &this.lTimestamp[1])) {
            list_view_move_selection(&this.tListView, 1, 1000);
            this.lTimestamp[1] = 0;
        }
    } else {
        if (arm_2d_helper_is_time_out(3000, &this.lTimestamp[1])) {
            list_view_move_selection(&this.tListView, 1, 1000);
            this.lTimestamp[1] = 0;
        }
    }

    if (LIST_ITEM_ID_IIR_BLUR == list_view_get_selected_item_id(&this.tListView) 
    &&  !__arm_2d_list_core_is_list_moving(&this.tListView.use_as____arm_2d_list_core_t) ) {
        int32_t nResult;
        if (arm_2d_helper_time_cos_slider(  0, 
                                            (255-16),
                                            5000,
                                            0,
                                            &nResult, 
                                            &this.lTimestamp[0])) {
        }

        this.chBlurDegree = nResult;
    } else {
        this.chBlurDegree = 0;
        this.lTimestamp[0] = 0;
    }
    
}

static void __on_scene_filters_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_2dp_filter_iir_blur_depose(&this.tBlurOP);

#if 0
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(3000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene_filters_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_filters_t *ptThis = (user_scene_filters_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_filters_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_filters_t *ptThis = (user_scene_filters_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/

        arm_2d_align_centre(__top_canvas, 240, 180) {

            while(arm_fsm_rt_cpl != list_view_show( &this.tListView, 
                                                    ptTile, 
                                                    &__centre_region, 
                                                    bIsNewFrame));

            arm_2d_dock_top(__centre_region, 60) {
                arm_2d_fill_colour_with_vertical_alpha_gradient(
                                        ptTile, 
                                        &__top_region,
                                        (__arm_2d_color_t){GLCD_COLOR_BLACK},
                                        (arm_2d_alpha_samples_2pts_t){255, 0});
            }

            arm_2d_dock_bottom(__centre_region, 60) {
                arm_2d_fill_colour_with_vertical_alpha_gradient(
                                        ptTile, 
                                        &__bottom_region,
                                        (__arm_2d_color_t){GLCD_COLOR_BLACK},
                                        (arm_2d_alpha_samples_2pts_t){0, 255});
            }

            /* update dirty regions */
            if (__arm_2d_list_core_is_list_scrolling(
                    &this.tListView.use_as____arm_2d_list_core_t)) {
                arm_2d_helper_dirty_region_update_item(
                    &this.use_as__arm_2d_scene_t
                            .tDirtyRegionHelper.tDefaultItem,
                    ptTile,
                    &__top_canvas, 
                    &__centre_region);

            }

        }

        arm_2d_align_centre(__top_canvas, 260, 200) {

            draw_round_corner_border(   ptTile, 
                                        &__centre_region, 
                                        GLCD_COLOR_WHITE, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});

        }

        

    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_filters_t *__arm_2d_scene_filters_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_filters_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_filters_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_filters_t),
                                                        __alignof__(user_scene_filters_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_filters_t));

    *ptThis = (user_scene_filters_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_filters_load,
            .fnScene        = &__pfb_draw_scene_filters_handler,
            //.fnAfterSwitch  = &__after_scene_filters_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_filters_background_start,
            //.fnOnBGComplete = &__on_scene_filters_background_complete,
            .fnOnFrameStart = &__on_scene_filters_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_filters_switching_out,
            .fnOnFrameCPL   = &__on_scene_filters_frame_complete,
            .fnDepose       = &__on_scene_filters_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_filters_t begin ---------------*/
    ARM_2D_OP_INIT(this.tBlurOP);

    do {
        static const arm_2d_helper_pi_slider_cfg_t c_tPIHelperCFG = {
            .fProportion = 0.040f,
            .fIntegration = 0.00300f,
            .nInterval = 10,
        };
        list_view_cfg_t tCFG = {
            .fnIterator = &ARM_2D_LIST_ITERATOR_ARRAY,
            .fnCalculator = &ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_FIXED_SIZED_ITEM_NO_STATUS_CHECK_VERTICAL,
            
            .tListSize = {
                0, 240
            },
            .bUsePIMode = true,
            .ptPISliderCFG = (arm_2d_helper_pi_slider_cfg_t *)&c_tPIHelperCFG,

            .ptItems = (arm_2d_list_item_t *)s_tListArray,
            .hwCount = dimof(s_tListArray),
            .hwItemSizeInByte = sizeof(arm_2d_list_item_t),
            
            //.bIgnoreBackground = true,
            //.bDisableRingMode = true,
            .tBackground.tColour = GLCD_COLOR_WHITE,
        };
        
        list_view_init(&this.tListView, &tCFG);
        
        arm_foreach(arm_2d_list_item_t, s_tListArray, ptItem) {
            ptItem->pTarget = (uintptr_t)ptThis;
        }
    } while(0);

    /* ------------   initialize members of user_scene_filters_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif
