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

#define __USER_SCENE_WARPED_DIAL_IMPLEMENT__
#include "arm_2d_scene_warped_dial.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include <stdlib.h>
#include <string.h>
#include <math.h>

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
#   define c_tileRingIndicator      c_tileRingIndicatorGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565
#   define c_tileRingIndicator      c_tileRingIndicatorRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#   define c_tileRingIndicator      c_tileRingIndicatorCCCA8888
#else
#   error Unsupported colour depth!
#endif

#if ARM_2D_DEMO_WARPED_DIAL_USE_LMSK
#   define INDICATION_IMAGE_MASK    this.LMSK.tHelper.tTile
#else
#   define INDICATION_IMAGE_MASK    c_tileDashboardRingMask
#endif

#undef this
#define this (*ptThis)

#define WARPED_CIRCLE_WIDTH     (INDICATION_IMAGE_MASK.tRegion.tSize.iWidth - 20)        
#define WARPED_CIRCLE_R         ((INDICATION_IMAGE_MASK.tRegion.tSize.iHeight - 10) >> 1)
#define WARPED_CIRCLE_THETA     ARM_2D_ANGLE(90.0f - 8.0f)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileDashboardRingMask;

extern const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_ALARM_CLOCK_64_A4;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
ARM_NOINIT
static
arm_2d_location_t s_tReferencePoints[4];

const
__spin_zoom_widget_indication_t c_tIndicatorValueMapping = {
    .LowerLimit = {
        .fAngleInDegree = 45.0f,
        .nValue = 0,
    },
    .UpperLimit = {
        .fAngleInDegree = 360.0f - 45.0f,
        .nValue = 999,
    },
    .Step = {
        .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
    },
};

static float s_fCosTheta;
static float s_fSinTheta;
/*============================ IMPLEMENTATION ================================*/

static void __on_scene_warped_dial_load(arm_2d_scene_t *ptScene)
{
    user_scene_warped_dial_t *ptThis = (user_scene_warped_dial_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if ARM_2D_DEMO_WARPED_DIAL_USE_LMSK
    arm_lmsk_loader_on_load(&this.LMSK.tHelper);
#endif

    arm_2d_helper_dirty_region_add_items(   
                                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                &this.Tracking.tDirtyRegionItem, 
                                1);

    this.Tracking.tDirtyRegionItem.tRegionPatch.tSize.iWidth = 24;
    this.Tracking.tDirtyRegionItem.tRegionPatch.tSize.iHeight = 24;
    this.Tracking.tDirtyRegionItem.tRegionPatch.tLocation.iX = -12;
    this.Tracking.tDirtyRegionItem.tRegionPatch.tLocation.iY = -12;

    ring_indication_on_load(&this.tIndicator);

    this.bDirtyRegionOptimizationStatus 
        = arm_2d_helper_pfb_disable_dirty_region_optimization(
            &ptScene->ptPlayer->use_as__arm_2d_helper_pfb_t);
}

static void __after_scene_warped_dial_switching(arm_2d_scene_t *ptScene)
{
    user_scene_warped_dial_t *ptThis = (user_scene_warped_dial_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_warped_dial_depose(arm_2d_scene_t *ptScene)
{
    user_scene_warped_dial_t *ptThis = (user_scene_warped_dial_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*--------------------- insert your depose code begin --------------------*/
#if ARM_2D_DEMO_WARPED_DIAL_USE_LMSK
    arm_lmsk_loader_depose(&this.LMSK.tHelper);
#endif

    arm_2d_helper_dirty_region_remove_items(   
                                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                &this.Tracking.tDirtyRegionItem, 
                                1);

    ring_indication_depose(&this.tIndicator);
    /*---------------------- insert your depose code end  --------------------*/

    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }
    ptScene->ptPlayer = NULL;
    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene warped_dial                                                       *
 *----------------------------------------------------------------------------*/
#if 0  /* deprecated */
static void __on_scene_warped_dial_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_warped_dial_t *ptThis = (user_scene_warped_dial_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_warped_dial_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_warped_dial_t *ptThis = (user_scene_warped_dial_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}
#endif

static
arm_2d_point_float_t __get_rotated_ellipse_point(   user_scene_warped_dial_t *ptThis, 
                                                    float_t fPhi)
{

#undef A
#undef B
#undef A_SQUARED
#undef B_SQUARED
#undef AB

#define A           (WARPED_CIRCLE_R)
#define B           (WARPED_CIRCLE_WIDTH / 2.0f)
#define AB          (A * B)

    arm_2d_point_float_t tPoint;
    float fCosDelta, fSinDelta;
    float fDenominator;
    float fRho;
    float fTemp;
    
    float_t fCosPhi, fSinPhi;
    fCosPhi = arm_cos_f32(fPhi);
    fSinPhi = arm_sin_f32(fPhi);
    
    /* cos(phi-theta) = cos(phi)cos(theta) + sin(phi)sin(theta) */
    /* sin(phi-theta) = sin(phi)cos(theta) - cos(phi)sin(theta) */
    
    fCosDelta = fCosPhi * s_fCosTheta + fSinPhi * s_fSinTheta;
    fSinDelta = fSinPhi * s_fCosTheta - fCosPhi * s_fSinTheta;
    
    float fBCos = B * fCosDelta;
    float fASin = A * fSinDelta;
    fTemp = (fBCos * fBCos) + (fASin * fASin);
    arm_sqrt_f32(fTemp, &fDenominator);
    fRho = AB / fDenominator;
    
    tPoint.fX = fRho * fCosPhi;
    tPoint.fY = fRho * fSinPhi;
    
    return tPoint;
}


static void __on_scene_warped_dial_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_warped_dial_t *ptThis = (user_scene_warped_dial_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*-----------------------    IMPORTANT MESSAGE    -----------------------*
     * It is better to update the 3 digits and pointer angle here            *
     *-----------------------------------------------------------------------*/

    do {

        /* generate a new position every 2000 sec */
        if (arm_2d_helper_is_time_out(4000,  &this.lTimestamp[0])) {
            this.lTimestamp[0] = 0;
            srand(arm_2d_helper_get_system_timestamp());
            this.iTargetNumber = rand() % 1000;
        }

        if (ring_indication_on_frame_start(&this.tIndicator, this.iTargetNumber)) {
            /* finish moving */
            arm_2d_helper_dirty_region_item_suspend_update(
                &ptScene->tDirtyRegionHelper.tDefaultItem,
                true);
            
            arm_2d_helper_dirty_region_item_suspend_update(
                                                &this.Tracking.tDirtyRegionItem,
                                                true);
        } else {
            arm_2d_helper_dirty_region_item_suspend_update(
                &ptScene->tDirtyRegionHelper.tDefaultItem,
                false);

            arm_2d_helper_dirty_region_item_suspend_update(
                                                &this.Tracking.tDirtyRegionItem,
                                                false);
        }

        arm_2d_point_float_t tPosition = 
            __get_rotated_ellipse_point(ptThis,
                ARM_2D_ANGLE(
                    spin_zoom_widget_get_actual_angle(
                        &this.tIndicator.tSector.use_as__spin_zoom_widget_t)));

        this.Tracking.tBox.iX = tPosition.fX;
        this.Tracking.tBox.iY = tPosition.fY;

    } while(0);


#if ARM_2D_DEMO_WARPED_DIAL_USE_LMSK
    arm_lmsk_loader_on_frame_start(&this.LMSK.tHelper);
#endif
}

static void __on_scene_warped_dial_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_warped_dial_t *ptThis = (user_scene_warped_dial_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if ARM_2D_DEMO_WARPED_DIAL_USE_LMSK
    arm_lmsk_loader_on_frame_complete(&this.LMSK.tHelper);
#endif

    ring_indication_on_frame_complete(&this.tIndicator);
}

static void __before_scene_warped_dial_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_warped_dial_t *ptThis = (user_scene_warped_dial_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    if (this.bDirtyRegionOptimizationStatus) {
        arm_2d_helper_pfb_enable_dirty_region_optimization(
            &ptScene->ptPlayer->use_as__arm_2d_helper_pfb_t,
            NULL, 
            0);
    }
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_warped_dial_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_warped_dial_t *ptThis = (user_scene_warped_dial_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/

        arm_2d_align_centre(__top_canvas, 
                            ring_indication_get_size(&this.tIndicator)) {

        #if ARM_2D_DEMO_WARPED_DIAL_USE_LMSK
            arm_2d_fill_colour_with_mask_and_opacity(   
                                    ptTile,
                                    &__centre_region,
                                    &INDICATION_IMAGE_MASK, 
                                    (__arm_2d_color_t){ GLCD_COLOR_LIGHT_GREY},
                                    64);
        #else
            arm_2d_fill_colour_with_mask_and_opacity(   
                                    ptTile,
                                    &__centre_region,
                                    &INDICATION_IMAGE_MASK, 
                                    (__arm_2d_color_t){ GLCD_COLOR_LIGHT_GREY},
                                    64);
        #endif

            ARM_2D_OP_WAIT_ASYNC();
        }

        arm_2d_align_centre(__top_canvas, 
                            INDICATION_IMAGE_MASK.tRegion.tSize) {

        #if ARM_2D_DEMO_WARPED_DIAL_USE_LMSK
            arm_2d_fill_colour_with_mask_and_opacity(   
                                    ptTile,
                                    &__centre_region,
                                    &INDICATION_IMAGE_MASK, 
                                    (__arm_2d_color_t){ GLCD_COLOR_LIGHT_GREY},
                                    64);
        #else
            arm_2d_fill_colour_with_mask_and_opacity(   
                                    ptTile,
                                    &__centre_region,
                                    &INDICATION_IMAGE_MASK, 
                                    (__arm_2d_color_t){ GLCD_COLOR_LIGHT_GREY},
                                    64);
        #endif

            ARM_2D_OP_WAIT_ASYNC();

        }

        ring_indication_show(   &this.tIndicator, 
                                ptTile, 
                                NULL,
                                bIsNewFrame);

        arm_2d_align_centre_open(__top_canvas, 1, 1) {
            arm_2d_region_t tTrackingBox = __centre_region;
            tTrackingBox.tLocation.iX += this.Tracking.tBox.iX;
            tTrackingBox.tLocation.iY += this.Tracking.tBox.iY;

            arm_2d_helper_dirty_region_update_item( &this.Tracking.tDirtyRegionItem, 
                                                    ptTile,
                                                    NULL,
                                                    &tTrackingBox);
        }

        arm_lcd_text_force_char_use_same_width(true);
        /* draw 3 digits numbers */
        do {
            
            /* 3 digits */
            arm_2d_size_t tTextSize 
                = arm_lcd_printf_to_buffer(
                    (const arm_2d_font_t *)&ARM_2D_FONT_ALARM_CLOCK_64_A4, 
                    "%03d", 
                    (int)ring_indication_get_current_value(&this.tIndicator));

            tTextSize.iHeight += 16; 

            arm_2d_align_centre_open(__top_canvas,  tTextSize) {
                
                __centre_region.tLocation.iX += 40;

                __arm_2d_hint_optimize_for_pfb__(__centre_region) {
                    arm_2d_layout(__centre_region) {
                    
                        arm_lcd_text_set_target_framebuffer(ptTile);
                        /* print value */
                        __item_line_vertical(tTextSize.iWidth, tTextSize.iHeight - 16) {

                            arm_lcd_text_set_draw_region(&__item_region);
                            arm_lcd_text_set_colour( GLCD_COLOR_NIXIE_TUBE, GLCD_COLOR_BLACK);
                            arm_lcd_text_set_opacity(255 - 64);

                            arm_lcd_text_reset_display_region_tracking();
                            arm_lcd_printf_buffer(0);
                            arm_2d_helper_dirty_region_update_item(
                                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
                                ptTile,
                                &__item_region,
                                arm_lcd_text_get_last_display_region());

                            arm_lcd_text_set_opacity(255);
                        }
                        
                        /* print "Setting" */
                        __item_line_vertical(tTextSize.iWidth,16) {
                            arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_6x8);
                            arm_lcd_text_set_draw_region(&__item_region);
                            arm_lcd_text_set_colour( GLCD_COLOR_DARK_GREY, GLCD_COLOR_BLACK);
                            arm_lcd_printf_label(ARM_2D_ALIGN_BOTTOM_CENTRE, "Setting");
                        }

                        arm_lcd_text_set_target_framebuffer(NULL);
                    }
                }
            }
            
        } while(0);
        arm_lcd_text_force_char_use_same_width(false);

        /* draw text at the top-left corner */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
    #if ARM_2D_DEMO_WARPED_DIAL_USE_LMSK
        arm_lcd_puts("Scene Warped Dial with LMSK");
    #else
        arm_lcd_puts("Scene Warped Dial");
    #endif

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_warped_dial_t *__arm_2d_scene_warped_dial_init(
                                        arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_warped_dial_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    /* get the screen region */
    arm_2d_region_t __top_canvas
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    ARM_2D_UNUSED(__top_canvas);

    if (NULL == ptThis) {
        ptThis = (user_scene_warped_dial_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_warped_dial_t),
                                                        __alignof__(user_scene_warped_dial_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_warped_dial_t));

    *ptThis = (user_scene_warped_dial_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_warped_dial_load,
            .fnScene        = &__pfb_draw_scene_warped_dial_handler,
            .fnAfterSwitch  = &__after_scene_warped_dial_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene_warped_dial_background_start,        /* deprecated */
            //.fnOnBGComplete = &__on_scene_warped_dial_background_complete,     /* deprecated */
            .fnOnFrameStart = &__on_scene_warped_dial_frame_start,
            .fnBeforeSwitchOut = &__before_scene_warped_dial_switching_out,
            .fnOnFrameCPL   = &__on_scene_warped_dial_frame_complete,
            .fnDepose       = &__on_scene_warped_dial_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_warped_dial_t begin ---------------*/

#if ARM_2D_DEMO_WARPED_DIAL_USE_LMSK
    /* initialize LMSK loader */
    do {
        extern const uint8_t c_lmskDashboardRing[3844];

        arm_loader_io_rom_init( &this.LMSK.LoaderIO.tROM, 
                                (uintptr_t)c_lmskDashboardRing, 
                                sizeof(c_lmskDashboardRing));

        arm_lmsk_loader_cfg_t tCFG = {
            //.bUseHeapForVRES = true,
            .ptScene = (arm_2d_scene_t *)ptThis,

        #if __ARM_LMSK_USE_LOADER_IO__
            .ImageIO = {
                .ptIO = &ARM_LOADER_IO_ROM,
                .pTarget = (uintptr_t)&this.LMSK.LoaderIO.tROM,
            },
        #else
            .pchLMSKSource = c_lmskDashboardRing,
        #endif
        };

        arm_lmsk_loader_init(&this.LMSK.tHelper, &tCFG);
    } while(0);
#endif
    
    /* initialize scan sector of the indicator*/
    do {
        extern const arm_2d_tile_t c_tileQuaterSectorMask;

        ring_indication_cfg_t tCFG = {
            .tIndication = c_tIndicatorValueMapping,
        
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR_WITH_TARGET_MASK,

            .QuarterSector = {
                .fAngleOffset = 0.0f,
                .ptMask = &c_tileQuaterSectorMask,
                .tCentre = {
                    .fX = 0.5f,
                    .fY = (float)c_tileQuaterSectorMask.tRegion.tSize.iHeight - (2.0f + 0.5f),
                },
            },
            .Foreground = {
                .tColourToFill = __RGB(0x20, 0x43, 0xa4),
            },

            .Background.ptMask = &INDICATION_IMAGE_MASK,

            .tPISliderCFG = {
                .fProportion = 0.0300f,
                .fIntegration = 0.0020f,
                .nInterval = 10,
            },

            .ptUserDirtyRegionItem = &this.Tracking.tDirtyRegionItem,

            .ptScene = (arm_2d_scene_t *)ptThis,
        };

        ring_indication_init(&this.tIndicator, &tCFG);

        this.iTargetNumber = 0;
    } while(0);

    s_fCosTheta = arm_cos_f32(WARPED_CIRCLE_THETA);  // 或者用 arm_cos_f32 预先计算
    s_fSinTheta = arm_sin_f32(WARPED_CIRCLE_THETA);  // 同上

    /* ------------   initialize members of user_scene_warped_dial_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


