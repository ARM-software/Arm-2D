/*
 * Copyright (C) 2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        #include "arm_2d_helper_list.h"
 * Description:  Public header file for list core related services
 *
 * $Date:        20. Nov 2022
 * $Revision:    V.1.1.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

#define __ARM_2D_HELPER_LIST_VIEW_IMPLEMENT__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "arm_2d_helper.h"
#include "arm_2d_helper_list.h"

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
#   pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wpedantic"
#   pragma clang diagnostic ignored "-Wcovered-switch-default"
#   pragma clang diagnostic ignored "-Wswitch-enum"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wunused-value"
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe174
#endif

/*============================ MACROS ========================================*/
#undef this
#define this        (*ptThis)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static
arm_2d_err_t __arm_2d_list_core_move_selection( __arm_2d_list_core_t *ptThis, 
                                                int16_t iSteps,
                                                int32_t nFinishInMs);
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t __arm_2d_list_core_init(   __arm_2d_list_core_t *ptThis,
                                        __arm_2d_list_core_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(NULL != ptCFG->fnCalculator);
    assert(NULL != ptCFG->fnIterator);

    memset(ptThis, 0, sizeof(__arm_2d_list_core_t));
    this.tCFG = *ptCFG;

    if (this.tCFG.hwSwitchingPeriodInMs) {
        this.Runtime.lPeriod 
            = arm_2d_helper_convert_ms_to_ticks(this.tCFG.hwSwitchingPeriodInMs);
    } else {
        this.Runtime.lPeriod = arm_2d_helper_convert_ms_to_ticks(500);          /*!< use 500 ms as default */
    }
    
    if (this.tCFG.hwItemSizeInByte < sizeof(arm_2d_list_item_t)) {
        this.tCFG.hwItemSizeInByte = sizeof(arm_2d_list_item_t);
    }
    
    this.CalMidAligned.bListHeightChanged = true;
    this.Runtime.bIsRegCalInit = false;

    return ARM_2D_ERR_NONE;
}


ARM_NONNULL(1,2)
arm_fsm_rt_t __arm_2d_list_core_show(   __arm_2d_list_core_t *ptThis,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        bool bIsNewFrame)
{
    assert(NULL != ptThis);
    assert(NULL != ptTarget);
    arm_fsm_rt_t tResult;
    
    ARM_2D_UNUSED(ptRegion);
    ARM_2D_UNUSED(bIsNewFrame);

ARM_PT_BEGIN(this.Runtime.chState)

    /* runtime initialize */
    do {
        arm_2d_region_t tListRegion = {0};
        if (NULL == ptRegion) {
            tListRegion.tSize = ptTarget->tRegion.tSize;
            ptRegion = &tListRegion;
        }
        
        /* validation for this.tCFG.tListSize */
        if (this.tCFG.tListSize.iWidth <= 0) {
            this.tCFG.tListSize.iWidth = ptRegion->tSize.iWidth;
        }
        if (this.tCFG.tListSize.iHeight <= 0) {
            this.tCFG.tListSize.iHeight = ptRegion->tSize.iHeight;
        }

        /* get target tile */
        if (NULL == arm_2d_tile_generate_child(ptTarget, 
                                               ptRegion,
                                               &this.Runtime.tileTarget, 
                                               false)) {
            /* nothing to draw: use the unified exist point */
            goto label_end_of_list_core_task;
        }

        arm_2d_align_centre(this.Runtime.tileTarget.tRegion, this.tCFG.tListSize) {
            /* get target tile */
            if (NULL == arm_2d_tile_generate_child(&this.Runtime.tileTarget, 
                                                   &__centre_region,
                                                   &this.Runtime.tileList, 
                                                   false)) {
                /* nothing to draw: use the unified exist point */
                goto label_end_of_list_core_task;
            }
        }
        
        if (bIsNewFrame) {
            if (__arm_2d_helper_time_liner_slider(this.Runtime.nStartOffset,    /* from */
                                                  this.Runtime.nTargetOffset,   /* to */
                                                  this.Runtime.lPeriod,         /* finish in specified period */
                                                  &this.Runtime.nOffset,        /* output offset */
                                                  &this.Runtime.lTimestamp)) {  /* timestamp */
                if (this.tCFG.nTotalLength) {
                    this.Runtime.nOffset = this.Runtime.nOffset % this.tCFG.nTotalLength;
                }
                this.Runtime.nTargetOffset = this.Runtime.nOffset;
                this.Runtime.nStartOffset = this.Runtime.nTargetOffset;
                
                if (this.Runtime.MoveReq.iSteps && this.Runtime.bIsRegCalInit) {
                    __arm_2d_list_core_move_selection(
                        ptThis, 
                        this.Runtime.MoveReq.iSteps,
                        this.Runtime.MoveReq.nFinishInMs);
                    /* reset request */
                    this.Runtime.MoveReq.iSteps = 0;
                    this.Runtime.MoveReq.nFinishInMs = 0;
                }
            }
        }
    } while(0);

    /* draw background */
    do {
    ARM_PT_ENTRY()

        tResult = ARM_2D_INVOKE(this.tCFG.fnOnDrawListBackground,
                                ARM_2D_PARAM(   
                                    ptThis, 
                                    &this.Runtime.tileList, 
                                    bIsNewFrame));

        if (arm_fsm_rt_on_going == tResult) {
    ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going)
        } else if (tResult < 0) {
            // error was reported
    ARM_PT_RETURN(tResult)
        } else if (arm_fsm_rt_cpl != tResult) {
    ARM_PT_REPORT_STATUS(tResult)
        } 

    ARM_PT_YIELD(arm_fsm_rt_on_going)
    } while(0);


    assert(NULL != this.tCFG.fnCalculator);
    
    /* loop until finishing drawing all visiable area */
    do {
        /* call calculator: it should handle the padding */
        if (NULL == ARM_2D_INVOKE(  this.tCFG.fnCalculator,
                        ARM_2D_PARAM(
                            ptThis, 
                            this.tCFG.fnIterator,
                            this.Runtime.nOffset))) {
            /* finish: use the unified exist point */
            //goto label_end_of_list_core_task;
            break;
        }
        arm_2d_list_item_t *ptItem = this.Runtime.tWorkingArea.ptItem;
        assert(NULL != ptItem);
        
        if (!ptItem->bIsReadOnly) {
            ptItem->ptListView = ptThis;
        }
        
        /* calculate alignment offset*/
        do {
            int16_t iOffset = 0;
            if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
                /* horizontal list */
                switch (    ptItem->u4Alignment 
                       &    (ARM_2D_ALIGN_TOP | ARM_2D_ALIGN_BOTTOM)) {
                    case ARM_2D_ALIGN_TOP:
                        /* do nothing */
                        break;
                    case ARM_2D_ALIGN_BOTTOM:
                        iOffset = this.Runtime.tileList.tRegion.tSize.iHeight
                                 - ptItem->tSize.iHeight;
                        break;
                    default:                    /* centre alignment */
                        iOffset = (    this.Runtime.tileList.tRegion.tSize.iHeight
                                    -   ptItem->tSize.iHeight) >> 1;
                        break;
                }
                this.Runtime.tWorkingArea.tRegion.tLocation.iY += iOffset;
            } else {
                /* vertical list */
                switch (    ptItem->u4Alignment 
                       &    (ARM_2D_ALIGN_LEFT | ARM_2D_ALIGN_RIGHT)) {
                    case ARM_2D_ALIGN_LEFT:
                        /* do nothing */
                        break;
                    case ARM_2D_ALIGN_RIGHT:
                        iOffset = this.Runtime.tileList.tRegion.tSize.iWidth
                                 - ptItem->tSize.iWidth;
                        break;
                    default:                    /* centre alignment */
                        iOffset = (    this.Runtime.tileList.tRegion.tSize.iWidth
                                    -   ptItem->tSize.iWidth) >> 1;
                        break;
                }
                this.Runtime.tWorkingArea.tRegion.tLocation.iX += iOffset;
            }
        } while(0);
        
        /* update this.Runtime.tWorkingArea.tRegion with margin */
        this.Runtime.tWorkingArea.tRegion.tLocation.iX += ptItem->Margin.chLeft;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY += ptItem->Margin.chTop;

        this.Runtime.tWorkingArea.tRegion.tSize.iWidth 
            -= ptItem->Margin.chLeft + ptItem->Margin.chRight;
        if (0 == this.Runtime.tWorkingArea.tRegion.tSize.iWidth) {
            continue;
        }
        
        this.Runtime.tWorkingArea.tRegion.tSize.iHeight 
            -= ptItem->Margin.chTop + ptItem->Margin.chBottom;
        if (0 == this.Runtime.tWorkingArea.tRegion.tSize.iHeight) {
            continue;
        }

        /* update selected field */
        this.Runtime.tWorkingArea.tParam.bIsSelected 
            = (this.Runtime.hwSelection == ptItem->hwID);

        if (NULL == arm_2d_tile_generate_child(&this.Runtime.tileList, 
                                               &this.Runtime.tWorkingArea.tRegion,
                                               &this.Runtime.tileItem,
                                               false)) {
            continue;
        }

        /* draw list core item background */
        do {
        ARM_PT_ENTRY()
            ptItem = this.Runtime.tWorkingArea.ptItem;
            
            tResult = ARM_2D_INVOKE(this.tCFG.fnOnDrawListItemBackground,
                                    ARM_2D_PARAM(   
                                        ptItem, 
                                        &this.Runtime.tileItem, 
                                        bIsNewFrame,
                                        &this.Runtime.tWorkingArea.tParam));

            if (arm_fsm_rt_on_going == tResult) {
        ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going)
            } else if (tResult < 0) {
                // error was reported
        ARM_PT_RETURN(tResult)
            } else if (arm_fsm_rt_cpl != tResult) {
        ARM_PT_REPORT_STATUS(tResult)
            } 

        ARM_PT_YIELD(arm_fsm_rt_on_going)
        } while(0);

        /* draw list item */
        do {
        ARM_PT_ENTRY()
            ptItem = this.Runtime.tWorkingArea.ptItem;

            /* draw item */
            tResult = ARM_2D_INVOKE(ptItem->fnOnDrawItem,
                                        ARM_2D_PARAM(
                                            ptItem,
                                            &this.Runtime.tileItem,
                                            bIsNewFrame,
                                            &this.Runtime.tWorkingArea.tParam
                                        ));

            if (arm_fsm_rt_on_going == tResult) {
        ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going)
            } else if (tResult < 0) {
                // error was reported
        ARM_PT_RETURN(tResult)
            } else if (arm_fsm_rt_cpl != tResult) {
        ARM_PT_REPORT_STATUS(tResult)
            } 
        ARM_PT_YIELD(arm_fsm_rt_on_going)
        } while(0);

    } while(true);

    /* draw list cover */
    do {
    ARM_PT_ENTRY()

        tResult = ARM_2D_INVOKE(this.tCFG.fnOnDrawListCover,
                                ARM_2D_PARAM(   
                                    ptThis, 
                                    &this.Runtime.tileList, 
                                    bIsNewFrame));

        if (arm_fsm_rt_on_going == tResult) {
    ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going)
        } else if (tResult < 0) {
            // error was reported
    ARM_PT_RETURN(tResult)
        } else if (arm_fsm_rt_cpl != tResult) {
    ARM_PT_REPORT_STATUS(tResult)
        } 
    } while(0);


