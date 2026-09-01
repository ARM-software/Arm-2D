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

#if __USER_SCENE_PANEL_USE_LMSK__
#   define ICON_SIGNAL_STRENGTH_MASK          ARM_LMSK_TILE(PANEL_LMSK_SIGNAL_STRENGTH)
#else
#   define ICON_SIGNAL_STRENGTH_MASK          c_tileSignalStrengthMask
#endif

/*============================ TYPES =========================================*/
enum {
    PANEL_ICON_IDX_SIGNAL_0 = 0,
    PANEL_ICON_IDX_SIGNAL_VERY_WEAK = PANEL_ICON_IDX_SIGNAL_0,

    PANEL_ICON_IDX_SIGNAL_1 = 1,
    PANEL_ICON_IDX_SIGNAL_WEAK = PANEL_ICON_IDX_SIGNAL_1,

    PANEL_ICON_IDX_SIGNAL_2 = 2,
    PANEL_ICON_IDX_SIGNAL_GOOD = PANEL_ICON_IDX_SIGNAL_2,

    PANEL_ICON_IDX_SIGNAL_3 = 3,
    PANEL_ICON_IDX_SIGNAL_STRONG = PANEL_ICON_IDX_SIGNAL_3,

    PANEL_ICON_IDX_WIFI_0 = 4,
    PANEL_ICON_IDX_WIFI_VERY_WEAK = PANEL_ICON_IDX_WIFI_0,

    PANEL_ICON_IDX_WIFI_1 = 5,
    PANEL_ICON_IDX_WIFI_WEAK = PANEL_ICON_IDX_WIFI_1,

    PANEL_ICON_IDX_WIFI_2 = 6,
    PANEL_ICON_IDX_WIFI_GOOD = PANEL_ICON_IDX_WIFI_2,

    PANEL_ICON_IDX_WIFI_3 = 7,
    PANEL_ICON_IDX_WIFI_STRONG = PANEL_ICON_IDX_WIFI_3,

    NO_SIGNAL = 0,
    SIGNAL_VERY_WEAK,
    SIGNAL_WEAK,
    SIGNAL_GOOD,
    SIGNAL_STRONG,
};

/*============================ GLOBAL VARIABLES ==============================*/
extern 
const arm_2d_tile_t c_tileListCoverLineMask;

extern
const arm_2d_tile_t c_tileWhiteDotMask;

extern 
const arm_2d_tile_t c_tileSignalStrengthMask;

/*============================ PROTOTYPES ====================================*/


/*============================ LOCAL VARIABLES ===============================*/
static
const struct {
    uint8_t chIndex;
    COLOUR_INT Color;
} 
c_SignalStrength[] = {
    [NO_SIGNAL] = {
        PANEL_ICON_IDX_SIGNAL_3,
        __RGB( 64, 64, 64  ),
    },
    [SIGNAL_VERY_WEAK] = {
        PANEL_ICON_IDX_SIGNAL_VERY_WEAK,
        GLCD_COLOR_WHITE,
    },
    [SIGNAL_WEAK] = {
        PANEL_ICON_IDX_SIGNAL_WEAK,
        GLCD_COLOR_WHITE,
    },
    [SIGNAL_GOOD] = {
        PANEL_ICON_IDX_SIGNAL_GOOD,
        GLCD_COLOR_WHITE,
    },
    [SIGNAL_STRONG] = {
        PANEL_ICON_IDX_SIGNAL_STRONG,
        GLCD_COLOR_WHITE,
    },
},
c_WIFIStrength[] = {
    [NO_SIGNAL] = {
        PANEL_ICON_IDX_WIFI_3,
        __RGB( 64, 64, 64  ),
    },
    [SIGNAL_VERY_WEAK] = {
        PANEL_ICON_IDX_WIFI_VERY_WEAK,
        GLCD_COLOR_WHITE,
    },
    [SIGNAL_WEAK] = {
        PANEL_ICON_IDX_WIFI_WEAK,
        GLCD_COLOR_WHITE,
    },
    [SIGNAL_GOOD] = {
        PANEL_ICON_IDX_WIFI_GOOD,
        GLCD_COLOR_WHITE,
    },
    [SIGNAL_STRONG] = {
        PANEL_ICON_IDX_WIFI_STRONG,
        GLCD_COLOR_WHITE,
    },
};



/*============================ IMPLEMENTATION ================================*/

static void __on_scene_panel_on_load(arm_2d_scene_t *ptScene)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)ptScene;

    progress_wheel_on_load(&this.tWheel);

#if __USER_SCENE_PANEL_USE_LMSK__ 
    ARM_LMSK_GROUP_ON_LOAD();
#endif
}

static void __on_scene_panel_depose(arm_2d_scene_t *ptScene)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*--------------------- insert your depose code begin --------------------*/

