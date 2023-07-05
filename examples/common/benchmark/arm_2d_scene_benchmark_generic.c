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

#include "arm_2d.h"

#define __USER_SCENE_BENCHMARK_GENERIC_IMPLEMENT__
#include "arm_2d_scene_benchmark_generic.h"

#include "arm_2d_helper.h"
#include "arm_extra_controls.h"

#include <stdlib.h>
#include <string.h>

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
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#   pragma clang diagnostic ignored "-Wbad-function-cast"  
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe177,Pe174
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#   pragma GCC diagnostic ignored "-Wunused-value"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileCMSISLogo          c_tileCMSISLogoGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#else
#   error Unsupported colour depth!
#endif

#undef this
#define this (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern uint32_t SystemCoreClock;

/*============================ PROTOTYPES ====================================*/
extern
void benchmark_generic_init(void);

extern
void benchmark_generic_draw(const arm_2d_tile_t *ptTile, bool bIsNewFrame);

extern
void benchmark_generic_do_events(void);

/*============================ LOCAL VARIABLES ===============================*/
static struct {
    uint32_t wMin;
    uint32_t wMax;
    uint64_t dwTotal;
    uint32_t wAverage;
    float fFPS30Freq;
    uint32_t wIterations;
    uint32_t wLCDLatency;
} BENCHMARK = {
    .wMin = UINT32_MAX,
    .wMax = 0,
    .dwTotal = 0,
    .wAverage = 0,
#if defined(__USE_FVP__)
    .wIterations = 2,
#else
    .wIterations = ITERATION_CNT,
#endif
};

/*============================ IMPLEMENTATION ================================*/

#if __IS_COMPILER_ARM_COMPILER_6__
static __inline__ int __attribute__((__always_inline__, __nodebug__))
__semihost(int val, const void *ptr) {
  register int v __asm__("r0") = val;
  register const void *p __asm__("r1") = ptr;
  __asm__ __volatile__(
#if defined(__thumb__)
#if defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'M'
      "bkpt 0xab"
#else /* !defined(__ARM_ARCH_PROFILE) || __ARM_ARCH_PROFILE != 'M' */
#if defined(__USE_HLT_SEMIHOSTING)
      ".inst.n 0xbabc"
#else
      "svc 0xab"
#endif
#endif
#else /* !defined(__thumb__) */
#if defined(__USE_HLT_SEMIHOSTING)
      ".inst 0xe10f0070"
#else
      "svc 0x123456"
#endif
#endif
      : "+r"(v), "+r"(p)
      :
      : "memory", "cc");
  return v;
}
#endif


