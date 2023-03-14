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
#include "arm_2d_helper.h"
#include "arm_extra_lcd_printf.h"
#include "__common.h"
#include "benchmark_generic.h"

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
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#   pragma clang diagnostic ignored "-Wbad-function-cast"
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

#ifndef ITERATION_CNT
#   define ITERATION_CNT       1000
#endif

#if defined(__USE_FVP__)
#   undef ITERATION_CNT
#   define ITERATION_CNT        1
#endif

#ifndef __ARM_2D_CFG_BENCHMARK_EXIT_WHEN_FINISH__
#   define __ARM_2D_CFG_BENCHMARK_EXIT_WHEN_FINISH__        0
#endif

#ifndef BENCHMARK_PFB_BLOCK_WIDTH
#   define BENCHMARK_PFB_BLOCK_WIDTH             __GLCD_CFG_SCEEN_WIDTH__
#endif

#ifndef BENCHMARK_PFB_BLOCK_HEIGHT
#   define BENCHMARK_PFB_BLOCK_HEIGHT            __GLCD_CFG_SCEEN_HEIGHT__
#endif

#ifndef BENCHMARK_PFB_HEAP_SIZE
#   define BENCHMARK_PFB_HEAP_SIZE              1
#endif


#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileGear01             c_tileGear01GRAY8
#   define c_tileGear02             c_tileGear02GRAY8
#   define c_tilePointerSec         c_tilePointerSecGRAY8
#   define c_tileBackground         c_tileBackgroundGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileGear01             c_tileGear01RGB565
#   define c_tileGear02             c_tileGear02RGB565
#   define c_tilePointerSec         c_tilePointerSecRGB565
#   define c_tileBackground         c_tileBackgroundRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileGear01             c_tileGear01CCCA8888
#   define c_tileGear02             c_tileGear02CCCA8888
#   define c_tilePointerSec         c_tilePointerSecCCCA8888
#   define c_tileBackground         c_tileBackgroundCCCA8888

#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern uint32_t SystemCoreClock;

extern 
const arm_2d_tile_t c_tileBackground;

/*============================ PROTOTYPES ====================================*/
extern 
void Disp0_DrawBitmap(  int16_t x,
                        int16_t y,
                        int16_t width,
                        int16_t height,
                        const uint8_t *bitmap);

/*============================ LOCAL VARIABLES ===============================*/

ARM_NOINIT arm_2d_scene_player_t BENCHMARK_DISP_ADAPTER;

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
    .wIterations = ITERATION_CNT,
};


/*============================ IMPLEMENTATION ================================*/


static volatile bool s_bDrawInfo = true;

static void on_example_gui_do_event(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
    
    s_bDrawInfo = false;
    example_gui_do_events();
}

static void on_frame_complete(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
    
    int32_t nTotalCyclCount = BENCHMARK_DISP_ADAPTER.use_as__arm_2d_helper_pfb_t.Statistics.nTotalCycle;
    int32_t nTotalLCDCycCount = BENCHMARK_DISP_ADAPTER.use_as__arm_2d_helper_pfb_t.Statistics.nRenderingCycle;
    BENCHMARK.wLCDLatency = nTotalLCDCycCount;

    if (BENCHMARK.wIterations) {
        BENCHMARK.wMin = MIN((uint32_t)nTotalCyclCount, BENCHMARK.wMin);
        BENCHMARK.wMax = MAX(nTotalCyclCount, (int32_t)BENCHMARK.wMax);
        BENCHMARK.dwTotal += nTotalCyclCount;
        BENCHMARK.wIterations--;

        if (0 == BENCHMARK.wIterations) {
            static bool s_bShowResult = false;
            
            if (!s_bShowResult) {
                s_bShowResult = true;
                BENCHMARK.wAverage =
                (uint32_t)(BENCHMARK.dwTotal / (uint64_t)ITERATION_CNT);
                BENCHMARK.fFPS30Freq = (float)
                    ((      (double)(BENCHMARK.wAverage * 30) 
                        /   (double)arm_2d_helper_get_reference_clock_frequency()) 
                     * ((float)SystemCoreClock / 1000000.0f));
                
                ptScene->fnBackground = NULL;
                arm_2d_scene_player_update_scene_background(ptScene->ptPlayer);
            }
        }
    }
}

