/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
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

#include "arm_2d.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#define __USER_SCENE_PANEL_IMPLEMENT__
#include "arm_2d_scene_panel.h"

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
#   pragma clang diagnostic ignored "-Wgnu-variable-sized-type-not-at-end"
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
#   pragma GCC diagnostic ignored "-Wunused-value"
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
extern 
const arm_2d_tile_t c_tileListCoverLineMask;

extern
const arm_2d_tile_t c_tileWhiteDotMask;
/*============================ PROTOTYPES ====================================*/


/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __on_scene_panel_on_load(arm_2d_scene_t *ptScene)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)ptScene;

    progress_wheel_on_load(&this.tWheel);
}

static void __on_scene_panel_depose(arm_2d_scene_t *ptScene)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    progress_wheel_depose(&this.tWheel);

    /* depose all number lists */
    arm_foreach(number_list_t,this.tNumberList, ptItem) {
        number_list_depose(ptItem);
    }

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene 2                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_panel_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_panel_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_panel_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)ptScene;

    progress_wheel_on_frame_start(&this.tWheel);

    arm_foreach(number_list_t,this.tNumberList, ptItem) {
        number_list_on_frame_start(ptItem);
    }
}

static void __on_scene_panel_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
        numer_list_move_selection(&this.tNumberList[0], 100, 10000);
    }
    
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[1])) {
        numer_list_move_selection(&this.tNumberList[1], 10, 10000);
    }
    
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[2])) {
        numer_list_move_selection(&this.tNumberList[2], 1, 10000);
    }
    
    if (arm_2d_helper_is_time_out(10, &this.lTimestamp[3])) {
        this.iProgress += 4;
        if (this.iProgress > 1000) {
            this.iProgress = 0;
        }
    }
    
    /* switch to next scene after 20s */
    if (arm_2d_helper_is_time_out(20000, &this.lTimestamp[4])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_panel_background_handler)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    /*-----------------------draw back ground begin-----------------------*/



    /*-----------------------draw back ground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static void draw_buttom(const arm_2d_tile_t *ptTile, 
                        arm_2d_region_t *ptRegion,
                        const char *pchString,
                        COLOUR_INT tColour,
                        uint8_t chOpacity,
                        bool bIsNewFrame)
{

    arm_2d_container(ptTile, __button, ptRegion) {
    
        draw_round_corner_box(&__button, NULL, GLCD_COLOR_WHITE, chOpacity, bIsNewFrame);
        
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)&__button);
        arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
        arm_lcd_text_set_opacity(chOpacity);
        arm_lcd_text_set_colour(tColour, GLCD_COLOR_BLACK);
        arm_print_banner(pchString, __button_canvas);
        arm_lcd_text_set_opacity(255);
        
    }
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_panel_handler)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    /*-----------------------draw the foreground begin-----------------------*/

    /* following code is just a demo, you can remove them */

    arm_2d_canvas(ptTile, __canvas) {
        arm_2d_align_centre(__canvas, 240, 128) {

            arm_2d_layout(__centre_region, true) {

                __item_line_horizontal(60,80) {
                    progress_wheel_show(&this.tWheel,
                                        ptTile, 
                                        &__item_region,       
                                        this.iProgress,         /* progress 0~1000 */
                                        255 - 32,               /* opacity */
                                        bIsNewFrame);
                }

                __item_line_horizontal(28, 80) {
                    while(arm_fsm_rt_cpl != number_list_show(   
                                                    &this.tNumberList[2], 
                                                    ptTile, 
                                                    &__item_region, 
                                                    bIsNewFrame));
                }

                __item_line_horizontal(28, 80) {
                    while(arm_fsm_rt_cpl != number_list_show(   
                                                    &this.tNumberList[1], 
                                                    ptTile, 
                                                    &__item_region, 
                                                    bIsNewFrame));
                }

                __item_line_horizontal(28, 80) {
                    while(arm_fsm_rt_cpl != number_list_show(   
                                                    &this.tNumberList[0], 
                                                    ptTile, 
                                                    &__item_region, 
                                                    bIsNewFrame));
                }
                
                __item_line_horizontal(96, 128) {
                
                    /*
                     * draw digits panel
                     *
                     *  [1][2][3]
                     *  [4][5][6]
                     *  [7][8][9]
                     *     [0]
                     */
                
                    arm_2d_layout(__item_region, true) {
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "1", GLCD_COLOR_WHITE, 128, bIsNewFrame);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "2", GLCD_COLOR_WHITE, 128, bIsNewFrame);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "3", GLCD_COLOR_WHITE, 128, bIsNewFrame);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "4", GLCD_COLOR_WHITE, 128, bIsNewFrame);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "5", GLCD_COLOR_WHITE, 128, bIsNewFrame);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "6", GLCD_COLOR_WHITE, 128, bIsNewFrame);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "7", GLCD_COLOR_WHITE, 128, bIsNewFrame);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "8", GLCD_COLOR_WHITE, 128, bIsNewFrame);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "9", GLCD_COLOR_WHITE, 128, bIsNewFrame);
                        }
                        __item_horizontal(28,28,34,34,2,2) {
                            draw_buttom(ptTile, &__item_region, "0", GLCD_COLOR_WHITE, 128, bIsNewFrame);
                        }
                    }
                }
            }
        }
        
        arm_2d_align_top_right( __canvas, 
                                c_tileWhiteDotMask.tRegion.tSize.iWidth + 10,
                                c_tileWhiteDotMask.tRegion.tSize.iHeight) {
            
            if (bIsNewFrame) {
                int32_t iResult;
                arm_2d_helper_time_cos_slider(0, 255, 2000, 0, &iResult);
                this.chOpacity = (uint8_t)iResult;
            }
            
            arm_2d_fill_colour_with_mask_and_opacity(ptTile,
                                                     &__top_right_region,
                                                     &c_tileWhiteDotMask,
                                                     (__arm_2d_color_t){GLCD_COLOR_RED},
                                                     this.chOpacity);
        }

        /* draw text at the top-left corner */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_printf("Scene Panel");
    }
    /*-----------------------draw the foreground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static 
IMPL_PFB_ON_DRAW(__arm_2d_number_list_draw_cover)
{
    ARM_2D_UNUSED(bIsNewFrame);
    
    number_list_t *ptThis = (number_list_t *)pTarget;

    arm_2d_canvas(ptTile, __canvas) {

        arm_2d_dock_vertical(__canvas, c_tileListCoverLineMask.tRegion.tSize.iHeight) {
            arm_2d_fill_colour_with_vertical_line_mask( ptTile, 
                                                        &__vertical_region, 
                                                        &c_tileListCoverLineMask, 
                                                        (__arm_2d_color_t){GLCD_COLOR_BLACK});
        }
    }

    arm_2d_op_wait_async(NULL);
    
    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_panel_t *__arm_2d_scene_panel_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_panel_t *ptScene)
{

    bool bUserAllocated = true;
    assert(NULL != ptDispAdapter);

    if (NULL == ptScene) {
        ptScene = (user_scene_panel_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_panel_t),
                                                        __alignof__(user_scene_panel_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptScene);
        if (NULL == ptScene) {
            return NULL;
        }
        bUserAllocated = false;
    }

    memset(ptScene, 0, sizeof(user_scene_panel_t));


    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

        /* add the last region:
         * it is the top left corner for text display 
         */
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {
                .iX = 0,
                .iY = 0,
            },
            .tSize = {
                .iWidth = 60,
                .iHeight = 8,
            },
        ),
    END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)
    
    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    

    arm_2d_align_top_right( tScreen, 
                            c_tileWhiteDotMask.tRegion.tSize.iWidth + 10,
                            c_tileWhiteDotMask.tRegion.tSize.iHeight) {
        s_tDirtyRegions[0].tRegion = __top_right_region;
    }


    *ptScene = (user_scene_panel_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 
            
            /* Please uncommon the callbacks if you need them
            */
            .fnOnLoad       = &__on_scene_panel_on_load,
            .fnScene        = &__pfb_draw_scene_panel_handler,
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_panel_background_start,
            //.fnOnBGComplete = &__on_scene_panel_background_complete,
            .fnOnFrameStart = &__on_scene_panel_frame_start,
            .fnOnFrameCPL   = &__on_scene_panel_frame_complete,
            .fnDepose       = &__on_scene_panel_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    user_scene_panel_t *ptThis = (user_scene_panel_t *)ptScene;

    do {
        progress_wheel_cfg_t tCFG = {
            .tDotColour     = GLCD_COLOR_WHITE,         /* dot colour */
            .tWheelColour   = GLCD_COLOR_GREEN,         /* arc colour */
            .iWheelDiameter = 60,                       /* diameter, 0 means use the mask's original size */
            .bUseDirtyRegions = true,                  /* use dirty regions */
        };

        progress_wheel_init(&this.tWheel, 
                            &this.use_as__arm_2d_scene_t,
                            &tCFG);
    } while(0);

    /* initialize number list */
    do {
        number_list_cfg_t tCFG = {
            .hwCount = 10,
            .nStart = 0,
            .iDelta = 1,
            .tFontColour = GLCD_COLOR_WHITE,
            .tBackgroundColour = GLCD_COLOR_BLACK,
            .chNextPadding = 3,
            .chPrviousePadding = 3,
            .tListSize = {
                .iHeight = 80,
                .iWidth = 28,
            },
            .ptFont = (arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY,
            /* draw list cover */
            .fnOnDrawListCover = &__arm_2d_number_list_draw_cover,

            .bUseDirtyRegion = true,
            .ptTargetScene = &this.use_as__arm_2d_scene_t,
        };
        number_list_init(&this.tNumberList[0], &tCFG);
        number_list_init(&this.tNumberList[1], &tCFG);
    } while(0);
    
    /* initialize number list */
    do {
        number_list_cfg_t tCFG = {
            .hwCount = 6,
            .nStart = 0,
            .iDelta = 1,
            .tFontColour = GLCD_COLOR_WHITE,
            .tBackgroundColour = GLCD_COLOR_BLACK,
            .chNextPadding = 3,
            .chPrviousePadding = 3,
            .tListSize = {
                .iHeight = 80,
                .iWidth = 28,
            },
            .ptFont = (arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY,
            /* draw list cover */
            .fnOnDrawListCover = &__arm_2d_number_list_draw_cover,

            .bUseDirtyRegion = true,
            .ptTargetScene = &this.use_as__arm_2d_scene_t,
        };
        number_list_init(&this.tNumberList[2], &tCFG);
    } while(0);
    
    numer_list_move_selection(&this.tNumberList[0], 100,   10000);
    numer_list_move_selection(&this.tNumberList[1], 10,    10000);
    numer_list_move_selection(&this.tNumberList[2], 1,     10000);


    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &ptScene->use_as__arm_2d_scene_t, 
                                        1);

    return ptScene;
}




#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

