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

#ifndef __ARM_2D_BENCHMARK_H__
#define __ARM_2D_BENCHMARK_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "__common.h"
/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern arm_2d_scene_player_t BENCHMARK_DISP_ADAPTER;

/*============================ PROTOTYPES ====================================*/

extern
void arm_2d_run_benchmark(void);


/*!
 * \brief It is an user implemented function that request an LCD flushing in 
 *        asynchronous manner. 
 * \note User MUST implement this function when 
 *       __ARM_2D_CFG_BENCHMARK_ENABLE_ASYNC_FLUSHING__ is set to '1'
 *
 * \param[in] pTarget an user specified object address
 * \param[in] bIsNewFrame whether this flushing request is the first iteration 
 *            of a new frame.
 * \param[in] iX the x coordinate of a flushing window in the target screen
 * \param[in] iY the y coordinate of a flushing window in the target screen
 * \param[in] iWidth the width of a flushing window
 * \param[in] iHeight the height of a flushing window
 * \param[in] pBuffer the frame buffer address
 */
extern void benchmark_request_async_flushing(   void *pTarget,
                                                bool bIsNewFrame,
                                                int16_t iX, 
                                                int16_t iY,
                                                int16_t iWidth,
                                                int16_t iHeight,
                                                const COLOUR_INT *pBuffer);


/*!
 * \brief the handler for the asynchronous flushing complete event.
 * \note When __ARM_2D_CFG_BENCHMARK_ENABLE_ASYNC_FLUSHING__ is set to '1', user 
 *       MUST call this function to notify the PFB helper that the previous
 *       asynchronous flushing is complete. 
 * \note When people using DMA+ISR to offload CPU, this fucntion is called in 
 *       the DMA transfer complete ISR.
 */
extern
void benchmark_insert_async_flushing_complete_event_handler(void);

#endif
