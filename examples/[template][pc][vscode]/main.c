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
#include "arm_2d_scenes.h"
#include "arm_2d_disp_adapters.h"

#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
#   include "arm_2d_benchmark.h"
#endif

#include "arm_2d_scene_meter.h"
#include "arm_2d_scene_watch.h"
#include "arm_2d_scene_fitness.h"
#include "arm_2d_scene_alarm_clock.h"
#include "arm_2d_scene_histogram.h"
#include "arm_2d_scene_fan.h"
#include "arm_2d_scene_bubble_charging.h"
#include "arm_2d_scene_animate_background.h"
#include "arm_2d_scene_knob.h"

#include "arm_2d_demos.h"

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

void scene_meter_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 500);

    arm_2d_scene_meter_init(&DISP0_ADAPTER);
}

void scene_watch_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 500);

    arm_2d_scene_watch_init(&DISP0_ADAPTER);
}

void scene_watch_face_01_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_FLY_IN_FROM_RIGHT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 500);

    arm_2d_scene_watch_face_01_init(&DISP0_ADAPTER);
}

void scene_fitness_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 500);
    arm_2d_scene_fitness_init(&DISP0_ADAPTER);
}

void scene_alarm_clock_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 500);
    arm_2d_scene_alarm_clock_init(&DISP0_ADAPTER);
}

void scene_histogram_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_ERASE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 1000);
    arm_2d_scene_histogram_init(&DISP0_ADAPTER);
}

void scene_audiomark_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 500);

    arm_2d_scene_audiomark_init(&DISP0_ADAPTER);
}

void scene_atom_loader(void) 
{
    arm_2d_scene_atom_init(&DISP0_ADAPTER);
}

void scene_pave_loader(void) 
{
    arm_2d_scene_pave_init(&DISP0_ADAPTER);
}

void scene_balls_loader(void) 
{
    arm_2d_scene_balls_init(&DISP0_ADAPTER);
}

void scene_basics_loader(void) 
{
    arm_2d_scene_basics_init(&DISP0_ADAPTER);
}

void scene_progress_status_loader(void) 
{
    arm_2d_scene_progress_status_init(&DISP0_ADAPTER);
}

void scene_panel_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 500);

    arm_2d_scene_panel_init(&DISP0_ADAPTER);
}

void scene_gas_gauge_loader(void) 
{
    arm_2d_scene_gas_gauge_init(&DISP0_ADAPTER);
}

void scene_listview_loader(void) 
{
    arm_2d_scene_listview_init(&DISP0_ADAPTER);
}

void scene_menu_loader(void) 
{
    arm_2d_scene_menu_init(&DISP0_ADAPTER);
}

void scene_fan_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 500);

    arm_2d_scene_fan_init(&DISP0_ADAPTER);
}

void scene_console_window_loader(void)
{
    arm_2d_scene_console_window_init(&DISP0_ADAPTER);
}

void scene_text_reader_loader(void)
{
    arm_2d_scene_text_reader_init(&DISP0_ADAPTER);
}

void scene_bubble_charging_loader(void) 
{
    arm_2d_scene_bubble_charging_init(&DISP0_ADAPTER);
}

void scene_ruler_loader(void) 
{
    arm_2d_scene_ruler_init(&DISP0_ADAPTER);
}

void scene_hollow_out_list_loader(void) 
{
    arm_2d_scene_hollow_out_list_init(&DISP0_ADAPTER);
}

void scene_transform_loader(void) 
{
    arm_2d_scene_transform_init(&DISP0_ADAPTER);
}

void scene_radars_loader(void) 
{
    arm_2d_scene_radars_init(&DISP0_ADAPTER);
}

void scene_blink_loader(void) 
{
    arm_2d_scene_blink_init(&DISP0_ADAPTER);
}

void scene_flight_attitude_instrument_loader(void) 
{
    arm_2d_scene_flight_attitude_instrument_init(&DISP0_ADAPTER);
}

void scene_shaped_panel_loader(void) 
{
    arm_2d_scene_shaped_panel_init(&DISP0_ADAPTER);
}

void scene_font_loader(void) 
{
    arm_2d_scene_font_init(&DISP0_ADAPTER);
}

void scene_qrcode_loader(void) 
{
    arm_2d_scene_qrcode_init(&DISP0_ADAPTER);
}

void scene_filters_loader(void) 
{
    arm_2d_scene_filters_init(&DISP0_ADAPTER);
}

void scene_compass_loader(void) 
{
    arm_2d_scene_compass_init(&DISP0_ADAPTER);
}

void scene_knob_loader(void) 
{
    arm_2d_scene_knob_init(&DISP0_ADAPTER);
}

