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

#define __USER_SCENE_RING_INDICATOR_IMPLEMENT__
#include "arm_2d_scene_ring_indicator.h"

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

#if ARM_2D_DEMO_RING_INDICATOR_USE_QOI
#   define BACKGROUND_RING_MASK        \
                        this.QOI[QOI_IDX_BACKGROUND_RING].tHelper.vres.tTile

#   define INDICATION_IMAGE    this.QOI[QOI_IDX_FOREGROUND].tHelper.vres.tTile
#   define INDICATION_IMAGE_MASK    (*(arm_2d_tile_t *)(NULL))
#else
#   define BACKGROUND_RING_MASK     c_tileThreeQuarterRingA4Mask
#   define INDICATION_IMAGE         c_tileRingIndicator
#   define INDICATION_IMAGE_MASK    c_tileRingIndicatorMask
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
extern const arm_2d_tile_t c_tileThreeQuarterRingA4Mask;
extern const arm_2d_tile_t c_tileRingIndicator;
extern const arm_2d_tile_t c_tileRingIndicatorMask;

extern const arm_2d_tile_t c_tileWhiteDotMiddleMask;
extern const arm_2d_tile_t c_tileWhiteDotMask;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
ARM_NOINIT
static
arm_2d_location_t s_tReferencePoints[4];


/*============================ IMPLEMENTATION ================================*/