label_end_of_list_core_task:
ARM_PT_END()
    
    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
void __arm_2d_list_core_move_offset(__arm_2d_list_core_t *ptThis, 
                                    int16_t iOffset)
{
    assert(NULL != ptThis);

    arm_irq_safe {
        this.Runtime.nOffset += iOffset;
        this.Runtime.nTargetOffset = this.Runtime.nOffset;
    }
}

ARM_NONNULL(1)
void __arm_2d_list_core_move_request(   __arm_2d_list_core_t *ptThis, 
                                                int16_t iSteps,
                                                int32_t nFinishInMs)
{
    assert(NULL != ptThis);
    arm_irq_safe {
        this.Runtime.MoveReq.iSteps = iSteps;
        this.Runtime.MoveReq.nFinishInMs = nFinishInMs;
    }
}

static
arm_2d_err_t __arm_2d_list_core_move_selection( __arm_2d_list_core_t *ptThis, 
                                                int16_t iSteps,
                                                int32_t nFinishInMs)
{
    assert(NULL != ptThis);
    int64_t lPeriod = this.Runtime.lPeriod;
    int32_t nOffsetChange = 0;
    uint16_t hwTargetID;
    
    /* update nPeriod */
    if (nFinishInMs > 0) {
        lPeriod = arm_2d_helper_convert_ms_to_ticks(nFinishInMs);
    } else if (nFinishInMs < 0) {
        if (this.tCFG.hwSwitchingPeriodInMs) {
            lPeriod 
                = arm_2d_helper_convert_ms_to_ticks(this.tCFG.hwSwitchingPeriodInMs);
        } else {
            lPeriod = arm_2d_helper_convert_ms_to_ticks(500);
        }
    }

    if (0 == iSteps) {
        /* nothing to do */
        this.Runtime.lPeriod = lPeriod;
        return ARM_2D_ERR_NONE;
    }

    /* calculate offset */
    do {
        __arm_2d_list_item_iterator *fnIterator = this.tCFG.fnIterator;
        assert(NULL != fnIterator);
        if (NULL == fnIterator) {
            return ARM_2D_ERR_NOT_AVAILABLE;
        }
       
        arm_2d_list_item_t *ptItem = NULL;
        
        ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_GET_CURRENT, 0));
        
        if (NULL == ptItem) {
            return ARM_2D_ERR_NOT_AVAILABLE;
        }
        
        uint16_t hwSaveID = ptItem->hwID;
        
        do {
            ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                            this.Runtime.hwSelection));

            if (NULL == ptItem) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            if (iSteps > 0) {
                
                /* handle the first item */
                do {
                    if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_VERTICAL) {
                        nOffsetChange -= ptItem->tSize.iHeight;
                    } else {
                        nOffsetChange -= ptItem->tSize.iWidth;
                    }
                    
                    nOffsetChange -= ptItem->Padding.chNext;
                    nOffsetChange -= ptItem->Padding.chPrevious;
                    
                    ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_GET_NEXT,
                            this.Runtime.hwSelection));
                    
                    if (NULL == ptItem) {
                        /* just in case the iterator doesn't support ring mode */
                        ptItem = ARM_2D_INVOKE(fnIterator, 
                                    ARM_2D_PARAM(
                                        ptThis, 
                                        __ARM_2D_LIST_GET_FIRST_ITEM,
                                        this.Runtime.hwSelection));
                        assert(NULL != ptItem);
                        
                        if (NULL == ptItem) {
                            /* this shouldn't happen */
                            return ARM_2D_ERR_NOT_AVAILABLE;
                        }
                    }
                } while(0);

                while(--iSteps) {
                    if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_VERTICAL) {
                        nOffsetChange -= ptItem->tSize.iHeight;
                    } else {
                        nOffsetChange -= ptItem->tSize.iWidth;
                    }
                    
                    nOffsetChange -= ptItem->Padding.chNext;
                    nOffsetChange -= ptItem->Padding.chPrevious;
                    
                    /* next */
                    ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_GET_NEXT,
                            this.Runtime.hwSelection));
                    
                    if (NULL == ptItem) {
                        /* just in case the iterator doesn't support ring mode */
                        ptItem = ARM_2D_INVOKE(fnIterator, 
                                    ARM_2D_PARAM(
                                        ptThis, 
                                        __ARM_2D_LIST_GET_FIRST_ITEM,
                                        this.Runtime.hwSelection));
                        assert(NULL != ptItem);
                        
                        if (NULL == ptItem) {
                            /* this shouldn't happen */
                            return ARM_2D_ERR_NOT_AVAILABLE;
                        }
                    }
                }

            } else {
            
                /* handle the first item */
                do {
                    if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_VERTICAL) {
                        nOffsetChange += ptItem->tSize.iHeight;
                    } else {
                        nOffsetChange += ptItem->tSize.iWidth;
                    }
                    
                    nOffsetChange += ptItem->Padding.chNext;
                    nOffsetChange += ptItem->Padding.chPrevious;
                    
                    ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_GET_PREVIOUS,
                            this.Runtime.hwSelection));
                    
                    if (NULL == ptItem) {
                        /* just in case the iterator doesn't support ring mode */
                        ptItem = ARM_2D_INVOKE(fnIterator, 
                                    ARM_2D_PARAM(
                                        ptThis, 
                                        __ARM_2D_LIST_GET_LAST_ITEM,
                                        this.Runtime.hwSelection));
                        assert(NULL != ptItem);
                        
                        if (NULL == ptItem) {
                            /* this shouldn't happen */
                            return ARM_2D_ERR_NOT_AVAILABLE;
                        }
                    }
                } while(0);
            
                while(++iSteps) {
                    if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_VERTICAL) {
                        nOffsetChange += ptItem->tSize.iHeight;
                    } else {
                        nOffsetChange += ptItem->tSize.iWidth;
                    }
                    
                    nOffsetChange += ptItem->Padding.chNext;
                    nOffsetChange += ptItem->Padding.chPrevious;
                    
                    ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_GET_PREVIOUS,
                            this.Runtime.hwSelection));
                    
                    if (NULL == ptItem) {
                        /* just in case the iterator doesn't support ring mode */
                        ptItem = ARM_2D_INVOKE(fnIterator, 
                                    ARM_2D_PARAM(
                                        ptThis, 
                                        __ARM_2D_LIST_GET_LAST_ITEM,
                                        this.Runtime.hwSelection));
                        assert(NULL != ptItem);
                        
                        if (NULL == ptItem) {
                            /* this shouldn't happen */
                            return ARM_2D_ERR_NOT_AVAILABLE;
                        }
                    }
                } 
            }

            hwTargetID = ptItem->hwID;
        } while(0);


        /* calculate compenstation for iStartOffset */
        do {
            int16_t iStartOffset;
            if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_VERTICAL) {
                iStartOffset
                            = (     this.Runtime.tileList.tRegion.tSize.iHeight 
                                -   ptItem->tSize.iHeight) 
                            >> 1;
            } else {
                iStartOffset
                            = (     this.Runtime.tileList.tRegion.tSize.iWidth 
                                -   ptItem->tSize.iWidth) 
                            >> 1;
            }
            iStartOffset -= ptItem->Padding.chPrevious;
        
            nOffsetChange += iStartOffset - this.CalMidAligned.iStartOffset;
        } while(0);


        /* resume id */
        ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                        hwSaveID));
    } while(0);


    arm_irq_safe {
        this.Runtime.hwSelection = hwTargetID;
        this.Runtime.nStartOffset = this.Runtime.nOffset;
        this.Runtime.nTargetOffset += nOffsetChange;
        this.Runtime.lPeriod = lPeriod;
        this.Runtime.lTimestamp = 0;
    }
    
    return ARM_2D_ERR_NONE;
}


/*----------------------------------------------------------------------------*
 * Region Calculator                                                          *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1,2)
arm_2d_list_item_t *__arm_2d_list_core_get_item(
                        __arm_2d_list_core_t *ptThis,
                        __arm_2d_list_item_iterator *fnIterator,
                        arm_2d_list_iterator_dir_t tDirection,
                        uint16_t hwID,
                        bool bIgnoreStatusCheck)
{
    assert(NULL != ptThis);
    assert(NULL != fnIterator);
    
    arm_2d_list_item_t *ptItem = NULL;

    ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        tDirection,
                        hwID));

    if (    (tDirection == __ARM_2D_LIST_GET_CURRENT)
        ||  (tDirection == __ARM_2D_LIST_GET_ITEM_WITH_ID_WITHOUT_MOVE_POINTER)
        ||  (tDirection == __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER)) {
        return ptItem;
    }

    if (NULL == ptItem) {
        if (tDirection == __ARM_2D_LIST_GET_NEXT) {
            /* just in case the iterator won't support ring mode */
            ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_GET_FIRST_ITEM,
                        0));
            assert(NULL != ptItem);
        } else if (tDirection == __ARM_2D_LIST_GET_PREVIOUS) {
            /* just in case the iterator won't support ring mode */
            ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_GET_LAST_ITEM,
                        0));
            assert(NULL != ptItem);
        } else {
            return NULL;
        }
    }

    do {
        if ((ptItem->bIsEnabled && ptItem->bIsVisible) || bIgnoreStatusCheck) {
            break;
        }
        
        switch (tDirection) {
            case __ARM_2D_LIST_GET_LAST_ITEM_WITHOUT_MOVE_POINTER:
            case __ARM_2D_LIST_GET_LAST_ITEM:
            case __ARM_2D_LIST_GET_PREVIOUS:
                ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_GET_PREVIOUS,
                        0));
                
                if (NULL == ptItem) {
                    /* just in case the iterator won't support ring mode */
                    ptItem = ARM_2D_INVOKE(fnIterator, 
                            ARM_2D_PARAM(
                                ptThis, 
                                __ARM_2D_LIST_GET_LAST_ITEM,
                                0));
                    assert(NULL != ptItem);
                }
                break;
            case __ARM_2D_LIST_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER:
            case __ARM_2D_LIST_GET_FIRST_ITEM:
            case __ARM_2D_LIST_GET_NEXT:
                ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_GET_NEXT,
                        0));
                
                if (NULL == ptItem) {
                    /* just in case the iterator won't support ring mode */
                    ptItem = ARM_2D_INVOKE(fnIterator, 
                            ARM_2D_PARAM(
                                ptThis, 
                                __ARM_2D_LIST_GET_FIRST_ITEM,
                                0));
                    assert(NULL != ptItem);
                }
                break;
            default:
                assert(false);
        }
    } while(true);

    return ptItem;
}

