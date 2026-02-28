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

#define __USER_SCENE_LMSK_IMPLEMENT__
#include "arm_2d_scene_lmsk.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)                                 \
 && defined(RTE_Acceleration_Arm_2D_Extra_Loader)

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

#if ARM_2D_DEMO_LMSK_USE_QOI_AS_REFERENCE
#   define REFERENCE_MASK   this.Reference.tLoader.tTile
#else
#   define REFERENCE_MASK   c_tileECGScanMask
#endif

#define LMSK_MASK           this.LMSK.tLoader.tTile

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern const arm_2d_tile_t c_tileMeterPanelMask;
extern const arm_2d_tile_t c_tileECGScanMask;

extern const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_LiberationSansRegular14_A4 ;
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

static void __on_scene_lmsk_load(arm_2d_scene_t *ptScene)
{
    user_scene_lmsk_t *ptThis = (user_scene_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if ARM_2D_DEMO_LMSK_USE_QOI_AS_REFERENCE
    arm_qoi_loader_on_load(&this.Reference.tLoader);
#endif
    arm_lmsk_loader_on_load(&this.LMSK.tLoader);

}

static void __after_scene_lmsk_switching(arm_2d_scene_t *ptScene)
{
    user_scene_lmsk_t *ptThis = (user_scene_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_lmsk_depose(arm_2d_scene_t *ptScene)
{
    user_scene_lmsk_t *ptThis = (user_scene_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*--------------------- insert your depose code begin --------------------*/
#if ARM_2D_DEMO_LMSK_USE_QOI_AS_REFERENCE
    arm_qoi_loader_depose(&this.Reference.tLoader);
#endif
    arm_lmsk_loader_depose(&this.LMSK.tLoader);

    /*---------------------- insert your depose code end  --------------------*/

    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }
    ptScene->ptPlayer = NULL;
    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene lmsk                                                                    *
 *----------------------------------------------------------------------------*/
#if 0  /* deprecated */
static void __on_scene_lmsk_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_lmsk_t *ptThis = (user_scene_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_lmsk_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_lmsk_t *ptThis = (user_scene_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}
#endif


static void __on_scene_lmsk_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_lmsk_t *ptThis = (user_scene_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
#if ARM_2D_DEMO_LMSK_USE_QOI_AS_REFERENCE
    arm_qoi_loader_on_frame_start(&this.Reference.tLoader);
#endif

    arm_lmsk_loader_on_frame_start(&this.LMSK.tLoader);

}

static void __on_scene_lmsk_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_lmsk_t *ptThis = (user_scene_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if ARM_2D_DEMO_LMSK_USE_QOI_AS_REFERENCE
    arm_qoi_loader_on_frame_complete(&this.Reference.tLoader);
#endif
    arm_lmsk_loader_on_frame_complete(&this.LMSK.tLoader);

}

static void __before_scene_lmsk_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_lmsk_t *ptThis = (user_scene_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_lmsk_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_lmsk_t *ptThis = (user_scene_lmsk_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/
        
        arm_2d_size_t tImageBoxSize = LMSK_MASK.tRegion.tSize;
        arm_2d_size_t tTitleSize = arm_lcd_printf_to_buffer(
                                    (arm_2d_font_t *)&ARM_2D_FONT_LiberationSansRegular14_A4, 
                                    "Reference Mask");

        tImageBoxSize.iHeight += tTitleSize.iHeight;

        arm_2d_dock_vertical(__top_canvas, tImageBoxSize.iHeight ) {

            arm_2d_layout(__vertical_region) {

            #if 0
                __item_line_dock_horizontal(tImageBoxSize.iWidth, 4, 4, 0, 0) {

                    arm_2d_layout(__item_region) {

                        __item_line_dock_vertical(tTitleSize.iHeight) {

                            arm_2d_fill_colour(ptTile, &__item_region, GLCD_COLOR_WHITE);

                            arm_2d_align_centre(__item_region, tTitleSize) {
                                arm_lcd_text_set_draw_region(&__centre_region);
                                arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);

                                arm_lcd_printf_buffer(0);
                            }

                        }

                        __item_line_dock_vertical() {
                            arm_2d_fill_colour_with_mask(   ptTile, 
                                                            &__item_region, 
                                                            &REFERENCE_MASK,
                                                            (__arm_2d_color_t){GLCD_COLOR_WHITE});

                        }
                    }
                }
            #endif

                __item_line_dock_horizontal(tImageBoxSize.iWidth, 4, 4, 0, 0) {

                    arm_2d_layout(__item_region) {

                        __item_line_dock_vertical(tTitleSize.iHeight) {

                            arm_2d_fill_colour(ptTile, &__item_region, GLCD_COLOR_WHITE);

                            arm_lcd_text_set_draw_region(&__item_region);
                            arm_lcd_text_set_colour(GLCD_COLOR_BLUE, GLCD_COLOR_WHITE);

                            arm_lcd_printf_label(ARM_2D_ALIGN_CENTRE, "LMSK A8");

                        }

                        __item_line_dock_vertical() {
                            arm_2d_fill_colour_with_mask(   ptTile, 
                                                            &__item_region, 
                                                            &LMSK_MASK,
                                                            (__arm_2d_color_t){GLCD_COLOR_WHITE});

                            arm_2d_helper_dirty_region_update_item( 
                                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
                                (arm_2d_tile_t *)ptTile,
                                NULL,
                                &__item_region);

                        }
                    }
                }
            }
        }

        /* draw text at the top-left corner */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene lmsk");

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_lmsk_t *__arm_2d_scene_lmsk_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_lmsk_t *ptThis)
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
        ptThis = (user_scene_lmsk_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_lmsk_t),
                                                        __alignof__(user_scene_lmsk_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_lmsk_t));

    *ptThis = (user_scene_lmsk_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_lmsk_load,
            .fnScene        = &__pfb_draw_scene_lmsk_handler,
            .fnAfterSwitch  = &__after_scene_lmsk_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene_lmsk_background_start,        /* deprecated */
            //.fnOnBGComplete = &__on_scene_lmsk_background_complete,     /* deprecated */
            .fnOnFrameStart = &__on_scene_lmsk_frame_start,
            .fnBeforeSwitchOut = &__before_scene_lmsk_switching_out,
            .fnOnFrameCPL   = &__on_scene_lmsk_frame_complete,
            .fnDepose       = &__on_scene_lmsk_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_lmsk_t begin ---------------*/
#if ARM_2D_DEMO_LMSK_USE_QOI_AS_REFERENCE
    /* initialize QOI loader */
    do {
    #if ARM_2D_DEMO_LMSK_QOI_USE_FILE && __ARM_QOI_USE_LOADER_IO__
        arm_loader_io_file_init(&this.Reference.LoaderIO.tFile, 
                                "../common/asset/ECGScanMaskSmall.qoi");
    #else
        extern const uint8_t c_qoiMeterPanel[20394];
        extern const uint8_t c_qoiRadarBackground[45557];

        arm_loader_io_rom_init( &this.Reference.LoaderIO.tROM, 
                                (uintptr_t)c_qoiMeterPanel, 
                                sizeof(c_qoiMeterPanel));
    #endif
        arm_qoi_loader_cfg_t tCFG = {
            //.bUseHeapForVRES = true,
            .ptScene = (arm_2d_scene_t *)ptThis,
            .u2WorkMode = ARM_QOI_MODE_PARTIAL_DECODED,

            /* you can only extract specific colour channel and use it as A8 mask */
            .tColourInfo.chScheme = ARM_2D_COLOUR_MASK_A8,
            //.u2ChannelIndex = ARM_QOI_MASK_CHN_GREEN,   

            //.bInvertColour = true,
            //.bForceDisablePreBlendwithBG = true,
            .tBackgroundColour.wColour = this.use_as__arm_2d_scene_t.tCanvas.wColour,
        #if ARM_2D_DEMO_LMSK_QOI_USE_FILE && __ARM_QOI_USE_LOADER_IO__
            .ImageIO = {
                .ptIO = &ARM_LOADER_IO_FILE,
                .pTarget = (uintptr_t)&this.Reference.LoaderIO.tFile,
            },
        #elif __ARM_QOI_USE_LOADER_IO__
            .ImageIO = {
                .ptIO = &ARM_LOADER_IO_ROM,
                .pTarget = (uintptr_t)&this.LoaderIO.tROM,
            },
        #else
            .pchQOISource = c_qoiMeterPanel,
        #endif
        };

        arm_qoi_loader_init(&this.Reference.tLoader, &tCFG);
    } while(0);
#endif

    /* initialize LMSK loader */
    do {
    #if ARM_2D_DEMO_LMSK_USE_FILE && __ARM_LMSK_USE_LOADER_IO__
        arm_loader_io_file_init(&this.LMSK.LoaderIO.tFile, 
                                "../common/loader/lmsk_loader/lmsk/encoder/Test.lmsk");
                                //"../common/asset/ECGScanMaskSmall.lmsk");
    #else
        extern const uint8_t c_lmskECGScan[913];

        arm_loader_io_rom_init( &this.LMSK.LoaderIO.tROM, 
                                (uintptr_t)c_lmskECGScan, 
                                sizeof(c_lmskECGScan));
    #endif
        arm_lmsk_loader_cfg_t tCFG = {
            //.bUseHeapForVRES = true,
            .ptScene = (arm_2d_scene_t *)ptThis,

        #if ARM_2D_DEMO_LMSK_USE_FILE && __ARM_LMSK_USE_LOADER_IO__
            .ImageIO = {
                .ptIO = &ARM_LOADER_IO_FILE,
                .pTarget = (uintptr_t)&this.LMSK.LoaderIO.tFile,
            },
        #elif __ARM_LMSK_USE_LOADER_IO__
            .ImageIO = {
                .ptIO = &ARM_LOADER_IO_ROM,
                .pTarget = (uintptr_t)&this.LMSK.LoaderIO.tROM,
            },
        #else
            .pchLMSKSource = c_lmskECGScan,
        #endif
        };

        arm_lmsk_loader_init(&this.LMSK.tLoader, &tCFG);
    } while(0);
    /* ------------   initialize members of user_scene_lmsk_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


