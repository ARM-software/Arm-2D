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
#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include "arm_2d.h"

#define __USER_SCENE_WATCH_IMPLEMENT__
#include "arm_2d_scene_watch.h"

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
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"  
#elif __IS_COMPILER_ARM_COMPILER_5__
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

#   define c_tileWatchPanel             c_tileWatchPanelGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileWatchPanel             c_tileWatchPanelRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileWatchPanel             c_tileWatchPanelCCCA8888
#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileWatchPanel;

extern const arm_2d_tile_t c_tilePointerSecMask;

extern
const arm_2d_tile_t c_tilePointerMask;

static
const arm_2d_tile_t c_tilePointerHourMask = 
    impl_child_tile(c_tilePointerSecMask, 0, 80, 9, 67);

static arm_2d_location_t s_tPointerSecCenter;
static arm_2d_location_t s_tPointerMinCenter;
static arm_2d_location_t s_tPointerHourCenter;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __on_scene_watch_load(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    // initialize second pointer
    arm_foreach(__clock_pointer_t, this.Pointers, ptItem) {
        arm_2d_helper_dirty_region_transform_init(
                                    &ptItem->tHelper,
                                    &ptScene->tDirtyRegionHelper,
                                    (arm_2d_op_t *)&ptItem->tOP,
                                    ARM_2D_ANGLE(0.5f),
                                    0.1f);
    }

}

static void __on_scene_watch_depose(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    arm_foreach(__clock_pointer_t, this.Pointers, ptItem) {
        arm_2d_helper_dirty_region_transform_depose(&ptItem->tHelper);
    }

    /* depose op */
    ARM_2D_OP_INIT(this.Pointers[0].tOP);
    ARM_2D_OP_INIT(this.Pointers[1].tOP);
    ARM_2D_OP_INIT(this.Pointers[2].tOP);

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene watch                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_watch_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_watch_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_watch_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    int32_t iResult;
    float fAngle;
    
    /* update second pointer */
    do {
        if (arm_2d_helper_time_liner_slider(0, 3600, 60 * 100, &iResult, &this.lTimestamp[1])) {
            this.lTimestamp[1] = 0;
        }
        fAngle = ARM_2D_ANGLE((float)iResult / 10.0f);


        /* update helper with new values*/
        arm_2d_helper_dirty_region_transform_update_value(&this.Pointers[0].tHelper, fAngle, 1.0f);
    } while(0);
    
    /* update minute pointer */
    do {
        if (arm_2d_helper_time_liner_slider(0, 3600, 60 * 60 * 100, &iResult, &this.lTimestamp[2])) {
            this.lTimestamp[2] = 0;
        }
        fAngle = ARM_2D_ANGLE((float)iResult / 10.0f);


        /* update helper with new values*/
        arm_2d_helper_dirty_region_transform_update_value(&this.Pointers[1].tHelper, fAngle, 1.0f);
    } while(0);

    /* update hour pointer */
    do {
        if (arm_2d_helper_time_liner_slider(0, 3600, 12*60*60*100, &iResult, &this.lTimestamp[3])) {
            this.lTimestamp[3] = 0;
        }
        fAngle = ARM_2D_ANGLE((float)iResult / 10.0f);


        /* update helper with new values*/
        arm_2d_helper_dirty_region_transform_update_value(&this.Pointers[2].tHelper, fAngle, 1.0f);
    } while(0);

    /* call helper's on-frame-start event handler */

    arm_foreach(__clock_pointer_t, this.Pointers, ptItem) {
        arm_2d_helper_dirty_region_transform_on_frame_start(&ptItem->tHelper);
    }
}

static void __on_scene_watch_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /* switch to next scene after 3s */
//    if (arm_2d_helper_is_time_out(3000, &this.lTimestamp[0])) {
//        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
//    }
}

