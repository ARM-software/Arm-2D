/****************************************************************************
*  Copyright 2021 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/
/*============================ INCLUDES ======================================*/
#include "pico/stdlib.h"
#include "perf_counter.h"
#include "bsp/bsp.h"

#include <stdio.h>

#include "RTE_Components.h"
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
#   include <EventRecorder.h>
#endif

#if defined(RTE_Script_PikaScript)
#   include "pikaScript.h"
#endif

#if defined(__RTE_ACCELERATION_ARM_2D__) || defined(RTE_Acceleration_Arm_2D)
#   include "arm_2d.h"
#   include "arm_2d_helper.h"
#   include "arm_2d_disp_adapters.h"
#   include "arm_2d_scenes.h"
#   include "arm_2d_demos.h"

#endif

#if defined(RTE_Acceleration_Arm_2D_Extra_Benchmark)
#   include "arm_2d_benchmark.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void SysTick_Handler(void)
{

}

static void system_init(void)
{
    extern void SystemCoreClockUpdate();

    SystemCoreClockUpdate();
    /*! \note if you do want to use SysTick in your application, please use 
     *!       init_cycle_counter(true); 
     *!       instead of 
     *!       init_cycle_counter(false); 
     */
    init_cycle_counter(false);

#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
    EventRecorderInitialize(0, 1);
#endif
    stdio_init_all();

    bsp_init();


#if defined(__RTE_ACCELERATION_ARM_2D__) || defined(RTE_Acceleration_Arm_2D)
    arm_2d_init();
    disp_adapter0_init();
#endif
}


void scene_mono_loading_loader(void) 
{
    arm_2d_scene_mono_loading_init(&DISP0_ADAPTER);
}

void scene_mono_histogram_loader(void) 
{
    arm_2d_scene_mono_histogram_init(&DISP0_ADAPTER);
}

void scene_mono_clock_loader(void) 
{
    arm_2d_scene_mono_clock_init(&DISP0_ADAPTER);
}

void scene_mono_list_loader(void) 
{
    arm_2d_scene_mono_list_init(&DISP0_ADAPTER);
}

void scene_mono_icon_menu_loader(void) 
{
    arm_2d_scene_mono_icon_menu_init(&DISP0_ADAPTER);
}

typedef void scene_loader_t(void);

static scene_loader_t * const c_SceneLoaders[] = {

    scene_mono_loading_loader,
    scene_mono_histogram_loader,
    scene_mono_clock_loader,
    scene_mono_list_loader,
    scene_mono_icon_menu_loader,
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

int main(void) 
{
    system_init();

    __cycleof__("printf") {
        printf("Hello Pico-Template\r\n");
    }

#if defined( __PERF_COUNTER_COREMARK__ ) && __PERF_COUNTER_COREMARK__
    printf("\r\nRun Coremark 1.0...\r\n");
    coremark_main();
#endif


#if defined(__RTE_ACCELERATION_ARM_2D__) || defined(RTE_Acceleration_Arm_2D)

#   if defined(RTE_Acceleration_Arm_2D_Extra_Benchmark)
    arm_2d_run_benchmark();
#else
    arm_2d_scene_player_register_before_switching_event_handler(
                                            &DISP0_ADAPTER,
                                            before_scene_switching_handler);
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
#   endif

#endif

    while (true) {
        breath_led();

#if defined(__RTE_ACCELERATION_ARM_2D__) || defined(RTE_Acceleration_Arm_2D)
    #if ARM_2D_VERSION >= 10105 && !defined(RTE_Acceleration_Arm_2D_Extra_Benchmark)
        /* lock framerate: 30 FPS */
        disp_adapter0_task(30);
    #else
        disp_adapter0_task();
    #endif
#endif
    }
    //return 0;
}