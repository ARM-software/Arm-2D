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

#ifndef __ARM_2D_SCENE_BENCHMARK_WATCH_PANEL_COVER_H__
#define __ARM_2D_SCENE_BENCHMARK_WATCH_PANEL_COVER_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d.h"

#include "arm_2d_helper_scene.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
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
#ifdef __USER_SCENE_BENCHMARK_WATCH_PANEL_COVER_IMPLEMENT__
#   undef __USER_SCENE_BENCHMARK_WATCH_PANEL_COVER_IMPLEMENT__
#   define __ARM_2D_IMPL__
#endif
#include "arm_2d_utils.h"

/*============================ MACROFIED FUNCTIONS ===========================*/

/*!
 * \brief initalize benchmark watch panel and add it to a user specified scene player
 * \param[in] ... this is an optional parameter. When it is NULL, DISP0_ADAPTER
 *                will be used as default.
 * \return user_scene_benchmark_watch_panel_cover_t * the benchmark scene object
 */
#define arm_2d_run_benchmark(...)                                               \
            __arm_2d_run_benchmark((&DISP0_ADAPTER,##__VA_ARGS__))

/*============================ TYPES =========================================*/
/*!
 * \brief a user class for scene benchmark generic cover
 */
typedef struct user_scene_benchmark_watch_panel_cover_t
        user_scene_benchmark_watch_panel_cover_t;

struct user_scene_benchmark_watch_panel_cover_t {
    implement(arm_2d_scene_t);                                                  //! derived from class: arm_2d_scene_t

    ARM_PRIVATE(
        bool bUserAllocated;
    )

};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief run benchmark-generic on user specified display adatper
 * \param[in] ptDispAdapter the address of the target display adapter
 * \return user_scene_benchmark_watch_panel_cover_t * the benchmark scene object
 */
extern
ARM_NONNULL(1)
user_scene_benchmark_watch_panel_cover_t * 
    __arm_2d_run_benchmark(arm_2d_scene_player_t *ptDispAdapter);

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
