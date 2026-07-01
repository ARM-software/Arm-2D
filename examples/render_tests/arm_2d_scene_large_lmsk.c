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

#define __USER_SCENE_LARGE_LMSK_IMPLEMENT__
#include "arm_2d_scene_large_lmsk.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)                                 \
&&  defined(RTE_Acceleration_Arm_2D_Extra_LMSK_Loader)

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

static uint32_t c_wColourTable[] = {
    __RGB32(0xFF, 0x00, 0x00),
    __RGB32(0x00, 0xFF, 0x00),
    __RGB32(0x00, 0x00, 0xFF),
    __RGB32(0x00, 0xFF, 0xFF),
    __RGB32(0xFF, 0xFF, 0xFF),
    __RGB32(0xFF, 0xFF, 0x00),
    __RGB32(0xFF, 0x00, 0x00),
    __RGB32(0xFF, 0x00, 0xFF),
    __RGB32(0xFF, 0x80, 0x00),      /* orange */
    __RGB32(0xFF, 0xA5, 0x00),      /* nixie tube */

};

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_large_lmsk_load(arm_2d_scene_t *ptScene)
{
    user_scene_large_lmsk_t *ptThis = (user_scene_large_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_lmsk_loader_on_load(&this.tAnimation);
#if ARM_2D_DEMO_LARGE_LMSK_USE_IMAGE_BOX
    image_box_on_load(&this.tImage);
#endif
}

static void __after_scene_large_lmsk_switching(arm_2d_scene_t *ptScene)
{
    user_scene_large_lmsk_t *ptThis = (user_scene_large_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_large_lmsk_depose(arm_2d_scene_t *ptScene)
{
    user_scene_large_lmsk_t *ptThis = (user_scene_large_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    arm_lmsk_loader_depose(&this.tAnimation);

#if ARM_2D_DEMO_LARGE_LMSK_USE_IMAGE_BOX
    image_box_depose(&this.tImage);
#endif

    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }
    ptScene->ptPlayer = NULL;
    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene large_lmsk                                                           *
 *----------------------------------------------------------------------------*/

static void __on_scene_large_lmsk_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_large_lmsk_t *ptThis = (user_scene_large_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_large_lmsk_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_large_lmsk_t *ptThis = (user_scene_large_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_large_lmsk_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_large_lmsk_t *ptThis = (user_scene_large_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    if (arm_2d_helper_is_time_out( this.tFilm.hwPeriodPerFrame , &this.lTimestamp[0])) {

    #if 0
        uint16_t hwIndex =  arm_2d_helper_film_get_frame_index(&this.tFilm);
        uint16_t hwFrameCount = arm_2d_helper_film_get_frame_count(&this.tFilm);

        if (hwIndex == 0) {
            hwIndex = hwFrameCount - 1;
        } else {
            hwIndex--;
        }
        arm_2d_helper_film_set_frame(&this.tFilm, hwIndex);
    #else
        arm_2d_helper_film_next_frame(&this.tFilm);
    #endif

        arm_2d_helper_dirty_region_item_suspend_update(
            &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
            false);
    } else {
        arm_2d_helper_dirty_region_item_suspend_update(
            &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
            true);
    }

    /* update cloud colour */
    do {
        int32_t nResult;
        if (arm_2d_helper_time_liner_slider(0, 1000, 5000, &nResult, &this.lTimestamp[1])) {
            this.lTimestamp[1] = 0;
            this.wPreviousColour = c_wColourTable[this.chColourTableIndex];

            srand((uint32_t)arm_2d_helper_get_system_timestamp());
            this.chColourTableIndex = rand() % dimof(c_wColourTable);
            nResult = 0;
        }

        this.tColour = arm_2d_pixel_from_brga8888(
                                            __arm_2d_helper_colour_slider(
                                                this.wPreviousColour, 
                                                c_wColourTable[this.chColourTableIndex],
                                                1000,
                                                nResult));

    } while(0);

    if (0 != ptScene->ptPlayer->Benchmark.wAverage) {
        this.iNumber = MIN(( arm_2d_helper_get_reference_clock_frequency() 
                           / ptScene->ptPlayer->Benchmark.wAverage), 
                          999);
    }

    arm_lmsk_loader_on_frame_start(&this.tAnimation);
#if ARM_2D_DEMO_LARGE_LMSK_USE_IMAGE_BOX
    image_box_on_frame_start(&this.tImage);
#endif
}

static void __on_scene_large_lmsk_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_large_lmsk_t *ptThis = (user_scene_large_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_lmsk_loader_on_frame_complete(&this.tAnimation);

#if ARM_2D_DEMO_LARGE_LMSK_USE_IMAGE_BOX
    image_box_on_frame_complete(&this.tImage);
#endif
}

static void __before_scene_large_lmsk_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_large_lmsk_t *ptThis = (user_scene_large_lmsk_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_large_lmsk_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_large_lmsk_t *ptThis = (user_scene_large_lmsk_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
    #if ARM_2D_DEMO_LARGE_LMSK_USE_IMAGE_BOX
        arm_2d_align_centre(__top_canvas, 
                            160, 120 + 10 ) {
    #else
        arm_2d_size_t tTVBoxSize = this.tFilm.use_as__arm_2d_tile_t.tRegion.tSize;
        tTVBoxSize.iHeight += 10;

        arm_2d_align_centre(__top_canvas, tTVBoxSize ) {
    #endif
            
                arm_2d_dock_top(__centre_region, 10) {

                    arm_2d_fill_colour(ptTile, &__top_region, GLCD_COLOR_DARK_GREY);

                    arm_2d_dock_vertical(__top_region, 8, 2) {
                        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
                        arm_lcd_text_set_draw_region(&__vertical_region);
                        arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);
                        arm_lcd_printf("[%04"PRId16"]\tFPS:%"PRId16, 
                                        arm_2d_helper_film_get_frame_index(&this.tFilm),
                                        this.iNumber);
                    }
                }

            #if ARM_2D_DEMO_LARGE_LMSK_USE_IMAGE_BOX
                image_box_show(&this.tImage, ptTile, &__centre_region, 255, bIsNewFrame);
            #else
                arm_2d_fill_colour_with_mask(   ptTile, 
                                        &__centre_region, 
                                        (const arm_2d_tile_t *)&this.tFilm, 
                                        (__arm_2d_color_t){this.tColour});
            #endif

            arm_2d_helper_dirty_region_update_item( 
                            &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
                            (arm_2d_tile_t *)ptTile,
                            &__top_canvas,
                            &__centre_region);
                                    
        }
    
        /* draw text at the top-left corner */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);

        arm_lcd_puts("Large LMSK decoding");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_large_lmsk_t *__arm_2d_scene_large_lmsk_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_large_lmsk_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    /* get the screen region */
    arm_2d_region_t __top_canvas
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    ARM_2D_UNUSED(__top_canvas);

    if (NULL == ptThis) {
        ptThis = (user_scene_large_lmsk_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_large_lmsk_t),
                                                        __alignof__(user_scene_large_lmsk_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_large_lmsk_t));

    *ptThis = (user_scene_large_lmsk_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_WHITE}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_large_lmsk_load,
            .fnScene        = &__pfb_draw_scene_large_lmsk_handler,
            //.fnAfterSwitch  = &__after_scene_large_lmsk_switching,

            //.fnOnBGStart    = &__on_scene_large_lmsk_background_start,
            //.fnOnBGComplete = &__on_scene_large_lmsk_background_complete,
            .fnOnFrameStart = &__on_scene_large_lmsk_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_large_lmsk_switching_out,
            .fnOnFrameCPL   = &__on_scene_large_lmsk_frame_complete,
            .fnDepose       = &__on_scene_large_lmsk_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_large_lmsk_t begin ---------------*/

    /* initialize Zjpgdec loader */
    do {

    #if ARM_2D_DEMO_LARGE_LMSK_USE_FILE && __ARM_LMSK_USE_LOADER_IO__
        arm_loader_io_file_init(&this.LoaderIO.tFile, 
                                //"../common/loader/lmsk_loader/lmsk/encoder/Test.lmsk");
                                //"../render_tests/bad_apple_320x240_24_a4.lmsk");
                                "../render_tests/bad_apple_100x75_16_a4.lmsk");
    #else
        extern 
        const uint8_t c_lmskBadApple100x75col16a4[2381132];

        extern 
        const uint8_t c_lmskBadApple160x120col20a4[4636049];
        
    #   if 1
        arm_loader_io_cache_init(   &this.LoaderIO.tCache, 
                                    (uintptr_t)c_lmskBadApple100x75col16a4, 
                                    sizeof(c_lmskBadApple100x75col16a4),
                                    this.tCachelines,
                                    dimof(this.tCachelines));
    #   else
        arm_loader_io_rom_init( &this.LoaderIO.tROM, 
                                (uintptr_t)c_lmskBadApple100x75col16a4, 
                                sizeof(c_lmskBadApple100x75col16a4));
    #   endif
    #endif

        arm_lmsk_loader_cfg_t tCFG = {
            .ptScene = (arm_2d_scene_t *)ptThis,

        #if __ARM_LMSK_USE_LOADER_IO__
            .ImageIO = {
            #if ARM_2D_DEMO_LARGE_LMSK_USE_FILE && __ARM_LMSK_USE_LOADER_IO__
                .ptIO = &ARM_LOADER_IO_FILE,
                .pTarget = (uintptr_t)&this.LoaderIO.tFile,
            #elif 1
                .ptIO = &ARM_LOADER_IO_CACHE,
                .pTarget = (uintptr_t)&this.LoaderIO.tCache,
            #else
                .ptIO = &ARM_LOADER_IO_ROM,
                .pTarget = (uintptr_t)&this.LoaderIO.tROM,
            #endif
            },
        #else
            .pchLMSKSource = c_lmskBadApple100x75col16a4,
        #endif
        };

        arm_lmsk_loader_init(&this.tAnimation, &tCFG);
    } while(0);

    this.tFilm = (arm_2d_helper_film_t)
                    impl_film(  this.tAnimation, 
                        100, 
                        75, 
                        16, 
                        3110, 
                        33);

#if ARM_2D_DEMO_LARGE_LMSK_USE_IMAGE_BOX
    do {
        image_box_cfg_t tCFG = {
            .ptilePhotoMask = &this.tFilm.tTile,
        };

        image_box_init(&this.tImage, &tCFG);

    } while(0);
#endif

    /* ------------   initialize members of user_scene_large_lmsk_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