static void __on_scene_benchmark_generic_depose(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_t *ptThis = (user_scene_benchmark_generic_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (!this.bUserAllocated) {
        free(ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene benchmark_generic                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_benchmark_generic_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_t *ptThis = (user_scene_benchmark_generic_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_benchmark_generic_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_t *ptThis = (user_scene_benchmark_generic_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_benchmark_generic_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_t *ptThis = (user_scene_benchmark_generic_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    benchmark_generic_do_events();
}

static void __on_scene_benchmark_generic_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_t *ptThis = (user_scene_benchmark_generic_t *)ptScene;
    arm_2d_helper_pfb_t *ptHelper = &ptScene->ptPlayer->use_as__arm_2d_helper_pfb_t;
    ARM_2D_UNUSED(ptThis);

    int32_t nTotalCyclCount = ptHelper->Statistics.nTotalCycle;
    int32_t nTotalLCDCycCount = ptHelper->Statistics.nRenderingCycle;

    BENCHMARK.wLCDLatency = nTotalLCDCycCount;

    if (BENCHMARK.wIterations) {
        BENCHMARK.wMin = MIN((uint32_t)nTotalCyclCount, BENCHMARK.wMin);
        BENCHMARK.wMax = MAX(nTotalCyclCount, (int32_t)BENCHMARK.wMax);
        BENCHMARK.dwTotal += nTotalCyclCount;
        BENCHMARK.wIterations--;

        if (0 == BENCHMARK.wIterations) {
            BENCHMARK.wAverage =
                (uint32_t)(BENCHMARK.dwTotal / (uint64_t)ITERATION_CNT);
            BENCHMARK.wAverage = MAX(1, BENCHMARK.wAverage);

#if defined(__i386__) || defined(__x86_64__) || defined(__APPLE__)
            BENCHMARK.fFPS30Freq = 0.0f;
#else
            BENCHMARK.fFPS30Freq = (float)
                ((      (double)(BENCHMARK.wAverage * 30) 
                    /   (double)arm_2d_helper_get_reference_clock_frequency()) 
                 * ((double)SystemCoreClock / 1000000.0f));
#endif

            /* resume low level flush */
            arm_2d_helper_resume_low_level_flush(ptHelper);

#if __ARM_2D_CFG_BENCHMARK_EXIT_WHEN_FINISH__
#   if defined(__MICROLIB)
            __semihost(0x18, (const void *)0x20026);
#   else
            exit(0);
#endif
#endif
        }
    }

}

static void __before_scene_benchmark_generic_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_benchmark_generic_t *ptThis = (user_scene_benchmark_generic_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_benchmark_generic_handler)
{
    user_scene_benchmark_generic_t *ptThis = (user_scene_benchmark_generic_t *)pTarget;
    ARM_2D_UNUSED(ptThis);

    benchmark_generic_draw(ptTile, bIsNewFrame);

    if (0 == BENCHMARK.wIterations) {

#if !defined(__USE_FVP__)
        do {
            arm_2d_helper_pfb_t *ptPFBHelper 
                = &this.use_as__arm_2d_scene_t.ptPlayer->use_as__arm_2d_helper_pfb_t;
            arm_2d_region_t tScreen = arm_2d_helper_pfb_get_display_area(ptPFBHelper);
            arm_2d_size_t tPFBSize = arm_2d_helper_pfb_get_pfb_size(ptPFBHelper);
            
            arm_lcd_text_set_font(NULL);
            arm_lcd_text_set_draw_region(NULL);

            arm_lcd_text_location( tScreen.tSize.iHeight / 8 - 7, 0);
            arm_lcd_puts(  "All-in-One Test, running "
                        ARM_TO_STRING(ITERATION_CNT)
                        " iterations\r\n");

            arm_lcd_printf( "PFB Size: %d*%d  Screen Size: %d*%d",
                            tPFBSize.iWidth,
                            tPFBSize.iHeight,
                            tScreen.tSize.iWidth, 
                            tScreen.tSize.iHeight);
#if defined(__i386__) || defined(__x86_64__) || defined(__APPLE__)
            arm_lcd_printf( "\r\nCPU Freq: N/A\r\n");
#else
            arm_lcd_printf( "\r\nCPU Freq: %dMHz\r\n", SystemCoreClock / 1000000ul);
#endif
            arm_lcd_puts( "Benchmark Report:\r\n");
            
            arm_lcd_printf("Average: %d ", BENCHMARK.wAverage);
#if defined(__i386__) || defined(__x86_64__) || defined(__APPLE__)
            arm_lcd_printf( "FPS30Freq: N/A\r\n");
#else
            arm_lcd_printf( "FPS30Freq: %4.2f MHz\r\n",  BENCHMARK.fFPS30Freq);
#endif
            arm_lcd_printf(
                "FPS: %3d:%dms   ",
                MIN(arm_2d_helper_get_reference_clock_frequency() / BENCHMARK.wAverage, 999),
                (int32_t)arm_2d_helper_convert_ticks_to_ms(BENCHMARK.wAverage));
            arm_lcd_printf( 
                "LCD Latency: %2dms", 
                (int32_t)arm_2d_helper_convert_ticks_to_ms(BENCHMARK.wLCDLatency) );
        } while(0);
#endif
    }

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_benchmark_generic_t *__arm_2d_scene_benchmark_generic_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_benchmark_generic_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_benchmark_generic_t *)malloc(sizeof(user_scene_benchmark_generic_t));
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_benchmark_generic_t));

    *ptThis = (user_scene_benchmark_generic_t){
        .use_as__arm_2d_scene_t = {
            .fnScene        = &__pfb_draw_scene_benchmark_generic_handler,
            //.fnOnBGStart    = &__on_scene_benchmark_generic_background_start,
            //.fnOnBGComplete = &__on_scene_benchmark_generic_background_complete,
            .fnOnFrameStart = &__on_scene_benchmark_generic_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_benchmark_generic_switching_out,
            .fnOnFrameCPL   = &__on_scene_benchmark_generic_frame_complete,
            .fnDepose       = &__on_scene_benchmark_generic_depose,
        },
        .bUserAllocated = bUserAllocated,
    };

    
    /* disable low level flush */
    arm_2d_helper_ignore_low_level_flush(
                                &(ptDispAdapter->use_as__arm_2d_helper_pfb_t));
    
    /* initialize benchmark generic */
    benchmark_generic_init();
    
    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


