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

#ifdef RTE_Acceleration_Arm_2D_Helper_Disp_Adapter%Instance%

#include <stdio.h>
#include <stdlib.h>
#include "arm_2d_helper.h"
#include "arm_extra_lcd_printf.h"
#include "arm_extra_controls.h"
#include "arm_2d_disp_adapter_%Instance%.h"

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

#ifndef __DISP%Instance%_CFG_ITERATION_CNT__
#   define __DISP%Instance%_CFG_ITERATION_CNT__     30
#endif

#if __DISP%Instance%_CFG_OPTIMIZE_DIRTY_REGIONS__
#   if      !defined(__DISP%Instance%_CFG_DIRTY_REGION_POOL_SIZE__)             \
        ||  __DISP%Instance%_CFG_DIRTY_REGION_POOL_SIZE__ < 4
#       undef __DISP%Instance%_CFG_DIRTY_REGION_POOL_SIZE__
#       define __DISP%Instance%_CFG_DIRTY_REGION_POOL_SIZE__            4
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern uint32_t SystemCoreClock;

/*============================ PROTOTYPES ====================================*/
extern 
int32_t Disp%Instance%_DrawBitmap(int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        const uint8_t *bitmap);

/*============================ LOCAL VARIABLES ===============================*/

ARM_NOINIT 
arm_2d_scene_player_t DISP%Instance%_ADAPTER;

#if __DISP%Instance%_CFG_ENABLE_3FB_HELPER_SERVICE__
ARM_NOINIT
static 
arm_2d_helper_3fb_t s_tDirectModeHelper;
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
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptTile);

    arm_2d_canvas(ptTile, __top_container) {
    
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
        
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

#if !__DISP%Instance%_CFG_DISABLE_NAVIGATION_LAYER__
__WEAK
IMPL_PFB_ON_DRAW(__disp_adapter%Instance%_draw_navigation)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
    arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
    arm_lcd_text_set_draw_region(NULL);

    /* draw real-time FPS info */
    if (__DISP%Instance%_CFG_ITERATION_CNT__) {
        arm_2dp_fill_colour_with_opacity(
                    NULL, 
                    ptTile, 
                    (arm_2d_region_t []){
                        {
                            .tLocation = {
                                .iX = 0,
                                .iY = ((__DISP%Instance%_CFG_SCEEN_HEIGHT__ + 7) / 8 - 2) * 8},
                            .tSize = {
                                .iWidth = __DISP%Instance%_CFG_SCEEN_WIDTH__,
                                .iHeight = 8,
                            },
                        },
                    }, 
                    (__arm_2d_color_t){__RGB(64,64,64)}, 
                    255 - 32);
        arm_2d_op_wait_async(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_GREEN, GLCD_COLOR_WHITE);
        arm_lcd_text_location((__DISP%Instance%_CFG_SCEEN_HEIGHT__ + 7) / 8 - 2,
                              0);

        if (DISP%Instance%_ADAPTER.Benchmark.wAverage) {
            arm_lcd_printf(
                "FPS:%3d:%dms ",
                MIN(arm_2d_helper_get_reference_clock_frequency() / DISP%Instance%_ADAPTER.Benchmark.wAverage, 999),
                (int32_t)arm_2d_helper_convert_ticks_to_ms(DISP%Instance%_ADAPTER.Benchmark.wAverage));
        }

#if __DISP%Instance%_CFG_SCEEN_WIDTH__ >= 240
        arm_lcd_printf( 
            "CPU:%2.2f%% LCD-Latency:%2dms", 
            DISP%Instance%_ADAPTER.Benchmark.fCPUUsage,
            (int32_t)arm_2d_helper_convert_ticks_to_ms(DISP%Instance%_ADAPTER.Benchmark.wLCDLatency));
#else
        arm_lcd_printf( 
            "LCD:%2dms",
            (int32_t)arm_2d_helper_convert_ticks_to_ms(DISP%Instance%_ADAPTER.Benchmark.wLCDLatency) );
#endif
    }

#if __DISP%Instance%_CFG_SCEEN_WIDTH__ >= 320 

    /* draw verion info on the bottom right corner */
    arm_lcd_text_set_colour(GLCD_COLOR_LIGHT_GREY, GLCD_COLOR_WHITE);
    arm_lcd_text_location( (__DISP%Instance%_CFG_SCEEN_HEIGHT__ + 7) / 8 - 2, 
                            (__DISP%Instance%_CFG_SCEEN_WIDTH__ / 6) - 12);
    arm_lcd_printf("v" 
                    ARM_TO_STRING(ARM_2D_VERSION_MAJOR)
                    "."
                    ARM_TO_STRING(ARM_2D_VERSION_MINOR)
                    "."
                    ARM_TO_STRING(ARM_2D_VERSION_PATCH)
                    "-"
                    ARM_2D_VERSION_STR
                    );
#endif

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}
#endif

