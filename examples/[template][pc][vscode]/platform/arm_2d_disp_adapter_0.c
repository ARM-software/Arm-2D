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

#include "arm_2d_disp_adapter_0.h"

#ifdef RTE_Acceleration_Arm_2D_Helper_Disp_Adapter0

#include <stdio.h>
#include <stdlib.h>

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
#   pragma clang diagnostic ignored "-Wembedded-directive"
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

#ifndef __DISP0_CFG_ITERATION_CNT__
#   define __DISP0_CFG_ITERATION_CNT__     30
#endif

#if __DISP0_CFG_OPTIMIZE_DIRTY_REGIONS__
#   if      !defined(__DISP0_CFG_DIRTY_REGION_POOL_SIZE__)             \
        ||  __DISP0_CFG_DIRTY_REGION_POOL_SIZE__ < 4
#       undef __DISP0_CFG_DIRTY_REGION_POOL_SIZE__
#       define __DISP0_CFG_DIRTY_REGION_POOL_SIZE__            4
#   endif
#endif

#if __DISP0_CFG_USE_CONSOLE__

#ifndef __DISP0_CONSOLE_WIDTH__
#   if __DISP0_CFG_SCEEN_WIDTH__ < 204
#       define __DISP0_CONSOLE_WIDTH__      __DISP0_CFG_SCEEN_WIDTH__
#   else
#       define __DISP0_CONSOLE_WIDTH__      204
#   endif
#endif

#ifndef __DISP0_CONSOLE_HEIGHT__
#   if __DISP0_CFG_SCEEN_HEIGHT__ < 200
#       define __DISP0_CONSOLE_HEIGHT__      __DISP0_CFG_SCEEN_HEIGHT__
#   else
#       define __DISP0_CONSOLE_HEIGHT__      192
#   endif
#endif

#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern uint32_t SystemCoreClock;

/*============================ PROTOTYPES ====================================*/
extern 
int32_t Disp0_DrawBitmap(int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        const uint8_t *bitmap);

/*============================ LOCAL VARIABLES ===============================*/

#if __DISP0_CFG_NAVIGATION_LAYER_MODE__ == 2
/* the round mode */
IMPL_ARM_2D_REGION_LIST(s_tNavDirtyRegionList, static)

    /* a region for the status bar on the bottom of the screen */
    ADD_LAST_REGION_TO_LIST(s_tNavDirtyRegionList,
        .tSize = {
            .iWidth = 100,
            .iHeight = 24,
        },
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tNavDirtyRegionList)
#endif

ARM_NOINIT 
arm_2d_scene_player_t DISP0_ADAPTER;

#if __DISP0_CFG_ENABLE_3FB_HELPER_SERVICE__
ARM_NOINIT
static 
arm_2d_helper_3fb_t s_tDirectModeHelper;
#endif

#if __DISP0_CFG_USE_CONSOLE__
static 
struct {
    console_box_t tConsole;
    int64_t lTimestamp;
    arm_2d_region_list_item_t tBackground;
    uint32_t Signature;
    bool bShowConsole;
    uint8_t chOpacity;
} DISP0_CONSOLE;
#endif

/*============================ IMPLEMENTATION ================================*/
static void __on_frame_start(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
}

static void __on_frame_complete(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
}


static
IMPL_PFB_ON_DRAW(__pfb_draw_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);

    arm_2d_canvas(ptTile, __top_container) {
        
        arm_2d_align_centre(__top_container, 100, 100) {
            draw_round_corner_box(  ptTile,
                                    &__centre_region,
                                    GLCD_COLOR_BLACK,
                                    64,
                                    bIsNewFrame);
        }

        busy_wheel2_show(ptTile, bIsNewFrame);
    }

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

#if __DISP0_CFG_NAVIGATION_LAYER_MODE__

__WEAK 
IMPL_PFB_ON_DRAW(__disp_adapter0_user_draw_navigation)
{
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(bIsNewFrame);

    return arm_fsm_rt_cpl;
}

__WEAK
IMPL_PFB_ON_DRAW(__disp_adapter0_draw_navigation)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(bIsNewFrame);

    while(  arm_fsm_rt_cpl != 
            __disp_adapter0_user_draw_navigation(  pTarget, 
                                                            ptTile, 
                                                            bIsNewFrame));