static
bool __arm_2d_list_core_update( __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator)
{
    arm_2d_list_item_t *ptItem = NULL;
    
    /* update start-offset */
    if (this.CalMidAligned.bListHeightChanged) {
        this.CalMidAligned.bListHeightChanged = false;
        
        /* update the iStartOffset */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER,
                            0,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            return false;
        }

        if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
            this.CalMidAligned.iStartOffset
            = (     this.Runtime.tileList.tRegion.tSize.iWidth 
                -   ptItem->tSize.iWidth) 
            >> 1;
        } else {
            this.CalMidAligned.iStartOffset
            = (     this.Runtime.tileList.tRegion.tSize.iHeight 
                -   ptItem->tSize.iHeight) 
            >> 1;
        }
        
        this.CalMidAligned.iStartOffset -= ptItem->Padding.chPrevious;
    }

    /* update total length and item count */
    if (    (0 == this.tCFG.nTotalLength)
        ||  (0 == this.tCFG.hwItemCount)) {

        uint16_t hwItemCount = 0; 
        uint32_t nTotalLength = 0;
        
        /* update the iStartOffset */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            false);
                            
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            return false;
        }
        
        uint16_t hwStartID = ptItem->hwID;
        
        do {
            hwItemCount++;
            if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
                nTotalLength += ptItem->tSize.iWidth 
                              + ptItem->Padding.chPrevious 
                              + ptItem->Padding.chNext;
            } else {
                nTotalLength += ptItem->tSize.iHeight 
                              + ptItem->Padding.chPrevious 
                              + ptItem->Padding.chNext;
            }
            ptItem = __arm_2d_list_core_get_item(   
                                ptThis, 
                                fnIterator, 
                                __ARM_2D_LIST_GET_NEXT,
                                0,
                                false);

            if (NULL == ptItem) {
                break;
            }
            if (ptItem->hwID == hwStartID) {
                break;  /* reach the starting point again */
            }
        } while(true);
        
        this.tCFG.nTotalLength = nTotalLength;

        if (0 == this.tCFG.hwItemCount) {
            this.tCFG.hwItemCount = hwItemCount;
        }
    }
    
    return true;
}


static
bool __arm_2d_list_core_update_fixed_size_no_status_check( 
                                        __arm_2d_list_core_t *ptThis,
                                        __arm_2d_list_item_iterator *fnIterator)
{
    arm_2d_list_item_t *ptItem = NULL;
    
    /* update start-offset */
    if (this.CalMidAligned.bListHeightChanged) {
        this.CalMidAligned.bListHeightChanged = false;
        
        /* update the iStartOffset */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER,
                            0,
                            true);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            return false;
        }

        if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
            this.CalMidAligned.iStartOffset
            = (     this.Runtime.tileList.tRegion.tSize.iWidth 
                -   ptItem->tSize.iWidth) 
            >> 1;
        } else {
            this.CalMidAligned.iStartOffset
            = (     this.Runtime.tileList.tRegion.tSize.iHeight 
                -   ptItem->tSize.iHeight) 
            >> 1;
        }
        
        this.CalMidAligned.iStartOffset -= ptItem->Padding.chPrevious;
    }

    /* update total length and item count */
    if (0 == this.tCFG.hwItemCount) {
        uint16_t hwItemCount = 0; 
        uint32_t nTotalLength = 0;
        
        /* update the iStartOffset */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            true);
                            
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            return false;
        }
        
        uint16_t hwStartID = ptItem->hwID;
        
        do {
            hwItemCount++;
            if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
                nTotalLength += ptItem->tSize.iWidth 
                              + ptItem->Padding.chPrevious 
                              + ptItem->Padding.chNext;
            } else {
                nTotalLength += ptItem->tSize.iHeight 
                              + ptItem->Padding.chPrevious 
                              + ptItem->Padding.chNext;
            }
            ptItem = __arm_2d_list_core_get_item(   
                                ptThis, 
                                fnIterator, 
                                __ARM_2D_LIST_GET_NEXT,
                                0,
                                true);

            if (NULL == ptItem) {
                break;
            }
            if (ptItem->hwID == hwStartID) {
                break;  /* reach the starting point again */
            }
        } while(true);
        
        this.tCFG.nTotalLength = nTotalLength;
        this.tCFG.hwItemCount = hwItemCount;
    } else if (0 == this.tCFG.nTotalLength){
        /* update the iStartOffset */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            true);
                            
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            return false;
        }
        if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
            this.tCFG.nTotalLength = this.tCFG.hwItemCount 
                                   * ptItem->tSize.iWidth;
        } else {
            this.tCFG.nTotalLength = this.tCFG.hwItemCount 
                                   * ptItem->tSize.iHeight;
        }
    }

    return true;
}


