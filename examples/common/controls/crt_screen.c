/*
 * Copyright (c) 2009-2025 Arm Limited. All rights reserved.
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
#define __CRT_SCREEN_IMPLEMENT__

#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "crt_screen.h"
#include <assert.h>
#include <string.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileWhiteNoiseBar      c_tileWhiteNoiseGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileWhiteNoiseBar      c_tileWhiteNoiseRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileWhiteNoiseBar      c_tileWhiteNoiseCCCN888
#else
#   error Unsupported colour depth!
#endif

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern const arm_2d_tile_t c_tileWhiteNoiseBar;
extern const arm_2d_tile_t c_tileWhiteNoiseGRAY8;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
void crt_screen_init(   crt_screen_t *ptThis,
                        crt_screen_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);

    memset(ptThis, 0, sizeof(crt_screen_t));
    this.tCRTCFG = *ptCFG;

    this.tWhiteNoise = (arm_2d_tile_t)
        impl_child_tile(c_tileWhiteNoiseBar, 0, 0, 32, 16);
    
    if (0 == this.tCRTCFG.chNoiseLasts) {
        this.tCRTCFG.chNoiseLasts = 8;
    }

    if (this.tCRTCFG.bShowScanningEffect) {
        /* validate scan bar 0 parameters */
        if (0 == this.tCRTCFG.ScanBar[0].chOpacity) {
            this.tCRTCFG.ScanBar[0].chOpacity = 64;
        }
        if (0 == this.tCRTCFG.ScanBar[0].chBarHeight) {
            this.tCRTCFG.ScanBar[0].chBarHeight = 32;
        }
        if (0 == this.tCRTCFG.ScanBar[0].hwPeriodInMs) {
            this.tCRTCFG.ScanBar[0].hwPeriodInMs = 2000;
        }

        /* validate scan bar 1 parameters */
        if (0 == this.tCRTCFG.ScanBar[1].chOpacity) {
            this.tCRTCFG.ScanBar[1].chOpacity = 128;
        }
        if (0 == this.tCRTCFG.ScanBar[1].chBarHeight) {
            this.tCRTCFG.ScanBar[1].chBarHeight = 4;
        }
        if (0 == this.tCRTCFG.ScanBar[1].hwPeriodInMs) {
            this.tCRTCFG.ScanBar[1].hwPeriodInMs = 333;
        }
    }

    image_box_init(&this.use_as__image_box_t, ptCFG->ptImageBoxCFG);
}

ARM_NONNULL(1)
void crt_screen_depose( crt_screen_t *ptThis)
{
    assert(NULL != ptThis);

    image_box_depose(&this.use_as__image_box_t);
}

ARM_NONNULL(1)
void crt_screen_on_load( crt_screen_t *ptThis)
{
    assert(NULL != ptThis);
    image_box_on_load(&this.use_as__image_box_t);
}

