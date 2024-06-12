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

void scene_basics_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE);
    //arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 3000);

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
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_DOWN);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 500);
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
    arm_2d_scene_fan_init(&DISP0_ADAPTER);
}

void scene_console_window_loader(void)
{
    arm_2d_scene_console_window_init(&DISP0_ADAPTER);
}

void scene_bubble_charging_loader(void) 
{
    arm_2d_scene_bubble_charging_init(&DISP0_ADAPTER);
}

#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__
void scene_virtual_resource_loader(void) 
{
    arm_2d_scene_virtual_resource_init(&DISP0_ADAPTER);
}
#endif

typedef void scene_loader_t(void);

static scene_loader_t * const c_SceneLoaders[] = {

#if 1
    scene_basics_loader,
    scene_progress_status_loader,
    scene_fan_loader,
    scene_console_window_loader,
    scene_meter_loader,
    scene_alarm_clock_loader,
    scene_atom_loader,
    scene_histogram_loader,
    scene_bubble_charging_loader,
    scene_gas_gauge_loader,
    scene_listview_loader,
    scene_menu_loader,

    scene_panel_loader,
    scene_fitness_loader,
    scene_audiomark_loader,
#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__
    scene_virtual_resource_loader,
#endif

#else
    scene_basics_loader,
    scene_fan_loader,
#endif


};


/* load scene one by one */
void before_scene_switching_handler(void *pTarget,
                                    arm_2d_scene_player_t *ptPlayer,
                                    arm_2d_scene_t *ptScene)
{
    static int_fast8_t s_chIndex = -1;

    switch (arm_2d_scene_player_get_switching_status(&DISP0_ADAPTER)) {
        case ARM_2D_SCENE_SWITCH_STATUS_MANUAL_CANCEL:
            s_chIndex--;
            break;
        default:
            s_chIndex++;
            break;
    }

    if (s_chIndex >= dimof(c_SceneLoaders)) {
        s_chIndex = 0;
    } else if (s_chIndex < 0) {
        s_chIndex += dimof(c_SceneLoaders);
    }
    
    /* call loader */
    c_SceneLoaders[s_chIndex]();
    
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

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/

int app_2d_main_thread (void *argument)
{
#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
    arm_2d_run_benchmark();
#else
    arm_2d_scene_player_register_before_switching_event_handler(
            &DISP0_ADAPTER,
            before_scene_switching_handler);

    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE);
    arm_2d_scene_player_set_auto_switching_period(&DISP0_ADAPTER, 3000);
    //arm_2d_scene_player_set_manual_switching_offset(&DISP0_ADAPTER, 0);

    arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
#endif

    while(1) {
        if (arm_fsm_rt_cpl == disp_adapter0_task()) {
            VT_sdl_flush(1);
        }

        if (arm_2d_scene_player_is_switching(&DISP0_ADAPTER)) {
            switch (arm_2d_scene_player_get_switching_status(&DISP0_ADAPTER)) {
                case ARM_2D_SCENE_SWITCH_STATUS_MANUAL: {
                    arm_2d_location_t tPointerLocation;
                    static arm_2d_location_t s_tLastLocation = {0};
                    static bool s_bTouchDown = false;

                    if (VT_mouse_get_location(&tPointerLocation)) {
                        /* mouse down */
                        arm_2d_scene_player_set_manual_switching_offset(&DISP0_ADAPTER, (__DISP0_CFG_SCEEN_WIDTH__ -  tPointerLocation.iX));
                        s_tLastLocation = tPointerLocation;
                        s_bTouchDown = true;
                    } else {
                        if (s_bTouchDown) {
                            /* touch up */
                            arm_2d_scene_player_finish_manual_switching(&DISP0_ADAPTER, 
                                                                        (tPointerLocation.iX > s_tLastLocation.iX),
                                                                        3000);
                            s_tLastLocation.iX = 0;
                            s_tLastLocation.iY = 0;
                        }

                        s_bTouchDown = false;
                    }

                    break;
                }
                default:
                    break;
            }
            //
        }
    }

    return 0;
}


int main(int argc, char* argv[])
{
    VT_init();

    printf("\r\nArm-2D PC Template\r\n");

    arm_irq_safe {
        arm_2d_init();
    }

    disp_adapter0_init();

    SDL_CreateThread(app_2d_main_thread, "arm-2d thread", NULL);

    while (1) {
        VT_sdl_refresh_task();
        if(VT_is_request_quit()){
            break;
        }
    }

    VT_deinit();
    return 0;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