__arm_2d_list_work_area_t *
ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_VERTICAL (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset
                            )
{

    arm_2d_list_item_t *ptItem = NULL;

    /* reset the calculator */
    if (!this.Runtime.bIsRegCalInit) {
        this.Runtime.bIsRegCalInit = true;
        this.CalMidAligned.chState = 0;
        this.Runtime.tWorkingArea.tDirection = ARM_2D_LIST_VERTICAL;
    }

ARM_PT_BEGIN(this.CalMidAligned.chState)

    if (!__arm_2d_list_core_update(ptThis, fnIterator)) {
        ARM_PT_RETURN(NULL)
    }

    if (this.tCFG.nTotalLength) {
        nOffset = nOffset % this.tCFG.nTotalLength;
        nOffset -= this.tCFG.nTotalLength;
    }

    /* find the first and last visible items */
    do {
        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        /* move the nOffset to the top invisible area */
        do {
            int32_t nStartY = nOffset 
                            +   this.CalMidAligned.iStartOffset 
                            +   ptItem->Padding.chPrevious;
                    
            if (nStartY <= 0) {
                break;
            } 
            nOffset -= this.tCFG.nTotalLength;
        } while(true);
        
        /* get the inital offset */
        this.CalMidAligned.nOffset = nOffset;

        int32_t nTempOffset = nOffset;
        
        while(NULL != ptItem) {
            int32_t nY1 = nTempOffset 
                            +   this.CalMidAligned.iStartOffset 
                            +   ptItem->Padding.chPrevious;
            int32_t nY2 = nY1 + ptItem->tSize.iHeight - 1;
        
            if (nY1 >= 0 || nY2 >=0) {
                /* we find the top item */
                break;
            }
            
            /* update nTempOffset */
            nTempOffset += ptItem->tSize.iHeight 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;
            
            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                                ptThis, 
                                fnIterator, 
                                __ARM_2D_LIST_GET_NEXT,
                                0,
                                false);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
        }
        
        /* mark the first visible item on top */
        this.CalMidAligned.iTopVisiableOffset = (int16_t)nTempOffset;
        this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;

        /* mark the last visible item on bottom */
        do {
            this.CalMidAligned.iBottomVisibleOffset = (int16_t)nTempOffset;
            this.CalMidAligned.hwBottomVisibleItemID = ptItem->hwID;
            
            /* update nTempOffset */
            nTempOffset += ptItem->tSize.iHeight 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;

            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_NEXT,
                    0,
                    false);

            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
            
            int32_t nY1 = nTempOffset 
                        +   this.CalMidAligned.iStartOffset 
                        +   ptItem->Padding.chPrevious;
            
            if (nY1 >= this.Runtime.tileList.tRegion.tSize.iHeight) {
                /* this item is not visible */
                break;
            }
        } while(true);
    } while(0);


    /* start draw items */
    do {
        /* move to the top item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwTopVisibleItemID,
                    false);

        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iTopVisiableOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX = 0;
        
        //! calculate distance and opacity
        do {
            int16_t iDistance 
                = this.Runtime.tWorkingArea.tRegion.tLocation.iY
                + (this.Runtime.tWorkingArea.tRegion.tSize.iHeight >> 1);
            int16_t iCentre = this.Runtime.tileList.tRegion.tSize.iHeight >> 1;
            
            iDistance = iCentre - iDistance;
            this.Runtime.tWorkingArea.tParam.hwRatio = ABS(iDistance);
            
            if (this.Runtime.tWorkingArea.tParam.hwRatio > iCentre) {
                this.Runtime.tWorkingArea.tParam.chOpacity = 0;
            } else {
                this.Runtime.tWorkingArea.tParam.chOpacity 
                    = 255 - this.Runtime.tWorkingArea.tParam.hwRatio * 255 
                          / iCentre;
            }
        } while(0);

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )

        if (    this.CalMidAligned.iBottomVisibleOffset 
            <=  this.CalMidAligned.iTopVisiableOffset) {
            break;
        }

        /* resume local context */
        ptItem = this.Runtime.tWorkingArea.ptItem;

        /* update top visiable item id and offset */
        do {
            this.CalMidAligned.iTopVisiableOffset += ptItem->tSize.iHeight 
                                                  + ptItem->Padding.chPrevious
                                                  + ptItem->Padding.chNext;
            
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_NEXT,
                        0,
                        false);

            this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;
        } while(0);

        /* move to the bottom item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwBottomVisibleItemID,
                    false);
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iBottomVisibleOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX = 0;
        
        //! calculate distance and opacity
        do {
            int16_t iDistance 
                = this.Runtime.tWorkingArea.tRegion.tLocation.iY
                + (this.Runtime.tWorkingArea.tRegion.tSize.iHeight >> 1);
            int16_t iCentre = this.Runtime.tileList.tRegion.tSize.iHeight >> 1;
            
            iDistance = iCentre - iDistance;
            this.Runtime.tWorkingArea.tParam.hwRatio = ABS(iDistance);
            
            if (this.Runtime.tWorkingArea.tParam.hwRatio > iCentre) {
                this.Runtime.tWorkingArea.tParam.chOpacity = 0;
            } else {
                this.Runtime.tWorkingArea.tParam.chOpacity 
                    = 255 - this.Runtime.tWorkingArea.tParam.hwRatio * 255 
                          / iCentre;
            }
        } while(0);

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )


        if (    this.CalMidAligned.iBottomVisibleOffset 
            <=  this.CalMidAligned.iTopVisiableOffset) {
            break;
        }

        /* update bottom visiable item id and offset */
        do {
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_PREVIOUS,
                        0,
                        false);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }

            this.CalMidAligned.hwBottomVisibleItemID = ptItem->hwID;
            
            this.CalMidAligned.iBottomVisibleOffset 
                -= ptItem->tSize.iHeight 
                 + ptItem->Padding.chPrevious
                 + ptItem->Padding.chNext;

        } while(0);

    } while(true);

ARM_PT_END()

    return NULL;
}


