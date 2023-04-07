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

#ifndef __CONTROL_TEMPLATE_H__
#define __CONTROL_TEMPLATE_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__common.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/*============================ MACROS ========================================*/

/* OOC header, please DO NOT modify  */
#ifdef __CONTROL_TEMPLATE_IMPLEMENT__
#   undef   __CONTROL_TEMPLATE_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__CONTROL_TEMPLATE_INHERIT__)
#   undef   __CONTROL_TEMPLATE_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*!
 * \brief a user class for user defined control
 */
typedef struct user_control_template_t user_control_template_t;

struct user_control_template_t {

ARM_PRIVATE(
    /* place your private member here, following two are examples */
    int64_t lTimestamp[1];
    uint8_t chOpacity;
)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void control_template_init( user_control_template_t *ptThis);

extern
ARM_NONNULL(1)
void control_template_depose( user_control_template_t *ptThis);

extern
ARM_NONNULL(1)
void control_template_show( user_control_template_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion, 
                            bool bIsNewFrame);


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
