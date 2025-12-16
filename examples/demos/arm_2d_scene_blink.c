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

#define __USER_SCENE_BLINK_IMPLEMENT__
#include "arm_2d_scene_blink.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

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
#   define c_tileEyeball            c_tileEyeballGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565
#   define c_tileEyeball            c_tileEyeballRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#   define c_tileEyeball            c_tileEyeballCCCA8888
#else
#   error Unsupported colour depth!
#endif

#define EYE_SIZE_RATIO              2.0f

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern const arm_2d_tile_t c_tileLeftEyeMask;
extern const arm_2d_tile_t c_tileEyeballMask;
extern const arm_2d_tile_t c_tileEyeballCCCA8888;
extern const arm_2d_tile_t c_tileEyeball;

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

static void __on_scene_blink_load(arm_2d_scene_t *ptScene)
{
    user_scene_blink_t *ptThis = (user_scene_blink_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    spin_zoom_widget_on_load(&this.Eye.tSocket);
    spin_zoom_widget_on_load(&this.Eye.tEyeBall);

#if ARM_2D_DEMO_BLINK_USE_QOI
    arm_qoi_loader_on_load(&this.tQOIEyeball);
#endif
}

static void __after_scene_blink_switching(arm_2d_scene_t *ptScene)
{
    user_scene_blink_t *ptThis = (user_scene_blink_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_blink_depose(arm_2d_scene_t *ptScene)
{
    user_scene_blink_t *ptThis = (user_scene_blink_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*--------------------- insert your depose code begin --------------------*/
    spin_zoom_widget_depose(&this.Eye.tSocket);
    spin_zoom_widget_depose(&this.Eye.tEyeBall);

#if ARM_2D_DEMO_BLINK_USE_QOI
    arm_qoi_loader_depose(&this.tQOIEyeball);
#endif
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
 * Scene blink                                                                    *
 *----------------------------------------------------------------------------*/
#if 0  /* deprecated */
static void __on_scene_blink_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_blink_t *ptThis = (user_scene_blink_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_blink_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_blink_t *ptThis = (user_scene_blink_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}
#endif

static arm_fsm_rt_t __blink_action(user_scene_blink_t *ptThis)
{
    uint8_t chRolling;

ARM_PT_BEGIN(this.Blink.chPT)

    do {
        chRolling = rand() & 0xFF;
        if (chRolling <= this.Blink.chRatio) {
            break;
        }
        ARM_PT_YIELD(arm_fsm_rt_on_going);
    } while(true);

    chRolling = rand() & 0xFF;
    
    if (chRolling <= this.Blink.chDoubleBlinkRatio) {
        this.Blink.chBlinkCount = 2;
    } else {
        this.Blink.chBlinkCount = 1;
    }

    /* double blink */
    do {
        do {
            int32_t nResult;
            if (arm_2d_helper_time_cos_slider(  100, 
                                                1,      /* IMPORTANT: This value can NOT be zero */
                                                200,    /* 200 ms*/
                                                0, 
                                                &nResult, 
                                                &this.lTimestamp[0])) {
                this.lTimestamp[0] = 0;
                this.Blink.iEyelidOffset = nResult;
                break;
            }
            this.Blink.iEyelidOffset = nResult;

            ARM_PT_YIELD(arm_fsm_rt_on_going);
        } while(true);
    } while(--this.Blink.chBlinkCount);

    ARM_PT_DELAY_MS((   this.Blink.chDelayAfterBlinkingIn100MS * 100ul 
                    +   100), 
                    &this.lTimestamp[1]);

ARM_PT_END()

    return arm_fsm_rt_cpl;

}

static arm_fsm_rt_t __eyeball_move(user_scene_blink_t *ptThis)
{

ARM_PT_BEGIN(this.EyeBallMove.chPT)

    do {
        if ((this.EyeBallMove.tNewOffset.iX != this.EyeBallMove.tOffset.iX)
        ||  (this.EyeBallMove.tNewOffset.iY != this.EyeBallMove.tOffset.iY)) {

            this.lTimestamp[2] = 0;
            this.lTimestamp[3] = 0;

            break;
        }
        this.EyeBallMove.tStartPoint = this.EyeBallMove.tOffset;
        
        ARM_PT_YIELD(arm_fsm_rt_on_going);
    } while(true);

    do {
        int32_t nResult;
        bool bFinishedX = false;
        bool bFinishedY = false;
        int32_t lFinishInMs = ((int32_t)this.EyeBallMove.iMoveTimeIn50Ms * 50ul) + 50ul;
        if (arm_2d_helper_time_liner_slider(this.EyeBallMove.tStartPoint.iX,
                                            this.EyeBallMove.tNewOffset.iX,
                                            lFinishInMs,
                                            &nResult,
                                            &this.lTimestamp[2])) {
            bFinishedX = true;
        }
        this.EyeBallMove.tOffset.iX = nResult;

        if (arm_2d_helper_time_liner_slider(this.EyeBallMove.tStartPoint.iY,
                                            this.EyeBallMove.tNewOffset.iY,
                                            lFinishInMs,
                                            &nResult,
                                            &this.lTimestamp[3])) {
            bFinishedY = true;
        }
        this.EyeBallMove.tOffset.iY = nResult;

        if (bFinishedX && bFinishedY) {
            break;
        }
        ARM_PT_YIELD(arm_fsm_rt_on_going);
    } while(true);

ARM_PT_END()

    return arm_fsm_rt_cpl;

}

static arm_fsm_rt_t __forcus_generator(user_scene_blink_t *ptThis)
{
    uint8_t chRolling;

ARM_PT_BEGIN(this.ForcusGenerator.chPT)

    do {
        chRolling = rand() & 0xFF;
        if (chRolling <= this.ForcusGenerator.chRatio) {

            arm_2d_size_t tSize = {
                .iWidth = (int16_t)((float)c_tileLeftEyeMask.tRegion.tSize.iWidth * EYE_SIZE_RATIO),
                .iHeight = (int16_t)((float)c_tileLeftEyeMask.tRegion.tSize.iHeight * EYE_SIZE_RATIO),
            };

            tSize.iHeight -= tSize.iHeight >> 2;
            tSize.iWidth -= tSize.iWidth >> 2;

            arm_2d_location_t tOffset = {
                .iX = rand() % tSize.iWidth,
                .iY = rand() % tSize.iHeight,
            };

            tOffset.iX -= tSize.iWidth >> 1;
            tOffset.iY -= tSize.iHeight >> 1;

            this.EyeBallMove.tNewOffset = tOffset;
            this.EyeBallMove.iMoveTimeIn50Ms = rand() % (500 / 50);
            break;
        }
        ARM_PT_YIELD(arm_fsm_rt_on_going);
    } while(true);


    /* wait move complete */
    do {
        if ((this.EyeBallMove.tNewOffset.iX == this.EyeBallMove.tOffset.iX)
        &&  (this.EyeBallMove.tNewOffset.iY == this.EyeBallMove.tOffset.iY)) {

            break;
        }
        ARM_PT_YIELD(arm_fsm_rt_on_going);
    } while(true);

    ARM_PT_DELAY_MS(this.ForcusGenerator.chDelayAfterEachMoveIn100MS * 100 + 500,
                    &this.lTimestamp[4]);

ARM_PT_END()

    return arm_fsm_rt_cpl;

}

static void __on_scene_blink_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_blink_t *ptThis = (user_scene_blink_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    srand(arm_2d_helper_get_system_timestamp());

    __blink_action(ptThis);
    __forcus_generator(ptThis);
    __eyeball_move(ptThis);

    spin_zoom_widget_on_frame_start_xy( &this.Eye.tSocket, 
                                        0, 
                                        EYE_SIZE_RATIO,
                                        (float)this.Blink.iEyelidOffset * EYE_SIZE_RATIO / 100.0f);
    spin_zoom_widget_on_frame_start_xy( &this.Eye.tEyeBall, 
                                        0, 
                                        EYE_SIZE_RATIO,
                                        (float)this.Blink.iEyelidOffset * EYE_SIZE_RATIO / 100.0f);

#if ARM_2D_DEMO_BLINK_USE_QOI
    arm_qoi_loader_on_frame_start(&this.tQOIEyeball);
#endif
}

static void __on_scene_blink_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_blink_t *ptThis = (user_scene_blink_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    spin_zoom_widget_on_frame_complete(&this.Eye.tSocket);
    spin_zoom_widget_on_frame_complete(&this.Eye.tEyeBall);

#if ARM_2D_DEMO_BLINK_USE_QOI
    arm_qoi_loader_on_frame_complete(&this.tQOIEyeball);
#endif
}

static void __before_scene_blink_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_blink_t *ptThis = (user_scene_blink_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_blink_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_blink_t *ptThis = (user_scene_blink_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/
        arm_2d_location_t tPivot = {
            .iX = __top_canvas.tSize.iWidth >> 1,
            .iY = (__top_canvas.tSize.iHeight >> 1) 
                + (c_tileLeftEyeMask.tRegion.tSize.iHeight >> 2),
        };


        spin_zoom_widget_show(  &this.Eye.tSocket, 
                                ptTile, 
                                NULL, 
                                &tPivot, 
                                255);

        arm_2d_align_centre_open(__top_canvas, c_tileEyeballMask.tRegion.tSize) {

            arm_2d_region_t tEyeBallRegion = {
                .tLocation = {
                    .iX =  __centre_region.tLocation.iX + this.EyeBallMove.tOffset.iX,
                    .iY = __centre_region.tLocation.iY + this.EyeBallMove.tOffset.iY,
                },
                .tSize = c_tileEyeballMask.tRegion.tSize,
            };

            __arm_2d_hint_optimize_for_pfb__(tEyeBallRegion) {

                spin_zoom_widget_show(  &this.Eye.tEyeBall, 
                                        ptTile, 
                                        &tEyeBallRegion, 
                                        &tPivot, 
                                        255);
            }
        }

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
    #if ARM_2D_DEMO_BLINK_USE_QOI
        arm_lcd_puts("Scene blink with QOI");
    #else
        arm_lcd_puts("Scene blink");
    #endif

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_blink_t *__arm_2d_scene_blink_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_blink_t *ptThis)
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
        ptThis = (user_scene_blink_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_blink_t),
                                                        __alignof__(user_scene_blink_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_blink_t));

    *ptThis = (user_scene_blink_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_blink_load,
            .fnScene        = &__pfb_draw_scene_blink_handler,
            .fnAfterSwitch  = &__after_scene_blink_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene_blink_background_start,        /* deprecated */
            //.fnOnBGComplete = &__on_scene_blink_background_complete,     /* deprecated */
            .fnOnFrameStart = &__on_scene_blink_frame_start,
            .fnBeforeSwitchOut = &__before_scene_blink_switching_out,
            .fnOnFrameCPL   = &__on_scene_blink_frame_complete,
            .fnDepose       = &__on_scene_blink_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_blink_t begin ---------------*/

    /* initialize Eye Socket */
    do {

        spin_zoom_widget_cfg_t tCFG = {
            .Indicator = {
                .LowerLimit = {
                    .fAngleInDegree = 0.0f,
                    .nValue = 0,
                },
                .UpperLimit = {
                    .fAngleInDegree = 360.0f,
                    .nValue = 3600,
                },
                .Step = {
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
            .Source = {
                .ptMask = &c_tileLeftEyeMask,
                .tCentre = (arm_2d_location_t){
                    .iX = c_tileLeftEyeMask.tRegion.tSize.iWidth >> 1,
                    .iY = c_tileLeftEyeMask.tRegion.tSize.iHeight
                        - (c_tileLeftEyeMask.tRegion.tSize.iHeight >> 2), 
                },
                .tColourToFill = GLCD_COLOR_WHITE,
            },
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.Eye.tSocket, &tCFG);
    } while(0);

#if ARM_2D_DEMO_BLINK_USE_QOI
    /* initialize QOI loader */
    do {
    #if ARM_2D_DEMO_QOI_USE_FILE
        arm_qoi_io_file_loader_init(&this.LoaderIO.tFile, "../common/asset/Eyeball.qoi");
    #else
        extern const uint8_t c_qoiEyeball[16840];

        arm_qoi_io_binary_loader_init(&this.LoaderIO.tBinary, c_qoiEyeball, sizeof(c_qoiEyeball));
    #endif
        arm_qoi_loader_cfg_t tCFG = {
            .bUseHeapForVRES = true,
            .ptScene = (arm_2d_scene_t *)ptThis,
            .u2WorkMode = ARM_QOI_MODE_PARTIAL_DECODED,

        #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .tColourInfo.chScheme = ARM_2D_COLOUR_CCCA8888,
        #endif

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

        arm_qoi_loader_init(&this.tQOIEyeball, &tCFG);
    } while(0);
#endif

    /* initialize Eyeball */
    do {
        spin_zoom_widget_cfg_t tCFG = {
            .Indicator = {
                .LowerLimit = {
                    .fAngleInDegree = 0.0f,
                    .nValue = 0,
                },
                .UpperLimit = {
                    .fAngleInDegree = 360.0f,
                    .nValue = 3600,
                },
                .Step = {
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
        #if ARM_2D_DEMO_BLINK_USE_QOI
            .ptTransformMode = &SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK,
        #else
            .ptTransformMode = &SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK_AND_SOURCE_MASK,
        #endif
            .Source = {
                .ptMask = &c_tileLeftEyeMask,
                .tCentre = (arm_2d_location_t){
                    .iX = c_tileLeftEyeMask.tRegion.tSize.iWidth >> 1,
                    .iY = c_tileLeftEyeMask.tRegion.tSize.iHeight
                        - (c_tileLeftEyeMask.tRegion.tSize.iHeight >> 2), 
                },
                .tColourToFill = GLCD_COLOR_RED,
            },
            .Extra = {
            #if ARM_2D_DEMO_BLINK_USE_QOI
                .ptTile = &this.tQOIEyeball.vres.tTile,
            #else
                .ptTile = &c_tileEyeball,
                .ptMask = &c_tileEyeballMask,
            #endif
            },

            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.Eye.tEyeBall, &tCFG);
    } while(0);

    /* Blink */
    do {
        this.Blink.chPT = 0;
        this.Blink.iEyelidOffset = 100;
        this.Blink.chRatio = 32;
        this.Blink.chDoubleBlinkRatio = 64;
        this.Blink.chDelayAfterBlinkingIn100MS = 20;

        this.ForcusGenerator.chDelayAfterEachMoveIn100MS = 5;
        this.ForcusGenerator.chPT = 0;
        this.ForcusGenerator.chRatio = 64;
    } while(0);

    /* ------------   initialize members of user_scene_blink_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


