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

#define __USER_SCENE_MONO_CLOCK_IMPLEMENT__
#include "arm_2d_scene_mono_clock.h"

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
    DIRTY_REGION_IDX_HOUR,
    DIRTY_REGION_IDX_MIN,
    DIRTY_REGION_IDX_SEC,
    DIRTY_REGION_IDX_TENMS,
    DIRTY_REGION_IDX_ECG,
};

/*============================ GLOBAL VARIABLES ==============================*/
extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_SQUARE_DIGITS_24_A1,
  ARM_2D_FONT_SQUARE_DIGITS_24_A2,
  ARM_2D_FONT_SQUARE_DIGITS_24_A4,
  ARM_2D_FONT_SQUARE_DIGITS_24_A8;

#define CLOCK_FONT      ARM_2D_FONT_SQUARE_DIGITS_24_A1


/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

    /* a dirty region for hour */
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),

    /* a dirty region for mins */
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),

    /* a dirty region for second */
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),

    /* a dirty region for TenMs */
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),

    /* add the last region for ECG */
    ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
        0
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_mono_clock_load(arm_2d_scene_t *ptScene)
{
    user_scene_mono_clock_t *ptThis = (user_scene_mono_clock_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __after_scene_mono_clock_switching(arm_2d_scene_t *ptScene)
{
    user_scene_mono_clock_t *ptThis = (user_scene_mono_clock_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_mono_clock_depose(arm_2d_scene_t *ptScene)
{
    user_scene_mono_clock_t *ptThis = (user_scene_mono_clock_t *)ptScene;
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
 * Scene mono_clock                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_mono_clock_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_mono_clock_t *ptThis = (user_scene_mono_clock_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_mono_clock_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_mono_clock_t *ptThis = (user_scene_mono_clock_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_mono_clock_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_mono_clock_t *ptThis = (user_scene_mono_clock_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    int64_t lTimeStampInMs = arm_2d_helper_convert_ticks_to_ms(
                                arm_2d_helper_get_system_timestamp());

    /* calculate the hours */
    do {
        uint_fast8_t chHour = lTimeStampInMs / (3600ul * 1000ul);
        chHour %= 24;

        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_IDX_HOUR],
                                            (chHour == this.chHour)); 
        this.chHour = chHour;

        lTimeStampInMs %= (3600ul * 1000ul);
    } while(0);

    /* calculate the Minutes */
    do {
        uint_fast8_t chMin = lTimeStampInMs / (60ul * 1000ul);

        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_IDX_MIN],
                                            (chMin == this.chMin)); 
        this.chMin = chMin;

        lTimeStampInMs %= (60ul * 1000ul);
    } while(0);

    /* calculate the Seconds */
    do {
        uint_fast8_t chSec = lTimeStampInMs / (1000ul);

        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_IDX_SEC],
                                            (chSec == this.chSec)); 
        this.chSec = chSec;

        lTimeStampInMs %= (1000ul);
    } while(0);

    /* calculate the Ten-Miliseconds */
    do {
        uint_fast8_t chTenMs = lTimeStampInMs / (10ul);

        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_IDX_TENMS],
                                            (chTenMs == this.chTenMs)); 
        this.chTenMs = chTenMs;
    } while(0);

}

static void __on_scene_mono_clock_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_mono_clock_t *ptThis = (user_scene_mono_clock_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if 1
    /* switch to next scene after 5s */
    if (arm_2d_helper_is_time_out(5000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene_mono_clock_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_mono_clock_t *ptThis = (user_scene_mono_clock_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mono_clock_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_mono_clock_t *ptThis = (user_scene_mono_clock_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {


        arm_2d_dock_vertical(__top_canvas, 16, 16) {

            arm_lcd_text_set_scale(1.0f);
            arm_2d_size_t tStringSize = arm_lcd_get_string_line_box("00:00:00", &CLOCK_FONT);
            
            arm_2d_size_t tTwoDigitsSizeBig = arm_lcd_get_string_line_box("00", &CLOCK_FONT);
            arm_2d_size_t tCommaSizeBig = arm_lcd_get_string_line_box(":", &CLOCK_FONT);

            arm_lcd_text_set_scale(0.5f);
            arm_2d_size_t tCommaSizeSmall = arm_lcd_get_string_line_box(":", &CLOCK_FONT);
            arm_2d_size_t tTwoDigitsSizeSmall = arm_lcd_get_string_line_box("00", &CLOCK_FONT);
            
            tStringSize.iWidth += tTwoDigitsSizeSmall.iWidth + tCommaSizeSmall.iWidth;
            
            arm_lcd_text_set_target_framebuffer(ptTile);
            arm_lcd_text_set_font((arm_2d_font_t *)&CLOCK_FONT);
            arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
            arm_lcd_text_set_display_mode(ARM_2D_DRW_PATN_MODE_COPY);
            arm_lcd_text_set_scale(1.0f);
            arm_2d_dock_horizontal(__vertical_region, tStringSize.iWidth) {

                arm_2d_layout(__horizontal_region) {

                    __item_line_dock_horizontal(tTwoDigitsSizeBig.iWidth) {
                        arm_lcd_text_set_draw_region(&__item_region);
                        arm_lcd_printf("%02d", this.chHour);
                    }

                    __item_line_dock_horizontal(tCommaSizeBig.iWidth) {
                        arm_lcd_text_set_draw_region(&__item_region);
                        arm_lcd_puts(":");
                    }

                    __item_line_dock_horizontal(tTwoDigitsSizeBig.iWidth) {
                        arm_lcd_text_set_draw_region(&__item_region);
                        arm_lcd_printf("%02d", this.chMin);
                    }

                    __item_line_dock_horizontal(tCommaSizeBig.iWidth) {
                        arm_lcd_text_set_draw_region(&__item_region);
                        arm_lcd_puts(":");
                    }

                    __item_line_dock_horizontal(tTwoDigitsSizeBig.iWidth) {
                        arm_lcd_text_set_draw_region(&__item_region);
                        arm_lcd_printf("%02d", this.chSec);
                    }

                    __item_line_dock_horizontal(tCommaSizeSmall.iWidth) {
                    }

                    __item_line_dock_horizontal(tTwoDigitsSizeSmall.iWidth) {
                        
                        arm_lcd_text_set_scale(0.5f);
                        arm_2d_align_centre(__item_region, tTwoDigitsSizeSmall) {
                            arm_lcd_text_set_draw_region(&__centre_region);

                            arm_lcd_text_reset_display_region_tracking();

                            arm_lcd_text_force_char_use_same_width(true);
                            arm_lcd_printf("%02d", this.chTenMs);
                            arm_lcd_text_force_char_use_same_width(false);
                            if (bIsNewFrame) {
                                s_tDirtyRegions[DIRTY_REGION_IDX_TENMS].tRegion = *arm_lcd_text_get_last_display_region();
                            }
                        }
                    }
                }
            }

            arm_lcd_text_set_scale(1.0f);
        }

    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_mono_clock_t *__arm_2d_scene_mono_clock_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_mono_clock_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);

    /*--------------initialize static dirty region items: begin---------------*/


    arm_2d_dock_vertical(tScreen, 16, 16) {

        arm_lcd_text_set_scale(1.0f);
        arm_2d_size_t tStringSize = arm_lcd_get_string_line_box("00:00:00", &CLOCK_FONT);
        
        arm_2d_size_t tTwoDigitsSizeBig = arm_lcd_get_string_line_box("00", &CLOCK_FONT);
        arm_2d_size_t tCommaSizeBig = arm_lcd_get_string_line_box(":", &CLOCK_FONT);

        arm_lcd_text_set_scale(0.5f);
        arm_2d_size_t tCommaSizeSmall = arm_lcd_get_string_line_box(":", &CLOCK_FONT);
        arm_2d_size_t tTwoDigitsSizeSmall = arm_lcd_get_string_line_box("00", &CLOCK_FONT);
        
        tStringSize.iWidth += tTwoDigitsSizeSmall.iWidth + tCommaSizeSmall.iWidth;
        
        arm_lcd_text_set_font((arm_2d_font_t *)&CLOCK_FONT);
        arm_lcd_text_set_scale(1.0f);

        arm_2d_dock_horizontal(__vertical_region, tStringSize.iWidth) {

            arm_2d_layout(__horizontal_region) {

                __item_line_dock_horizontal(tTwoDigitsSizeBig.iWidth) {
                    
                    s_tDirtyRegions[DIRTY_REGION_IDX_HOUR].tRegion = __item_region;

                }

                __item_line_dock_horizontal(tCommaSizeBig.iWidth) {

                }

                __item_line_dock_horizontal(tTwoDigitsSizeBig.iWidth) {
                    s_tDirtyRegions[DIRTY_REGION_IDX_MIN].tRegion = __item_region;
                }

                __item_line_dock_horizontal(tCommaSizeBig.iWidth) {
                    
                }

                __item_line_dock_horizontal(tTwoDigitsSizeBig.iWidth) {
                    s_tDirtyRegions[DIRTY_REGION_IDX_SEC].tRegion = __item_region;
                }

                __item_line_dock_horizontal(tCommaSizeSmall.iWidth) {
                }

            #if 0
                __item_line_dock_horizontal(tTwoDigitsSizeSmall.iWidth) {
                    arm_2d_align_centre(__item_region, tTwoDigitsSizeSmall) {
                        s_tDirtyRegions[DIRTY_REGION_IDX_TENMS].tRegion = __centre_region;
                    }
                }
            #endif
            }
        }

        arm_lcd_text_set_scale(1.0f);
    }


    /*--------------initialize static dirty region items: end  ---------------*/

    if (NULL == ptThis) {
        ptThis = (user_scene_mono_clock_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_mono_clock_t),
                                                        __alignof__(user_scene_mono_clock_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_mono_clock_t));

    *ptThis = (user_scene_mono_clock_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_mono_clock_load,
            .fnScene        = &__pfb_draw_scene_mono_clock_handler,
            //.fnAfterSwitch  = &__after_scene_mono_clock_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_mono_clock_background_start,
            //.fnOnBGComplete = &__on_scene_mono_clock_background_complete,
            .fnOnFrameStart = &__on_scene_mono_clock_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_mono_clock_switching_out,
            .fnOnFrameCPL   = &__on_scene_mono_clock_frame_complete,
            .fnDepose       = &__on_scene_mono_clock_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_mono_clock_t begin ---------------*/

    /* ------------   initialize members of user_scene_mono_clock_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


