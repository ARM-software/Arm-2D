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

#ifdef __RTE_ACCELERATION_ARM_2D_HELPER_DISP_ADAPTER0__

#include <stdio.h>
#include "arm_2d_helper.h"
#include "arm_extra_lcd_printf.h"
#include "arm_extra_controls.h"
#include "__common.h"
#include "arm_2d_disp_adapter_0.h"

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

#ifndef __DISP0_CFG_ITERATION_CNT
#   define __DISP0_CFG_ITERATION_CNT       30
#endif

#ifndef __DISP0_CFG_PFB_BLOCK_WIDTH__
#   define __DISP0_CFG_PFB_BLOCK_WIDTH__   __DISP0_CFG_SCEEN_WIDTH__
#endif

#ifndef __DISP0_CFG_PFB_BLOCK_WIDTH__
#   define __DISP0_CFG_PFB_BLOCK_WIDTH__   __DISP0_CFG_SCEEN_HEIGHT__
#endif

#if __DISP0_CFG_COLOUR_DEPTH__ == 8
#   define __arm_2d_color_t         arm_2d_color_gray8_t
#   define COLOUR_INT               uint8_t
#   define arm_2d_fill_colour       arm_2d_c8it_fill_colour
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_gray8_fill_colour_with_opacity
#elif __DISP0_CFG_COLOUR_DEPTH__ == 16
#   define __arm_2d_color_t         arm_2d_color_rgb565_t
#   define COLOUR_INT               uint16_t
#   define arm_2d_fill_colour       arm_2d_rgb16_fill_colour
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_rgb565_fill_colour_with_opacity
#elif __DISP0_CFG_COLOUR_DEPTH__ == 32
#   define __arm_2d_color_t         arm_2d_color_cccn888_t
#   define COLOUR_INT               uint32_t
#   define arm_2d_fill_colour       arm_2d_rgb32_fill_colour
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_cccn888_fill_colour_with_opacity
#else
#   error Unsupported colour depth!
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

ARM_NOINIT 
arm_2d_scene_player_t DISP0_ADAPTER;

static struct {
    uint32_t wMin;
    uint32_t wMax;
    uint64_t dwTotal;
    uint32_t wAverage;
    uint32_t wIterations;
    uint32_t wLCDLatency;
} BENCHMARK = {
    .wMin = UINT32_MAX,
    .wMax = 0,
    .dwTotal = 0,
    .wAverage = 0,
    .wIterations = __DISP0_CFG_ITERATION_CNT,
};


/*============================ IMPLEMENTATION ================================*/
static void __on_frame_start(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
    
}

static void on_frame_complete(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
    
    int32_t nTotalCyclCount = DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t.Statistics.nTotalCycle;
    int32_t nTotalLCDCycCount = DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t.Statistics.nRenderingCycle;
    BENCHMARK.wLCDLatency = nTotalLCDCycCount;

    if (__DISP0_CFG_ITERATION_CNT) {
        if (BENCHMARK.wIterations) {
            BENCHMARK.wMin = MIN((uint32_t)nTotalCyclCount, BENCHMARK.wMin);
            BENCHMARK.wMax = MAX(nTotalCyclCount, (int32_t)BENCHMARK.wMax);
            BENCHMARK.dwTotal += nTotalCyclCount;
            BENCHMARK.wIterations--;

            if (0 == BENCHMARK.wIterations) {
                BENCHMARK.wAverage =
                    (uint32_t)(BENCHMARK.dwTotal / (uint64_t)__DISP0_CFG_ITERATION_CNT);
                    
                BENCHMARK.wMin = UINT32_MAX;
                BENCHMARK.wMax = 0;
                BENCHMARK.dwTotal = 0;
                BENCHMARK.wIterations = __DISP0_CFG_ITERATION_CNT;
            }
        }
    }
}


