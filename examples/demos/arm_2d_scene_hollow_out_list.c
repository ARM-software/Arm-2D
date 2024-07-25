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

#define __USER_SCENE_HOLLOW_OUT_LIST_IMPLEMENT__
#include "arm_2d_scene_hollow_out_list.h"

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
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern const arm_2d_tile_t c_tileBackgroundMaskGRAY8;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

enum{
    DIRTY_REGION_LIST_SMALL_1,
    DIRTY_REGION_LIST_SMALL_2,
    DIRTY_REGION_LIST_SMALL_3,
    DIRTY_REGION_LIST_SMALL_4,
    DIRTY_REGION_LIST_CENTRAL,
};

/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

    /* a dirty region to be specified at runtime*/
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        .tSize = {
            .iWidth = 26,
            .iHeight = 36,
        },
    ),

    ADD_REGION_TO_LIST(s_tDirtyRegions,
        .tSize = {
            .iWidth = 26,
            .iHeight = 36,
        },
    ),

    ADD_REGION_TO_LIST(s_tDirtyRegions,
        .tSize = {
            .iWidth = 26,
            .iHeight = 36,
        },
    ),

    ADD_REGION_TO_LIST(s_tDirtyRegions,
        .tSize = {
            .iWidth = 26,
            .iHeight = 36,
        },
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
            .iWidth = 45,
            .iHeight = 45,
        },
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_hollow_out_list_load(arm_2d_scene_t *ptScene)
{
    user_scene_hollow_out_list_t *ptThis = (user_scene_hollow_out_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_hollow_out_list_depose(arm_2d_scene_t *ptScene)
{
    user_scene_hollow_out_list_t *ptThis = (user_scene_hollow_out_list_t *)ptScene;
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
 * Scene hollow_out_list                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_hollow_out_list_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_hollow_out_list_t *ptThis = (user_scene_hollow_out_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_hollow_out_list_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_hollow_out_list_t *ptThis = (user_scene_hollow_out_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_hollow_out_list_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_hollow_out_list_t *ptThis = (user_scene_hollow_out_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    int16_t nStepsToMove = 1; 
    uint16_t hwMoveSpeedInMs = 300;


    uint16_t hwABSSteps = ABS(nStepsToMove);
    if (arm_2d_helper_is_time_out(1000, &this.lTimestamp[1])) {
        number_list_move_selection(&this.tNumberList[0], nStepsToMove, hwMoveSpeedInMs * hwABSSteps);
        number_list_move_selection(&this.tNumberList[1], nStepsToMove, hwMoveSpeedInMs * hwABSSteps);
    }


    do {
        bool bIsListMoving = __arm_2d_list_core_is_list_moving(&this.tNumberList[0].use_as____arm_2d_list_core_t);
        
        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_1],
                                            !bIsListMoving); 
        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_2],
                                            !bIsListMoving);
        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_3],
                                            !bIsListMoving); 
        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_4],
                                            !bIsListMoving); 
        
        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_LIST_CENTRAL],
                                            !bIsListMoving); 

    } while(0);


    number_list_on_frame_start(&this.tNumberList[0]);
    number_list_on_frame_start(&this.tNumberList[1]);

}

