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
    this.tCFG = *ptCFG;

    this.tWhiteNoise = (arm_2d_tile_t)
        impl_child_tile(c_tileWhiteNoiseBar, 0, 0, 32, 16);

    do {
        this.tCFG.__bShowAsGrayScale = false;
        this.tCFG.__bAutoFit = false;

        if (NULL == this.tCFG.ptilePhoto) {
            break;
        }
        const arm_2d_tile_t *ptRootTile 
            = arm_2d_tile_get_root(this.tCFG.ptilePhoto, NULL, NULL);
        if (NULL == ptRootTile) {
            this.tCFG.ptilePhoto = NULL;
            break;
        }

        if (    ptRootTile->bHasEnforcedColour 
            &&  ptRootTile->tColourInfo.chScheme == ARM_2D_COLOUR_GRAY8) {
            this.tCFG.__bShowAsGrayScale = true;
        }

        if (this.tCFG.fXRatio == 0 && this.tCFG.fYRatio == 0) {
            /* auto fit */
            this.tCFG.__bAutoFit = true;
        } else if (this.tCFG.fXRatio == 0) {
            this.tCFG.fXRatio = this.tCFG.fYRatio;
        } else if (this.tCFG.fYRatio == 0) {
            this.tCFG.fYRatio = this.tCFG.fXRatio;
        }
    } while(0);

    

    /* initialize op */
    ARM_2D_OP_INIT(this.OPCODE);

}

ARM_NONNULL(1)
void crt_screen_depose( crt_screen_t *ptThis)
{
    assert(NULL != ptThis);

    /* depose op */
    ARM_2D_OP_DEPOSE(this.OPCODE);
}

ARM_NONNULL(1)
void crt_screen_on_load( crt_screen_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void crt_screen_on_frame_start( crt_screen_t *ptThis)
{
    assert(NULL != ptThis);

    if (this.tCFG.bShowWhiteNoise) {
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
    }
}

ARM_NONNULL(1)
void crt_screen_on_frame_complete( crt_screen_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void crt_screen_show( crt_screen_t *ptThis,
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

        if (bIsNewFrame && this.tCFG.__bAutoFit && NULL != this.tCFG.ptilePhoto) {

            do {
                if ((__crt_screen_canvas.tSize.iHeight == this.tTargetRegionSize.iHeight)
                &&  (__crt_screen_canvas.tSize.iWidth == this.tTargetRegionSize.iWidth)) {
                    /* no need to re-calculate */
                    break;
                }
                this.tTargetRegionSize = __crt_screen_canvas.tSize;

                float fXRatio = (float)__crt_screen_canvas.tSize.iWidth
                              / this.tCFG.ptilePhoto->tRegion.tSize.iWidth;
                float fYRatio = (float)__crt_screen_canvas.tSize.iHeight
                              / this.tCFG.ptilePhoto->tRegion.tSize.iHeight;
                
                this.tCFG.fXRatio = MAX(fXRatio, fYRatio);
                this.tCFG.fYRatio = this.tCFG.fXRatio;

            } while(0);

        }

        /* draw photo */
        if (NULL != this.tCFG.ptilePhoto) {

            arm_2d_point_float_t tCentre = {
                .fX = this.tCFG.ptilePhoto->tRegion.tSize.iWidth / 2.0,
                .fY = this.tCFG.ptilePhoto->tRegion.tSize.iHeight / 2.0,
            };

            if (this.tCFG.__bShowAsGrayScale) {

                /* draw pointer */
                arm_2dp_fill_colour_with_mask_opacity_and_transform_xy(
                                    &this.OPCODE.tFillColourTransform,
                                    this.tCFG.ptilePhoto,
                                    &__crt_screen,
                                    &__crt_screen_canvas,
                                    tCentre,
                                    0.0f,
                                    this.tCFG.fXRatio,
                                    this.tCFG.fYRatio,
                                    this.tCFG.tScreenColour.tColour,
                                    chOpacity);

            } else {
                arm_2dp_tile_transform_xy_only_with_opacity(
                                    &this.OPCODE.tTile,
                                    this.tCFG.ptilePhoto,
                                    &__crt_screen,
                                    &__crt_screen_canvas,
                                    tCentre,
                                    0.0f,
                                    this.tCFG.fXRatio,
                                    this.tCFG.fYRatio,
                                    chOpacity);

            }

        }

        if (this.tCFG.bShowWhiteNoise) {
            arm_2d_tile_fill_with_src_mask_and_opacity_only (   &this.tWhiteNoise,
                                                                &c_tileWhiteNoiseGRAY8,
                                                                &__crt_screen, 
                                                                &__crt_screen_canvas,
                                                                chOpacity);
        }

    }

    ARM_2D_OP_WAIT_ASYNC();
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
