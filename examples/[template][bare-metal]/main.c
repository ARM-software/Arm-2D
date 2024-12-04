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
#include "platform.h"
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

#ifndef LCD_TARGET_FPS
#   define LCD_TARGET_FPS       60
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


__OVERRIDE_WEAK 
int64_t arm_2d_helper_get_system_timestamp(void)
{
    return get_system_ticks();
}

__OVERRIDE_WEAK 
void Disp0_DrawBitmap(  int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        const uint8_t *bitmap)
{
#if __DISP0_CFG_COLOUR_DEPTH__ == 8
    extern
    void __arm_2d_impl_gray8_to_rgb565( uint8_t *__RESTRICT pchSourceBase,
                                        int16_t iSourceStride,
                                        uint16_t *__RESTRICT phwTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize);

    static uint16_t s_hwFrameBuffer[__DISP0_CFG_SCEEN_WIDTH__ * __DISP0_CFG_SCEEN_HEIGHT__];
    
    arm_2d_size_t size = {
        .iWidth = width,
        .iHeight = height,
    };
    __arm_2d_impl_gray8_to_rgb565( (uint8_t *)bitmap,
                                    width,
                                    (uint16_t *)s_hwFrameBuffer,
                                    width,
                                    &size);
    GLCD_DrawBitmap(x, y, width, height, (const uint8_t *)s_hwFrameBuffer);
#elif __DISP0_CFG_COLOUR_DEPTH__ == 32
    extern
    void __arm_2d_impl_cccn888_to_rgb565(uint32_t *__RESTRICT pwSourceBase,
                                        int16_t iSourceStride,
                                        uint16_t *__RESTRICT phwTargetBase,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize);

    arm_2d_size_t size = {
        .iWidth = width,
        .iHeight = height,
    };
    __arm_2d_impl_cccn888_to_rgb565((uint32_t *)bitmap,
                                    width,
                                    (uint16_t *)bitmap,
                                    width,
                                    &size);
    GLCD_DrawBitmap(x, y, width, height, bitmap);
#else
    GLCD_DrawBitmap(x, y, width, height, bitmap);
#endif
}

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

void scene_basics_loader(void) 
{
#if 0
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 3000);
#endif 

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

void scene_user_defined_opcode_loader(void) 
{
    arm_2d_scene_user_defined_opcode_init(&DISP0_ADAPTER);
}

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

#if 0

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
        13000,
        scene_fan_loader,
    },
    {
        10000,
        scene_console_window_loader,
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
        50000,
        scene_filters_loader,
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
        scene_filters_loader,
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


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) 
{
    arm_irq_safe {
        arm_2d_init();
    }

    printf("\r\nArm-2D Bare-metal Template\r\n");
 
    disp_adapter0_init();

#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
    arm_2d_run_benchmark();
#else
    arm_2d_scene_player_register_before_switching_event_handler(
            &DISP0_ADAPTER,
            before_scene_switching_handler);
    
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 3000);
    
    arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
#endif

    while (1) {
    #ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
        disp_adapter0_task();
    #else
        /* lock framerate */
        disp_adapter0_task(LCD_TARGET_FPS);
    #endif

        if (!s_tDemoCTRL.bIsTimeout) {
            if (arm_2d_helper_is_time_out(s_tDemoCTRL.nDelay, &s_tDemoCTRL.lTimeStamp)) {
                s_tDemoCTRL.bIsTimeout = true;

                arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
            }
        }

    }
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


