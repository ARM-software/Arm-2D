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
 * $Date:        03. Aug 2024
 * $Revision:    V.0.7.4
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

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static arm_2d_err_t __arm_2d_enum_policy_preorder_init(
                                            arm_2d_control_enumerator_t *ptThis, 
                                            const arm_2d_control_node_t *ptRoot);
static arm_2d_err_t __arm_2d_enum_policy_preorder_depose (
                                            arm_2d_control_enumerator_t *ptThis);
static arm_2d_control_node_t *__arm_2d_enum_policy_preorder_get_next_node(
                                            arm_2d_control_enumerator_t *ptThis);

static arm_2d_err_t __arm_2d_enum_policy_postorder_init(
                                            arm_2d_control_enumerator_t *ptThis, 
                                            const arm_2d_control_node_t *ptRoot);
static arm_2d_err_t __arm_2d_enum_policy_postorder_depose (
                                            arm_2d_control_enumerator_t *ptThis);
static arm_2d_control_node_t *__arm_2d_enum_policy_postorder_get_next_node(
                                            arm_2d_control_enumerator_t *ptThis);

static arm_2d_err_t __arm_2d_enum_policy_bottom_up_init(
                                            arm_2d_control_enumerator_t *ptThis, 
                                            const arm_2d_control_node_t *ptRoot);
static arm_2d_err_t __arm_2d_enum_policy_bottom_up_depose (
                                            arm_2d_control_enumerator_t *ptThis);
static arm_2d_control_node_t *__arm_2d_enum_policy_bottom_up_get_next_node(
                                            arm_2d_control_enumerator_t *ptThis);

/*============================ GLOBAL VARIABLES ==============================*/

const  arm_2d_control_enumeration_policy_t
ARM_2D_CONTROL_ENUMERATION_POLICY_PREORDER_TRAVERSAL = {
    .fnInit         = &__arm_2d_enum_policy_preorder_init,
    .fnDepose       = &__arm_2d_enum_policy_preorder_depose,
    .fnGetNextNode  = &__arm_2d_enum_policy_preorder_get_next_node,
};

const  arm_2d_control_enumeration_policy_t
ARM_2D_CONTROL_ENUMERATION_POLICY_POSTORDER_TRAVERSAL = {
    .fnInit         = &__arm_2d_enum_policy_postorder_init,
    .fnDepose       = &__arm_2d_enum_policy_postorder_depose,
    .fnGetNextNode  = &__arm_2d_enum_policy_postorder_get_next_node,
};

const  arm_2d_control_enumeration_policy_t
ARM_2D_CONTROL_ENUMERATION_POLICY_BOTTOM_UP_TRAVERSAL = {
    .fnInit         = &__arm_2d_enum_policy_bottom_up_init,
    .fnDepose       = &__arm_2d_enum_policy_bottom_up_depose,
    .fnGetNextNode  = &__arm_2d_enum_policy_bottom_up_get_next_node,
};
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1)
arm_2d_control_node_t *arm_2d_helper_control_find_node_with_location(
                                                arm_2d_control_node_t *ptRoot, 
                                                arm_2d_location_t tLocation)
{
    arm_2d_control_node_t *ptNode = NULL;
    arm_2d_control_node_t *ptCandidate = NULL;
    arm_2d_control_node_t *ptTheLastContainer = NULL;

    /* tVisibleArea is an absolute region in the virtual screen */
    arm_2d_region_t tVisibleArea = {0};

    ptNode = ptRoot;

    if (NULL == ptNode) {
        return NULL;
    }

    /* this must be the root node */
    assert(NULL == ptRoot->ptParent);

    /* get the root node region which is a relative region of the host tile
     * IMPORTANT: The region of the control root MUST be an absolute region
     */
    tVisibleArea = ptRoot->tRegion;
    
    if (tVisibleArea.tSize.iWidth <= 0 || tVisibleArea.tSize.iHeight <= 0) {
        assert(false); /* The root node region should have a valid size */
        return NULL;
    }

    do {
        arm_2d_region_t tControlRegion = ptNode->tRegion;

        /* the node address is an relative address inside the container */
        tControlRegion.tLocation.iX += tVisibleArea.tLocation.iX;
        tControlRegion.tLocation.iY += tVisibleArea.tLocation.iY;

        bool bInsideControlRegion = false;

        /* we have to make sure the control's region is inside the visible area */
        if (arm_2d_region_intersect(&tControlRegion, &tVisibleArea, &tControlRegion)) {

            /* if it is inside the visible area, we check the touch point */
            bInsideControlRegion = arm_2d_is_point_inside_region(&tControlRegion, &tLocation);
        }

        if (bInsideControlRegion) {
            /* update candidate */
            ptCandidate = ptNode;
        }

        if (NULL != ptNode->ptNext) {
            /* There are more peers in the list, let's check them first */
            ptNode = ptNode->ptNext;
            continue;
        }

        /* When we reach here, we have already visited all peers in the list */
        if (NULL == ptCandidate) {
            /* the touch point hits nothing, let's end the search and
             * return NULL 
             */
            break;
        }

        if (ptCandidate == ptTheLastContainer) {
            /* the only candidate is the container, i.e. the touch point hits 
             * no controls in the container, let's return the container.
             */
            break;
        }

        if (NULL == ptCandidate->ptChildList) {
            /* the touch point hits a leaf, let's return the candidate */
            break;
        }

        /* When we reach here, the most recent candidate is a container, we 
         * have to jump into it 
         */
        ptCandidate = ptNode;                   /* update candidate */
        ptTheLastContainer = ptNode;            /* update the last container */

        /* since we jump into a container, we have to update the visible 
         * area to fit the container 
         */
        tVisibleArea.tLocation.iX += ptNode->tRegion.tLocation.iX;
        tVisibleArea.tLocation.iY += ptNode->tRegion.tLocation.iY;

        /* use the clipped size */
        tVisibleArea.tSize = tControlRegion.tSize;

        /* search the child nodes */
        ptNode = ptNode->ptChildList;

    } while(true);

    return ptCandidate;
}


