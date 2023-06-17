/*
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
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
#include "arm_2d_disp_adapters.h"
#include "arm_2d_scenes.h"

#include "arm_2d_scene_meter.h"
#include "arm_2d_scene_watch.h"

#if defined(RTE_Acceleration_Arm_2D_Extra_Benchmark)
#   include "arm_2d_benchmark.h"
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wbad-function-cast"
#   pragma clang diagnostic ignored "-Wunreachable-code-break"
#   pragma clang diagnostic ignored "-Wshorten-64-to-32"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif

/*============================ MACROS ========================================*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

// <o>Target FPS <1-120>
// <i> Try to lock framerate to a specified value
// <i> Default: 30
#ifndef LCD_TARGET_FPS
#   define LCD_TARGET_FPS       30
#endif

// <o>Arm-2D APP Stack Size <1024-32767>
// <i> Specify the arm-2d application thread stack size
// <i> Default: 2048
#ifndef APP_STACK_SIZE
#   define APP_STACK_SIZE       (3 * 1024ul)
#endif

// <<< end of configuration section >>>

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

void scene_meter_loader(void) 
{
    arm_2d_scene_meter_init(&DISP0_ADAPTER);
}

void scene_watch_loader(void) 
{
    arm_2d_scene_watch_init(&DISP0_ADAPTER);
}


void scene0_loader(void) 
{
    arm_2d_scene0_init(&DISP0_ADAPTER);
}

void scene1_loader(void) 
{
    arm_2d_scene1_init(&DISP0_ADAPTER);
}

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
    scene1_loader,
    scene_meter_loader,
    scene3_loader,
    scene4_loader,
    scene2_loader,
    scene_watch_loader,
};



/*============================ IMPLEMENTATION ================================*/

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



int32_t Disp0_DrawBitmap(int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        const uint8_t *bitmap)
{
    return GLCD_DrawBitmap(x,y,width, height, bitmap);
}

 __NO_RETURN
void app_2d_main_thread (void *argument) 
{

#if defined(RTE_Acceleration_Arm_2D_Extra_Benchmark)
    arm_2d_run_benchmark();
#endif

    while(1) {
        //! retrieve the number of system ticks
        uint32_t wTick = osKernelGetTickCount();        
        while(arm_fsm_rt_cpl != disp_adapter0_task());
        
        //! lock frame rate
        osDelayUntil(wTick + (1000 / LCD_TARGET_FPS));
    }

    //osThreadExit();
}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) 
{
    /* Initialize CMSIS-RTOS2 */
    osKernelInitialize ();

    arm_irq_safe {
        arm_2d_init();
    }

    disp_adapter0_init();

#if !defined(RTE_Acceleration_Arm_2D_Extra_Benchmark)
    arm_2d_scene_player_register_before_switching_event_handler(
            &DISP0_ADAPTER,
            before_scene_switching_handler);
    
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 3000);
    
    arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
#endif

    static uint64_t thread1_stk_1[APP_STACK_SIZE / sizeof(uint64_t)];
     
    const osThreadAttr_t thread1_attr = {
      .stack_mem  = &thread1_stk_1[0],
      .stack_size = sizeof(thread1_stk_1)
    };

    /* Create application main thread */
    osThreadNew(app_2d_main_thread, NULL, &thread1_attr);

    /* Start thread execution */
    osKernelStart();

    while (1) {
    }
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


