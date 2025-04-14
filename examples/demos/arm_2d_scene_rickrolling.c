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

#define __USER_SCENE_RICKROLLING_IMPLEMENT__
#include "arm_2d_scene_rickrolling.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)                                 \
 && defined(RTE_Acceleration_Arm_2D_Extra_TJpgDec_Loader)

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
/*============================ IMPLEMENTATION ================================*/

static void __on_scene_rickrolling_load(arm_2d_scene_t *ptScene)
{
    user_scene_rickrolling_t *ptThis = (user_scene_rickrolling_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_tjpgd_loader_on_load(&this.tAnimation);
}

static void __after_scene_rickrolling_switching(arm_2d_scene_t *ptScene)
{
    user_scene_rickrolling_t *ptThis = (user_scene_rickrolling_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_rickrolling_depose(arm_2d_scene_t *ptScene)
{
    user_scene_rickrolling_t *ptThis = (user_scene_rickrolling_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    arm_tjpgd_loader_depose(&this.tAnimation);

    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene rickrolling                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_rickrolling_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_rickrolling_t *ptThis = (user_scene_rickrolling_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_rickrolling_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_rickrolling_t *ptThis = (user_scene_rickrolling_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_rickrolling_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_rickrolling_t *ptThis = (user_scene_rickrolling_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    if (arm_2d_helper_is_time_out( this.tFilm.hwPeriodPerFrame , &this.lTimestamp[0])) {

        arm_2d_helper_film_next_frame(&this.tFilm);
    }

    arm_tjpgd_loader_on_frame_start(&this.tAnimation);
}

static void __on_scene_rickrolling_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_rickrolling_t *ptThis = (user_scene_rickrolling_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_tjpgd_loader_on_frame_complete(&this.tAnimation);
}

static void __before_scene_rickrolling_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_rickrolling_t *ptThis = (user_scene_rickrolling_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_rickrolling_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_rickrolling_t *ptThis = (user_scene_rickrolling_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        arm_2d_align_centre(__top_canvas, 120, 120 ) {
            
            draw_round_corner_border(   ptTile, 
                                        &__centre_region, 
                                        GLCD_COLOR_BLACK, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});
                                    
        }

        arm_2d_align_centre(__top_canvas, 
                            this.tFilm.use_as__arm_2d_tile_t.tRegion.tSize ) {
            
            arm_2d_tile_copy_only(  (const arm_2d_tile_t *)&this.tFilm,
                                    ptTile,
                                    &__centre_region);
            
            arm_2d_helper_dirty_region_update_item( 
                    &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
                    (arm_2d_tile_t *)ptTile,
                    &__top_canvas,
                    &__centre_region);
                                    
        }
    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_rickrolling_t *__arm_2d_scene_rickrolling_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_rickrolling_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    /* get the screen region */
    arm_2d_region_t __top_canvas
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    ARM_2D_UNUSED(__top_canvas);

    if (NULL == ptThis) {
        ptThis = (user_scene_rickrolling_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_rickrolling_t),
                                                        __alignof__(user_scene_rickrolling_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_rickrolling_t));

    *ptThis = (user_scene_rickrolling_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_WHITE}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_rickrolling_load,
            .fnScene        = &__pfb_draw_scene_rickrolling_handler,
            //.fnAfterSwitch  = &__after_scene_rickrolling_switching,

            //.fnOnBGStart    = &__on_scene_rickrolling_background_start,
            //.fnOnBGComplete = &__on_scene_rickrolling_background_complete,
            .fnOnFrameStart = &__on_scene_rickrolling_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_rickrolling_switching_out,
            .fnOnFrameCPL   = &__on_scene_rickrolling_frame_complete,
            .fnDepose       = &__on_scene_rickrolling_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_rickrolling_t begin ---------------*/
    /* initialize TJpgDec loader */
    do {
    #if ARM_2D_DEMO_TJPGD_USE_FILE
        arm_tjpgd_io_file_loader_init(&this.LoaderIO.tFile, "Rickrolling75.jpg");
    #else
        extern const uint8_t c_chRickRolling75[104704];

        arm_tjpgd_io_binary_loader_init(&this.LoaderIO.tBinary, c_chRickRolling75, sizeof(c_chRickRolling75));
    #endif

        arm_tjpgd_loader_cfg_t tCFG = {
            .bUseHeapForVRES = true,
            .ptScene = (arm_2d_scene_t *)ptThis,
            .u2WorkMode = ARM_TJPGD_MODE_PARTIAL_DECODED,
        
        #if ARM_2D_DEMO_TJPGD_USE_FILE
            .ImageIO = {
                .ptIO = &ARM_TJPGD_IO_FILE_LOADER,
                .pTarget = (uintptr_t)&this.LoaderIO.tFile,
            },
        #else
            .ImageIO = {
                .ptIO = &ARM_TJPGD_IO_BINARY_LOADER,
                .pTarget = (uintptr_t)&this.LoaderIO.tBinary,
            },
        #endif

        };

        arm_tjpgd_loader_init(&this.tAnimation, &tCFG);
    } while(0);

    this.tFilm = (arm_2d_helper_film_t)impl_film(this.tAnimation, 100, 108, 1, 62, 16);

    /* ------------   initialize members of user_scene_rickrolling_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


