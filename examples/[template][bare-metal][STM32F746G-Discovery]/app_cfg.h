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

#ifndef __TOP_APP_CFG_H__
#define __TOP_APP_CFG_H__

/*============================ INCLUDES ======================================*/

/*============================ MACROS ========================================*/

#define __GLCD_CFG_SCEEN_WIDTH__            480
#define __GLCD_CFG_SCEEN_HEIGHT__           272

#define __DISP0_CFG_SCEEN_WIDTH__           __GLCD_CFG_SCEEN_WIDTH__
#define __DISP0_CFG_SCEEN_HEIGHT__          __GLCD_CFG_SCEEN_HEIGHT__

#define __DISP0_CFG_PFB_BLOCK_WIDTH__       __DISP0_CFG_SCEEN_WIDTH__
#define __DISP0_CFG_PFB_BLOCK_HEIGHT__      (__DISP0_CFG_SCEEN_HEIGHT__/10)


#define ARM_2D_SCENE_METER_USE_JPG          1
#define ARM_2D_SCENE_HISTOGRAM_USE_JPG      1

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif
