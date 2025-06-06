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
#include "arm_2d_example_controls.h"
#include "benchmark_generic.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wextra-semi"
#   pragma clang diagnostic ignored "-Wfloat-conversion"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wswitch-default"
#endif

/*============================ MACROS ========================================*/
#ifndef __GLCD_CFG_SCEEN_WIDTH__
#   warning Please specify the screen width by defining the macro __GLCD_CFG_SCEEN_WIDTH__, default value 320 is used for now
#   define __GLCD_CFG_SCEEN_WIDTH__      320
#endif

#ifndef __GLCD_CFG_SCEEN_HEIGHT__
#   warning Please specify the screen height by defining the macro __GLCD_CFG_SCEEN_HEIGHT__, default value 240 is used for now
#   define __GLCD_CFG_SCEEN_HEIGHT__      320
#endif

#if __GLCD_CFG_COLOUR_DEPTH__ == 8
#   define c_tileCMSISLogo          c_tileCMSISLogoGRAY8
#   define c_tilePictureSun         c_tilePictureSunGRAY8
#   define c_tileHelium             c_tileHeliumGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16
#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565
#   define c_tilePictureSun         c_tilePictureSunRGB565
#   define c_tileHelium             c_tileHeliumRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32
#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#   define c_tilePictureSun         c_tilePictureSunCCCA8888
#   define c_tileHelium             c_tileHeliumCCCN888

#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_layer(__TILE_ADDR, __OPACITY, __X, __Y, ...)                     \
    {                                                                           \
        .ptTile = (__TILE_ADDR),                                                \
        .tRegion.tLocation.iX = (__X),                                          \
        .tRegion.tLocation.iY = (__Y),                                          \
        .chOpacity = (__OPACITY),                                               \
        __VA_ARGS__                                                             \
    }

/*============================ TYPES =========================================*/
typedef struct arm_2d_layer_t {
    const arm_2d_tile_t         *ptTile;
    arm_2d_region_t             tRegion;
    uint32_t                    wMode;
    uint8_t                     chOpacity;
    uint8_t                     bIsIrregular        : 1;
    uint8_t                     bIsRoundedCorner    : 1;
    uint8_t                     bIsAlphaGradient    : 1; 
    uint8_t                                     : 5;
    COLOUR_INT                  tKeyColour;
    arm_2d_alpha_samples_4pts_t tSamplePoints;
} arm_2d_layer_t;

typedef struct floating_range_t {
    arm_2d_region_t     tRegion;
    arm_2d_layer_t     *ptLayer;
    arm_2d_location_t   tOffset;
} floating_range_t;

enum {
#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
    BENCHMARK_LAYER_HELIUM_ROUNDED_CORNER,
    BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING,
#endif
    BENCHMARK_LAYER_RED_OPA,
    BENCHMARK_LAYER_ICON,
    BENCHMARK_LAYER_ALPHA_GRADIENT,
};

/*============================ GLOBAL VARIABLES ==============================*/

/*! picture helium */
extern
const arm_2d_tile_t c_tileHelium ;

extern 
const arm_2d_tile_t c_tileSoftwareMask2;

extern 
const arm_2d_tile_t c_tileSoftwareMask;

extern 
const arm_2d_tile_t c_tileSoftwareA2Mask;

extern 
const arm_2d_tile_t c_tileSoftwareA4Mask;

extern
const arm_2d_tile_t c_tileCMSISLogo;

extern 
const arm_2d_tile_t c_tileCMSISLogoMask;

extern 
const arm_2d_tile_t c_tileCMSISLogoA2Mask;

extern 
const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern 
const arm_2d_tile_t c_tileCMSISLogoMask2;

extern
const arm_2d_tile_t c_tilePictureSun;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
dcl_fb(c_tLayerB)

/* implement a 50*50 framebuffer */
impl_fb(c_tLayerB, 90, 50, __arm_2d_color_t);
#endif

ARM_NOINIT static uint8_t s_bmpFadeMask[__GLCD_CFG_SCEEN_WIDTH__ >> 1];
const arm_2d_tile_t c_tileFadeMask = {
    .tRegion = {
        .tSize = {
            .iWidth = (__GLCD_CFG_SCEEN_WIDTH__ >> 1),
            .iHeight = 1,
        },
    },
    .tInfo = {
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {
            .chScheme = ARM_2D_COLOUR_8BIT,
        },
    },
    .pchBuffer = (uint8_t *)s_bmpFadeMask,
};