__OVERRIDE_WEAK
void example_gui_on_refresh_evt_handler(const arm_2d_tile_t *ptFrameBuffer)
{
    ARM_2D_UNUSED(ptFrameBuffer);
    
    //! print performance info

    if (0 == BENCHMARK.wIterations) {
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);

#if !defined(__USE_FVP__)
        arm_lcd_text_location( __GLCD_CFG_SCEEN_HEIGHT__ / 8 - 7, 0);
        arm_lcd_puts(  "Transform Test, running "
                    STR(ITERATION_CNT)
                    " iterations\r\n");

        arm_lcd_puts(   "PFB Size: " STR(BENCHMARK_PFB_BLOCK_WIDTH)"*" STR(BENCHMARK_PFB_BLOCK_HEIGHT)
                    "  Screen Size: "STR(__GLCD_CFG_SCEEN_WIDTH__)"*" STR(__GLCD_CFG_SCEEN_HEIGHT__));
        arm_lcd_printf( "\r\nCPU Freq: %dMHz\r\n", SystemCoreClock / 1000000ul);
        arm_lcd_puts( "Benchmark Report:\r\n");
        
        arm_lcd_printf("Average: %d ", BENCHMARK.wAverage);
        arm_lcd_printf( "FPS30Freq: %4.2f MHz\r\n",  BENCHMARK.fFPS30Freq);
        arm_lcd_printf(
            "FPS: %3d:%dms   ",
            arm_2d_helper_get_reference_clock_frequency() / BENCHMARK.wAverage,
            (int32_t)arm_2d_helper_convert_ticks_to_ms(BENCHMARK.wAverage));
        arm_lcd_printf( 
            "LCD Latency: %2dms", 
            (int32_t)arm_2d_helper_convert_ticks_to_ms(BENCHMARK.wLCDLatency) );
#endif
    }
}


static
IMPL_PFB_ON_DRAW(__pfb_draw_handler)
{
    ARM_2D_UNUSED(pTarget);
    example_gui_refresh(ptTile, bIsNewFrame);

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}



static
IMPL_PFB_ON_DRAW(__pfb_draw_background_handler)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_BLACK);

#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
    arm_2d_canvas(ptTile, __canvas) {
        arm_2d_align_centre(__canvas, c_tileBackground.tRegion.tSize) {
            arm_2d_tile_copy_only(  &c_tileBackground,
                                    ptTile,
                                    &__centre_region);
        }
    }
#endif

    arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);

    arm_print_banner("Arm-2D Benchmark");
    
#if !defined(__USE_FVP__)
    arm_lcd_text_location( __GLCD_CFG_SCEEN_HEIGHT__ / 8 - 7, 0);
    arm_lcd_puts(  "Transform Test, running "
                STR(ITERATION_CNT)
                " iterations\r\n");

    arm_lcd_puts(   "PFB Size: " STR(BENCHMARK_PFB_BLOCK_WIDTH)"*" STR(BENCHMARK_PFB_BLOCK_HEIGHT)
                "  Screen Size: "STR(__GLCD_CFG_SCEEN_WIDTH__)"*" STR(__GLCD_CFG_SCEEN_HEIGHT__));
    arm_lcd_printf( "\r\nCPU Freq: %dMHz\r\n", SystemCoreClock / 1000000ul);
    arm_lcd_puts( "Testing...\r\n\r\n");
#endif

    //arm_lcd_text_location( GLCD_HEIGHT / 8 - 2, 0);
    //arm_lcd_puts("Cycles\tAvrage\tUPS30Freq\tUPS\tLCD Latency");
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}



#if __ARM_2D_CFG_BENCHMARK_ENABLE_ASYNC_FLUSHING__

/* using asynchronous flushing, e.g. using DMA + ISR to offload CPU etc. 
 * It can significantly reduce the LCD Latency hence improve the overrall 
 * framerate. 
 */

void benchmark_insert_async_flushing_complete_event_handler(void)
{
    arm_2d_helper_pfb_report_rendering_complete(
                    &BENCHMARK_DISP_ADAPTER.use_as__arm_2d_helper_pfb_t);
}

__WEAK
IMPL_PFB_ON_LOW_LV_RENDERING(__pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    if (0 == BENCHMARK.wIterations || s_bDrawInfo) {
        /* request an asynchronous flushing */
        benchmark_request_async_flushing(
                        pTarget,
                        bIsNewFrame,
                        ptTile->tRegion.tLocation.iX,
                        ptTile->tRegion.tLocation.iY,
                        ptTile->tRegion.tSize.iWidth,
                        ptTile->tRegion.tSize.iHeight,
                        (const COLOUR_INT *)ptTile->pchBuffer);
    } else {
        arm_2d_helper_pfb_report_rendering_complete(
            &BENCHMARK_DISP_ADAPTER.use_as__arm_2d_helper_pfb_t);
    }
}

