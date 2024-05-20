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

#ifndef __ARM_2D_SCENE_AUDIOMARK_H__
#define __ARM_2D_SCENE_AUDIOMARK_H__

/*============================ INCLUDES ======================================*/

#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include "arm_2d.h"

#include "arm_2d_helper_scene.h"
#include "arm_2d_example_controls.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wembedded-directive"
#   pragma clang diagnostic ignored "-Wpadded"
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wpadded"
#endif

/*============================ MACROS ========================================*/

/* OOC header, please DO NOT modify  */
#ifdef __USER_SCENE_AUDIOMARK_IMPLEMENT__
#   undef __USER_SCENE_AUDIOMARK_IMPLEMENT__
#   define __ARM_2D_IMPL__
#endif
#include "arm_2d_utils.h"


#ifndef __FITNESS_CFG_NEBULA_ENABLE__
#   define __FITNESS_CFG_NEBULA_ENABLE__        0
#endif

#if __FITNESS_CFG_NEBULA_ENABLE__
#   define NEBULA_PARTICLE_COUNT                200
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/*!
 * \brief initalize scene_audiomark and add it to a user specified scene player
 * \param[in] __DISP_ADAPTER_PTR the target display adapter (i.e. scene player)
 * \param[in] ... this is an optional parameter. When it is NULL, a new 
 *            user_scene_audiomark_t will be allocated from HEAP and freed on
 *            the deposing event. When it is non-NULL, the life-cycle is managed
 *            by user.
 * \return user_scene_audiomark_t* the user_scene_audiomark_t instance
 */
#define arm_2d_scene_audiomark_init(__DISP_ADAPTER_PTR, ...)                    \
            __arm_2d_scene_audiomark_init((__DISP_ADAPTER_PTR), (NULL, ##__VA_ARGS__))

/*============================ TYPES =========================================*/
enum {
    AUDIOMARK_CORTEX_M4,
    AUDIOMARK_CORTEX_M33,
    AUDIOMARK_CORTEX_M7,
    AUDIOMARK_CORTEX_M85_SCALER,
    AUDIOMARK_CORTEX_M55_HELIUM,
    AUDIOMARK_CORTEX_M85_HELIUM,
    __AUDIOMARK_PROCESSOR_NUM,
};

/*!
 * \brief a user class for scene audiomark
 */
typedef struct user_scene_audiomark_t user_scene_audiomark_t;

struct user_scene_audiomark_t {
    implement(arm_2d_scene_t);                                                  //! derived from class: arm_2d_scene_t

ARM_PRIVATE(
    /* place your private member here, following two are examples */
    int64_t lTimestamp[3];
    bool bUserAllocated;

#if __FITNESS_CFG_NEBULA_ENABLE__
    dynamic_nebula_t tNebula;
    dynamic_nebula_particle_t tParticles[NEBULA_PARTICLE_COUNT];
#endif

    struct {
        progress_wheel_t tWheel;
        int16_t iProgress;
        int64_t lTimestamp;
        const char *pchName;
    } Processor[__AUDIOMARK_PROCESSOR_NUM];
)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

ARM_NONNULL(1)
extern
user_scene_audiomark_t *__arm_2d_scene_audiomark_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_audiomark_t *ptScene);

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif

#endif
