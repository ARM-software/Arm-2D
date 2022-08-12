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

#ifdef __RTE_ACCELERATION_ARM_2D_SCENE%Instance%__

#include "arm_2d_scene_%Instance%.h"
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
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
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
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


static void __on_scene%Instance%_depose(arm_2d_scene_t *ptScene)
{
    ptScene->ptPlayer = NULL;
    free(ptScene);
}

/*----------------------------------------------------------------------------*
 * Scene %Instance%                                                           *
 *----------------------------------------------------------------------------*/

static void __on_scene%Instance%_background_start(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
}

static void __on_scene%Instance%_background_complete(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
}


static void __on_scene%Instance%_frame_start(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
}

static void __on_scene%Instance%_frame_complete(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
    
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(3000)) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}




static
IMPL_PFB_ON_DRAW(__pfb_draw_scene%Instance%_background)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    /*-----------------------draw back ground begin-----------------------*/



    /*-----------------------draw back ground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene%Instance%)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    /*-----------------------draw the foreground begin-----------------------*/
    
    /* following code is just a demo, you can remove them */
    
    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

#if 0
    /* draw the cmsis logo in the centre of the screen */
    arm_2d_align_centre(ptTile->tRegion, c_tileCMSISLogo.tRegion.tSize) {
        arm_2d_tile_copy_with_src_mask( &c_tileCMSISLogo,
                                        &c_tileCMSISLogoMask,
                                        ptTile,
                                        &__centre_region,
                                        ARM_2D_CP_MODE_COPY);
    }
#else
    /* draw the cmsis logo using mask in the centre of the screen */
    arm_2d_align_centre(ptTile->tRegion, c_tileCMSISLogo.tRegion.tSize) {
        arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTile, 
                                            &__centre_region, 
                                            &c_tileCMSISLogoMask, 
                                            (__arm_2d_color_t){GLCD_COLOR_BLACK},
                                            64);
    }
#endif

    /* draw text at the top-left corner */
    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
    arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
    arm_lcd_text_location(0,0);
    arm_lcd_puts("Scene %Instance%");

    /*-----------------------draw the foreground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

void arm_2d_scene%Instance%_init(arm_2d_scene_player_t *ptDispAdapter)
{
    assert(NULL != ptDispAdapter);

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
                .iWidth = 320,
                .iHeight = 8,
            },
        ),

    END_IMPL_ARM_2D_REGION_LIST()
    
    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    /* initialise dirty region 0 at runtime
     * this demo shows that we create a region in the centre of a screen(320*240)
     * for a image stored in the tile c_tileCMSISLogoMask
     */
    s_tDirtyRegions[0].tRegion.tLocation = (arm_2d_location_t){
        .iX = ((tScreen.tSize.iWidth - c_tileCMSISLogoMask.tRegion.tSize.iWidth) >> 1),
        .iY = ((tScreen.tSize.iHeight - c_tileCMSISLogoMask.tRegion.tSize.iHeight) >> 1),
    };
    s_tDirtyRegions[0].tRegion.tSize = c_tileCMSISLogoMask.tRegion.tSize;
    
    
    arm_2d_scene_t *ptScene = (arm_2d_scene_t *)malloc(sizeof(arm_2d_scene_t));
    assert(NULL != ptScene);
    
    *ptScene = (arm_2d_scene_t){
        
        /* If you don't need dirty region list, please comment the following line */
        .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
        
        /* Please uncommon the callbacks if you need them
         */
        //.fnOnBGStart    = &__on_scene%Instance%_background_start,
        //.fnBackground   = &__pfb_draw_scene%Instance%_background,
        //.fnOnBGComplete = &__on_scene%Instance%_background_complete,
        //.fnOnFrameStart = &__on_scene%Instance%_frame_start,
        .fnScene        = &__pfb_draw_scene%Instance%,
        .fnOnFrameCPL   = &__on_scene%Instance%_frame_complete,
        .fnDepose       = &__on_scene%Instance%_depose,
    };
    arm_2d_scene_player_append_scenes( ptDispAdapter, ptScene, 1);
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

