/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
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

#ifndef __ARM_2D_DEMOS_H__
#define __ARM_2D_DEMOS_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d_scene_basics.h"
#include "arm_2d_scene_audiomark.h"
#include "arm_2d_scene_console.h"
#include "arm_2d_scene_console_window.h"
#include "arm_2d_scene_atom.h"
#include "arm_2d_scene_gas_gauge.h"
#include "arm_2d_scene_listview.h"
#include "arm_2d_scene_panel.h"
#include "arm_2d_scene_progress_status.h"
#include "arm_2d_scene_menu.h"
#include "arm_2d_scene_virtual_resource.h"
#include "arm_2d_scene_ruler.h"
#include "arm_2d_scene_hollow_out_list.h"
#include "arm_2d_scene_transform.h"
#include "arm_2d_scene_watch_face_01.h"
#include "arm_2d_scene_compass.h"
#include "arm_2d_scene_user_defined_opcode.h"
#include "arm_2d_scene_filters.h"
#include "arm_2d_scene_text_reader.h"
#include "arm_2d_scene_tjpgd.h"
#include "arm_2d_scene_rickrolling.h"
#include "arm_2d_scene_matrix.h"
#include "arm_2d_scene_iir_blur.h"
#include "arm_2d_scene_balls.h"
#include "arm_2d_scene_music_player.h"
#include "arm_2d_scene_font.h"
#include "arm_2d_scene_pave.h"

/* monochrome demos */
#if defined(RTE_Acceleration_Arm_2D_Demos_Monochrome)
#   include "arm_2d_scene_mono_loading.h"
#   include "arm_2d_scene_mono_histogram.h"
#   include "arm_2d_scene_mono_clock.h"
#   include "arm_2d_scene_mono_list.h"
#   include "arm_2d_scene_mono_tracking_list.h"
#   include "arm_2d_scene_mono_icon_menu.h"
#else
#   include "./monochrome/arm_2d_scene_mono_loading.h"
#   include "./monochrome/arm_2d_scene_mono_histogram.h"
#   include "./monochrome/arm_2d_scene_mono_clock.h"
#   include "./monochrome/arm_2d_scene_mono_list.h"
#   include "./monochrome/arm_2d_scene_mono_tracking_list.h"
#   include "./monochrome/arm_2d_scene_mono_icon_menu.h"
#endif

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef   __cplusplus
}
#endif

#endif
