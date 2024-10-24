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
    //arm_2d_scene0_init(&DISP0_ADAPTER);
    //arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
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
