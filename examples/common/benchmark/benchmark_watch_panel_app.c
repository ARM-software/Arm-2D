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


#ifndef BENCHMARK_PFB_BLOCK_WIDTH
#   define BENCHMARK_PFB_BLOCK_WIDTH             __GLCD_CFG_SCEEN_WIDTH__
#endif

#ifndef BENCHMARK_PFB_BLOCK_HEIGHT
#   define BENCHMARK_PFB_BLOCK_HEIGHT            __GLCD_CFG_SCEEN_HEIGHT__
#endif


#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileGear01             c_tileGear01Mask
#   define c_tileGear02             c_tileGear02Mask
#   define c_tilePointerSec         c_tilePointerSecMask
#   define c_tileBackground         c_tileBackgroundCHNB

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

static ARM_NOINIT arm_2d_scene_player_t s_tScenePlayer;

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
    
    int32_t nTotalCyclCount = s_tScenePlayer.use_as__arm_2d_helper_pfb_t.Statistics.nTotalCycle;
    int32_t nTotalLCDCycCount = s_tScenePlayer.use_as__arm_2d_helper_pfb_t.Statistics.nRenderingCycle;
    BENCHMARK.wLCDLatency = nTotalLCDCycCount;

    if (BENCHMARK.wIterations) {
        BENCHMARK.wMin = MIN((uint32_t)nTotalCyclCount, BENCHMARK.wMin);
        BENCHMARK.wMax = MAX(nTotalCyclCount, (int32_t)BENCHMARK.wMax);
        BENCHMARK.dwTotal += nTotalCyclCount;
        BENCHMARK.wIterations--;

        if (0 == BENCHMARK.wIterations) {
            BENCHMARK.wAverage =
                (uint32_t)(BENCHMARK.dwTotal / (uint64_t)ITERATION_CNT);
            BENCHMARK.fFPS30Freq = (float)
                ((      (double)(BENCHMARK.wAverage * 30) 
                    /   (double)arm_2d_helper_get_reference_clock_frequency()) 
                 * ((float)SystemCoreClock / 1000000.0f));
        }
    }
}

__OVERRIDE_WEAK
void example_gui_on_refresh_evt_handler(const arm_2d_tile_t *ptFrameBuffer)
{
    ARM_2D_UNUSED(ptFrameBuffer);

    //! print performance info

    if (0 == BENCHMARK.wIterations) {

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
    arm_2d_tile_copy_only(  &c_tileBackground,
                            ptTile,
                            NULL);
    
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
    arm_2d_helper_pfb_report_rendering_complete(&s_tScenePlayer.use_as__arm_2d_helper_pfb_t,
                                                (arm_2d_pfb_t *)ptPFB);
}


void arm_2d_user_scene_player_init(void)
{
    memset(&s_tScenePlayer, 0, sizeof(s_tScenePlayer));

    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
        &s_tScenePlayer.use_as__arm_2d_helper_pfb_t,                            //!< FPB Helper object
        __GLCD_CFG_SCEEN_WIDTH__,                                               //!< screen width
        __GLCD_CFG_SCEEN_HEIGHT__,                                              //!< screen height
        COLOUR_INT,                                                             //!< colour date type
        BENCHMARK_PFB_BLOCK_WIDTH,                                              //!< PFB block width
        BENCHMARK_PFB_BLOCK_HEIGHT,                                             //!< PFB block height
        1,                                                                      //!< number of PFB in the PFB pool
        {
            .evtOnLowLevelRendering = {
                //! callback for low level rendering
                .fnHandler = &__pfb_render_handler,
            },
        },
        //.FrameBuffer.bSwapRGB16 = true,
    ) < 0) {
        //! error detected
        assert(false);
    }
}

/*----------------------------------------------------------------------------*
 * Benchmark Entry                                                            *
 *----------------------------------------------------------------------------*/

__NO_RETURN
void arm_2d_run_benchmark(void)
{
    example_gui_init();

    arm_2d_user_scene_player_init();
    
    do {
        /*! define dirty regions */
        IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, const static)

            /* a region for the busy wheel */
            ADD_REGION_TO_LIST(s_tDirtyRegions,
                .tLocation = {
                    .iX = ((__GLCD_CFG_SCEEN_WIDTH__ - 222) >> 1),
                    .iY = ((__GLCD_CFG_SCEEN_HEIGHT__ - 222) >> 1),
                },
                .tSize = {
                    .iWidth = 222,
                    .iHeight = 222,
                },
            ),

            /* a region for the status bar on the bottom of the screen */
            ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
                .tLocation = {0,__GLCD_CFG_SCEEN_HEIGHT__ - 16},
                .tSize = {
                    .iWidth = __GLCD_CFG_SCEEN_WIDTH__,
                    .iHeight = 16,
                },
            ),

        END_IMPL_ARM_2D_REGION_LIST()
        
        static arm_2d_scene_t s_tBenchmarkScene[] = {
            [0] = {
                .fnBackground   = &__pfb_draw_background_handler,
                .fnScene        = &__pfb_draw_handler,
                .ptDirtyRegion  = NULL, //s_tDirtyRegions,
                .fnOnFrameStart = &on_example_gui_do_event,
                .fnOnFrameCPL   = &on_frame_complete,
                .fnDepose       = NULL,
            },
        };
        arm_2d_user_scene_player_append_scenes( 
                                        &s_tScenePlayer,
                                        (arm_2d_scene_t *)s_tBenchmarkScene,
                                        dimof(s_tBenchmarkScene));
    } while(0);
    
    while(true) {
        arm_2d_user_scene_player_task(&s_tScenePlayer);
    }
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


