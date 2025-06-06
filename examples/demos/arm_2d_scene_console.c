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

#define __USER_SCENE_CONSOLE_IMPLEMENT__
#include "arm_2d_scene_console.h"

#include "arm_2d_helper.h"

#include "arm_2d_disp_adapters.h"

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


static void __on_scene_console_depose(arm_2d_scene_t *ptScene)
{
    user_scene_console_t *ptThis = (user_scene_console_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    console_box_depose(&this.tConsole);
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    ptScene->ptPlayer = NULL;
    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene console                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_console_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_console_t *ptThis = (user_scene_console_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_console_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_console_t *ptThis = (user_scene_console_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_console_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_console_t *ptThis = (user_scene_console_t *)ptScene;

    console_box_on_frame_start(&this.tConsole);
}

static void __on_scene_console_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_console_t *ptThis = (user_scene_console_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
}

static void __before_scene_console_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_console_t *ptThis = (user_scene_console_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_console_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_console_t *ptThis = (user_scene_console_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

        /* draw console */
        console_box_show(   &this.tConsole,
                            ptTile,
                            &__top_canvas,
                            bIsNewFrame,
                            255);



    /*-----------------------draw the foreground end  -----------------------*/
    }
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_console_t *__arm_2d_scene_console_init(
                                        arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_console_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    if (NULL == ptThis) {
        ptThis = (user_scene_console_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_console_t),
                                                        __alignof__(user_scene_console_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_console_t));

    *ptThis = (user_scene_console_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 
        
            /* Please uncommon the callbacks if you need them
             */
            .fnScene        = &__pfb_draw_scene_console_handler,
            
            //.fnOnBGStart    = &__on_scene_console_background_start,
            //.fnOnBGComplete = &__on_scene_console_background_complete,
            .fnOnFrameStart = &__on_scene_console_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_console_switching_out,
            .fnOnFrameCPL   = &__on_scene_console_frame_complete,
            .fnDepose       = &__on_scene_console_depose,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_console_t begin ---------------*/
    do {
    #if __DISP0_CFG_CONSOLE_INPUT_BUFFER__
        static uint8_t s_chInputBuffer[__DISP0_CFG_CONSOLE_INPUT_BUFFER__];
    #endif
        static uint8_t s_chConsoleBuffer[(__DISP0_CFG_SCEEN_WIDTH__ / 6) * (__DISP0_CFG_SCEEN_HEIGHT__ / 8)];
        console_box_cfg_t tCFG = {
            .tBoxSize = tScreen.tSize,
            
            .pchConsoleBuffer = s_chConsoleBuffer,
            .hwConsoleBufferSize = sizeof(s_chConsoleBuffer),

        #if __DISP0_CFG_CONSOLE_INPUT_BUFFER__
            .pchInputBuffer = s_chInputBuffer,
            .hwInputBufferSize = sizeof(s_chInputBuffer),
        #endif
            .tColor = GLCD_COLOR_GREEN,
            .bUseDirtyRegion = true,
        };

        console_box_init(   &this.tConsole, 
                            &this.use_as__arm_2d_scene_t, 
                            &tCFG);
    } while(0);

    /* ------------   initialize members of user_scene_console_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