#if __DISP%Instance%_CFG_ENABLE_3FB_HELPER_SERVICE__
__WEAK
IMPL_PFB_ON_LOW_LV_RENDERING(__disp_adapter%Instance%_pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    if (__arm_2d_helper_3fb_draw_bitmap(&s_tDirectModeHelper,
                                        ptPFB)) {

        arm_2d_helper_pfb_report_rendering_complete(
                        &DISP%Instance%_ADAPTER.use_as__arm_2d_helper_pfb_t);
    }
}

void *disp_adapter%Instance%_3fb_get_flush_pointer(void)
{
    return arm_2d_helper_3fb_get_flush_pointer(&s_tDirectModeHelper);
}


#   if __DISP%Instance%_CFG_ENABLE_ASYNC_FLUSHING__
/* using asynchronous flushing, e.g. using DMA + ISR to offload CPU etc. 
 * It can significantly reduce the LCD Latency hence improve the overrall 
 * framerate. 
 */

void disp_adapter%Instance%_insert_2d_copy_complete_event_handler(void)
{
    arm_2d_helper_pfb_report_rendering_complete(
                    &DISP%Instance%_ADAPTER.use_as__arm_2d_helper_pfb_t);
}

/* using asynchronous flushing, e.g. using DMA + ISR to offload CPU etc. 
 * It can significantly reduce the LCD Latency hence improve the overrall 
 * framerate. 
 */

void disp_adapter%Instance%_insert_dma_copy_complete_event_handler(void)
{
    arm_2d_helper_3fb_report_dma_copy_complete(&s_tDirectModeHelper);
}


#   endif

#else
#   if __DISP%Instance%_CFG_ENABLE_ASYNC_FLUSHING__

/* using asynchronous flushing, e.g. using DMA + ISR to offload CPU etc. 
 * It can significantly reduce the LCD Latency hence improve the overrall 
 * framerate. 
 */

void disp_adapter%Instance%_insert_async_flushing_complete_event_handler(void)
{
    arm_2d_helper_pfb_report_rendering_complete(
                    &DISP%Instance%_ADAPTER.use_as__arm_2d_helper_pfb_t);
}

__WEAK
IMPL_PFB_ON_LOW_LV_RENDERING(__disp_adapter%Instance%_pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    /* request an asynchronous flushing */
    __disp_adapter%Instance%_request_async_flushing(
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
IMPL_PFB_ON_LOW_LV_RENDERING(__disp_adapter%Instance%_pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    Disp%Instance%_DrawBitmap(ptTile->tRegion.tLocation.iX,
                    ptTile->tRegion.tLocation.iY,
                    ptTile->tRegion.tSize.iWidth,
                    ptTile->tRegion.tSize.iHeight,
                    (const uint8_t *)ptTile->pchBuffer);

    arm_2d_helper_pfb_report_rendering_complete(
                    &DISP%Instance%_ADAPTER.use_as__arm_2d_helper_pfb_t);
}
#   endif
#endif

static bool __on_each_frame_complete(void *ptTarget)
{
    ARM_2D_UNUSED(ptTarget);
    
    int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();
    
#if __DISP%Instance%_CFG_FPS_CACULATION_MODE__ == ARM_2D_FPS_MODE_REAL
    static int64_t s_lLastTimeStamp = 0;

    int32_t nElapsed = 0;
    if (0 != s_lLastTimeStamp) {
        nElapsed = (int32_t)(lTimeStamp - s_lLastTimeStamp);
    }
    s_lLastTimeStamp = lTimeStamp;
    
#else /* __DISP%Instance%_CFG_FPS_CACULATION_MODE__ == ARM_2D_FPS_MODE_RENDER_ONLY */
    int32_t nElapsed = DISP%Instance%_ADAPTER.use_as__arm_2d_helper_pfb_t.Statistics.nTotalCycle;
#endif

    int32_t nTotalLCDCycCount = DISP%Instance%_ADAPTER.use_as__arm_2d_helper_pfb_t.Statistics.nRenderingCycle;
    DISP%Instance%_ADAPTER.Benchmark.wLCDLatency = nTotalLCDCycCount;

    /* calculate real-time FPS */
    if (__DISP%Instance%_CFG_ITERATION_CNT__) {
        if (DISP%Instance%_ADAPTER.Benchmark.wIterations) {
            DISP%Instance%_ADAPTER.Benchmark.wMin = MIN((uint32_t)nElapsed, DISP%Instance%_ADAPTER.Benchmark.wMin);
            DISP%Instance%_ADAPTER.Benchmark.wMax = MAX(nElapsed, (int32_t)DISP%Instance%_ADAPTER.Benchmark.wMax);
            DISP%Instance%_ADAPTER.Benchmark.dwTotal += nElapsed;
            DISP%Instance%_ADAPTER.Benchmark.dwRenderTotal += DISP%Instance%_ADAPTER.use_as__arm_2d_helper_pfb_t.Statistics.nTotalCycle;
            DISP%Instance%_ADAPTER.Benchmark.wIterations--;

            if (0 == DISP%Instance%_ADAPTER.Benchmark.wIterations) {
                DISP%Instance%_ADAPTER.Benchmark.wAverage =
                    (uint32_t)(DISP%Instance%_ADAPTER.Benchmark.dwTotal / (uint64_t)__DISP%Instance%_CFG_ITERATION_CNT__);
                DISP%Instance%_ADAPTER.Benchmark.wAverage = MAX(1, DISP%Instance%_ADAPTER.Benchmark.wAverage);
 
                int64_t lElapsed = lTimeStamp - DISP%Instance%_ADAPTER.Benchmark.lTimestamp;
                DISP%Instance%_ADAPTER.Benchmark.fCPUUsage = (float)((double)DISP%Instance%_ADAPTER.Benchmark.dwRenderTotal / (double)lElapsed) * 100.0f;
                 
                DISP%Instance%_ADAPTER.Benchmark.wMin = UINT32_MAX;
                DISP%Instance%_ADAPTER.Benchmark.wMax = 0;
                DISP%Instance%_ADAPTER.Benchmark.dwTotal = 0;
                DISP%Instance%_ADAPTER.Benchmark.dwRenderTotal = 0;
                DISP%Instance%_ADAPTER.Benchmark.wIterations = __DISP%Instance%_CFG_ITERATION_CNT__;
                
                DISP%Instance%_ADAPTER.Benchmark.lTimestamp = arm_2d_helper_get_system_timestamp();
            }
        }
    }
    
    return true;
}

static void __user_scene_player_init(void)
{
    memset(&DISP%Instance%_ADAPTER, 0, sizeof(DISP%Instance%_ADAPTER));

#if __DISP%Instance%_CFG_OPTIMIZE_DIRTY_REGIONS__
    ARM_NOINIT
    static arm_2d_region_list_item_t s_tDirtyRegionList[__DISP%Instance%_CFG_DIRTY_REGION_POOL_SIZE__]; 
#endif

    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
        &DISP%Instance%_ADAPTER.use_as__arm_2d_helper_pfb_t,                            //!< FPB Helper object
        __DISP%Instance%_CFG_SCEEN_WIDTH__,                                     //!< screen width
        __DISP%Instance%_CFG_SCEEN_HEIGHT__,                                    //!< screen height
        COLOUR_INT,                                                             //!< colour date type
        __DISP%Instance%_CFG_PFB_BLOCK_WIDTH__,                                 //!< PFB block width
        __DISP%Instance%_CFG_PFB_BLOCK_HEIGHT__,                                //!< PFB block height
        __DISP%Instance%_CFG_PFB_HEAP_SIZE__                                    //!< number of PFB in the PFB pool

#if     __DISP%Instance%_CFG_VIRTUAL_RESOURCE_HELPER__                          \
    &&  !__DISP%Instance%_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__
        + 3
#endif
        ,{
            .evtOnLowLevelRendering = {
                //! callback for low level rendering
                .fnHandler = &__disp_adapter%Instance%_pfb_render_handler,
            },
            .evtOnEachFrameCPL = {
                .fnHandler = &__on_each_frame_complete,
            },
        },
#if __DISP%Instance%_CFG_SWAP_RGB16_HIGH_AND_LOW_BYTES__
        .FrameBuffer.bSwapRGB16 = true,
#endif
#if __DISP%Instance%_CFG_DEBUG_DIRTY_REGIONS__
        .FrameBuffer.bDebugDirtyRegions = true,
#endif
        .FrameBuffer.u3PixelWidthAlign = __DISP%Instance%_CFG_PFB_PIXEL_ALIGN_WIDTH__,
        .FrameBuffer.u3PixelHeightAlign = __DISP%Instance%_CFG_PFB_PIXEL_ALIGN_HEIGHT__,
#if     __DISP%Instance%_CFG_VIRTUAL_RESOURCE_HELPER__                          \
    &&  !__DISP%Instance%_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__
        .FrameBuffer.u4PoolReserve = 3,                                         // reserve 3 PFB blocks for the virtual resource service
#endif
#if __DISP%Instance%_CFG_OPTIMIZE_DIRTY_REGIONS__
        .DirtyRegion.ptRegions = s_tDirtyRegionList,
        .DirtyRegion.chCount = dimof(s_tDirtyRegionList),
#endif
    ) < 0) {
        //! error detected
        assert(false);
    }

#if __DISP%Instance%_CFG_ENABLE_3FB_HELPER_SERVICE__
    do {
    extern uintptr_t __DISP_ADAPTER%Instance%_3FB_FB0_ADDRESS__;
    extern uintptr_t __DISP_ADAPTER%Instance%_3FB_FB1_ADDRESS__;
    extern uintptr_t __DISP_ADAPTER%Instance%_3FB_FB2_ADDRESS__;
    
    extern arm_2d_helper_2d_copy_handler_t __disp_adapter%Instance%_request_2d_copy;
    extern arm_2d_helper_dma_copy_handler_t __disp_adapter%Instance%_request_dma_copy;
    
    
        arm_2d_helper_3fb_cfg_t tCFG = {
            .tScreenSize = {
                __DISP%Instance%_CFG_SCEEN_WIDTH__,
                __DISP%Instance%_CFG_SCEEN_HEIGHT__,
            },
            .chPixelBits = __DISP%Instance%_CFG_COLOUR_DEPTH__,
            .pnAddress = {
                [0] = ((uintptr_t)__DISP_ADAPTER%Instance%_3FB_FB0_ADDRESS__),
                [1] = ((uintptr_t)__DISP_ADAPTER%Instance%_3FB_FB1_ADDRESS__),
                [2] = ((uintptr_t)__DISP_ADAPTER%Instance%_3FB_FB2_ADDRESS__),
            },
#if __DISP0_CFG_ENABLE_ASYNC_FLUSHING__
            .evtOn2DCopy = {
                .fnHandler = __disp_adapter%Instance%_request_2d_copy,
            },
            .evtOnDMACopy = {
                .fnHandler = __disp_adapter%Instance%_request_dma_copy,
            },
#endif
        };
        
        arm_2d_helper_3fb_init(&s_tDirectModeHelper, &tCFG);
    
    } while(0);
#endif

    arm_lcd_text_init((arm_2d_region_t []) {
                        { .tSize = {
                            .iWidth = __DISP%Instance%_CFG_SCEEN_WIDTH__,
                            .iHeight = __DISP%Instance%_CFG_SCEEN_HEIGHT__,
                        }}});

    DISP%Instance%_ADAPTER.Benchmark.wMin = UINT32_MAX;
    DISP%Instance%_ADAPTER.Benchmark.wIterations = __DISP%Instance%_CFG_ITERATION_CNT__;

}

