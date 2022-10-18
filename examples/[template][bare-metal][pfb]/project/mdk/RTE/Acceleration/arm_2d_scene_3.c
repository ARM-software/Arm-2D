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

#ifdef __RTE_ACCELERATION_ARM_2D_SCENE3__

#define __USER_SCENE3_IMPLEMENT__
#include "arm_2d_scene_3.h"

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
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/

typedef struct my_list_item_t {
    implement(arm_2d_list_item_t);
    
    /* you can put your own members here */
} my_list_item_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static 
arm_fsm_rt_t __list_view_item_0_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam);

static 
arm_fsm_rt_t __list_view_item_1_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam);

static 
arm_fsm_rt_t __list_view_item_2_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam);

static 
arm_fsm_rt_t __list_view_item_3_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam);

/*============================ LOCAL VARIABLES ===============================*/

enum {
    MY_LIST_ITEM_ID_0,
    MY_LIST_ITEM_ID_1,
    MY_LIST_ITEM_ID_2,
    MY_LIST_ITEM_ID_3,
};

const static
my_list_item_t s_tListArray[] = {
    [MY_LIST_ITEM_ID_0] = {
        .use_as__arm_2d_list_item_t = {
            .hwID = MY_LIST_ITEM_ID_0,
            .bIsEnabled = true,
            .bIsVisible = true,
            .bIsReadOnly = true,    /* indicate this is a const item */
            .Padding = {
                5,5,
            },
            
            .tSize = { 120, 100 },
            .fnOnDrawItem = &__list_view_item_0_draw_item,
        },
    },

    [MY_LIST_ITEM_ID_1] = {
        .use_as__arm_2d_list_item_t = {
            .hwID = MY_LIST_ITEM_ID_1,
            .bIsEnabled = true,
            .bIsVisible = true,
            .bIsReadOnly = true,    /* indicate this is a const item */
            
            .Padding = {
                5,5,
            },
            .tSize = { 80, 100 },
            .fnOnDrawItem = &__list_view_item_1_draw_item,
        },
    },

    [MY_LIST_ITEM_ID_2] = {
        .use_as__arm_2d_list_item_t = {
            .hwID = MY_LIST_ITEM_ID_2,
            .bIsEnabled = true,
            .bIsVisible = true,
            .bIsReadOnly = true,    /* indicate this is a const item */

            .Padding = {
                5,5,
            },
            .tSize = { 100, 100 },
            .fnOnDrawItem = &__list_view_item_2_draw_item,
        },
    },


    [MY_LIST_ITEM_ID_3] = {
        .use_as__arm_2d_list_item_t = {
            .hwID = MY_LIST_ITEM_ID_3,
            .bIsEnabled = true,
            .bIsVisible = true,
            .bIsReadOnly = true,    /* indicate this is a const item */

            .Padding = {
                5,5,
            },
            .tSize = { 60, 100 },
            .fnOnDrawItem = &__list_view_item_3_draw_item,
        },
    },
};


/*============================ IMPLEMENTATION ================================*/


static 
arm_fsm_rt_t __list_view_item_0_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    my_list_item_t *ptThis = (my_list_item_t *)ptItem;
    ARM_2D_UNUSED(ptItem);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    draw_round_corner_box(ptTile, NULL, GLCD_COLOR_BLUE, 64, bIsNewFrame);

    arm_2d_align_centre(ptTile->tRegion, 
                        ARM_2D_FONT_A4_DIGITS_ONLY.use_as__arm_2d_font_t.tCharSize) {
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
        arm_lcd_text_set_draw_region(&__centre_region);
        arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
        arm_lcd_puts("0");
    }

    
    return arm_fsm_rt_cpl;
}


static 
arm_fsm_rt_t __list_view_item_1_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    my_list_item_t *ptThis = (my_list_item_t *)ptItem;
    ARM_2D_UNUSED(ptItem);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    draw_round_corner_box(ptTile, NULL, GLCD_COLOR_RED, 64, bIsNewFrame);
    
    spinning_wheel2_show(ptTile, GLCD_COLOR_GREEN, bIsNewFrame);

    arm_2d_align_centre(ptTile->tRegion, 
                        ARM_2D_FONT_A4_DIGITS_ONLY.use_as__arm_2d_font_t.tCharSize) {
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
        arm_lcd_text_set_draw_region(&__centre_region);
        arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
        arm_lcd_puts("1");
    }
    
    return arm_fsm_rt_cpl;
}

