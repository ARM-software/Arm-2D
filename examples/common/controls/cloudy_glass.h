/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
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

#ifndef __CLOUDY_GLASS_H__
#define __CLOUDY_GLASS_H__

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
#ifdef __CLOUDY_GLASS_IMPLEMENT__
#   undef   __CLOUDY_GLASS_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__CLOUDY_GLASS_INHERIT__)
#   undef   __CLOUDY_GLASS_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


typedef struct cloudy_glass_cfg_t {
    arm_2d_scene_t *ptScene;

    arm_2d_size_t tSize;
    dynamic_nebula_particle_t *ptParticles;
    uint16_t hwParticleCount;

    COLOUR_INT_TYPE tColour;

} cloudy_glass_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct cloudy_glass_t cloudy_glass_t;

struct cloudy_glass_t {

ARM_PRIVATE(

    cloudy_glass_cfg_t tCFG;

    /* place your private member here, following two are examples */
    int64_t lTimestamp[1];

    arm_2d_helper_dirty_region_item_t tDirtyRegionItem;

    arm_2d_filter_iir_blur_descriptor_t tBlurOP;
    dynamic_nebula_t tNebula;
)
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void cloudy_glass_init( cloudy_glass_t *ptThis,
                        cloudy_glass_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void cloudy_glass_depose( cloudy_glass_t *ptThis);

extern
ARM_NONNULL(1)
void cloudy_glass_on_load( cloudy_glass_t *ptThis);

extern
ARM_NONNULL(1)
void cloudy_glass_on_frame_start( cloudy_glass_t *ptThis);

extern
ARM_NONNULL(1)
void cloudy_glass_set_colour( cloudy_glass_t *ptThis, 
                                COLOUR_INT_TYPE tColour);

extern
ARM_NONNULL(1)
void cloudy_glass_show(   cloudy_glass_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion);


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
