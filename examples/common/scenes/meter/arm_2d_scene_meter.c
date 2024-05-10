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

#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include "arm_2d.h"

#define __USER_SCENE_METER_IMPLEMENT__
#include "arm_2d_scene_meter.h"

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

/*
 * note: If you want to change the background picture, you can simply update 
 *       the macro c_tileMeterPanel to your own tile of the background picture.
 * 
 *       If you want to change the pointer picture, please define a macro called
 *       c_tilePointerMask and let it map the tile of your pointer mask.
 *       Don't forget to update the pivot on the pointer mask in c_tPointerCenter
 *       initialization.
 */

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileMeterPanel         c_tileMeterPanelGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileMeterPanel         c_tileMeterPanelRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileMeterPanel         c_tileMeterPanelCCCA8888
#else
#   error Unsupported colour depth!
#endif

//#define c_tilePointerMask           <your pointer tile mask name>

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern 
const arm_2d_tile_t c_tileMeterPanel;

extern
const arm_2d_tile_t c_tilePointerMask;

static arm_2d_location_t s_tPointerCenter;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __on_scene_meter_load(arm_2d_scene_t *ptScene)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /* initialize transform helper */
    arm_2d_helper_dirty_region_transform_init(
                                &this.Pointer.tHelper,
                                &ptScene->tDirtyRegionHelper,
                                 (arm_2d_op_t *)&this.Pointer.tOP,
                                 0.01f,
                                 0.1f);

}

static void __on_scene_meter_depose(arm_2d_scene_t *ptScene)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    arm_2d_helper_dirty_region_transform_depose(&this.Pointer.tHelper);

    /* depose op */
    ARM_2D_OP_DEPOSE(this.Pointer.tOP);

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene meter                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_meter_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_meter_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_meter_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /*-----------------------    IMPORTANT MESSAGE    -----------------------*
     * It is better to update the 3 digits and pointer angle here            *
     *-----------------------------------------------------------------------*/
    
    int32_t iResult;

#if 1
    do {
        /* example: let the pointer swing back and forth between -120 degree and 100 degree */
        arm_2d_helper_time_cos_slider(-1200, 1000, 3000, 0, &iResult, &this.lTimestamp[1]);
    } while(0);
#else
    do {
        arm_2d_helper_pi_slider(&this.tPISlider, 1000, &iResult);
    } while(0);
#endif

    float fAngle = ARM_2D_ANGLE((float)iResult / 10.0f);
    /* 0 ~ 200 km / h */

    do {
        int16_t iNumber = (int16_t)(200 * (iResult + 1200) / 2400);
        bool bNumberUnchanged = (this.iNumber == iNumber);

        this.iNumber = iNumber;

        arm_2d_helper_dirty_region_item_suspend_update(
                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
                bNumberUnchanged);

    } while(0);

    /* update helper with new values*/
    arm_2d_helper_dirty_region_transform_update_value(&this.Pointer.tHelper, fAngle,1.0f);

    /* call helper's on-frame-start event handler */
    arm_2d_helper_dirty_region_transform_on_frame_start(&this.Pointer.tHelper);
}

static void __on_scene_meter_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /* switch to next scene after 10s */
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static void __before_scene_meter_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_meter_handler)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(tScreenSize);
    
    arm_2d_canvas(ptTile, __canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

        /* draw the cmsis logo using mask in the centre of the screen */
        arm_2d_align_centre(__canvas, c_tileMeterPanel.tRegion.tSize) {

            arm_2d_tile_copy_only(  &c_tileMeterPanel,
                                    ptTile,
                                    &__centre_region);

            arm_2d_op_wait_async(NULL);
            
            /* draw pointer */
            arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                &this.Pointer.tOP,
                                &c_tilePointerMask,
                                ptTile,
                                &__centre_region,
                                s_tPointerCenter,
                                this.Pointer.tHelper.fAngle,
                                this.Pointer.tHelper.fScale,
                                GLCD_COLOR_RED,
                                255);

            arm_2d_helper_dirty_region_transform_update(&this.Pointer.tHelper,
                                                         &__centre_region,
                                                         bIsNewFrame);

            arm_2d_op_wait_async((arm_2d_op_core_t *)&this.Pointer.tOP);
        }
        
        
        
        /* draw 3 digits numbers */
        do {
            arm_2d_size_t tTextSize = ARM_2D_FONT_A4_DIGITS_ONLY
                                            .use_as__arm_2d_user_font_t
                                                .use_as__arm_2d_font_t
                                                    .tCharSize;
            tTextSize.iWidth *=3;     /* 3 digits */
            tTextSize.iHeight += 16;  /* for "km/h */


            arm_2d_align_centre(__canvas,  tTextSize) {
                
                arm_2d_layout(__centre_region, true) {
                
                    arm_lcd_text_set_target_framebuffer(ptTile);
                    /* print speed */
                    __item_line_vertical(tTextSize.iWidth, tTextSize.iHeight - 16) {
                        arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
                        arm_lcd_text_set_draw_region(&__item_region);
                        arm_lcd_text_set_colour( GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
                        arm_lcd_text_set_opacity(255 - 64);
                        arm_lcd_printf("%03d", (int)this.iNumber);
                        arm_lcd_text_set_opacity(255);

                        arm_2d_helper_dirty_region_update_item(
                                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
                                ptTile,
                                NULL,
                                &__item_region);
                    }
                    
                    /* print "km/h" */
                    __item_line_vertical(tTextSize.iWidth,16) {
                        arm_2d_align_centre(__item_region, 4*6, 8) {
                            arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_6x8);
                            arm_lcd_text_set_draw_region(&__centre_region);
                            arm_lcd_text_set_colour( GLCD_COLOR_DARK_GREY, GLCD_COLOR_BLACK);
                            arm_lcd_printf("km/h");
                        }
                    }

                    arm_lcd_text_set_target_framebuffer(NULL);
                }
            }
            
        } while(0);

        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene meter");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_meter_t *__arm_2d_scene_meter_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_meter_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);
    
    if (NULL == ptThis) {
        ptThis = (user_scene_meter_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_meter_t),
                                                        __alignof__(user_scene_meter_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
       
    }
     memset(ptThis, 0, sizeof(user_scene_meter_t));
    
    *ptThis = (user_scene_meter_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 
        
            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_meter_load,
            .fnScene        = &__pfb_draw_scene_meter_handler,

            //.fnOnBGStart    = &__on_scene_meter_background_start,
            //.fnOnBGComplete = &__on_scene_meter_background_complete,
            .fnOnFrameStart = &__on_scene_meter_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_meter_switching_out,
            .fnOnFrameCPL   = &__on_scene_meter_frame_complete,
            .fnDepose       = &__on_scene_meter_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };
    
    /* initialize op */
    ARM_2D_OP_INIT(this.Pointer.tOP);


    s_tPointerCenter.iX = c_tilePointerMask.tRegion.tSize.iWidth >> 1;
    s_tPointerCenter.iY = 100; /* radius */

    /* initialize PI slider */
    do {
        static const arm_2d_helper_pi_slider_cfg_t tCFG = {
            .fProportion = 0.15f,
            .fIntegration = 0.6667f,
            .nInterval = 20, 
        };

        arm_2d_helper_pi_slider_init(&this.tPISlider, (arm_2d_helper_pi_slider_cfg_t *)&tCFG, 0);
    } while(0);

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

