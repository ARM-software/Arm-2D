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

#define __USER_SCENE_PROGRESS_STATUS_IMPLEMENT__
#include "arm_2d_scene_progress_status.h"

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
#   pragma GCC diagnostic ignored "-Wunused-value"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileCMSISLogo          c_tileCMSISLogoGRAY8
#   define c_tileWIFISignal         c_tileWIFISignalGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565
#   define c_tileWIFISignal         c_tileWIFISignalRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#   define c_tileWIFISignal         c_tileWIFISignalCCCA8888

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

extern const arm_2d_tile_t c_tileWIFISignal;
extern const arm_2d_tile_t c_tileWIFISignalMask;

static arm_2d_helper_film_t s_tileWIFISignalFilm = 
    impl_film(c_tileWIFISignal, 32, 32, 6, 36, 33);


static arm_2d_helper_film_t s_tileWIFISignalFilmMask = 
    impl_film(c_tileWIFISignalMask, 32, 32, 6, 36, 33);


/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


static void __on_scene_progress_status_depose(arm_2d_scene_t *ptScene)
{
    user_scene_progress_status_t *ptThis = (user_scene_progress_status_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene 1                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_progress_status_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_progress_status_t *ptThis = (user_scene_progress_status_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_progress_status_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_progress_status_t *ptThis = (user_scene_progress_status_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_progress_status_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_progress_status_t *ptThis = (user_scene_progress_status_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    if (arm_2d_helper_is_time_out(  s_tileWIFISignalFilm.hwPeriodPerFrame, 
                                    &this.lTimestamp[2])) {

        arm_2d_helper_film_next_frame(&s_tileWIFISignalFilm);
        arm_2d_helper_film_next_frame(&s_tileWIFISignalFilmMask);
    }
}

static void __on_scene_progress_status_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_progress_status_t *ptThis = (user_scene_progress_status_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}




static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_progress_status_background_handler)
{
    user_scene_progress_status_t *ptThis = (user_scene_progress_status_t *)pTarget;
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    /*-----------------------draw back ground begin-----------------------*/



    /*-----------------------draw back ground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}



static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_progress_status_handler)
{
    user_scene_progress_status_t *ptThis = (user_scene_progress_status_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(tScreenSize);

    /*-----------------------draw the foreground begin-----------------------*/

    /* following code is just a demo, you can remove them */
    arm_2d_canvas(ptTile, __canvas) {

        if (bIsNewFrame) {
            int32_t iResult;
            if (arm_2d_helper_time_half_cos_slider(0, 1000, 6000, &iResult, &this.lTimestamp[1])) {
                this.hwProgress = -1;
            } else {
                this.hwProgress = (uint16_t)iResult;
            }
        }
        arm_using(arm_2d_size_t tWiFiLogoSize = s_tileWIFISignalFilm .use_as__arm_2d_tile_t.tRegion .tSize) {

            arm_2d_align_centre(__canvas, 
                                tScreenSize.iWidth, 
                                120+tWiFiLogoSize.iHeight) {
                
                arm_2d_layout(__centre_region, true) {
                    __item_line_vertical(__centre_region.tSize.iWidth, tWiFiLogoSize.iHeight) {
                        arm_2d_align_centre(__item_region, tWiFiLogoSize) {
                            
                            arm_2d_tile_copy_with_src_mask_only(
                                    (arm_2d_tile_t *)&s_tileWIFISignalFilm,
                                    (arm_2d_tile_t *)&s_tileWIFISignalFilmMask,
                                    ptTile,
                                    &__centre_region);

                        }
                    }
                    __item_line_vertical(tScreenSize.iWidth, 40) {
                        arm_2d_container(ptTile, __progress_bar, &__item_region) {
                            progress_bar_simple_show(&__progress_bar, NULL, this.hwProgress, bIsNewFrame);
                        }
                    }
                    __item_line_vertical(tScreenSize.iWidth, 40) {
                        arm_2d_container(ptTile, __progress_bar, &__item_region) {
                            progress_bar_drill_show(&__progress_bar, NULL, this.hwProgress, bIsNewFrame);
                        }
                    }
                    __item_line_vertical(tScreenSize.iWidth, 40) {
                        arm_2d_container(ptTile, __progress_bar, &__item_region) {
                            progress_bar_flowing_show(&__progress_bar, NULL, this.hwProgress, bIsNewFrame);
                        }
                    }
                }
            }
        }
/*
        arm_2d_align_top_right( __canvas, 
                                s_tileWIFISignalFilm
                                    .use_as__arm_2d_tile_t
                                        .tRegion
                                            .tSize) {

            arm_2d_tile_copy_with_src_mask_only(
                                    (arm_2d_tile_t *)&s_tileWIFISignalFilm,
                                    (arm_2d_tile_t *)&s_tileWIFISignalFilmMask,
                                    ptTile,
                                    &__top_right_region);

        }
*/
        /* draw text at the top-left corner */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_printf("Scene Progress and Status");
    }

    /*-----------------------draw the foreground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

#define PROGRESSBAR_WIDTH       (__DISP0_CFG_SCEEN_WIDTH__ * 3 >> 3)

ARM_NONNULL(1)
user_scene_progress_status_t *__arm_2d_scene_progress_status_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_progress_status_t *ptScene)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

        /* a dirty region to be specified at runtime*/
        ADD_REGION_TO_LIST(s_tDirtyRegions),

        ADD_REGION_TO_LIST(s_tDirtyRegions),

       /* add the last region:
         * it is the top left corner for text display 
         */
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {
                .iX = 0,
                .iY = 0,
            },
            .tSize = {
                .iWidth = __GLCD_CFG_SCEEN_WIDTH__,
                .iHeight = 8,
            },
        ),

    END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    if (NULL == ptScene) {
        ptScene = (user_scene_progress_status_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_progress_status_t),
                                                        __alignof__(user_scene_progress_status_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptScene);
        if (NULL == ptScene) {
            return NULL;
        }
        bUserAllocated = true;
    } else {
        memset(ptScene, 0, sizeof(user_scene_progress_status_t));
    }

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);

    /* initialise dirty region 0 at runtime
     */
    arm_2d_align_centre(tScreen, tScreen.tSize.iWidth, 120) {
        s_tDirtyRegions[0].tRegion = __centre_region;
    }

    /* initialise dirty region 1 at runtime
     */
    arm_2d_align_top_right( tScreen, 
                            s_tileWIFISignalFilm
                                .use_as__arm_2d_tile_t
                                    .tRegion
                                        .tSize) {
        s_tDirtyRegions[1].tRegion = __top_right_region;
    }

    arm_using(arm_2d_size_t tWiFiLogoSize = s_tileWIFISignalFilm .use_as__arm_2d_tile_t.tRegion .tSize) {

        arm_2d_align_centre(tScreen, 
                            tScreen.tSize.iWidth, 
                            120+tWiFiLogoSize.iHeight) {
            
            arm_2d_layout(__centre_region) {
                __item_line_vertical(__centre_region.tSize.iWidth, tWiFiLogoSize.iHeight) {
                    arm_2d_align_centre(__item_region, tWiFiLogoSize) {
                        s_tDirtyRegions[1].tRegion = __centre_region;
                    }
                }
                __item_line_vertical(tScreen.tSize.iWidth, 120) {
                    s_tDirtyRegions[0].tRegion = __item_region;
                }
            }
        }
    }

    /* set to the last frame */
    arm_2d_helper_film_set_frame(&s_tileWIFISignalFilm, -1);
    arm_2d_helper_film_set_frame(&s_tileWIFISignalFilmMask, -1);


    *ptScene = (user_scene_progress_status_t){
        .use_as__arm_2d_scene_t = {
        
        /* the canvas colour */
        .tCanvas = {GLCD_COLOR_WHITE}, 
        
        /* Please uncommon the callbacks if you need them
         */
        //.fnBackground   = &__pfb_draw_scene_progress_status_background_handler,
        .fnScene        = &__pfb_draw_scene_progress_status_handler,
        .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
        

        //.fnOnBGStart    = &__on_scene_progress_status_background_start,
        //.fnOnBGComplete = &__on_scene_progress_status_background_complete,
        .fnOnFrameStart = &__on_scene_progress_status_frame_start,
        .fnOnFrameCPL   = &__on_scene_progress_status_frame_complete,
        .fnDepose       = &__on_scene_progress_status_depose,
        },
        .bUserAllocated = bUserAllocated,
    };

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &ptScene->use_as__arm_2d_scene_t, 
                                        1);

    return ptScene;
}




#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