static arm_2d_layer_t s_ptRefreshLayers[] = {
#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
    [BENCHMARK_LAYER_HELIUM_ROUNDED_CORNER] = 
        arm_2d_layer(&c_tileHelium, 255-64, 100, 100,
                    .bIsRoundedCorner = true,
                    ),
#endif
    [BENCHMARK_LAYER_RED_OPA] = 
        arm_2d_layer(   NULL, 128, 10, 80, 
                        .tRegion.tSize.iWidth = 100, 
                        .tRegion.tSize.iHeight = 100
                    ),
#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
    [BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING] = 
        arm_2d_layer(   &c_tLayerB, 112, 50, 150),
#endif
    [BENCHMARK_LAYER_ICON] = 
        arm_2d_layer(&c_tilePictureSun, 255, 0, 0, 
                    .bIsIrregular = true, 
                    .tKeyColour = GLCD_COLOR_WHITE),

    [BENCHMARK_LAYER_ALPHA_GRADIENT] = 
        arm_2d_layer(   NULL, 255, 0, 0, 
                        .tRegion.tSize.iWidth = __GLCD_CFG_SCEEN_WIDTH__ >> 1, 
                        .tRegion.tSize.iHeight = __GLCD_CFG_SCEEN_HEIGHT__ >> 2,
                        .bIsAlphaGradient = true,
                        .tSamplePoints = {
                            128, 64,
                            0, 255
                        },
                    ),
};

static floating_range_t s_ptFloatingBoxes[] = {
#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__

    [BENCHMARK_LAYER_HELIUM_ROUNDED_CORNER] = {
        .tRegion = {{0-200, 0-200}, {__GLCD_CFG_SCEEN_WIDTH__ + 400, __GLCD_CFG_SCEEN_HEIGHT__ + 400}},
        .ptLayer = &s_ptRefreshLayers[BENCHMARK_LAYER_HELIUM_ROUNDED_CORNER],
        .tOffset = {-1, 1},
    },
    [BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING] = {
        .tRegion = {{0, 0}, {__GLCD_CFG_SCEEN_WIDTH__, __GLCD_CFG_SCEEN_HEIGHT__}},
        .ptLayer = &s_ptRefreshLayers[BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING],
        .tOffset = {-2, 4},
    },
#endif
    [BENCHMARK_LAYER_RED_OPA] = {
        .tRegion = {{0, 0}, {__GLCD_CFG_SCEEN_WIDTH__, __GLCD_CFG_SCEEN_HEIGHT__}},
        .ptLayer = &s_ptRefreshLayers[BENCHMARK_LAYER_RED_OPA],
        .tOffset = {5, -2},
    },

    [BENCHMARK_LAYER_ICON] = {
        .tRegion = {{-100, -100}, {__GLCD_CFG_SCEEN_WIDTH__+200, __GLCD_CFG_SCEEN_HEIGHT__+200}},
        .ptLayer = &s_ptRefreshLayers[BENCHMARK_LAYER_ICON],
        .tOffset = {5, 5},
    },

    [BENCHMARK_LAYER_ALPHA_GRADIENT] = {
        .tRegion = {{0, 0}, {__GLCD_CFG_SCEEN_WIDTH__, __GLCD_CFG_SCEEN_HEIGHT__}},
        .ptLayer = &s_ptRefreshLayers[BENCHMARK_LAYER_ALPHA_GRADIENT],
        .tOffset = {-1, 5},
    },
};

/*============================ IMPLEMENTATION ================================*/

void benchmark_generic_init(void)
{
    arm_extra_controls_init();

#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
    s_ptRefreshLayers[BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING].wMode = ARM_2D_CP_MODE_FILL;
#endif


    s_ptRefreshLayers[BENCHMARK_LAYER_ICON].wMode = ARM_2D_CP_MODE_FILL;
    
    //! generate line-fading template for a half of the screen
    do {
        memset(s_bmpFadeMask, 0, sizeof(s_bmpFadeMask));
        float fRatio = 255.0f / (float)(__GLCD_CFG_SCEEN_WIDTH__ >> 1);
        for (int32_t n = 0; n < (__GLCD_CFG_SCEEN_WIDTH__ >> 1); n++) {
            s_bmpFadeMask[n] = 255 - ((float)n * fRatio);
        }
    } while(0);

    arm_foreach(arm_2d_layer_t, s_ptRefreshLayers) {
        arm_2d_region_t tRegion = _->tRegion;
        if (!tRegion.tSize.iHeight) {
            tRegion.tSize.iHeight = _->ptTile->tRegion.tSize.iHeight;
        }
        if (!tRegion.tSize.iWidth) {
            tRegion.tSize.iWidth = _->ptTile->tRegion.tSize.iWidth;
        }
        
        _->tRegion = tRegion;
    }
}

static void __update_boxes(floating_range_t *ptBoxes, uint_fast16_t hwCount)
{
    assert(NULL != ptBoxes);
    assert(hwCount > 0);

    do {
        arm_2d_region_t tOldRegion = ptBoxes->ptLayer->tRegion;
        if (   (tOldRegion.tLocation.iX + tOldRegion.tSize.iWidth + ptBoxes->tOffset.iX)
            >= ptBoxes->tRegion.tLocation.iX + ptBoxes->tRegion.tSize.iWidth) {
            ptBoxes->tOffset.iX = -ptBoxes->tOffset.iX;
        }

        if (    (tOldRegion.tLocation.iX + ptBoxes->tOffset.iX)
            <   (ptBoxes->tRegion.tLocation.iX)) {
            ptBoxes->tOffset.iX = -ptBoxes->tOffset.iX;
        }

        if (   (tOldRegion.tLocation.iY + tOldRegion.tSize.iHeight + ptBoxes->tOffset.iY)
            >= ptBoxes->tRegion.tLocation.iY + ptBoxes->tRegion.tSize.iHeight) {
            ptBoxes->tOffset.iY = -ptBoxes->tOffset.iY;
        }

        if (    (tOldRegion.tLocation.iY + ptBoxes->tOffset.iY)
            <   (ptBoxes->tRegion.tLocation.iY)) {
            ptBoxes->tOffset.iY = -ptBoxes->tOffset.iY;
        }

        ptBoxes->ptLayer->tRegion.tLocation.iX += ptBoxes->tOffset.iX;
        ptBoxes->ptLayer->tRegion.tLocation.iY += ptBoxes->tOffset.iY;

        ptBoxes++;

    }while(--hwCount);
}

void benchmark_generic_do_events(void)
{
    static uint32_t s_wCounter = 0;
    static uint32_t s_wFrameCounter = 0;

    s_wFrameCounter++;

    __update_boxes(s_ptFloatingBoxes, dimof(s_ptFloatingBoxes));
    
    if (s_wFrameCounter == 25) {                       //!< every 25 frames
        s_wFrameCounter = 0;
        
        switch(s_wCounter++ & 0x03) {
            case 0:
                s_ptRefreshLayers[BENCHMARK_LAYER_ICON].wMode = 
                    ARM_2D_CP_MODE_FILL;
                break;
            case 1:
                s_ptRefreshLayers[BENCHMARK_LAYER_ICON].wMode = 
                    ARM_2D_CP_MODE_FILL       | 
                    ARM_2D_CP_MODE_X_MIRROR;
                break;
            case 2:
                s_ptRefreshLayers[BENCHMARK_LAYER_ICON].wMode = 
                    ARM_2D_CP_MODE_FILL       | 
                    ARM_2D_CP_MODE_Y_MIRROR;
                break;
            case 3:
                s_ptRefreshLayers[BENCHMARK_LAYER_ICON].wMode = 
                    ARM_2D_CP_MODE_FILL       | 
                    ARM_2D_CP_MODE_X_MIRROR   | 
                    ARM_2D_CP_MODE_Y_MIRROR;
                break;
        }
#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
        s_ptRefreshLayers[BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING].wMode 
            = s_ptRefreshLayers[BENCHMARK_LAYER_ICON].wMode;
#endif
    }
}


static void show_icon_with_background(  const arm_2d_tile_t *ptTile, 
                                        bool bIsNewFrame)
{
    assert(NULL != ptTile);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_canvas(ptTile, __canvas) {
        arm_2d_align_centre(__canvas, 100, 100) {
            
            draw_round_corner_box(  ptTile, 
                                    &__centre_region,
                                    GLCD_COLOR_BLACK,
                                    32);    
            arm_2d_op_wait_async(NULL);
        }


        arm_2d_align_centre(__canvas, c_tileSoftwareMask.tRegion.tSize) {
            arm_2d_fill_colour_with_mask(
                                    ptTile, 
                                    &__centre_region, 
                                    &c_tileSoftwareMask, 
                                    (__arm_2d_color_t){GLCD_COLOR_DARK_GREY});
            arm_2d_op_wait_async(NULL);
        }
    }
}

static void show_icon_without_background(   const arm_2d_tile_t *ptTile, 
                                            bool bIsNewFrame)
{
    assert(NULL != ptTile);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_canvas(ptTile, __canvas) {
        arm_2d_align_centre(__canvas, c_tileSoftwareMask.tRegion.tSize) {
            
            arm_2d_fill_colour_with_a2_mask_and_opacity(   
                                    ptTile, 
                                    &__centre_region, 
                                    &c_tileSoftwareA2Mask, 
                                    (__arm_2d_color_t){GLCD_COLOR_DARK_GREY},
                                    128);
        
            arm_2d_op_wait_async(NULL);
            
            __centre_region.tLocation.iX -= 2;
            __centre_region.tLocation.iY -= 2;
        
            arm_2d_fill_colour_with_a4_mask(   ptTile, 
                                            &__centre_region, 
                                            &c_tileSoftwareA4Mask, 
                                            (__arm_2d_color_t){GLCD_COLOR_WHITE});
            arm_2d_op_wait_async(NULL);
        }
    }
}

