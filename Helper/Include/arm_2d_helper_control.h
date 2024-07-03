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

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        #include "arm_2d_helper_control.h"
 * Description:  the helper service header file for control management
 *
 * $Date:        3. July 2024
 * $Revision:    V.0.5.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_CONTROL_H__
#define __ARM_2D_HELPER_CONTROL_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/* OOC header, please DO NOT modify  */
#ifdef __ARM_2D_HELPER_CONTROL_IMPLEMENT__
#   define __ARM_2D_IMPL__
#endif
#ifdef __ARM_2D_HELPER_CONTROL_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct arm_2d_control_node_t arm_2d_control_node_t;

struct arm_2d_control_node_t {

ARM_PROTECTED(
    arm_2d_control_node_t *ptNext;              /*!< points to the next peer */
    arm_2d_control_node_t *ptParent;            /*!< points to the parent */
    arm_2d_control_node_t *ptChildList;         /*!< NULL: leaf, !NULL: container */
)

    arm_2d_region_t tRegion;
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

ARM_NONNULL(1)
extern
arm_2d_control_node_t *arm_2d_helper_control_find_node_with_location(
                                                arm_2d_control_node_t *ptRoot, 
                                                arm_2d_location_t tLocation);


#undef __ARM_2D_HELPER_CONTROL_IMPLEMENT__
#undef __ARM_2D_HELPER_CONTROL_INHERIT__

#ifdef   __cplusplus
}
#endif

#endif
