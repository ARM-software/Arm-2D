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


#include "arm_2d_helper.h"
#include "arm_2d_disp_adapters.h"
#include "cmsis_os2.h"

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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

__OVERRIDE_WEAK
arm_2d_runtime_feature_t ARM_2D_RUNTIME_FEATURE = {
    .TREAT_OUT_OF_RANGE_AS_COMPLETE         = 1,
    .HAS_DEDICATED_THREAD_FOR_2D_TASK       = __ARM_2D_HAS_ASYNC__,
};



/*----------------------------------------------------------------------------*
 * RTOS Port                                                                  *
 *----------------------------------------------------------------------------*/

__OVERRIDE_WEAK
uintptr_t arm_2d_port_new_semaphore(void)
{
    return (uintptr_t)osEventFlagsNew(NULL);
}

__OVERRIDE_WEAK
void arm_2d_port_free_semaphore(uintptr_t pSemaphore)
{
    osEventFlagsDelete((osEventFlagsId_t)pSemaphore);
}

__OVERRIDE_WEAK
bool arm_2d_port_wait_for_semaphore(uintptr_t pSemaphore)
{
    osEventFlagsId_t evtFlag = (osEventFlagsId_t)pSemaphore;
    
    if (NULL != evtFlag) {
        osEventFlagsWait(evtFlag, 0x01, osFlagsWaitAny, osWaitForever );
    }
    return true;
}

__OVERRIDE_WEAK
void arm_2d_port_set_semaphoret(uintptr_t pSemaphore)
{
    osEventFlagsId_t evtFlag = (osEventFlagsId_t)pSemaphore;
    if (NULL != evtFlag) {
        osEventFlagsSet(evtFlag, 0x01); 
    }
}


/*----------------------------------------------------------------------------*
 * Application main thread                                                    *
 *----------------------------------------------------------------------------*/
 

__NO_RETURN
void arm_2d_backend_thread(void *argument)
{
    ARM_2D_UNUSED(argument);

    arm_2d_helper_backend_task();
    
    osThreadExit();
}


void arm_2d_helper_rtos_init(void)
{
    static uint64_t s_dwThreadStack[2048 / sizeof(uint64_t)];
     
    const osThreadAttr_t c_tThreadAttribute = {
      .stack_mem  = &s_dwThreadStack[0],
      .stack_size = sizeof(s_dwThreadStack)
    };

    osThreadNew(arm_2d_backend_thread, NULL, &c_tThreadAttribute);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


