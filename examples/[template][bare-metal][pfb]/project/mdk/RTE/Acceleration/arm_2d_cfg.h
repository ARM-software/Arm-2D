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

#ifndef __ARM_2D_USER_CFG_H__
#define __ARM_2D_USER_CFG_H__

/*============================ INCLUDES ======================================*/

#include "RTE_Components.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/


//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>Arm-2D General Configurations
// =======================
//
// <q>Enable Asynchronous Programmers' model support
// <i> Note that if you don't have any hardware accelerator, disable this feature can reduce code size and gain a small performance uplift.
// <i> This feature is enabled by default.
#ifndef __ARM_2D_HAS_ASYNC__
#   define __ARM_2D_HAS_ASYNC__                                     0
#endif

// <q>Enable anti-alias support for all tranform operations.
// <i> Note that enabling this feature suffers a non-negligible performance drop.
// <i> This feature is disabled by default.
#ifndef __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
#   define __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__                      0
#endif

// <q>Enable support for accessing individual colour channels
// <i> Note that enabling this feature will add the support for a special colour type: ARM_2D_CHANNEL_8in32
// <i> This feature is disabled by default to save code size
#ifndef __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
#   define __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__             1
#endif
// </h>

// <h>Patches for improving performance
// =======================
// 
// <c1> Do NOT treat alpha value 255 as completely opaque in mask related operations
// <i> When define this macro, alpha value 0xFF will not be treated as opaque in mask related operations and you can barely see the background. Defining this macro improves performance.
//#define __ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__  
// </c>

// <c1> Ignore calibrartion for small angles in transform operations
// <i> This option is used to speed up M-cores without DSP support. It skips saturation in the QADD/QDADD/QDSUB involved in the rotation. The chances of overflow remain low as elements involved are using non-accumulating Q15.16 format and integer parts are in the range of the screen size providing enough margin.
//#define __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__ 
// </c>

// <c1> Ignore satuation protection in fixed-point operations
// <i> This option is used to remove calibration in angle computations to gain a better performance, small error might be noticible for angles like 90, 180, 270 etc.
//#define __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT__ 
// </c>



// <<< end of configuration section >>>


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#ifdef   __cplusplus
}
#endif

#endif
