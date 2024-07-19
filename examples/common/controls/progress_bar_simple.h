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

#ifndef __PROGRESS_BAR_SIMPLE_H__
#define __PROGRESS_BAR_SIMPLE_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define progress_bar_simple_show4(   __target_ptr,                              \
                                    __region_ptr,                               \
                                    __progress,                                 \
                                    __is_new_frame)                             \
            __progress_bar_simple_show( (__target_ptr),                         \
                                        (__region_ptr),                         \
                                        (int_fast16_t)(__progress))

#define progress_bar_simple_show3(   __target_ptr,                              \
                                    __region_ptr,                               \
                                    __progress)                                 \
            __progress_bar_simple_show( (__target_ptr),                         \
                                        (__region_ptr),                         \
                                        (int_fast16_t)(__progress))


#define progress_bar_simple_show2(  __target_ptr,                               \
                                    __progress)                                 \
            progress_bar_simple_show3(  (__target_ptr),                         \
                                        NULL,                                   \
                                        (int_fast16_t)(__progress))

#define progress_bar_simple_show1(__progress)                                   \
            progress_bar_simple_show3(   ((const arm_2d_tile_t *)(-1))),        \
                                        NULL,                                   \
                                        (int_fast16_t)(__progress))

#define progress_bar_simple_show0()                                             \
            progress_bar_simple_show3(   ((const arm_2d_tile_t *)(-1))),        \
                                        NULL,                                   \
                                        (-1))

#define progress_bar_simple_show(...)                                           \
            ARM_CONNECT2(   progress_bar_simple_show,                           \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
void progress_bar_simple_init(void);

extern
void __progress_bar_simple_show(const arm_2d_tile_t *ptTarget, 
                                const arm_2d_region_t *ptRegion,
                                int_fast16_t iProgress);

#ifdef   __cplusplus
}
#endif

#endif