__arm_2d_list_work_area_t *
ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_HORIZONTAL (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset
                            )
{

    arm_2d_list_item_t *ptItem = NULL;

    /* reset the calculator */
    if (!this.Runtime.bIsRegCalInit) {
        this.Runtime.bIsRegCalInit = true;
        this.CalMidAligned.chState = 0;
        this.Runtime.tWorkingArea.tDirection = ARM_2D_LIST_HORIZONTAL;
    }

ARM_PT_BEGIN(this.CalMidAligned.chState)

    if (!__arm_2d_list_core_update(ptThis, fnIterator)) {
        ARM_PT_RETURN(NULL)
    }

    if (this.tCFG.nTotalLength) {
        nOffset = nOffset % this.tCFG.nTotalLength;
        nOffset -= this.tCFG.nTotalLength;
    }

    /* find the first and last visible items */
    do {
        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        /* move the nOffset to the top invisible area */
        do {
            int32_t nStartX = nOffset 
                            +   this.CalMidAligned.iStartOffset 
                            +   ptItem->Padding.chPrevious;
                    
            if (nStartX <= 0) {
                break;
            } 
            nOffset -= this.tCFG.nTotalLength;
        } while(true);
        
        /* get the inital offset */
        this.CalMidAligned.nOffset = nOffset;

        int32_t nTempOffset = nOffset;
        
        while(NULL != ptItem) {
            int32_t nX1 = nTempOffset 
                            +   this.CalMidAligned.iStartOffset 
                            +   ptItem->Padding.chPrevious;
            int32_t nX2 = nX1 + ptItem->tSize.iWidth - 1;
        
            if (nX1 >= 0 || nX2 >=0) {
                /* we find the top item */
                break;
            }
            
            /* update nTempOffset */
            nTempOffset += ptItem->tSize.iWidth 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;
            
            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                                ptThis, 
                                fnIterator, 
                                __ARM_2D_LIST_GET_NEXT,
                                0,
                                false);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
        }
        
        /* mark the first visible item on top */
        this.CalMidAligned.iTopVisiableOffset = (int16_t)nTempOffset;
        this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;

        /* mark the last visible item on bottom */
        do {
            this.CalMidAligned.iBottomVisibleOffset = (int16_t)nTempOffset;
            this.CalMidAligned.hwBottomVisibleItemID = ptItem->hwID;
            
            /* update nTempOffset */
            nTempOffset += ptItem->tSize.iWidth 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;

            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_NEXT,
                    0,
                    false);

            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
            
            int32_t nX1 = nTempOffset 
                            +   this.CalMidAligned.iStartOffset 
                            +   ptItem->Padding.chPrevious;
            
            if (nX1 >= this.Runtime.tileList.tRegion.tSize.iWidth) {
                /* this item is not visible */
                break;
            }
        } while(true);
    } while(0);


    /* start draw items */
    do {
        /* move to the top item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwTopVisibleItemID,
                    false);

        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX 
            = this.CalMidAligned.iTopVisiableOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY = 0;
        
        //! calculate distance and opacity
        do {
            int16_t iDistance 
                = this.Runtime.tWorkingArea.tRegion.tLocation.iX
                + (this.Runtime.tWorkingArea.tRegion.tSize.iWidth >> 1);
            int16_t iCentre = this.Runtime.tileList.tRegion.tSize.iWidth >> 1;
            
            iDistance = iCentre - iDistance;
            this.Runtime.tWorkingArea.tParam.hwRatio = ABS(iDistance);
            
            if (this.Runtime.tWorkingArea.tParam.hwRatio > iCentre) {
                this.Runtime.tWorkingArea.tParam.chOpacity = 0;
            } else {
                this.Runtime.tWorkingArea.tParam.chOpacity 
                    = 255 - this.Runtime.tWorkingArea.tParam.hwRatio * 255 
                          / iCentre;
            }
        } while(0);

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )

        if (    this.CalMidAligned.iBottomVisibleOffset 
            <=  this.CalMidAligned.iTopVisiableOffset) {
            break;
        }

        /* resume local context */
        ptItem = this.Runtime.tWorkingArea.ptItem;

        /* update top visiable item id and offset */
        do {
            this.CalMidAligned.iTopVisiableOffset += ptItem->tSize.iWidth 
                                                  + ptItem->Padding.chPrevious
                                                  + ptItem->Padding.chNext;
            
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_NEXT,
                        0,
                        false);

            this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;
        } while(0);

        /* move to the bottom item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwBottomVisibleItemID,
                    false);
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX 
            = this.CalMidAligned.iBottomVisibleOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY = 0;

        //! calculate distance and opacity
        do {
            int16_t iDistance 
                = this.Runtime.tWorkingArea.tRegion.tLocation.iX
                + (this.Runtime.tWorkingArea.tRegion.tSize.iWidth >> 1);
            int16_t iCentre = this.Runtime.tileList.tRegion.tSize.iWidth >> 1;
            
            iDistance = iCentre - iDistance;
            this.Runtime.tWorkingArea.tParam.hwRatio = ABS(iDistance);
            
            if (this.Runtime.tWorkingArea.tParam.hwRatio > iCentre) {
                this.Runtime.tWorkingArea.tParam.chOpacity = 0;
            } else {
                this.Runtime.tWorkingArea.tParam.chOpacity 
                    = 255 - this.Runtime.tWorkingArea.tParam.hwRatio * 255 
                          / iCentre;
            }
        } while(0);

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )


        if (    this.CalMidAligned.iBottomVisibleOffset 
            <=  this.CalMidAligned.iTopVisiableOffset) {
            break;
        }

        /* update bottom visiable item id and offset */
        do {
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_PREVIOUS,
                        0,
                        false);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }

            this.CalMidAligned.hwBottomVisibleItemID = ptItem->hwID;
            
            this.CalMidAligned.iBottomVisibleOffset 
                -= ptItem->tSize.iWidth 
                 + ptItem->Padding.chPrevious
                 + ptItem->Padding.chNext;

        } while(0);

    } while(true);

ARM_PT_END()

    return NULL;
}