#if __USER_SCENE_PANEL_USE_LMSK__ 
    ARM_LMSK_GROUP_DEPOSE();
#endif

    /* depose all number lists */
    arm_foreach(number_list_t,this.tNumberList, ptItem) {
        number_list_depose(ptItem);
    }

    progress_wheel_depose(&this.tWheel);
    /*---------------------- insert your depose code end  --------------------*/

    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    ptScene->ptPlayer = NULL;
    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene Panel                                                                *
 *----------------------------------------------------------------------------*/
#if 0  /* deprecated */
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
#endif

static void __on_scene_panel_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)ptScene;

    progress_wheel_on_frame_start(&this.tWheel);

    arm_foreach(number_list_t,this.tNumberList, ptItem) {
        number_list_on_frame_start(ptItem);
    }

    if (arm_2d_helper_is_time_out(1000, &this.lTimestamp[4])) {
        this.u3SignalStrength += 1;
        if (this.u3SignalStrength == dimof(c_SignalStrength)) {
            this.u3SignalStrength = 0;
        }

        this.u3WifiStrength += 1;
        if (this.u3WifiStrength == dimof(c_WIFIStrength)) {
            this.u3WifiStrength = 0;
        }

        this.bIconBarChanged = true;
    }

    do {
        int32_t nResult;
        /* simulate a full battery charging/discharge cycle */
        arm_2d_helper_time_cos_slider(0, 1000, 30000, 0, &nResult, &this.lTimestamp[5]);
        
        if (this.i11SoC != nResult) {
            this.bIconBarChanged = true;
        }

        if (this.i11SoC < nResult) {
            this.i2BatteryStatus = BATTERY_STATUS_CHARGING;
        } else if (this.i11SoC > nResult) {
            this.i2BatteryStatus = BATTERY_STATUS_DISCHARGING;
        }
        this.i11SoC = (uint16_t)nResult;

    } while(0);


#if __USER_SCENE_PANEL_USE_LMSK__ 
    ARM_LMSK_GROUP_ON_FRAME_START();
#endif
}

static void __on_scene_panel_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_panel_t *ptThis = (user_scene_panel_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
        number_list_move_selection(&this.tNumberList[0], 100, 10000);
    }
    
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[1])) {
        number_list_move_selection(&this.tNumberList[1], 10, 10000);
    }
    
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[2])) {
        number_list_move_selection(&this.tNumberList[2], 1, 10000);
    }
    
    if (arm_2d_helper_is_time_out(10, &this.lTimestamp[3])) {
        this.iProgress += 4;
        if (this.iProgress > 1000) {
            this.iProgress = 0;
        }
    }

#if __USER_SCENE_PANEL_USE_LMSK__ 
    ARM_LMSK_GROUP_ON_FRAME_COMPLETE();
#endif

}