#if !__DISP%Instance%_CFG_DISABLE_NAVIGATION_LAYER__
__WEAK 
void disp_adapter%Instance%_navigator_init(void)
{
    /*! define dirty regions for the navigation layer */
    IMPL_ARM_2D_REGION_LIST(s_tNavDirtyRegionList, static)

        /* a region for the status bar on the bottom of the screen */
        ADD_LAST_REGION_TO_LIST(s_tNavDirtyRegionList,
            .tLocation = {
                .iX = 0,
                .iY = ((__DISP%Instance%_CFG_SCEEN_HEIGHT__ + 7) / 8 - 2) * 8},
            .tSize = {
                .iWidth = __DISP%Instance%_CFG_SCEEN_WIDTH__,
                .iHeight = 8,
            },
        ),

    END_IMPL_ARM_2D_REGION_LIST()
    /* register event handler for evtOnDrawNavigation */
    arm_2d_scene_player_register_on_draw_navigation_event_handler(
                    &DISP%Instance%_ADAPTER,
                    __disp_adapter%Instance%_draw_navigation,
                    NULL,
                    (arm_2d_region_list_item_t *)s_tNavDirtyRegionList);
}
#else
__WEAK 
void disp_adapter%Instance%_navigator_init(void)
{

}
#endif

/*----------------------------------------------------------------------------*
 * Display Adapter Entry                                                      *
 *----------------------------------------------------------------------------*/

