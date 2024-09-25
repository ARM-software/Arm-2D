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

#define __USER_SCENE_KNOB_IMPLEMENT__
#include "arm_2d_scene_knob.h"

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

enum {
    DIRTY_REGION_SETTING,
};


/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern const arm_2d_tile_t c_tileRadialLineCoverA4Mask;
extern const arm_2d_tile_t c_tileKnobCircleMask;
extern const arm_2d_tile_t c_tileWedgeMask;

ARM_NOINIT
static arm_2d_tile_t s_tileRadialCircleQuaterMask;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

    /* add the last region:
        * it is the top left corner for text display 
        */
    ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
        0
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_knob_load(arm_2d_scene_t *ptScene)
{
    user_scene_knob_t *ptThis = (user_scene_knob_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    progress_wheel_on_load(&this.tWheel);
    meter_pointer_on_load(&this.tPointer);

}

static void __after_scene_knob_switching(arm_2d_scene_t *ptScene)
{
    user_scene_knob_t *ptThis = (user_scene_knob_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_knob_depose(arm_2d_scene_t *ptScene)
{
    user_scene_knob_t *ptThis = (user_scene_knob_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    progress_wheel_depose(&this.tWheel);
    meter_pointer_depose(&this.tPointer);

    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene knob                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_knob_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_knob_t *ptThis = (user_scene_knob_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_knob_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_knob_t *ptThis = (user_scene_knob_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_knob_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_knob_t *ptThis = (user_scene_knob_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    do {
        /* generate a new position every 2000 sec */
        if (arm_2d_helper_is_time_out(2000,  &this.lTimestamp[1])) {
            this.lTimestamp[1] = 0;
            srand(arm_2d_helper_get_system_timestamp());
            this.iTargetSetting = rand() % 1000;
        }

        meter_pointer_on_frame_start(&this.tPointer, this.iTargetSetting, 1.0f);
    } while(0);

    do {
        int16_t iDisplayTargetSetting = meter_pointer_get_current_value(&this.tPointer);

        iDisplayTargetSetting = MAX(0, iDisplayTargetSetting);
        iDisplayTargetSetting = MIN(iDisplayTargetSetting, 999);

        bool bIsSettingUnchanged = (this.iDisplayTargetSetting == iDisplayTargetSetting);

        this.iDisplayTargetSetting = iDisplayTargetSetting;

        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_SETTING],
                                            bIsSettingUnchanged); 

    } while(0);
    
    do {
        int16_t iTemp = spin_zoom_widget_get_current_angle(&this.tPointer.use_as__spin_zoom_widget_t) * 10.0f + 1800;
        int32_t nResult;
        arm_2d_helper_pi_slider(&this.tPISlider, 
                                iTemp, 
                                &nResult);
        this.iActual = nResult;
    } while(0);

    progress_wheel_on_frame_start(&this.tWheel);

}

static void __on_scene_knob_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_knob_t *ptThis = (user_scene_knob_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    meter_pointer_on_frame_complete(&this.tPointer);

#if 1
    /* switch to next scene after 10s */
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene_knob_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_knob_t *ptThis = (user_scene_knob_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_knob_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_knob_t *ptThis = (user_scene_knob_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/

        /* draw the radial line cover */
        arm_2d_align_centre(__top_canvas, c_tileRadialLineCoverA4Mask.tRegion.tSize) {

            arm_2d_fill_colour_with_opacity(ptTile, 
                                            &__centre_region,
                                             (__arm_2d_color_t){GLCD_COLOR_WHITE}, 
                                            128);


            progress_wheel_show(&this.tWheel,
                            ptTile, 
                            &__centre_region,       
                            this.iActual,    /* progress 0~1000 */
                            255,                    /* opacity */
                            bIsNewFrame);

            arm_2d_fill_colour_with_a4_mask(ptTile, 
                                            &__centre_region, 
                                            &c_tileRadialLineCoverA4Mask, 
                                            (__arm_2d_color_t){GLCD_COLOR_BLACK});
            
        }

        meter_pointer_show(&this.tPointer, 
                        ptTile,
                        &__top_canvas,
                        NULL,
                        255);
        
                /* draw 3 digits numbers */
        do {
            /* 3 digits */
            arm_2d_size_t tTextSize = arm_lcd_get_string_line_box("000", &ARM_2D_FONT_A4_DIGITS_ONLY);
            tTextSize.iHeight += 16;    /* for "km/h */

            arm_2d_align_centre(__top_canvas,  tTextSize) {
                
                arm_2d_layout(__centre_region, true) {
                
                    arm_lcd_text_set_target_framebuffer(ptTile);
                    /* print speed */
                    __item_line_vertical(tTextSize.iWidth, tTextSize.iHeight - 16) {
                        arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
                        arm_lcd_text_set_draw_region(&__item_region);
                        arm_lcd_text_set_colour( GLCD_COLOR_NIXIE_TUBE, GLCD_COLOR_BLACK);
                        arm_lcd_text_set_opacity(255 - 64);
                        arm_lcd_printf("%03d", (int)this.iDisplayTargetSetting);
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
        arm_lcd_puts("Scene knob");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_knob_t *__arm_2d_scene_knob_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_knob_t *ptThis)
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
                s_tDirtyRegions[DIRTY_REGION_SETTING].tRegion = __item_region;
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
        ptThis = (user_scene_knob_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_knob_t),
                                                        __alignof__(user_scene_knob_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_knob_t));

    *ptThis = (user_scene_knob_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_knob_load,
            .fnScene        = &__pfb_draw_scene_knob_handler,
            //.fnAfterSwitch  = &__after_scene_knob_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene_knob_background_start,
            //.fnOnBGComplete = &__on_scene_knob_background_complete,
            .fnOnFrameStart = &__on_scene_knob_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_knob_switching_out,
            .fnOnFrameCPL   = &__on_scene_knob_frame_complete,
            .fnDepose       = &__on_scene_knob_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_knob_t begin ---------------*/

    do {
        s_tileRadialCircleQuaterMask = (arm_2d_tile_t)
            impl_child_tile(c_tileKnobCircleMask,
                            0, 
                            0,
                            c_tileKnobCircleMask.tRegion.tSize.iWidth >> 1,
                            c_tileKnobCircleMask.tRegion.tSize.iHeight >> 1,
                            );

        progress_wheel_cfg_t tCFG = {
            .ptileArcMask   = &s_tileRadialCircleQuaterMask, /* mask for arc */
            .tWheelColour   = GLCD_COLOR_NIXIE_TUBE,         /* arc colour */
            .iWheelDiameter = 0,                            /* diameter, 0 means use the mask's original size */

            .bIgnoreDot = true,
            .iRingWidth = 25,
            .u2StartPosition = PROGRESS_WHEEL_START_POSITION_BOTTOM,
            .u15FullLength = 3600,

            .bUseDirtyRegions = true,                       /* use dirty regions */
        };

        progress_wheel_init(&this.tWheel, 
                            &this.use_as__arm_2d_scene_t,
                            &tCFG);
    } while(0);

    do {
        meter_pointer_cfg_t tCFG = {
            .tSpinZoom = {
                .Indicator = {
                    .LowerLimit = {
                        .fAngleInDegree = -135.0f,
                        .nValue = 0,
                    },
                    .UpperLimit = {
                        .fAngleInDegree = 135.0f,
                        .nValue = 1000,
                    },
                    .Step = {
                        .fAngle = 1.0f,
                    },
                },
                .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
                .Source = {
                    .ptMask = &c_tileWedgeMask,
                    .tColourToFill = GLCD_COLOR_WHITE,
                },
                .ptScene = (arm_2d_scene_t *)ptThis,
            },

            .Pointer = {
                .bIsSourceHorizontal = false,
                .iRadius = 115,
            },

            .tPISliderCFG = {
                .fProportion = 0.1000f,
                .fIntegration = 0.00000f,
                .nInterval = 10,
            }
        };
        meter_pointer_init(&this.tPointer, &tCFG);
    } while(0);

    do {
        arm_2d_helper_pi_slider_cfg_t tCFG = {
            .fProportion = 0.0300f,
            .fIntegration = 0.0020f,
            .nInterval = 10,
        };
        arm_2d_helper_pi_slider_init(&this.tPISlider, 
                                     &tCFG, 
                                     0);
    } while(0);

    /* ------------   initialize members of user_scene_knob_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


