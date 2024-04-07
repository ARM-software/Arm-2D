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
#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#if defined(RTE_CMSIS_Compiler_STDOUT_LCD_Console)

#include <stdio.h>

#include "arm_2d_helper.h"
#include "arm_2d_disp_adapters.h"

#include "arm_2d_scene_console.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


ARM_NOINIT
static user_scene_console_t LCD_CONSOLE;

static 
uintptr_t s_hWaitConsole = 0;

static void __console_wait_for_flush_sync(void)
{
    arm_2d_port_wait_for_semaphore(s_hWaitConsole);
}

static void __console_signal_flush_sync(void)
{
    arm_2d_port_set_semaphore(s_hWaitConsole);
}


int stdout_putchar(int ch)
{
    while (!console_box_putchar(&LCD_CONSOLE.tConsole, ch)) {
        __console_wait_for_flush_sync();
    }
    
    return ch;
}

#if __IS_COMPILER_ARM_COMPILER__
int $Sub$$fflush(FILE *fn)
{
    extern int $Super$$fflush(FILE *fn);
    
    int result = $Super$$fflush(fn);
    if (fn == &__stdout) {
        __console_wait_for_flush_sync();
    }
    return result;
}
#endif

void arm_2d_lcd_console_init(void)
{
    static bool s_bInitalized = false;

    arm_irq_safe {
        if (s_bInitalized) {
            arm_exit_irq_safe;
        }
        s_bInitalized = true;
        
        /* RTX5 requires irq is enabled */
        __enable_irq();
        
        arm_2d_init();
        
        disp_adapter0_init();
        arm_2d_scene_console_init(&DISP0_ADAPTER, &LCD_CONSOLE);
        
        
        s_hWaitConsole = arm_2d_port_new_semaphore();
    }
}

__NO_RETURN
void arm_2d_lcd_console_insert_to_rtos_idle_task(void)
{
    arm_2d_lcd_console_init();

    while(1) {
 
        if (arm_fsm_rt_cpl == disp_adapter0_task()) {
            __console_signal_flush_sync();
        }
        
    }
}



#endif
