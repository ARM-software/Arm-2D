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

enum {
    DIRTY_REGION_SPEED,
};

/*============================ GLOBAL VARIABLES ==============================*/

extern 
const arm_2d_tile_t c_tileMeterPanel;

extern
const arm_2d_tile_t c_tilePointerMask;

static arm_2d_location_t s_tPointerCenter;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

    /* add the last region for Speed String */
    ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
        0
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_meter_load(arm_2d_scene_t *ptScene)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    meter_pointer_on_load(&this.tMeterPointer);

#if ARM_2D_SCENE_METER_USE_JPG
    arm_tjpgd_loader_on_load(&this.tJPGBackground);
#endif
}

static void __after_scene_meter_switching(arm_2d_scene_t *ptScene)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_meter_depose(arm_2d_scene_t *ptScene)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if ARM_2D_SCENE_METER_USE_JPG
    arm_tjpgd_loader_depose(&this.tJPGBackground);
#endif

    ptScene->ptPlayer = NULL;
    
    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    meter_pointer_depose(&this.tMeterPointer);

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

#if ARM_2D_SCENE_METER_USE_JPG
    arm_tjpgd_loader_on_frame_start(&this.tJPGBackground);
#endif

    /*-----------------------    IMPORTANT MESSAGE    -----------------------*
     * It is better to update the 3 digits and pointer angle here            *
     *-----------------------------------------------------------------------*/

    do {
        /* generate a new position every 2000 sec */
        if (arm_2d_helper_is_time_out(3000,  &this.lTimestamp[1])) {
            this.lTimestamp[1] = 0;
            srand(arm_2d_helper_get_system_timestamp());
            this.iTargetNumber = rand() % 200;
        }

        meter_pointer_on_frame_start(&this.tMeterPointer, this.iTargetNumber, 1.0f);
    } while(0);

    do {
        int16_t iNumber = meter_pointer_get_current_value(&this.tMeterPointer);
        bool bNumberUnchanged = (this.iNumber == iNumber);

        this.iNumber = iNumber;

        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_SPEED],
                                            bNumberUnchanged); 

    } while(0);

    
}

static void __on_scene_meter_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_meter_t *ptThis = (user_scene_meter_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    meter_pointer_on_frame_complete(&this.tMeterPointer);

#if ARM_2D_SCENE_METER_USE_JPG
    arm_tjpgd_loader_on_frame_complete(&this.tJPGBackground);
#endif

#if 0
    /* switch to next scene after 15s */
    if (arm_2d_helper_is_time_out(15000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
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

    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    arm_2d_canvas(ptTile, __canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

    #if !ARM_2D_SCENE_METER_USE_JPG 
        arm_2d_align_centre(__canvas, c_tileMeterPanel.tRegion.tSize) {

            arm_2d_tile_copy_only(  &c_tileMeterPanel,
                                    ptTile,
                                    &__centre_region);
        
            
        }
    #else
        arm_2d_align_centre(__canvas, this.tJPGBackground.vres.tTile.tRegion.tSize) {

            arm_2d_tile_copy_only(  &this.tJPGBackground.vres.tTile,
                                    ptTile,
                                    &__centre_region);

        }
    #endif

        meter_pointer_show(&this.tMeterPointer,
                            ptTile,
                            &__canvas,
                            NULL,
                            255);
        
        /* draw 3 digits numbers */
        do {
            /* 3 digits */
            arm_2d_size_t tTextSize = arm_lcd_get_string_line_box("000", &ARM_2D_FONT_A4_DIGITS_ONLY);
            tTextSize.iHeight += 16;    /* for "km/h */

            arm_2d_align_centre(__canvas,  tTextSize) {
                
                arm_2d_layout(__centre_region, DEFAULT, true) {
                
                    arm_lcd_text_set_target_framebuffer(ptTile);
                    /* print speed */
                    __item_line_vertical(tTextSize.iWidth, tTextSize.iHeight - 16) {
                        arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
                        arm_lcd_text_set_draw_region(&__item_region);
                        arm_lcd_text_set_colour( GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
                        arm_lcd_text_set_opacity(255 - 64);
                        arm_lcd_printf("%03d", (int)this.iNumber);
                        arm_lcd_text_set_opacity(255);
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
    
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_meter_t *__arm_2d_scene_meter_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_meter_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);

    arm_2d_size_t tTextSize = arm_lcd_get_string_line_box("000", &ARM_2D_FONT_A4_DIGITS_ONLY);
    tTextSize.iHeight += 16;

    arm_2d_align_centre(tScreen,  tTextSize) {
                
        arm_2d_layout(__centre_region) {
            /* print speed */
            __item_line_vertical(tTextSize.iWidth, tTextSize.iHeight - 16) {
                s_tDirtyRegions[DIRTY_REGION_SPEED].tRegion = __item_region;
            }
        
        #if 0
            /* print "km/h" */
            __item_line_vertical(tTextSize.iWidth,16) {
                arm_2d_align_centre(__item_region, 4*6, 8) {

                }
            }
        #endif
        }
    }

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
            //.fnAfterSwitch  = &__after_scene_meter_switching,

            .fnScene        = &__pfb_draw_scene_meter_handler,
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,

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

    do {
        meter_pointer_cfg_t tCFG = {
            .tSpinZoom = {
                .Indicator = {
                    .LowerLimit = {
                        .fAngleInDegree = -120.0f,
                        .nValue = 0,
                    },
                    .UpperLimit = {
                        .fAngleInDegree = 100.0f,
                        .nValue = 200,
                    },
                },
                .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
                .Source = {
                    .ptMask = &c_tilePointerMask,
                    .tColourToFill = GLCD_COLOR_RED,
                },
                .ptScene = (arm_2d_scene_t *)ptThis,
            },

            .Pointer = {
                .bIsSourceHorizontal = false,
                .iRadius = 100,
            },

            .tPISliderCFG = {
                .fProportion = 0.0300f,
                .fIntegration = 0.0020f,
                .nInterval = 10,
            }

        };
        meter_pointer_init(&this.tMeterPointer, &tCFG);

        this.iTargetNumber = 150;
    } while(0);

    /* initialize TJpgDec loader */
#if ARM_2D_SCENE_METER_USE_JPG
    do {
    #if ARM_2D_DEMO_TJPGD_USE_FILE
        arm_tjpgd_io_file_loader_init(&this.LoaderIO.tFile, "../common/asset/Helium.jpg");
    #else
        extern
        const uint8_t c_chMeterPanel80jpg[10089];

        arm_tjpgd_io_binary_loader_init(&this.LoaderIO.tBinary, c_chMeterPanel80jpg, sizeof(c_chMeterPanel80jpg));
    #endif
        arm_tjpgd_loader_cfg_t tCFG = {
            .bUseHeapForVRES = true,
            .ptScene = (arm_2d_scene_t *)ptThis,
            .u2WorkMode = ARM_TJPGD_MODE_PARTIAL_DECODED_TINY,
        #if ARM_2D_DEMO_TJPGD_USE_FILE
            .ImageIO = {
                .ptIO = &ARM_TJPGD_IO_FILE_LOADER,
                .pTarget = (uintptr_t)&this.LoaderIO.tFile,
            },
        #else
            .ImageIO = {
                .ptIO = &ARM_TJPGD_IO_BINARY_LOADER,
                .pTarget = (uintptr_t)&this.LoaderIO.tBinary,
            },
        #endif
        };

        arm_tjpgd_loader_init(&this.tJPGBackground, &tCFG);
    } while(0);
#endif

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

