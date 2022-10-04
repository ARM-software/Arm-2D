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
 * $Date:        04. Oct 2022
 * $Revision:    V.0.6.0
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
arm_2d_err_t __arm_2d_list_view_init(   __arm_2d_list_view_t *ptThis,
                                        __arm_2d_list_view_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(NULL != ptCFG->fnCalculator);
    assert(NULL != ptCFG->fnIterator);

    memset(ptThis, 0, sizeof(__arm_2d_list_view_t));
    this.tCFG = *ptCFG;

    if (this.tCFG.hwSwitchingPeriodInMs) {
        this.Runtime.iPeriod 
            = arm_2d_helper_convert_ms_to_ticks(this.tCFG.hwSwitchingPeriodInMs);
    } else {
        this.Runtime.iPeriod = arm_2d_helper_convert_ms_to_ticks(500);          /*!< use 500 ms as default */
    }

    return ARM_2D_ERR_NONE;
}


ARM_NONNULL(1,2)
arm_fsm_rt_t __arm_2d_list_view_show(   __arm_2d_list_view_t *ptThis,
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
        if (NULL == ptRegion) {
            ptRegion = &ptTarget->tRegion;
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
                                               &this.Runtime.tileList, 
                                               false)) {
            /* nothing to draw: use the unified exist point */
            goto label_end_of_list_view_task;
        }
        
        if (bIsNewFrame) {
            int64_t lTimestamp = arm_2d_helper_get_system_timestamp();
            if (this.Runtime.iTargetOffset != this.Runtime.iOffset) {
                /* code for update this.Runtime.iOffset */
                int32_t nElapsed = (int32_t)(lTimestamp - this.Runtime.lTimestamp);
                
                if (nElapsed < this.Runtime.iPeriod) {
                    int32_t iDelta = this.Runtime.iTargetOffset - this.Runtime.iStartOffset;
                    iDelta = nElapsed * iDelta / this.Runtime.iPeriod;
                    
                    this.Runtime.iOffset = this.Runtime.iStartOffset + iDelta;
                } else {
                    /* timeout */
                    this.Runtime.iOffset = this.Runtime.iTargetOffset;
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
    ARM_PT_GOTO_PREV_ENTRY()
        } else if (tResult < 0) {
            // error was reported
    ARM_PT_RETURN(tResult)
        } else if (arm_fsm_rt_cpl != tResult) {
    ARM_PT_REPORT_STATUS(tResult)
        } 

    ARM_PT_YIELD()
    } while(0);


    assert(NULL != this.tCFG.fnCalculator);
    
    /* loop until finishing drawing all visiable area */
    do {
        /* call calculator: it should handle the padding */
        if (NULL == ARM_2D_INVOKE(  this.tCFG.fnCalculator,
                        ARM_2D_PARAM(
                            ptThis, 
                            this.tCFG.fnIterator,
                            this.Runtime.iOffset))) {
            /* finish: use the unified exist point */
            goto label_end_of_list_view_task;
        }
        arm_2d_list_view_item_t *ptItem = this.Runtime.tWorkingArea.ptItem;
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
        ARM_PT_GOTO_PREV_ENTRY()
            } else if (tResult < 0) {
                // error was reported
        ARM_PT_RETURN(tResult)
            } else if (arm_fsm_rt_cpl != tResult) {
        ARM_PT_REPORT_STATUS(tResult)
            } 

        ARM_PT_YIELD()
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
        ARM_PT_GOTO_PREV_ENTRY()
            } else if (tResult < 0) {
                // error was reported
        ARM_PT_RETURN(tResult)
            } else if (arm_fsm_rt_cpl != tResult) {
        ARM_PT_REPORT_STATUS(tResult)
            } 
        ARM_PT_YIELD()
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
    ARM_PT_GOTO_PREV_ENTRY()
        } else if (tResult < 0) {
            // error was reported
    ARM_PT_RETURN(tResult)
        } else if (arm_fsm_rt_cpl != tResult) {
    ARM_PT_REPORT_STATUS(tResult)
        } 

    ARM_PT_YIELD()
    } while(0);


label_end_of_list_view_task:
ARM_PT_END()
    
    return arm_fsm_rt_cpl;
}



#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif
