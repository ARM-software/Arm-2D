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

#define __USER_SCENE%Instance%_IMPLEMENT__
#include "arm_2d_scene_%Instance%.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#ifdef RTE_Acceleration_Arm_2D_Scene%Instance%

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

/*============================ IMPLEMENTATION ================================*/

static void __on_scene%Instance%_load(arm_2d_scene_t *ptScene)
{
    user_scene_%Instance%_t *ptThis = (user_scene_%Instance%_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __after_scene%Instance%_switching(arm_2d_scene_t *ptScene)
{
    user_scene_%Instance%_t *ptThis = (user_scene_%Instance%_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene%Instance%_depose(arm_2d_scene_t *ptScene)
{
    user_scene_%Instance%_t *ptThis = (user_scene_%Instance%_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /*--------------------- insert your depose code begin --------------------*/
    

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
 * Scene %Instance%                                                                    *
 *----------------------------------------------------------------------------*/

#if 0 /* deprecated */
static void __on_scene%Instance%_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_%Instance%_t *ptThis = (user_scene_%Instance%_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene%Instance%_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_%Instance%_t *ptThis = (user_scene_%Instance%_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}
#endif


static void __on_scene%Instance%_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_%Instance%_t *ptThis = (user_scene_%Instance%_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene%Instance%_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_%Instance%_t *ptThis = (user_scene_%Instance%_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if 0
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(3000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene%Instance%_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_%Instance%_t *ptThis = (user_scene_%Instance%_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene%Instance%_handler)
{
    user_scene_%Instance%_t *ptThis = (user_scene_%Instance%_t *)pTarget;

    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/
        
        /* following code is just a demo, you can remove them */
        arm_2d_align_centre(__top_canvas, 200, 100 ) {
            draw_round_corner_box(  ptTile, 
                                    &__centre_region, 
                                    GLCD_COLOR_WHITE, 
                                    255);
            
            ARM_2D_OP_WAIT_ASYNC();
            
            draw_round_corner_border(   ptTile, 
                                        &__centre_region, 
                                        GLCD_COLOR_BLACK, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});
                                    
        }


    #if 0
        /* draw the cmsis logo in the centre of the screen */
        arm_2d_align_centre(__top_canvas, c_tileCMSISLogo.tRegion.tSize) {
            arm_2d_tile_copy_with_src_mask( &c_tileCMSISLogo,
                                            &c_tileCMSISLogoMask,
                                            ptTile,
                                            &__centre_region,
                                            ARM_2D_CP_MODE_COPY);
        }
    #else
        /* draw the cmsis logo using mask in the centre of the screen */
        arm_2d_align_centre(__top_canvas, c_tileCMSISLogo.tRegion.tSize) {
            arm_2d_fill_colour_with_a4_mask_and_opacity(   
                                                ptTile, 
                                                &__centre_region, 
                                                &c_tileCMSISLogoA4Mask, 
                                                (__arm_2d_color_t){GLCD_COLOR_BLACK},
                                                128);
        }
    #endif

        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene %Instance%");

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_%Instance%_t *__arm_2d_scene%Instance%_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_%Instance%_t *ptThis)
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
        ptThis = (user_scene_%Instance%_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_%Instance%_t),
                                                        __alignof__(user_scene_%Instance%_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }
    memset(ptThis, 0, sizeof(user_scene_%Instance%_t));

    *ptThis = (user_scene_%Instance%_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_WHITE}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene%Instance%_load,
            .fnScene        = &__pfb_draw_scene%Instance%_handler,
            .fnAfterSwitch  = &__after_scene%Instance%_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene%Instance%_background_start,         /* deprecated */
            //.fnOnBGComplete = &__on_scene%Instance%_background_complete,      /* deprecated */
            .fnOnFrameStart = &__on_scene%Instance%_frame_start,
            .fnBeforeSwitchOut = &__before_scene%Instance%_switching_out,
            .fnOnFrameCPL   = &__on_scene%Instance%_frame_complete,
            .fnDepose       = &__on_scene%Instance%_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_%Instance%_t begin ---------------*/


    /* ------------   initialize members of user_scene_%Instance%_t end   ---------------*/

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