#if __DISP0_CFG_USE_CONSOLE__

    if (bIsNewFrame) {
        if (console_box_on_frame_start(&DISP0_CONSOLE.tConsole)) {
            DISP0_CONSOLE.lTimestamp = 0;
            if (!DISP0_CONSOLE.bShowConsole) {
                arm_2d_dirty_region_item_ignore_set(&DISP0_CONSOLE.tBackground, false);
            } else {
                arm_2d_dirty_region_item_ignore_set(&DISP0_CONSOLE.tBackground, true);
            }
            DISP0_CONSOLE.bShowConsole = true;
            DISP0_CONSOLE.chOpacity = 255;
        }

    #if __DISP0_CFG_CONSOLE_DISPALY_TIME__ >= 1000                              \
    && __DISP0_CFG_CONSOLE_DISPALY_TIME__ != 0xFFFFFFFF
        if (DISP0_CONSOLE.bShowConsole) {
            if (arm_2d_helper_is_time_out(__DISP0_CFG_CONSOLE_DISPALY_TIME__, &DISP0_CONSOLE.lTimestamp)) {
                DISP0_CONSOLE.bShowConsole = false;
            } else {
                int64_t lTimeElapsedInMs = -arm_2d_helper_time_elapsed(&DISP0_CONSOLE.lTimestamp);
                if (lTimeElapsedInMs > 255) {
                    DISP0_CONSOLE.chOpacity = 255;
                } else {
                    DISP0_CONSOLE.chOpacity = lTimeElapsedInMs;
                }
            }
        }
    #endif
    }

    arm_2d_canvas(ptTile, __navigation_canvas) {

        if (DISP0_CONSOLE.bShowConsole) {
            arm_2d_align_top_left(  __navigation_canvas, 
                                    __DISP0_CONSOLE_WIDTH__ + 8, 
                                    __DISP0_CONSOLE_HEIGHT__ + 8) {

                draw_round_corner_box(  ptTile, 
                                        &__top_left_region, 
                                        GLCD_COLOR_DARK_GREY, 
                                        (128 * DISP0_CONSOLE.chOpacity) >> 8,
                                        bIsNewFrame);

                console_box_show(&DISP0_CONSOLE.tConsole,
                                ptTile,
                                &__top_left_region,
                                bIsNewFrame,
                                DISP0_CONSOLE.chOpacity);
            }
        }
    }

#endif

#if __DISP0_CFG_NAVIGATION_LAYER_MODE__ == 2
    /* round mode */
    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
    arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
    arm_lcd_text_set_draw_region(&(s_tNavDirtyRegionList[0].tRegion));
    
    if (__DISP0_CFG_ITERATION_CNT__) {
        draw_round_corner_box(  ptTile, 
                                &(s_tNavDirtyRegionList[0].tRegion), 
                                __RGB(64,64,64),
                                255-32,
                                bIsNewFrame);

        ARM_2D_OP_WAIT_ASYNC();

    #if __DISP0_CFG_COLOUR_DEPTH__ == 8
        arm_lcd_text_set_colour(GLCD_COLOR_LIGHT_GREY, GLCD_COLOR_BLACK);
    #else
        arm_lcd_text_set_colour(GLCD_COLOR_GREEN, GLCD_COLOR_BLACK);
    #endif

        arm_lcd_text_location(0,0);
        if (DISP0_ADAPTER.Benchmark.wAverage) {
            arm_lcd_printf(
                "  FPS:%3"PRIu32":%"PRIu32"ms\r\n",
                MIN(arm_2d_helper_get_reference_clock_frequency() / DISP0_ADAPTER.Benchmark.wAverage, 999),
                (uint32_t)arm_2d_helper_convert_ticks_to_ms(DISP0_ADAPTER.Benchmark.wAverage));
        }
        arm_lcd_printf( 
            "  CPU:%2.2f%% \r\n", 
            DISP0_ADAPTER.Benchmark.fCPUUsage);

        arm_lcd_printf( 
            "  LCD:%2"PRIu32"ms",
            (uint32_t)arm_2d_helper_convert_ticks_to_ms(DISP0_ADAPTER.Benchmark.wLCDLatency) );
        
    }
#else
    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
    arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
    arm_lcd_text_set_draw_region(NULL);

    /* draw real-time FPS info */
    if (__DISP0_CFG_ITERATION_CNT__) {
        arm_2dp_fill_colour_with_opacity(
                    NULL, 
                    ptTile, 
                    (arm_2d_region_t []){
                        {
                            .tLocation = {
                                .iX = 0,
                                .iY = ((__DISP0_CFG_SCEEN_HEIGHT__ + 7) / 8 - 2) * 8},
                            .tSize = {
                                .iWidth = __DISP0_CFG_SCEEN_WIDTH__,
                                .iHeight = 8,
                            },
                        },
                    }, 
                    (__arm_2d_color_t){__RGB(64,64,64)}, 
                    255 - 32);

        ARM_2D_OP_WAIT_ASYNC();

    #if __DISP0_CFG_COLOUR_DEPTH__ == 8
        arm_lcd_text_set_colour(GLCD_COLOR_LIGHT_GREY, GLCD_COLOR_BLACK);
    #else
        arm_lcd_text_set_colour(GLCD_COLOR_GREEN, GLCD_COLOR_BLACK);
    #endif
        arm_lcd_text_location((__DISP0_CFG_SCEEN_HEIGHT__ + 7) / 8 - 2,
                              0);

        if (DISP0_ADAPTER.Benchmark.wAverage) {
            arm_lcd_printf(
                "FPS:%3"PRIu32":%"PRIu32"ms ",
                MIN(arm_2d_helper_get_reference_clock_frequency() / DISP0_ADAPTER.Benchmark.wAverage, 999),
                (uint32_t)arm_2d_helper_convert_ticks_to_ms(DISP0_ADAPTER.Benchmark.wAverage));
        }

#if __DISP0_CFG_SCEEN_WIDTH__ >= 240
        arm_lcd_printf( 
            "CPU:%2.2f%% LCD-Latency:%2"PRIu32"ms", 
            DISP0_ADAPTER.Benchmark.fCPUUsage,
            (uint32_t)arm_2d_helper_convert_ticks_to_ms(DISP0_ADAPTER.Benchmark.wLCDLatency));
#else
        arm_lcd_printf( 
            "LCD:%2"PRIu32"ms",
            (uint_fast64_t)arm_2d_helper_convert_ticks_to_ms(DISP0_ADAPTER.Benchmark.wLCDLatency) );
#endif
    }

