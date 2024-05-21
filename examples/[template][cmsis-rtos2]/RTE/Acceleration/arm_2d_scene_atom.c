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

#define __USER_SCENE_ATOM_IMPLEMENT__
#include "arm_2d_scene_atom.h"

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
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern const arm_2d_tile_t c_tileWhiteDotMiddleA4Mask;
extern const arm_2d_tile_t c_tileWhiteDotMask;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __on_scene_atom_load(arm_2d_scene_t *ptScene)
{
    user_scene_atom_t *ptThis = (user_scene_atom_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_2d_helper_dirty_region_add_items(&this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                         &this.Electronic[0].tDirtyRegionItem,
                                         1);

    arm_2d_helper_dirty_region_add_items(&this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                         &this.Electronic[1].tDirtyRegionItem,
                                         1);

}

static void __on_scene_atom_depose(arm_2d_scene_t *ptScene)
{
    user_scene_atom_t *ptThis = (user_scene_atom_t *)ptScene;
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
 * Scene atom                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_atom_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_atom_t *ptThis = (user_scene_atom_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_atom_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_atom_t *ptThis = (user_scene_atom_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_atom_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_atom_t *ptThis = (user_scene_atom_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /* update core and electronics coordinates */
    do {

        int32_t nResult;
        const int16_t iRadiusX = 110;
        const int16_t iRadiusY = 110;

        /* calculate core vibration */
        arm_2d_helper_time_cos_slider(-5, 5, 100, ARM_2D_ANGLE(0.0f), &nResult, &this.lTimestamp[1]);
        this.Core.tVibration.iX = nResult;
        
        arm_2d_helper_time_cos_slider(-5, 5, 150, ARM_2D_ANGLE(30.0f), &nResult, &this.lTimestamp[2]);
        this.Core.tVibration.iY = nResult;

        /* calculate electronic0 vibration */
        arm_2d_helper_time_cos_slider(-iRadiusX, iRadiusX, 1300, ARM_2D_ANGLE(0.0f), &nResult, &this.lTimestamp[3]);
        this.Electronic[0].tOffset.iX = nResult;
        
        arm_2d_helper_time_cos_slider(-iRadiusY, iRadiusY, 1300, ARM_2D_ANGLE(45.0f), &nResult, &this.lTimestamp[4]);
        this.Electronic[0].tOffset.iY = nResult;

        arm_2d_helper_time_cos_slider(128, 255, 1300, ARM_2D_ANGLE(45.0f), &nResult, &this.lTimestamp[7]);
        this.Electronic[0].chOpacity = nResult;

        /* calculate electronic 1 vibration */
        arm_2d_helper_time_cos_slider(-iRadiusX, iRadiusX, 1300, ARM_2D_ANGLE(180.0f), &nResult, &this.lTimestamp[5]);
        this.Electronic[1].tOffset.iX = nResult;
        
        arm_2d_helper_time_cos_slider(-iRadiusY, iRadiusY, 1300, ARM_2D_ANGLE(45.0f), &nResult, &this.lTimestamp[6]);
        this.Electronic[1].tOffset.iY = nResult;

        arm_2d_helper_time_cos_slider(128, 255, 1300, ARM_2D_ANGLE(45.0f), &nResult, &this.lTimestamp[8]);
        this.Electronic[1].chOpacity = nResult;

    } while(0);

}

static void __on_scene_atom_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_atom_t *ptThis = (user_scene_atom_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /* switch to next scene after 5s */
    if (arm_2d_helper_is_time_out(5000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static void __before_scene_atom_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_atom_t *ptThis = (user_scene_atom_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_atom_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_atom_t *ptThis = (user_scene_atom_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/

        arm_2d_size_t tCharSize = ARM_2D_FONT_A4_DIGITS_ONLY
                                .use_as__arm_2d_user_font_t
                                    .use_as__arm_2d_font_t.tCharSize;
        arm_2d_size_t tAtomCoreSize = {
            .iWidth = c_tileWhiteDotMiddleA4Mask.tRegion.tSize.iWidth * 2,
            .iHeight = c_tileWhiteDotMiddleA4Mask.tRegion.tSize.iHeight * 2 
                     + tCharSize.iHeight,
        };
        
        /* draw atom core */
        arm_2d_align_centre(__top_canvas, tAtomCoreSize) {

            arm_2d_layout(__centre_region, true) {
            
                __item_line_dock_vertical(c_tileWhiteDotMiddleA4Mask.tRegion.tSize.iHeight * 2) {
                    
                    /* apply vibration */
                    __item_region.tLocation.iX += this.Core.tVibration.iX;
                    __item_region.tLocation.iY += this.Core.tVibration.iY;

                    arm_2d_region_t tDirtyRegion = __item_region;
                    tDirtyRegion.tSize.iHeight += tCharSize.iHeight;

                    /* update dirty region */
                    arm_2d_helper_dirty_region_update_item(&this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                                           &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
                                                            (arm_2d_tile_t *)ptTile,
                                                            &__top_canvas,
                                                            &tDirtyRegion);

                    do {
                        arm_2d_region_t tHadron = __item_region;

                        tHadron.tLocation.iX += (c_tileWhiteDotMiddleA4Mask.tRegion.tSize.iWidth >> 1) + 5;
                        arm_2d_fill_colour_with_a4_mask_and_opacity(
                            ptTile,
                            &tHadron,
                            &c_tileWhiteDotMiddleA4Mask,
                            (__arm_2d_color_t){GLCD_COLOR_RED},
                            128);
                        
                        ARM_2D_OP_WAIT_ASYNC();
                    } while(0);

                    do {
                        arm_2d_region_t tHadron = __item_region;

                        tHadron.tLocation.iX += c_tileWhiteDotMiddleA4Mask.tRegion.tSize.iWidth >> 1;
                        tHadron.tLocation.iY += (c_tileWhiteDotMiddleA4Mask.tRegion.tSize.iHeight >> 1) + 5;
                        arm_2d_fill_colour_with_a4_mask_and_opacity(
                            ptTile,
                            &tHadron,
                            &c_tileWhiteDotMiddleA4Mask,
                            (__arm_2d_color_t){GLCD_COLOR_BLUE},
                            256 - 64);
                        
                        ARM_2D_OP_WAIT_ASYNC();
                    } while(0);

                    do {
                        arm_2d_region_t tHadron = __item_region;
                        tHadron.tLocation.iX += 5;
                        tHadron.tLocation.iY += (c_tileWhiteDotMiddleA4Mask.tRegion.tSize.iHeight >> 1) - 5;
                        arm_2d_fill_colour_with_a4_mask_and_opacity(
                            ptTile,
                            &tHadron,
                            &c_tileWhiteDotMiddleA4Mask,
                            (__arm_2d_color_t){GLCD_COLOR_BLUE},
                            256 - 32);
                        
                        ARM_2D_OP_WAIT_ASYNC();
                    } while(0);

                    do {
                        arm_2d_region_t tHadron = __item_region;

                        tHadron.tLocation.iX += c_tileWhiteDotMiddleA4Mask.tRegion.tSize.iWidth;
                        tHadron.tLocation.iY += c_tileWhiteDotMiddleA4Mask.tRegion.tSize.iHeight >> 1;
                        arm_2d_fill_colour_with_a4_mask_and_opacity(
                            ptTile,
                            &tHadron,
                            &c_tileWhiteDotMiddleA4Mask,
                            (__arm_2d_color_t){GLCD_COLOR_RED},
                            255);
                        
                        ARM_2D_OP_WAIT_ASYNC();
                    } while(0);
                }

                __item_line_dock_vertical() {

                    /* apply vibration */
                    __item_region.tLocation.iX += this.Core.tVibration.iX;
                    __item_region.tLocation.iY += this.Core.tVibration.iY;

                    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                    arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
                    arm_print_banner("2+", __item_region, &ARM_2D_FONT_A4_DIGITS_ONLY);
                }
            }
        }

        /* draw electronics */
        do {
            arm_2d_align_centre(__top_canvas, 
                                220, 
                                220) {

                arm_2d_size_t tElectronicSize = {
                    .iWidth = c_tileWhiteDotMask.tRegion.tSize.iWidth
                            + tCharSize.iWidth,
                    .iHeight = c_tileWhiteDotMask.tRegion.tSize.iWidth
                            + tCharSize.iHeight,
                };

                /* electronic 0 */
                arm_2d_align_centre(__centre_region, tElectronicSize) {

                    __centre_region.tLocation.iX += this.Electronic[0].tOffset.iX;
                    __centre_region.tLocation.iY += this.Electronic[0].tOffset.iY;
              
                    /* update dirty region */
                    arm_2d_helper_dirty_region_update_item( &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                                            &this.Electronic[0].tDirtyRegionItem,
                                                            (arm_2d_tile_t *)ptTile,
                                                            &__top_canvas,
                                                            &__centre_region);

                    arm_2d_align_bottom_left(__centre_region, c_tileWhiteDotMask.tRegion.tSize) {

                        arm_2d_fill_colour_with_mask_and_opacity(
                            ptTile,
                            &__bottom_left_region,
                            &c_tileWhiteDotMask,
                            (__arm_2d_color_t){GLCD_COLOR_GREEN},
                            this.Electronic[0].chOpacity);

                        ARM_2D_OP_WAIT_ASYNC();
                    }

                    arm_2d_align_mid_right(__centre_region, tCharSize) {
                        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                        arm_lcd_text_set_colour(GLCD_COLOR_GREEN, GLCD_COLOR_WHITE);
                        arm_print_banner("-", __mid_right_region, &ARM_2D_FONT_A4_DIGITS_ONLY);
                    }
                }
                
                /* electronic 1 */
                arm_2d_align_centre(__centre_region, tElectronicSize) {

                    __centre_region.tLocation.iX += this.Electronic[1].tOffset.iX;
                    __centre_region.tLocation.iY += this.Electronic[1].tOffset.iY;

                    /* update dirty region */
                    arm_2d_helper_dirty_region_update_item( &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                                            &this.Electronic[1].tDirtyRegionItem,
                                                            (arm_2d_tile_t *)ptTile,
                                                            &__top_canvas,
                                                            &__centre_region);

                    arm_2d_align_bottom_left(__centre_region, c_tileWhiteDotMask.tRegion.tSize) {

                        arm_2d_fill_colour_with_mask_and_opacity(
                            ptTile,
                            &__bottom_left_region,
                            &c_tileWhiteDotMask,
                            (__arm_2d_color_t){GLCD_COLOR_GREEN},
                            this.Electronic[1].chOpacity);

                        ARM_2D_OP_WAIT_ASYNC();
                    }

                    arm_2d_align_mid_right(__centre_region, tCharSize) {

                        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                        arm_lcd_text_set_colour(GLCD_COLOR_GREEN, GLCD_COLOR_WHITE);
                        arm_print_banner("-", __mid_right_region, &ARM_2D_FONT_A4_DIGITS_ONLY);
                        
                    }
                }

            }
        } while(0);

        /* draw text at the top-left corner */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene Atom");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_atom_t *__arm_2d_scene_atom_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_atom_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_atom_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_atom_t),
                                                        __alignof__(user_scene_atom_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_atom_t));

    *ptThis = (user_scene_atom_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_atom_load,
            .fnScene        = &__pfb_draw_scene_atom_handler,

            //.fnOnBGStart    = &__on_scene_atom_background_start,
            //.fnOnBGComplete = &__on_scene_atom_background_complete,
            .fnOnFrameStart = &__on_scene_atom_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_atom_switching_out,
            .fnOnFrameCPL   = &__on_scene_atom_frame_complete,
            .fnDepose       = &__on_scene_atom_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_atom_t begin ---------------*/

    /* ------------   initialize members of user_scene_atom_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