static void __draw_layers(  const arm_2d_tile_t *ptTile,
                            arm_2d_layer_t *ptLayers, 
                            uint_fast16_t hwCount,
                            bool bIsNewFrame)
{
    assert(NULL != ptLayers);
    assert(hwCount > 0);
    arm_2d_tile_t tTempPanel;
    
    static const arm_2d_region_t c_tFillRegion = {
                                .tLocation = {
                                    .iX = -200, 
                                    .iY = -100, 
                                },
                                .tSize = {
                                    .iWidth = (__GLCD_CFG_SCEEN_WIDTH__ >> 1) + 200, 
                                    .iHeight = __GLCD_CFG_SCEEN_HEIGHT__ + 100 
                                }};

#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
    //! handle the left half of the screen
    do {
    #if 0 /* equivalent */
        //!< fill background with CMSISlogo (with colour keying)
        arm_2d_rgb16_tile_copy( &c_tileCMSISLogo,
                                ptTile,
                                &c_tFillRegion,
                                ptLayers[BENCHMARK_LAYER_ICON].wMode);
    #else

        switch (ptLayers[BENCHMARK_LAYER_ICON].wMode) {
            case ARM_2D_CP_MODE_FILL | ARM_2D_CP_MODE_X_MIRROR:
                arm_2d_tile_fill_with_x_mirror( &c_tileCMSISLogo,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_FILL | ARM_2D_CP_MODE_Y_MIRROR:
                arm_2d_tile_fill_with_y_mirror( &c_tileCMSISLogo,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_FILL | ARM_2D_CP_MODE_XY_MIRROR:
                arm_2d_tile_fill_with_xy_mirror(&c_tileCMSISLogo,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_FILL:
                arm_2d_tile_fill_only(&c_tileCMSISLogo,
                                            ptTile,
                                            &c_tFillRegion);
                break;
                
            case ARM_2D_CP_MODE_COPY | ARM_2D_CP_MODE_X_MIRROR:
                arm_2d_tile_copy_with_x_mirror( &c_tileCMSISLogo,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_COPY | ARM_2D_CP_MODE_Y_MIRROR:
                arm_2d_tile_copy_with_y_mirror( &c_tileCMSISLogo,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_COPY | ARM_2D_CP_MODE_XY_MIRROR:
                arm_2d_tile_copy_with_xy_mirror(&c_tileCMSISLogo,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_COPY:
                arm_2d_tile_copy_only(  &c_tileCMSISLogo,
                                        ptTile,
                                        &c_tFillRegion);
                break;
        }

    #endif
    } while(0);
#else
    //! handle the left half of the screen
    do {
    #if 1 /* equivalent */
        //!< fill background with Sun
        arm_2d_tile_copy(   
            &c_tilePictureSun,
            ptTile,
            &c_tFillRegion,
            ptLayers[BENCHMARK_LAYER_ICON].wMode | ARM_2D_CP_MODE_FILL);
    #else
        switch (ptLayers[BENCHMARK_LAYER_ICON].wMode) {
            case ARM_2D_CP_MODE_FILL | ARM_2D_CP_MODE_X_MIRROR:
                arm_2d_tile_fill_with_x_mirror( &c_tilePictureSun,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_FILL | ARM_2D_CP_MODE_Y_MIRROR:
                arm_2d_tile_fill_with_y_mirror( &c_tilePictureSun,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_FILL | ARM_2D_CP_MODE_XY_MIRROR:
                arm_2d_tile_fill_with_xy_mirror(&c_tilePictureSun,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_FILL:
                arm_2d_tile_fill_only(&c_tilePictureSun,
                                            ptTile,
                                            &c_tFillRegion);
                break;
                
            case ARM_2D_CP_MODE_COPY | ARM_2D_CP_MODE_X_MIRROR:
                arm_2d_tile_copy_with_x_mirror( &c_tilePictureSun,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_COPY | ARM_2D_CP_MODE_Y_MIRROR:
                arm_2d_tile_copy_with_y_mirror( &c_tilePictureSun,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_COPY | ARM_2D_CP_MODE_XY_MIRROR:
                arm_2d_tile_copy_with_xy_mirror(&c_tilePictureSun,
                                                ptTile,
                                                &c_tFillRegion);
                break;
            case ARM_2D_CP_MODE_COPY:
                arm_2d_tile_copy_only(  &c_tilePictureSun,
                                        ptTile,
                                        &c_tFillRegion);
                break;
        }
    #endif
    } while(0);
#endif
    ARM_2D_OP_WAIT_ASYNC();

    //! handle the right half of the screen
    do {
        
        static const arm_2d_region_t tRightHalfScreen = {
                                                (__GLCD_CFG_SCEEN_WIDTH__ >> 1), 
                                                0, 
                                                (__GLCD_CFG_SCEEN_WIDTH__ >> 1), 
                                                __GLCD_CFG_SCEEN_HEIGHT__
                                            };
        
        //!< generate a child tile for this half of screen
        arm_2d_tile_generate_child( ptTile, 
                                    &tRightHalfScreen, 
                                    &tTempPanel, 
                                    false);

        arm_2d_canvas(&tTempPanel, __canvas) {

#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
        //!< set background colour
        arm_2d_fill_colour( &tTempPanel,
                            NULL,
                            GLCD_COLOR_WHITE);
#else
        //!< set background colour
        arm_2d_fill_colour( &tTempPanel,
                            NULL,
                            GLCD_COLOR_DARK_GREY);
#endif
        ARM_2D_OP_WAIT_ASYNC();

#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
        /*! \note arm_2d_align_centre generate a region '__centre_region' based
         *!       on the given target tile (frame buffer) and the size of the 
         *!       target area of your following drawing.
         *!       Use '__centre_region' when required as the target region in
         *!       2D operations inside the {...} .
         */
        //arm_2d_align_centre(__canvas, c_tileCMSISLogoMask.tRegion.tSize) {

#if 0
            arm_2d_tile_copy_with_masks(
                &c_tileCMSISLogo,
                &c_tileCMSISLogoMask,
                &tTempPanel,
                &c_tileFadeMask,
                &c_tFillRegion,                                               
                ptLayers[BENCHMARK_LAYER_ICON].wMode);
#else
            switch (ptLayers[BENCHMARK_LAYER_ICON].wMode 
                    &~ ARM_2D_CP_MODE_FILL) {
                case ARM_2D_CP_MODE_COPY:
                    arm_2d_tile_fill_with_masks_only(
                        &c_tileCMSISLogo,
                        &c_tileCMSISLogoMask,
                        &tTempPanel,
                        &c_tileFadeMask,
                        &c_tFillRegion);
                    break;
                case ARM_2D_CP_MODE_X_MIRROR:
                    arm_2d_tile_fill_with_masks_and_x_mirror(
                        &c_tileCMSISLogo,
                        &c_tileCMSISLogoMask,
                        &tTempPanel,
                        &c_tileFadeMask,
                        &c_tFillRegion);
                    break;
                case ARM_2D_CP_MODE_Y_MIRROR:
                    arm_2d_tile_fill_with_masks_and_y_mirror(
                        &c_tileCMSISLogo,
                        &c_tileCMSISLogoMask,
                        &tTempPanel,
                        &c_tileFadeMask,
                        &c_tFillRegion);
                    break;
                case ARM_2D_CP_MODE_XY_MIRROR:
                    arm_2d_tile_fill_with_masks_and_xy_mirror(
                        &c_tileCMSISLogo,
                        &c_tileCMSISLogoMask,
                        &tTempPanel,
                        &c_tileFadeMask,
                        &c_tFillRegion);
                    break;
            }
#endif
            ARM_2D_OP_WAIT_ASYNC();
        //}
#else
        arm_2d_align_centre(__canvas, 
                            c_tileCMSISLogoMask.tRegion.tSize.iWidth,
                            c_tileCMSISLogoMask.tRegion.tSize.iHeight + 20
                            ) {
            arm_2d_fill_colour_with_a4_mask(
                    &tTempPanel,
                    &__centre_region,
                    &c_tileCMSISLogoA4Mask,
                    (__arm_2d_color_t){GLCD_COLOR_LIGHT_GREY}
                );
            
            ARM_2D_OP_WAIT_ASYNC();
            
            do {
                arm_2d_tile_t tProgressBar;
                __centre_region.tSize.iWidth = tTempPanel.tRegion.tSize.iWidth;
                __centre_region.tLocation.iX = 0;
                __centre_region.tSize.iHeight -= c_tileCMSISLogoA4Mask.tRegion.tSize.iHeight;
                __centre_region.tLocation.iY += c_tileCMSISLogoA4Mask.tRegion.tSize.iHeight;
                
                if (NULL != arm_2d_tile_generate_child( &tTempPanel, 
                                                        &__centre_region, 
                                                        &tProgressBar, 
                                                        false)) {
                    progress_bar_flowing_show(&tProgressBar, -1);
                }
            } while(0);
        }
#endif
        }
    } while(0);

#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
    arm_2d_fill_colour( s_ptRefreshLayers[BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING].ptTile, 
                        NULL, 
                        GLCD_COLOR_GREEN);

    
    //!< fill a given tile with the sun icon (with colour-keying)
    switch(s_ptRefreshLayers[BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING].wMode) {
        case ARM_2D_CP_MODE_FILL:
            arm_2d_tile_fill_with_colour_keying_only(
                &c_tilePictureSun,
                s_ptRefreshLayers[BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING].ptTile,
                NULL,
                GLCD_COLOR_WHITE);
                break;
        case ARM_2D_CP_MODE_FILL | ARM_2D_CP_MODE_X_MIRROR:
            arm_2d_tile_fill_with_colour_keying_and_x_mirror(
                &c_tilePictureSun,
                s_ptRefreshLayers[BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING].ptTile,
                NULL,
                GLCD_COLOR_WHITE);
                break;
        case ARM_2D_CP_MODE_FILL | ARM_2D_CP_MODE_Y_MIRROR:
            arm_2d_tile_fill_with_colour_keying_and_y_mirror(
                &c_tilePictureSun,
                s_ptRefreshLayers[BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING].ptTile,
                NULL,
                GLCD_COLOR_WHITE);
                break;
        case ARM_2D_CP_MODE_FILL | ARM_2D_CP_MODE_XY_MIRROR:
            arm_2d_tile_fill_with_colour_keying_and_xy_mirror(
                &c_tilePictureSun,
                s_ptRefreshLayers[BENCHMARK_LAYER_FILL_ICON_WITH_COLOUR_KEYING].ptTile,
                NULL,
                GLCD_COLOR_WHITE);
                break;
    }

#endif

    ARM_2D_OP_WAIT_ASYNC();

    arm_foreach(arm_2d_layer_t, ptLayers, hwCount, ptLayer) {
        arm_2d_region_t tRegion = ptLayer->tRegion;

        if (NULL == ptLayer->ptTile) { 
            continue;
        }
        
        if (ptLayer->bIsIrregular) {
            if (255 != ptLayer->chOpacity) {
                arm_2d_tile_copy_with_colour_keying_and_opacity(
                            ptLayer->ptTile,
                            ptTile,
                            &tRegion,
                            ptLayer->chOpacity,
                            (__arm_2d_color_t){ ptLayer->tKeyColour });
            } else {
                switch(ptLayer->wMode & ~ARM_2D_CP_MODE_FILL) {
                    case ARM_2D_CP_MODE_COPY:
                        arm_2d_tile_copy_with_colour_keying_only( 
                                                    ptLayer->ptTile,
                                                    ptTile,
                                                    &tRegion,
                                                    ptLayer->tKeyColour);
                        break;
                    case ARM_2D_CP_MODE_X_MIRROR:
                        arm_2d_tile_copy_with_colour_keying_and_x_mirror( 
                                                    ptLayer->ptTile,
                                                    ptTile,
                                                    &tRegion,
                                                    ptLayer->tKeyColour);
                        break;
                    case ARM_2D_CP_MODE_Y_MIRROR:
                        arm_2d_tile_copy_with_colour_keying_and_y_mirror( 
                                                    ptLayer->ptTile,
                                                    ptTile,
                                                    &tRegion,
                                                    ptLayer->tKeyColour);
                        break;
                    case ARM_2D_CP_MODE_XY_MIRROR:
                        arm_2d_tile_copy_with_colour_keying_and_xy_mirror( 
                                                    ptLayer->ptTile,
                                                    ptTile,
                                                    &tRegion,
                                                    ptLayer->tKeyColour);
                        break;
//                    default:
//                        arm_2d_tile_copy_with_colour_keying( 
//                                                    ptLayer->ptTile,
//                                                    ptTile,
//                                                    &tRegion,
//                                                    ptLayer->tKeyColour,
//                                                    ptLayer->wMode);
//                        break;
                }
            }
        } else {
            if (ptLayer->bIsRoundedCorner) {
                draw_round_corner_image(ptLayer->ptTile,
                                        ptTile,
                                        &tRegion,
                                        bIsNewFrame,
                                        ptLayer->chOpacity);
            } else if (255 != ptLayer->chOpacity) {
                arm_2d_tile_copy_with_opacity(  ptLayer->ptTile,
                                                ptTile,
                                                &tRegion,
                                                ptLayer->chOpacity);
            } else {
                arm_2d_tile_copy_only( ptLayer->ptTile,
                                        ptTile,
                                        &tRegion);
            }
        }
        ARM_2D_OP_WAIT_ASYNC();
    }
    
    arm_2d_fill_colour_with_opacity(   
                        ptTile, 
                        &s_ptRefreshLayers[BENCHMARK_LAYER_RED_OPA].tRegion,
                        (__arm_2d_color_t){GLCD_COLOR_RED},
                        s_ptRefreshLayers[BENCHMARK_LAYER_RED_OPA].chOpacity);
    
    arm_2d_fill_colour_with_4pts_alpha_gradient_and_opacity(   
                        ptTile, 
                        &s_ptRefreshLayers[BENCHMARK_LAYER_ALPHA_GRADIENT].tRegion,
                        (__arm_2d_color_t){GLCD_COLOR_ORANGE},
                        s_ptRefreshLayers[BENCHMARK_LAYER_ALPHA_GRADIENT].chOpacity,
                        s_ptRefreshLayers[BENCHMARK_LAYER_ALPHA_GRADIENT].tSamplePoints);

    
    if (NULL != arm_2d_tile_generate_child( ptTile, 
                                (arm_2d_region_t []){
                                    {
                                #if __ARM_2D_CFG_BENCHMARK_TINY_MODE__
                                        .tSize = {
                                            .iWidth = __GLCD_CFG_SCEEN_WIDTH__ >> 1,
                                            .iHeight = __GLCD_CFG_SCEEN_HEIGHT__,
                                        },
                                #else
                                        .tSize = {
                                            .iWidth = __GLCD_CFG_SCEEN_WIDTH__ >> 1,
                                            .iHeight = __GLCD_CFG_SCEEN_HEIGHT__ >> 1,
                                        },
                                #endif
                                    },
                                },
                                &tTempPanel,
                                false)) {


#if __ARM_2D_CFG_BENCHMARK_TINY_MODE__
        arm_2d_canvas(&tTempPanel, __canvas) {
            arm_2d_align_centre(__canvas, 100, 100) {
                
                draw_round_corner_box(  &tTempPanel, 
                                        &__centre_region,
                                        GLCD_COLOR_BLACK,
                                        64);    
                ARM_2D_OP_WAIT_ASYNC();
            }
        }
#endif

            //! show busy wheel
            busy_wheel2_show(&tTempPanel, bIsNewFrame);
        
        ARM_2D_OP_WAIT_ASYNC();
    }

#if !__ARM_2D_CFG_BENCHMARK_TINY_MODE__
#if 0
    if (NULL != arm_2d_tile_generate_child( ptTile, 
                                (arm_2d_region_t []){
                                    {
                                        .tLocation = {
                                            .iX = 0,
                                            .iY = __GLCD_CFG_SCEEN_HEIGHT__ >> 1,
                                        },
                                        .tSize = {
                                            .iWidth = __GLCD_CFG_SCEEN_WIDTH__ >> 1,
                                            .iHeight = __GLCD_CFG_SCEEN_HEIGHT__ >> 1,
                                        },
                                    },
                                },
                                &tTempPanel,
                                false)) {
    //  arm_2d_canvas(&tTempPanel, __canvas) {
    //        arm_2d_align_centre(__canvas, 100, 100) {
    //            
    //            draw_round_corner_box(  &tTempPanel, 
    //                                    &__centre_region,
    //                                    GLCD_COLOR_BLACK,
    //                                    128,
    //                                    bIsNewFrame);    
    //            arm_2d_op_wait_async(NULL);
    //        }
            //! show busy wheel
            spinning_wheel2_show(   &tTempPanel, 
                                    //__RGB(0x92, 0xD0, 0x50), 
                                    GLCD_COLOR_WHITE,
                                    bIsNewFrame);
    //  }
        ARM_2D_OP_WAIT_ASYNC();
    }
#endif
#endif

#if !defined(__ARM_2D_CFG_BENCHMARK_TINY_MODE__) || !__ARM_2D_CFG_BENCHMARK_TINY_MODE__

    if (NULL != arm_2d_tile_generate_child( ptTile, 
                                (arm_2d_region_t []){
                                    {
                                        .tLocation = {
                                            .iX = __GLCD_CFG_SCEEN_WIDTH__ >> 1,
                                            .iY = 0
                                        },
                                        .tSize = {
                                            .iWidth = __GLCD_CFG_SCEEN_WIDTH__ >> 1,
                                            .iHeight = __GLCD_CFG_SCEEN_HEIGHT__ >> 1,
                                        },
                                    },
                                },
                                &tTempPanel,
                                false)) {
        //show_icon_with_background(&tTempPanel, bIsNewFrame);
        spinning_wheel_show(&tTempPanel, bIsNewFrame);
        
        ARM_2D_OP_WAIT_ASYNC();
    }


    if (NULL != arm_2d_tile_generate_child( ptTile, 
                                (arm_2d_region_t []){
                                    {
                                        .tLocation = {
                                            .iX = __GLCD_CFG_SCEEN_WIDTH__ >> 1,
                                            .iY = __GLCD_CFG_SCEEN_HEIGHT__ >> 1,
                                        },
                                        .tSize = {
                                            .iWidth = __GLCD_CFG_SCEEN_WIDTH__ >> 1,
                                            .iHeight = __GLCD_CFG_SCEEN_HEIGHT__ >> 1,
                                        },
                                    },
                                },
                                &tTempPanel,
                                false)) {
        show_icon_without_background(&tTempPanel, bIsNewFrame);
        
        ARM_2D_OP_WAIT_ASYNC();
    }
#endif
}

void benchmark_generic_draw(const arm_2d_tile_t *ptTile, bool bIsNewFrame)
{
    __draw_layers(  ptTile, 
                    s_ptRefreshLayers, 
                    dimof(s_ptRefreshLayers),
                    bIsNewFrame);
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


