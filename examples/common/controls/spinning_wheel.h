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

#ifndef __SPINNING_WHEEL_H__
#define __SPINNING_WHEEL_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "__common.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
void spinning_wheel_init(void);

extern
void spinning_wheel_show(const arm_2d_tile_t *ptTarget, bool bIsNewFrame);

extern
void spinning_wheel2_show(  const arm_2d_tile_t *ptTarget, 
                            COLOUR_INT Colour, 
                            bool bIsNewFrame);

#ifdef   __cplusplus
}
#endif

#endif