static
IMPL_PFB_ON_DRAW(__pfb_draw_handler)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    
    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
    
    arm_2d_align_centre(ptTile->tRegion, 100, 100) {
        draw_round_corner_box(  ptTile,
                                &__centre_region,
                                GLCD_COLOR_BLACK,
                                64,
                                bIsNewFrame);
    }
    
    busy_wheel2_show(ptTile, bIsNewFrame);
    
    if (__DISP0_CFG_ITERATION_CNT) {
        arm_2dp_fill_colour_with_opacity(
                    NULL, 
                    ptTile, 
                    (arm_2d_region_t []){
                        {
                            .tLocation = {
                                .iX = 0,
                                .iY = __DISP0_CFG_SCEEN_HEIGHT__ - 9},
                            .tSize = {
                                .iWidth = __DISP0_CFG_SCEEN_WIDTH__,
                                .iHeight = 9,
                            },
                        },
                    }, 
                    (__arm_2d_color_t){__RGB(64,64,64)}, 
                    255 - 32);
        arm_2d_op_wait_async(NULL);
        
        arm_lcd_text_location( (__DISP0_CFG_SCEEN_HEIGHT__ + 7) / 8 - 1, 0);
        if (BENCHMARK.wAverage) {
            arm_lcd_printf("FPS: %3d:%dms   ",
                            SystemCoreClock / BENCHMARK.wAverage,
                            BENCHMARK.wAverage / (SystemCoreClock / 1000ul));
        }
        arm_lcd_printf("LCD Latency: %2dms", BENCHMARK.wLCDLatency / (SystemCoreClock / 1000ul) );
    } 
    
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_background_handler)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

    arm_2d_align_centre(ptTile->tRegion, 100, 100) {
        draw_round_corner_box(  ptTile,
                                &__centre_region,
                                GLCD_COLOR_BLACK,
                                64,
                                bIsNewFrame);
    }
                            
    

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

__WEAK
IMPL_PFB_ON_LOW_LV_RENDERING(__glcd0_pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    Disp0_DrawBitmap(ptTile->tRegion.tLocation.iX,
                    ptTile->tRegion.tLocation.iY,
                    ptTile->tRegion.tSize.iWidth,
                    ptTile->tRegion.tSize.iHeight,
                    (const uint8_t *)ptTile->pchBuffer);

    arm_2d_helper_pfb_report_rendering_complete(
                    &DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t,
                    (arm_2d_pfb_t *)ptPFB);
}


static void __user_scene_player_init(void)
{
    memset(&DISP0_ADAPTER, 0, sizeof(DISP0_ADAPTER));

    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
        &DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t,                            //!< FPB Helper object
        __DISP0_CFG_SCEEN_WIDTH__,                                     //!< screen width
        __DISP0_CFG_SCEEN_HEIGHT__,                                    //!< screen height
        COLOUR_INT,                                                             //!< colour date type
        __DISP0_CFG_PFB_BLOCK_WIDTH__,                                 //!< PFB block width
        __DISP0_CFG_PFB_BLOCK_HEIGHT__,                                //!< PFB block height
        1,                                                                      //!< number of PFB in the PFB pool
        {
            .evtOnLowLevelRendering = {
                //! callback for low level rendering
                .fnHandler = &__glcd0_pfb_render_handler,
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

void disp_adapter0_init(void)
{
    arm_extra_controls_init();
    
    __user_scene_player_init();
    
    do {
        /*! define dirty regions */
        IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, const static)

            /* a region for the busy wheel */
            ADD_REGION_TO_LIST(s_tDirtyRegions,
                .tLocation = {
                    .iX = ((__DISP0_CFG_SCEEN_WIDTH__ - 100) >> 1),
                    .iY = ((__DISP0_CFG_SCEEN_HEIGHT__ - 100) >> 1),
                },
                .tSize = {
                    .iWidth = 100,
                    .iHeight = 100,
                },
            ),

            /* a region for the status bar on the top of the screen */
            ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
                .tLocation = {
                    .iX = 0,
                    .iY = __DISP0_CFG_SCEEN_HEIGHT__ - 9},
                .tSize = {
                    .iWidth = __DISP0_CFG_SCEEN_WIDTH__,
                    .iHeight = 9,
                },
            ),

        END_IMPL_ARM_2D_REGION_LIST()
        
        static arm_2d_scene_t s_tScenes[] = {
            [0] = {
                .fnBackground   = &__pfb_draw_background_handler,
                .fnScene        = &__pfb_draw_handler,
                .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
                .fnOnFrameStart = &__on_frame_start,
                .fnOnFrameCPL   = &on_frame_complete,
                .fnDepose       = NULL,
            },
        };
        arm_2d_user_scene_player_append_scenes( 
                                        &DISP0_ADAPTER,
                                        (arm_2d_scene_t *)s_tScenes);
    } while(0);
}

arm_fsm_rt_t disp_adapter0_task(void)
{
    return arm_2d_user_scene_player_task(&DISP0_ADAPTER);
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


