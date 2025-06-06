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

#ifdef RTE_Acceleration_Arm_2D_Scene5

#define __USER_SCENE5_IMPLEMENT__
#include "arm_2d_scene_5.h"

#include "arm_2d_helper.h"
#include "arm_2d_example_controls.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

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
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#   pragma GCC diagnostic ignored "-Wunused-value"
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe177,Pe174
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
    int16_t iOffset;
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

static
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
            //.u4Alignment = ARM_2D_ALIGN_TOP,
            .tSize = { 100, 200 },
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
            .tSize = { 100, 200 },
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
            //.u4Alignment = ARM_2D_ALIGN_BOTTOM,
            .tSize = { 100, 200 },
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
            .tSize = { 100, 200 },
            .fnOnDrawItem = &__list_view_item_3_draw_item,
        },
    },
};

/*============================ IMPLEMENTATION ================================*/


#define ITEM_BG_OPACITY     (255)

#define RADIUS      800.0f

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

    uint8_t chOpacity = arm_2d_helper_alpha_mix(ITEM_BG_OPACITY, ptParam->chOpacity);
    int32_t q7ScaleRatio = ((ptParam->chOpacity >> 1) + 128);
    /* re-use opacity (0~255) as scaling ratio */
    int32_t nSize = 100 * q7ScaleRatio >> 8;

    arm_2d_canvas(ptTile, __canvas) {
        arm_2d_align_top_centre(__canvas, nSize, nSize) {

            /* adjust item position around a curve*/
            do {
                float fYOffset =  (float)ptParam->hwRatio;
                fYOffset = RADIUS - sqrt(RADIUS * RADIUS - fYOffset * fYOffset);

                __top_centre_region.tLocation.iY += (int16_t) fYOffset;
            } while(0);
            
            draw_round_corner_box(ptTile, &__top_centre_region, GLCD_COLOR_WHITE, chOpacity);

            arm_lcd_text_set_target_framebuffer(ptTile);
            arm_lcd_text_set_colour(__RGB(0x94, 0xd2, 0x52), GLCD_COLOR_BLACK);
            arm_lcd_text_set_opacity(chOpacity);
            arm_lcd_text_set_scale((float)q7ScaleRatio / 256.0f);
            arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A8_DIGITS_ONLY);
            arm_print_banner("0", __top_centre_region);
            arm_lcd_text_set_opacity(255);
        }
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
    user_scene_5_t *ptScene = (user_scene_5_t *)ptItem->pTarget;
    ARM_2D_UNUSED(ptItem);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    uint8_t chOpacity = arm_2d_helper_alpha_mix(ITEM_BG_OPACITY, ptParam->chOpacity);

    int32_t q7ScaleRatio = ((ptParam->chOpacity >> 1) + 128);
    /* re-use opacity (0~255) as scaling ratio */
    int32_t nSize = 100 * q7ScaleRatio >> 8;

    if (bIsNewFrame) {
        progress_wheel_set_diameter(&ptScene->tWheel, 60 * q7ScaleRatio >> 8);
    }

    arm_2d_canvas(ptTile, __canvas) {
        arm_2d_align_top_centre(__canvas, nSize, nSize) {

            /* adjust item position around a curve*/
            do {
                float fYOffset =  (float)ptParam->hwRatio;
                fYOffset = RADIUS - sqrt(RADIUS * RADIUS - fYOffset * fYOffset);

                __top_centre_region.tLocation.iY += (int16_t) fYOffset;
            } while(0);

            draw_round_corner_box(ptTile, &__top_centre_region, GLCD_COLOR_WHITE, chOpacity);

            progress_wheel_show(&ptScene->tWheel, 
                                ptTile, 
                                &__top_centre_region, 
                                ptScene->iProgress,   /* progress 0~1000 */
                                chOpacity, 
                                bIsNewFrame);
        
            arm_lcd_text_set_target_framebuffer(ptTile);
            arm_lcd_text_set_colour(__RGB(0x94, 0xd2, 0x52), GLCD_COLOR_BLACK);
            arm_lcd_text_set_opacity(chOpacity);
            arm_lcd_text_set_scale((float)q7ScaleRatio / 256.0f);
            arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A8_DIGITS_ONLY);
            arm_print_banner("1", __top_centre_region);
            arm_lcd_text_set_opacity(255);
        }
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
    
    uint8_t chOpacity = arm_2d_helper_alpha_mix(ITEM_BG_OPACITY, ptParam->chOpacity);

    int32_t q7ScaleRatio = ((ptParam->chOpacity >> 1) + 128);
    /* re-use opacity (0~255) as scaling ratio */
    int32_t nSize = 100 * q7ScaleRatio >> 8;

    arm_2d_canvas(ptTile, __canvas) {
        arm_2d_align_top_centre(__canvas, nSize, nSize) {

            /* adjust item position around a curve*/
            do {
                float fYOffset =  (float)ptParam->hwRatio;
                fYOffset = RADIUS - sqrt(RADIUS * RADIUS - fYOffset * fYOffset);

                __top_centre_region.tLocation.iY += (int16_t) fYOffset;
            } while(0);

            draw_round_corner_box(ptTile, &__top_centre_region, GLCD_COLOR_WHITE, chOpacity);
        
            arm_lcd_text_set_target_framebuffer(ptTile);
            arm_lcd_text_set_colour(__RGB(0x94, 0xd2, 0x52), GLCD_COLOR_BLACK);
            arm_lcd_text_set_opacity(chOpacity);
            arm_lcd_text_set_scale((float)q7ScaleRatio / 256.0f);
            arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A8_DIGITS_ONLY);
            arm_print_banner("2", __top_centre_region);
            arm_lcd_text_set_opacity(255);
        }
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

    uint8_t chOpacity = arm_2d_helper_alpha_mix(ITEM_BG_OPACITY, ptParam->chOpacity);

    int32_t q7ScaleRatio = ((ptParam->chOpacity >> 1) + 128);
    /* re-use opacity (0~255) as scaling ratio */
    int32_t nSize = 100 * q7ScaleRatio >> 8;

    arm_2d_canvas(ptTile, __canvas) {
        arm_2d_align_top_centre(__canvas, nSize, nSize) {

            /* adjust item position around a curve*/
            do {
                float fYOffset =  (float)ptParam->hwRatio;
                fYOffset = RADIUS - sqrt(RADIUS * RADIUS - fYOffset * fYOffset);

                __top_centre_region.tLocation.iY += (int16_t) fYOffset;
            } while(0);


            draw_round_corner_box(ptTile, &__top_centre_region, GLCD_COLOR_WHITE, chOpacity);

            arm_lcd_text_set_target_framebuffer(ptTile);
            arm_lcd_text_set_colour(__RGB(0x94, 0xd2, 0x52), GLCD_COLOR_BLACK);
            arm_lcd_text_set_opacity(chOpacity);
            arm_lcd_text_set_scale((float)q7ScaleRatio / 256.0f);
            arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A8_DIGITS_ONLY);
            arm_print_banner("3", __top_centre_region);
            arm_lcd_text_set_opacity(255);
        }
    }
    
    return arm_fsm_rt_cpl;
}


static void __on_scene5_depose(arm_2d_scene_t *ptScene)
{
    user_scene_5_t *ptThis = (user_scene_5_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }
    
    progress_wheel_depose(&this.tWheel);

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene 3                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene5_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_5_t *ptThis = (user_scene_5_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene5_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_5_t *ptThis = (user_scene_5_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene5_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_5_t *ptThis = (user_scene_5_t *)ptScene;

    int nResult;
    if (arm_2d_helper_time_liner_slider(0, 1000, 10000, &nResult, &this.lTimestamp[2])) {
        this.lTimestamp[2] = 0;
    }
    this.iProgress = (int16_t)nResult;

    progress_wheel_on_frame_start(&this.tWheel);
}

static void __on_scene5_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_5_t *ptThis = (user_scene_5_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /* switch to next scene after 10s */
    if (arm_2d_helper_is_time_out(12000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
    
    if (arm_2d_helper_is_time_out(1500, &this.lTimestamp[1])) {
        list_view_move_selection(&this.tListView, 1, 300);
    }
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene5_background_handler)
{
    user_scene_5_t *ptThis = (user_scene_5_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    /*-----------------------draw back ground begin-----------------------*/



    /*-----------------------draw back ground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene5_handler)
{
    user_scene_5_t *ptThis = (user_scene_5_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    /*-----------------------draw the foreground begin-----------------------*/
    
    /* following code is just a demo, you can remove them */
    
    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_BLACK);

    while(arm_fsm_rt_cpl != list_view_show(&this.tListView, ptTile, NULL, bIsNewFrame));

    /* draw text at the top-left corner */
    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
    arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
    arm_lcd_text_set_draw_region(NULL);
    arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
    arm_lcd_text_location(0,0);
    //arm_lcd_puts("Scene 5");
    arm_lcd_printf("scene 5");

    /*-----------------------draw the foreground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_5_t *__arm_2d_scene5_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_5_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_5_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_5_t),
                                                        __alignof__(user_scene_5_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        memset(ptThis, 0, sizeof(user_scene_5_t));
        bUserAllocated = true;
    }

    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

        /* a dirty region to be specified at runtime*/
        ADD_REGION_TO_LIST(s_tDirtyRegions,
            .tSize = {
                0, 200,
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
                .iWidth = 60,
                .iHeight = 8,
            },
        ),
    END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)
    
    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    /* initialise dirty region 0 at runtime
     * this demo shows that we create a region in the centre of a screen(320*240)
     * for a image stored in the tile c_tileCMSISLogoMask
     */
    s_tDirtyRegions[0].tRegion.tLocation = (arm_2d_location_t){
        .iX = 0,
        .iY = ((tScreen.tSize.iHeight - 200) >> 1),
    };
    s_tDirtyRegions[0].tRegion.tSize.iWidth = tScreen.tSize.iWidth;

    *ptThis = (user_scene_5_t){
        .use_as__arm_2d_scene_t = {
        /* Please uncommon the callbacks if you need them
         */
        //.fnBackground   = &__pfb_draw_scene5_background_handler,
        .fnScene        = &__pfb_draw_scene5_handler,
        .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
        

        //.fnOnBGStart    = &__on_scene5_background_start,
        //.fnOnBGComplete = &__on_scene5_background_complete,
        .fnOnFrameStart = &__on_scene5_frame_start,
        .fnOnFrameCPL   = &__on_scene5_frame_complete,
        .fnDepose       = &__on_scene5_depose,
        },
        .bUserAllocated = bUserAllocated,
    };

    do {
        progress_wheel_cfg_t tCFG = {
            .tDotColour     = GLCD_COLOR_WHITE,         /* dot colour */
            .tWheelColour   = GLCD_COLOR_GREEN,         /* arc colour */
            .iWheelDiameter = 60,                       /* diameter, 0 means use the mask's original size */
            .bUseDirtyRegions = false,                  /* use dirty regions */
        };

        progress_wheel_init(&this.tWheel, 
                            &this.use_as__arm_2d_scene_t,
                            &tCFG);
    } while(0);

    do {
        list_view_cfg_t tCFG = {
            .fnIterator = &ARM_2D_LIST_ITERATOR_ARRAY,
            .fnCalculator = &ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_HORIZONTAL,
            
            .tListSize = {0, 200},
            .ptItems = (arm_2d_list_item_t *)s_tListArray,
            .hwCount = dimof(s_tListArray),
            .hwItemSizeInByte = sizeof(my_list_item_t),
            
            .bIgnoreBackground = true,
        };
        
        list_view_init(&this.tListView, &tCFG);
        
        arm_foreach(my_list_item_t, s_tListArray, ptItem) {
            ptItem->use_as__arm_2d_list_item_t.pTarget = (uintptr_t)ptThis;
        }
    } while(0);

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}




#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