__arm_2d_list_work_area_t *
ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_FIXED_SIZED_ITEM_NO_STATUS_CHECK_VERTICAL (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset
                            )
{

    arm_2d_list_item_t *ptItem = NULL;

    /* reset the calculator */
    if (!this.Runtime.bIsRegCalInit) {
        this.Runtime.bIsRegCalInit = true;
        this.CalMidAligned.chState = 0;
        this.Runtime.tWorkingArea.tDirection = ARM_2D_LIST_VERTICAL;
    }

ARM_PT_BEGIN(this.CalMidAligned.chState)

    if (!__arm_2d_list_core_update_fixed_size_no_status_check(  ptThis, 
                                                                fnIterator)) {
        ARM_PT_RETURN(NULL)
    }

    if (this.tCFG.nTotalLength) {
        nOffset = nOffset % this.tCFG.nTotalLength;
        nOffset -= this.tCFG.nTotalLength;
    }

    /* find the first and last visible items */
    do {
        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            true);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        /* move the nOffset to the top invisible area */
        do {
            int32_t nStartY = nOffset 
                            +   this.CalMidAligned.iStartOffset 
                            +   ptItem->Padding.chPrevious;
                    
            if (nStartY <= 0) {
                break;
            } 
            nOffset -= this.tCFG.nTotalLength;
        } while(true);
        
        /* get the inital offset */
        this.CalMidAligned.nOffset = nOffset;

        int32_t nTempOffset = nOffset;
        
        do {
            int16_t iItemActualHeight = ptItem->tSize.iHeight 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;
            
            int32_t nLength = ABS(nTempOffset) - this.CalMidAligned.iStartOffset;
            int32_t nCount = nLength / iItemActualHeight;
            
            nTempOffset += nCount * iItemActualHeight;
            
            do {
                int32_t nY1 = nTempOffset 
                                +   this.CalMidAligned.iStartOffset 
                                +   ptItem->Padding.chPrevious;
                int32_t nY2 = nY1 + ptItem->tSize.iHeight - 1;
                
                if (nY1 >= 0 || nY2 >=0) {
                    /* we find the top item */
                    break;
                }
                nTempOffset += iItemActualHeight;
                nCount++;
            } while(0);
            
            uint16_t hwTempID = (ptItem->hwID + nCount) % this.tCFG.hwItemCount;
            
            
            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                                ptThis, 
                                fnIterator, 
                                __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                                hwTempID,
                                true);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
            
        } while(0);

        /* mark the first visible item on top */
        this.CalMidAligned.iTopVisiableOffset = (int16_t)nTempOffset;
        this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;

        /* mark the last visible item on bottom */
        do {
            this.CalMidAligned.iBottomVisibleOffset = (int16_t)nTempOffset;
            this.CalMidAligned.hwBottomVisibleItemID = ptItem->hwID;
            
            /* update nTempOffset */
            nTempOffset += ptItem->tSize.iHeight 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;

            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_NEXT,
                    0,
                    true);

            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
            
            int32_t nY1 = nTempOffset 
                        +   this.CalMidAligned.iStartOffset 
                        +   ptItem->Padding.chPrevious;
            
            if (nY1 >= this.Runtime.tileList.tRegion.tSize.iHeight) {
                /* this item is not visible */
                break;
            }
        } while(true);
    } while(0);


    /* start draw items */
    do {
        /* move to the top item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwTopVisibleItemID,
                    true);

        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iTopVisiableOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX = 0;
        
        //! calculate distance and opacity
        do {
            int16_t iDistance 
                = this.Runtime.tWorkingArea.tRegion.tLocation.iY
                + (this.Runtime.tWorkingArea.tRegion.tSize.iHeight >> 1);
            int16_t iCentre = this.Runtime.tileList.tRegion.tSize.iHeight >> 1;
            
            iDistance = iCentre - iDistance;
            this.Runtime.tWorkingArea.tParam.hwRatio = ABS(iDistance);
            
            if (this.Runtime.tWorkingArea.tParam.hwRatio > iCentre) {
                this.Runtime.tWorkingArea.tParam.chOpacity = 0;
            } else {
                this.Runtime.tWorkingArea.tParam.chOpacity 
                    = 255 - this.Runtime.tWorkingArea.tParam.hwRatio * 255 
                          / iCentre;
            }
        } while(0);

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )

        if (    this.CalMidAligned.iBottomVisibleOffset 
            <=  this.CalMidAligned.iTopVisiableOffset) {
            break;
        }

        /* resume local context */
        ptItem = this.Runtime.tWorkingArea.ptItem;

        /* update top visiable item id and offset */
        do {
            this.CalMidAligned.iTopVisiableOffset += ptItem->tSize.iHeight 
                                                  + ptItem->Padding.chPrevious
                                                  + ptItem->Padding.chNext;
            
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_NEXT,
                        0,
                        true);

            this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;
        } while(0);

        /* move to the bottom item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwBottomVisibleItemID,
                    true);
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iBottomVisibleOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX = 0;
        
        //! calculate distance and opacity
        do {
            int16_t iDistance 
                = this.Runtime.tWorkingArea.tRegion.tLocation.iY
                + (this.Runtime.tWorkingArea.tRegion.tSize.iHeight >> 1);
            int16_t iCentre = this.Runtime.tileList.tRegion.tSize.iHeight >> 1;
            
            iDistance = iCentre - iDistance;
            this.Runtime.tWorkingArea.tParam.hwRatio = ABS(iDistance);
            
            if (this.Runtime.tWorkingArea.tParam.hwRatio > iCentre) {
                this.Runtime.tWorkingArea.tParam.chOpacity = 0;
            } else {
                this.Runtime.tWorkingArea.tParam.chOpacity 
                    = 255 - this.Runtime.tWorkingArea.tParam.hwRatio * 255 
                          / iCentre;
            }
        } while(0);

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )


        if (    this.CalMidAligned.iBottomVisibleOffset 
            <=  this.CalMidAligned.iTopVisiableOffset) {
            break;
        }

        /* update bottom visiable item id and offset */
        do {
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_PREVIOUS,
                        0,
                        true);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }

            this.CalMidAligned.hwBottomVisibleItemID = ptItem->hwID;
            
            this.CalMidAligned.iBottomVisibleOffset 
                -= ptItem->tSize.iHeight 
                 + ptItem->Padding.chPrevious
                 + ptItem->Padding.chNext;

        } while(0);

    } while(true);

ARM_PT_END()

    return NULL;
}