void scene_matrix_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_FADE_BLACK);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 2000);

    arm_2d_scene_matrix_init(&DISP0_ADAPTER);
}

void scene_iir_blur_loader(void) 
{
    arm_2d_scene_iir_blur_init(&DISP0_ADAPTER);
}

void scene_music_player_loader(void) 
{
    arm_2d_scene_music_player_init(&DISP0_ADAPTER);
}

#if __GLCD_CFG_COLOUR_DEPTH__ == 16
void scene_user_defined_opcode_loader(void) 
{
    arm_2d_scene_user_defined_opcode_init(&DISP0_ADAPTER);
}

void scene_space_badge_loader(void) 
{
    arm_2d_scene_space_badge_init(&DISP0_ADAPTER);
}
#endif

void scene_mono_loading_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_loading_init(&DISP0_ADAPTER);
}

void scene_mono_histogram_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_histogram_init(&DISP0_ADAPTER);
}

void scene_mono_clock_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_clock_init(&DISP0_ADAPTER);
}

void scene_mono_list_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_list_init(&DISP0_ADAPTER);
}

void scene_mono_tracking_list_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_tracking_list_init(&DISP0_ADAPTER);
}

void scene_mono_icon_menu_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_icon_menu_init(&DISP0_ADAPTER);
}

#if defined(RTE_Acceleration_Arm_2D_Extra_TJpgDec_Loader)
void scene_tjpgd_loader(void) 
{
    arm_2d_scene_tjpgd_init(&DISP0_ADAPTER);
}

void scene_rickrolling_loader(void) 
{
    arm_2d_scene_rickrolling_init(&DISP0_ADAPTER);
}
#endif

#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__
void scene_animate_background_loader(void) 
{
    arm_2d_scene_animate_background_init(&DISP0_ADAPTER);
}

void scene_virtual_resource_loader(void) 
{
    arm_2d_scene_virtual_resource_init(&DISP0_ADAPTER);
}
#endif

typedef struct demo_scene_t {
    int32_t nLastInMS;
    void (*fnLoader)(void);
} demo_scene_t;

static demo_scene_t const c_SceneLoaders[] = {

#if 1

#if defined(__DISP0_CFG_COLOR_SOLUTION__) && __DISP0_CFG_COLOR_SOLUTION__ == 1
    {
        13000,
        scene_mono_loading_loader,
    },
    {
        5000,
        scene_mono_histogram_loader,
    },
    {
        5000,
        scene_mono_clock_loader,
    },
    {
        7000,
        scene_mono_list_loader,
    },
    {
        15000,
        scene_mono_tracking_list_loader,
    },
    {
        12000,
        scene_mono_icon_menu_loader,
    }
#else
    {
        3000,
        scene_basics_loader,
    },
    {
        10000,
        scene_progress_status_loader,
    },
    {
        20000,
        scene_matrix_loader,
    },
    {
        13000,
        scene_fan_loader,
    },
    {
        10000,
        scene_console_window_loader,
    },
    {
        20000,
        scene_text_reader_loader,
    },
    {
        30000,
        scene_music_player_loader,
    },
    {
        15000,
        scene_meter_loader,
    },
    {
        30000,
        scene_watch_face_01_loader,
    },
    {
        30000,
        scene_watch_loader,
    },
    {
        20000,
        scene_compass_loader,
    },
    {
        10000,
        scene_knob_loader,
    },
    {
        5000,
        scene_qrcode_loader,
    },
    {
        5000,
        scene_pave_loader,
    },
#if defined(RTE_Acceleration_Arm_2D_Extra_TJpgDec_Loader)
    {
        5000,
        scene_tjpgd_loader,
    },
    {
        8000,
        scene_rickrolling_loader,
    },
#endif
    {
        10000,
        scene_alarm_clock_loader,
    },
    {
        5000,
        scene_atom_loader,
    },
    {
        10000,
        scene_histogram_loader,
    },
    {
        20000,
        scene_iir_blur_loader,
    },
    {
        30000,
        scene_bubble_charging_loader,
    },
    {
        29000,
        scene_gas_gauge_loader,
    },
    {
        12000,
        scene_listview_loader,
    },
    {
        12000,
        scene_menu_loader,
    },
    {
        10000,
        scene_ruler_loader,
    },

    {
        10000,
        scene_hollow_out_list_loader,
    },
    {
        20000,
        scene_panel_loader,
    },
    {
        20000,
        scene_fitness_loader,
    },
    {
        15000,
        scene_transform_loader,
    },
    {
        15000,
        scene_filters_loader,
    },
#if __GLCD_CFG_COLOUR_DEPTH__ == 16
    {
        28000,
        scene_user_defined_opcode_loader,
    },
    {
        20000,
        scene_space_badge_loader,
    },
#endif
    {
        43000,
        scene_radars_loader,
    },
    {
        20000,
        scene_blink_loader,
    },
    {
        10000,
        scene_audiomark_loader,
    },
    
#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__
    {
        3000,
        scene_virtual_resource_loader,
    },
    {
        5000,
        scene_animate_background_loader,
    },
#endif

#endif

#else
    {
        .fnLoader = 
        scene_shaped_panel_loader,
        //scene_flight_attitude_instrument_loader,
        //scene_blink_loader,
        //scene_rickrolling_loader,
        //scene_histogram_loader,
        //scene_space_badge_loader,
        //scene_pave_loader,
        //scene_qrcode_loader,
        //scene_font_loader,
        //scene_music_player_loader,
        //scene_console_window_loader
        //scene_balls_loader,
        //scene_iir_blur_loader,
        //scene_progress_status_loader,
        //scene_matrix_loader,
        //scene_tjpgd_loader,
        //scene_rickrolling_loader,
        //scene_fan_loader,
        //scene_transform_loader,
        //scene_tjpgd_loader,
        //scene_text_reader_loader,
        //scene_ruler_loader,
        //scene_filters_loader,
        //scene_listview_loader,
        //scene_mono_tracking_list_loader
        //scene_mono_list_loader,
        //scene_gas_gauge_loader,
        //scene_meter_loader,
        //scene_compass_loader,
        //scene_basics_loader,
        //scene_fitness_loader,
        //scene_user_defined_opcode_loader,
        //scene_knob_loader,
        //scene_panel_loader,
    },
#endif

};

