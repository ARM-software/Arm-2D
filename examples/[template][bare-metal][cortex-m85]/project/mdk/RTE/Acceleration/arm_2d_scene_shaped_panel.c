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

#define __USER_SCENE_SHAPED_PANEL_IMPLEMENT__
#include "arm_2d_scene_shaped_panel.h"

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

#define PANEL_SHAPE_MASK            c_tileSolidCircleSmallMask

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern const arm_2d_tile_t c_tileSolidCircleSmallMask;
extern const arm_2d_tile_t c_tileRoundedSquareMask;
extern const arm_2d_tile_t c_tilePointerSecMask;
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

enum {
    API_FILL_COLOUR,
    API_TILE_COPY,
};

static const
struct {
    uint8_t u4CopyMode          : 4;
    uint8_t u1APISelect         : 1;
} c_tModes[] = {
    {ARM_2D_CP_MODE_NO_MIRROR,  API_FILL_COLOUR},
    {ARM_2D_CP_MODE_X_MIRROR,   API_FILL_COLOUR},
    {ARM_2D_CP_MODE_Y_MIRROR,   API_FILL_COLOUR},
    {ARM_2D_CP_MODE_XY_MIRROR,  API_FILL_COLOUR},

    {ARM_2D_CP_MODE_NO_MIRROR,  API_TILE_COPY},
    {ARM_2D_CP_MODE_X_MIRROR,   API_TILE_COPY},
    {ARM_2D_CP_MODE_Y_MIRROR,   API_TILE_COPY},
    {ARM_2D_CP_MODE_XY_MIRROR,  API_TILE_COPY},
};

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_shaped_panel_load(arm_2d_scene_t *ptScene)
{
    user_scene_shaped_panel_t *ptThis = (user_scene_shaped_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    spin_zoom_widget_on_load(&this.tTransform);

}

static void __after_scene_shaped_panel_switching(arm_2d_scene_t *ptScene)
{
    user_scene_shaped_panel_t *ptThis = (user_scene_shaped_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_shaped_panel_depose(arm_2d_scene_t *ptScene)
{
    user_scene_shaped_panel_t *ptThis = (user_scene_shaped_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*--------------------- insert your depose code begin --------------------*/
    spin_zoom_widget_depose(&this.tTransform);

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
 * Scene shaped_panel                                                                    *
 *----------------------------------------------------------------------------*/
#if 0  /* deprecated */
static void __on_scene_shaped_panel_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_shaped_panel_t *ptThis = (user_scene_shaped_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_shaped_panel_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_shaped_panel_t *ptThis = (user_scene_shaped_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}
#endif


static void __on_scene_shaped_panel_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_shaped_panel_t *ptThis = (user_scene_shaped_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /* vibration */
    do {
        int32_t nResult;
        int16_t iWidth = (PANEL_SHAPE_MASK.tRegion.tSize.iWidth >> 1);
        int16_t iHeight = (PANEL_SHAPE_MASK.tRegion.tSize.iHeight >> 1);

        /* calculate core vibration */
        arm_2d_helper_time_cos_slider(-iWidth, iWidth, 2500, ARM_2D_ANGLE(0.0f), &nResult, &this.lTimestamp[0]);
        this.tOffset.iX = nResult;
        
        if (arm_2d_helper_time_cos_slider(-iHeight, iHeight, 5000, ARM_2D_ANGLE(30.0f), &nResult, &this.lTimestamp[1])) {
            this.chTestIndex++;
            if (this.chTestIndex >= dimof(c_tModes)) {
                this.chTestIndex = 0;
            }
        }
        this.tOffset.iY = nResult;
    } while(0);

    /* calculate the Seconds */
    do {
        int64_t lTimeStampInMs = arm_2d_helper_convert_ticks_to_ms(
                                arm_2d_helper_get_system_timestamp());

        lTimeStampInMs %= (5ul * 1000ul); 
        spin_zoom_widget_on_frame_start(&this.tTransform, lTimeStampInMs, 0.9f);
    } while(0);
}

static void __on_scene_shaped_panel_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_shaped_panel_t *ptThis = (user_scene_shaped_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __before_scene_shaped_panel_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_shaped_panel_t *ptThis = (user_scene_shaped_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_shaped_panel_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_shaped_panel_t *ptThis = (user_scene_shaped_panel_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/

        arm_2d_align_centre(__top_canvas, PANEL_SHAPE_MASK.tRegion.tSize) {

            arm_2d_container(ptTile, __shaped_panel, &__centre_region) {
        
                /* draw the shaped panel */
                arm_2d_fill_colour_with_mask(ptTile,
                            &__centre_region,
                            &PANEL_SHAPE_MASK,
                            (__arm_2d_color_t){GLCD_COLOR_ORANGE});

                arm_2d_align_centre_open(   __shaped_panel_canvas, 
                                            c_tileCMSISLogoMask.tRegion.tSize) {
                    
                    arm_2d_region_t tLogoRegion = __centre_region;

                    tLogoRegion.tLocation.iX += this.tOffset.iX;
                    tLogoRegion.tLocation.iY += this.tOffset.iY;

                    __arm_2d_hint_optimize_for_pfb__(tLogoRegion) {

                        /* update dirty region */
                        arm_2d_helper_dirty_region_update_item( 
                            &this.use_as__arm_2d_scene_t
                                    .tDirtyRegionHelper
                                        .tDefaultItem,
                            (arm_2d_tile_t *)&__shaped_panel,
                            NULL,
                            &tLogoRegion);

                        switch (c_tModes[this.chTestIndex].u1APISelect) {
                            default:
                            case API_FILL_COLOUR:
                                arm_2d_fill_colour_with_masks_and_opacity(
                                    &__shaped_panel,
                                    &tLogoRegion,
                                    &c_tileCMSISLogoMask,
                                    &PANEL_SHAPE_MASK,
                                    (__arm_2d_color_t){GLCD_COLOR_WHITE},
                                    255,
                                    c_tModes[this.chTestIndex].u4CopyMode);
                                break;
                            case API_TILE_COPY:
                                arm_2d_tile_copy_with_masks(
                                    &c_tileCMSISLogo,
                                    &c_tileCMSISLogoMask,
                                    &__shaped_panel,
                                    &PANEL_SHAPE_MASK,
                                    &tLogoRegion,
                                    c_tModes[this.chTestIndex].u4CopyMode);
                                break;
                        }
                    }
                }

                arm_2d_align_bottom_right_open(   __shaped_panel_canvas,
                                            __shaped_panel_canvas.tSize.iWidth >> 2,
                                            __shaped_panel_canvas.tSize.iHeight >> 2) {

                    spin_zoom_widget_show_with_normal_pivot(
                        &this.tTransform,
                        &__shaped_panel,
                        NULL,
                        &__bottom_right_region.tLocation,
                        255);
                }


            }
        }

        /* draw text at the top-left corner */

    #if 0
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene shaped_panel");
    #endif

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_shaped_panel_t *__arm_2d_scene_shaped_panel_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_shaped_panel_t *ptThis)
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
        ptThis = (user_scene_shaped_panel_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_shaped_panel_t),
                                                        __alignof__(user_scene_shaped_panel_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_shaped_panel_t));

    *ptThis = (user_scene_shaped_panel_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_shaped_panel_load,
            .fnScene        = &__pfb_draw_scene_shaped_panel_handler,
            .fnAfterSwitch  = &__after_scene_shaped_panel_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene_shaped_panel_background_start,        /* deprecated */
            //.fnOnBGComplete = &__on_scene_shaped_panel_background_complete,     /* deprecated */
            .fnOnFrameStart = &__on_scene_shaped_panel_frame_start,
            .fnBeforeSwitchOut = &__before_scene_shaped_panel_switching_out,
            .fnOnFrameCPL   = &__on_scene_shaped_panel_frame_complete,
            .fnDepose       = &__on_scene_shaped_panel_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_shaped_panel_t begin ---------------*/
    // initialize second pointer
    do {
        spin_zoom_widget_cfg_t tCFG = {
            .Indicator = {
                .LowerLimit = {
                    .fAngleInDegree = 0.0f,
                    .nValue = 0,
                },
                .UpperLimit = {
                    .fAngleInDegree = 360.0f,
                    .nValue = 5*1000ul,
                },
                .Step = {
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR_WITH_TARGET_MASK,
            .Source = {
                .ptMask = &c_tilePointerSecMask,
                .tCentre = (arm_2d_location_t) {
                    .iX = (c_tilePointerSecMask.tRegion.tSize.iWidth >> 1),
                    .iY = 100,
                },
                .tColourToFill = GLCD_COLOR_RED,
            },
            .Target.ptMask = &PANEL_SHAPE_MASK,

            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tTransform, &tCFG);
    } while(0);

    /* ------------   initialize members of user_scene_shaped_panel_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


