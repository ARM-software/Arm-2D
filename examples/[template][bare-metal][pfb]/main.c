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
#include "arm_2d_disp_adapter_0.h"
#include "scene_player_demo.h"
#include "virtual_resource_demo.h"

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
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
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
static int64_t s_lTimestap;
/*============================ IMPLEMENTATION ================================*/


__OVERRIDE_WEAK 
void arm_2d_helper_perf_counter_start(void)
{
    s_lTimestap = get_system_ticks();
}

__OVERRIDE_WEAK 
int32_t arm_2d_helper_perf_counter_stop(void)
{
    return (int32_t)(get_system_ticks() - s_lTimestap);
}

__OVERRIDE_WEAK 
int64_t arm_2d_helper_get_system_timestamp(void)
{
    return get_system_ticks();
}

__OVERRIDE_WEAK 
uint32_t arm_2d_helper_get_reference_clock_frequency(void)
{
    return SystemCoreClock;
}

__OVERRIDE_WEAK 
void Disp0_DrawBitmap(  int16_t x, 
                            int16_t y, 
                            int16_t width, 
                            int16_t height, 
                            const uint8_t *bitmap)
{
#if __GLCD_CFG_COLOUR_DEPTH__ == 32
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
#endif

    GLCD_DrawBitmap(x, y, width, height, bitmap);
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
    scene_player_demo_init();
    
#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__
    virtual_resource_demo_init();
#endif

    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 1000);
    
    arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
    
    while (1) {
        disp_adapter0_task();
    }
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