static
struct {
    int8_t chIndex;
    bool bIsTimeout;
    int32_t nDelay;
    int64_t lTimeStamp;
    
} s_tDemoCTRL = {
    .chIndex = -1,
    .bIsTimeout = true,
};

/* load scene one by one */
void before_scene_switching_handler(void *pTarget,
                                    arm_2d_scene_player_t *ptPlayer,
                                    arm_2d_scene_t *ptScene)
{

    switch (arm_2d_scene_player_get_switching_status(&DISP0_ADAPTER)) {
        case ARM_2D_SCENE_SWITCH_STATUS_MANUAL_CANCEL:
            s_tDemoCTRL.chIndex--;
            break;
        default:
            s_tDemoCTRL.chIndex++;
            break;
    }

    if (s_tDemoCTRL.chIndex >= dimof(c_SceneLoaders)) {
        s_tDemoCTRL.chIndex = 0;
    } else if (s_tDemoCTRL.chIndex < 0) {
        s_tDemoCTRL.chIndex += dimof(c_SceneLoaders);
    }
    
    /* call loader */
    arm_with(const demo_scene_t, &c_SceneLoaders[s_tDemoCTRL.chIndex]) {
        if (_->nLastInMS > 0) {
            s_tDemoCTRL.bIsTimeout = false;
            s_tDemoCTRL.lTimeStamp = 0;
            s_tDemoCTRL.nDelay = _->nLastInMS;
        }
        _->fnLoader();
    }
}

#if __DISP0_CFG_SCEEN_WIDTH__ == 480                                            \
 && __DISP0_CFG_SCEEN_HEIGHT__ == 480

extern const arm_2d_tile_t c_tileWatchCoverRoundGRAY8;
extern const arm_2d_tile_t c_tileGlassHaloMask;

__OVERRIDE_WEAK
IMPL_PFB_ON_DRAW(__disp_adapter0_user_draw_navigation)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(bIsNewFrame);

    arm_2d_canvas(ptTile, __watch_cover_canvas) {

        arm_2d_align_centre(__watch_cover_canvas, 
                            c_tileWatchCoverRoundGRAY8.tRegion.tSize) {
            
            arm_2d_fill_colour_with_mask(
                                        ptTile,
                                        &__centre_region,
                                        &c_tileWatchCoverRoundGRAY8, 
                                        (__arm_2d_color_t){GLCD_COLOR_BLACK});
        }

        arm_2d_align_centre(__watch_cover_canvas, 
                            c_tileGlassHaloMask.tRegion.tSize) {
            arm_2d_fill_colour_with_mask(
                                        ptTile,
                                        &__centre_region,
                                        &c_tileGlassHaloMask, 
                                        (__arm_2d_color_t){GLCD_COLOR_LIGHT_GREY});
        }
    }

    return arm_fsm_rt_cpl;
}
#endif

