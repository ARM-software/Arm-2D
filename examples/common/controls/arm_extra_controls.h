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



#ifndef __CONTROL_H__
#define __CONTROL_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__common.h"
#include "./busy_wheel.h"
#include "./shape_round_corner_box.h"
#include "./spinning_wheel.h"
#include "./progress_bar_drill.h"
#include "./progress_bar_flowing.h"
#include "./progress_bar_simple.h"
#include "./number_list.h"
#include "./progress_wheel.h"
#include "./list_view.h"

#if defined(__RTE_ACCELERATION_ARM_2D_EXTRA_LCD_PRINTF__)
#   include "./arm_extra_lcd_printf.h"
#endif

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
void arm_extra_controls_init(void);

extern
void arm_2d_draw_box(const arm_2d_tile_t *ptTarget,
                     const arm_2d_region_t *ptRegion,
                     int16_t iBorderWidth, 
                     COLOUR_INT tColour,
                     uint8_t chOpacity);

#ifdef   __cplusplus
}
#endif

#endif
