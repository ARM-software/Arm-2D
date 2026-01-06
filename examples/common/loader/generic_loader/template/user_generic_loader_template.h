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

#ifndef __<NAME>_H__
#define __<NAME>_H__

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
#ifdef __<NAME>_IMPLEMENT__
#   undef   __<NAME>_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__<NAME>_INHERIT__)
#   undef   __<NAME>_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


typedef struct <name>_cfg_t {

    arm_2d_size_t tSize;

    uint16_t bUseHeapForVRES        : 1;
    uint16_t bAntiAlias             : 1;

    struct {
        const arm_loader_io_t *ptIO;
        uintptr_t pTarget;
    } ImageIO;

    arm_2d_scene_t *ptScene;
} <name>_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct <name>_t <name>_t;

struct <name>_t {

    union {
        arm_2d_tile_t tTile;
        inherit(arm_generic_loader_t);
    };

ARM_PRIVATE(
    <name>_cfg_t tCFG;
)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1, 2)
arm_2d_err_t <name>_init(<name>_t *ptThis,
                                <name>_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void <name>_depose( <name>_t *ptThis);

extern
ARM_NONNULL(1)
void <name>_on_load( <name>_t *ptThis);

extern
ARM_NONNULL(1)
void <name>_on_frame_start( <name>_t *ptThis);

extern
ARM_NONNULL(1)
void <name>_on_frame_complete( <name>_t *ptThis);



#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#else

#define <name>_init(...)                 ARM_2D_ERR_NOT_AVAILABLE
#define <name>_depose(...)
#define <name>_on_load(...)
#define <name>_on_frame_start(...)
#define <name>_on_frame_complete(...)

#endif 

#endif
