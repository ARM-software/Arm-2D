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

#define __USER_SCENE_MONO_HISTOGRAM_IMPLEMENT__
#include "arm_2d_scene_mono_histogram.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

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
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion" 
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 64,177
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe177,Pe174
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#   pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
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

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static
const uint8_t c_chScanLineVerticalLineMask[]= {
    255, 0
};

static
const arm_2d_tile_t c_tileLineVerticalLineMask = {
    .tRegion = {
        .tSize = {
            .iWidth = 1,
            .iHeight = 2,
        },
    },
    .tInfo = {
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {
            .chScheme = ARM_2D_COLOUR_8BIT,
        },
    },
    .pchBuffer = (uint8_t *)(c_chScanLineVerticalLineMask),
};


/*============================ IMPLEMENTATION ================================*/

static void __on_scene_mono_histogram_load(arm_2d_scene_t *ptScene)
{
    user_scene_mono_histogram_t *ptThis = (user_scene_mono_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __after_scene_mono_histogram_switching(arm_2d_scene_t *ptScene)
{
    user_scene_mono_histogram_t *ptThis = (user_scene_mono_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_mono_histogram_depose(arm_2d_scene_t *ptScene)
{
    user_scene_mono_histogram_t *ptThis = (user_scene_mono_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene mono_histogram                                                       *
 *----------------------------------------------------------------------------*/

static void __on_scene_mono_histogram_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_mono_histogram_t *ptThis = (user_scene_mono_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_mono_histogram_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_mono_histogram_t *ptThis = (user_scene_mono_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_mono_histogram_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_mono_histogram_t *ptThis = (user_scene_mono_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    do {
        /* a simple sin wave generator */
        int32_t nResult;
        arm_2d_helper_time_cos_slider(0, 1000, 1000, ARM_2D_ANGLE(0.0f), &nResult, &this.lTimestamp[1]);

        /* enqueue */
        histogram_enqueue_new_value(ptThis, nResult);
    } while(0);

    histogram_on_frame_start(&this.tHistogram);
}

static void __on_scene_mono_histogram_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_mono_histogram_t *ptThis = (user_scene_mono_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if 0
    /* switch to next scene after 5s */
    if (arm_2d_helper_is_time_out(5000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene_mono_histogram_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_mono_histogram_t *ptThis = (user_scene_mono_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mono_histogram_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_mono_histogram_t *ptThis = (user_scene_mono_histogram_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {


        arm_2d_align_centre(__top_canvas, 128 - 16, 64 - 8) { 

            arm_2d_layout(__centre_region) {
                __item_line_dock_vertical(10) {

                    
                    arm_2d_fill_colour(ptTile, &__item_region, GLCD_COLOR_WHITE);

                    /* print label */
                    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                    arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
                    arm_lcd_text_set_draw_region(&__item_region);
                    //arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
                    arm_lcd_text_set_display_mode(ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR);
                    
                    arm_lcd_printf_label(ARM_2D_ALIGN_MIDDLE_LEFT, " histogram");
                    arm_lcd_printf_label(ARM_2D_ALIGN_MIDDLE_RIGHT, "_x");

                }

                __item_line_vertical(histogram_get_size(&this.tHistogram))
                {
                    histogram_show( &this.tHistogram,
                                    ptTile,
                                    &__item_region,
                                    255);
                }
            }

        }

    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
void histogram_enqueue_new_value(user_scene_mono_histogram_t *ptThis, 
                                 int_fast16_t iValue)
{
    assert(NULL != ptThis);

    arm_irq_safe {  /* ensure the enqueue access thread-safe */
        this.WindowFIFO.iBuffer[this.WindowFIFO.hwPointer++] = iValue;
        if (this.WindowFIFO.hwPointer >= dimof(this.WindowFIFO.iBuffer)) {
            this.WindowFIFO.hwPointer = 0;
        }
    }
}

static
int32_t histogram_get_bin_value(void *pTarget, 
                                histogram_t *ptHistogram, 
                                uint_fast16_t hwBinIndex)
{
    ARM_2D_UNUSED(ptHistogram);
    user_scene_mono_histogram_t *ptThis = (user_scene_mono_histogram_t *)pTarget;

    //arm_irq_safe {    /* no need this protection */
        uint16_t hwAccessIndex = (hwBinIndex + this.WindowFIFO.hwPointer);
        hwAccessIndex -= (hwAccessIndex >= dimof(this.WindowFIFO.iBuffer)) 
                    * dimof(this.WindowFIFO.iBuffer);
    //}

    return (int32_t)this.WindowFIFO.iBuffer[hwAccessIndex];
}


ARM_NONNULL(1)
user_scene_mono_histogram_t *__arm_2d_scene_mono_histogram_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_mono_histogram_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);


    if (NULL == ptThis) {
        ptThis = (user_scene_mono_histogram_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_mono_histogram_t),
                                                        __alignof__(user_scene_mono_histogram_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_mono_histogram_t));

    *ptThis = (user_scene_mono_histogram_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_mono_histogram_load,
            .fnScene        = &__pfb_draw_scene_mono_histogram_handler,
            //.fnAfterSwitch  = &__after_scene_mono_histogram_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_mono_histogram_background_start,
            //.fnOnBGComplete = &__on_scene_mono_histogram_background_complete,
            .fnOnFrameStart = &__on_scene_mono_histogram_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_mono_histogram_switching_out,
            .fnOnFrameCPL   = &__on_scene_mono_histogram_frame_complete,
            .fnDepose       = &__on_scene_mono_histogram_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_mono_histogram_t begin ---------------*/

    do {
        histogram_cfg_t tCFG = {
            .Bin = {
                .tSize = {6, 46},
                .chPadding = 2,
                .bUseScanLine = true,
                //.bDrawEndPointOnly = true,
                //.bSupportNegative = true,
                .nMaxValue = 1000,

                .ptItems = this.tBins,
                .hwCount = dimof(this.tBins),

                .ptVerticalLineMask = &c_tileLineVerticalLineMask,
            },

            .Colour = {
                .wFrom =    __RGB32(0xFF, 0xFF, 0xFF),
                .wTo =      __RGB32(0xFF, 0xFF, 0xFF), 
            },

            .ptParent = &this.use_as__arm_2d_scene_t,

            .evtOnGetBinValue = {
                .fnHandler = &histogram_get_bin_value,
                .pTarget = ptThis,
            },
        };

        histogram_init(&this.tHistogram, &tCFG);
    } while(0);

    /* ------------   initialize members of user_scene_mono_histogram_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


