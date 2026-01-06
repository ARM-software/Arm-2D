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

#define __USER_SCENE_IIR_BLUR_IMPLEMENT__
#include "arm_2d_scene_iir_blur.h"

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

static void __on_scene_iir_blur_load(arm_2d_scene_t *ptScene)
{
    user_scene_iir_blur_t *ptThis = (user_scene_iir_blur_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_2d_helper_dirty_region_add_items(
                                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                this.tDirtyRegionItems,
                                dimof(this.tDirtyRegionItems));
}

static void __after_scene_iir_blur_switching(arm_2d_scene_t *ptScene)
{
    user_scene_iir_blur_t *ptThis = (user_scene_iir_blur_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_iir_blur_depose(arm_2d_scene_t *ptScene)
{
    user_scene_iir_blur_t *ptThis = (user_scene_iir_blur_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ARM_2D_OP_DEPOSE(this.tBlurOP);

    arm_2d_helper_dirty_region_remove_items(
                                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                this.tDirtyRegionItems,
                                dimof(this.tDirtyRegionItems));

    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }
    ptScene->ptPlayer = NULL;
    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene iir_blur                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_iir_blur_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_iir_blur_t *ptThis = (user_scene_iir_blur_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_iir_blur_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_iir_blur_t *ptThis = (user_scene_iir_blur_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_iir_blur_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_iir_blur_t *ptThis = (user_scene_iir_blur_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    do {
        int32_t nResult;
        arm_2d_helper_time_cos_slider(0, 255-16, 20000, 0, &nResult, &this.lTimestamp[0]);
        this.chBlurDegree = nResult;
    } while(0);
}

static void __on_scene_iir_blur_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_iir_blur_t *ptThis = (user_scene_iir_blur_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_2dp_filter_iir_blur_depose(&this.tBlurOP);

}

static void __before_scene_iir_blur_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_iir_blur_t *ptThis = (user_scene_iir_blur_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_iir_blur_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_iir_blur_t *ptThis = (user_scene_iir_blur_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/

        arm_2d_align_centre(__top_canvas, 200, 100 ) {
            

            arm_2d_layout(__centre_region) {

                __item_line_dock_vertical(20) {


                    arm_2d_fill_colour_with_opacity(ptTile, 
                        &__item_region, 
                        (__arm_2d_color_t){GLCD_COLOR_BLACK}, 
                        128);

                    arm_lcd_text_set_target_framebuffer(ptTile);
                    arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
                    
                    arm_2d_size_t tStringSize 
                        = arm_lcd_printf_to_buffer( &ARM_2D_FONT_6x8.use_as__arm_2d_font_t, 
                                                    " IIR Blur: %d", 
                                                    this.chBlurDegree);

                    arm_2d_align_mid_left(__item_region, tStringSize) {

                        arm_lcd_text_set_draw_region(&__mid_left_region);

                        /* printf first 11 leters */
                        arm_lcd_printf_buffer(11);

                        arm_lcd_text_reset_display_region_tracking();

                        /* print the numbers */
                        arm_lcd_printf_buffer(0);

                        arm_2d_helper_dirty_region_update_item( 
                            &this.tDirtyRegionItems[0],
                            (arm_2d_tile_t *)ptTile,
                            &__centre_region,
                            arm_lcd_text_get_last_display_region());
                    }
                }

                __item_line_dock_vertical() {

                    /* draw the cmsis logo in the centre of the screen */
                    arm_2d_align_centre(__item_region, c_tileCMSISLogo.tRegion.tSize) {
                        arm_2d_tile_copy_with_src_mask( &c_tileCMSISLogo,
                                                        &c_tileCMSISLogoMask,
                                                        ptTile,
                                                        &__centre_region,
                                                        ARM_2D_CP_MODE_COPY);
                    }

                    arm_2dp_filter_iir_blur(&this.tBlurOP,
                        ptTile,
                        &__item_region,
                        this.chBlurDegree);

                    arm_2d_helper_draw_box(ptTile, &__item_region, 1, GLCD_COLOR_DARK_GREY, 128);

                    arm_2d_helper_dirty_region_update_item( 
                        &this.tDirtyRegionItems[1],
                        (arm_2d_tile_t *)ptTile,
                        &__centre_region,
                        &__item_region);
                }
            }                    
        }

        /* draw text at the top-left corner */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene iir_blur");

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_iir_blur_t *__arm_2d_scene_iir_blur_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_iir_blur_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_iir_blur_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_iir_blur_t),
                                                        __alignof__(user_scene_iir_blur_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_iir_blur_t));

    *ptThis = (user_scene_iir_blur_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_WHITE}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_iir_blur_load,
            .fnScene        = &__pfb_draw_scene_iir_blur_handler,
            //.fnAfterSwitch  = &__after_scene_iir_blur_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_iir_blur_background_start,
            //.fnOnBGComplete = &__on_scene_iir_blur_background_complete,
            .fnOnFrameStart = &__on_scene_iir_blur_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_iir_blur_switching_out,
            .fnOnFrameCPL   = &__on_scene_iir_blur_frame_complete,
            .fnDepose       = &__on_scene_iir_blur_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_iir_blur_t begin ---------------*/

    ARM_2D_OP_INIT(this.tBlurOP);

    /* ------------   initialize members of user_scene_iir_blur_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