#if __DISP0_CFG_SCEEN_WIDTH__ >= 320 

    /* draw verion info on the bottom right corner */
    arm_lcd_text_set_colour(GLCD_COLOR_LIGHT_GREY, GLCD_COLOR_WHITE);
    arm_lcd_text_location( (__DISP0_CFG_SCEEN_HEIGHT__ + 7) / 8 - 2, 
                            (__DISP0_CFG_SCEEN_WIDTH__ / 6) - 12);
    arm_lcd_printf("v" 
                    ARM_TO_STRING(ARM_2D_VERSION_MAJOR)
                    "."
                    ARM_TO_STRING(ARM_2D_VERSION_MINOR)
                    "."
                    ARM_TO_STRING(ARM_2D_VERSION_PATCH)
                    " "
                    ARM_2D_VERSION_STR
                    );
#endif
#endif
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}
#endif

#if __DISP0_CFG_ENABLE_3FB_HELPER_SERVICE__
__WEAK
IMPL_PFB_ON_LOW_LV_RENDERING(__disp_adapter0_pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(bIsNewFrame);

    if (__arm_2d_helper_3fb_draw_bitmap(&s_tDirectModeHelper,
                                        ptPFB)) {

        arm_2d_helper_pfb_report_rendering_complete(
                        &DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t);
    }
}

void *disp_adapter0_3fb_get_flush_pointer(void)
{
    return arm_2d_helper_3fb_get_flush_pointer(&s_tDirectModeHelper);
}


#   if __DISP0_CFG_ENABLE_ASYNC_FLUSHING__
/* using asynchronous flushing, e.g. using DMA + ISR to offload CPU etc. 
 * It can significantly reduce the LCD Latency hence improve the overrall 
 * framerate. 
 */

void disp_adapter0_insert_2d_copy_complete_event_handler(void)
{
    arm_2d_helper_pfb_report_rendering_complete(
                    &DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t);
}

/* using asynchronous flushing, e.g. using DMA + ISR to offload CPU etc. 
 * It can significantly reduce the LCD Latency hence improve the overrall 
 * framerate. 
 */

void disp_adapter0_insert_dma_copy_complete_event_handler(void)
{
    arm_2d_helper_3fb_report_dma_copy_complete(&s_tDirectModeHelper);
}


#   endif

#else
#   if __DISP0_CFG_ENABLE_ASYNC_FLUSHING__

/* using asynchronous flushing, e.g. using DMA + ISR to offload CPU etc. 
 * It can significantly reduce the LCD Latency hence improve the overrall 
 * framerate. 
 */

void disp_adapter0_insert_async_flushing_complete_event_handler(void)
{
    arm_2d_helper_pfb_report_rendering_complete(
                    &DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t);
}

__WEAK
IMPL_PFB_ON_LOW_LV_RENDERING(__disp_adapter0_pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(bIsNewFrame);

    /* request an asynchronous flushing */
    __disp_adapter0_request_async_flushing(
                    pTarget,
                    bIsNewFrame,
                    ptTile->tRegion.tLocation.iX,
                    ptTile->tRegion.tLocation.iY,
                    ptTile->tRegion.tSize.iWidth,
                    ptTile->tRegion.tSize.iHeight,
                    (const COLOUR_INT *)ptTile->pchBuffer);

}

#   else
/* using asynchronous flushing, i.e. use CPU to flush LCD.
 * The LCD Latency will be high and reduce the overral framerate.
 * Meanwhile, in developing stage, this method can ensure a robust flushing. 
 */

__WEAK
IMPL_PFB_ON_LOW_LV_RENDERING(__disp_adapter0_pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(bIsNewFrame);

    Disp0_DrawBitmap(ptTile->tRegion.tLocation.iX,
                    ptTile->tRegion.tLocation.iY,
                    ptTile->tRegion.tSize.iWidth,
                    ptTile->tRegion.tSize.iHeight,
                    (const uint8_t *)ptTile->pchBuffer);

    arm_2d_helper_pfb_report_rendering_complete(
                    &DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t);
}
#   endif
#endif

