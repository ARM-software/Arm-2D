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

#define __USER_SCENE_BUBBLE_CHARGING_IMPLEMENT__
#include "arm_2d_scene_bubble_charging.h"

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
extern const arm_2d_tile_t c_tileQuaterArcMask;
extern const arm_2d_tile_t c_tileBigWhiteDotMask;
extern const arm_2d_tile_t c_tileWhiteDotMiddleMask;
extern const arm_2d_tile_t c_tileRadialGradientMask;
extern const arm_2d_tile_t c_tileGlassBallMask;
extern const arm_2d_tile_t c_tileHallowOutCircleMask;
extern const arm_2d_tile_t c_tileSinWaveMask;

/*============================ PROTOTYPES ====================================*/
extern
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_ALARM_CLOCK_32_A4;

/*============================ LOCAL VARIABLES ===============================*/

static arm_2d_size_t c_tChargingArea = {0};
static const arm_2d_tile_t *s_ptileQuaterArcMask = &c_tileQuaterArcMask;

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_bubble_charging_load(arm_2d_scene_t *ptScene)
{
    user_scene_bubble_charging_t *ptThis = (user_scene_bubble_charging_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_bubble_charging_depose(arm_2d_scene_t *ptScene)
{
    user_scene_bubble_charging_t *ptThis = (user_scene_bubble_charging_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    dynamic_nebula_depose(&this.tNebula);

    ARM_2D_OP_DEPOSE(this.tBlurOP);

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene bubble_charging                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_bubble_charging_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_bubble_charging_t *ptThis = (user_scene_bubble_charging_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_bubble_charging_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_bubble_charging_t *ptThis = (user_scene_bubble_charging_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_bubble_charging_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_bubble_charging_t *ptThis = (user_scene_bubble_charging_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    int32_t nResult;

    do {
        /* simulate a full battery charging/discharge cycle */
        arm_2d_helper_time_cos_slider(0, 1000, 60000, 0, &nResult, &this.lTimestamp[1]);

        this.iSoC = nResult;
    } while(0);

    if (arm_2d_helper_is_time_out(10, &this.lTimestamp[2])) {

        if (this.iWaveOffset++ >= c_tileSinWaveMask.tRegion.tSize.iWidth) {
            this.iWaveOffset = 0;
        }
    }

}

static void __on_scene_bubble_charging_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_bubble_charging_t *ptThis = (user_scene_bubble_charging_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /* switch to next scene after 30s */
    if (arm_2d_helper_is_time_out(30000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }

    arm_2d_scratch_memory_free(&this.tBlurOP.tScratchMemory);

}

static void __before_scene_bubble_charging_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_bubble_charging_t *ptThis = (user_scene_bubble_charging_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_bubble_charging_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_bubble_charging_t *ptThis = (user_scene_bubble_charging_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    
    arm_2d_canvas(ptTile, __charging_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/


        arm_2d_align_centre(__charging_canvas, c_tileHallowOutCircleMask.tRegion.tSize) {

            draw_liquid_wave(   ptTile, &
                                __centre_region, 
                                this.iSoC,
                                this.iWaveOffset,
                                (arm_2d_margin_t){0},
                                GLCD_COLOR_GREEN,
                                &c_tileSinWaveMask);

            arm_2d_fill_colour_with_mask(
                                    ptTile, 
                                    &__centre_region,
                                    &c_tileHallowOutCircleMask,
                                    (__arm_2d_color_t) {GLCD_COLOR_BLACK});

            ARM_2D_OP_WAIT_ASYNC();

        }

        /* show nebula */
        dynamic_nebula_show(&this.tNebula, 
                            ptTile, 
                            &__charging_canvas, 
                            GLCD_COLOR_WHITE, 
                            255,
                            bIsNewFrame);

        if (bIsNewFrame) {

            if (NULL == arm_2d_scratch_memory_new(  
                                        &this.tBlurOP.tScratchMemory,
                                        sizeof(__arm_2d_iir_blur_acc_t),
                                        (   __charging_canvas.tSize.iHeight 
                                        +   __charging_canvas.tSize.iWidth),
                                        __alignof__(__arm_2d_iir_blur_acc_t),
                                        ARM_2D_MEM_TYPE_FAST)) {
                assert(false);  /* insufficient memory */
            }

        }
        
        arm_2dp_filter_iir_blur(&this.tBlurOP,
                                ptTile,
                                &__charging_canvas,
                                255 - 16);

        arm_2d_align_centre(__charging_canvas, c_tileGlassBallMask.tRegion.tSize) {

            arm_2d_fill_colour_with_mask_and_opacity(
                                    ptTile, 
                                    &__centre_region,
                                    &c_tileGlassBallMask,
                                    (__arm_2d_color_t) {GLCD_COLOR_WHITE},
                                    255);

        }

        arm_2d_size_t tStringSize = arm_lcd_get_string_line_box("00", &ARM_2D_FONT_ALARM_CLOCK_32_A4);

        arm_2d_align_centre(__charging_canvas, tStringSize) {

            arm_lcd_text_set_draw_region(&__centre_region);
            arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_ALARM_CLOCK_32_A4);
            arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);

            arm_lcd_printf("%02d", (this.iSoC / 10));

            arm_lcd_text_set_font(NULL);
            
            __centre_region.tLocation.iX += tStringSize.iWidth + 6;
            __centre_region.tLocation.iY += tStringSize.iHeight - 8;
            arm_lcd_text_set_draw_region(&__centre_region);
            arm_lcd_text_set_colour(__RGB(0, 128, 0), GLCD_COLOR_BLACK);
            arm_lcd_printf("%%");
        }

    /*-----------------------draw the foreground end  -----------------------*/
    }
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static
void __draw_bubble_handler( void *pObj,
                            dynamic_nebula_t *ptDN,
                            const arm_2d_tile_t *ptTile,
                            arm_2d_location_t tLocation,
                            uint8_t chOpacity,
                            int16_t iDistance)
{
    user_scene_bubble_charging_t *ptThis = (user_scene_bubble_charging_t *)pObj;

    iDistance -= s_ptileQuaterArcMask->tRegion.tSize.iWidth;


    arm_2d_region_t tBubbleRegion = c_tileRadialGradientMask.tRegion;
    tBubbleRegion.tLocation.iX = tLocation.iX - c_tileRadialGradientMask.tRegion.tSize.iWidth / 2;
    tBubbleRegion.tLocation.iY = tLocation.iY - c_tileRadialGradientMask.tRegion.tSize.iHeight / 2;

    uint8_t chOriginalOpacity = 256 - chOpacity;

    if (iDistance > 16) {
        chOpacity = chOriginalOpacity;
    } else {
        chOpacity = MIN((iDistance << 4), chOriginalOpacity);
    }

    arm_2d_fill_colour_with_mask_and_opacity(ptTile, 
                                    &tBubbleRegion, 
                                    &c_tileRadialGradientMask, 
                                    (__arm_2d_color_t){GLCD_COLOR_GREEN},
                                    chOpacity);
}

ARM_NONNULL(1)
user_scene_bubble_charging_t *__arm_2d_scene_bubble_charging_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_bubble_charging_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    c_tChargingArea = s_ptileQuaterArcMask->tRegion.tSize;
    c_tChargingArea.iWidth *= 4;
    c_tChargingArea.iHeight *= 4;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);

    if (NULL == ptThis) {
        ptThis = (user_scene_bubble_charging_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_bubble_charging_t),
                                                        __alignof__(user_scene_bubble_charging_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_bubble_charging_t));

    *ptThis = (user_scene_bubble_charging_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_bubble_charging_load,
            .fnScene        = &__pfb_draw_scene_bubble_charging_handler,
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_bubble_charging_background_start,
            //.fnOnBGComplete = &__on_scene_bubble_charging_background_complete,
            .fnOnFrameStart = &__on_scene_bubble_charging_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_bubble_charging_switching_out,
            .fnOnFrameCPL   = &__on_scene_bubble_charging_frame_complete,
            .fnDepose       = &__on_scene_bubble_charging_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_bubble_charging_t begin ---------------*/
    do {
        int16_t iRadius = MIN(tScreen.tSize.iHeight, tScreen.tSize.iWidth) / 2;
        dynamic_nebula_cfg_t tCFG = {
            .fSpeed = 0.5f,
            .iRadius = iRadius,
            .iVisibleRingWidth = iRadius - s_ptileQuaterArcMask->tRegion.tSize.iWidth,
            .hwParticleCount = dimof(this.tParticles),
            .ptParticles = this.tParticles,

            .evtOnDrawParticles = {
                .fnHandler = &__draw_bubble_handler,
                .pTarget = ptThis,
            },
        };
        dynamic_nebula_init(&this.tNebula, &tCFG);
    } while(0);

    ARM_2D_OP_INIT(this.tBlurOP);

    /* ------------   initialize members of user_scene_bubble_charging_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