#else
/* using asynchronous flushing, i.e. use CPU to flush LCD.
 * The LCD Latency will be high and reduce the overral framerate.
 * Meanwhile, in developing stage, this method can ensure a robust flushing. 
 */

__WEAK 
void Disp0_DrawBitmap(  int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        const uint8_t *bitmap)
{
    ARM_2D_UNUSED(x);
    ARM_2D_UNUSED(y);
    ARM_2D_UNUSED(width);
    ARM_2D_UNUSED(height);
    ARM_2D_UNUSED(bitmap);
}

__WEAK 
void Benchmark_DrawBitmap(  int16_t x, 
                            int16_t y, 
                            int16_t width, 
                            int16_t height, 
                            const uint8_t *bitmap)
{
    Disp0_DrawBitmap(x, y, width, height, bitmap);
}


__WEAK
IMPL_PFB_ON_LOW_LV_RENDERING(__pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    if (0 == BENCHMARK.wIterations || s_bDrawInfo) {
        Benchmark_DrawBitmap(ptTile->tRegion.tLocation.iX,
                        ptTile->tRegion.tLocation.iY,
                        ptTile->tRegion.tSize.iWidth,
                        ptTile->tRegion.tSize.iHeight,
                        (const uint8_t *)ptTile->pchBuffer);
    }
    arm_2d_helper_pfb_report_rendering_complete(
        &BENCHMARK_DISP_ADAPTER.use_as__arm_2d_helper_pfb_t);
}
#endif


void arm_2d_scene_player_init(void)
{
    memset(&BENCHMARK_DISP_ADAPTER, 0, sizeof(BENCHMARK_DISP_ADAPTER));

    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
        &BENCHMARK_DISP_ADAPTER.use_as__arm_2d_helper_pfb_t,                            //!< FPB Helper object
        __GLCD_CFG_SCEEN_WIDTH__,                                               //!< screen width
        __GLCD_CFG_SCEEN_HEIGHT__,                                              //!< screen height
        COLOUR_INT,                                                             //!< colour date type
        BENCHMARK_PFB_BLOCK_WIDTH,                                              //!< PFB block width
        BENCHMARK_PFB_BLOCK_HEIGHT,                                             //!< PFB block height
        BENCHMARK_PFB_HEAP_SIZE,                                                //!< number of PFB in the PFB pool
        {
            .evtOnLowLevelRendering = {
                //! callback for low level rendering
                .fnHandler = &__pfb_render_handler,
            },
        },
        //.FrameBuffer.bSwapRGB16 = true,
        .FrameBuffer.u3PixelWidthAlign = 3,
    ) < 0) {
        //! error detected
        assert(false);
    }
}

/*----------------------------------------------------------------------------*
 * Benchmark Entry                                                            *
 *----------------------------------------------------------------------------*/
#if !__ARM_2D_CFG_BENCHMARK_EXIT_WHEN_FINISH__
__NO_RETURN
#endif
void arm_2d_run_benchmark(void)
{
    example_gui_init();

    arm_2d_scene_player_init();

    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {
                .iX = (__GLCD_CFG_SCEEN_WIDTH__ - 243) >> 1,
                .iY = (__GLCD_CFG_SCEEN_HEIGHT__ - 243) >> 1,
            },
            .tSize = {
                .iWidth = 240,
                .iHeight = 240,
            },
        ),

    END_IMPL_ARM_2D_REGION_LIST()

    do {
        static arm_2d_scene_t s_tBenchmarkScene[] = {
            [0] = {
                .fnBackground   = &__pfb_draw_background_handler,
                .fnScene        = &__pfb_draw_handler,
                .ptDirtyRegion  = s_tDirtyRegions,
                .fnOnFrameStart = &on_example_gui_do_event,
                .fnOnFrameCPL   = &on_frame_complete,
                .fnDepose       = NULL,
            },
        };
        arm_2d_scene_player_append_scenes( 
                                        &BENCHMARK_DISP_ADAPTER,
                                        (arm_2d_scene_t *)s_tBenchmarkScene,
                                        dimof(s_tBenchmarkScene));
    } while(0);
    
    while(true) {
        arm_2d_scene_player_task(&BENCHMARK_DISP_ADAPTER);

#if __ARM_2D_CFG_BENCHMARK_EXIT_WHEN_FINISH__
        if (0 == BENCHMARK.wIterations) {
            return ;
        }
#endif
    }
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