static bool __on_each_frame_complete(void *ptTarget)
{
    ARM_2D_PARAM(ptTarget);
    
    int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();
    
#if __DISP0_CFG_FPS_CACULATION_MODE__ == ARM_2D_FPS_MODE_REAL
    static int64_t s_lLastTimeStamp = 0;

    int32_t nElapsed = 0;
    if (0 != s_lLastTimeStamp) {
        nElapsed = (int32_t)(lTimeStamp - s_lLastTimeStamp);
    }
    s_lLastTimeStamp = lTimeStamp;
    
#else /* __DISP0_CFG_FPS_CACULATION_MODE__ == ARM_2D_FPS_MODE_RENDER_ONLY */
    int32_t nElapsed = DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t.Statistics.nTotalCycle;
#endif

    int32_t nTotalLCDCycCount = DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t.Statistics.nRenderingCycle;
    DISP0_ADAPTER.Benchmark.wLCDLatency = nTotalLCDCycCount;

    /* calculate real-time FPS */
    if (__DISP0_CFG_ITERATION_CNT__) {
        if (DISP0_ADAPTER.Benchmark.hwIterations) {
            int32_t nRenderCycle = DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t.Statistics.nTotalCycle;
            DISP0_ADAPTER.Benchmark.wMin = MIN((uint32_t)nElapsed, DISP0_ADAPTER.Benchmark.wMin);
            DISP0_ADAPTER.Benchmark.wMax = MAX(nElapsed, (int32_t)DISP0_ADAPTER.Benchmark.wMax);
            DISP0_ADAPTER.Benchmark.dwTotal += nElapsed;
            DISP0_ADAPTER.Benchmark.dwRenderTotal += nRenderCycle;
            DISP0_ADAPTER.Benchmark.hwIterations--;
            DISP0_ADAPTER.Benchmark.hwFrameCounter += (nRenderCycle != 0) ? 1 : 0;

            if (0 == DISP0_ADAPTER.Benchmark.hwIterations) {

                if (0 == DISP0_ADAPTER.Benchmark.hwFrameCounter) {
                    DISP0_ADAPTER.Benchmark.wAverage = 0;
                } else {
                    DISP0_ADAPTER.Benchmark.wAverage =
                        (uint32_t)(DISP0_ADAPTER.Benchmark.dwTotal / (uint64_t)DISP0_ADAPTER.Benchmark.hwFrameCounter);
                    DISP0_ADAPTER.Benchmark.wAverage = MAX(1, DISP0_ADAPTER.Benchmark.wAverage);
                }

                int64_t lElapsed = lTimeStamp - DISP0_ADAPTER.Benchmark.lTimestamp;
                if (lElapsed) {
                    DISP0_ADAPTER.Benchmark.fCPUUsage = (float)((double)DISP0_ADAPTER.Benchmark.dwRenderTotal / (double)lElapsed) * 100.0f;
                }

                /* log statistics */
                if (DISP0_ADAPTER.Benchmark.wAverage) {
                    ARM_2D_LOG_INFO(
                        STATISTICS, 
                        0, 
                        "DISP_ADAPTER0", 
                        "FPS:%3d(%dms)\tCPU:%2.2f%%\tLCD-Latency:%2dms",
                        MIN(arm_2d_helper_get_reference_clock_frequency() / DISP0_ADAPTER.Benchmark.wAverage, 999),
                        (int32_t)arm_2d_helper_convert_ticks_to_ms(DISP0_ADAPTER.Benchmark.wAverage),
                        DISP0_ADAPTER.Benchmark.fCPUUsage,
                        (int32_t)arm_2d_helper_convert_ticks_to_ms(DISP0_ADAPTER.Benchmark.wLCDLatency)
                    );
                } else {
                    ARM_2D_LOG_INFO(
                        STATISTICS, 
                        0, 
                        "DISP_ADAPTER0", 
                        "FPS: SKIPPED\tCPU:%2.2f%%\tLCD-Latency:%2dms",
                        DISP0_ADAPTER.Benchmark.fCPUUsage,
                        (int32_t)arm_2d_helper_convert_ticks_to_ms(DISP0_ADAPTER.Benchmark.wLCDLatency)
                    );
                }
                 
                DISP0_ADAPTER.Benchmark.wMin = UINT32_MAX;
                DISP0_ADAPTER.Benchmark.wMax = 0;
                DISP0_ADAPTER.Benchmark.dwTotal = 0;
                DISP0_ADAPTER.Benchmark.dwRenderTotal = 0;
                DISP0_ADAPTER.Benchmark.hwIterations = __DISP0_CFG_ITERATION_CNT__;
                DISP0_ADAPTER.Benchmark.hwFrameCounter = 0;

                DISP0_ADAPTER.Benchmark.lTimestamp = arm_2d_helper_get_system_timestamp();
            }
        }
    }
    
    return true;
}


#if __DISP0_CFG_ROTATE_SCREEN__
/*!
 * \brief before-flushing event handler
 * \param[in] ptOrigin the original PFB
 * \param[in] ptScratch A scratch PFB
 * \return true the new content is stored in ptScratch
 * \return false the new content is stored in ptOrigin
 */
static IMPL_PFB_BEFORE_FLUSHING(__before_flushing)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptOrigin);
    ARM_2D_PARAM(ptScratch);


#if      __DISP0_CFG_COLOUR_DEPTH__ == 8
#   define __COLOUR_NAME__  c8bit
#elif    __DISP0_CFG_COLOUR_DEPTH__ == 16
#   define __COLOUR_NAME__  rgb16
#elif    __DISP0_CFG_COLOUR_DEPTH__ == 32
#   define __COLOUR_NAME__  rgb32
#endif

