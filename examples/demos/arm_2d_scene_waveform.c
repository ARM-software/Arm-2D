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

#define __USER_SCENE_WAVEFORM_IMPLEMENT__
#include "arm_2d_scene_waveform.h"

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

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_LiberationSansRegular14_A4;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

    /* a dirty region to be specified at runtime*/
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),
    
    /* add the last region:
        * it is the top left corner for text display 
        */
    ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
        .tLocation = {
            .iX = 0,
            .iY = 0,
        },
        .tSize = {
            .iWidth = 0,
            .iHeight = 8,
        },
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_waveform_load(arm_2d_scene_t *ptScene)
{
    user_scene_waveform_t *ptThis = (user_scene_waveform_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    waveform_view_on_load(&this.Waveform.tHelper);

}

static void __after_scene_waveform_switching(arm_2d_scene_t *ptScene)
{
    user_scene_waveform_t *ptThis = (user_scene_waveform_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_waveform_depose(arm_2d_scene_t *ptScene)
{
    user_scene_waveform_t *ptThis = (user_scene_waveform_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*--------------------- insert your depose code begin --------------------*/
    waveform_view_depose(&this.Waveform.tHelper);

    /*---------------------- insert your depose code end  --------------------*/

    if (NULL != this.pchBuffer) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, this.pchBuffer);
    }

    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }
    ptScene->ptPlayer = NULL;
    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene waveform                                                                    *
 *----------------------------------------------------------------------------*/
#if 0  /* deprecated */
static void __on_scene_waveform_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_waveform_t *ptThis = (user_scene_waveform_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_waveform_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_waveform_t *ptThis = (user_scene_waveform_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}
#endif

ARM_NONNULL(1)
void arm_2d_scene_waveform_enqueue( user_scene_waveform_t *ptThis, 
                                    int16_t *piSamples, 
                                    uint16_t hwSampleCount)
{
    assert(NULL != ptThis);
    if (NULL == piSamples || 0 == hwSampleCount) {
        return ;
    }

    arm_loader_io_window_enqueue(   &this.Waveform.tWindowIO, 
                                    piSamples, 
                                    hwSampleCount * 2);
}

static void __generate_cos_samples( user_scene_waveform_t *ptThis, 
                                    size_t tCount,
                                    float fStep)
{
    assert(NULL != ptThis);

    if (0 == tCount) {
        return ;
    }
    do {
        this.fDegree += fStep;
        int16_t iData = 900 * arm_cos_f32(ARM_2D_ANGLE(this.fDegree));

        /* add random noise */
        //srand(arm_2d_helper_get_system_timestamp());
        //iData += (rand() & 0x0FF) - 0x07F;

        arm_2d_scene_waveform_enqueue(  ptThis,
                                        &iData,
                                        1);
    } while(--tCount);

    this.fDegree = ARM_2D_FMODF(this.fDegree, 360.0f);
}

static void __on_scene_waveform_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_waveform_t *ptThis = (user_scene_waveform_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    do {

        int32_t nResult;
            /* simulate a full battery charging/discharge cycle */
        arm_2d_helper_time_cos_slider(50, 350, 20000, 0, &nResult, &this.lTimestamp[0]);

        __generate_cos_samples(ptThis, 5, (float)nResult / 100.0f);
        
    } while(0);

    /* NOTE: Please only set true to indicate the diagram data is updated (changed).
     *       Since we change the data for each frame, we pass true here directly.
     */
    waveform_view_on_frame_start(&this.Waveform.tHelper, true);      

}

static void __on_scene_waveform_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_waveform_t *ptThis = (user_scene_waveform_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    waveform_view_on_frame_complete(&this.Waveform.tHelper);

}

static void __before_scene_waveform_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_waveform_t *ptThis = (user_scene_waveform_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_waveform_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_waveform_t *ptThis = (user_scene_waveform_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/

        /* draw a background logo */
        arm_2d_align_centre(__top_canvas, c_tileCMSISLogo.tRegion.tSize) {
            arm_2d_fill_colour_with_a4_mask_and_opacity(   
                                                ptTile, 
                                                &__centre_region, 
                                                &c_tileCMSISLogoA4Mask, 
                                                (__arm_2d_color_t){GLCD_COLOR_BLACK},
                                                64);
        }

        arm_2d_align_centre(__top_canvas, 240, 240 ) {

            arm_2d_dock(__centre_region, 10) {

                /* use the bottom-up layout */
                arm_2d_layout(__dock_region, BOTTOM_UP) {

                    /* waveform */
                    __item_line_dock_vertical(this.Waveform.tHelper.tTile.tRegion.tSize.iHeight + 20) {

                        draw_round_corner_box(  ptTile, 
                                                &__item_region, 
                                                GLCD_COLOR_DARK_GREY, 
                                                32);

                        arm_2d_align_centre(__item_region, this.Waveform.tHelper.tTile.tRegion.tSize) {

                            waveform_view_show( &this.Waveform.tHelper, 
                                                ptTile, 
                                                &__centre_region,
                                                bIsNewFrame);

                        }
                    }

                    /* title */
                    __item_line_dock_vertical() {

                        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                        arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_LiberationSansRegular14_A4);
                        arm_lcd_text_set_draw_region(&__item_region);
                        arm_lcd_text_set_colour(GLCD_COLOR_NIXIE_TUBE, GLCD_COLOR_WHITE);
                        arm_lcd_text_set_scale(1.3f);

                        arm_lcd_printf_label(ARM_2D_ALIGN_LEFT, "+ Waveform Demo");

                        arm_lcd_text_set_scale(0);

                    }
                }
            }                       
        }

        /* draw text at the top-left corner */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene waveform");

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_waveform_t *__arm_2d_scene_waveform_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_waveform_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t __top_canvas
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    /* initialise dirty region 0 at runtime
     * this demo shows that we create a region in the centre of a screen(320*240)
     * for a image stored in the tile c_tileCMSISLogoMask
     */
    arm_2d_align_centre(__top_canvas, c_tileCMSISLogoMask.tRegion.tSize) {
        s_tDirtyRegions[0].tRegion = __centre_region;
    }

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].tRegion.tSize.iWidth 
                                                        = __top_canvas.tSize.iWidth;

    if (NULL == ptThis) {
        ptThis = (user_scene_waveform_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_waveform_t),
                                                        __alignof__(user_scene_waveform_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_waveform_t));

    *ptThis = (user_scene_waveform_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_WHITE}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_waveform_load,
            .fnScene        = &__pfb_draw_scene_waveform_handler,
            .fnAfterSwitch  = &__after_scene_waveform_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene_waveform_background_start,        /* deprecated */
            //.fnOnBGComplete = &__on_scene_waveform_background_complete,     /* deprecated */
            .fnOnFrameStart = &__on_scene_waveform_frame_start,
            .fnBeforeSwitchOut = &__before_scene_waveform_switching_out,
            .fnOnFrameCPL   = &__on_scene_waveform_frame_complete,
            .fnDepose       = &__on_scene_waveform_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_waveform_t begin ---------------*/

    do {
        size_t tBufferSize = ARM_2D_DEMO_WAVE_FORM_WINDOW_SIZE 
                           * sizeof(uint16_t) 
                           * 2;
        this.pchBuffer = 
            __arm_2d_allocate_scratch_memory(   tBufferSize,
                                                sizeof(uint16_t),
                                                ARM_2D_MEM_TYPE_UNSPECIFIED);

        if (NULL == this.pchBuffer) {
            assert(false);
            __on_scene_waveform_depose(&this.use_as__arm_2d_scene_t);
            return NULL;
        }
        
        arm_loader_io_window_init(  &this.Waveform.tWindowIO, 
                                    this.pchBuffer, 
                                    tBufferSize,
                                    ARM_2D_DEMO_WAVE_FORM_WINDOW_SIZE * sizeof(uint16_t));
        
                                            
        waveform_view_cfg_t tCFG = {
            .tSize = {
                .iWidth = ARM_2D_DEMO_WAVE_FORM_WINDOW_SIZE,
                .iHeight = 180,
            },

            .IO = {
                .ptIO = &ARM_LOADER_IO_WINDOW,
                .pTarget = (uintptr_t)&this.Waveform.tWindowIO,
            },

            .ChartScale = {
                .nUpperLimit = 1000,
                .nLowerLimit = -1000,
            },

            .u2SampleSize = WAVEFORM_SAMPLE_SIZE_HWORD,
            .u5DotHeight = 2,
            .bUnsigned = false,

            .tBrushColour.tColour = GLCD_COLOR_NIXIE_TUBE,
            .tBackgroundColour.tColour = this.use_as__arm_2d_scene_t.tCanvas.wColour,

            .chDirtyRegionItemCount = dimof(this.Waveform.tDirtyBins),
            .ptDirtyBins = this.Waveform.tDirtyBins,
            .bUseDirtyRegion = true,
            .ptScene = &this.use_as__arm_2d_scene_t,
        };

        waveform_view_init(&this.Waveform.tHelper, &tCFG);
    } while(0);

#if 0
    do {

        int16_t iData = -1000;
        for (int32_t n = 0; n < 50; n++) {
            iData += 40;
            arm_2d_scene_waveform_enqueue(  ptThis,
                                            &iData,
                                            1);
        }

    } while(0);
#endif

    /* ------------   initialize members of user_scene_waveform_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


