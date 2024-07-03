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
 * $Revision:    V.0.6.0
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

#define ARM_CONTROL_ENUMERATE(__ROOT, __ITEM_NAME, __POLICY )                   \
    for (                                                                       \
        arm_2d_control_enumerator_t ARM_2D_SAFE_NAME(tEnum),                    \
        *ARM_2D_SAFE_NAME(ptPointer) = NULL;                                    \
        (({                                                                     \
            arm_2d_helper_control_enum_init(                                    \
                        &ARM_2D_SAFE_NAME(tEnum),                               \
                        &(ARM_2D_CONTROL_ENUMERATION_POLICY_##__POLICY),        \
                        ptRoot);                                                \
            ARM_2D_SAFE_NAME(ptPointer)++;                                      \
        }) == NULL);                                                            \
        ({                                                                      \
            arm_2d_helper_control_enum_depose(&ARM_2D_SAFE_NAME(tEnum));        \
        })) for (                                                               \
            arm_2d_control_node_t *(__ITEM_NAME) = NULL;                        \
            (NULL != ((__ITEM_NAME) = arm_2d_helper_control_enum_get_next_node( \
                                                    &ARM_2D_SAFE_NAME(tEnum)))  \
            );                                                                  \
        )

#define arm_control_enumerate           ARM_CONTROL_ENUMERATE
#define arm_ctrl_enum                   ARM_CONTROL_ENUMERATE
#define ARM_CTRL_ENUM                   ARM_CONTROL_ENUMERATE

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

typedef struct arm_2d_control_enumerator_t arm_2d_control_enumerator_t;

typedef struct arm_2d_control_enumeration_policy_t {
    arm_2d_err_t            (*fnInit)           (arm_2d_control_enumerator_t *ptThis, 
                                                 const arm_2d_control_node_t *ptRoot);
    arm_2d_err_t            (*fnDepose)         (arm_2d_control_enumerator_t *ptThis);
    arm_2d_control_node_t * (*fnGetNextNode)    (arm_2d_control_enumerator_t *ptThis);
} arm_2d_control_enumeration_policy_t;


struct arm_2d_control_enumerator_t {
    const arm_2d_control_enumeration_policy_t *ptPolicy;
    arm_2d_control_node_t *ptRoot;
    arm_2d_control_node_t *ptCurrent;
    union {
        uint32_t                : 32;
        struct {
            bool    bFirstNode;
        } Preorder;
        struct {
            uint8_t chPTState;
        } DFS;
    };

};

/*============================ GLOBAL VARIABLES ==============================*/

extern 
const  arm_2d_control_enumeration_policy_t
ARM_2D_CONTROL_ENUMERATION_POLICY_PREORDER_TRAVERSAL;


extern 
const  arm_2d_control_enumeration_policy_t
ARM_2D_CONTROL_ENUMERATION_POLICY_DEPTH_FIRST_TRAVERSAL;

/*============================ PROTOTYPES ====================================*/

ARM_NONNULL(1,2)
extern
arm_2d_err_t arm_2d_helper_control_enum_init(
                            arm_2d_control_enumerator_t *ptThis,
                            const arm_2d_control_enumeration_policy_t *ptPolicy,
                            const arm_2d_control_node_t *ptRoot);

ARM_NONNULL(1)
extern
arm_2d_control_node_t *arm_2d_helper_control_enum_get_next_node(
                                            arm_2d_control_enumerator_t *ptThis);


ARM_NONNULL(1)
extern
arm_2d_err_t arm_2d_helper_control_enum_depose(
                                            arm_2d_control_enumerator_t *ptThis);



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
