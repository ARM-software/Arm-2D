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
#include "platform.h"
#include "arm_2d_helper.h"

#include "virtual_resource_demo.h"
#include "arm_2d_scenes.h"
#include "arm_2d_disp_adapters.h"

#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
#   include "arm_2d_benchmark.h"
#endif

#include "arm_2d_scene_meter.h"
#include "arm_2d_scene_fitness.h"
#include "arm_2d_scene_alarm_clock.h"

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
#   define LCD_TARGET_FPS       30
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

void scene_audiomark_loader(void) 
{

    arm_2d_scene_audiomark_init(&DISP0_ADAPTER);
}

void scene_meter_loader(void) 
{
    arm_2d_scene_meter_init(&DISP0_ADAPTER);
}

void scene_fitness_loader(void) 
{
    arm_2d_scene_fitness_init(&DISP0_ADAPTER);
}

void scene_alarm_clock_loader(void) 
{
    arm_2d_scene_alarm_clock_init(&DISP0_ADAPTER);
}

void scene0_loader(void) 
{
    arm_2d_scene0_init(&DISP0_ADAPTER);
}

void scene1_loader(void) 
{
    arm_2d_scene1_init(&DISP0_ADAPTER);
}

#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__
void virtual_resource_demo_loader(void)
{
    virtual_resource_demo_init();
}
#endif

void scene2_loader(void) 
{
    arm_2d_scene2_init(&DISP0_ADAPTER);
}

void scene3_loader(void) 
{
    arm_2d_scene3_init(&DISP0_ADAPTER);
}

void scene4_loader(void) 
{
    arm_2d_scene4_init(&DISP0_ADAPTER);
}

typedef void scene_loader_t(void);


static scene_loader_t * const c_SceneLoaders[] = {

    scene0_loader,
//    scene1_loader,
//    scene_meter_loader,
//    scene_alarm_clock_loader,
//#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__
//    virtual_resource_demo_loader,
//#endif
//    scene3_loader,
//    scene4_loader,
//    scene2_loader,

//    scene_fitness_loader,
    //scene_audiomark_loader
};


/* load scene one by one */
void before_scene_switching_handler(void *pTarget,
                                    arm_2d_scene_player_t *ptPlayer,
                                    arm_2d_scene_t *ptScene)
{
    static uint_fast8_t s_chIndex = 0;

    if (s_chIndex >= dimof(c_SceneLoaders)) {
        s_chIndex = 0;
    }
    
    /* call loader */
    c_SceneLoaders[s_chIndex]();
    s_chIndex++;
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
    }
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


