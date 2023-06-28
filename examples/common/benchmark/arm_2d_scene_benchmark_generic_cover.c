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

#define __USER_SCENE_BENCHMARK_GENERIC_COVER_IMPLEMENT__
#include "arm_2d_scene_benchmark_generic_cover.h"

#include "arm_2d_helper.h"
#include "arm_extra_controls.h"

#include <stdlib.h>
#include <string.h>

#include "arm_2d_scene_benchmark_generic.h"

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

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern uint32_t SystemCoreClock;

/*============================ PROTOTYPES ====================================*/

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


static void __on_scene_benchmark_generic_cover_depose(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_cover_t *ptThis = (user_scene_benchmark_generic_cover_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    ptScene->ptPlayer = NULL;

    if (!this.bUserAllocated) {
        free(ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene benchmark_generic_cover                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_benchmark_generic_cover_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_cover_t *ptThis = (user_scene_benchmark_generic_cover_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_benchmark_generic_cover_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_cover_t *ptThis = (user_scene_benchmark_generic_cover_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_benchmark_generic_cover_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_cover_t *ptThis = (user_scene_benchmark_generic_cover_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_benchmark_generic_cover_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_cover_t *ptThis = (user_scene_benchmark_generic_cover_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /* switch to the benchmark scene */
    arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
}

static void __before_scene_benchmark_generic_cover_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_cover_t *ptThis = (user_scene_benchmark_generic_cover_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /* disable scene switching effect */
    arm_2d_scene_player_set_switching_mode( ptScene->ptPlayer,
                                            ARM_2D_SCENE_SWITCH_MODE_NONE);

    /* add benchmark scene */
    arm_2d_scene_benchmark_generic_init(ptScene->ptPlayer);
    
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_benchmark_generic_cover_handler)
{
    user_scene_benchmark_generic_cover_t *ptThis 
        = (user_scene_benchmark_generic_cover_t *)pTarget;
    
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/

        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_BLACK);

        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_colour(GLCD_COLOR_GREEN, GLCD_COLOR_WHITE);

        arm_print_banner("Arm-2D Benchmark Generic");
        
#if !defined(__USE_FVP__)
        do {
            arm_2d_helper_pfb_t *ptPFBHelper 
                = &this.use_as__arm_2d_scene_t.ptPlayer->use_as__arm_2d_helper_pfb_t;
            arm_2d_region_t tScreen = arm_2d_helper_pfb_get_display_area(ptPFBHelper);
            arm_2d_size_t tPFBSize = arm_2d_helper_pfb_get_pfb_size(ptPFBHelper);
            
            arm_lcd_text_location( __GLCD_CFG_SCEEN_HEIGHT__ / 8 - 7, 0);
            arm_lcd_puts(  "All-in-One Test, running "
                        ARM_TO_STRING(ITERATION_CNT)
                        " iterations\r\n");

            arm_lcd_printf( "PFB Size: %d*%d  Screen Size: %d*%d",
                            tPFBSize.iWidth,
                            tPFBSize.iHeight,
                            tScreen.tSize.iWidth, 
                            tScreen.tSize.iHeight);
            arm_lcd_printf( "\r\nCPU Freq: %dMHz\r\n", SystemCoreClock / 1000000ul);
            arm_lcd_puts( "Testing...\r\n\r\n");
        } while(0);
#endif

    /*-----------------------draw the foreground end  -----------------------*/
    }
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
static
user_scene_benchmark_generic_cover_t *
    __arm_2d_scene_benchmark_generic_cover_init(   
                                arm_2d_scene_player_t *ptDispAdapter, 
                                user_scene_benchmark_generic_cover_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_benchmark_generic_cover_t *)
                    malloc(sizeof(user_scene_benchmark_generic_cover_t));
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_benchmark_generic_cover_t));

    *ptThis = (user_scene_benchmark_generic_cover_t){
        .use_as__arm_2d_scene_t = {
            .fnScene        = &__pfb_draw_scene_benchmark_generic_cover_handler,

            //.fnOnBGStart    = &__on_scene_benchmark_generic_cover_background_start,
            //.fnOnBGComplete = &__on_scene_benchmark_generic_cover_background_complete,
            .fnOnFrameStart = &__on_scene_benchmark_generic_cover_frame_start,
            .fnBeforeSwitchOut = &__before_scene_benchmark_generic_cover_switching_out,
            .fnOnFrameCPL   = &__on_scene_benchmark_generic_cover_frame_complete,
            .fnDepose       = &__on_scene_benchmark_generic_cover_depose,
        },
        .bUserAllocated = bUserAllocated,
    };

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}

#if defined(RTE_Acceleration_Arm_2D_Extra_Benchmark_Generic)
ARM_NONNULL(1)
user_scene_benchmark_generic_cover_t * 
    __arm_2d_run_benchmark(arm_2d_scene_player_t *ptDispAdapter)
{
    assert(NULL != ptDispAdapter);
    arm_2d_scene_player_flush_fifo(ptDispAdapter);

    arm_2d_scene_player_hide_navigation_layer(ptDispAdapter);

    return __arm_2d_scene_benchmark_generic_cover_init(ptDispAdapter, NULL);
}
#endif

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


