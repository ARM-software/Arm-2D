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

#define __USER_SCENE_FLIGHT_ATTITUDE_INSTRUMENT_IMPLEMENT__
#include "arm_2d_scene_flight_attitude_instrument.h"

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

#define VISUAL_AREA_MASK                    c_tileRoundedSquareMask

#define LAND_MASK                           c_tileSolidSquareMask
#define LAND_MASK_SCARE_RATIO               1.5f
#define HORIZON_MASK                        c_tileSolidLineMask

#define ROLL_SCALE_MARKER_MASK              c_tileRollScaleMarkerMask
#define ROLL_SCALE_MARKER_SCALE_RATIO       0.75f

#define PITCH_SCALE_MARKER_MASK             c_tilePitchScaleMask
#define PITCH_SCALE_MARKER_SCARE_RATIO      1.0f
#define PITCH_SCALE_MARKER_VISUAL_AREA_MASK c_tileSolidCircleSmallMask


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern const arm_2d_tile_t c_tileSolidCircleMask;
extern const arm_2d_tile_t c_tileSolidCircleSmallMask;
extern const arm_2d_tile_t c_tilePitchScaleMask;
extern const arm_2d_tile_t c_tileSolidSquareMask;
extern const arm_2d_tile_t c_tileRoundedSquareMask;
extern const arm_2d_tile_t c_tileSolidLineMask;
extern const arm_2d_tile_t c_tileRollScaleMarkerMask;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_LiberationSansRegular14_A4;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/


/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

    /* a dirty region to be specified at runtime*/
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),
    
    /* add the last region:
        * it is the top left corner for text display 
        */
    ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
        .tLocation = {
            .iX = 0,
            .iY = 0,
        },
        .tSize = {
            .iWidth = 0,
            .iHeight = 8,
        },
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

ARM_NOINIT
static
arm_2d_location_t s_tReferencePoints[4];

