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

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#define __USER_SCENE_GAS_GAUGE_IMPLEMENT__
#include "arm_2d_scenes.h"

#include "arm_2d_helper.h"
#include "arm_2d_example_controls.h"

#include "arm_2d_scene_gas_gauge.h"

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


static void __on_scene_gas_gauge_depose(arm_2d_scene_t *ptScene)
{
    user_scene_gas_gauge_t *ptThis = (user_scene_gas_gauge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;

    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene 4                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_gas_gauge_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_gas_gauge_t *ptThis = (user_scene_gas_gauge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_gas_gauge_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_gas_gauge_t *ptThis = (user_scene_gas_gauge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_gas_gauge_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_gas_gauge_t *ptThis = (user_scene_gas_gauge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    int32_t nResult;
    
    /* simulate a full battery charging/discharge cycle */
    arm_2d_helper_time_cos_slider(0, 1000, 30000, 0, &nResult, &this.lTimestamp[1]);
    
    if (this.hwGasgauge < nResult) {
        this.tStatus = BATTERY_STATUS_CHARGING;
    } else if (this.hwGasgauge > nResult) {
        this.tStatus = BATTERY_STATUS_DISCHARGING;
    }
    this.hwGasgauge = (uint16_t)nResult;

}

static void __before_scene_gas_gauge_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_gas_gauge_t *ptThis = (user_scene_gas_gauge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
}

static void __on_scene_gas_gauge_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_gas_gauge_t *ptThis = (user_scene_gas_gauge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(29000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_gas_gauge_background_handler)
{
    user_scene_gas_gauge_t *ptThis = (user_scene_gas_gauge_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    /*-----------------------draw back ground begin-----------------------*/



    /*-----------------------draw back ground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_gas_gauge_handler)
{
    user_scene_gas_gauge_t *ptThis = (user_scene_gas_gauge_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);

    
    /*-----------------------draw the foreground begin-----------------------*/
    
    /* following code is just a demo, you can remove them */

    arm_2d_canvas(ptTile, __canvas) {

        arm_2d_align_centre( __canvas, 128, 130) {

            arm_2d_layout(__centre_region, true) {

                __item_line_horizontal(64, 130) {
                    battery_gasgauge_nixie_tube_show(   &this.tBatteryNixieTube, 
                                                        ptTile, 
                                                        &__item_region, 
                                                        this.hwGasgauge,
                                                        this.tStatus,
                                                        bIsNewFrame);
                    
                    arm_2d_op_wait_async(NULL);

                    
                    arm_2d_size_t tTextSize = arm_lcd_get_string_line_box("00", &ARM_2D_FONT_A4_DIGITS_ONLY);
                    
                    arm_2d_align_bottom_centre(__item_region, tTextSize) {

                        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                        arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
                        arm_lcd_text_set_draw_region(&__bottom_centre_region);
                        arm_lcd_text_set_colour(GLCD_COLOR_NIXIE_TUBE, GLCD_COLOR_BLACK);
                        arm_lcd_text_location(0,0);
                        arm_lcd_printf("%02d", this.hwGasgauge / 10);
                        
                        arm_2d_op_wait_async(NULL);
                    }
                }
                
                __item_line_horizontal(64, 130) {
                    battery_gasgauge_liquid_show(   &this.tBatteryLiquid, 
                                                    ptTile, 
                                                    &__item_region, 
                                                    this.hwGasgauge,
                                                    this.tStatus,
                                                    bIsNewFrame);
                    
                    arm_2d_op_wait_async(NULL);
                    
                    arm_2d_size_t tTextSize = arm_lcd_get_string_line_box("00", &ARM_2D_FONT_A4_DIGITS_ONLY);
                    
                    arm_2d_align_bottom_centre(__item_region, tTextSize) {

                        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                        arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
                        arm_lcd_text_set_draw_region(&__bottom_centre_region);
                        arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
                        arm_lcd_text_location(0,0);
                        arm_lcd_text_set_opacity(128);
                        arm_lcd_printf("%02d", this.hwGasgauge / 10);
                        arm_lcd_text_set_opacity(255);

                        arm_2d_op_wait_async(NULL);
                    }
                }
            }
        }
    }

    /* draw text at the top-left corner */

    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
    arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
    arm_lcd_text_set_draw_region(NULL);
    arm_lcd_text_set_colour(GLCD_COLOR_NIXIE_TUBE, GLCD_COLOR_WHITE);
    arm_lcd_text_location(0,0);
    arm_lcd_puts("Scene Battery Gas Gauge");

    /*-----------------------draw the foreground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_gas_gauge_t *__arm_2d_scene_gas_gauge_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_gas_gauge_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

        /* a dirty region to be specified at runtime*/
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            .tSize = {
                128, 130,
            },
        ),
        
    END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)
    
    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    /* initialise dirty region 0 at runtime
     */
    arm_2d_align_centre(tScreen, s_tDirtyRegions[0].tRegion.tSize) {
        s_tDirtyRegions[0].tRegion = __centre_region;
    }
    
    if (NULL == ptThis) {
        ptThis = (user_scene_gas_gauge_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_gas_gauge_t),
                                                        __alignof__(user_scene_gas_gauge_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
        memset(ptThis, 0, sizeof(user_scene_gas_gauge_t));
    }

    *ptThis = (user_scene_gas_gauge_t){
        .use_as__arm_2d_scene_t = {

        /* the canvas colour */
        .tCanvas = {GLCD_COLOR_BLACK}, 

        /* Please uncommon the callbacks if you need them
         */
        //.fnBackground   = &__pfb_draw_scene_gas_gauge_background_handler,
        .fnScene        = &__pfb_draw_scene_gas_gauge_handler,
        .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
        

        //.fnOnBGStart    = &__on_scene_gas_gauge_background_start,
        //.fnOnBGComplete = &__on_scene_gas_gauge_background_complete,
        .fnOnFrameStart = &__on_scene_gas_gauge_frame_start,
        .fnBeforeSwitchOut = &__before_scene_gas_gauge_switching_out,
        .fnOnFrameCPL   = &__on_scene_gas_gauge_frame_complete,
        .fnDepose       = &__on_scene_gas_gauge_depose,
        },
        .bUserAllocated = bUserAllocated,
    };

    battery_gasgauge_nixie_tube_init(&this.tBatteryNixieTube);
    battery_gasgauge_liquid_init(&this.tBatteryLiquid);

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}




#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

