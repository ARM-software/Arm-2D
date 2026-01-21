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
#include <stdio.h>
#include "Virtual_TFT_Port.h"
#include "arm_2d_helper.h"
#include "arm_2d_disp_adapters.h"

#include "arm_2d_example_controls.h"
#include "arm_2d_example_loaders.h"

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
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
#if __DISP0_CFG_NANO_ONLY__

void disp_adapter_nano_draw_example_blocking_version(void)
{
    /* on frame start */
    do {

    } while(0);

    DISP_ADAPTER0_NANO_DRAW() {

        extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

        arm_2d_canvas(ptTile, __top_canvas) {

            arm_2d_align_centre(__top_canvas, c_tileCMSISLogoA4Mask.tRegion.tSize) {
                arm_2d_fill_colour_with_a4_mask_and_opacity(   
                                                    ptTile, 
                                                    &__centre_region, 
                                                    &c_tileCMSISLogoA4Mask, 
                                                    (__arm_2d_color_t){GLCD_COLOR_BLACK},
                                                    128);
            }
        }
    }

    /* on frame complete */
    do {

    } while(0);
}

arm_fsm_rt_t disp_adapter_nano_draw_example_non_blocking_version(void)
{
    static uint8_t s_chPT = 0;

ARM_PT_BEGIN(s_chPT)

    /* on frame start */
    do {

    } while(0);

    DISP_ADAPTER0_NANO_DRAW() {

        extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

        arm_2d_canvas(ptTile, __top_canvas) {

            arm_2d_align_centre(__top_canvas, c_tileCMSISLogoA4Mask.tRegion.tSize) {
                arm_2d_fill_colour_with_a4_mask_and_opacity(   
                                                    ptTile, 
                                                    &__centre_region, 
                                                    &c_tileCMSISLogoA4Mask, 
                                                    (__arm_2d_color_t){GLCD_COLOR_BLACK},
                                                    128);
            }
        }

        ARM_PT_YIELD(arm_fsm_rt_on_going);
    }

    /* on frame complete */
    do {

    } while(0);

ARM_PT_END()

    return arm_fsm_rt_cpl;
}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/



int app_2d_main_thread (void *argument)
{

    while(1) {
        if (VT_is_request_quit()) {
            break;
        }

        /* example code for nano-drawing in blocking mode */
        do {
            arm_2d_scene_t *ptScene = disp_adapter0_nano_prepare();

            /* change canvas colour */
            ptScene->tCanvas.wColour = GLCD_COLOR_GREEN;

            /* NOTE: 
             * 1. Please do NOT call disp_adapter0_nano_prepare() for each frame. 
             *    Usually you just need to call it once.
             * 2. You can call disp_adapter0_nano_prepare() at anytime to get 
             *    the ONLY and Default scene instance. 
             */

            /* draw one frame */
            disp_adapter_nano_draw_example_blocking_version();

            /* delay 1s to make the frame visible, 
            * NOTE: You don't have to keep this delay in your application
            */
            SDL_Delay(1000);
        } while(0);

        /* example code for nano-drawing in non-blocking mode  */
        do {
            arm_2d_scene_t *ptScene = disp_adapter0_nano_prepare();

            /* NOTE: 
             * 1. Please do NOT call disp_adapter0_nano_prepare() for each frame. 
             *    Usually you just need to call it once.
             * 2. You can call disp_adapter0_nano_prepare() at anytime to get 
             *    the ONLY and Default scene instance. 
             */

            /* change canvas colour */
            ptScene->tCanvas.wColour = GLCD_COLOR_BLUE;
            while(arm_fsm_rt_cpl != disp_adapter_nano_draw_example_non_blocking_version());

            /* delay 1s to make the frame visible, 
            * NOTE: You don't have to keep this delay in your application
            */
            SDL_Delay(1000);
        } while(0);

    }

    return 0;
}
#endif




#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
