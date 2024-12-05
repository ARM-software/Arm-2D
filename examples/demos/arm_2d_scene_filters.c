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
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

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

    int32_t nResult;
    if (arm_2d_helper_time_cos_slider(  0, 
                                        (255-16),
                                        5000,
                                        0,
                                        &nResult, 
                                        &this.lTimestamp[0])) {
        
    }

    this.chBlurDegree = nResult;
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

    int16_t iBarHeight = 80;
    arm_2d_size_t tDisplaySize = tScreenSize;
    tDisplaySize.iWidth = MIN(tDisplaySize.iWidth, 320);
    tDisplaySize.iWidth = MAX(tDisplaySize.iWidth, 240);

    tDisplaySize.iHeight = 240;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/

        arm_2d_align_centre(__top_canvas, tDisplaySize) {

            arm_2d_layout(__centre_region) {

                arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_6x8);
                arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);
                arm_lcd_text_set_opacity(255);
                arm_lcd_text_set_scale(1.0f);

                /* reference */
                __item_line_dock_vertical(iBarHeight) {
                    
                    arm_2d_dock(__item_region, 4) {

                        arm_2d_layout(__dock_region) {
                            __item_line_dock_horizontal(64) {

                                arm_2d_fill_colour(ptTile, &__item_region, GLCD_COLOR_DARK_GREY);

                                arm_lcd_text_set_draw_region(&__item_region);
                                arm_lcd_printf_label(ARM_2D_ALIGN_CENTRE, "Reference");
                            }

                            __item_line_dock_horizontal() {
                                arm_2d_align_centre(__item_region, 
                                                    c_tileCMSISLogo.tRegion.tSize) {
                                    arm_2d_tile_copy_with_src_mask_only(
                                                                &c_tileCMSISLogo,
                                                                &c_tileCMSISLogoMask,
                                                                ptTile,
                                                                &__centre_region);
                                }
                            }
                        }

                        draw_glass_bar(ptTile, &__dock_region, 255, true);

                        arm_2d_draw_box(ptTile, &__dock_region, 1, GLCD_COLOR_DARK_GREY, 255);
                    }

                }

                /* reverse colour */
                __item_line_dock_vertical(iBarHeight) {
                    
                    arm_2d_dock(__item_region, 4) {

                        arm_2d_layout(__dock_region) {
                            __item_line_dock_horizontal(64) {

                                arm_2d_fill_colour(ptTile, &__item_region, GLCD_COLOR_DARK_GREY);

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
                                                                ptTile,
                                                                &__centre_region);
                                    
                                    arm_2d_filter_reverse_colour(ptTile, &__centre_region);

                                }
                            }
                        }

                        draw_glass_bar(ptTile, &__dock_region, 255, true);

                        arm_2d_draw_box(ptTile, &__dock_region, 1, GLCD_COLOR_DARK_GREY, 255);
                    }
                }

                /* reference */
                __item_line_dock_vertical(iBarHeight) {
                    
                    arm_2d_dock(__item_region, 4) {

                        arm_2d_layout(__dock_region) {
                            __item_line_dock_horizontal(64) {

                                arm_2d_fill_colour(ptTile, &__item_region, GLCD_COLOR_DARK_GREY);

                                arm_lcd_text_set_draw_region(&__item_region);
                                arm_lcd_printf_label(ARM_2D_ALIGN_CENTRE, "IIR Blur");
                            }

                            __item_line_dock_horizontal() {
                                arm_2d_align_centre(__item_region, 
                                                    c_tileCMSISLogo.tRegion.tSize) {
                                    arm_2d_tile_copy_with_src_mask_only(
                                                                &c_tileCMSISLogo,
                                                                &c_tileCMSISLogoMask,
                                                                ptTile,
                                                                &__centre_region);
                                }

                                arm_2dp_filter_iir_blur(&this.tBlurOP,
                                                        ptTile,
                                                        &__item_region,
                                                        this.chBlurDegree);
                                    
                                ARM_2D_OP_WAIT_ASYNC(&this.tBlurOP);
                                
                                arm_2d_helper_dirty_region_update_item(
                                    &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
                                    ptTile,
                                    &__dock_region,
                                    &__item_region);
                            }
                        }

                        draw_glass_bar(ptTile, &__dock_region, 255, true);

                        arm_2d_draw_box(ptTile, &__dock_region, 1, GLCD_COLOR_DARK_GREY, 255);
                    }
                }


            }
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
            .tCanvas = {GLCD_COLOR_WHITE}, 

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