__arm_2d_list_work_area_t *
ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_FIXED_SIZED_ITEM_NO_STATUS_CHECK_HORIZONTAL(
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset
                            )
{

    arm_2d_list_item_t *ptItem = NULL;

    /* reset the calculator */
    if (!this.Runtime.bIsRegCalInit) {
        this.Runtime.bIsRegCalInit = true;
        this.CalMidAligned.chState = 0;
        this.Runtime.tWorkingArea.tDirection = ARM_2D_LIST_HORIZONTAL;
    }

ARM_PT_BEGIN(this.CalMidAligned.chState)

    if (!__arm_2d_list_core_update_fixed_size_no_status_check(  ptThis, 
                                                                fnIterator)) {
        ARM_PT_RETURN(NULL)
    }

    if (this.tCFG.nTotalLength) {
        nOffset = nOffset % this.tCFG.nTotalLength;
        nOffset -= this.tCFG.nTotalLength;
    }

    /* find the first and last visible items */
    do {
        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            true);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        /* move the nOffset to the top invisible area */
        do {
            int32_t nStartX = nOffset 
                            +   this.CalMidAligned.iStartOffset 
                            +   ptItem->Padding.chPrevious;
                    
            if (nStartX <= 0) {
                break;
            } 
            nOffset -= this.tCFG.nTotalLength;
        } while(true);
        
        /* get the inital offset */
        this.CalMidAligned.nOffset = nOffset;

        int32_t nTempOffset = nOffset;
        
        do {
            int16_t iItemActualWidth = ptItem->tSize.iWidth 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;
            
            int32_t nLength = ABS(nTempOffset) - this.CalMidAligned.iStartOffset;
            int32_t nCount = nLength / iItemActualWidth;
            
            nTempOffset += nCount * iItemActualWidth;
            
            do {
                int32_t nX1 = nTempOffset 
                                +   this.CalMidAligned.iStartOffset 
                                +   ptItem->Padding.chPrevious;
                int32_t nX2 = nX1 + ptItem->tSize.iWidth - 1;
                
                if (nX1 >= 0 || nX2 >=0) {
                    /* we find the top item */
                    break;
                }
                nTempOffset += iItemActualWidth;
                nCount++;
            } while(0);
            
            uint16_t hwTempID = (ptItem->hwID + nCount) % this.tCFG.hwItemCount;
            
            
            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                                ptThis, 
                                fnIterator, 
                                __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                                hwTempID,
                                true);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
            
        } while(0);

        /* mark the first visible item on top */
        this.CalMidAligned.iTopVisiableOffset = (int16_t)nTempOffset;
        this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;

        /* mark the last visible item on bottom */
        do {
            this.CalMidAligned.iBottomVisibleOffset = (int16_t)nTempOffset;
            this.CalMidAligned.hwBottomVisibleItemID = ptItem->hwID;
            
            /* update nTempOffset */
            nTempOffset += ptItem->tSize.iWidth 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;

            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_NEXT,
                    0,
                    true);

            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
            
            int32_t nX1 = nTempOffset 
                        +   this.CalMidAligned.iStartOffset 
                        +   ptItem->Padding.chPrevious;
            
            if (nX1 >= this.Runtime.tileList.tRegion.tSize.iWidth) {
                /* this item is not visible */
                break;
            }
        } while(true);
    } while(0);


    /* start draw items */
    do {
        /* move to the top item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwTopVisibleItemID,
                    true);

        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX 
            = this.CalMidAligned.iTopVisiableOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY = 0;
        
        //! calculate distance and opacity
        do {
            int16_t iDistance 
                = this.Runtime.tWorkingArea.tRegion.tLocation.iX
                + (this.Runtime.tWorkingArea.tRegion.tSize.iWidth >> 1);
            int16_t iCentre = this.Runtime.tileList.tRegion.tSize.iWidth >> 1;
            
            iDistance = iCentre - iDistance;
            this.Runtime.tWorkingArea.tParam.hwRatio = ABS(iDistance);
            
            if (this.Runtime.tWorkingArea.tParam.hwRatio > iCentre) {
                this.Runtime.tWorkingArea.tParam.chOpacity = 0;
            } else {
                this.Runtime.tWorkingArea.tParam.chOpacity 
                    = 255 - this.Runtime.tWorkingArea.tParam.hwRatio * 255 
                          / iCentre;
            }
        } while(0);

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )

        if (    this.CalMidAligned.iBottomVisibleOffset 
            <=  this.CalMidAligned.iTopVisiableOffset) {
            break;
        }

        /* resume local context */
        ptItem = this.Runtime.tWorkingArea.ptItem;

        /* update top visiable item id and offset */
        do {
            this.CalMidAligned.iTopVisiableOffset += ptItem->tSize.iWidth 
                                                  + ptItem->Padding.chPrevious
                                                  + ptItem->Padding.chNext;
            
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_NEXT,
                        0,
                        true);

            this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;
        } while(0);

        /* move to the bottom item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwBottomVisibleItemID,
                    true);
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX 
            = this.CalMidAligned.iBottomVisibleOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY = 0;
        
        //! calculate distance and opacity
        do {
            int16_t iDistance 
                = this.Runtime.tWorkingArea.tRegion.tLocation.iX
                + (this.Runtime.tWorkingArea.tRegion.tSize.iWidth >> 1);
            int16_t iCentre = this.Runtime.tileList.tRegion.tSize.iWidth >> 1;
            
            iDistance = iCentre - iDistance;
            this.Runtime.tWorkingArea.tParam.hwRatio = ABS(iDistance);
            
            if (this.Runtime.tWorkingArea.tParam.hwRatio > iCentre) {
                this.Runtime.tWorkingArea.tParam.chOpacity = 0;
            } else {
                this.Runtime.tWorkingArea.tParam.chOpacity 
                    = 255 - this.Runtime.tWorkingArea.tParam.hwRatio * 255 
                          / iCentre;
            }
        } while(0);

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )


        if (    this.CalMidAligned.iBottomVisibleOffset 
            <=  this.CalMidAligned.iTopVisiableOffset) {
            break;
        }

        /* update bottom visiable item id and offset */
        do {
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_PREVIOUS,
                        0,
                        true);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }

            this.CalMidAligned.hwBottomVisibleItemID = ptItem->hwID;
            
            this.CalMidAligned.iBottomVisibleOffset 
                -= ptItem->tSize.iWidth 
                 + ptItem->Padding.chPrevious
                 + ptItem->Padding.chNext;

        } while(0);

    } while(true);

ARM_PT_END()

    return NULL;
}



/*----------------------------------------------------------------------------*
 * Iterator                                                                   *
 *----------------------------------------------------------------------------*/

arm_2d_list_item_t *ARM_2D_LIST_ITERATOR_ARRAY(
                                        __arm_2d_list_core_t *ptThis,
                                        arm_2d_list_iterator_dir_t tDirection,
                                        uint_fast16_t hwID
                                    )
{
    int32_t nIterationIndex;
    switch (tDirection) {
        default:
        case __ARM_2D_LIST_GET_ITEM_WITH_ID_WITHOUT_MOVE_POINTER:
            nIterationIndex = hwID;
            nIterationIndex %= this.tCFG.hwItemCount;
            break;

        case __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER:
            this.Runtime.Iterator.Array.hwIndex = hwID;
            this.Runtime.Iterator.Array.hwIndex %= this.tCFG.hwItemCount;
            nIterationIndex = this.Runtime.Iterator.Array.hwIndex;
            break;

        case __ARM_2D_LIST_GET_PREVIOUS:
            if (this.Runtime.Iterator.Array.hwIndex) {
                this.Runtime.Iterator.Array.hwIndex--;
            } else {
                this.Runtime.Iterator.Array.hwIndex = this.tCFG.hwItemCount - 1;
            }
            nIterationIndex = this.Runtime.Iterator.Array.hwIndex;
            break;

        case __ARM_2D_LIST_GET_NEXT:
            this.Runtime.Iterator.Array.hwIndex++;
            this.Runtime.Iterator.Array.hwIndex %= this.tCFG.hwItemCount;
            
            nIterationIndex = this.Runtime.Iterator.Array.hwIndex ;
            break;

        case __ARM_2D_LIST_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER:
            nIterationIndex = 0;
            break;

        case __ARM_2D_LIST_GET_FIRST_ITEM:
            this.Runtime.Iterator.Array.hwIndex  = 0;
            nIterationIndex = this.Runtime.Iterator.Array.hwIndex ;
            break;

        case __ARM_2D_LIST_GET_CURRENT:
            nIterationIndex = this.Runtime.Iterator.Array.hwIndex ;
            break;

        case __ARM_2D_LIST_GET_LAST_ITEM_WITHOUT_MOVE_POINTER:
            nIterationIndex = this.tCFG.hwItemCount - 1;
            break;

        case __ARM_2D_LIST_GET_LAST_ITEM:
            this.Runtime.Iterator.Array.hwIndex  = this.tCFG.hwItemCount - 1;
            nIterationIndex = this.Runtime.Iterator.Array.hwIndex ;
            break;
    }



    nIterationIndex %= this.tCFG.hwItemCount;

#define __REF_ITEM_ARRAY(__PTR, __INDEX) (arm_2d_list_item_t *)                 \
                                    (   ((uintptr_t)(__PTR))                    \
                                    +   this.tCFG.hwItemSizeInByte * (__INDEX))
    arm_2d_list_item_t *ptItem 
        = __REF_ITEM_ARRAY(this.tCFG.ptItems, nIterationIndex);
    if (!(ptItem->bIsReadOnly)) {
        ptItem->hwID = nIterationIndex;
    }
    
    return ptItem;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif
