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
 * Description:  Public header file for list view related services
 *
 * $Date:        05. Oct 2022
 * $Revision:    V.0.7.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

#define __ARM_2D_HELPER_LIST_VIEW_IMPLEMENT__

#include <stdbool.h>
#include <stdint.h>
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
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wunused-value"
#endif

/*============================ MACROS ========================================*/
#undef this
#define this        (*ptThis)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
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
        this.Runtime.nPeriod 
            = arm_2d_helper_convert_ms_to_ticks(this.tCFG.hwSwitchingPeriodInMs);
    } else {
        this.Runtime.nPeriod = arm_2d_helper_convert_ms_to_ticks(500);          /*!< use 500 ms as default */
    }

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

//        arm_2d_region_t tListRegion = {
//            .tSize = this.tCFG.tListSize,
//        };

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
            int64_t lTimestamp = arm_2d_helper_get_system_timestamp();
            if (this.Runtime.nTargetOffset != this.Runtime.nOffset) {
                /* code for update this.Runtime.iOffset */
                int32_t nElapsed = (int32_t)(lTimestamp - this.Runtime.lTimestamp);
                
                if (nElapsed < this.Runtime.nPeriod) {
                    int32_t iDelta = this.Runtime.nTargetOffset - this.Runtime.nStartOffset;
                    iDelta = nElapsed * iDelta / this.Runtime.nPeriod;
                    
                    this.Runtime.nOffset = this.Runtime.nStartOffset + iDelta;
                } else {
                    /* timeout */
                    this.Runtime.nOffset = this.Runtime.nTargetOffset;
                    this.Runtime.lTimestamp = lTimestamp;
                }
            } else {
                this.Runtime.lTimestamp = lTimestamp;    /* reset timestamp */
            }
        }
        
    } while(0);

    /* draw background */
    do {
    ARM_PT_ENTRY()

        tResult = ARM_2D_INVOKE(this.tCFG.fnOnDrawListViewBackground,
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
        
        ptItem->ptListView = ptThis;
        
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

        /* draw list view item background */
        do {
        ARM_PT_ENTRY()
            ptItem = this.Runtime.tWorkingArea.ptItem;
            
            tResult = ARM_2D_INVOKE(this.tCFG.fnOnDrawItemBackground,
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

        /* draw list view item */
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

    /* draw list view cover */
    do {
    ARM_PT_ENTRY()

        tResult = ARM_2D_INVOKE(this.tCFG.fnOnDrawListViewCover,
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


label_end_of_list_core_task:
ARM_PT_END()
    
    return arm_fsm_rt_cpl;
}

/*----------------------------------------------------------------------------*
 * Region Calculator                                                          *
 *----------------------------------------------------------------------------*/
__arm_2d_list_core_work_area_t *
ARM_2D_LIST_VIEW_CALCULATOR_MIDDLE_ALIGNED_VERTICAL (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_core_item_iterator *fnIterator,
                                int32_t nOffset
                            )
{

    arm_2d_list_item_t *ptItem = NULL;
    
ARM_PT_BEGIN(this.CalMidAligned.chState)

    /* update start-offset */
    if (this.CalMidAligned.bListHeightChanged) {
        this.CalMidAligned.bListHeightChanged = false;
        
         /* update the iStartOffset */
         ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER,
                        0));
        this.CalMidAligned.iStartOffset
            = (     this.Runtime.tileList.tRegion.tSize.iHeight 
                -   ptItem->tSize.iHeight) 
            >> 1;
        this.CalMidAligned.iStartOffset -= ptItem->Padding.chPrevious;
    }

    /* update total length and item count */
    if (    (0 == this.tCFG.nTotalLength)
        ||  (0 == this.tCFG.hwItemCount)) {

        uint16_t hwItemCount = 0; 
        uint32_t nTotalLength = 0;
        
        /* update the iStartOffset */
        ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM,
                        0));
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        uint16_t hwStartID = ptItem->hwID;
        
        do {
            hwItemCount++;
            nTotalLength += ptItem->tSize.iHeight 
                          + ptItem->Padding.chPrevious 
                          + ptItem->Padding.chNext;
            
            ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_NEXT,
                        0));
            if (NULL == ptItem) {
                break;
            }
            if (ptItem->hwID == hwStartID) {
                break;  /* reach the starting point again */
            }
        } while(true);
        
        this.tCFG.nTotalLength = nTotalLength;
        this.tCFG.hwItemCount = hwItemCount;
        
    } while(0);

    if (this.tCFG.nTotalLength) {
        nOffset = nOffset % this.tCFG.nTotalLength;
        nOffset -= this.tCFG.nTotalLength;
    }

    /* find the first and last visible items */
    do {
        /* move to the first item */
        ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM,
                        0));
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
            ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_VIEW_GET_NEXT,
                            0));

            /* just in case the iterator won't support ring mode */
            if (NULL == ptItem) {
                ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM,
                        0));
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
            ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_VIEW_GET_NEXT,
                            0));

            /* just in case the iterator won't support ring mode */
            if (NULL == ptItem) {
                ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM,
                        0));
                /* this should not happen as we checked before */
                assert(NULL != ptItem);
            }
            
            int32_t nX1 = nTempOffset 
                            +   this.CalMidAligned.iStartOffset 
                            +   ptItem->Padding.chPrevious;
            
            if (nX1 >= this.Runtime.tileList.tRegion.tSize.iHeight) {
                /* this item is not visible */
                break;
            }
        } while(true);
    } while(0);


    /* start draw items */
    do {
        /* move to the top item */
        ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_ITEM_AND_MOVE_POINTER,
                        this.CalMidAligned.hwTopVisibleItemID));
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iTopVisiableOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )

        if (    this.CalMidAligned.hwTopVisibleItemID 
            ==  this.CalMidAligned.hwBottomVisibleItemID) {
            /* reach the centre item */
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
            ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_VIEW_GET_NEXT,
                            0));
            /* just in case the iterator won't support ring mode */
            if (NULL == ptItem) {
                ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_FIRST_ITEM,
                        0));
                /* this should not happen as we checked before */
                assert(NULL != ptItem);
            }
            this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;
        } while(0);

        /* move to the bottom item */
        ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_ITEM_AND_MOVE_POINTER,
                        this.CalMidAligned.hwBottomVisibleItemID));
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iBottomVisibleOffset 
            + this.CalMidAligned.iStartOffset 
            +   ptItem->Padding.chPrevious;

    ARM_PT_YIELD( &this.Runtime.tWorkingArea )

        /* update bottom visiable item id and offset */
        do {
            /* move to the top item */
            ptItem = ARM_2D_INVOKE(fnIterator, 
                        ARM_2D_PARAM(
                            ptThis, 
                            __ARM_2D_LIST_VIEW_GET_PREVIOUS,
                            0));
            /* just in case the iterator won't support ring mode */
            if (NULL == ptItem) {
                ptItem = ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_VIEW_GET_LAST_ITEM,
                        0));
                /* this should not happen as we checked before */
                assert(NULL != ptItem);
            }

            this.CalMidAligned.hwBottomVisibleItemID = ptItem->hwID;
            
            this.CalMidAligned.iBottomVisibleOffset 
                -= ptItem->tSize.iHeight 
                 + ptItem->Padding.chPrevious
                 + ptItem->Padding.chNext;

        } while(0);
        


//        if (    this.CalMidAligned.hwTopVisibleItemID 
//            ==  this.CalMidAligned.hwBottomVisibleItemID) {
//            /* reach the centre item */
//            break;
//        }

    } while(true);

ARM_PT_END()

    return NULL;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif
