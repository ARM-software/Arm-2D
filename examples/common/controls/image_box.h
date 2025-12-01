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

#ifndef __IMAGE_BOX_H__
#define __IMAGE_BOX_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__common.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/*============================ MACROS ========================================*/

/* OOC header, please DO NOT modify  */
#ifdef __IMAGE_BOX_IMPLEMENT__
#   undef   __IMAGE_BOX_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__IMAGE_BOX_INHERIT__)
#   undef   __IMAGE_BOX_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    IMG_BOX_MODE_AUTO_CLIP,
    IMG_BOX_MODE_STRETCH,
    IMG_BOX_MODE_MANUAL,
};

typedef struct image_box_cfg_t {

    const arm_2d_tile_t *ptilePhoto;
    const arm_2d_tile_t *ptilePhotoMask;
    float fXRatio;
    float fYRatio;

    /*! \note When ptilePhoto points to a tile having enforced colour type GRAY8,
     *        it will be used as an A8 mask for colour-filling. In such a case, we 
     *        use tScreenColour to specify the colour.
     */
    COLOUR_TYPE_T tScreenColour;
    uint8_t u2Mode              : 2;                                            /* IMG_BOX_MODE_xxxx */
    uint8_t                     : 4;
    uint8_t __bNoScaling        : 1;                                            /* please ignore this flag */
    uint8_t __bShowGrayScale    : 1;                                            /* please ignore this flag */

} image_box_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct image_box_t image_box_t;

struct image_box_t {

ARM_PRIVATE(

    image_box_cfg_t tCFG;
    arm_2d_size_t tTargetRegionSize;

    union {
        arm_2d_op_fill_cl_msk_opa_trans_t   tFillColourTransform;
        arm_2d_op_trans_msk_opa_t           tTileTransSrcMaskOpa;
        arm_2d_op_trans_opa_t               tTileTransOpa;
    } OPCODE;

)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void image_box_init( image_box_t *ptThis,
                     image_box_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void image_box_depose( image_box_t *ptThis);

extern
ARM_NONNULL(1)
void image_box_on_load( image_box_t *ptThis);

extern
ARM_NONNULL(1)
void image_box_on_frame_start( image_box_t *ptThis);

extern
ARM_NONNULL(1)
void image_box_on_frame_complete( image_box_t *ptThis);

extern
ARM_NONNULL(1)
arm_2d_size_t image_box_get_target_region_size(image_box_t *ptThis);

extern
ARM_NONNULL(1)
void image_box_show(image_box_t *ptThis,
                    const arm_2d_tile_t *ptTile, 
                    const arm_2d_region_t *ptRegion, 
                    uint8_t chOpacity,
                    bool bIsNewFrame);


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
