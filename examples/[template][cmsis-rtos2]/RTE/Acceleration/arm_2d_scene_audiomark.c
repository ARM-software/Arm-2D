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

#define __USER_SCENE_AUDIOMARK_IMPLEMENT__
#include "arm_2d_scene_audiomark.h"

#include "arm_2d_helper.h"

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


extern const arm_2d_tile_t c_tileBigWhiteDotMask;
extern const arm_2d_tile_t c_tileQuaterArcBigMask;


/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

#define PROCESSOR_INFO(__NAME, __SIZE, __SCORE, __COLOUR, __SCORE_COLOUR)       \
            {__NAME, (__SIZE), (__SCORE), (__COLOUR), (__SCORE_COLOUR)}

static const struct {
    const char  *pchName;
    int16_t     iWheelSize;
    int16_t     iAudioMark;
    COLOUR_INT  tColour;
    COLOUR_INT  tScoreColour;
} c_tProcessorInfo[] = {
    [AUDIOMARK_CORTEX_M4]           
        = PROCESSOR_INFO( "Cortex-M4",           100 + 75 * 0,    73,     __RGB(0x40, 0x40, 0x40),  __RGB(0x80, 0x80, 0x80) ),
    [AUDIOMARK_CORTEX_M33]          
        = PROCESSOR_INFO( "Cortex-M33",           65 + 75 * 1,    101,    __RGB(0x60, 0x60, 0x60),  __RGB(0xFF, 0xFF, 0xFF) ),
    [AUDIOMARK_CORTEX_M7]           
        = PROCESSOR_INFO( "Cortex-M7",            35 + 75 * 2,    132,    __RGB(0x80, 0x80, 0x80),  __RGB(0x80, 0x80, 0x80) ),
    [AUDIOMARK_CORTEX_M85_SCALER]   
        = PROCESSOR_INFO( "Cortex-M85 Scaler",    20 + 75 * 3,    201,    __RGB(0x00, 0x80, 0x00),  __RGB(0xFF, 0xFF, 0xFF) ),
    [AUDIOMARK_CORTEX_M55_HELIUM]   
        = PROCESSOR_INFO( "Cortex-M55 Helium",    20 + 75 * 4,    367,    __RGB(0x00, 0xC0, 0x00),  __RGB(0x00, 0xC0, 0x00) ),
    [AUDIOMARK_CORTEX_M85_HELIUM]   
        = PROCESSOR_INFO( "Cortex-M85 Helium",    40 + 75 * 5,    423,    GLCD_COLOR_GREEN,         __RGB(0xFF, 0x80, 0x00) ),
};

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_audiomark_on_load(arm_2d_scene_t *ptScene)
{
    user_scene_audiomark_t *ptThis = (user_scene_audiomark_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    for (uint_fast8_t n = 0; n < dimof(this.Processor); n++) {
        progress_wheel_on_load(&this.Processor[n].tWheel);
    }

}


static void __on_scene_audiomark_depose(arm_2d_scene_t *ptScene)
{
    user_scene_audiomark_t *ptThis = (user_scene_audiomark_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    for (int_fast8_t n = 0; n < dimof(this.Processor); n ++) {
        progress_wheel_depose(&this.Processor[n].tWheel);
        this.Processor[n].lTimestamp = 0;
    }
#if __FITNESS_CFG_NEBULA_ENABLE__
    dynamic_nebula_depose(&this.tNebula);
#endif

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene audiomark                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_audiomark_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_audiomark_t *ptThis = (user_scene_audiomark_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_audiomark_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_audiomark_t *ptThis = (user_scene_audiomark_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_audiomark_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_audiomark_t *ptThis = (user_scene_audiomark_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    for (int_fast8_t n = 0; n < dimof(this.Processor); n++) {
        
        int32_t nResult;
        if (arm_2d_helper_time_liner_slider(0, 
                                            1000, 
                                            5000ul * 100ul / (int32_t)c_tProcessorInfo[n].iAudioMark , 
                                            &nResult, 
                                            &this.Processor[n].lTimestamp)) {
            this.Processor[n].lTimestamp = 0;
        }

        this.Processor[n].iProgress = (int16_t)nResult;
    }
    for (uint_fast8_t n = 0; n < dimof(this.Processor); n++) {
        progress_wheel_on_frame_start(&this.Processor[n].tWheel);
    }

}

static void __on_scene_audiomark_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_audiomark_t *ptThis = (user_scene_audiomark_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /* switch to next scene after 10s */
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static void __before_scene_audiomark_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_audiomark_t *ptThis = (user_scene_audiomark_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_audiomark_handler)
{
    user_scene_audiomark_t *ptThis = (user_scene_audiomark_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    int32_t iResult;
    float fAngle;

    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(tScreenSize);
    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

        arm_2d_align_centre(__top_canvas, 415, 415) {
            
            for (int_fast8_t n = 0; n < dimof(this.Processor); n++) {

                progress_wheel_show(&this.Processor[n].tWheel,
                    ptTile, 
                    &__centre_region,       
                    this.Processor[n].iProgress,
                    255,
                    bIsNewFrame);
            }

        #if __FITNESS_CFG_NEBULA_ENABLE__
            /* show nebula */
            dynamic_nebula_show(&this.tNebula, 
                                ptTile, 
                                &__bottom_centre_region, 
                                GLCD_COLOR_WHITE, 
                                255,
                                bIsNewFrame);
        #endif
        }

    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_audiomark_t *__arm_2d_scene_audiomark_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_audiomark_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_audiomark_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_audiomark_t),
                                                        __alignof__(user_scene_audiomark_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_audiomark_t));

    *ptThis = (user_scene_audiomark_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 
        
            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_audiomark_on_load,
            .fnScene        = &__pfb_draw_scene_audiomark_handler,
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_audiomark_background_start,
            //.fnOnBGComplete = &__on_scene_audiomark_background_complete,
            .fnOnFrameStart = &__on_scene_audiomark_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_audiomark_switching_out,
            .fnOnFrameCPL   = &__on_scene_audiomark_frame_complete,
            .fnDepose       = &__on_scene_audiomark_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_audiomark_t begin ---------------*/

    for (uint_fast8_t n = 0; n < dimof(this.Processor); n++) {

        switch (n) {
            case AUDIOMARK_CORTEX_M4:
            case AUDIOMARK_CORTEX_M33:
            case AUDIOMARK_CORTEX_M7:
                do {
                    progress_wheel_cfg_t tCFG = {
                        .tDotColour     = GLCD_COLOR_WHITE,                 /* dot colour */
                        .tWheelColour   = c_tProcessorInfo[n].tColour,      /* arc colour */
                        .iWheelDiameter = c_tProcessorInfo[n].iWheelSize,   /* diameter, 0 means use the mask's original size */
                        .bUseDirtyRegions = true,                           /* use dirty regions */
                    };

                    progress_wheel_init(&this.Processor[n].tWheel, 
                                        &this.use_as__arm_2d_scene_t,
                                        &tCFG);
                } while(0);

                break;
            case AUDIOMARK_CORTEX_M55_HELIUM:
            case AUDIOMARK_CORTEX_M85_SCALER:
            case AUDIOMARK_CORTEX_M85_HELIUM:
                do {
                    progress_wheel_cfg_t tCFG = {
                        .ptileArcMask   = &c_tileQuaterArcBigMask,          /* mask for arc */
                        .ptileDotMask   = &c_tileBigWhiteDotMask,           /* mask for dot */
                        .tDotColour     = GLCD_COLOR_WHITE,                 /* dot colour */
                        .tWheelColour   = c_tProcessorInfo[n].tColour,      /* arc colour */
                        .iWheelDiameter = c_tProcessorInfo[n].iWheelSize,   /* diameter, 0 means use the mask's original size */
                        .bUseDirtyRegions = true,                           /* use dirty regions */
                    };

                    progress_wheel_init(&this.Processor[n].tWheel, 
                                        &this.use_as__arm_2d_scene_t,
                                        &tCFG);
                } while(0);
                break;
            default:
                assert(false);
                break;
        }

        this.Processor[n].pchName = c_tProcessorInfo[n].pchName;
        this.Processor[n].iProgress = 0;
    }

#if __FITNESS_CFG_NEBULA_ENABLE__
    do {
        dynamic_nebula_cfg_t tCFG = {
            .fSpeed = 1.0f,
            .iRadius = 80,
            .iVisibleRingWidth = 80,
            .hwParticleCount = dimof(this.tParticles),
            .ptParticles = this.tParticles,
        };
        dynamic_nebula_init(&this.tNebula, &tCFG);
    } while(0);
#endif

    /* ------------   initialize members of user_scene_audiomark_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