ARM_NONNULL(1,2)
arm_2d_err_t arm_2d_helper_control_enum_init(
                            arm_2d_control_enumerator_t *ptThis,
                            const arm_2d_control_enumeration_policy_t *ptPolicy,
                            const arm_2d_control_node_t *ptRoot)
{

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

/*----------------------------------------------------------------------------*
 * Preorder traversal                                                         *
 *----------------------------------------------------------------------------*/

static arm_2d_err_t __arm_2d_enum_policy_preorder_init(
                                            arm_2d_control_enumerator_t *ptThis, 
                                            const arm_2d_control_node_t *ptRoot)
{
    assert(NULL != ptThis);
    assert(NULL != ptRoot);

    this.ptCurrent = NULL;
    this.ptRoot = (arm_2d_control_node_t *)ptRoot;
    this.Preorder.bFirstNode = true;
    this.ptCurrent = this.ptRoot;

    return ARM_2D_ERR_NONE;
}


static arm_2d_err_t __arm_2d_enum_policy_preorder_depose (
                                            arm_2d_control_enumerator_t *ptThis)
{
    assert(NULL != ptThis);

    ARM_2D_UNUSED(ptThis);

    return ARM_2D_ERR_NONE;
}

static arm_2d_control_node_t *__arm_2d_enum_policy_preorder_get_next_node(
                                            arm_2d_control_enumerator_t *ptThis)
{
    assert(NULL != ptThis);

    if (this.Preorder.bFirstNode) {
        this.Preorder.bFirstNode = false;
        return this.ptCurrent;
    }

    arm_2d_control_node_t *ptNode = this.ptCurrent;

    do {
        if (NULL == ptNode) {
            break;
        }

        if (NULL != ptNode->ptChildList) {
            this.ptCurrent = ptNode->ptChildList;
            break;
        }

        do {
            if (ptNode->ptNext == NULL) {
                if (    (NULL == ptNode->ptParent)
                   ||   (ptNode == this.ptRoot)) {
                    /* return back to the root node: finish visiting */ 
                    this.ptCurrent = NULL;
                    break;
                }
                /* go back to parent */
                ptNode = ptNode->ptParent;
                continue;
            } else {
                this.ptCurrent = ptNode->ptNext;
                break;
            }
        } while(true);

    } while(0);

    return this.ptCurrent;

}


/*----------------------------------------------------------------------------*
 * Postorder Traversal                                                        *
 *----------------------------------------------------------------------------*/

static arm_2d_err_t __arm_2d_enum_policy_postorder_init(
                                            arm_2d_control_enumerator_t *ptThis, 
                                            const arm_2d_control_node_t *ptRoot)
{
    assert(NULL != ptThis);
    assert(NULL != ptRoot);

    this.ptCurrent = NULL;
    this.ptRoot = (arm_2d_control_node_t *)ptRoot;
    this.Postorder.chPTState = 0;
    this.ptCurrent = this.ptRoot;

    return ARM_2D_ERR_NONE;
}


static arm_2d_err_t __arm_2d_enum_policy_postorder_depose (
                                            arm_2d_control_enumerator_t *ptThis)
{
    assert(NULL != ptThis);
    ARM_2D_UNUSED(ptThis);

    return ARM_2D_ERR_NONE;
}

static arm_2d_control_node_t *__arm_2d_enum_policy_postorder_get_next_node(
                                            arm_2d_control_enumerator_t *ptThis)
{
    assert(NULL != ptThis);

    arm_2d_control_node_t *ptNode = this.ptCurrent;

ARM_PT_BEGIN(this.Postorder.chPTState)

    while(NULL != ptNode) {
        if (NULL != ptNode->ptChildList) {
            ptNode = ptNode->ptChildList;
            continue;
        }

        this.ptCurrent = ptNode;
        ARM_PT_YIELD(ptNode);               /* create an entry and visit a leaf node */

        if (NULL != ptNode->ptNext) {
            ptNode = ptNode->ptNext;        /* move to the next peer node */
            continue;
        }

        /* reach the end of a child-list */
        if (    (NULL != ptNode->ptParent) 
           &&   (ptNode != this.ptRoot)) {
            ptNode = ptNode->ptParent;

            /* it is time to visit the parent node */
            this.ptCurrent = ptNode;
            ARM_PT_GOTO_PREV_ENTRY(ptNode);
        }

        /* return to the root node */
        this.ptCurrent = NULL;
        ptNode = NULL;
        break;
    }

ARM_PT_END()

    return this.ptCurrent;

}

/*----------------------------------------------------------------------------*
 * Bottom Up traversal                                                        *
 *----------------------------------------------------------------------------*/

static arm_2d_err_t __arm_2d_enum_policy_bottom_up_init(
                                            arm_2d_control_enumerator_t *ptThis, 
                                            const arm_2d_control_node_t *ptRoot)
{
    assert(NULL != ptThis);
    assert(NULL != ptRoot);

    this.ptCurrent = NULL;
    this.ptRoot = (arm_2d_control_node_t *)ptRoot;
    this.BottomUp.bFirstNode = true;
    this.ptCurrent = this.ptRoot;

    return ARM_2D_ERR_NONE;
}


static arm_2d_err_t __arm_2d_enum_policy_bottom_up_depose (
                                            arm_2d_control_enumerator_t *ptThis)
{
    assert(NULL != ptThis);
    ARM_2D_UNUSED(ptThis);

    return ARM_2D_ERR_NONE;
}

static arm_2d_control_node_t *__arm_2d_enum_policy_bottom_up_get_next_node(
                                            arm_2d_control_enumerator_t *ptThis)
{
    assert(NULL != ptThis);

    arm_2d_control_node_t *ptNode = this.ptCurrent;

    if (NULL == ptNode) {
        return NULL;
    }

    /* handle the first node */
    if (this.Preorder.bFirstNode) {
        this.Preorder.bFirstNode = false;
        return ptNode;
    }

    if (NULL == ptNode->ptParent) {
        /* reach the top */
        this.ptCurrent = NULL;
    } else {
        /* return the parent */
        this.ptCurrent = ptNode->ptParent;
    }

    return this.ptCurrent;

}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
