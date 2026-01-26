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

    struct {
        uint8_t chPT;
        uint8_t chHour;
        uint8_t chMin;
        uint8_t chSec;
        uint8_t chTenMs;

    }s_tLocal, *ptThis = &s_tLocal;

ARM_PT_BEGIN(this.chPT)

    /* on frame start */
    do {
        int64_t lTimeStampInMs = arm_2d_helper_convert_ticks_to_ms(
                                    arm_2d_helper_get_system_timestamp());

        /* calculate the hours */
        do {
            uint_fast8_t chHour = lTimeStampInMs / (3600ul * 1000ul);
            chHour %= 24;
            this.chHour = chHour;

            lTimeStampInMs %= (3600ul * 1000ul);
        } while(0);

        /* calculate the Minutes */
        do {
            uint_fast8_t chMin = lTimeStampInMs / (60ul * 1000ul);

            this.chMin = chMin;

            lTimeStampInMs %= (60ul * 1000ul);
        } while(0);

        /* calculate the Seconds */
        do {
            uint_fast8_t chSec = lTimeStampInMs / (1000ul);

            this.chSec = chSec;

            lTimeStampInMs %= (1000ul);
        } while(0);

        /* calculate the Ten-Miliseconds */
        do {
            uint_fast8_t chTenMs = lTimeStampInMs / (10ul);

            this.chTenMs = chTenMs;
        } while(0);
    } while(0);

    DISP_ADAPTER0_NANO_DRAW() {
 
        extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

        arm_2d_canvas(ptTile, __top_canvas) {

            arm_lcd_text_reset_display_region_tracking();
            
            arm_lcd_text_set_draw_region(&__top_canvas);
            arm_lcd_text_set_colour(GLCD_COLOR_BLACK, GLCD_COLOR_WHITE);
            arm_lcd_printf_label(
                ARM_2D_ALIGN_CENTRE, 
                "%02d:%02d:%02d:%02d", 
                this.chHour,
                this.chMin,
                this.chSec,
                this.chTenMs);
            
            arm_2d_region_t *ptTextRegion =  arm_lcd_text_get_last_display_region();

            arm_2d_helper_dirty_region_update_item( 
                &ptScene->tDirtyRegionHelper.tDefaultItem,
                ptTile,
                &__top_canvas,
                ptTextRegion);
        }

        /* You can ONLY yield here */
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

static
arm_2d_scene_t s_tSceneClock = {
    .tCanvas.wColour = GLCD_COLOR_WHITE,
    .bUseDirtyRegionHelper = true,
};

int app_2d_main_thread (void *argument)
{

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

#if 0
    arm_2d_scene_t *ptScene = disp_adapter0_get_default_scene();
    ptScene->tCanvas.wColour = GLCD_COLOR_WHITE;
    ptScene->bUseDirtyRegionHelper = true;

    disp_adapter0_nano_prepare();
#else
    disp_adapter0_nano_prepare(&s_tSceneClock);
#endif

    while(1) {
        if (VT_is_request_quit()) {
            break;
        }

        while(arm_fsm_rt_cpl != disp_adapter_nano_draw_example_non_blocking_version());

    }

    return 0;
}
#endif




#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