void run_os_command(const char *pchCommandLine) {
    static char s_chBuffer[4096];
    FILE *ptPipe = popen(pchCommandLine, "r");
    if (!ptPipe) {
        printf("Failed to execute command: [%s]\r\n", pchCommandLine);
        return;
    }

    while (fgets(s_chBuffer, sizeof(s_chBuffer), ptPipe) != NULL) {
        printf("%s", s_chBuffer);
    }

    pclose(ptPipe);
}


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
#if 0
    /* example code for nano-drawing in blocking mode */
    do {
        arm_2d_scene_t *ptScene = disp_adapter0_nano_prepare();

        /* change canvas colour */
        ptScene->tCanvas.wColour = GLCD_COLOR_GREEN;

        disp_adapter_nano_draw_example_blocking_version();

        /* delay 1s to make the frame visible, 
         * NOTE: You don't have to keep this delay in your application
         */
        SDL_Delay(1000);
    } while(0);

    /* example code for nano-drawing in non-blocking mode  */
    do {
        arm_2d_scene_t *ptScene = disp_adapter0_nano_prepare();

        /* change canvas colour */
        ptScene->tCanvas.wColour = GLCD_COLOR_BLUE;
        while(arm_fsm_rt_cpl != disp_adapter_nano_draw_example_non_blocking_version());

        /* delay 1s to make the frame visible, 
         * NOTE: You don't have to keep this delay in your application
         */
        SDL_Delay(1000);
    } while(0);
#endif

#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
    arm_2d_run_benchmark();
#else
    arm_2d_scene_player_register_before_switching_event_handler(
            &DISP0_ADAPTER,
            before_scene_switching_handler);

#if 1
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE);

    #if 0
        /*
         * Fly-In with Blur background.
         */
        arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                                ARM_2D_SCENE_SWITCH_MODE_FLY_IN_FROM_RIGHT,
                                                ARM_2D_SCENE_SWITCH_CFG_BG_BLUR);
    #endif

    arm_2d_scene_player_set_auto_switching_period(&DISP0_ADAPTER, 3000);
#else
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_FLY_IN_FROM_RIGHT);
    arm_2d_scene_player_set_auto_switching_period(&DISP0_ADAPTER, -1);
#endif

    arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
#endif

    while(1) {
        if (VT_is_request_quit()) {
            break;
        }

        /* if you ONLY use the NANO-Drawing mode, please remove the disp_adapter0_task() */
        disp_adapter0_task(60);

        if (!s_tDemoCTRL.bIsTimeout) {

            if (arm_2d_helper_is_time_out(s_tDemoCTRL.nDelay, &s_tDemoCTRL.lTimeStamp)) {
                s_tDemoCTRL.bIsTimeout = true;

                arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
            }
        }

        if (arm_2d_scene_player_is_switching(&DISP0_ADAPTER)) {
            switch (arm_2d_scene_player_get_switching_status(&DISP0_ADAPTER)) {
                case ARM_2D_SCENE_SWITCH_STATUS_MANUAL: {
                    arm_2d_location_t tPointerLocation;
                    static arm_2d_location_t s_tLastLocation = {0};
                    static bool s_bTouchDown = false;

                    if (VT_mouse_get_location(&tPointerLocation)) {
                        /* mouse down */
                        arm_2d_scene_player_set_manual_switching_offset(&DISP0_ADAPTER, tPointerLocation);
                        s_tLastLocation = tPointerLocation;
                        s_bTouchDown = true;
                    } else {
                        #if 1
                        if (s_bTouchDown) {
                            /* touch up */
                            arm_2d_scene_player_finish_manual_switching(&DISP0_ADAPTER, 
                                                                        false,
                                                                        500);
                            s_tLastLocation.iX = 0;
                            s_tLastLocation.iY = 0;
                        }
                        
                        s_bTouchDown = false;
                        #endif
                    }

                    break;
                }
                default:
                    break;
            }
        }
    }

    return 0;
}

static bool __lcd_sync_handler(void *pTarget)
{
    return VT_sdl_vsync();
}

int main(int argc, char* argv[])
{
    VT_init();

    printf("\r\nArm-2D PC Template\r\n");

    arm_irq_safe {
        arm_2d_init();
    }

    disp_adapter0_init();

    //arm_2d_helper_pfb_anti_noise_scan_mode(&DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t, true);

    /* register a low level sync-up handler to wait LCD finish rendering the previous frame */
    do {
        arm_2d_helper_pfb_dependency_t tDependency = {
            .evtOnLowLevelSyncUp = {
                .fnHandler = &__lcd_sync_handler,
            },
        };
        arm_2d_helper_pfb_update_dependency(&DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t, 
                                            ARM_2D_PFB_DEPEND_ON_LOW_LEVEL_SYNC_UP,
                                            &tDependency);
    } while(0);

    SDL_CreateThread(app_2d_main_thread, "arm-2d thread", NULL);

    while (1) {
        if(!VT_sdl_refresh_task()){
            break;
        }
    }

    VT_deinit();
    return 0;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