static void __before_scene_watch_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_watch_handler)
{
    user_scene_watch_t *ptThis = (user_scene_watch_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(tScreenSize);
    
    arm_2d_canvas(ptTile, __canvas) {
    /*-----------------------draw the foreground begin-----------------------*/

        arm_2d_align_centre(__canvas, c_tileWatchPanel.tRegion.tSize) {

            arm_2d_tile_copy_with_opacity(  &c_tileWatchPanel,
                                            ptTile,
                                            &__centre_region,
                                            64);

            arm_2d_op_wait_async(NULL);

            /* draw pointer hour */
            do {
                arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                    &this.Pointers[2].tOP,
                                    &c_tilePointerHourMask,
                                    ptTile,
                                    &__centre_region,
                                    s_tPointerHourCenter,
                                    this.Pointers[2].tHelper.fAngle,
                                    this.Pointers[2].tHelper.fScale,
                                    GLCD_COLOR_GREEN,
                                    128);

                arm_2d_helper_dirty_region_transform_update(
                                                    &this.Pointers[2].tHelper,
                                                    &__centre_region,
                                                    bIsNewFrame);
                arm_2d_op_wait_async(
                                &this.Pointers[2].tOP.use_as__arm_2d_op_core_t);
            } while(0);

            /* draw pointer minutes */
            do {
                arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                    &this.Pointers[1].tOP,
                                    &c_tilePointerMask,
                                    ptTile,
                                    &__centre_region,
                                    s_tPointerMinCenter,
                                    this.Pointers[1].tHelper.fAngle,
                                    this.Pointers[1].tHelper.fScale,
                                    GLCD_COLOR_GREEN,
                                    128);

                arm_2d_helper_dirty_region_transform_update(
                                                    &this.Pointers[1].tHelper,
                                                    &__centre_region,
                                                    bIsNewFrame);

                arm_2d_op_wait_async(
                                &this.Pointers[1].tOP.use_as__arm_2d_op_core_t);
            } while(0);

            /* draw pointer sec */
            do {
                arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                    &this.Pointers[0].tOP,
                                    &c_tilePointerSecMask,
                                    ptTile,
                                    &__centre_region,
                                    s_tPointerSecCenter,
                                    this.Pointers[0].tHelper.fAngle,
                                    this.Pointers[0].tHelper.fScale,
                                    GLCD_COLOR_RED,
                                    255);

                arm_2d_helper_dirty_region_transform_update(
                                                    &this.Pointers[0].tHelper,
                                                    &__centre_region,
                                                    bIsNewFrame);


                arm_2d_op_wait_async(
                    &this.Pointers[0].tOP.use_as__arm_2d_op_core_t);
            } while(0);
        }

        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene watch");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_watch_t *__arm_2d_scene_watch_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_watch_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

//        /* a dirty region to be specified at runtime*/
//        ADD_REGION_TO_LIST(s_tDirtyRegions,
//            0  /* initialize at runtime later */
//        ),
        
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

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);

    s_tDirtyRegions[0].tRegion.tSize.iWidth = tScreen.tSize.iWidth;

    if (NULL == ptThis) {
        ptThis = (user_scene_watch_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_watch_t),
                                                        __alignof__(user_scene_watch_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
        memset(ptThis, 0, sizeof(user_scene_watch_t));
    }
    
    *ptThis = (user_scene_watch_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 
        
            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_watch_load,
            .fnScene        = &__pfb_draw_scene_watch_handler,
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_watch_background_start,
            //.fnOnBGComplete = &__on_scene_watch_background_complete,
            .fnOnFrameStart = &__on_scene_watch_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_watch_switching_out,
            .fnOnFrameCPL   = &__on_scene_watch_frame_complete,
            .fnDepose       = &__on_scene_watch_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };
    
    // initialize second pointer
    do {
        s_tPointerSecCenter.iX = (c_tilePointerSecMask.tRegion.tSize.iWidth >> 1);
        s_tPointerSecCenter.iY = 100;  /* radius */
        
        /* initialize op */
        ARM_2D_OP_INIT(this.Pointers[0].tOP);

    } while(0);

    // initialize minutes pointer
    do {
        s_tPointerMinCenter.iX = (c_tilePointerMask.tRegion.tSize.iWidth >> 1);
        s_tPointerMinCenter.iY = 70;  /* radius */
        
        /* initialize op */
        ARM_2D_OP_INIT(this.Pointers[1].tOP);

    } while(0);

    // initialize hour pointer
    do {
        s_tPointerHourCenter.iX = (c_tilePointerMask.tRegion.tSize.iWidth >> 1);
        s_tPointerHourCenter.iY = 67;  /* radius */
        
        /* initialize op */
        ARM_2D_OP_INIT(this.Pointers[2].tOP);

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

