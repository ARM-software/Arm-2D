/*
 * Copyright (c) 2017-2024 Arm Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include "arm_2d.h"
#include "arm_2d_helper.h"

#include "arm_2d_disp_adapters.h"
#include "perf_counter.h"
#include "stdbool.h"


#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
#   include "arm_2d_benchmark.h"
#endif

#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
#   include "arm_2d_benchmark.h"
#endif

#ifndef __ARM_2D_NOLCD
#include "Board_GLCD.h"
#endif


#if defined (USECACHE) && USECACHE==1
#ifdef SSE300
#include "SSE300MPS3.h"
#elif SSE310
#include "SSE310MPS3.h"
#endif
#include <cmsis_compiler.h>
#include "cachel1_armv7.h"

#define DCACHE_ENA()                SCB_EnableDCache()
#define ICACHE_ENA()                SCB_EnableICache()
#define DCACHE_DISA()               SCB_DisableDCache()
#define CLEAN_INV_DC()              SCB_CleanInvalidateDCache ()
#define PRELOAD_DC(add,sz)          for(int i=0; i< ROUND_UP(sz, CACHE_LINE_SIZE); i+= CACHE_LINE_SIZE) \
                                          __pld((void *)(add + i));
#else
#define DCACHE_ENA()
#define ICACHE_ENA()
#define DCACHE_DISA()
#define CLEAN_INV_DC()
#define PRELOAD_DC(add,sz)
#endif

#define dbgInst(imm) __asm volatile("DBG %0\n\t" : :"Ir" ((imm)) )

#define CM_DWT_CYCCNTENA_BIT   (1UL<<0)
#define CM_TRCENA_BIT          (1UL<<24)

#define CM_DWT_CONTROL         (*((volatile uint32_t*)0xE0001000))
#define CM_DWT_CYCCNT          (*((volatile uint32_t*)0xE0001004))
#define CM_DWT_LAR             (*((volatile uint32_t*)0xE0001FB0))
#define CM_DEMCR               (*((volatile uint32_t*)0xE000EDFC))

#define CM_DWT_CPICNT          (*((volatile uint32_t*)0xE0001008))
#define CM_DWT_EXCCNT          (*((volatile uint32_t*)0xE000100C))
#define CM_DWT_SLEEPCNT        (*((volatile uint32_t*)0xE0001010))
#define CM_DWT_LSUCNT          (*((volatile uint32_t*)0xE0001014))
#define CM_DWT_FOLDCNT         (*((volatile uint32_t*)0xE0001018))

#define SysTick_START         0xFFFFFF

#define SysTick_CSR           (*((volatile uint32_t*)(0xE000E010)))
#define SysTick_RVR           (*((volatile uint32_t*)(0xE000E014)))
#define SysTick_CVR           (*((volatile uint32_t*)(0xE000E018)))

#define SysTick_Enable        0x1
#define SysTick_ClockSource   0x4


static void start_marker_ccnt()
{
    CM_DEMCR |= CM_TRCENA_BIT;
    CM_DWT_LAR = 0xC5ACCE55;
    CM_DWT_CYCCNT = 0;
    CM_DWT_CONTROL |= CM_DWT_CYCCNTENA_BIT;
    dbgInst(9);
}

static  uint32_t stop_marker_ccnt()
{
    CM_DWT_CONTROL &= ~CM_DWT_CYCCNTENA_BIT;
    SysTick_CSR &= ~SysTick_Enable;
    dbgInst(1);
    return CM_DWT_CYCCNT;
}


#ifndef __ARM_2D_NOLCD
int stdout_putchar (int ch) {
  return (ch);
}
#endif


#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif
 #define STR1(x)  #x
 #define STR(x)  STR1(x)


__attribute__((used))
void SysTick_Handler(void)
{
}

__NO_RETURN
extern void $Super$$exit(int status);

__NO_RETURN
void $Sub$$exit(int status)
{
    int cycles = stop_marker_ccnt();
    printf("finished in %d cycles \n", cycles);

    $Super$$exit(status);
}

#ifdef __ARM_2D_NOLCD
__OVERRIDE_WEAK
void Disp0_DrawBitmap(  int16_t x,
                        int16_t y,
                        int16_t width,
                        int16_t height,
                        const uint8_t *bitmap)
{
}
#else
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

    static uint16_t s_hwFrameBuffer[BENCHMARK_PFB_BLOCK_WIDTH * BENCHMARK_PFB_BLOCK_HEIGHT];

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
        .iHeight =
				height,
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
#endif

int main(void)
{
    DCACHE_ENA();
    ICACHE_ENA();

    init_cycle_counter(false);

    arm_2d_init();

    printf("start %s benchmark\n", STR(BENCH));

    disp_adapter0_init();
    arm_2d_run_benchmark();


    while (1) {

        uint32_t        start = get_system_ticks();
        dbgInst(9);

        while(arm_fsm_rt_cpl != disp_adapter0_task()) __NOP();

        dbgInst(1);
        uint32_t        end = get_system_ticks();
        //printf("stop %d\n", end- start);
    }

    return 0;
}
