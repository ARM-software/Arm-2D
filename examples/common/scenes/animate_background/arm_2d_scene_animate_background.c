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
#include "arm_2d_disp_adapters.h"

#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__

#define __USER_SCENE_ANIMATE_BACKGROUND_IMPLEMENT__
#include "arm_2d_scene_animate_background.h"

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
extern const arm_2d_vres_t c_vrFire;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)
    
    /* add the last region:
        * it is the top left corner for text display 
        */
    ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
        .tSize = {
            .iWidth = 80,
            .iHeight = 160,
        },
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_animate_background_load(arm_2d_scene_t *ptScene)
{
    user_scene_animate_background_t *ptThis = (user_scene_animate_background_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __after_scene_animate_background_switching(arm_2d_scene_t *ptScene)
{
    user_scene_animate_background_t *ptThis = (user_scene_animate_background_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_animate_background_depose(arm_2d_scene_t *ptScene)
{
    user_scene_animate_background_t *ptThis = (user_scene_animate_background_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    

    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    ptScene->ptPlayer = NULL;

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene animate_background                                                   *
 *----------------------------------------------------------------------------*/

static void __on_scene_animate_background_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_animate_background_t *ptThis = (user_scene_animate_background_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_animate_background_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_animate_background_t *ptThis = (user_scene_animate_background_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_animate_background_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_animate_background_t *ptThis = (user_scene_animate_background_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    if (arm_2d_helper_is_time_out(  this.Resource.tileFireFilm.hwPeriodPerFrame, 
                                    &this.lTimestamp[1])) {

        arm_2d_helper_film_next_frame(&this.Resource.tileFireFilm);
    }

}

static void __on_scene_animate_background_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_animate_background_t *ptThis = (user_scene_animate_background_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if 0
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(3000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene_animate_background_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_animate_background_t *ptThis = (user_scene_animate_background_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_animate_background_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_animate_background_t *ptThis = (user_scene_animate_background_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

        arm_2d_align_centre(__top_canvas, 80, 160 ) {

            arm_2d_tile_copy_only(
                (arm_2d_tile_t *)&this.Resource.tileFireFilm,
                ptTile,
                &__centre_region
            );
            
            ARM_2D_OP_WAIT_ASYNC();                      
        }

        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene animate_background");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_animate_background_t *__arm_2d_scene_animate_background_init(   
                                        arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_animate_background_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    /* initialise dirty region 0 at runtime
     * this demo shows that we create a region in the centre of a screen(320*240)
     * for a image stored in the tile c_tileCMSISLogoMask
     */
    arm_2d_align_centre(tScreen, s_tDirtyRegions[0].tRegion.tSize) {
        s_tDirtyRegions[0].tRegion = __centre_region;
    }

    if (NULL == ptThis) {
        ptThis = (user_scene_animate_background_t *)
                    __arm_2d_allocate_scratch_memory(   
                                sizeof(user_scene_animate_background_t),
                                __alignof__(user_scene_animate_background_t),
                                ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_animate_background_t));

    *ptThis = (user_scene_animate_background_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_animate_background_load,
            //.fnAfterSwitch  = &__after_scene_animate_background_switching,

            .fnScene        = &__pfb_draw_scene_animate_background_handler,
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_animate_background_background_start,
            //.fnOnBGComplete = &__on_scene_animate_background_background_complete,
            .fnOnFrameStart = &__on_scene_animate_background_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_animate_background_switching_out,
            .fnOnFrameCPL   = &__on_scene_animate_background_frame_complete,
            .fnDepose       = &__on_scene_animate_background_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_animate_background_t begin ---------------*/

    /* initialize the vrFire with const value */
    this.Resource.vrFire = c_vrFire;

    /* initialize the arm_2d_helper_film_t object */
    this.Resource.tileFireFilm = (arm_2d_helper_film_t)
        impl_film(  this.Resource.vrFire, 
                    80, 
                    160, 
                    8, 
                    32, 
                    33);

    arm_2d_helper_film_set_frame(&this.Resource.tileFireFilm, -1);

    /* ------------   initialize members of user_scene_animate_background_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

#endif


