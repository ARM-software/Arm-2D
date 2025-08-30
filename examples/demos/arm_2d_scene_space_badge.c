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

#define __USER_SCENE_SPACE_BADGE_IMPLEMENT__
#include "arm_2d_scene_space_badge.h"

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
#   define c_tileSpaceFleet         c_tileSpaceFleetGRAY8
#   define c_tileDoge               c_tileDogeGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565
#   define c_tileSpaceFleet         c_tileSpaceFleetRGB565
#   define c_tileDoge               c_tileDogeRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#   define c_tileSpaceFleet         c_tileSpaceFleetCCCA8888
#   define c_tileDoge               c_tileDogeCCCN888
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

extern const arm_2d_tile_t c_tileSpaceFleetMask;
extern const arm_2d_tile_t c_tileSpaceFleet;
extern const arm_2d_tile_t c_tileDoge;
extern const arm_2d_tile_t c_tileDogeGRAY8;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_LiberationSansRegular32_A4;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_LiberationSansRegular32_A2;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_LiberationSansRegular14_A2;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_LiberationSansRegular14_A4;


/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static arm_2d_size_t s_tPhotoSize = { 10 * 7, 10 * 9 };

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_space_badge_load(arm_2d_scene_t *ptScene)
{
    user_scene_space_badge_t *ptThis = (user_scene_space_badge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if !SPACE_BADGE_90TH_BATTLE_STYLE
    do {
        arm_2d_size_t tBattleZone = c_tileSpaceFleet.tRegion.tSize;

        srand(arm_2d_helper_get_system_timestamp());
        arm_foreach(__space_badge_explosion_halo_t, this.tHalos, ptHalo) {

            ptHalo->tPivot.iX = rand() % tBattleZone.iWidth;
            ptHalo->tPivot.iY = rand() % tBattleZone.iHeight;
            ptHalo->iRadius = rand() & 0x07;
            ptHalo->chOpacity = rand() & 0xFF;

        }
    } while(0);
#endif

    crt_screen_on_load(&this.tCRTScreen);
}

static void __after_scene_space_badge_switching(arm_2d_scene_t *ptScene)
{
    user_scene_space_badge_t *ptThis = (user_scene_space_badge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_space_badge_depose(arm_2d_scene_t *ptScene)
{
    user_scene_space_badge_t *ptThis = (user_scene_space_badge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*--------------------- insert your depose code begin --------------------*/
    /* draw line */
    do {
        arm_foreach(arm_2d_user_draw_line_descriptor_t, this.tDrawLineOP, ptLineOP) {
            ARM_2D_OP_DEPOSE(*ptLineOP);
        }
    } while(0);

    crt_screen_depose(&this.tCRTScreen);

#if SPACE_BADGE_SHOW_NEBULA
    dynamic_nebula_depose(&this.tNebula);
#endif

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
 * Scene space_badge                                                                    *
 *----------------------------------------------------------------------------*/
#if 0  /* deprecated */
static void __on_scene_space_badge_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_space_badge_t *ptThis = (user_scene_space_badge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_space_badge_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_space_badge_t *ptThis = (user_scene_space_badge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}
#endif

static void __on_scene_space_badge_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_space_badge_t *ptThis = (user_scene_space_badge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    crt_screen_on_frame_start(&this.tCRTScreen);

    if (arm_2d_helper_is_time_out(30, &this.lTimestamp[0])) {

        if (this.iStartOffset <= 0) {
            this.iStartOffset = 100;
        }
        this.iStartOffset -= 8;

        arm_2d_size_t tBattleZone = c_tileSpaceFleet.tRegion.tSize;

        srand(arm_2d_helper_get_system_timestamp());
        arm_foreach(__space_badge_explosion_halo_t, this.tHalos, ptHalo) {

            if (ptHalo->chOpacity == 0) {

                ptHalo->tPivot.iX = rand() % tBattleZone.iWidth;
                ptHalo->tPivot.iY = rand() % tBattleZone.iHeight;
                ptHalo->iRadius = rand() & 0x07;
                ptHalo->chOpacity = 255;

            } else if (ptHalo->chOpacity > SPACE_BADGE_EXPLOSION_SPEED) {
                ptHalo->chOpacity -= SPACE_BADGE_EXPLOSION_SPEED;
            } else {
                ptHalo->chOpacity = 0;
            }
        }
    }
}

static void __on_scene_space_badge_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_space_badge_t *ptThis = (user_scene_space_badge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    crt_screen_on_frame_complete(&this.tCRTScreen);
#if 0
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(3000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene_space_badge_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_space_badge_t *ptThis = (user_scene_space_badge_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_space_badge_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_space_badge_t *ptThis = (user_scene_space_badge_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/

        arm_2d_location_t tStartPoint = {
            .iY = __top_canvas.tSize.iHeight - 1,
            .iX = (__top_canvas.tSize.iWidth >> 1) - 200 * 8,
        };
        arm_2d_location_t tStopPoint = {
            .iY = __top_canvas.tSize.iHeight >> 1,
            .iX = __top_canvas.tSize.iWidth >> 1,
        };

        for (int n = 0; n < 16; n++) {

            /* draw line*/
            do {
                arm_2d_user_draw_line_api_params_t tParam = {
                    .tStart = tStartPoint,
                    .tEnd = tStopPoint,
                };

                arm_2dp_rgb565_user_draw_line(
                                &this.tDrawLineOP[n],
                                ptTile,
                                &__top_canvas,
                                &tParam,
                                (arm_2d_color_rgb565_t){GLCD_COLOR_GREEN},
                                255);
            } while(0);
            ARM_2D_OP_WAIT_ASYNC(&this.tDrawLineOP[n]);


            tStartPoint.iX += 200;
        }

        /* draw horizontal line */
        int32_t nCellLength = 100;
        int32_t nOffset = this.iStartOffset;
        int32_t nObserverHeight = __top_canvas.tSize.iHeight >> 1;

        arm_2d_region_t tHorizontalLine = {
            .tSize = {
                .iWidth = __top_canvas.tSize.iWidth,
                .iHeight = 1,
            }, 
        };

        int16_t nY = 0;
        for (;;) {
            int32_t nDistanceFromObserver = nOffset + 100;
            int32_t nHeightOnScreen = nObserverHeight * nOffset / nDistanceFromObserver;

            nY = __top_canvas.tSize.iHeight - nHeightOnScreen - 1;
            if (tHorizontalLine.tLocation.iY == nY) {
                break;
            }
            tHorizontalLine.tLocation.iY = nY;

            arm_2d_fill_colour(ptTile, &tHorizontalLine, GLCD_COLOR_GREEN);

            ARM_2D_OP_WAIT_ASYNC();

            nOffset += nCellLength;
        }

        arm_2d_dock_bottom(__top_canvas, (__top_canvas.tSize.iHeight >> 1) + 4) {

            arm_2d_dock_top(__bottom_region, 100) {
                arm_2d_fill_colour_with_vertical_alpha_gradient(
                            ptTile,
                            &__top_region, 
                            (__arm_2d_color_t){GLCD_COLOR_BLACK}, 
                            (arm_2d_alpha_samples_2pts_t){255, 0});
                ARM_2D_OP_WAIT_ASYNC();
            }
        }

        /* draw space fleet */
        arm_2d_dock_top_open(__top_canvas, __top_canvas.tSize.iHeight >> 1) {
            arm_2d_align_bottom_centre_open(__top_region, c_tileSpaceFleetMask.tRegion.tSize) {

                arm_2d_tile_copy_with_src_mask_and_opacity_only(    &c_tileSpaceFleet,
                                                                    &c_tileSpaceFleetMask, 
                                                                    ptTile, 
                                                                    &__bottom_centre_region, 
                                                                    255 - 32);

                arm_foreach(__space_badge_explosion_halo_t, this.tHalos, ptHalo) {
                    /* draw circle */

                    arm_2d_location_t tPivot = ptHalo->tPivot;
                    tPivot.iX += __bottom_centre_region.tLocation.iX;
                    tPivot.iY += __bottom_centre_region.tLocation.iY;


                    arm_2d_user_draw_circle_api_params_t tParam = {
                        .iRadius = ptHalo->iRadius,
                        .bAntiAlias = true,
                        .ptPivot = &tPivot,
                    };

                    arm_2dp_rgb565_user_draw_circle(
                                    NULL,
                                    ptTile,
                                    &__top_canvas,
                                    &tParam,
                                    (arm_2d_color_rgb565_t){__RGB( 255, 200, 0)},
                                    ptHalo->chOpacity);
                    
                    ARM_2D_OP_WAIT_ASYNC();

                }
            }
        }


        arm_2d_layout(__top_canvas, RIGHT_TO_LEFT) {

            __item_line_dock_horizontal(s_tPhotoSize.iWidth + 8, 16, 32, 0, 0) {

                arm_2d_align_centre(__item_region, s_tPhotoSize) {

                    crt_screen_show(&this.tCRTScreen, ptTile, &__centre_region, 128 + 64, bIsNewFrame);
                    
                }

                arm_2d_align_centre(__item_region, s_tPhotoSize.iWidth + 8, s_tPhotoSize.iHeight + 8) {
                    arm_2d_helper_draw_box(ptTile, &__centre_region, 1, GLCD_COLOR_GREEN, 64);
                }

            }

        #if SPACE_BADGE_SHOW_NEBULA
            /* show nebula */
            dynamic_nebula_show(&this.tNebula, 
                                ptTile, 
                                &__top_canvas, 
                                GLCD_COLOR_WHITE, 
                                255,
                                bIsNewFrame);
        #endif

            __item_line_dock_horizontal() {
                arm_2d_dock_vertical(__item_region, s_tPhotoSize.iHeight + 14, 32, 0) {

                    arm_2d_layout(__vertical_region) {
                        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);

                        arm_lcd_text_set_scale(0.7f);
                        arm_2d_size_t tNameStringSize = 
                            arm_lcd_printf_to_buffer(
                                (const arm_2d_font_t *)&ARM_2D_FONT_LiberationSansRegular32_A2, "Kabosu Doge");

                        __item_line_dock_vertical(tNameStringSize.iHeight) {

                            arm_lcd_text_set_draw_region(&__item_region);
                            arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);
                            arm_lcd_text_location(0,0);
                            arm_lcd_text_set_opacity(255);
                            arm_lcd_printf_buffer(0);
                        }

                        __item_line_dock_vertical(0, 0, 20, 0) {
                             arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_LiberationSansRegular14_A2);
                            arm_lcd_text_set_scale(1.0f);
                            arm_lcd_text_set_draw_region(&__item_region);
                            arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);
                            arm_lcd_text_location(0,0);

                            arm_lcd_puts("Commander of\r\nthe Woofer Fleet");


                        }
                    }

                #if 0
                    draw_round_corner_border(   ptTile, 
                                                &__vertical_region, 
                                                GLCD_COLOR_YELLOW, 
                                                (arm_2d_border_opacity_t)
                                                    {64, 64, 64, 64},
                                                (arm_2d_corner_opacity_t)
                                                    {64, 64, 64, 64});
                #endif

                }

                
            }

        }

        /* draw text at the top-left corner */
        arm_lcd_text_set_scale(0.0f);
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene space_badge");

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_space_badge_t *__arm_2d_scene_space_badge_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_space_badge_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    /* get the screen region */
    arm_2d_region_t __top_canvas
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    ARM_2D_UNUSED(__top_canvas);

    if (NULL == ptThis) {
        ptThis = (user_scene_space_badge_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_space_badge_t),
                                                        __alignof__(user_scene_space_badge_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_space_badge_t));

    *ptThis = (user_scene_space_badge_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_space_badge_load,
            .fnScene        = &__pfb_draw_scene_space_badge_handler,
            .fnAfterSwitch  = &__after_scene_space_badge_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene_space_badge_background_start,        /* deprecated */
            //.fnOnBGComplete = &__on_scene_space_badge_background_complete,     /* deprecated */
            .fnOnFrameStart = &__on_scene_space_badge_frame_start,
            .fnBeforeSwitchOut = &__before_scene_space_badge_switching_out,
            .fnOnFrameCPL   = &__on_scene_space_badge_frame_complete,
            .fnDepose       = &__on_scene_space_badge_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_space_badge_t begin ---------------*/
    /* draw line */
    do {
        arm_foreach(arm_2d_user_draw_line_descriptor_t, this.tDrawLineOP, ptLineOP) {
            ARM_2D_OP_INIT(*ptLineOP);
        }
    } while(0);

    /* CRT Screen */
    do {
        crt_screen_cfg_t tCFG = {
            .ptScene = &this.use_as__arm_2d_scene_t,
            //.ptilePhoto = &c_tileDogeGRAY8,
            .ptilePhoto = &c_tileDoge,
            .tScreenColour.tColour = GLCD_COLOR_GREEN,
            .tScanBarColour.tColour = GLCD_COLOR_WHITE,
            .chWhiteNoiseRatio = 32,
            .chNoiseLasts = 32,
            .bStrongNoise = true,
            .bShowScanningEffect = true,
        };

        crt_screen_init(&this.tCRTScreen, &tCFG);
    } while(0);

#if SPACE_BADGE_SHOW_NEBULA
    do {
        int16_t iRadius = MIN(__top_canvas.tSize.iHeight, __top_canvas.tSize.iWidth) >> 1;
        dynamic_nebula_cfg_t tCFG = {
            .fSpeed = SPACE_BADGE_NEBULA_FLYING_SPEED,
            .iRadius = iRadius,
            .iVisibleRingWidth = 80,
            .hwParticleCount = dimof(this.tParticles),
            .ptParticles = this.tParticles,
            .bMovingOutward = true,
            .u8FadeOutEdgeWidth = 16,
        };
        dynamic_nebula_init(& this.tNebula, &tCFG);
    } while(0);
#endif

    /* ------------   initialize members of user_scene_space_badge_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