ARM_NOINIT
static
arm_2d_location_t s_tRollScaleMarkerReferencePoints[5];

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_flight_attitude_instrument_load(arm_2d_scene_t *ptScene)
{
    user_scene_flight_attitude_instrument_t *ptThis = (user_scene_flight_attitude_instrument_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    spin_zoom_widget_on_load(&this.Roll.tLand);

    this.Roll.tLand.tHelper.SourceReference.ptPoints = s_tReferencePoints;
    this.Roll.tLand.tHelper.SourceReference.chCount = dimof(s_tReferencePoints);

#if ARM_2D_DEMO_FAI_SHOW_HORIZON
    spin_zoom_widget_on_load(&this.Roll.tHorizon);
#endif
    spin_zoom_widget_on_load(&this.Roll.tMarker);
    this.Roll.tMarker.tHelper.SourceReference.ptPoints = s_tRollScaleMarkerReferencePoints;
    this.Roll.tMarker.tHelper.SourceReference.chCount = dimof(s_tRollScaleMarkerReferencePoints);

    spin_zoom_widget_on_load(&this.Pitch.tMarker);


}

static void __after_scene_flight_attitude_instrument_switching(arm_2d_scene_t *ptScene)
{
    user_scene_flight_attitude_instrument_t *ptThis = (user_scene_flight_attitude_instrument_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_flight_attitude_instrument_depose(arm_2d_scene_t *ptScene)
{
    user_scene_flight_attitude_instrument_t *ptThis = (user_scene_flight_attitude_instrument_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*--------------------- insert your depose code begin --------------------*/

    spin_zoom_widget_depose(&this.Roll.tLand);
#if ARM_2D_DEMO_FAI_SHOW_HORIZON
    spin_zoom_widget_depose(&this.Roll.tHorizon);
#endif
    spin_zoom_widget_depose(&this.Roll.tMarker);
    spin_zoom_widget_depose(&this.Pitch.tMarker);

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
 * Scene flight_attitude_instrument                                                                    *
 *----------------------------------------------------------------------------*/
#if 0  /* deprecated */
static void __on_scene_flight_attitude_instrument_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_flight_attitude_instrument_t *ptThis = (user_scene_flight_attitude_instrument_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_flight_attitude_instrument_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_flight_attitude_instrument_t *ptThis = (user_scene_flight_attitude_instrument_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}
#endif


static void __on_scene_flight_attitude_instrument_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_flight_attitude_instrument_t *ptThis = (user_scene_flight_attitude_instrument_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /* dummy rolling */
    do {
        int32_t nResult;

        arm_2d_helper_time_cos_slider(-450, 450, 7000, 0, &nResult, &this.lTimestamp[0]);
        //nResult += 3600;
        //if (nResult >= 3600) {
        //    nResult -= 3600;
        //}

        this.iRollScale = nResult;

    } while(0);

    /* dummy pulling back or pushing forward */
    do {
        int32_t nResult;

        arm_2d_helper_time_cos_slider(-300, 300, 17000, 0, &nResult, &this.lTimestamp[1]);

        this.iPitchScale = nResult;
    } while(0);

    spin_zoom_widget_set_source(&this.Roll.tLand, 
                                NULL,
                                &LAND_MASK,
                                (arm_2d_location_t) {
                                    LAND_MASK.tRegion.tSize.iWidth >> 1,
                                    reinterpret_s16_q16( 
                                        mul_n_q16(  this.Roll.q16PitchRatio, 
                                                    this.iPitchScale))
                                });

#if ARM_2D_DEMO_FAI_SHOW_HORIZON
    spin_zoom_widget_set_source(&this.Roll.tHorizon, 
                                NULL,
                                &HORIZON_MASK,
                                (arm_2d_location_t) {
                                    LAND_MASK.tRegion.tSize.iWidth >> 1,
                                    reinterpret_s16_q16( 
                                        mul_n_q16(  this.Roll.q16PitchRatio, 
                                                    this.iPitchScale))
                                });
#endif

    spin_zoom_widget_set_source(&this.Pitch.tMarker, 
                                NULL,
                                &PITCH_SCALE_MARKER_MASK,
                                (arm_2d_location_t) {
                                    PITCH_SCALE_MARKER_MASK.tRegion.tSize.iWidth >> 1,
                                    (PITCH_SCALE_MARKER_MASK.tRegion.tSize.iHeight >> 1) +
                                    reinterpret_s16_q16( 
                                        mul_n_q16(  this.Pitch.q16PitchRatio, 
                                                    this.iPitchScale))
                                });

    spin_zoom_widget_on_frame_start(&this.Roll.tLand, 
                                    this.iRollScale, 
                                    LAND_MASK_SCARE_RATIO);

#if ARM_2D_DEMO_FAI_SHOW_HORIZON
    spin_zoom_widget_on_frame_start(&this.Roll.tHorizon, 
                                    this.iRollScale, 
                                    LAND_MASK_SCARE_RATIO);
#endif

    spin_zoom_widget_on_frame_start(&this.Roll.tMarker, 
                                    this.iRollScale, 
                                    ROLL_SCALE_MARKER_SCALE_RATIO);

    spin_zoom_widget_on_frame_start(&this.Pitch.tMarker, 
                                    this.iRollScale, 
                                    PITCH_SCALE_MARKER_SCARE_RATIO);

}

static void __on_scene_flight_attitude_instrument_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_flight_attitude_instrument_t *ptThis = (user_scene_flight_attitude_instrument_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __before_scene_flight_attitude_instrument_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_flight_attitude_instrument_t *ptThis = (user_scene_flight_attitude_instrument_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_flight_attitude_instrument_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_flight_attitude_instrument_t *ptThis = (user_scene_flight_attitude_instrument_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/
        

        arm_2d_align_centre(__top_canvas, VISUAL_AREA_MASK.tRegion.tSize) {

            int16_t iVisualAreaWidth = VISUAL_AREA_MASK.tRegion.tSize.iWidth;

            arm_2d_fill_colour_with_mask(   
                                    ptTile, 
                                    &__centre_region,
                                    &VISUAL_AREA_MASK,
                                    (__arm_2d_color_t) {GLCD_COLOR_SKY_BLUE});


            /* draw land */
            spin_zoom_widget_show(  &this.Roll.tLand,
                                    ptTile,
                                    &__centre_region, 
                                    NULL, 
                                    255);

        #if ARM_2D_DEMO_FAI_SHOW_HORIZON
            /* draw horizon */
            spin_zoom_widget_show(  &this.Roll.tHorizon,
                                    ptTile,
                                    &__centre_region, 
                                    NULL, 
                                    255);
        #endif

            arm_2d_align_centre(__centre_region, PITCH_SCALE_MARKER_VISUAL_AREA_MASK.tRegion.tSize) {
                /* draw pitch marker */
                spin_zoom_widget_show(  &this.Pitch.tMarker,
                                        ptTile,
                                        &__centre_region, 
                                        NULL, 
                                        255);
            }

            arm_2d_align_centre(__centre_region, 
                                (iVisualAreaWidth >> 2)  + (iVisualAreaWidth >> 3),
                                3) {
                arm_2d_dock_left(__centre_region, (iVisualAreaWidth >> 3)) {
                    arm_2d_fill_colour(ptTile, &__left_region, GLCD_COLOR_GREEN);
                }

                arm_2d_dock_right(__centre_region, (iVisualAreaWidth >> 3)) {
                    arm_2d_fill_colour(ptTile, &__right_region, GLCD_COLOR_GREEN);
                }

                arm_2d_align_centre(__centre_region, 3, 3) {
                    arm_2d_fill_colour(ptTile, &__centre_region, GLCD_COLOR_RED);
                }
            }

            /* draw roll scale marker */
            spin_zoom_widget_show(  &this.Roll.tMarker,
                                    ptTile,
                                    &__centre_region, 
                                    NULL, 
                                    255);

            arm_2d_dock_with_margin(__centre_region, 20) {
                arm_2d_align_top_centre(__dock_region, 2, 16) {
                    arm_2d_fill_colour(ptTile, &__top_centre_region, GLCD_COLOR_RED);
                }
            }

            arm_2d_dock_with_margin(__centre_region, 40) {
                arm_2d_size_t tRollScaleLabelSize = ARM_2D_FONT_LiberationSansRegular14_A4.use_as__arm_2d_user_font_t.use_as__arm_2d_font_t.tCharSize;
                        tRollScaleLabelSize.iWidth *= 3;

                arm_2d_align_top_centre(__dock_region, tRollScaleLabelSize.iWidth + 4,
                                                       tRollScaleLabelSize.iHeight + 6) {
                        
                    //arm_2d_helper_draw_box(ptTile, &__top_centre_region, 1, GLCD_COLOR_GREEN, 255);
                        draw_round_corner_border(   ptTile, 
                                                    &__top_centre_region, 
                                                    GLCD_COLOR_WHITE, 
                                                    (arm_2d_border_opacity_t)
                                                        {128, 128, 128, 128},
                                                    (arm_2d_corner_opacity_t)
                                                        {128, 128, 128, 128});


                        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                        arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_LiberationSansRegular14_A4);
                        arm_lcd_text_set_draw_region(&__top_centre_region);
                        arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);

                        arm_lcd_printf_label(ARM_2D_ALIGN_TOP_CENTRE, "%"PRId16, -this.iRollScale / 10);

                }
                                        
                
            }
            
            
        }



        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene flight_attitude_instrument");

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_flight_attitude_instrument_t *__arm_2d_scene_flight_attitude_instrument_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_flight_attitude_instrument_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t __top_canvas
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    /* initialise dirty region 0 at runtime
     * this demo shows that we create a region in the centre of a screen(320*240)
     * for a image stored in the tile c_tileCMSISLogoMask
     */
    arm_2d_align_centre(__top_canvas, c_tileCMSISLogoMask.tRegion.tSize) {
        s_tDirtyRegions[0].tRegion = __centre_region;
    }

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].tRegion.tSize.iWidth 
                                                        = __top_canvas.tSize.iWidth;

    if (NULL == ptThis) {
        ptThis = (user_scene_flight_attitude_instrument_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_flight_attitude_instrument_t),
                                                        __alignof__(user_scene_flight_attitude_instrument_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_flight_attitude_instrument_t));

    *ptThis = (user_scene_flight_attitude_instrument_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_flight_attitude_instrument_load,
            .fnScene        = &__pfb_draw_scene_flight_attitude_instrument_handler,
            .fnAfterSwitch  = &__after_scene_flight_attitude_instrument_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene_flight_attitude_instrument_background_start,        /* deprecated */
            //.fnOnBGComplete = &__on_scene_flight_attitude_instrument_background_complete,     /* deprecated */
            .fnOnFrameStart = &__on_scene_flight_attitude_instrument_frame_start,
            .fnBeforeSwitchOut = &__before_scene_flight_attitude_instrument_switching_out,
            .fnOnFrameCPL   = &__on_scene_flight_attitude_instrument_frame_complete,
            .fnDepose       = &__on_scene_flight_attitude_instrument_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_flight_attitude_instrument_t begin ---------------*/

    

    /* initialize Land */
    do {
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
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR_WITH_TARGET_MASK,
            .Source = {
                .ptMask = &LAND_MASK,
                .tCentre = (arm_2d_location_t){
                    .iX = LAND_MASK.tRegion.tSize.iWidth >> 1,
                    .iY = 0, 
                },
                .tColourToFill = __RGB(111, 78, 55),
            },
            .Target.ptMask = &VISUAL_AREA_MASK,

            .ptScene = (arm_2d_scene_t *)ptThis,

        };
        spin_zoom_widget_init(&this.Roll.tLand, &tCFG);

        float fPitchHeight = ((float)VISUAL_AREA_MASK.tRegion.tSize.iHeight / LAND_MASK_SCARE_RATIO) / 2.0f;

        this.Roll.q16PitchRatio = reinterpret_q16_f32(fPitchHeight / 900.0f);

        /* update reference points*/
        do {
            s_tReferencePoints[0].iX = 0;
            s_tReferencePoints[0].iY = 0;

            s_tReferencePoints[1].iX = LAND_MASK.tRegion.tSize.iWidth - 1;
            s_tReferencePoints[1].iY = 0;

            s_tReferencePoints[2].iX = 0;
            s_tReferencePoints[2].iY = 3;

            s_tReferencePoints[3].iX = LAND_MASK.tRegion.tSize.iWidth - 1;
            s_tReferencePoints[3].iY = 3;
        } while(0);
    } while(0);

#if ARM_2D_DEMO_FAI_SHOW_HORIZON
    /* initialize horizon */
    do {
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
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR_WITH_TARGET_MASK,
            .Source = {
                .ptMask = &HORIZON_MASK,
                .tCentre = (arm_2d_location_t){
                    .iX = HORIZON_MASK.tRegion.tSize.iWidth >> 1,
                    .iY = HORIZON_MASK.tRegion.tSize.iHeight >> 1, 
                },
                .tColourToFill = GLCD_COLOR_WHITE,
            },
            .Target.ptMask = &VISUAL_AREA_MASK,

            //.ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.Roll.tHorizon, &tCFG);
    } while(0);
#endif

    /* initialize roll marker */
    do {
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
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
            .Source = {
                .ptMask = &ROLL_SCALE_MARKER_MASK,
                .tCentreFloat = {
                    .fX = ROLL_SCALE_MARKER_MASK.tRegion.tSize.iWidth / 2.0f,
                    .fY = ROLL_SCALE_MARKER_MASK.tRegion.tSize.iWidth / 1.414f
                },
                .tColourToFill = GLCD_COLOR_WHITE,
            },
            .bUseFloatPointInCentre = true,

            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.Roll.tMarker, &tCFG);

        /* update reference points*/
        do {
            s_tRollScaleMarkerReferencePoints[0].iX = ROLL_SCALE_MARKER_MASK.tRegion.tSize.iWidth >> 1;
            s_tRollScaleMarkerReferencePoints[0].iY = 0;

            s_tRollScaleMarkerReferencePoints[1].iX = 0;
            s_tRollScaleMarkerReferencePoints[1].iY = ROLL_SCALE_MARKER_MASK.tRegion.tSize.iHeight >> 1;

            s_tRollScaleMarkerReferencePoints[2].iX = ROLL_SCALE_MARKER_MASK.tRegion.tSize.iWidth - 1;
            s_tRollScaleMarkerReferencePoints[2].iY = ROLL_SCALE_MARKER_MASK.tRegion.tSize.iHeight >> 1;

            s_tRollScaleMarkerReferencePoints[3].iX = 0;
            s_tRollScaleMarkerReferencePoints[3].iY = ROLL_SCALE_MARKER_MASK.tRegion.tSize.iHeight - 1;

            s_tRollScaleMarkerReferencePoints[4].iX = ROLL_SCALE_MARKER_MASK.tRegion.tSize.iWidth - 1;
            s_tRollScaleMarkerReferencePoints[4].iY = ROLL_SCALE_MARKER_MASK.tRegion.tSize.iHeight - 1;
        } while(0);

    } while(0);


    /* initialize Pitch Scale Marker */
    do {
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
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR_WITH_TARGET_MASK,
            .Source = {
                .ptMask = &PITCH_SCALE_MARKER_MASK,
                .tCentre = (arm_2d_location_t){
                    .iX = PITCH_SCALE_MARKER_MASK.tRegion.tSize.iWidth >> 1,
                    .iY = PITCH_SCALE_MARKER_MASK.tRegion.tSize.iHeight >> 1, 
                },
                .tColourToFill = GLCD_COLOR_WHITE,
            },
            .Target.ptMask = &PITCH_SCALE_MARKER_VISUAL_AREA_MASK,

            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.Pitch.tMarker, &tCFG);

        float fPitchHeight = ((float)PITCH_SCALE_MARKER_MASK.tRegion.tSize.iHeight / PITCH_SCALE_MARKER_SCARE_RATIO) / 2.0f;

        this.Pitch.q16PitchRatio = reinterpret_q16_f32(fPitchHeight / 550.0f);
    } while(0);
    /* ------------   initialize members of user_scene_flight_attitude_instrument_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


