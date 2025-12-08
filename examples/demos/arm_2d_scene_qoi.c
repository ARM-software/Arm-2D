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

#define __USER_SCENE_QOI_IMPLEMENT__
#include "arm_2d_scene_qoi.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)                                 \
 && defined(RTE_Acceleration_Arm_2D_Extra_QOI_Loader)

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

static void __on_scene_qoi_load(arm_2d_scene_t *ptScene)
{
    user_scene_qoi_t *ptThis = (user_scene_qoi_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_qoi_loader_on_load(&this.tQOIBackground);

}

static void __after_scene_qoi_switching(arm_2d_scene_t *ptScene)
{
    user_scene_qoi_t *ptThis = (user_scene_qoi_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_qoi_depose(arm_2d_scene_t *ptScene)
{
    user_scene_qoi_t *ptThis = (user_scene_qoi_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    arm_qoi_loader_depose(&this.tQOIBackground);

    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }
    ptScene->ptPlayer = NULL;
    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene qoi                                                                *
 *----------------------------------------------------------------------------*/

static void __on_scene_qoi_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_qoi_t *ptThis = (user_scene_qoi_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_qoi_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_qoi_t *ptThis = (user_scene_qoi_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_qoi_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_qoi_t *ptThis = (user_scene_qoi_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_qoi_loader_on_frame_start(&this.tQOIBackground);

}

static void __on_scene_qoi_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_qoi_t *ptThis = (user_scene_qoi_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_qoi_loader_on_frame_complete(&this.tQOIBackground);

}

static void __before_scene_qoi_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_qoi_t *ptThis = (user_scene_qoi_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_qoi_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_qoi_t *ptThis = (user_scene_qoi_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

        arm_2d_align_centre(__top_canvas, this.tQOIBackground.vres.tTile.tRegion.tSize) {
            extern const arm_2d_tile_t c_tileMeterPanelCCCA8888;

            switch (this.tQOIBackground.vres.tTile.tColourInfo.chScheme) {
                case ARM_2D_COLOUR_MASK_A8:
                    arm_2d_fill_colour_with_mask(   
                                    ptTile, 
                                    &__centre_region, 
                                    &this.tQOIBackground.vres.tTile,
                                    (__arm_2d_color_t){ GLCD_COLOR_NIXIE_TUBE});
                    break;
                case ARM_2D_COLOUR_CCCA8888:
                    arm_2d_tile_copy_with_opacity(  
                                            &this.tQOIBackground.vres.tTile,
                                            ptTile,
                                            &__centre_region,
                                            255);
                    break;
                default:
                    arm_2d_tile_copy_only(  &this.tQOIBackground.vres.tTile,
                                            ptTile,
                                            &__centre_region);
                    break;
            }
        }

        /* draw text at the top-left corner */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene QOI Loader");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_qoi_t *__arm_2d_scene_qoi_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_qoi_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_qoi_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_qoi_t),
                                                        __alignof__(user_scene_qoi_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_qoi_t));

    *ptThis = (user_scene_qoi_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_WHITE}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_qoi_load,
            .fnScene        = &__pfb_draw_scene_qoi_handler,
            //.fnAfterSwitch  = &__after_scene_qoi_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_qoi_background_start,
            //.fnOnBGComplete = &__on_scene_qoi_background_complete,
            .fnOnFrameStart = &__on_scene_qoi_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_qoi_switching_out,
            .fnOnFrameCPL   = &__on_scene_qoi_frame_complete,
            .fnDepose       = &__on_scene_qoi_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_qoi_t begin ---------------*/

    /* initialize Qoiec loader */
    do {
    #if ARM_2D_DEMO_QOI_USE_FILE
        arm_qoi_io_file_loader_init(&this.LoaderIO.tFile, "./radar_background.qoi");
    #else
        extern const uint8_t c_qoiMeterPanel[20394];
        extern const uint8_t c_qoiRadarBackground[45557];

        arm_qoi_io_binary_loader_init(&this.LoaderIO.tBinary, c_qoiMeterPanel, sizeof(c_qoiMeterPanel));
    #endif
        arm_qoi_loader_cfg_t tCFG = {
            //.bUseHeapForVRES = true,
            .ptScene = (arm_2d_scene_t *)ptThis,
            .u2WorkMode = ARM_QOI_MODE_PARTIAL_DECODED,

            /* you can only extract specific colour channel and use it as A8 mask */
            //.tColourInfo.chScheme = ARM_2D_COLOUR_MASK_A8,
            //.u2ChannelIndex = ARM_QOI_MASK_CHN_GREEN,   

            //.bInvertColour = true,
            //.bForceDisablePreBlendwithBG = true,
            .tBackgroundColour.wColour = GLCD_COLOR_WHITE,
        #if ARM_2D_DEMO_QOI_USE_FILE
            .ImageIO = {
                .ptIO = &ARM_QOI_IO_FILE_LOADER,
                .pTarget = (uintptr_t)&this.LoaderIO.tFile,
            },
        #else
            .ImageIO = {
                .ptIO = &ARM_QOI_IO_BINARY_LOADER,
                .pTarget = (uintptr_t)&this.LoaderIO.tBinary,
            },
        #endif
        };

        arm_qoi_loader_init(&this.tQOIBackground, &tCFG);
    } while(0);

    /* ------------   initialize members of user_scene_qoi_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


