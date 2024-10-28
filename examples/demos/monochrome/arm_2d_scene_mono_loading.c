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

#define __USER_SCENE_MONO_LOADING_IMPLEMENT__
#include "arm_2d_scene_mono_loading.h"

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
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __on_scene_mono_loading_load(arm_2d_scene_t *ptScene)
{
    user_scene_mono_loading_t *ptThis = (user_scene_mono_loading_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_2d_helper_dirty_region_add_items(
                            &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                            this.tDirtyRegionItems,
                            dimof(this.tDirtyRegionItems));
}

static void __after_scene_mono_loading_switching(arm_2d_scene_t *ptScene)
{
    user_scene_mono_loading_t *ptThis = (user_scene_mono_loading_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_mono_loading_depose(arm_2d_scene_t *ptScene)
{
    user_scene_mono_loading_t *ptThis = (user_scene_mono_loading_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    arm_2d_helper_dirty_region_remove_items(
                            &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                            this.tDirtyRegionItems,
                            dimof(this.tDirtyRegionItems));

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene mono_loading                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_mono_loading_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_mono_loading_t *ptThis = (user_scene_mono_loading_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_mono_loading_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_mono_loading_t *ptThis = (user_scene_mono_loading_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_mono_loading_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_mono_loading_t *ptThis = (user_scene_mono_loading_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    if (this.iProgress >= 0) {
        int32_t nResult;
        if (arm_2d_helper_time_half_cos_slider(0, 1000, 10000, &nResult, &this.lTimestamp[1])) {
            this.iProgress = -1;    /* busy ...*/
            this.lTimestamp[1] = 0;

            arm_2d_scene_player_update_scene_background(this.use_as__arm_2d_scene_t.ptPlayer);
        } else {
            this.iProgress = (int16_t)nResult;
        }
    } else {
        if (arm_2d_helper_is_time_out(333, &this.lTimestamp[1])) {
            this.chSpinStickIndex++;
        }
    }
}

static void __on_scene_mono_loading_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_mono_loading_t *ptThis = (user_scene_mono_loading_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if 1
    /* switch to next scene after 13s */
    if (arm_2d_helper_is_time_out(13000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene_mono_loading_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_mono_loading_t *ptThis = (user_scene_mono_loading_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mono_loading_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_mono_loading_t *ptThis = (user_scene_mono_loading_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {

        arm_2d_dock_vertical(__top_canvas, 10) {

            arm_2d_layout(__vertical_region) {

                __item_line_dock_vertical(8) {
                    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                    arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
                    arm_lcd_text_set_draw_region(&__item_region);
                    arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
                    
                    arm_lcd_text_reset_display_region_tracking();
                    if (this.iProgress >= 0) {
                        
                        arm_lcd_printf_label(ARM_2D_ALIGN_CENTRE, "loading...%"PRIi16"%%", this.iProgress / 10);
                        arm_2d_helper_dirty_region_update_item( &this.tDirtyRegionItems[0],
                                                                (arm_2d_tile_t *)ptTile,
                                                                &__item_region,
                                                                arm_lcd_text_get_last_display_region());
                    } else {
                        static const char c_chSpinStick[] = {"/-\\|"};
                        arm_lcd_printf_label(ARM_2D_ALIGN_CENTRE, "loading...%c", c_chSpinStick[this.chSpinStickIndex & 0x03]);

                        arm_2d_helper_dirty_region_update_item( &this.tDirtyRegionItems[0],
                                                                (arm_2d_tile_t *)ptTile,
                                                                &__item_region,
                                                                arm_lcd_text_get_last_display_region());
                    }
                }

                /* progress bar */
                __item_line_dock_vertical(1, 10, 10, 2, 2) {

                    q16_t q16Ratio = div_q16(reinterpret_q16_s16(__item_region.tSize.iWidth), reinterpret_q16_s16(1000));
                    
                    arm_2d_region_t tProgressRegion = {
                        .tLocation = __item_region.tLocation,
                        .tSize = {
                            .iHeight = 1,
                            .iWidth = reinterpret_s16_q16(mul_n_q16(q16Ratio, this.iProgress)),
                        },
                    };

                    arm_2d_fill_colour(ptTile, &tProgressRegion, GLCD_COLOR_WHITE);
                    
                    arm_2d_helper_dirty_region_update_item( &this.tDirtyRegionItems[1],
                                                            (arm_2d_tile_t *)ptTile,
                                                            &__item_region,
                                                            &tProgressRegion);

                    ARM_2D_OP_WAIT_ASYNC();
                }

            }

            /* draw progress */
            do {


                //arm_2d_fill_colour(ptTile, )
            } while(0);

        }

    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_mono_loading_t *__arm_2d_scene_mono_loading_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_mono_loading_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_mono_loading_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_mono_loading_t),
                                                        __alignof__(user_scene_mono_loading_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_mono_loading_t));

    *ptThis = (user_scene_mono_loading_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_mono_loading_load,
            .fnScene        = &__pfb_draw_scene_mono_loading_handler,
            //.fnAfterSwitch  = &__after_scene_mono_loading_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_mono_loading_background_start,
            //.fnOnBGComplete = &__on_scene_mono_loading_background_complete,
            .fnOnFrameStart = &__on_scene_mono_loading_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_mono_loading_switching_out,
            .fnOnFrameCPL   = &__on_scene_mono_loading_frame_complete,
            .fnDepose       = &__on_scene_mono_loading_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_mono_loading_t begin ---------------*/


    /* ------------   initialize members of user_scene_mono_loading_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


