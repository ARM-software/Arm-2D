/*
 * Copyright (C) 2024 Arm Limited or its affiliates. All rights reserved.
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

#ifndef __ARM_2D_<USER OPCODE TEMPLATE>_H__
#define __ARM_2D_<USER OPCODE TEMPLATE>_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d_types.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#endif


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


typedef struct arm_2d_<user opcode template>_api_params_t {
    /* add your parameters here */
    uint_fast8_t chChannel;
} arm_2d_<user opcode template>_api_params_t;


typedef struct arm_2d_<user opcode template>_descriptor_t {
    implement(arm_2d_op_cp_t);      /* inherit from base class arm_2d_op_cp_t*/

    arm_2d_<user opcode template>_api_params_t tParams;

}arm_2d_<user opcode template>_descriptor_t;


/*============================ GLOBAL VARIABLES ==============================*/

extern
const __arm_2d_op_info_t ARM_2D_OP_<USER OPCODE TEMPLATE>;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

extern
ARM_NONNULL(2,3,5)
arm_fsm_rt_t arm_2dp_cccn888_<user opcode template>(  
                    arm_2d_<user opcode template>_descriptor_t *ptOP,
                    const arm_2d_tile_t *ptSource,
                    const arm_2d_tile_t *ptTarget,
                    const arm_2d_region_t *ptRegion,
                    const arm_2d_<user opcode template>_api_params_t *ptParams);


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif


#endif /* __ARM_2D_<USER OPCODE TEMPLATE>_H__ */
