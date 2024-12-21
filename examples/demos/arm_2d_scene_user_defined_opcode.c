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

#define __USER_SCENE_USER_DEFINED_OPCODE_IMPLEMENT__
#include "arm_2d_scene_user_defined_opcode.h"

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
    BATTLESHIP_START,
    BATTLESHIP_HYPER_JUMP,
    BATTLESHIP_BATTLE,
    BATTLESHIP_EXPLOSION,
    BATTLESHIP_VANISH,
    BATTLESHIP_IDLE,
};


/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileBattleshipA4Mask;
extern const arm_2d_tile_t c_tileBattleshipMask;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

    
    /* add the last region:
        * it is the top left corner for text display 
        */
    ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
        0,
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_user_defined_opcode_load(arm_2d_scene_t *ptScene)
{
    user_scene_user_defined_opcode_t *ptThis = (user_scene_user_defined_opcode_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __after_scene_user_defined_opcode_switching(arm_2d_scene_t *ptScene)
{
    user_scene_user_defined_opcode_t *ptThis = (user_scene_user_defined_opcode_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_user_defined_opcode_depose(arm_2d_scene_t *ptScene)
{
    user_scene_user_defined_opcode_t *ptThis = (user_scene_user_defined_opcode_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    /* draw line */
    do {
        arm_foreach(arm_2d_user_draw_line_descriptor_t, this.tDrawLineOP, ptLineOP) {
            ARM_2D_OP_DEPOSE(*ptLineOP);
        }
    } while(0);

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene user_defined_opcode                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_user_defined_opcode_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_user_defined_opcode_t *ptThis = (user_scene_user_defined_opcode_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_user_defined_opcode_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_user_defined_opcode_t *ptThis = (user_scene_user_defined_opcode_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_user_defined_opcode_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_user_defined_opcode_t *ptThis = (user_scene_user_defined_opcode_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    if (this.iStartOffset == 0) {
        this.iStartOffset = 100;
    }
    this.iStartOffset -= 4;

    arm_2d_size_t tBattleZone = c_tileBattleshipA4Mask.tRegion.tSize;

    switch(this.chBattleshipState) {

        case BATTLESHIP_START:
            memset(this.tHalos, 0, sizeof(this.tHalos));
            this.chBattleshipState++;
            this.chHyperJumpOpacity = 255;
            this.lTimestamp[0] = 0;
            //fall-through
        
        case BATTLESHIP_HYPER_JUMP: {
                int32_t nResult;
                if (arm_2d_helper_time_half_cos_slider(500, 0, 4000, &nResult, &this.lTimestamp[0])) {
                    this.chBattleshipState = BATTLESHIP_BATTLE;
                    this.lTimestamp[0] = 0;
                }
                this.chHyperJumpOpacity = MIN(255, nResult);
            }
            break;
        
        case BATTLESHIP_BATTLE:
            srand(arm_2d_helper_get_system_timestamp());
            arm_foreach(__explosion_halo_t, this.tHalos, ptHalo) {

                if (ptHalo->chOpacity == 0) {

                    ptHalo->tPivot.iX = rand() % tBattleZone.iWidth;
                    ptHalo->tPivot.iY = rand() % tBattleZone.iHeight;
                    ptHalo->iRadius = rand() & (_BV(4) - 1);
                    ptHalo->chOpacity = 255;

                } else if (ptHalo->chOpacity > 8) {
                    ptHalo->chOpacity -= 8;
                } else {
                    ptHalo->chOpacity = 0;
                }
            }
            if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
                this.lTimestamp[0] = 0;
                this.chBattleshipState = BATTLESHIP_EXPLOSION;
                this.iExplosionRadius = 0;
            }
            break;
        
        case BATTLESHIP_EXPLOSION:
            {
                int32_t nResult;
                if (arm_2d_helper_time_half_cos_slider(0, 80, 8000, &nResult, &this.lTimestamp[0])) {
                    this.chBattleshipState = BATTLESHIP_VANISH;
                    this.lTimestamp[0] = 0;
                }
                this.iExplosionRadius = nResult;

                this.tExplosion.tColour=  arm_2d_pixel_from_brga8888( 
                                            __arm_2d_helper_colour_slider(
                                                __RGB32(0xFF, 0xFF, 0xFF), 
                                                __RGB32(0xFF, 0xFF, 0),
                                                80,
                                                nResult));

            }

            srand(arm_2d_helper_get_system_timestamp());
            arm_foreach(__explosion_halo_t, this.tHalos, ptHalo) {

                if (ptHalo->chOpacity == 0) {

                    ptHalo->tPivot.iX = rand() % tBattleZone.iWidth;
                    ptHalo->tPivot.iY = rand() % tBattleZone.iHeight;
                    ptHalo->iRadius = rand() & (_BV(4) - 1);
                    ptHalo->chOpacity = 255;

                } else if (ptHalo->chOpacity > 8) {
                    ptHalo->chOpacity -= 8;
                } else {
                    ptHalo->chOpacity = 0;
                }
            }
            break;
            
        case BATTLESHIP_VANISH:
            {
                int32_t nResult;
                if (arm_2d_helper_time_half_cos_slider(80, 0, 500, &nResult, &this.lTimestamp[0])) {
                    this.chBattleshipState = BATTLESHIP_IDLE;
                    this.lTimestamp[0] = 0;
                }
                this.iExplosionRadius = nResult;
            }
            break;
        
        case BATTLESHIP_IDLE:
            if (arm_2d_helper_is_time_out(5000, &this.lTimestamp[0])) {
                this.chBattleshipState = BATTLESHIP_START;
            }
            break;

    }
}

static void __on_scene_user_defined_opcode_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_user_defined_opcode_t *ptThis = (user_scene_user_defined_opcode_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if 0
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(3000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene_user_defined_opcode_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_user_defined_opcode_t *ptThis = (user_scene_user_defined_opcode_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_user_defined_opcode_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_user_defined_opcode_t *ptThis = (user_scene_user_defined_opcode_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/

        arm_2d_location_t tStartPoint = {
            .iY = tScreenSize.iHeight - 1,
            .iX = (tScreenSize.iWidth >> 1) - 200 * 8,
        };
        arm_2d_location_t tStopPoint = {
            .iY = tScreenSize.iHeight >> 1,
            .iX = tScreenSize.iWidth >> 1,
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
        int32_t nObserverHeight = tScreenSize.iHeight >> 1;

        arm_2d_region_t tHorizontalLine = {
            .tSize = {
                .iWidth = tScreenSize.iWidth,
                .iHeight = 1,
            }, 
        };

        int16_t nY = 0;
        for (;;) {
            int32_t nDistanceFromObserver = nOffset + 100;
            int32_t nHeightOnScreen = nObserverHeight * nOffset / nDistanceFromObserver;

            nY = tScreenSize.iHeight - nHeightOnScreen - 1;
            if (tHorizontalLine.tLocation.iY == nY) {
                break;
            }
            tHorizontalLine.tLocation.iY = nY;

            arm_2d_fill_colour(ptTile, &tHorizontalLine, GLCD_COLOR_GREEN);

            ARM_2D_OP_WAIT_ASYNC();

            nOffset += nCellLength;
        }

        arm_2d_dock_bottom(__top_canvas, (tScreenSize.iHeight >> 1)) {

            arm_2d_dock_top(__bottom_region, 100) {
                arm_2d_fill_colour_with_vertical_alpha_gradient(
                            ptTile,
                            &__top_region, 
                            (__arm_2d_color_t){GLCD_COLOR_BLACK}, 
                            (arm_2d_alpha_samples_2pts_t){255, 0});
            }
        }

    #if 0
        arm_2d_align_centre(__top_canvas, 100, 100) {

            /* draw circle */
            do {
                arm_2d_user_draw_circle_api_params_t tParam = {
                    .iRadius = 25,
                    .bAntiAlias = true,
                };

                arm_2dp_rgb565_user_draw_circle(
                                NULL,
                                ptTile,
                                &__centre_region,
                                &tParam,
                                (arm_2d_color_rgb565_t){__RGB( 255, 200, 0)},
                                128);
                
                ARM_2D_OP_WAIT_ASYNC();
            } while(0);
        }
    #endif

        arm_2d_dock_top(__top_canvas, (__top_canvas.tSize.iHeight >> 1)) {

            arm_2d_dock_bottom(__top_region, 60, 50, 0) {

                arm_2d_align_centre(__bottom_region, c_tileBattleshipA4Mask.tRegion.tSize) {

                    if (BATTLESHIP_VANISH !=  this.chBattleshipState && this.chBattleshipState != BATTLESHIP_IDLE) {

                        arm_2d_fill_colour_with_a4_mask_and_opacity(ptTile,
                                                                    &__centre_region,
                                                                    &c_tileBattleshipA4Mask,
                                                                    (__arm_2d_color_t){ __RGB(0, 64, 0)},
                                                                    255);
                    }


                    if (BATTLESHIP_HYPER_JUMP ==  this.chBattleshipState) {

                        arm_2d_fill_colour_with_a4_mask_and_opacity(ptTile,
                                                                &__centre_region,
                                                                &c_tileBattleshipA4Mask,
                                                                (__arm_2d_color_t){ GLCD_COLOR_WHITE},
                                                                this.chHyperJumpOpacity);

                    } else if (BATTLESHIP_BATTLE == this.chBattleshipState 
                            || BATTLESHIP_EXPLOSION == this.chBattleshipState) {
                        arm_foreach(__explosion_halo_t, this.tHalos, ptHalo) {
                            /* draw circle */

                            arm_2d_location_t tPivot = ptHalo->tPivot;
                            tPivot.iX += __centre_region.tLocation.iX;
                            tPivot.iY += __centre_region.tLocation.iY;


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

                    if ((BATTLESHIP_EXPLOSION == this.chBattleshipState)
                    ||  (BATTLESHIP_VANISH == this.chBattleshipState)) {

                        arm_2d_location_t tPivot = {
                            .iX = (__centre_region.tSize.iWidth >> 1) + __centre_region.tLocation.iX,
                            .iY = (__centre_region.tSize.iHeight >> 1) + __centre_region.tLocation.iY,
                        };

                        arm_2d_user_draw_circle_api_params_t tParam = {
                            .iRadius = this.iExplosionRadius,
                            .bAntiAlias = true,
                            .ptPivot = &tPivot,
                        };

                        arm_2dp_rgb565_user_draw_circle(
                                        NULL,
                                        ptTile,
                                        &__top_canvas,
                                        &tParam,
                                        (arm_2d_color_rgb565_t){this.tExplosion.tColour},
                                        255);

                    }

                }
            }

        }



    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_user_defined_opcode_t *__arm_2d_scene_user_defined_opcode_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_user_defined_opcode_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

#if __GLCD_CFG_COLOUR_DEPTH__ != 16
    /* this demo only works for rgb565*/

    ARM_2D_LOG_ERROR(
        APP, 
        0, 
        "Demo", 
        "The User Defined Opcode Demo currently only works for RGB565, i.e. __GLCD_CFG_COLOUR_DEPTH__ equals 16"
    );

    return NULL;
#endif

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    /* initialise dirty region 0 at runtime
     * this demo shows that we create a region in the centre of a screen(320*240)
     * for a image stored in the tile c_tileCMSISLogoMask
     */
    arm_2d_dock_bottom(tScreen, tScreen.tSize.iHeight >> 1) {
        s_tDirtyRegions[0].tRegion = __bottom_region;
    }

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].tRegion.tSize.iWidth 
                                                        = tScreen.tSize.iWidth;

    if (NULL == ptThis) {
        ptThis = (user_scene_user_defined_opcode_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_user_defined_opcode_t),
                                                        __alignof__(user_scene_user_defined_opcode_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_user_defined_opcode_t));

    *ptThis = (user_scene_user_defined_opcode_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_user_defined_opcode_load,
            .fnScene        = &__pfb_draw_scene_user_defined_opcode_handler,
            //.fnAfterSwitch  = &__after_scene_user_defined_opcode_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_user_defined_opcode_background_start,
            //.fnOnBGComplete = &__on_scene_user_defined_opcode_background_complete,
            .fnOnFrameStart = &__on_scene_user_defined_opcode_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_user_defined_opcode_switching_out,
            .fnOnFrameCPL   = &__on_scene_user_defined_opcode_frame_complete,
            .fnDepose       = &__on_scene_user_defined_opcode_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_user_defined_opcode_t begin ---------------*/
    /* draw line */
    do {
        arm_foreach(arm_2d_user_draw_line_descriptor_t, this.tDrawLineOP, ptLineOP) {
            ARM_2D_OP_INIT(*ptLineOP);
        }
    } while(0);

    /* ------------   initialize members of user_scene_user_defined_opcode_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


