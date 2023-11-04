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

#define __USER_SCENE_FITNESS_IMPLEMENT__
#include "arm_2d_scene_fitness.h"

#include "arm_2d_helper.h"
#include "arm_extra_controls.h"

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


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32


#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern 
const arm_2d_tile_t c_tileListCoverMask;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


static void __on_scene_fitness_depose(arm_2d_scene_t *ptScene)
{
    user_scene_fitness_t *ptThis = (user_scene_fitness_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    progress_wheel_depose(&this.tWheel);

    if (!this.bUserAllocated) {
        free(ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene fitness                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_fitness_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_fitness_t *ptThis = (user_scene_fitness_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_fitness_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_fitness_t *ptThis = (user_scene_fitness_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_fitness_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_fitness_t *ptThis = (user_scene_fitness_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_fitness_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_fitness_t *ptThis = (user_scene_fitness_t *)ptScene;
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

    do {
        int32_t nResult;
        if (arm_2d_helper_time_liner_slider(0, 
                                            1000, 
                                            15000, 
                                            &nResult, 
                                            &this.lTimestamp[3])) {
            this.lTimestamp[3] = 0;
        }

        this.iProgress = (int16_t)nResult;
    } while(0);


    /* switch to next scene after 20s */
    if (arm_2d_helper_is_time_out(20000, &this.lTimestamp[4])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static void __before_scene_fitness_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_fitness_t *ptThis = (user_scene_fitness_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_fitness_handler)
{
    user_scene_fitness_t *ptThis = (user_scene_fitness_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(tScreenSize);
    
    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */
        
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_BLACK);

    #if __FITNESS_CFG_NEBULA_ENABLE__
        /* show nebula */
        dynamic_nebula_show(&this.tNebula, 
                            ptTile, 
                            &__top_canvas, 
                            GLCD_COLOR_WHITE, 
                            255,
                            bIsNewFrame);
    #endif

        arm_2d_align_centre(__top_canvas, 240, 240 ) {
            progress_wheel_show(&this.tWheel,
                                ptTile, 
                                &__centre_region,       
                                this.iProgress,         /* progress 0~1000 */
                                128,                    /* opacity */
                                bIsNewFrame);
            arm_2d_op_wait_async(NULL);
        }

        arm_2d_align_centre(__top_canvas, 84, 80 ) {
            arm_2d_layout(__centre_region) {

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
            }
        }

        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene fitness");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static 
IMPL_PFB_ON_DRAW(__arm_2d_number_list_draw_cover)
{
    ARM_2D_UNUSED(bIsNewFrame);
    
    number_list_t *ptThis = (number_list_t *)pTarget;

    arm_2d_canvas(ptTile, __canvas) {

        arm_2d_align_centre(__canvas, c_tileListCoverMask.tRegion.tSize) {
            arm_2d_fill_colour_with_mask(   ptTile, 
                                            &__centre_region, 
                                            &c_tileListCoverMask, 
                                            (__arm_2d_color_t){GLCD_COLOR_BLACK});
        }
    }

    arm_2d_op_wait_async(NULL);
    
    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_fitness_t *__arm_2d_scene_fitness_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_fitness_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

#if __FITNESS_CFG_NEBULA_ENABLE__
    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

        /* a dirty region to be specified at runtime*/
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            0  /* initialize at runtime later */
        ),

    END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    /* initialise dirty region 0 at runtime
     * this demo shows that we create a region in the centre of a screen(320*240)
     * for a image stored in the tile c_tileCMSISLogoMask
     */
    arm_2d_align_centre(tScreen, 240, 240) {
        s_tDirtyRegions[0].tRegion = __centre_region;
    }
#else
    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

        /* a dirty region to be specified at runtime*/
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            0  /* initialize at runtime later */
        ),

    END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    /* initialise dirty region 0 at runtime
     * this demo shows that we create a region in the centre of a screen(320*240)
     * for a image stored in the tile c_tileCMSISLogoMask
     */
    arm_2d_align_centre(tScreen, 84, 80) {
        s_tDirtyRegions[0].tRegion = __centre_region;
    }

#endif

    if (NULL == ptThis) {
        ptThis = (user_scene_fitness_t *)malloc(sizeof(user_scene_fitness_t));
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_fitness_t));

    *ptThis = (user_scene_fitness_t){
        .use_as__arm_2d_scene_t = {
            /* Please uncommon the callbacks if you need them
             */
            .fnScene        = &__pfb_draw_scene_fitness_handler,
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_fitness_background_start,
            //.fnOnBGComplete = &__on_scene_fitness_background_complete,
            .fnOnFrameStart = &__on_scene_fitness_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_fitness_switching_out,
            .fnOnFrameCPL   = &__on_scene_fitness_frame_complete,
            .fnDepose       = &__on_scene_fitness_depose,
        },
        .bUserAllocated = bUserAllocated,
    };

    progress_wheel_init(&this.tWheel, 
                        240, 
                        GLCD_COLOR_GREEN,
                        GLCD_COLOR_WHITE,
                        &this.use_as__arm_2d_scene_t.ptDirtyRegion);


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
        };
        number_list_init(&this.tNumberList[2], &tCFG);
    } while(0);
    
    numer_list_move_selection(&this.tNumberList[0], 100,   10000);
    numer_list_move_selection(&this.tNumberList[1], 10,    10000);
    numer_list_move_selection(&this.tNumberList[2], 1,     10000);

#if __FITNESS_CFG_NEBULA_ENABLE__
    do {
        dynamic_nebula_cfg_t tCFG = {
            .fSpeed = 1.0f,
            .iRadius = 90,
            .iVisibleRingWidth = 50,
            .hwParticleCount = dimof(this.tParticles),
            .ptParticles = this.tParticles,
        };
        dynamic_nebula_init(&this.tNebula, &tCFG);
    } while(0);
#endif

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


