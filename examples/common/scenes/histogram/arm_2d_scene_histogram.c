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

#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include "arm_2d.h"

#define __USER_SCENE_HISTOGRAM_IMPLEMENT__
#include "arm_2d_scene_histogram.h"

#include "arm_2d_helper.h"
#include "arm_2d_example_controls.h"

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

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


static void __on_scene_histogram_depose(arm_2d_scene_t *ptScene)
{
    user_scene_histogram_t *ptThis = (user_scene_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    histogram_depose(&this.tHistogram);

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene histogram                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_histogram_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_histogram_t *ptThis = (user_scene_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_histogram_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_histogram_t *ptThis = (user_scene_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_histogram_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_histogram_t *ptThis = (user_scene_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);


    for (int32_t n = 0; n < dimof(this.tBins); n++) {
        int32_t nResult;
        arm_2d_helper_time_cos_slider(0, 1000, 1000, ARM_2D_ANGLE(15.0f * (float)n), &nResult, &this.lTimestamp[1+n]);

        this.tBins[n].iNewValue = (int16_t)nResult;
    }


    histogram_on_frame_start(&this.tHistogram);
}

static void __on_scene_histogram_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_histogram_t *ptThis = (user_scene_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /* switch to next scene after 10s */
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static void __before_scene_histogram_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_histogram_t *ptThis = (user_scene_histogram_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_histogram_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_histogram_t *ptThis = (user_scene_histogram_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

        arm_2d_align_centre(__top_canvas, 224, 180 ) {

            histogram_show( &this.tHistogram,
                            ptTile,
                            &__centre_region,
                            255);
        }


        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene histogram");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_histogram_t *__arm_2d_scene_histogram_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_histogram_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_histogram_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_histogram_t),
                                                        __alignof__(user_scene_histogram_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_histogram_t));

    *ptThis = (user_scene_histogram_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnScene        = &__pfb_draw_scene_histogram_handler,
            
            //.fnOnBGStart    = &__on_scene_histogram_background_start,
            //.fnOnBGComplete = &__on_scene_histogram_background_complete,
            .fnOnFrameStart = &__on_scene_histogram_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_histogram_switching_out,
            .fnOnFrameCPL   = &__on_scene_histogram_frame_complete,
            .fnDepose       = &__on_scene_histogram_depose,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_histogram_t begin ---------------*/

    do {
        

        histogram_cfg_t tCFG = {
            .Bin = {
                .tSize = {10, 180},
                .chPadding = 4,
                .u6BinsPerDirtyRegion = 1,
                .bUseScanLine = true,
                .iMaxValue = 1000,

                .ptItems = this.tBins,
                .hwCount = dimof(this.tBins),
            },

            .Colour = {
                .wFrom =    __RGB32(0, 0xFF, 0),
                .wTo =      __RGB32(0xFF, 0, 0), 
            },

            //.ptParent = &this.use_as__arm_2d_scene_t,
        };

        histogram_init(&this.tHistogram, &tCFG);
    } while(0);

    /* ------------   initialize members of user_scene_histogram_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


