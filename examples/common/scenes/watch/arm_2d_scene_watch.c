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

#define __USER_SCENE_WATCH_IMPLEMENT__
#include "arm_2d_scene_watch.h"

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
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"  
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

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileWatchPanel             c_tileWatchPanelGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileWatchPanel             c_tileWatchPanelRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileWatchPanel             c_tileWatchPanelCCCA8888
#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileWatchPanel;

extern const arm_2d_tile_t c_tilePointerSecMask;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
}   ARM_2D_FONT_ALARM_CLOCK_32_A8, 
    ARM_2D_FONT_ALARM_CLOCK_32_A4,
    ARM_2D_FONT_ALARM_CLOCK_32_A2,
    ARM_2D_FONT_ALARM_CLOCK_32_A1;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static
const arm_2d_tile_t c_tilePointerHourMask = 
    impl_child_tile(c_tilePointerSecMask, 0, 0, 9, 67);

static
const arm_2d_tile_t c_tilePointerMinMask = 
    impl_child_tile(c_tilePointerSecMask, 0, 0, 9, 80);

static arm_2d_location_t s_tPointerSecCenter;
static arm_2d_location_t s_tPointerMinCenter;
static arm_2d_location_t s_tPointerHourCenter;

ARM_NOINIT
static struct {
    arm_2d_location_t tLocation;
    uint8_t chNumber;
} s_tDigitsTable[12];

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_watch_load(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_foreach(spin_zoom_widget_t, this.tPointers, ptPointer) {
        spin_zoom_widget_on_load(ptPointer);
    }

#if __SCENE_WATCH_CFG_UPDATE_SECOND_POINTER_ONCE_PER_SECOND__
    meter_pointer_on_load(&this.tSecPointer);
#endif

}

static void __on_scene_watch_depose(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    arm_foreach(spin_zoom_widget_t, this.tPointers, ptPointer) {
        spin_zoom_widget_depose(ptPointer);
    }

#if __SCENE_WATCH_CFG_UPDATE_SECOND_POINTER_ONCE_PER_SECOND__
    meter_pointer_depose(&this.tSecPointer);
#endif

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene watch                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_watch_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_watch_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_watch_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    int64_t lTimeStampInMs = arm_2d_helper_convert_ticks_to_ms(
                                arm_2d_helper_get_system_timestamp());

    /* calculate the hours */
    do {
        uint32_t chHour = lTimeStampInMs / 1000ul;
        chHour %= 12 * 3600ul;
        this.chHour = chHour / 3600;
        spin_zoom_widget_on_frame_start(&this.tPointers[0], chHour, 1.0f);

        lTimeStampInMs %= (3600ul * 1000ul);
        
    } while(0);

    /* calculate the Minutes */
    do {
        uint32_t chMin = lTimeStampInMs / 1000ul;

        this.chMin = chMin / 60;
        spin_zoom_widget_on_frame_start(&this.tPointers[1], chMin, 1.0f);

        lTimeStampInMs %= (60ul * 1000ul);
    } while(0);

    /* calculate the Seconds */
    do {
        uint32_t chSec = lTimeStampInMs;
        this.chSec = chSec / 1000;

#if __SCENE_WATCH_CFG_UPDATE_SECOND_POINTER_ONCE_PER_SECOND__
        meter_pointer_on_frame_start(&this.tSecPointer, this.chSec, 1.0f);
#else
        spin_zoom_widget_on_frame_start(&this.tPointers[2], chSec, 1.0f);
#endif
        lTimeStampInMs %= (1000ul);
    } while(0);

    /* calculate the Ten-Miliseconds */
    do {
        this.chMs = lTimeStampInMs;
    } while(0);

}

static void __on_scene_watch_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_foreach(spin_zoom_widget_t, this.tPointers, ptPointer) {
        spin_zoom_widget_on_frame_complete(ptPointer);
    }

#if __SCENE_WATCH_CFG_UPDATE_SECOND_POINTER_ONCE_PER_SECOND__
    meter_pointer_on_frame_complete(&this.tSecPointer);
#endif
    
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
        //arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static void __before_scene_watch_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
void __draw_watch_panel(const arm_2d_tile_t *ptTile, 
                        const arm_2d_region_t *ptRegion, 
                        user_scene_watch_t *ptThis)
{
    arm_2d_container(ptTile, __panel, ptRegion) {


    #if 0 /* use simple background picture */
        arm_2d_tile_copy_with_opacity(  &c_tileWatchPanel,
                                        &__panel,
                                        &__panel_canvas,
                                        64);

        ARM_2D_OP_WAIT_ASYNC();
    #else   /* draw digits directly */

        arm_2d_align_centre(__panel_canvas, 200, 200) {
            arm_2d_size_t tDigitsSize = arm_lcd_get_string_line_box("00", &ARM_2D_FONT_ALARM_CLOCK_32_A4);

            for (int_fast8_t n = 0; n < dimof(s_tDigitsTable); n++) {
                arm_2d_region_t tDigitsRegion = {
                    .tLocation = __centre_region.tLocation, //s_tDigitsTable[n].tLocation,
                    .tSize = tDigitsSize,
                };

                tDigitsRegion.tLocation.iX += s_tDigitsTable[n].tLocation.iX - (tDigitsSize.iWidth >> 1);
                tDigitsRegion.tLocation.iY += s_tDigitsTable[n].tLocation.iY - (tDigitsSize.iHeight >> 1);

                arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)&__panel);
                arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_ALARM_CLOCK_32_A4);
                arm_lcd_text_set_draw_region(&tDigitsRegion);

                arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
                
                arm_lcd_text_location(0,0);
                
                
                arm_lcd_printf_label(ARM_2D_ALIGN_CENTRE, "%d", s_tDigitsTable[n].chNumber);
            
            }
        }

    #endif
    }
}


static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_watch_handler)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(tScreenSize);
    
    arm_2d_canvas(ptTile, __canvas) {
    /*-----------------------draw the foreground begin-----------------------*/

        arm_2d_align_centre(__canvas, 240, 240) {

            __draw_watch_panel(ptTile, &__centre_region, ptThis);

            arm_foreach(spin_zoom_widget_t, this.tPointers, ptPointer) {
                spin_zoom_widget_show(ptPointer, ptTile, &__centre_region, NULL, 255);
            }
        
        #if __SCENE_WATCH_CFG_UPDATE_SECOND_POINTER_ONCE_PER_SECOND__
                meter_pointer_show(&this.tSecPointer, ptTile, &__centre_region, NULL, 255);
        #endif

        }

        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene watch");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_watch_t *__arm_2d_scene_watch_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_watch_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_watch_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_watch_t),
                                                        __alignof__(user_scene_watch_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
        memset(ptThis, 0, sizeof(user_scene_watch_t));
    }
    
    *ptThis = (user_scene_watch_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 
        
            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_watch_load,
            .fnScene        = &__pfb_draw_scene_watch_handler,
            

            //.fnOnBGStart    = &__on_scene_watch_background_start,
            //.fnOnBGComplete = &__on_scene_watch_background_complete,
            .fnOnFrameStart = &__on_scene_watch_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_watch_switching_out,
            .fnOnFrameCPL   = &__on_scene_watch_frame_complete,
            .fnDepose       = &__on_scene_watch_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

#if __SCENE_WATCH_CFG_UPDATE_SECOND_POINTER_ONCE_PER_SECOND__
    do {
        meter_pointer_cfg_t tCFG = {
            .tSpinZoom = {
                .Indicator = {
                    .LowerLimit = {
                        .fAngleInDegree = 0.0f,
                        .nValue = 0,
                    },
                    .UpperLimit = {
                        .fAngleInDegree = 360.0f,
                        .nValue = 60,
                    },
                    .Step = {
                        .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                    },
                },
                .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
                .Source = {
                    .ptMask = &c_tilePointerSecMask,
                    .tColourToFill = GLCD_COLOR_RED,
                },
                .ptScene = (arm_2d_scene_t *)ptThis,
            },

            .Pointer = {
                .bIsSourceHorizontal = false,
                .iRadius = 100,
            },

            .tPISliderCFG = {
                .fProportion = 0.3000f,
                .fIntegration = 0.150f,
                .nInterval = 10,
            }

        };
        meter_pointer_init(&this.tSecPointer, &tCFG);
    } while(0);
#else
    // initialize second pointer
    do {
        s_tPointerSecCenter.iX = (c_tilePointerSecMask.tRegion.tSize.iWidth >> 1);
        s_tPointerSecCenter.iY = 100;  /* radius */

        spin_zoom_widget_cfg_t tCFG = {
            .Indicator = {
                .LowerLimit = {
                    .fAngleInDegree = 0.0f,
                    .nValue = 0,
                },
                .UpperLimit = {
                    .fAngleInDegree = 360.0f,
                    .nValue = 60*1000ul,
                },
                .Step = {
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
            .Source = {
                .ptMask = &c_tilePointerSecMask,
                .tCentre = s_tPointerSecCenter,
                .tColourToFill = GLCD_COLOR_RED,
            },
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tPointers[2], &tCFG);
    } while(0);
#endif

    // initialize minutes pointer
    do {
        s_tPointerMinCenter.iX = (c_tilePointerMinMask.tRegion.tSize.iWidth >> 1);
        s_tPointerMinCenter.iY = c_tilePointerMinMask.tRegion.tSize.iHeight;
        
        spin_zoom_widget_cfg_t tCFG = {
            .Indicator = {
                .LowerLimit = {
                    .fAngleInDegree = 0.0f,
                    .nValue = 0,
                },
                .UpperLimit = {
                    .fAngleInDegree = 360.0f,
                    .nValue = 3600,
                },
                .Step = {
                    .fAngle = 1.0f,
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
            .Source = {
                .ptMask = &c_tilePointerMinMask,
                .tCentre = s_tPointerMinCenter,
                .tColourToFill = GLCD_COLOR_WHITE,
            },
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tPointers[1], &tCFG);
    } while(0);

    // initialize hour pointer
    do {
        s_tPointerHourCenter.iX = (c_tilePointerHourMask.tRegion.tSize.iWidth >> 1);
        s_tPointerHourCenter.iY = c_tilePointerHourMask.tRegion.tSize.iHeight;
        
        spin_zoom_widget_cfg_t tCFG = {
            .Indicator = {
                .LowerLimit = {
                    .fAngleInDegree = 0.0f,
                    .nValue = 0,
                },
                .UpperLimit = {
                    .fAngleInDegree = 360.0f,
                    .nValue = 12 * 3600ul,
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
            .Source = {
                .ptMask = &c_tilePointerHourMask,
                .tCentre = s_tPointerHourCenter,
                .tColourToFill = GLCD_COLOR_WHITE,
            },
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tPointers[0], &tCFG);

    } while(0);

    /* initialize watch panel digits table */
    do {
        #define __PI        (3.1415926f)
        #define __RADIUS    (100.0f)

        for (int_fast8_t n = 0; n < dimof(s_tDigitsTable); n++) {
            s_tDigitsTable[n].chNumber = n;

            s_tDigitsTable[n].tLocation.iY = (int16_t)(arm_sin_f32(ARM_2D_ANGLE(n*30.0f - 90.0f)) * __RADIUS) + __RADIUS;
            s_tDigitsTable[n].tLocation.iX = (int16_t)(arm_cos_f32(ARM_2D_ANGLE(n*30.0f - 90.0f)) * __RADIUS) + __RADIUS;
        }

        s_tDigitsTable[0].chNumber = 12;
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