#if     __DISP0_CFG_ROTATE_SCREEN__ == 1
#   define __ROTATE__       90
#elif   __DISP0_CFG_ROTATE_SCREEN__ == 2
#   define __ROTATE__       180
#elif   __DISP0_CFG_ROTATE_SCREEN__ == 3
#   define __ROTATE__       270
#endif

    ARM_CONNECT(__arm_2d_helper_pfb_rotate, __ROTATE__,_, __COLOUR_NAME__)(
        ptOrigin, 
        ptScratch,
        (arm_2d_size_t []) {
            {
                __DISP0_CFG_SCEEN_WIDTH__,
                __DISP0_CFG_SCEEN_HEIGHT__
            }
        });

    return true;
}

#endif

static void __user_scene_player_init(void)
{
    memset(&DISP0_ADAPTER, 0, sizeof(DISP0_ADAPTER));

#if __DISP0_CFG_OPTIMIZE_DIRTY_REGIONS__
    ARM_NOINIT
    static arm_2d_region_list_item_t s_tDirtyRegionList[__DISP0_CFG_DIRTY_REGION_POOL_SIZE__]; 
#endif

    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
        &DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t,                            //!< FPB Helper object
        __DISP0_CFG_SCEEN_WIDTH__,                                     //!< screen width
        __DISP0_CFG_SCEEN_HEIGHT__,                                    //!< screen height
        COLOUR_INT,                                                             //!< colour date type
        __DISP0_COLOUR_FORMAT__,                                       //!< colour format
        __DISP0_CFG_PFB_BLOCK_WIDTH__,                                 //!< PFB block width
        __DISP0_CFG_PFB_BLOCK_HEIGHT__,                                //!< PFB block height
        __DISP0_CFG_PFB_HEAP_SIZE__                                    //!< number of PFB in the PFB pool

#if     __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__                          \
    &&  !__DISP0_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__
        + __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__ - 1
#else
        + (__DISP0_CFG_ROTATE_SCREEN__ > 0)
#endif
        ,{
            .evtOnLowLevelRendering = {
                //! callback for low level rendering
                .fnHandler = &__disp_adapter0_pfb_render_handler,
            },
            .evtOnEachFrameCPL = {
                .fnHandler = &__on_each_frame_complete,
            },
#if __DISP0_CFG_ROTATE_SCREEN__
            .evtBeforeFlushing = {
                .fnHandler = &__before_flushing,
            },
#endif
        },
#if __DISP0_CFG_SWAP_RGB16_HIGH_AND_LOW_BYTES__
        .FrameBuffer.bSwapRGB16 = true,
#endif
#if __DISP0_CFG_DEBUG_DIRTY_REGIONS__
        .FrameBuffer.bDebugDirtyRegions = true,
#endif
        .FrameBuffer.u3PixelWidthAlign = __DISP0_CFG_PFB_PIXEL_ALIGN_WIDTH__,
        .FrameBuffer.u3PixelHeightAlign = __DISP0_CFG_PFB_PIXEL_ALIGN_HEIGHT__,
#if     __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__                          \
    &&  !__DISP0_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__
        // reserve PFB blocks for the virtual resource service
        .FrameBuffer.u4PoolReserve = __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__ - 1,
#endif
#if __DISP0_CFG_OPTIMIZE_DIRTY_REGIONS__
        .DirtyRegion.ptRegions = s_tDirtyRegionList,
        .DirtyRegion.chCount = dimof(s_tDirtyRegionList),
#endif
    ) < 0) {
        //! error detected
        assert(false);
    }

#if __DISP0_CFG_ENABLE_3FB_HELPER_SERVICE__
    do {
    extern uintptr_t __DISP_ADAPTER0_3FB_FB0_ADDRESS__;
    extern uintptr_t __DISP_ADAPTER0_3FB_FB1_ADDRESS__;
    extern uintptr_t __DISP_ADAPTER0_3FB_FB2_ADDRESS__;
    
    extern arm_2d_helper_2d_copy_handler_t __disp_adapter0_request_2d_copy;
    extern arm_2d_helper_dma_copy_handler_t __disp_adapter0_request_dma_copy;
    
    
        arm_2d_helper_3fb_cfg_t tCFG = {
            .tScreenSize = {
                __DISP0_CFG_SCEEN_WIDTH__,
                __DISP0_CFG_SCEEN_HEIGHT__,
            },
            .chPixelBits = __DISP0_CFG_COLOUR_DEPTH__,
            .pnAddress = {
                [0] = ((uintptr_t)__DISP_ADAPTER0_3FB_FB0_ADDRESS__),
                [1] = ((uintptr_t)__DISP_ADAPTER0_3FB_FB1_ADDRESS__),
                [2] = ((uintptr_t)__DISP_ADAPTER0_3FB_FB2_ADDRESS__),
            },
#if __DISP0_CFG_ENABLE_ASYNC_FLUSHING__
            .evtOn2DCopy = {
                .fnHandler = __disp_adapter0_request_2d_copy,
            },
            .evtOnDMACopy = {
                .fnHandler = __disp_adapter0_request_dma_copy,
            },
#endif
        };
        
        arm_2d_helper_3fb_init(&s_tDirectModeHelper, &tCFG);
    
    } while(0);