static void __on_scene_hollow_out_list_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_hollow_out_list_t *ptThis = (user_scene_hollow_out_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if 0
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(3000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene_hollow_out_list_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_hollow_out_list_t *ptThis = (user_scene_hollow_out_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_hollow_out_list_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_hollow_out_list_t *ptThis = (user_scene_hollow_out_list_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        arm_2d_align_centre(__top_canvas, 76, 284 ) {

            while(arm_fsm_rt_cpl != number_list_show(   &this.tNumberList[0], 
                                                        ptTile,
                                                        &__centre_region,
                                                        bIsNewFrame));

        }


        arm_2d_align_centre(__top_canvas, 45, 45 ) {

            while(arm_fsm_rt_cpl != number_list_show(   &this.tNumberList[1], 
                                                        ptTile,
                                                        &__centre_region,
                                                        bIsNewFrame));

        }

        arm_2d_align_centre(__top_canvas, c_tileBackgroundMaskGRAY8.tRegion.tSize) {

            /* apply mask */
            arm_2d_fill_colour_with_4pts_alpha_gradient_and_mask(
                                            ptTile, 
                                            &__centre_region, 
                                            &c_tileBackgroundMaskGRAY8,
                                            (__arm_2d_color_t){GLCD_COLOR_NIXIE_TUBE},
                                            (arm_2d_alpha_samples_4pts_t) {
                                                255, 128,
                                                128, 255,
                                            });


        }


                /* draw border only when necessary */
        if (tScreenSize.iWidth > 76 && tScreenSize.iHeight > 284) {
            arm_2d_align_centre(__top_canvas, 76+8, 284+8 ) {
                draw_round_corner_border(   ptTile, 
                                            &__centre_region, 
                                            GLCD_COLOR_WHITE, 
                                            (arm_2d_border_opacity_t)
                                                {64, 64, 255-64, 255-64},
                                            (arm_2d_corner_opacity_t)
                                                {0, 128, 128, 128});
            }
        }




    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}


static 
arm_fsm_rt_t __ruler_number_list_draw_central_list_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    number_list_t *ptThis = (number_list_t *)ptItem->ptListView;
    ARM_2D_UNUSED(ptItem);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    arm_2d_size_t tListSize = __arm_2d_list_core_get_list_size(&this.use_as____arm_2d_list_core_t);


    arm_2d_canvas(ptTile, __top_container) {

        arm_lcd_text_set_scale(1.5);
        arm_2d_size_t tTextSize = arm_lcd_get_string_line_box("00", &ARM_2D_FONT_A4_DIGITS_ONLY);

        arm_2d_align_centre(__top_container, tTextSize) {

            __centre_region.tLocation.iY += 5;
            __centre_region.tLocation.iX += 5;

            arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
            arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
            arm_lcd_text_set_draw_region(&__centre_region);
            arm_lcd_text_set_colour(GLCD_COLOR_NIXIE_TUBE, GLCD_COLOR_BLACK);
            arm_lcd_text_location(0,0);
            
            arm_lcd_text_set_scale(1.5);

            arm_lcd_text_reset_display_region_tracking();
            arm_lcd_printf("%02d", ptItem->hwID);

            ARM_2D_OP_WAIT_ASYNC();
        }
    }
    
    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_hollow_out_list_t *__arm_2d_scene_hollow_out_list_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_hollow_out_list_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    arm_2d_align_centre(tScreen, 76, 284) {

        arm_2d_align_centre(__centre_region, s_tDirtyRegions[DIRTY_REGION_LIST_CENTRAL].tRegion.tSize) {
            s_tDirtyRegions[DIRTY_REGION_LIST_CENTRAL].tRegion = __centre_region;
        }

        arm_2d_dock_horizontal(__centre_region, s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_1].tRegion.tSize.iWidth) {
            arm_2d_layout(__horizontal_region) {

                __item_line_vertical(s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_1].tRegion.tSize,
                                    0, 0, 12, 10
                                    ) {
                    s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_1].tRegion = __item_region;
                }

                __item_line_vertical(s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_2].tRegion.tSize,
                                    0, 0, 10, 11
                                    ) {
                    s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_2].tRegion = __item_region;
                }
            }

            arm_2d_layout(__horizontal_region, BOTTOM_UP, false) {

                __item_line_vertical(s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_3].tRegion.tSize,
                                    0, 0, 10, 11
                                    ) {
                    s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_3].tRegion = __item_region;
                }

                __item_line_vertical(s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_4].tRegion.tSize,
                                    0, 0, 10, 11
                                    ) {
                    s_tDirtyRegions[DIRTY_REGION_LIST_SMALL_4].tRegion = __item_region;
                }

            }
        }
        
    }

    

    if (NULL == ptThis) {
        ptThis = (user_scene_hollow_out_list_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_hollow_out_list_t),
                                                        __alignof__(user_scene_hollow_out_list_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_hollow_out_list_t));

    *ptThis = (user_scene_hollow_out_list_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_hollow_out_list_load,
            .fnScene        = &__pfb_draw_scene_hollow_out_list_handler,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_hollow_out_list_background_start,
            //.fnOnBGComplete = &__on_scene_hollow_out_list_background_complete,
            .fnOnFrameStart = &__on_scene_hollow_out_list_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_hollow_out_list_switching_out,
            .fnOnFrameCPL   = &__on_scene_hollow_out_list_frame_complete,
            .fnDepose       = &__on_scene_hollow_out_list_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_hollow_out_list_t begin ---------------*/

    /* initialize number list */
    do {
        number_list_cfg_t tCFG = {
            .hwCount = 100,
            .nStart = 0,
            .iDelta = 1,
            .tFontColour = GLCD_COLOR_WHITE,
            .tBackgroundColour = GLCD_COLOR_BLACK,
            .chNextPadding = 20,
            .chPrviousePadding = 20,

            .tListSize = {
                .iWidth = 76,
                .iHeight = 284,
            },

            .ptFont = (arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY,

            .bUseDirtyRegion = false,
            .ptTargetScene = &this.use_as__arm_2d_scene_t,
        };
        number_list_init(&this.tNumberList[0], &tCFG);
    } while(0);

    do {
        number_list_cfg_t tCFG = {
            .hwCount = 100,
            .nStart = 0,
            .iDelta = 1,
            .tFontColour = GLCD_COLOR_WHITE,
            .tBackgroundColour = GLCD_COLOR_BLACK,

            .tListSize = {
                .iWidth = 45,
                .iHeight = 45,
            },

            .tItemSize = {
                .iWidth = 45,
                .iHeight = 45,
            },

            .fnOnDrawListItem = &__ruler_number_list_draw_central_list_item,

            .bUseDirtyRegion = false,
            .ptTargetScene = &this.use_as__arm_2d_scene_t,
        };
        number_list_init(&this.tNumberList[1], &tCFG);
    } while(0);


    number_list_move_selection(&this.tNumberList[0], 50, 0);
    number_list_move_selection(&this.tNumberList[1], 50, 0);

    /* ------------   initialize members of user_scene_hollow_out_list_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


