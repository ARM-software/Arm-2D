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
#define __IMAGE_BOX_IMPLEMENT__

#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "image_box.h"
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


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1)
void image_box_init(image_box_t *ptThis,
                    image_box_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    memset(ptThis, 0, sizeof(image_box_t));

    if (NULL != ptCFG) {
        this.tCFG = *ptCFG;
    }

    do {
        this.tCFG.__bShowGrayScale = false;

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
            this.tCFG.__bShowGrayScale = true;
        }

        if (this.tCFG.fXRatio == 0 && this.tCFG.fYRatio == 0) {
            /* auto fit */
            if (this.tCFG.u2Mode == IMG_BOX_MODE_MANUAL) {
                this.tCFG.u2Mode = IMG_BOX_MODE_AUTO_CLIP;
            }

        } else if (this.tCFG.fXRatio == 0) {
            this.tCFG.fXRatio = this.tCFG.fYRatio;
        } else if (this.tCFG.fYRatio == 0) {
            this.tCFG.fYRatio = this.tCFG.fXRatio;
        }
    } while(0);

#if !defined(RTE_Acceleration_Arm_2D_Transform)
    this.tCFG.__bNoScaling = true;
#endif
    /* initialize op */
    ARM_2D_OP_INIT(this.OPCODE);
}

ARM_NONNULL(1)
void image_box_depose( image_box_t *ptThis)
{
    assert(NULL != ptThis);

    /* depose op */
    ARM_2D_OP_DEPOSE(this.OPCODE);
}

