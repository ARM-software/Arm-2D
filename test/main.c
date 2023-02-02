/*
 * Copyright (c) 2017-2022 Arm Limited
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

void arm_2d_run_benchmark(void);
#include "perf_counter.h"
#include "stdbool.h"

/* for perf_counter */
void SysTick_Handler(void)
{
    user_code_insert_to_systick_handler();
}



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


#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif
 #define STR1(x)  #x
 #define STR(x)  STR1(x)

int main()
{
    DCACHE_ENA();
    ICACHE_ENA();

    init_cycle_counter(false);

    arm_2d_init();

    printf("start %s benchmark \n", STR(BENCH));

    start_marker_ccnt();
    arm_2d_run_benchmark();
    int cycles = stop_marker_ccnt();

    printf("finished in %d cycles \n", cycles);
    return 0;
}