static 
arm_fsm_rt_t __list_view_item_2_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    my_list_item_t *ptThis = (my_list_item_t *)ptItem;
    ARM_2D_UNUSED(ptItem);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);
    
    draw_round_corner_box(ptTile, NULL, GLCD_COLOR_YELLOW, 64, bIsNewFrame);

    busy_wheel_show(ptTile, bIsNewFrame);

    arm_2d_align_centre(ptTile->tRegion, 
                        ARM_2D_FONT_A4_DIGITS_ONLY.use_as__arm_2d_font_t.tCharSize) {
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
        arm_lcd_text_set_draw_region(&__centre_region);
        arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
        arm_lcd_puts("2");
    }


    return arm_fsm_rt_cpl;
}

static 
arm_fsm_rt_t __list_view_item_3_draw_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    my_list_item_t *ptThis = (my_list_item_t *)ptItem;
    ARM_2D_UNUSED(ptItem);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    draw_round_corner_box(ptTile, NULL, GLCD_COLOR_BLACK, 64, bIsNewFrame);

    busy_wheel2_show(ptTile, bIsNewFrame);

    arm_2d_align_centre(ptTile->tRegion, 
                        ARM_2D_FONT_A4_DIGITS_ONLY.use_as__arm_2d_font_t.tCharSize) {
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
        arm_lcd_text_set_draw_region(&__centre_region);
        arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_BLACK);
        arm_lcd_puts("3");
    }
    
    return arm_fsm_rt_cpl;
}


static void __on_scene3_depose(arm_2d_scene_t *ptScene)
{
    user_scene_3_t *ptThis = (user_scene_3_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    /* reset timestamp */
    this.lTimestamp = 0;

    if (this.bUserAllocated) {
        free(ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene 3                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene3_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_3_t *ptThis = (user_scene_3_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene3_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_3_t *ptThis = (user_scene_3_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene3_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_3_t *ptThis = (user_scene_3_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene3_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_3_t *ptThis = (user_scene_3_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /* switch to next scene after 10s */
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp)) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene3_background_handler)
{
    user_scene_3_t *ptThis = (user_scene_3_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    /*-----------------------draw back ground begin-----------------------*/



    /*-----------------------draw back ground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene3_handler)
{
    user_scene_3_t *ptThis = (user_scene_3_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    /*-----------------------draw the foreground begin-----------------------*/
    
    /* following code is just a demo, you can remove them */
    
    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

    while(arm_fsm_rt_cpl != list_view_show(&this.tListView, ptTile, NULL, bIsNewFrame));

    /* draw text at the top-left corner */
    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
    arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
    arm_lcd_text_set_draw_region(NULL);
    arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
    arm_lcd_text_location(0,0);
    arm_lcd_puts("Scene 3");

    /*-----------------------draw the foreground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_3_t *__arm_2d_scene3_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_3_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);
    
    if (NULL == ptThis) {
        ptThis = (user_scene_3_t *)malloc(sizeof(user_scene_3_t));
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
        bUserAllocated = true;
    } else {
        memset(ptThis, 0, sizeof(user_scene_3_t));
    }
    
    *ptThis = (user_scene_3_t){
        .use_as__arm_2d_scene_t = {
        /* Please uncommon the callbacks if you need them
         */
        //.fnBackground   = &__pfb_draw_scene3_background_handler,
        .fnScene        = &__pfb_draw_scene3_handler,
        //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
        

        //.fnOnBGStart    = &__on_scene3_background_start,
        //.fnOnBGComplete = &__on_scene3_background_complete,
        //.fnOnFrameStart = &__on_scene3_frame_start,
        .fnOnFrameCPL   = &__on_scene3_frame_complete,
        .fnDepose       = &__on_scene3_depose,
        },
        .bUserAllocated = bUserAllocated,
    };


    do {
        list_view_cfg_t tCFG = {
            .fnIterator = &ARM_2D_LIST_ITERATOR_ARRAY,
            .fnCalculator = &ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_HORIZONTAL,
            
            .tListSize = {0, 100},
            .ptItems = (arm_2d_list_item_t *)s_tListArray,
            .hwCount = dimof(s_tListArray),
            .hwItemSizeInByte = sizeof(my_list_item_t),
            
            .bIgnoreBackground = true,
        };
        
        list_view_init(&this.tListView, &tCFG);
    } while(0);


    list_view_move_selection(&this.tListView, 12, 13000);

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}




#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