static void __on_scene_ring_indicator_load(arm_2d_scene_t *ptScene)
{
    user_scene_ring_indicator_t *ptThis = (user_scene_ring_indicator_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if ARM_2D_DEMO_RING_INDICATOR_USE_QOI
    arm_foreach(this.QOI) {
        arm_qoi_loader_on_load(&_->tHelper);
    }
#endif

    spin_zoom_widget_on_load(&this.tPointer);
    ring_indication_on_load(&this.tIndicator);

    this.tPointer.tHelper.SourceReference.ptPoints = s_tReferencePoints;
    this.tPointer.tHelper.SourceReference.chCount = dimof(s_tReferencePoints);

}

static void __after_scene_ring_indicator_switching(arm_2d_scene_t *ptScene)
{
    user_scene_ring_indicator_t *ptThis = (user_scene_ring_indicator_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_ring_indicator_depose(arm_2d_scene_t *ptScene)
{
    user_scene_ring_indicator_t *ptThis = (user_scene_ring_indicator_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*--------------------- insert your depose code begin --------------------*/
#if ARM_2D_DEMO_RING_INDICATOR_USE_QOI
    arm_foreach(this.QOI) {
        arm_qoi_loader_depose(&_->tHelper);
    }
#endif

    ring_indication_depose(&this.tIndicator);
    spin_zoom_widget_depose(&this.tPointer);
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
 * Scene ring_indicator                                                       *
 *----------------------------------------------------------------------------*/
#if 0  /* deprecated */
static void __on_scene_ring_indicator_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_ring_indicator_t *ptThis = (user_scene_ring_indicator_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_ring_indicator_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_ring_indicator_t *ptThis = (user_scene_ring_indicator_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}
#endif


static void __on_scene_ring_indicator_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_ring_indicator_t *ptThis = (user_scene_ring_indicator_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*-----------------------    IMPORTANT MESSAGE    -----------------------*
     * It is better to update the 3 digits and pointer angle here            *
     *-----------------------------------------------------------------------*/

    do {

    #if 1
        /* generate a new position every 2000 sec */
        if (arm_2d_helper_is_time_out(3000,  &this.lTimestamp[0])) {
            this.lTimestamp[0] = 0;
            srand(arm_2d_helper_get_system_timestamp());
            this.iTargetNumber = rand() % 1000;
        }
    #else
        int32_t nResult;
        /* simulate a full battery charging/discharge cycle */
        arm_2d_helper_time_cos_slider(0, 1000, 3000, 0, &nResult, &this.lTimestamp[0]);

        this.iTargetNumber = nResult;

    #endif

        ring_indication_on_frame_start(&this.tIndicator, this.iTargetNumber);

        int32_t nCurrentValue = ring_indication_get_current_value(&this.tIndicator);
        spin_zoom_widget_on_frame_start(&this.tPointer, nCurrentValue, 1.0f);

    #if 0
        spin_zoom_widget_set_colour(&this.tPointer,
                                    arm_2d_pixel_from_brga8888( 
                                            __arm_2d_helper_colour_slider(
                                                __RGB32(0x00, 0xFF, 0x00),
                                                __RGB32(0xFF, 0xFF, 0x00),
                                                1000,
                                                nCurrentValue)));
    #endif

    } while(0);

#if ARM_2D_DEMO_RING_INDICATOR_USE_QOI
    arm_foreach(this.QOI) {
        arm_qoi_loader_on_frame_start(&_->tHelper);
    }
#endif
}

static void __on_scene_ring_indicator_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_ring_indicator_t *ptThis = (user_scene_ring_indicator_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if ARM_2D_DEMO_RING_INDICATOR_USE_QOI
    arm_foreach(this.QOI) {
        arm_qoi_loader_on_frame_complete(&_->tHelper);
    }
#endif

    ring_indication_on_frame_complete(&this.tIndicator);
    spin_zoom_widget_on_frame_complete(&this.tPointer);
}

static void __before_scene_ring_indicator_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_ring_indicator_t *ptThis = (user_scene_ring_indicator_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
RING_INDICATION_USER_DRAW(__user_draw_pointer)
{
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(ptPivot);
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(bIsNewFrame);

    spin_zoom_widget_t *ptHelper = (spin_zoom_widget_t *)pTarget;

    assert(NULL != ptHelper);

    /* draw circle */
    spin_zoom_widget_show(  ptHelper, 
                            ptTile, 
                            NULL, 
                            ptPivot, 
                            255);
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_ring_indicator_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_ring_indicator_t *ptThis = (user_scene_ring_indicator_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/

        arm_2d_align_centre(__top_canvas, 
                            ring_indication_get_size(&this.tIndicator)) {

        #if ARM_2D_DEMO_RING_INDICATOR_USE_QOI
            arm_2d_fill_colour_with_mask_and_opacity(   
                                    ptTile,
                                    &__centre_region,
                                    &BACKGROUND_RING_MASK, 
                                    (__arm_2d_color_t){ GLCD_COLOR_LIGHT_GREY},
                                    64);
        #else
            arm_2d_fill_colour_with_a4_mask_and_opacity(   
                                    ptTile,
                                    &__centre_region,
                                    &BACKGROUND_RING_MASK, 
                                    (__arm_2d_color_t){ GLCD_COLOR_LIGHT_GREY},
                                    64);
        #endif

            ARM_2D_OP_WAIT_ASYNC();
        }

        ring_indication_show(   &this.tIndicator, 
                                ptTile, 
                                NULL,
                                bIsNewFrame);

        /* draw text at the top-left corner */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
    #if ARM_2D_DEMO_RING_INDICATOR_USE_QOI
        arm_lcd_puts("Scene Ring Indicator with QOI");
    #else
        arm_lcd_puts("Scene Ring Indicator");
    #endif

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_ring_indicator_t *__arm_2d_scene_ring_indicator_init(
                                        arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_ring_indicator_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    /* get the screen region */
    arm_2d_region_t __top_canvas
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    ARM_2D_UNUSED(__top_canvas);

    if (NULL == ptThis) {
        ptThis = (user_scene_ring_indicator_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_ring_indicator_t),
                                                        __alignof__(user_scene_ring_indicator_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_ring_indicator_t));

    *ptThis = (user_scene_ring_indicator_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_WHITE}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_ring_indicator_load,
            .fnScene        = &__pfb_draw_scene_ring_indicator_handler,
            .fnAfterSwitch  = &__after_scene_ring_indicator_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene_ring_indicator_background_start,        /* deprecated */
            //.fnOnBGComplete = &__on_scene_ring_indicator_background_complete,     /* deprecated */
            .fnOnFrameStart = &__on_scene_ring_indicator_frame_start,
            .fnBeforeSwitchOut = &__before_scene_ring_indicator_switching_out,
            .fnOnFrameCPL   = &__on_scene_ring_indicator_frame_complete,
            .fnDepose       = &__on_scene_ring_indicator_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_ring_indicator_t begin ---------------*/

#if ARM_2D_DEMO_RING_INDICATOR_USE_QOI
    /* initialize QOI loader */
    do {
    #if ARM_2D_DEMO_QOI_USE_FILE
        arm_qoi_io_file_loader_init(&this.QOI[QOI_IDX_BACKGROUND_RING].LoaderIO.tFile, 
                                    "../common/asset/three_quarter_ring.qoi");
    #else
        extern const uint8_t c_qoiThreeQuarterRing[8252];

        arm_qoi_io_binary_loader_init(  &this.QOI[QOI_IDX_BACKGROUND_RING].LoaderIO.tBinary, 
                                        c_qoiThreeQuarterRing, 
                                        sizeof(c_qoiThreeQuarterRing));
    #endif
        arm_qoi_loader_cfg_t tCFG = {
            //.bUseHeapForVRES = true,
            .ptScene = (arm_2d_scene_t *)ptThis,
            .u2WorkMode = ARM_QOI_MODE_PARTIAL_DECODED,

            /* you can only extract specific colour channel and use it as A8 mask */
            .tColourInfo.chScheme = ARM_2D_COLOUR_MASK_A8,
            .u2ChannelIndex = ARM_QOI_MASK_CHN_ALPHA,   

            //.bInvertColour = true,
            //.bForceDisablePreBlendwithBG = true,
            //.tBackgroundColour.wColour = this.use_as__arm_2d_scene_t.tCanvas.wColour,
        #if ARM_2D_DEMO_QOI_USE_FILE
            .ImageIO = {
                .ptIO = &ARM_QOI_IO_FILE_LOADER,
                .pTarget = (uintptr_t)&this.QOI[QOI_IDX_BACKGROUND_RING].LoaderIO.tFile,
            },
        #else
            .ImageIO = {
                .ptIO = &ARM_QOI_IO_BINARY_LOADER,
                .pTarget = (uintptr_t)&this.QOI[QOI_IDX_BACKGROUND_RING].LoaderIO.tBinary,
            },
        #endif
        };

        arm_qoi_loader_init(&this.QOI[QOI_IDX_BACKGROUND_RING].tHelper, &tCFG);
    } while(0);

    do {
    #if ARM_2D_DEMO_QOI_USE_FILE
        arm_qoi_io_file_loader_init(&this.QOI[QOI_IDX_FOREGROUND].LoaderIO.tFile, 
                                    "../common/asset/ring_indicator.qoi");
    #else
        extern const uint8_t c_qoiRingIndicator[32948];

        arm_qoi_io_binary_loader_init(  &this.QOI[QOI_IDX_FOREGROUND].LoaderIO.tBinary, 
                                        c_qoiRingIndicator, 
                                        sizeof(c_qoiRingIndicator));
    #endif
        arm_qoi_loader_cfg_t tCFG = {
            //.bUseHeapForVRES = true,
            .ptScene = (arm_2d_scene_t *)ptThis,
            .u2WorkMode = ARM_QOI_MODE_PARTIAL_DECODED,

            //.bInvertColour = true,
            //.bForceDisablePreBlendwithBG = true,
            .tBackgroundColour.wColour = this.use_as__arm_2d_scene_t.tCanvas.wColour,
        #if ARM_2D_DEMO_QOI_USE_FILE
            .ImageIO = {
                .ptIO = &ARM_QOI_IO_FILE_LOADER,
                .pTarget = (uintptr_t)&this.QOI[QOI_IDX_FOREGROUND].LoaderIO.tFile,
            },
        #else
            .ImageIO = {
                .ptIO = &ARM_QOI_IO_BINARY_LOADER,
                .pTarget = (uintptr_t)&this.QOI[QOI_IDX_FOREGROUND].LoaderIO.tBinary,
            },
        #endif
        };

        arm_qoi_loader_init(&this.QOI[QOI_IDX_FOREGROUND].tHelper, &tCFG);
    } while(0);
#endif

    do {
        int16_t iRadius = (float)c_tileWhiteDotMiddleMask.tRegion.tSize.iWidth 
                        - (float)INDICATION_IMAGE.tRegion.tSize.iWidth / 2.0f
                        - 4;

        spin_zoom_widget_cfg_t tCFG = {
            .Indicator = {
                .LowerLimit = {
                    .fAngleInDegree = 134.0f,
                    .nValue = 0,
                },
                .UpperLimit = {
                    .fAngleInDegree = 45.0f + 360.0f,
                    .nValue = 1000,
                },
                .Step = {
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,

            .Source = {
                .ptMask = &c_tileWhiteDotMiddleMask,
                .tCentreFloat = {
                    .fX = iRadius,
                    .fY = (float)(c_tileWhiteDotMiddleMask.tRegion.tSize.iHeight - 1) / 2.0f,
                },
                .tColourToFill = GLCD_COLOR_NIXIE_TUBE,
            },
            .bUseFloatPointInCentre = true,

            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tPointer, &tCFG);

        s_tReferencePoints[0].iX = -30;
        s_tReferencePoints[0].iY = 0;

        s_tReferencePoints[1].iX = c_tileWhiteDotMiddleMask.tRegion.tSize.iWidth - 1;
        s_tReferencePoints[1].iY = 0;

        s_tReferencePoints[2].iX = c_tileWhiteDotMiddleMask.tRegion.tSize.iWidth - 1;
        s_tReferencePoints[2].iY = c_tileWhiteDotMiddleMask.tRegion.tSize.iHeight - 1;

        s_tReferencePoints[3].iX = -30;
        s_tReferencePoints[3].iY = c_tileWhiteDotMiddleMask.tRegion.tSize.iHeight - 1;

        /*
         * NOTE: Reference Point
         *
         *              0     +-----1
         *   +------...-+-----+ DOT |
         * pivot        3     +-----2
         * 
         *   |<--- radius --->|
         */

    } while(0);
    
    /* initialize scan sector of the indicator*/
    do {
        extern const arm_2d_tile_t c_tileQuaterSectorMask;

        ring_indication_cfg_t tCFG = {
            .tIndication = {
                .LowerLimit = {
                    .fAngleInDegree = 134.0f,
                    .nValue = 0,
                },
                .UpperLimit = {
                    .fAngleInDegree = 45.0f,
                    .nValue = 1000,
                },
                .Step = {
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
        
        #if ARM_2D_DEMO_RING_INDICATOR_USE_QOI                      \
         || (   (__GLCD_CFG_COLOUR_DEPTH__ == 32)                   \
            &&  __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__)
            .ptTransformMode = &SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK,
        #else
            .ptTransformMode = &SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK,
        #endif
            .QuarterSector = {
                .fAngleOffset = 0.0f,
                .ptMask = &c_tileQuaterSectorMask,
                .tCentre = {
                    .fX = 0.5f,
                    .fY = (float)c_tileQuaterSectorMask.tRegion.tSize.iHeight - (2.0f + 0.5f),
                },
            },
            .Foreground = {
                .ptTile = &INDICATION_IMAGE,
                .ptMask = &INDICATION_IMAGE_MASK,
            },

            .tPISliderCFG = {
                .fProportion = 0.0300f,
                .fIntegration = 0.0020f,
                .nInterval = 10,
            },

            /* use external dirty region item */
            .ptUserDirtyRegionItem = &this.tPointer.tHelper.tItem,

            /* add a user defined draw handler to draw the pointer */
            .evtUserDraw = {
                .fnHandler = &__user_draw_pointer,
                .pTarget = &this.tPointer,
            },

            .ptScene = (arm_2d_scene_t *)ptThis,
        };

        ring_indication_init(&this.tIndicator, &tCFG);

        this.iTargetNumber = 0;
    } while(0);

    /* ------------   initialize members of user_scene_ring_indicator_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


