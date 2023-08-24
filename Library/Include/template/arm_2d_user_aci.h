/*
 * Copyright (C) 2010-2022 Arm Limited or its affiliates. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        arm_2d_user_aci.h
 * Description:  Public header file template for adding ACI acceleration
 *
 * $Date:        24 Aug 2023
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */

#if !defined(__ARM_2D_USER_ACI_H__) && __ARM_2D_HAS_ACI__
#define __ARM_2D_USER_ACI_H__

/*============================ INCLUDES ======================================*/
/*
 * NOTE: You can modify this header file for your ACI acceleration.
 */



/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/




/*============================ DISABLE DEFAULT HELIUM IMPLEMENTATION =========*/

#if defined(__ARM_2D_IMPLEMENT_HELIUM__) && __ARM_2D_IMPLEMENT_HELIUM__
/* 
 * NOTE: inside this block, you can rename the target low level functions, i.e. 
 *       functions with prefix like __arm_2d_impl_xxxxx to a different name,
 *       by doing so, your ACI accelerated low level implementation can override
 *       the defaultC version. 
 * 
 *       For example:
 *
 *       #define __arm_2d_impl_cccn888_to_rgb565    \
 *                    __arm_2d_impl_cccn888_to_rgb565_origin
 * 
 *       by doing so, your own ACI version of __arm_2d_impl_cccn888_to_rgb565 
 *       can override the default WEAK version in arm-2d library.
 */



#endif

#endif  /* end of __ARM_2D_USER_ACI_H__ */

