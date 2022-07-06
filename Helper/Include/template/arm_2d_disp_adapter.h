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

#ifndef __ARM_2D_DISP_ADAPTER%Instance%_H__
#define __ARM_2D_DISP_ADAPTER%Instance%_H__

#include "arm_2d.h"

#ifdef __RTE_ACCELERATION_ARM_2D_HELPER_DISP_ADAPTER%Instance%__

#include "arm_2d_helper_scene.h"

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <o> Select the screen colour depth
//     <8=>     8 Bits
//     <16=>    16Bits
//     <32=>    32Bits
// <i> The colour depth of your screen
#ifndef __DISP%Instance%_CFG_COLOUR_DEPTH__
#   define __DISP%Instance%_CFG_COLOUR_DEPTH__              16
#endif

// <o>Width of the screen <8-32767>
// <i> The width of your screen
// <i> Default: 320
#ifndef __DISP%Instance%_CFG_SCEEN_WIDTH__
#   define __DISP%Instance%_CFG_SCEEN_WIDTH__               320
#endif

// <o>Height of the screen <8-32767>
// <i> The height of your screen
// <i> Default: 240
#ifndef __DISP%Instance%_CFG_SCEEN_HEIGHT__
#   define __DISP%Instance%_CFG_SCEEN_HEIGHT__              240
#endif

// <o>Width of the PFB block
// <i> The width of your PFB block size used in disp%Instance%
#ifndef __DISP%Instance%_CFG_PFB_BLOCK_WIDTH__
#   define __DISP%Instance%_CFG_PFB_BLOCK_WIDTH__           320
#endif

// <o>Height of the PFB block
// <i> The height of your PFB block size used in disp%Instance%
#ifndef __DISP%Instance%_CFG_PFB_BLOCK_HEIGHT__
#   define __DISP%Instance%_CFG_PFB_BLOCK_HEIGHT__          240
#endif

// <o>Number of iterations <0-2000>
// <i> run number of iterations before calculate the FPS.
#ifndef __DISP%Instance%_CFG_ITERATION_CNT
#   define __DISP%Instance%_CFG_ITERATION_CNT               30
#endif

// <<< end of configuration section >>>


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
ARM_NOINIT
extern
arm_2d_scene_player_t DISP%Instance%_ADAPTER;

/*============================ PROTOTYPES ====================================*/

extern
void disp_adapter%Instance%_init(void);

extern
arm_fsm_rt_t disp_adapter%Instance%_task(void);

#endif

#endif