ARM_NONNULL(1)
void crt_screen_on_frame_start( crt_screen_t *ptThis)
{
    assert(NULL != ptThis);

    image_box_on_frame_start(&this.use_as__image_box_t);

    if (this.tCRTCFG.chWhiteNoiseRatio > 0) {
        /* update random generator */
        srand(arm_2d_helper_get_system_timestamp());

        int16_t iWidth = (rand() % 64) + 16;
        int16_t iHeight = (rand() % 8) + 8;
        int16_t iX = rand() % (c_tileWhiteNoiseBar.tRegion.tSize.iWidth - iWidth);
        int16_t iY = rand() % (c_tileWhiteNoiseBar.tRegion.tSize.iHeight - iHeight);

        this.tWhiteNoise.tRegion.tLocation.iX = iX;
        this.tWhiteNoise.tRegion.tLocation.iY = iY;
        this.tWhiteNoise.tRegion.tSize.iWidth = iWidth;
        this.tWhiteNoise.tRegion.tSize.iHeight = iHeight;

        if (0 == this.chWhiteNoiseVisibleFrameCounter) {
            if (((rand() & 0xFFF) < this.tCRTCFG.chWhiteNoiseRatio)) {
                this.chWhiteNoiseVisibleFrameCounter = (rand() % this.tCRTCFG.chNoiseLasts) + 1;
            }
        } else {
            this.chWhiteNoiseVisibleFrameCounter--;
        }
    }

    do {
        int16_t iHeight = image_box_get_target_region_size(&this.use_as__image_box_t).iHeight;
        if (0 == iHeight) {
            break;
        }

        if (this.tCRTCFG.bShowScanningEffect) {
            
            int32_t iResult = 0;
            if (arm_2d_helper_time_liner_slider(
                    -this.tCRTCFG.ScanBar[0].chBarHeight, 
                    iHeight,
                    this.tCRTCFG.ScanBar[0].hwPeriodInMs,
                    &iResult,
                    &this.ScanBar[0].lTimestamp)) {
                /* reset */
                this.ScanBar[0].lTimestamp = 0;
            }
            this.ScanBar[0].iYOffset = iResult;

            if (arm_2d_helper_time_liner_slider(
                    -this.tCRTCFG.ScanBar[1].chBarHeight, 
                    iHeight,
                    this.tCRTCFG.ScanBar[1].hwPeriodInMs,
                    &iResult,
                    &this.ScanBar[1].lTimestamp)) {
                /* reset */
                this.ScanBar[1].lTimestamp = 0;
            }
            this.ScanBar[1].iYOffset = iResult;

        }
    } while(0);
}

ARM_NONNULL(1)
void crt_screen_on_frame_complete( crt_screen_t *ptThis)
{
    assert(NULL != ptThis);

    image_box_on_frame_complete(&this.use_as__image_box_t);
}

ARM_NONNULL(1)
void crt_screen_show(   crt_screen_t *ptThis,
                        const arm_2d_tile_t *ptTile, 
                        const arm_2d_region_t *ptRegion, 
                        uint8_t chOpacity,
                        bool bIsNewFrame)
{
    assert(NULL!= ptThis);

    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }

    arm_2d_container(ptTile, __crt_screen, ptRegion) {

        do {
            if (this.chWhiteNoiseVisibleFrameCounter > 0 && this.tCRTCFG.bStrongNoise) {
                break;
            }
            image_box_show( &this.use_as__image_box_t,
                            &__crt_screen,
                            &__crt_screen_canvas,
                            chOpacity,
                            bIsNewFrame);

        } while(0);
        
        if (this.chWhiteNoiseVisibleFrameCounter > 0) {
            arm_2d_tile_fill_with_src_mask_and_opacity_only (   &this.tWhiteNoise,
                                                                &c_tileWhiteNoiseGRAY8,
                                                                &__crt_screen, 
                                                                &__crt_screen_canvas,
                                                                chOpacity);
        }

        if (this.tCRTCFG.bShowScanningEffect) {
        
            for (int_fast8_t n = 0; n < dimof(this.ScanBar); n++) {

                arm_2d_region_t tScanBar = {
                    .tSize = {
                        .iHeight = this.tCRTCFG.ScanBar[n].chBarHeight,
                        .iWidth = image_box_get_target_region_size(&this.use_as__image_box_t).iWidth,
                    },
                    .tLocation.iY = this.ScanBar[n].iYOffset,
                };

                uint8_t chScanBarOpacity = arm_2d_helper_alpha_mix(chOpacity, this.tCRTCFG.ScanBar[n].chOpacity);

                arm_2d_fill_colour_with_opacity(&__crt_screen, 
                                                &tScanBar, 
                                                (__arm_2d_color_t){this.tCRTCFG.tScanBarColour.tColour},
                                                chScanBarOpacity);

            }
        }

    }

    ARM_2D_OP_WAIT_ASYNC();
}

ARM_NONNULL(1)
bool crt_screen_is_showing_noise( crt_screen_t *ptThis)
{
    assert(NULL != ptThis);

    return this.chWhiteNoiseVisibleFrameCounter > 0;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
