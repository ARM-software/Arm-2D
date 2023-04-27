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

#ifndef __PROGRESS_BAR_FLOWING_H__
#define __PROGRESS_BAR_FLOWING_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "__common.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __progress_bar_flowing_show0( __TARGET_TILE_PTR,                        \
                                    __PROGRESS,                                 \
                                    __IS_NEW_FRAME)                             \
            __progress_bar_flowing_show((__TARGET_TILE_PTR),                    \
                                        (__PROGRESS),                           \
                                        (bIsNewFrame),                          \
                                        __RGB(0x94, 0xd2, 0x52),                \
                                        GLCD_COLOR_WHITE,                       \
                                        __RGB(0xa5, 0xc6, 0xef))

#define __progress_bar_flowing_show1( __TARGET_TILE_PTR,                        \
                                    __PROGRESS,                                 \
                                    __IS_NEW_FRAME,                             \
                                    __BAR_COLOUR)                               \
            __progress_bar_flowing_show((__TARGET_TILE_PTR),                    \
                                        (__PROGRESS),                           \
                                        (bIsNewFrame),                          \
                                        (__BAR_COLOUR),                         \
                                        GLCD_COLOR_WHITE,                       \
                                        __RGB(0xa5, 0xc6, 0xef))

#define __progress_bar_flowing_show2( __TARGET_TILE_PTR,                        \
                                    __PROGRESS,                                 \
                                    __IS_NEW_FRAME,                             \
                                    __BAR_COLOUR,                               \
                                    __PITCH_COLOUR)                             \
            __progress_bar_flowing_show((__TARGET_TILE_PTR),                    \
                                        (__PROGRESS),                           \
                                        (bIsNewFrame),                          \
                                        (__BAR_COLOUR),                         \
                                        (__PITCH_COLOUR),                       \
                                        __RGB(0xa5, 0xc6, 0xef))

#define __progress_bar_flowing_show3( __TARGET_TILE_PTR,                        \
                                    __PROGRESS,                                 \
                                    __IS_NEW_FRAME,                             \
                                    __BAR_COLOUR,                               \
                                    __PITCH_COLOUR,                             \
                                    __BOARDER_COLOUR)                           \
            __progress_bar_flowing_show((__TARGET_TILE_PTR),                    \
                                        (__PROGRESS),                           \
                                        (bIsNewFrame),                          \
                                        (__BAR_COLOUR),                         \
                                        (__PITCH_COLOUR),                       \
                                        (__BOARDER_COLOUR))

#define progress_bar_flowing_show(  __TARGET_TILE_PTR,                          \
                                    __PROGRESS,                                 \
                                    __IS_NEW_FRAME,                             \
                                    ...)                                        \
            ARM_CONNECT2(   __progress_bar_flowing_show,                        \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(                    \
                                        (__TARGET_TILE_PTR),                    \
                                        (__PROGRESS),                           \
                                        (bIsNewFrame),##__VA_ARGS__)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
void progress_bar_flowing_init(void);


extern
void __progress_bar_flowing_show(   const arm_2d_tile_t *ptTarget, 
                                    int_fast16_t iProgress,
                                    bool bIsNewFrame, 
                                    COLOUR_INT tBarColour,
                                    COLOUR_INT tPitchColour,
                                    COLOUR_INT tBoarderColour);

#ifdef   __cplusplus
}
#endif

#endif
