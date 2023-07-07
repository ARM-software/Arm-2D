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

#ifndef __DYNAMIC_NEBULA_H__
#define __DYNAMIC_NEBULA_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d_helper.h"

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
#ifdef __DYNAMIC_NEBULA_IMPLEMENT__
#   undef   __DYNAMIC_NEBULA_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__DYNAMIC_NEBULA_INHERIT__)
#   undef   __DYNAMIC_NEBULA_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct dynamic_nebula_particle_t {
ARM_PRIVATE(
    float fSin;
    float fCos;
    float fOffset;
)
} dynamic_nebula_particle_t;

typedef struct dynamic_nebula_cfg_t {
    int16_t                     iRadius;
    int16_t                     iVisibleRingWidth;
    float                       fSpeed;
    uint16_t                    hwParticleCount;
    dynamic_nebula_particle_t  *ptParticles;
} dynamic_nebula_cfg_t;

/*!
 * \brief a class for dynamic nebula
 */
typedef struct dynamic_nebula_t dynamic_nebula_t;

struct dynamic_nebula_t {

ARM_PRIVATE(
    dynamic_nebula_cfg_t tCFG;
    int64_t lTimestamp[1];
    float fOpacityStep;
)
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void dynamic_nebula_init( dynamic_nebula_t *ptThis, dynamic_nebula_cfg_t *ptCFG);

extern
ARM_NONNULL(1)
void dynamic_nebula_depose( dynamic_nebula_t *ptThis);

extern
ARM_NONNULL(1)
void dynamic_nebula_show(   dynamic_nebula_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion,
                            COLOUR_INT tColour,
                            uint8_t chOpacity,
                            bool bIsNewFrame);


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