ARM_NONNULL(1)
void image_box_on_load( image_box_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void image_box_on_frame_start( image_box_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void image_box_on_frame_complete( image_box_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
arm_2d_size_t image_box_get_target_region_size(image_box_t *ptThis)
{
    assert(NULL != ptThis);

    return this.tTargetRegionSize;
}

ARM_NONNULL(1)
void image_box_show(image_box_t *ptThis,
                    const arm_2d_tile_t *ptTile, 
                    const arm_2d_region_t *ptRegion, 
                    uint8_t chOpacity,
                    bool bIsNewFrame)
{
    assert(NULL!= ptThis);

    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }

    arm_2d_container(ptTile, __image_box, ptRegion) {

        if (bIsNewFrame) {
            bool bRegionChanged = false;
            if (    (__image_box_canvas.tSize.iHeight != this.tTargetRegionSize.iHeight)
               ||   (__image_box_canvas.tSize.iWidth != this.tTargetRegionSize.iWidth)) {
                /* no need to re-calculate */
                bRegionChanged = true;
                this.tTargetRegionSize = __image_box_canvas.tSize;
            }
            
            bool bAutoFit = this.tCFG.u2Mode != IMG_BOX_MODE_MANUAL;

            if (bRegionChanged && bAutoFit && (NULL != this.tCFG.ptilePhoto || NULL != this.tCFG.ptilePhotoMask)) {

                float fXRatio = (float)__image_box_canvas.tSize.iWidth
                              / this.tCFG.ptilePhoto->tRegion.tSize.iWidth;
                float fYRatio = (float)__image_box_canvas.tSize.iHeight
                              / this.tCFG.ptilePhoto->tRegion.tSize.iHeight;
                
                if (this.tCFG.u2Mode == IMG_BOX_MODE_STRETCH) {
                    this.tCFG.fXRatio = fXRatio;
                    this.tCFG.fYRatio = fYRatio;
                } else {
                    this.tCFG.fXRatio = MAX(fXRatio, fYRatio);
                    this.tCFG.fYRatio = this.tCFG.fXRatio;
                }

            #if defined(RTE_Acceleration_Arm_2D_Transform)
                if (fabs(fXRatio - 1.0f) <= 0.01 && fabs(fYRatio - 1.0f) <= 0.01) {
                    this.tCFG.__bNoScaling = true;
                } else {
                    this.tCFG.__bNoScaling = false;
                }
            #endif
            }
        }

        /* draw photo */
        if (NULL != this.tCFG.ptilePhoto) {
            do {
                if (this.tCFG.__bShowGrayScale) {
                    
                    if (this.tCFG.__bNoScaling) {
                        arm_2d_fill_colour_with_mask_and_opacity(  
                                                &__image_box,
                                                &__image_box_canvas,
                                                this.tCFG.ptilePhoto,
                                                (__arm_2d_color_t){this.tCFG.tScreenColour.tColour},
                                                chOpacity);
                    #if defined(RTE_Acceleration_Arm_2D_Transform)
                    } else {
                    
                        arm_2d_point_float_t tCentre = {
                            .fX = this.tCFG.ptilePhoto->tRegion.tSize.iWidth / 2.0,
                            .fY = this.tCFG.ptilePhoto->tRegion.tSize.iHeight / 2.0,
                        };
                        arm_2dp_fill_colour_with_mask_opacity_and_transform_xy(
                                            &this.OPCODE.tFillColourTransform,
                                            this.tCFG.ptilePhoto,
                                            &__image_box,
                                            &__image_box_canvas,
                                            tCentre,
                                            0.0f,
                                            this.tCFG.fXRatio,
                                            this.tCFG.fYRatio,
                                            this.tCFG.tScreenColour.tColour,
                                            chOpacity);
                        ARM_2D_OP_WAIT_ASYNC(&this.OPCODE.tFillColourTransform);
                    #endif
                    }

                } else if (NULL == this.tCFG.ptilePhotoMask) {

                    if (this.tCFG.__bNoScaling) {
                        if (255 == chOpacity) {
                            arm_2d_tile_copy_only(  this.tCFG.ptilePhoto,
                                                    &__image_box,
                                                    &__image_box_canvas);
                        } else {
                            arm_2d_tile_copy_with_opacity_only(  
                                                    this.tCFG.ptilePhoto,
                                                    &__image_box,
                                                    &__image_box_canvas,
                                                    chOpacity);
                        }
                #if defined(RTE_Acceleration_Arm_2D_Transform)
                    } else {
                        arm_2d_point_float_t tCentre = {
                            .fX = this.tCFG.ptilePhoto->tRegion.tSize.iWidth / 2.0,
                            .fY = this.tCFG.ptilePhoto->tRegion.tSize.iHeight / 2.0,
                        };
                        arm_2dp_tile_transform_xy_only_with_opacity(
                                            &this.OPCODE.tTileTransOpa,
                                            this.tCFG.ptilePhoto,
                                            &__image_box,
                                            &__image_box_canvas,
                                            tCentre,
                                            0.0f,
                                            this.tCFG.fXRatio,
                                            this.tCFG.fYRatio,
                                            chOpacity);

                        ARM_2D_OP_WAIT_ASYNC(&this.OPCODE.tTileTransOpa);
                #endif
                    }

                    
                } else {
                    if (this.tCFG.__bNoScaling) {
                        arm_2d_tile_copy_with_src_mask_and_opacity_only(  
                                                this.tCFG.ptilePhoto,
                                                this.tCFG.ptilePhotoMask,
                                                &__image_box,
                                                &__image_box_canvas,
                                                chOpacity);
                #if defined(RTE_Acceleration_Arm_2D_Transform)
                    } else {
                        arm_2d_point_float_t tCentre = {
                            .fX = this.tCFG.ptilePhoto->tRegion.tSize.iWidth / 2.0,
                            .fY = this.tCFG.ptilePhoto->tRegion.tSize.iHeight / 2.0,
                        };
                        arm_2dp_tile_transform_xy_with_src_mask_and_opacity(
                                            &this.OPCODE.tTileTransSrcMaskOpa,
                                            this.tCFG.ptilePhoto,
                                            this.tCFG.ptilePhotoMask,
                                            &__image_box,
                                            &__image_box_canvas,
                                            tCentre,
                                            0.0f,
                                            this.tCFG.fXRatio,
                                            this.tCFG.fYRatio,
                                            chOpacity);

                        ARM_2D_OP_WAIT_ASYNC(&this.OPCODE.tTileTransSrcMaskOpa);
                #endif
                    }
                }
            } while(0);
        } else if (NULL != this.tCFG.ptilePhotoMask) {
             if (this.tCFG.__bNoScaling) {
                arm_2d_fill_colour_with_mask_and_opacity(  
                                        &__image_box,
                                        &__image_box_canvas,
                                        this.tCFG.ptilePhotoMask,
                                        (__arm_2d_color_t){this.tCFG.tScreenColour.tColour},
                                        chOpacity);
        #if defined(RTE_Acceleration_Arm_2D_Transform)
            } else {
                arm_2d_point_float_t tCentre = {
                        .fX = this.tCFG.ptilePhoto->tRegion.tSize.iWidth / 2.0,
                        .fY = this.tCFG.ptilePhoto->tRegion.tSize.iHeight / 2.0,
                    };

                arm_2dp_fill_colour_with_mask_opacity_and_transform_xy(
                                    &this.OPCODE.tFillColourTransform,
                                    this.tCFG.ptilePhotoMask,
                                    &__image_box,
                                    &__image_box_canvas,
                                    tCentre,
                                    0.0f,
                                    this.tCFG.fXRatio,
                                    this.tCFG.fYRatio,
                                    this.tCFG.tScreenColour.tColour,
                                    chOpacity);

                ARM_2D_OP_WAIT_ASYNC(&this.OPCODE.tFillColourTransform);
        #endif
            }

        }
    }

    ARM_2D_OP_WAIT_ASYNC();
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
