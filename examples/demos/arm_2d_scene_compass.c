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

#define __USER_SCENE_COMPASS_IMPLEMENT__
#include "arm_2d_scene_compass.h"

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
#   define c_tileCompass            c_tileCompassGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565
#   define c_tileCompass            c_tileCompassRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#   define c_tileCompass            c_tileCompassCCCA8888
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

extern const arm_2d_tile_t c_tileCompass;
extern const arm_2d_tile_t c_tileCompassMask;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
}   ARM_2D_FONT_ALARM_CLOCK_32_A4,
    ARM_2D_FONT_ALARM_CLOCK_32_A2;


enum {
    COMPASS_DIRTY_REGION_TOP,
    COMPASS_DIRTY_REGION_BOTTOM,
    COMPASS_DIRTY_REGION_LEFT,
    COMPASS_DIRTY_REGION_RIGHT,

    COMPASS_DIRTY_REGION_CORNER_TOP_LEFT,
    COMPASS_DIRTY_REGION_CORNER_TOP_RIGHT,
    COMPASS_DIRTY_REGION_CORNER_BOTTOM_LEFT,
    COMPASS_DIRTY_REGION_CORNER_BOTTOM_RIGHT,

    COMPASS_DIRTY_REGION_TEXT,
};

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

    /* a dirty region to be specified at runtime*/
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),

    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),

    /* a dirty region to be specified at runtime*/
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),

    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),

    /* a dirty region to be specified at runtime*/
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),

    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),

    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),
    
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),

    /* add the last region:
        * it is the top left corner for text display 
        */
    ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
        0
    ),



END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_compass_load(arm_2d_scene_t *ptScene)
{
    user_scene_compass_t *ptThis = (user_scene_compass_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    meter_pointer_on_load(&this.tCompass);

}

static void __after_scene_compass_switching(arm_2d_scene_t *ptScene)
{
    user_scene_compass_t *ptThis = (user_scene_compass_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_compass_depose(arm_2d_scene_t *ptScene)
{
    user_scene_compass_t *ptThis = (user_scene_compass_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    meter_pointer_depose(&this.tCompass);

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene compass                                                              *
 *----------------------------------------------------------------------------*/

static void __on_scene_compass_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_compass_t *ptThis = (user_scene_compass_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_compass_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_compass_t *ptThis = (user_scene_compass_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_compass_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_compass_t *ptThis = (user_scene_compass_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    do {
        /* generate a new position every 2000 sec */
        if (arm_2d_helper_is_time_out(3000,  &this.lTimestamp[1])) {
            this.lTimestamp[1] = 0;


            srand(arm_2d_helper_get_system_timestamp());

            this.iTargetAngle = rand() % 3600;
        } 
    } while(0);

    int16_t iCurrentAngle = meter_pointer_get_current_value(&this.tCompass);

    if (iCurrentAngle < 0) {
        iCurrentAngle += 3600;
    } else if (iCurrentAngle >= 3600) {
        iCurrentAngle -= 3600;
    }

    arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[COMPASS_DIRTY_REGION_TEXT],
                                        (iCurrentAngle == this.iDisplayAngle));
    this.iDisplayAngle = iCurrentAngle;

    meter_pointer_on_frame_start(&this.tCompass, this.iTargetAngle, 1.0f);

}

static void __on_scene_compass_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_compass_t *ptThis = (user_scene_compass_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    meter_pointer_on_frame_complete(&this.tCompass);

    /* switch to next scene after 10s */
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static void __before_scene_compass_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_compass_t *ptThis = (user_scene_compass_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_compass_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_compass_t *ptThis = (user_scene_compass_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */


        /* draw the cmsis logo in the centre of the screen */

        arm_2d_align_centre(__top_canvas, c_tileCompass.tRegion.tSize) {
            meter_pointer_show(&this.tCompass, ptTile, &__centre_region, NULL, 255);

        }

        /* draw 3 digits numbers */
        do {
            /* 3 digits */
            arm_2d_size_t tTextSize = arm_lcd_get_string_line_box("000", &ARM_2D_FONT_ALARM_CLOCK_32_A4);

            arm_2d_align_centre(__top_canvas,  tTextSize) {
                    arm_lcd_text_set_target_framebuffer(ptTile);
                    /* print speed */
                    arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_ALARM_CLOCK_32_A4);
                    arm_lcd_text_set_draw_region(&__centre_region);
                    arm_lcd_text_set_colour( GLCD_COLOR_RED, GLCD_COLOR_BLACK);
                    arm_lcd_text_set_opacity(255);
                    arm_lcd_printf("%03d", (int)this.iDisplayAngle / 10);
                    arm_lcd_text_set_opacity(255);
            }
            
        } while(0);


        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene compass");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_compass_t *__arm_2d_scene_compass_init(  arm_2d_scene_player_t *ptDispAdapter, 
                                                    user_scene_compass_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    
    arm_2d_align_centre(tScreen, c_tileCompass.tRegion.tSize) {
        arm_2d_dock_top(__centre_region, 50) {
            s_tDirtyRegions[COMPASS_DIRTY_REGION_TOP].tRegion = __top_region;
        }

        arm_2d_dock_bottom(__centre_region, 50) {
            s_tDirtyRegions[COMPASS_DIRTY_REGION_BOTTOM].tRegion = __bottom_region;
        }

        arm_2d_dock_left(__centre_region, 50) {
            s_tDirtyRegions[COMPASS_DIRTY_REGION_LEFT].tRegion = __left_region;
        }
    
        arm_2d_dock_right(__centre_region, 50) {
            s_tDirtyRegions[COMPASS_DIRTY_REGION_RIGHT].tRegion = __right_region;
        }

        arm_2d_align_centre(tScreen, 
                            c_tileCompass.tRegion.tSize.iWidth - 100, 
                            c_tileCompass.tRegion.tSize.iHeight - 100) {

            arm_2d_align_top_left(__centre_region, 30, 30) {
                s_tDirtyRegions[COMPASS_DIRTY_REGION_CORNER_TOP_LEFT].tRegion = __top_left_region;
            }

            arm_2d_align_top_right(__centre_region, 30, 30) {
                s_tDirtyRegions[COMPASS_DIRTY_REGION_CORNER_TOP_RIGHT].tRegion = __top_right_region;
            }

            arm_2d_align_bottom_left(__centre_region, 30, 30) {
                s_tDirtyRegions[COMPASS_DIRTY_REGION_CORNER_BOTTOM_LEFT].tRegion = __bottom_left_region;
            }

            arm_2d_align_bottom_right(__centre_region, 30, 30) {
                s_tDirtyRegions[COMPASS_DIRTY_REGION_CORNER_BOTTOM_RIGHT].tRegion = __bottom_right_region;
            }

        }

        arm_2d_size_t tTextSize = arm_lcd_get_string_line_box("000", &ARM_2D_FONT_ALARM_CLOCK_32_A4);

        arm_2d_align_centre(tScreen,  tTextSize) {
            s_tDirtyRegions[COMPASS_DIRTY_REGION_TEXT].tRegion = __centre_region;
        }
    
    }

    if (NULL == ptThis) {
        ptThis = (user_scene_compass_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_compass_t),
                                                        __alignof__(user_scene_compass_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_compass_t));

    *ptThis = (user_scene_compass_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_WHITE}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_compass_load,
            .fnScene        = &__pfb_draw_scene_compass_handler,
            //.fnAfterSwitch  = &__after_scene_compass_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_compass_background_start,
            //.fnOnBGComplete = &__on_scene_compass_background_complete,
            .fnOnFrameStart = &__on_scene_compass_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_compass_switching_out,
            .fnOnFrameCPL   = &__on_scene_compass_frame_complete,
            .fnDepose       = &__on_scene_compass_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_compass_t begin ---------------*/

    do {
        meter_pointer_cfg_t tCFG = {
            .tSpinZoom = {
                .Indicator = {
                    .LowerLimit = {
                        .fAngleInDegree = 0.0f,
                        .nValue = 0,
                    },
                    .UpperLimit = {
                        .fAngleInDegree = 359.9f,
                        .nValue = 3599,
                    },
                },
                .ptScene = (arm_2d_scene_t *)ptThis,
                .ptTransformMode = &SPIN_ZOOM_MODE_TILE_WITH_COLOUR_KEYING,
                .Source = {
                    .ptMask = &c_tileCompassMask,
                    .ptSource = &c_tileCompass,
                    .tCentre = {
                        .iX = c_tileCompass.tRegion.tSize.iWidth >> 1,
                        .iY = c_tileCompass.tRegion.tSize.iHeight >> 1,
                    },
                    .tColourForKeying = 0xef9e,
                },
            },
            .tPISliderCFG = {
                .fProportion = 0.0300f,
                .fIntegration = 0.0020f,
                .nInterval = 10,
            },
        };

        meter_pointer_init(&this.tCompass, &tCFG);
    } while(0);


    /* ------------   initialize members of user_scene_compass_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


