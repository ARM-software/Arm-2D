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

#include "arm_2d.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#define __USER_SCENE_BASICS_IMPLEMENT__
#include "arm_2d_scene_basics.h"

#include "arm_2d_helper.h"
#include "arm_2d_example_controls.h"

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
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe177,Pe174
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#   pragma GCC diagnostic ignored "-Wunused-value"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileCMSISLogo          c_tileCMSISLogoGRAY8
#   define c_tileBackground         c_tileBackgroundGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565
#   define c_tileBackground         c_tileBackgroundRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#   define c_tileBackground         c_tileBackgroundCCCA8888

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
extern const arm_2d_tile_t c_tileCMSISLogoMask2;

extern const arm_2d_tile_t c_tileBackground;
extern const arm_2d_tile_t c_tileCMSISLogoCCCA8888;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


static void __on_scene_basics_depose(arm_2d_scene_t *ptScene)
{
    user_scene_basics_t *ptThis = (user_scene_basics_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene 0                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_basics_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_basics_t *ptThis = (user_scene_basics_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_basics_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_basics_t *ptThis = (user_scene_basics_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_basics_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_basics_t *ptThis = (user_scene_basics_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_basics_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_basics_t *ptThis = (user_scene_basics_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(3000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static void __before_scene_basics_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_basics_t *ptThis = (user_scene_basics_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_basics_handler)
{
    user_scene_basics_t *ptThis = (user_scene_basics_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

    #if 0
        arm_2d_align_centre(__top_canvas, c_tileBackground.tRegion.tSize) {
            draw_round_corner_image(&c_tileBackground,
                                    ptTile,
                                    &__centre_region,
                                    bIsNewFrame);
        }
    #endif

        arm_2d_align_centre(__top_canvas, 240, 120 ) {
            arm_2d_layout(__centre_region, true) {

                __item_line_dock_vertical(c_tileCMSISLogoA4Mask.tRegion.tSize.iHeight, 0, 0, 0, 5) {
                    draw_round_corner_box(  ptTile, 
                                            &__item_region, 
                                            GLCD_COLOR_WHITE, 
                                            128,
                                            bIsNewFrame);
                    
                    arm_2d_op_wait_async(NULL);

                #if 1
                    /* draw the cmsis logo in the centre of the screen */
                    arm_2d_align_centre(__item_region, c_tileCMSISLogo.tRegion.tSize) {
                    #if 0
                        #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
                        arm_2d_tile_copy_with_src_mask_and_opacity_only( 
                                                        &c_tileCMSISLogoCCCA8888,
                                                        &c_tileCMSISLogoMask,
                                                        ptTile,
                                                        &__centre_region,
                                                        128);
                        #else
                        arm_2d_tile_copy_with_src_mask( &c_tileCMSISLogo,
                                                        &c_tileCMSISLogoMask,
                                                        ptTile,
                                                        &__centre_region,
                                                        ARM_2D_CP_MODE_COPY);
                        #endif
                        
                    #elif __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
                        arm_2d_tile_copy_with_opacity(  &c_tileCMSISLogoCCCA8888,
                                                        ptTile,
                                                        &__centre_region,
                                                        128);
                    #endif
                    }
                #else
                    /* draw the cmsis logo using mask in the centre of the screen */
                    arm_2d_align_centre(__item_region, c_tileCMSISLogo.tRegion.tSize) {
                    #if 1
                        arm_2d_fill_colour_with_a4_mask_and_opacity(   
                                                            ptTile, 
                                                            &__centre_region, 
                                                            &c_tileCMSISLogoA4Mask, 
                                                            (__arm_2d_color_t){GLCD_COLOR_BLACK},
                                                            128);
                    #else
                        arm_2d_fill_colour_with_mask_xy_mirror_and_opacity(   
                                                            ptTile, 
                                                            &__centre_region, 
                                                            &c_tileCMSISLogoMask2, 
                                                            (__arm_2d_color_t){GLCD_COLOR_BLACK},
                                                            128);
                    #endif
                    }
                #endif
                    arm_2d_op_wait_async(NULL);

                    draw_round_corner_border(   ptTile, 
                                                &__item_region, 
                                                GLCD_COLOR_BLACK, 
                                                (arm_2d_border_opacity_t)
                                                    {32, 32, 255-64, 255-64},
                                                (arm_2d_corner_opacity_t)
                                                    {0, 128, 128, 128});
                }

                __item_line_dock_vertical() {
                    
                    draw_round_corner_box(  ptTile, 
                                            &__item_region, 
                                            GLCD_COLOR_WHITE, 
                                            255,
                                            bIsNewFrame);
                    
                    arm_2d_op_wait_async(NULL);

                    draw_round_corner_border(   ptTile, 
                                                &__item_region, 
                                                GLCD_COLOR_BLACK, 
                                                (arm_2d_border_opacity_t)
                                                    {32, 32, 255-64, 255-64},
                                                (arm_2d_corner_opacity_t)
                                                    {0, 128, 128, 128});

                    arm_2d_dock_vertical(__item_region, 
                                        ARM_2D_FONT_A8_DIGITS_ONLY
                                            .use_as__arm_2d_user_font_t
                                                .use_as__arm_2d_font_t
                                                    .tCharSize.iHeight) {
                        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                        arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_A8_DIGITS_ONLY);
                        arm_lcd_text_set_draw_region(&__vertical_region);
                        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
                        arm_lcd_text_location(0,0);
                        
                        for (int n = 0; n < 10; n++) {
                            arm_lcd_text_set_scale(0.5f + 0.1f * (float)n);
                            arm_lcd_printf("%d", n);
                        }
                    }
                }
            }                
        }

        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene 0");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_basics_t *__arm_2d_scene_basics_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_basics_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_basics_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_basics_t),
                                                        __alignof__(user_scene_basics_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }
    memset(ptThis, 0, sizeof(user_scene_basics_t));

    *ptThis = (user_scene_basics_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_WHITE}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnScene        = &__pfb_draw_scene_basics_handler,

            //.fnOnBGStart    = &__on_scene_basics_background_start,
            //.fnOnBGComplete = &__on_scene_basics_background_complete,
            .fnOnFrameStart = &__on_scene_basics_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_basics_switching_out,
            .fnOnFrameCPL   = &__on_scene_basics_frame_complete,
            .fnDepose       = &__on_scene_basics_depose,
        },
        .bUserAllocated = bUserAllocated,
    };
    

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}




#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

