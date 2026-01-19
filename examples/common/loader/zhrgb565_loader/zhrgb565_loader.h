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

#ifndef __ARM_ZHRGB565_LOADER_H__
#define __ARM_ZHRGB565_LOADER_H__

/*============================ INCLUDES ======================================*/
#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB) && defined(RTE_Acceleration_Arm_2D_Extra_Loader)

#include "arm_2d_helper.h"
#include "arm_2d_example_loaders.h"

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
#ifdef __ZHRGB565_LOADER_IMPLEMENT__
#   undef   __ZHRGB565_LOADER_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__ZHRGB565_LOADER_INHERIT__)
#   undef   __ZHRGB565_LOADER_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


typedef struct arm_zhrgb565_loader_cfg_t {

    uint16_t bUseHeapForVRES        : 1;

    struct {
        const arm_loader_io_t *ptIO;
        uintptr_t pTarget;
    } ImageIO;

    arm_2d_scene_t *ptScene;
} arm_zhrgb565_loader_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct arm_zhrgb565_loader_t arm_zhrgb565_loader_t;

struct arm_zhrgb565_loader_t {
    union {
        arm_2d_tile_t tTile;
        inherit(arm_generic_loader_t);
    };
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1, 2)
arm_2d_err_t arm_zhrgb565_loader_init(arm_zhrgb565_loader_t *ptThis,
                                arm_zhrgb565_loader_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void arm_zhrgb565_loader_depose( arm_zhrgb565_loader_t *ptThis);

extern
ARM_NONNULL(1)
void arm_zhrgb565_loader_on_load( arm_zhrgb565_loader_t *ptThis);

extern
ARM_NONNULL(1)
void arm_zhrgb565_loader_on_frame_start( arm_zhrgb565_loader_t *ptThis);

extern
ARM_NONNULL(1)
void arm_zhrgb565_loader_on_frame_complete( arm_zhrgb565_loader_t *ptThis);



#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#else

#define arm_zhrgb565_loader_init(...)                 ARM_2D_ERR_NOT_AVAILABLE
#define arm_zhrgb565_loader_depose(...)
#define arm_zhrgb565_loader_on_load(...)
#define arm_zhrgb565_loader_on_frame_start(...)
#define arm_zhrgb565_loader_on_frame_complete(...)

#endif 

#endif