static void draw_buttom(const arm_2d_tile_t *ptTile, 
                        arm_2d_region_t *ptRegion,
                        const char *pchString,
                        COLOUR_INT tColour,
                        uint8_t chOpacity)
{
    arm_2d_container(ptTile, __button, ptRegion) {
    
        draw_round_corner_box(&__button, NULL, GLCD_COLOR_WHITE, chOpacity);
        
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

        arm_2d_dock_top(__canvas, 16, 2) {
            arm_2d_layout(__top_region, RIGHT_TO_LEFT) {

                __item_line_dock_horizontal(32, 2, 2, 0, 0){

                    draw_battery_gasgauge_bar(  ptTile, 
                                                &__item_region, 
                                                this.i11SoC, 
                                                this.i2BatteryStatus);
                }

                __item_line_dock_horizontal(this.tIcons.tTile.tRegion.tSize.iWidth, 2, 2, 0, 0){

                    arm_2d_helper_film_set_frame(
                        &this.tIcons, 
                        c_WIFIStrength[this.u3WifiStrength].chIndex);

                    arm_2d_fill_colour_with_mask(   
                        ptTile, 
                        &__item_region,  
                        &this.tIcons.tTile, 
                        (__arm_2d_color_t){c_WIFIStrength[this.u3WifiStrength].Color});
                }

                __item_line_dock_horizontal(this.tIcons.tTile.tRegion.tSize.iWidth, 2, 2, 0, 0){

                    arm_2d_helper_film_set_frame(
                        &this.tIcons, 
                        c_SignalStrength[this.u3SignalStrength].chIndex);

                    arm_2d_fill_colour_with_mask(   
                        ptTile, 
                        &__item_region,  
                        &this.tIcons.tTile, 
                        (__arm_2d_color_t){c_SignalStrength[this.u3SignalStrength].Color});
                }

            }

            if (this.bIconBarChanged) {
                this.bIconBarChanged = false;
                arm_2d_helper_dirty_region_update_item( 
                    &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
                    (arm_2d_tile_t *)ptTile,
                    &__canvas,
                    &__top_region);
            }
        }

        arm_2d_align_centre(__canvas, 240, 128) {

            arm_2d_layout(__centre_region) {

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
                
                    arm_2d_layout(__item_region) {
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "1", GLCD_COLOR_WHITE, 128);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "2", GLCD_COLOR_WHITE, 128);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "3", GLCD_COLOR_WHITE, 128);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "4", GLCD_COLOR_WHITE, 128);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "5", GLCD_COLOR_WHITE, 128);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "6", GLCD_COLOR_WHITE, 128);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "7", GLCD_COLOR_WHITE, 128);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "8", GLCD_COLOR_WHITE, 128);
                        }
                        __item_horizontal(28,28,2,2,2,2) {
                            draw_buttom(ptTile, &__item_region, "9", GLCD_COLOR_WHITE, 128);
                        }
                        __item_horizontal(28,28,34,34,2,2) {
                            draw_buttom(ptTile, &__item_region, "0", GLCD_COLOR_WHITE, 128);
                        }
                    }
                }
            }
        }
        
        arm_2d_align_top_left( __canvas, 
                                c_tileWhiteDotMask.tRegion.tSize.iWidth + 10,
                                c_tileWhiteDotMask.tRegion.tSize.iHeight) {
            
            if (bIsNewFrame) {
                int32_t iResult;
                arm_2d_helper_time_cos_slider(0, 255, 2000, 0, &iResult);
                this.chOpacity = (uint8_t)iResult;
            }
            
            arm_2d_fill_colour_with_mask_and_opacity(ptTile,
                                                     &__top_left_region,
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
    ARM_2D_OP_WAIT_ASYNC();

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

    ARM_2D_OP_WAIT_ASYNC();
    
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
    arm_2d_region_t __top_canvas__
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    

    arm_2d_align_top_left( __top_canvas__, 
                            c_tileWhiteDotMask.tRegion.tSize.iWidth + 10,
                            c_tileWhiteDotMask.tRegion.tSize.iHeight) {
        s_tDirtyRegions[0].tRegion = __top_left_region;
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

#if __USER_SCENE_PANEL_USE_LMSK__ 
    ARM_LMSK_ITEM_INIT_WITH_ROM(PANEL_LMSK_SIGNAL_STRENGTH, 
                                c_lmskSignalStrength, 
                                1216);
#endif

    this.tIcons = (arm_2d_helper_film_t)impl_film(  ICON_SIGNAL_STRENGTH_MASK, 
                                                    16, 
                                                    16, 
                                                    8, 
                                                    8, 
                                                    1000);

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
            .nStart = 0,
            .iDelta = 1,

            .use_as____simple_list_cfg_t = {
                .hwCount = 10,
                
                .tFontColour = GLCD_COLOR_WHITE,
                .tBackgroundColour = GLCD_COLOR_BLACK,
                .chNextPadding = 3,
                .chPreviousPadding = 3,
                .tListSize = {
                    .iHeight = 80,
                    .iWidth = 28,
                },
                .ptFont = (arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY,
                /* draw list cover */
                .fnOnDrawListCover = &__arm_2d_number_list_draw_cover,

                .bUseDirtyRegion = true,
                .ptTargetScene = &this.use_as__arm_2d_scene_t,
            },
        };
        number_list_init(&this.tNumberList[0], &tCFG);
        number_list_init(&this.tNumberList[1], &tCFG);
    } while(0);
    
    /* initialize number list */
    do {
        number_list_cfg_t tCFG = {
            .nStart = 0,
            .iDelta = 1,

            .use_as____simple_list_cfg_t = {
                .hwCount = 6,
                
                .tFontColour = GLCD_COLOR_WHITE,
                .tBackgroundColour = GLCD_COLOR_BLACK,
                .chNextPadding = 3,
                .chPreviousPadding = 3,
                .tListSize = {
                    .iHeight = 80,
                    .iWidth = 28,
                },
                .ptFont = (arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY,
                /* draw list cover */
                .fnOnDrawListCover = &__arm_2d_number_list_draw_cover,

                .bUseDirtyRegion = true,
                .ptTargetScene = &this.use_as__arm_2d_scene_t,
            },
        };
        number_list_init(&this.tNumberList[2], &tCFG);
    } while(0);
    
    number_list_move_selection(&this.tNumberList[0], 100,   10000);
    number_list_move_selection(&this.tNumberList[1], 10,    10000);
    number_list_move_selection(&this.tNumberList[2], 1,     10000);


    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &ptScene->use_as__arm_2d_scene_t, 
                                        1);

    return ptScene;
}




#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

