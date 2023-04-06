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
#include "example_gui.h"
#include "arm_2d_disp_adapters.h"

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
#ifndef __STR
#   define __STR(__A)      #__A
#endif

#ifndef STR
#   define STR(__A)         __STR(__A)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

ARM_NOINIT
static task_cycle_info_t s_tArm2DTaskInfo;
ARM_NOINIT
static task_cycle_info_agent_t s_tArm2DTaskInfoAgent;

__OVERRIDE_WEAK 
void __arm_2d_helper_perf_counter_start(int64_t *plTimestamp)
{
    start_task_cycle_counter(&s_tArm2DTaskInfo);
}

__OVERRIDE_WEAK 
int32_t __arm_2d_helper_perf_counter_stop(int64_t *plTimestamp)
{
    return stop_task_cycle_counter(&s_tArm2DTaskInfo);
}


int32_t Disp0_DrawBitmap(int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        const uint8_t *bitmap)
{
    return GLCD_DrawBitmap(x,y,width, height, bitmap);
}


/*----------------------------------------------------------------------------*
 * RTOS Port                                                                  *
 *----------------------------------------------------------------------------*/

uintptr_t arm_2d_port_new_semaphore(void)
{
    return (uintptr_t)osEventFlagsNew(NULL);
}


bool arm_2d_port_wait_for_semaphore(uintptr_t pSemaphore)
{
    osEventFlagsId_t evtFlag = (osEventFlagsId_t)pSemaphore;
    assert (NULL != evtFlag) ;

    osEventFlagsWait(evtFlag, 0x01, osFlagsWaitAny, osWaitForever );
    return true;
}

void arm_2d_port_set_semaphoret(uintptr_t pSemaphore)
{
    osEventFlagsId_t evtFlag = (osEventFlagsId_t)pSemaphore;
    assert (NULL != evtFlag) ;
    osEventFlagsSet(evtFlag, 0x01); 
}


/*----------------------------------------------------------------------------*
 * Application main thread                                                    *
 *----------------------------------------------------------------------------*/
 
 __NO_RETURN
void app_main (void *argument) 
{
    init_task_cycle_counter();
    register_task_cycle_agent(&s_tArm2DTaskInfo, &s_tArm2DTaskInfoAgent);

    while(1) {
        //! retrieve the number of system ticks
        uint32_t wTick = osKernelGetTickCount();        
        while(arm_fsm_rt_cpl != disp_adapter0_task());
        
        //! lock frame rate
        osDelayUntil(wTick + (1000 / APP_TARGET_FPS));
    }

    //osThreadExit();
}

__NO_RETURN
void arm_2d_thread(void *argument)
{
    init_task_cycle_counter();
    register_task_cycle_agent(&s_tArm2DTaskInfo, &s_tArm2DTaskInfoAgent);

    ARM_2D_UNUSED(argument);

    arm_2d_helper_backend_task();
    
    osThreadExit();
}


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) 
{
    /* Initialize CMSIS-RTOS2 */
    osKernelInitialize ();

    init_task_cycle_info(&s_tArm2DTaskInfo);

    arm_irq_safe {
        arm_2d_init();
    } 

    disp_adapter0_init();

    /* Create application main thread */
    osThreadNew(app_main, NULL, NULL);
    
    osThreadNew(arm_2d_thread, NULL, NULL);

    /* Start thread execution */
    osKernelStart();

    while (1) {
    }
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