void disp_adapter%Instance%_init(void)
{
    __user_scene_player_init();

    arm_extra_controls_init();

    disp_adapter%Instance%_navigator_init();

    DISP%Instance%_ADAPTER.Benchmark.lTimestamp = arm_2d_helper_get_system_timestamp();

    if (!__DISP%Instance%_CFG_DISABLE_DEFAULT_SCENE__) {
    #if 0
        /*! define dirty regions */
        IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, const static)

            /* a region for the busy wheel */
            ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
                .tLocation = {
                    .iX = ((__DISP%Instance%_CFG_SCEEN_WIDTH__ - 100) >> 1),
                    .iY = ((__DISP%Instance%_CFG_SCEEN_HEIGHT__ - 100) >> 1),
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
                .fnScene        = &__pfb_draw_handler,
                //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
                .fnOnFrameStart = &__on_frame_start,
                .fnOnFrameCPL   = &__on_frame_complete,
                .fnDepose       = NULL,
            },
        };
        arm_2d_scene_player_append_scenes( 
                                        &DISP%Instance%_ADAPTER,
                                        (arm_2d_scene_t *)s_tScenes,
                                        dimof(s_tScenes));
    }
}

arm_fsm_rt_t disp_adapter%Instance%_task(void)
{
    return arm_2d_scene_player_task(&DISP%Instance%_ADAPTER);
}


/*----------------------------------------------------------------------------*
 * Virtual Resource Helper                                                    *
 *----------------------------------------------------------------------------*/

#if __DISP%Instance%_CFG_VIRTUAL_RESOURCE_HELPER__

__WEAK
void * __disp_adapter%Instance%_aligned_malloc(size_t nSize, size_t nAlign)
{
    ARM_2D_UNUSED(nAlign);

    /* ensure nAlign is 2^n */
    assert((((~nAlign) + 1) & nAlign) == nAlign);

    void * pMem = malloc(nSize);
    assert( 0 == ((uintptr_t)pMem & (nAlign - 1)));
    return pMem;
}

__WEAK
void __disp_adapter%Instance%_free(void *pMem)
{
    if (NULL != pMem) {
        free(pMem);
    }
}



intptr_t __disp_adapter%Instance%_vres_asset_loader (
                                            uintptr_t pObj, 
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
    
    /* default condition */
    tBufferSize = ptRegion->tSize.iHeight * nBytesPerLine;
    
    
#if __DISP%Instance%_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__
    pBuffer = __disp_adapter%Instance%_aligned_malloc(tBufferSize, nPixelSize);
    assert(NULL != pBuffer);

    if (NULL == pBuffer) {
        return (intptr_t)NULL;
    }
#else
    arm_2d_pfb_t *ptPFB = __arm_2d_helper_pfb_new(&DISP%Instance%_ADAPTER.use_as__arm_2d_helper_pfb_t);
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
        uintptr_t pSrc = __disp_adapter%Instance%_vres_get_asset_address(pObj, ptVRES);
        uintptr_t pDes = (uintptr_t)pBuffer;
        
        uint32_t iBitsperLineInSource = ptVRES->tTile.tRegion.tSize.iWidth * nBitsPerPixel;
        int16_t iSourceStride = (int16_t)((uint32_t)(iBitsperLineInSource + 7) >> 3);

        /* calculate offset */
        pSrc += (ptRegion->tLocation.iY * iSourceStride);
        pSrc += (ptRegion->tLocation.iX * nBitsPerPixel) >> 3;
        
        for (int_fast16_t y = 0; y < ptRegion->tSize.iHeight; y++) {
            __disp_adapter%Instance%_vres_read_memory(   pObj, 
                                                (void *)pDes, 
                                                (uintptr_t)pSrc, 
                                                nBytesPerLine);

            pDes += nBytesPerLine;
            pSrc += iSourceStride;
        }
    } else {
        uintptr_t pSrc = __disp_adapter%Instance%_vres_get_asset_address(pObj, ptVRES);
        uintptr_t pDes = (uintptr_t)pBuffer;
        int16_t iTargetStride = ptRegion->tSize.iWidth;
        int16_t iSourceStride = ptVRES->tTile.tRegion.tSize.iWidth;

        /* calculate offset */
        pSrc += (ptRegion->tLocation.iY * iSourceStride + ptRegion->tLocation.iX) * nPixelSize;
        
        for (int_fast16_t y = 0; y < ptRegion->tSize.iHeight; y++) {
            __disp_adapter%Instance%_vres_read_memory( 
                                            pObj, 
                                            (void *)pDes, 
                                            (uintptr_t)pSrc, 
                                            nPixelSize * iTargetStride);
            
            pDes += iTargetStride * nPixelSize;
            pSrc += iSourceStride * nPixelSize;
        }
    } while(0);
    
    return (intptr_t)pBuffer;
}


void __disp_adapter%Instance%_vres_buffer_deposer (
                                            uintptr_t pTarget, 
                                            arm_2d_vres_t *ptVRES, 
                                            intptr_t pBuffer )
{
#if __DISP%Instance%_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptVRES);

    if ((intptr_t)NULL != pBuffer) {
        __disp_adapter%Instance%_free((void *)pBuffer);
    }
#else
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptVRES);
    if ((intptr_t)NULL == pBuffer) {
        return ;
    }
    
    arm_2d_pfb_t *ptPFB = (arm_2d_pfb_t *)((uintptr_t)pBuffer - sizeof(arm_2d_pfb_t));
    __arm_2d_helper_pfb_free(&DISP%Instance%_ADAPTER.use_as__arm_2d_helper_pfb_t, ptPFB);
#endif
}

#endif


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


