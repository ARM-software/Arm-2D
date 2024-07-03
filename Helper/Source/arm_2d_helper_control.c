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
 * Title:        #include "arm_2d_helper_control.c"
 * Description:  the helper service source code for control management
 *
 * $Date:        3. July 2024
 * $Revision:    V.0.5.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/
#define __ARM_2D_HELPER_CONTROL_IMPLEMENT__

#include "arm_2d_helper.h"
#include <math.h>
#include <time.h>
#include <assert.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Winitializer-overrides"
#endif

/*============================ MACROS ========================================*/

#undef this
#define this (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t arm_2d_helper_control_enum_init(
                            arm_2d_control_enumerator_t *ptThis,
                            const arm_2d_control_enumeration_policy_t *ptPolicy,
                            const arm_2d_control_node_t *ptRoot)
{



    ARM_CONTROL_ENUMERATE(  ptRoot, ptItem, PREORDER_TRAVERSAL) {
        ARM_2D_UNUSED(ptItem);
    }


    assert(NULL != ptThis);
    assert(NULL != ptPolicy);

    if (NULL == ptRoot) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    memset(ptThis, 0, sizeof(*ptThis));
    this.ptPolicy = ptPolicy;
    
    return ARM_2D_INVOKE(   this.ptPolicy->fnInit, 
                            ARM_2D_PARAM(
                                ptThis, ptRoot
                            ));
}

ARM_NONNULL(1)
arm_2d_control_node_t *arm_2d_helper_control_enum_get_next_node(
                                            arm_2d_control_enumerator_t *ptThis)
{
    assert(NULL != ptThis);
    assert(NULL != this.ptPolicy);

    return  ARM_2D_INVOKE(  this.ptPolicy->fnGetNextNode, 
                            ARM_2D_PARAM(
                                ptThis
                            ));
}

ARM_NONNULL(1)
arm_2d_err_t arm_2d_helper_control_enum_depose(
                                            arm_2d_control_enumerator_t *ptThis)
{
    assert(NULL != ptThis);
    assert(NULL != this.ptPolicy);

    return  ARM_2D_INVOKE(  this.ptPolicy->fnDepose, 
                            ARM_2D_PARAM(
                                ptThis
                            ));

}


ARM_NONNULL(1)
arm_2d_control_node_t *arm_2d_helper_control_find_node_with_location(
                                                arm_2d_control_node_t *ptRoot, 
                                                arm_2d_location_t tLocation)
{
    arm_2d_control_node_t *ptNode = NULL;
    arm_2d_control_node_t *ptContainer = NULL;

    ptNode = ptRoot;

    if (NULL == ptNode) {
        return NULL;
    }

    do {
        if (!arm_2d_is_point_inside_region(&ptNode->tRegion, &tLocation)) {
            /* out of region */
            if (NULL == ptNode->ptNext) {
                /* no more peers */
                return ptContainer;
            }

            /* try next peer */
            ptNode = ptNode->ptNext;
            continue;
        } else if (NULL == ptNode->ptChildList) {
            /* it is the one */
            break;
        } else {
            /* it is a container */
            ptContainer = ptNode;
            ptNode = ptNode->ptChildList;

            /* search the child nodes */
            continue;
        }

    } while(true);

    return ptNode;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