#endif

    arm_lcd_text_init((arm_2d_region_t []) {
                        { .tSize = {
                            .iWidth = __DISP0_CFG_SCEEN_WIDTH__,
                            .iHeight = __DISP0_CFG_SCEEN_HEIGHT__,
                        }}});

    DISP0_ADAPTER.Benchmark.wMin = UINT32_MAX;
    DISP0_ADAPTER.Benchmark.hwIterations = __DISP0_CFG_ITERATION_CNT__;
    DISP0_ADAPTER.Benchmark.hwFrameCounter = 0;
}

#if __DISP0_CFG_NAVIGATION_LAYER_MODE__
__WEAK 
void disp_adapter0_navigator_init(void)
{
#if __DISP0_CFG_NAVIGATION_LAYER_MODE__ == 2
    
    static const arm_2d_region_t tScreen = {
        .tSize = {
            .iWidth = __DISP0_CFG_SCEEN_WIDTH__,
            .iHeight = __DISP0_CFG_SCEEN_HEIGHT__,
        },
    };
    
    arm_2d_align_bottom_centre(tScreen, s_tNavDirtyRegionList[0].tRegion.tSize) {
        s_tNavDirtyRegionList[0].tRegion = __bottom_centre_region;
        s_tNavDirtyRegionList[0].tRegion.tLocation.iY -= 16;
    }
#else
    /*! define dirty regions for the navigation layer */
    IMPL_ARM_2D_REGION_LIST(s_tNavDirtyRegionList, static)

        /* a region for the status bar on the bottom of the screen */
        ADD_LAST_REGION_TO_LIST(s_tNavDirtyRegionList,
            .tLocation = {
                .iX = 0,
                .iY = ((__DISP0_CFG_SCEEN_HEIGHT__ + 7) / 8 - 2) * 8},
            .tSize = {
                .iWidth = __DISP0_CFG_SCEEN_WIDTH__,
                .iHeight = 8,
            },
        ),

    END_IMPL_ARM_2D_REGION_LIST(s_tNavDirtyRegionList)
#endif

#if __DISP0_CFG_USE_CONSOLE__
    do {

    #if __DISP0_CFG_CONSOLE_INPUT_BUFFER__
        static uint8_t s_chInputBuffer[256];
    #endif
        static uint8_t s_chConsoleBuffer[   (__DISP0_CONSOLE_WIDTH__ / 6) 
                                        *   (__DISP0_CONSOLE_HEIGHT__ / 8)];
        console_box_cfg_t tCFG = {
            .tBoxSize = {
                .iWidth = __DISP0_CONSOLE_WIDTH__, 
                .iHeight = __DISP0_CONSOLE_HEIGHT__,
            },

            .pchConsoleBuffer = s_chConsoleBuffer,
            .hwConsoleBufferSize = sizeof(s_chConsoleBuffer),

        #if __DISP0_CFG_CONSOLE_INPUT_BUFFER__
            .pchInputBuffer = s_chInputBuffer,
            .hwInputBufferSize = sizeof(s_chInputBuffer),
        #endif

        #if __DISP0_CFG_COLOUR_DEPTH__ == 8
            .tColor = GLCD_COLOR_WHITE,
        #else
            .tColor = GLCD_COLOR_GREEN,
        #endif
            
            .bUseDirtyRegion = true,
            .ppDirtyRegionList = (arm_2d_region_list_item_t **)&s_tNavDirtyRegionList,
        };

        console_box_init(   &DISP0_CONSOLE.tConsole, 
                            NULL, 
                            &tCFG);
    } while(0);

    arm_2d_dirty_region_item_ignore_set(&DISP0_CONSOLE.tBackground, true);
    
    arm_2d_region_t tScreen = {
        .tSize = {
            __DISP0_CFG_SCEEN_WIDTH__, 
            __DISP0_CFG_SCEEN_HEIGHT__
        },
    };

    arm_2d_align_top_left(tScreen, 220, 200) {
        DISP0_CONSOLE.tBackground.tRegion = __top_left_region;
    }

    arm_2d_helper_pfb_append_dirty_regions_to_list(
                                (arm_2d_region_list_item_t **)&s_tNavDirtyRegionList,
                                &DISP0_CONSOLE.tBackground,
                                1);
 
    DISP0_CONSOLE.lTimestamp = 0;

    DISP0_CONSOLE.Signature = ARM_2D_VERSION;
#endif

    /* register event handler for evtOnDrawNavigation */
    arm_2d_scene_player_register_on_draw_navigation_event_handler(
                    &DISP0_ADAPTER,
                    __disp_adapter0_draw_navigation,
                    NULL,
                    (arm_2d_region_list_item_t *)s_tNavDirtyRegionList);
}
#else
__WEAK 
void disp_adapter0_navigator_init(void)
{

}
#endif

#if __DISP0_CFG_USE_CONSOLE__

#include <stdarg.h>

#if defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__
#define __va_list    va_list
#endif

ARM_NONNULL(1)
int disp_adapter0_printf(const char *format, ...)
{
    int real_size, n;
    char s_chBuffer[128];
    char *pchSrc = s_chBuffer;

    __va_list ap;
    va_start(ap, format);
        real_size = vsnprintf(s_chBuffer, sizeof(s_chBuffer)-1, format, ap);
    va_end(ap);
    real_size = MIN(sizeof(s_chBuffer)-1, real_size);
    s_chBuffer[real_size] = '\0';
    n = real_size;

    do {
        if (!console_box_putchar(&DISP0_CONSOLE.tConsole, *pchSrc++)) {
            break;
        }
    } while(--n);

    return real_size;
}

bool disp_adapter0_putchar(uint8_t chChar)
{
    if (DISP0_CONSOLE.Signature != ARM_2D_VERSION) {
        return false;
    }

    return console_box_putchar(&DISP0_CONSOLE.tConsole,chChar);
}

#endif

/*----------------------------------------------------------------------------*
 * Display Adapter Entry                                                      *
 *----------------------------------------------------------------------------*/

void disp_adapter0_init(void)
{
    __user_scene_player_init();

    arm_2d_helper_init();

#if defined(RTE_Acceleration_Arm_2D_Extra_Controls)
    extern
    void arm_extra_controls_init(void);

    arm_extra_controls_init();
#endif

    disp_adapter0_navigator_init();

    DISP0_ADAPTER.Benchmark.lTimestamp = arm_2d_helper_get_system_timestamp();

    if (!__DISP0_CFG_DISABLE_DEFAULT_SCENE__) {
    #if 0
        /*! define dirty regions */
        IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

            /* a region for the busy wheel */
            ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
                .tLocation = {
                    .iX = ((__DISP0_CFG_SCEEN_WIDTH__ - 100) >> 1),
                    .iY = ((__DISP0_CFG_SCEEN_HEIGHT__ - 100) >> 1),
                },
                .tSize = {
                    .iWidth = 100,
                    .iHeight = 100,
                },
            ),

        END_IMPL_ARM_2D_REGION_LIST()
    #endif
    
        static arm_2d_scene_t s_tScenes[] = {
            [0] = {
            
                /* the canvas colour */
                .tCanvas = {GLCD_COLOR_WHITE}, 
        
                .fnScene        = &__pfb_draw_handler,
                //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
                .fnOnFrameStart = &__on_frame_start,
                .fnOnFrameCPL   = &__on_frame_complete,
                .fnDepose       = NULL,
            },
        };
        arm_2d_scene_player_append_scenes( 
                                        &DISP0_ADAPTER,
                                        (arm_2d_scene_t *)s_tScenes,
                                        dimof(s_tScenes));
    }
}

arm_fsm_rt_t __disp_adapter0_task(void)
{
    return arm_2d_scene_player_task(&DISP0_ADAPTER);
}


/*----------------------------------------------------------------------------*
 * Virtual Resource Helper                                                    *
 *----------------------------------------------------------------------------*/

#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__

__WEAK
void * __disp_adapter0_aligned_malloc(size_t nSize, size_t nAlign)
{
    ARM_2D_UNUSED(nAlign);

    /* ensure nAlign is 2^n */
    assert((((~nAlign) + 1) & nAlign) == nAlign);

    void * pMem = malloc(nSize);
    assert( 0 == ((uintptr_t)pMem & (nAlign - 1)));
    return pMem;
}

__WEAK
void __disp_adapter0_free(void *pMem)
{
    if (NULL != pMem) {
        free(pMem);
    }
}

__WEAK 
void __disp_adapter0_vres_asset_2dcopy( uintptr_t pObj,
                                        arm_2d_vres_t *ptVRES,
                                        arm_2d_region_t *ptRegion,
                                        uintptr_t pSrc,
                                        uintptr_t pDes,
                                        int16_t iTargetStride,
                                        int16_t iSourceStride,
                                        int16_t iPixelSize)
{
    assert(NULL != ptRegion);
    assert(NULL != ptVRES);

    int16_t iSourceWidth = ptRegion->tSize.iWidth;
    int16_t iSourceHeight = ptRegion->tSize.iHeight;

    /* calculate offset */
    pSrc += (ptRegion->tLocation.iY * iSourceStride + ptRegion->tLocation.iX) * iPixelSize;
    
    for (int_fast16_t y = 0; y < iSourceHeight; y++) {
        __disp_adapter0_vres_read_memory( 
                                        pObj, 
                                        (void *)pDes, 
                                        (uintptr_t)pSrc, 
                                        iPixelSize * iSourceWidth);
        
        pDes += iTargetStride * iPixelSize;
        pSrc += iSourceStride * iPixelSize;
    }
}

intptr_t __disp_adapter0_vres_asset_loader (uintptr_t pObj, 
                                            arm_2d_vres_t *ptVRES, 
                                            arm_2d_region_t *ptRegion)
{
    COLOUR_INT *pBuffer = NULL;
    size_t nPixelSize = sizeof(COLOUR_INT);
    size_t tBufferSize;
    uint32_t nBytesPerLine = ptRegion->tSize.iWidth * nPixelSize;
    size_t nBitsPerPixel = sizeof(COLOUR_INT) << 3;

    if (0 != ptVRES->tTile.tColourInfo.chScheme) {
        nBitsPerPixel = (1 << ptVRES->tTile.tColourInfo.u3ColourSZ);
        if (ptVRES->tTile.tColourInfo.u3ColourSZ >= 3) {
            nPixelSize = (1 << (ptVRES->tTile.tColourInfo.u3ColourSZ - 3));
            nBytesPerLine = ptRegion->tSize.iWidth * nPixelSize;
        } else {
            /* for A1, A2 and A4 */
            size_t nPixelPerByte = 1 << (3 - ptVRES->tTile.tColourInfo.u3ColourSZ);
            int16_t iOffset = ptRegion->tLocation.iX & (nPixelPerByte - 1);
            
            uint32_t nBitsPerLine =  nBitsPerPixel * (iOffset + ptRegion->tSize.iWidth);
            nBytesPerLine = (nBitsPerLine + 7) >> 3;
        }
    }

    /* background load mode */
    do {
        if (ptVRES->tTile.tInfo.u3ExtensionID != ARM_2D_TILE_EXTENSION_VRES) {
            break;
        }

        assert ((uintptr_t)NULL != ptVRES->tTile.nAddress);

        uintptr_t pSrc = __disp_adapter0_vres_get_asset_address(pObj, ptVRES);
        uintptr_t pDes = (uintptr_t)ptVRES->tTile.nAddress;
        int16_t iTargetStride = ptVRES->tTile.tInfo.Extension.VRES.iTargetStride;
        int16_t iSourceStride = ptVRES->tTile.tRegion.tSize.iWidth;
    
        __disp_adapter0_vres_asset_2dcopy(  pObj, 
                                            ptVRES, 
                                            ptRegion, 
                                            pSrc, 
                                            pDes, 
                                            iTargetStride, 
                                            iSourceStride, 
                                            nPixelSize);

        return ptVRES->tTile.nAddress;
    } while(0);

    /* default condition */
    tBufferSize = ptRegion->tSize.iHeight * nBytesPerLine;
    
    
#if __DISP0_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__
    pBuffer = __disp_adapter0_aligned_malloc(tBufferSize, nPixelSize);
    assert(NULL != pBuffer);

    if (NULL == pBuffer) {
        return (intptr_t)NULL;
    }
#else
    arm_2d_pfb_t *ptPFB = __arm_2d_helper_pfb_new(&DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t);
    assert(NULL != ptPFB);
    
    assert(ptPFB->u24Size >= tBufferSize);
    
    if (tBufferSize > ptPFB->u24Size) {
        return (intptr_t)NULL;
    }
    pBuffer = (COLOUR_INT *)((uintptr_t)ptPFB + sizeof(arm_2d_pfb_t));
#endif
    /* load content into the buffer */
    if (nBitsPerPixel < 8) {
        /* A1, A2 and A4 support */
        uintptr_t pSrc = __disp_adapter0_vres_get_asset_address(pObj, ptVRES);
        uintptr_t pDes = (uintptr_t)pBuffer;
        
        uint32_t iBitsperLineInSource = ptVRES->tTile.tRegion.tSize.iWidth * nBitsPerPixel;
        int16_t iSourceStride = (int16_t)((uint32_t)(iBitsperLineInSource + 7) >> 3);

        /* calculate offset */
        pSrc += (ptRegion->tLocation.iY * iSourceStride);
        pSrc += (ptRegion->tLocation.iX * nBitsPerPixel) >> 3;
        
        for (int_fast16_t y = 0; y < ptRegion->tSize.iHeight; y++) {
            __disp_adapter0_vres_read_memory(   pObj, 
                                                (void *)pDes, 
                                                (uintptr_t)pSrc, 
                                                nBytesPerLine);

            pDes += nBytesPerLine;
            pSrc += iSourceStride;
        }
    } else {
        uintptr_t pSrc = __disp_adapter0_vres_get_asset_address(pObj, ptVRES);
        uintptr_t pDes = (uintptr_t)pBuffer;
        int16_t iTargetStride = ptRegion->tSize.iWidth;
        int16_t iSourceStride = ptVRES->tTile.tRegion.tSize.iWidth;

        __disp_adapter0_vres_asset_2dcopy(  pObj, 
                                            ptVRES, 
                                            ptRegion, 
                                            pSrc, 
                                            pDes, 
                                            iTargetStride, 
                                            iSourceStride, 
                                            nPixelSize);
    } while(0);
    
    return (intptr_t)pBuffer;
}


void __disp_adapter0_vres_buffer_deposer (
                                            uintptr_t pTarget, 
                                            arm_2d_vres_t *ptVRES, 
                                            intptr_t pBuffer )
{
#if __DISP0_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptVRES);

    if ((intptr_t)NULL != pBuffer) {
        __disp_adapter0_free((void *)pBuffer);
    }
#else
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptVRES);
    if ((intptr_t)NULL == pBuffer) {
        return ;
    }
    
    arm_2d_pfb_t *ptPFB = (arm_2d_pfb_t *)((uintptr_t)pBuffer - sizeof(arm_2d_pfb_t));
    __arm_2d_helper_pfb_free(&DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t, ptPFB);
#endif
}

#endif


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


