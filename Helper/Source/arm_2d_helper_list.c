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
 * $Date:        28. Dec 2024
 * $Revision:    V.2.3.1
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
#   pragma clang diagnostic ignored "-Wswitch-default"
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 188,546
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wunused-value"
#   pragma GCC diagnostic ignored "-Wnonnull-compare"
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe174,Pe546
#endif

/*============================ MACROS ========================================*/
#undef this
#define this        (*ptThis)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static
arm_2d_err_t __arm_2d_list_core_move_selection( __arm_2d_list_core_t *ptThis, 
                                                int16_t iSteps,
                                                int32_t nFinishInMs);
/*============================ LOCAL VARIABLES ===============================*/
static const arm_2d_helper_pi_slider_cfg_t c_tDefaultPICFG = {
    .fProportion = 0.030f,
    .fIntegration = 0.0115f,
    .nInterval = 5,
};

/*============================ IMPLEMENTATION ================================*/

__STATIC_INLINE
int32_t __arm_2d_list_safe_mod(int32_t nDividend, int32_t nDivisor, bool bMidAligned)
{
    if (bMidAligned) {
        nDividend %= nDivisor;
    }
#if 0
    int32_t nModeResult = nDividend % nDivisor;

    if (ABS(nDividend) > (nDivisor * 2)) {
        if (nDividend >= 0) {
            nDividend = nModeResult + nDivisor;
        } else {
            nDividend = nModeResult - nDivisor;
        }
    }
#endif

    return nDividend;
}

ARM_NONNULL(1,2)
arm_2d_err_t __arm_2d_list_core_init(   __arm_2d_list_core_t *ptThis,
                                        __arm_2d_list_core_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(NULL != ptCFG->ptCalculator);
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

    /* PI slider selection indicator */
    if (this.tCFG.bUsePISliderForSelectionIndicator) {
        if (NULL == this.tCFG.ptPISliderCFG) {
            arm_2d_helper_pi_slider_init(   &this.Runtime.tPISlider, 
                                            (arm_2d_helper_pi_slider_cfg_t *)&c_tDefaultPICFG, 
                                            0);
            
        } else {
            arm_2d_helper_pi_slider_init(   &this.Runtime.tPISlider, 
                                            this.tCFG.ptPISliderCFG, 
                                            0);
        }
    }
    
    this.bListSizeChanged = true;
    this.Runtime.bIsRegCalInit = false;

    return ARM_2D_ERR_NONE;
}


ARM_NONNULL(1)
arm_2d_size_t __arm_2d_list_core_get_list_size(__arm_2d_list_core_t *ptThis)
{
    assert(NULL != ptThis);
    return this.tCFG.tListSize;
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

        arm_2d_canvas(&(this.Runtime.tileTarget), __top_container, true) {

            arm_2d_align_centre(__top_container, this.tCFG.tListSize) {
                /* get target tile */
                if (NULL == arm_2d_tile_generate_child(&this.Runtime.tileTarget, 
                                                       &__centre_region,
                                                       &this.Runtime.tileList, 
                                                       false)) {
                    /* nothing to draw: use the unified exist point */
                    goto label_end_of_list_core_task;
                }
            }
        }
        
        if (bIsNewFrame) {

            if (!this.Runtime.bIsRegCalInit) {
                ARM_2D_INVOKE(  this.tCFG.ptCalculator->fnCalculator,
                            ARM_2D_PARAM(
                                ptThis, 
                                this.tCFG.fnIterator,
                                //this.Runtime.Selection.nOffset
                                this.nOffset
                                ));
                this.chState = 0;
            }

            int32_t nOldOffset = this.Runtime.Selection.nOffset;
            bool bUpdateOffset = false;
            if (0 == this.Runtime.MoveReq.nFinishInMs && 0 != this.Runtime.MoveReq.iSteps) {
                bUpdateOffset = true;
            } else if (this.tCFG.bUsePISliderForSelectionIndicator) {
                int16_t iCurrentOffset = 0;
                if (arm_2d_helper_pi_slider(&this.Runtime.tPISlider, 
                                            this.Runtime.Selection.nTargetOffset, 
                                            &iCurrentOffset)) {

                    this.Runtime.bIsMoving = false;     /* update flag to indicate moving complete */
                    //this.Runtime.Selection.nTargetOffset = iCurrentOffset;
                    this.Runtime.Selection.nStartOffset = iCurrentOffset;

                }
                this.Runtime.Selection.nOffset = iCurrentOffset;
                bUpdateOffset = true;
            
            } else if (__arm_2d_helper_time_liner_slider(   
                                                    this.Runtime.Selection.nStartOffset,  /* from */
                                                    this.Runtime.Selection.nTargetOffset, /* to */
                                                    this.Runtime.lPeriod,       /* finish in specified period */
                                                    &this.Runtime.Selection.nOffset,      /* output offset */
                                                    &this.Runtime.lTimestamp)) {/* timestamp */

                this.Runtime.Selection.nTargetOffset = this.Runtime.Selection.nOffset;
                this.Runtime.Selection.nStartOffset = this.Runtime.Selection.nTargetOffset;

                this.Runtime.bIsMoving = false;     /* update flag to indicate moving complete */

                bUpdateOffset = true;
            }

            if (bUpdateOffset) {
                /* check whether there are new request or not */
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

            if (nOldOffset != this.Runtime.Selection.nOffset) {
                this.Runtime.bNeedRedraw = true;    /* update the sticky bit, it is cleared by user */
            }
        }
    } while(0);

    /* call calculator before drawing the background */
    if (NULL == ARM_2D_INVOKE(  this.tCFG.ptCalculator->fnCalculator,
                    ARM_2D_PARAM(
                        ptThis, 
                        this.tCFG.fnIterator,
                        this.nOffset
                        ))) {
        /* finish: use the unified exist point */
        goto label_end_of_list_core_task;
        //break;
    }

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


    assert(NULL != this.tCFG.ptCalculator);
    
    /* loop until finishing drawing all visiable area */
    do {

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
            goto label_next_list_item;
        }
        
        this.Runtime.tWorkingArea.tRegion.tSize.iHeight 
            -= ptItem->Margin.chTop + ptItem->Margin.chBottom;
        if (0 == this.Runtime.tWorkingArea.tRegion.tSize.iHeight) {
            goto label_next_list_item;
        }

        /* update selected field */
        this.Runtime.tWorkingArea.tParam.bIsSelected 
            = (this.Runtime.hwSelection == ptItem->hwID);

        if (!arm_2d_helper_pfb_is_region_active(&this.Runtime.tileList, 
                                                &this.Runtime.tWorkingArea.tRegion, 
                                                true)) {
            goto label_next_list_item;
        }

        if (NULL == arm_2d_tile_generate_child(&this.Runtime.tileList, 
                                               &this.Runtime.tWorkingArea.tRegion,
                                               &this.Runtime.tileItem,
                                               false)) {
            goto label_next_list_item;
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

label_next_list_item:

        /* call calculator: it should handle the padding */
        if (NULL == ARM_2D_INVOKE(  this.tCFG.ptCalculator->fnCalculator,
                        ARM_2D_PARAM(
                            ptThis, 
                            this.tCFG.fnIterator,
                            this.nOffset
                            ))) {
            /* finish: use the unified exist point */
            //goto label_end_of_list_core_task;
            break;
        }

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


    /* finish drawing */
    if (!this.Runtime.bIsMoving) {
        if (this.tCFG.nTotalLength > 0) {
            int32_t nModeResult = this.nOffset % this.tCFG.nTotalLength;

            int32_t nDelta = this.Runtime.Selection.nOffset - this.nOffset;

            this.nOffset = nModeResult;
            this.Runtime.Selection.nOffset = this.nOffset + nDelta;

            this.Runtime.Selection.nStartOffset = this.Runtime.Selection.nOffset;
            this.Runtime.Selection.nTargetOffset = this.Runtime.Selection.nOffset;

            arm_2d_helper_pi_slider_set_current(&this.Runtime.tPISlider, this.Runtime.Selection.nOffset);
        }
    }


label_end_of_list_core_task:
ARM_PT_END()
    
    return arm_fsm_rt_cpl;
}

#if 0
ARM_NONNULL(1)
void __arm_2d_list_core_move_offset(__arm_2d_list_core_t *ptThis, 
                                    int16_t iOffset)
{
    assert(NULL != ptThis);

    arm_irq_safe {
        this.Runtime.Selection.nOffset += iOffset;
        this.Runtime.Selection.nTargetOffset = this.Runtime.Selection.nOffset;
    }
}
#endif

ARM_NONNULL(1)
void __arm_2d_list_core_move_request(   __arm_2d_list_core_t *ptThis, 
                                        int16_t iSteps,
                                        int32_t nFinishInMs)
{
    assert(NULL != ptThis);
    arm_irq_safe {
        this.Runtime.MoveReq.iSteps = iSteps;
        this.Runtime.MoveReq.nFinishInMs = nFinishInMs;
        this.Runtime.bIsMoving = true;
    }
}

ARM_NONNULL(1)
uint16_t __arm_2d_list_core_get_selected_item_id(__arm_2d_list_core_t *ptThis)
{
    assert(NULL != ptThis);
    return this.Runtime.hwSelection;
}

ARM_NONNULL(1)
arm_2d_list_item_t *__arm_2d_list_core_get_selected_item(__arm_2d_list_core_t *ptThis)
{
    assert(NULL != ptThis);
    
    return ARM_2D_INVOKE(this.tCFG.fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_GET_ITEM_WITH_ID_WITHOUT_MOVE_POINTER,
                        this.Runtime.hwSelection));
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
    if (!this.tCFG.bUsePISliderForSelectionIndicator) {
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
       
        arm_2d_list_item_t *ptItem = __arm_2d_list_core_get_item(   
                                        ptThis, 
                                        fnIterator,  
                                        __ARM_2D_LIST_GET_CURRENT, 
                                        0, 
                                        this.tCFG.bDisableStatusCheck,
                                        false);

        if (NULL == ptItem) {
            return ARM_2D_ERR_NOT_AVAILABLE;
        }
        
        uint16_t hwSaveID = ptItem->hwID;
        int16_t iMovedSteps = 0;
        do {

            ptItem = __arm_2d_list_core_get_item(   
                                        ptThis, 
                                        fnIterator,  
                                        __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER, 
                                        this.Runtime.hwSelection, 
                                        this.tCFG.bDisableStatusCheck,
                                        false);

            if (NULL == ptItem) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }
            

            if (iSteps > 0) {
                do { 
                    /* next */
                    arm_2d_list_item_t *ptItemNew = __arm_2d_list_core_get_item(
                                                ptThis, 
                                                fnIterator,  
                                                __ARM_2D_LIST_GET_NEXT, 
                                                this.Runtime.hwSelection, 
                                                this.tCFG.bDisableStatusCheck,
                                                false);

                    if (NULL == ptItemNew) {
                        if (this.tCFG.bDisableRingMode) {
                            break;
                        } else {
                            /* this shouldn't happen */
                            return ARM_2D_ERR_NOT_AVAILABLE;
                        }
                    } 
                    
                    if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
                        nOffsetChange -= ptItem->tSize.iWidth;
                    } else {
                        nOffsetChange -= ptItem->tSize.iHeight;
                    }
                
                    nOffsetChange -= ptItem->Padding.chNext;
                    nOffsetChange -= ptItem->Padding.chPrevious;

                    ptItem = ptItemNew;                    
                    iMovedSteps++;

                } while(--iSteps);

            } else {

                do {
                    arm_2d_list_item_t *ptItemNew = __arm_2d_list_core_get_item(
                                                ptThis, 
                                                fnIterator,  
                                                __ARM_2D_LIST_GET_PREVIOUS, 
                                                this.Runtime.hwSelection, 
                                                this.tCFG.bDisableStatusCheck,
                                                false);

                    if (NULL == ptItemNew) {
                        if (this.tCFG.bDisableRingMode) {
                            break;
                        } else {
                            /* this shouldn't happen */
                            return ARM_2D_ERR_NOT_AVAILABLE;
                        }
                    } 

                    if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_VERTICAL) {
                        nOffsetChange += ptItem->tSize.iHeight;
                    } else {
                        nOffsetChange += ptItem->tSize.iWidth;
                    }
                    
                    nOffsetChange += ptItem->Padding.chNext;
                    nOffsetChange += ptItem->Padding.chPrevious;

                    ptItem = ptItemNew;

                    iMovedSteps++;

                } while(++iSteps);
            }

            hwTargetID = ptItem->hwID;
        } while(0);

        if (0 == iMovedSteps) {
            return ARM_2D_ERR_OUT_OF_REGION;
        }

        nOffsetChange += ARM_2D_INVOKE(this.tCFG.ptCalculator->fnSelectionCompensation,
                            ARM_2D_PARAM(ptThis, ptItem));

        /* resume id */
        ARM_2D_INVOKE(fnIterator, 
                    ARM_2D_PARAM(
                        ptThis, 
                        __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                        hwSaveID));

    } while(0);

    if (0 == nFinishInMs && !this.tCFG.bUsePISliderForSelectionIndicator) {
        /* jump to the new position immediately */
        arm_irq_safe {
            int32_t nNewOffset = this.Runtime.Selection.nTargetOffset + nOffsetChange;

            if (this.Runtime.Selection.nOffset != nNewOffset) {
                this.Runtime.bNeedRedraw = true;
            }
            this.Runtime.Selection.nOffset = nNewOffset;

            this.Runtime.Selection.nTargetOffset = this.Runtime.Selection.nOffset;
            this.Runtime.Selection.nStartOffset = this.Runtime.Selection.nTargetOffset;
            this.Runtime.bIsMoving = false;  /* update flag to indicate the list is moving */
            this.Runtime.hwSelection = hwTargetID;

            /* reset */
            this.Runtime.lTimestamp = 0;
            this.Runtime.MoveReq.iSteps = 0;
            this.Runtime.MoveReq.nFinishInMs = 0;
        }
    } else {
        arm_irq_safe {
            this.Runtime.hwSelection = hwTargetID;
            this.Runtime.Selection.nStartOffset = this.Runtime.Selection.nOffset;
            this.Runtime.Selection.nTargetOffset += nOffsetChange;
            this.Runtime.lPeriod = lPeriod;
            this.Runtime.lTimestamp = 0;
            this.Runtime.bIsMoving = true;  /* update flag to indicate the list is moving */
        }
    }
    
    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
bool __arm_2d_list_core_need_redraw(__arm_2d_list_core_t *ptThis, bool bAutoreset)
{
    bool bNeedRedraw = false;

    assert(NULL != ptThis);

    arm_irq_safe {
        bNeedRedraw = this.Runtime.bNeedRedraw;
        if (bAutoreset) {
            /* auto reset */
            this.Runtime.bNeedRedraw = false;
        }
    }

    return bNeedRedraw;
}

ARM_NONNULL(1)
bool __arm_2d_list_core_is_list_moving(__arm_2d_list_core_t *ptThis)
{
    assert(NULL != ptThis);

    return this.Runtime.bIsMoving;
}

ARM_NONNULL(1)
bool __arm_2d_list_core_is_list_scrolling(__arm_2d_list_core_t *ptThis)
{
    assert(NULL != ptThis);

    return this.Runtime.bScrolling;
}

ARM_NONNULL(1)
bool __arm_2d_list_core_indicator_pi_mode_config(
                                    __arm_2d_list_core_t *ptThis, 
                                    bool bEnable, 
                                    arm_2d_helper_pi_slider_cfg_t *ptNewCFG)
{
    assert(NULL != ptThis);
    bool bPrevious = this.tCFG.bUsePISliderForSelectionIndicator;

    if (!bEnable) {
        this.tCFG.bUsePISliderForSelectionIndicator = false;
    } else if (NULL != ptNewCFG) {

        this.tCFG.bUsePISliderForSelectionIndicator = true;

        if (NULL != ptNewCFG) {
            /* we cannot trust this pointer, because the address could be a temporary one*/
            this.tCFG.ptPISliderCFG = ptNewCFG;   

            arm_2d_helper_pi_slider_init(&this.Runtime.tPISlider, ptNewCFG, this.nOffset);
        }
    }

    return bPrevious;
}

ARM_NONNULL(1,2)
arm_2d_list_item_t *__arm_2d_list_core_get_item(
                        __arm_2d_list_core_t *ptThis,
                        __arm_2d_list_item_iterator *fnIterator,
                        arm_2d_list_iterator_dir_t tDirection,
                        uint16_t hwID,
                        bool bIgnoreStatusCheck,
                        bool bForceRingMode)
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
        if (this.tCFG.bDisableRingMode && !bForceRingMode) {
            return NULL;
        } else {
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
    }

    do {
        if (ptItem->bIsVisible || bIgnoreStatusCheck) {
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
                    if (this.tCFG.bDisableRingMode && !bForceRingMode) {
                        return NULL;
                    } else {
                        /* just in case the iterator won't support ring mode */
                        ptItem = ARM_2D_INVOKE(fnIterator, 
                                ARM_2D_PARAM(
                                    ptThis, 
                                    __ARM_2D_LIST_GET_LAST_ITEM,
                                    0));
                        assert(NULL != ptItem);
                    }
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
                    if (this.tCFG.bDisableRingMode && !bForceRingMode) {
                        return NULL;
                    } else {
                        /* just in case the iterator won't support ring mode */
                        ptItem = ARM_2D_INVOKE(fnIterator, 
                                ARM_2D_PARAM(
                                    ptThis, 
                                    __ARM_2D_LIST_GET_FIRST_ITEM,
                                    0));
                        assert(NULL != ptItem);
                    }
                }
                break;
            default:
                assert(false);
        }
    } while(true);

    return ptItem;
}

/*----------------------------------------------------------------------------*
 * Region Calculator                                                          *
 *----------------------------------------------------------------------------*/

static
int16_t __selection_compensation_mid_aligned(__arm_2d_list_core_t *ptThis,
                                             arm_2d_list_item_t *ptItem)
{
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

    return iStartOffset - this.iStartOffset;
}

static
bool __arm_2d_list_core_update_normal( 
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                bool bMidAligned)
{
    arm_2d_list_item_t *ptItem = NULL;
    
    /* update start-offset */
    if (this.bListSizeChanged) {
        this.bListSizeChanged = false;
        
        /* update the iStartOffset */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER,
                            0,
                            false,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            return false;
        }

        if (bMidAligned) {
            if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
                this.iStartOffset
                = (     this.Runtime.tileList.tRegion.tSize.iWidth 
                    -   ptItem->tSize.iWidth) 
                >> 1;
            } else {
                this.iStartOffset
                = (     this.Runtime.tileList.tRegion.tSize.iHeight 
                    -   ptItem->tSize.iHeight) 
                >> 1;
            }
        } else {
            this.iStartOffset = 0;
        }
        
        this.iStartOffset -= ptItem->Padding.chPrevious;
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
                            false,
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
                                false,
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
                                        __arm_2d_list_item_iterator *fnIterator,
                                        bool bMidAligned)
{
    arm_2d_list_item_t *ptItem = NULL;
    

    /* update start-offset */
    if (this.bListSizeChanged) {
        this.bListSizeChanged = false;
        
        /* update the iStartOffset */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER,
                            0,
                            true,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            return false;
        }

        if (bMidAligned) {
            if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
                this.iStartOffset
                = (     this.Runtime.tileList.tRegion.tSize.iWidth 
                    -   ptItem->tSize.iWidth) 
                >> 1;
            } else {
                this.iStartOffset
                = (     this.Runtime.tileList.tRegion.tSize.iHeight 
                    -   ptItem->tSize.iHeight) 
                >> 1;
            }
        } else {
            this.iStartOffset = 0;
        }
        
        this.iStartOffset -= ptItem->Padding.chPrevious;
    }

    /* update total length and item count */
    if (0 == this.tCFG.hwItemCount) {
        uint16_t hwItemCount = 0; 
        uint32_t nTotalLength = 0;
        
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            true,
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
                                true,
                                false);

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

        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            true,
                            false);
                            
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            return false;
        }
        if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
            this.tCFG.nTotalLength = this.tCFG.hwItemCount 
                                   *    (   ptItem->tSize.iWidth
                                        +   ptItem->Padding.chPrevious
                                        +   ptItem->Padding.chNext);
        } else {
            this.tCFG.nTotalLength = this.tCFG.hwItemCount 
                                   *    (   ptItem->tSize.iHeight
                                        +   ptItem->Padding.chPrevious
                                        +   ptItem->Padding.chNext);
        }
    }

    return true;
}

static
ARM_NONNULL(1,2,3)
arm_2d_err_t __calculator_offset_update(__arm_2d_list_core_t *ptThis, 
                                        __arm_2d_list_item_iterator *fnIterator,
                                        int32_t *pnOffset,
                                        bool bMidAligned)
{
    assert(NULL != pnOffset);
    assert(NULL != ptThis);
    assert(NULL != fnIterator);

    bool bIsNewFrame = (ARM_2D_RT_TRUE == arm_2d_target_tile_is_new_frame(&this.Runtime.tileList));

    int32_t nOffset = *pnOffset;
    int32_t nSelectionOffset = -this.Runtime.Selection.nOffset;

    if (this.tCFG.nTotalLength) {
        nSelectionOffset = __arm_2d_list_safe_mod(nSelectionOffset, this.tCFG.nTotalLength, false);
    
    }

    int32_t nVisualWindowStart = (-nOffset);
    
    if (this.tCFG.nTotalLength) {
        nVisualWindowStart = __arm_2d_list_safe_mod(nVisualWindowStart, this.tCFG.nTotalLength, false);
    }

    /* move to the first item */
    arm_2d_list_item_t *ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_ITEM_WITH_ID_WITHOUT_MOVE_POINTER,
                        this.Runtime.hwSelection,
                        true,
                        false);
    if (NULL == ptItem) {
        /* no valid item, return NULL */
        return ARM_2D_ERR_NOT_AVAILABLE;
    }

    
    bool bWindowMoved = false;

    if (!bMidAligned) {
        if (nSelectionOffset < nVisualWindowStart) {
            /* move visual window */
            int32_t nDelta = nVisualWindowStart - nSelectionOffset;

            this.nOffset += nDelta;
            nVisualWindowStart -= nDelta;

            nOffset += nDelta;
            bWindowMoved = true;
        } else {

            int16_t iItemSize = 0;
            int16_t iListRegionSize = 0;

            if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
                iItemSize = ptItem->tSize.iWidth;
                iListRegionSize = this.Runtime.tileList.tRegion.tSize.iWidth;
            } else {
                iItemSize = ptItem->tSize.iHeight;
                iListRegionSize = this.Runtime.tileList.tRegion.tSize.iHeight;
            }

            int16_t iItemActualSize = iItemSize + ptItem->Padding.chPrevious;

            int16_t iItemBottomBoarder = nSelectionOffset + iItemActualSize - 1;

            int16_t iVisualWindowBottomBoarder  = nVisualWindowStart 
                                                + iListRegionSize 
                                                - 1;
            if (iItemBottomBoarder > iVisualWindowBottomBoarder) {
                /* move visual window */
                int32_t nDelta = iItemBottomBoarder - iVisualWindowBottomBoarder;

                this.nOffset -= nDelta;

                nOffset -= nDelta;
                nVisualWindowStart += nDelta;
                bWindowMoved = true;
            }
        }

        if (bIsNewFrame) {
            this.Runtime.Selection.tRegion.tSize = ptItem->tSize;
            if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
                this.Runtime.Selection.tRegion.tLocation.iX = nSelectionOffset 
                                                            - nVisualWindowStart;
                this.Runtime.Selection.tRegion.tLocation.iY = 0;
            } else {
                this.Runtime.Selection.tRegion.tLocation.iX = 0;
                this.Runtime.Selection.tRegion.tLocation.iY = nSelectionOffset 
                                                            - nVisualWindowStart;
            }
        }

    } else if (bIsNewFrame) {
        this.Runtime.Selection.tRegion.tSize = ptItem->tSize;
        if (this.Runtime.tWorkingArea.tDirection == ARM_2D_LIST_HORIZONTAL) {
            this.Runtime.Selection.tRegion.tLocation.iX 
                                                = this.iStartOffset 
                                                + ptItem->Padding.chPrevious;
            this.Runtime.Selection.tRegion.tLocation.iY = 0;
        } else {
            this.Runtime.Selection.tRegion.tLocation.iX = 0;
            this.Runtime.Selection.tRegion.tLocation.iY 
                                                = this.iStartOffset 
                                                + ptItem->Padding.chPrevious;
        }
    }

    if (bIsNewFrame) {
        if (bMidAligned) {
            this.Runtime.bScrolling = this.Runtime.bIsMoving;
        } else {
            this.Runtime.bScrolling = !!bWindowMoved;
        }
    }


    (*pnOffset) = nOffset;

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
arm_2d_tile_t *__arm_2d_list_core_get_inner_tile(__arm_2d_list_core_t *ptThis)
{
    assert(NULL != ptThis);

    return &this.Runtime.tileList;
}

ARM_NONNULL(1,2)
arm_2d_region_t *__arm_2d_list_core_get_selection_region(__arm_2d_list_core_t *ptThis,
                                                         arm_2d_region_t *ptRegionBuffer)
{
    assert(NULL != ptThis);

    do {
        if (NULL == ptRegionBuffer) {
            break;
        }

        *ptRegionBuffer = this.Runtime.Selection.tRegion;
    } while(0);

    return ptRegionBuffer;
}


static
__arm_2d_list_work_area_t * __calculator_vertical (
                                    __arm_2d_list_core_t *ptThis,
                                    __arm_2d_list_item_iterator *fnIterator,
                                    int32_t nOffset,
                                    bool bMidAligned)
{

    arm_2d_list_item_t *ptItem = NULL;

    /* reset the calculator */
    if (!this.Runtime.bIsRegCalInit) {
        this.Runtime.bIsRegCalInit = true;
        this.chState = 0;
        this.Runtime.tWorkingArea.tDirection = ARM_2D_LIST_VERTICAL;
        this.tCFG.bDisableStatusCheck = false;
    }

ARM_PT_BEGIN(this.chState)

    if (!__arm_2d_list_core_update_normal(ptThis, fnIterator, bMidAligned)) {
        ARM_PT_RETURN(NULL)
    }

    if (bMidAligned) {
        this.nOffset = this.Runtime.Selection.nOffset;
        nOffset = this.nOffset;
    }

    if (ARM_2D_ERR_NONE != __calculator_offset_update(ptThis, fnIterator, &nOffset, bMidAligned)) {
        /* no valid item, return NULL */
        ARM_PT_RETURN(NULL)
    }

    if (this.tCFG.nTotalLength) {
        nOffset = __arm_2d_list_safe_mod(nOffset, this.tCFG.nTotalLength, bMidAligned);

        if (!this.tCFG.bDisableRingMode) {
            if (nOffset >= 0) {
                nOffset -= this.tCFG.nTotalLength;
            }
        }
    }

    /* no ring mode */
    if (this.tCFG.bDisableRingMode && bMidAligned) {
        
        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            false,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        /* move the nOffset to the top invisible area */
        do {
            int32_t nStartY = nOffset 
                            + this.iStartOffset 
                            + ptItem->Padding.chPrevious;
                    
            if (nStartY <= 0) {
                break;
            } 
            nOffset -= this.tCFG.nTotalLength;
        } while(true);

        int32_t nTempOffset = nOffset;
        
        /* find the centre most item */
        int32_t nCentreLocation = this.Runtime.tileList.tRegion.tSize.iHeight >> 1;
        int32_t nMinimalDistance = nCentreLocation;

        struct {
            int32_t nOffset;
            uint16_t hwID;
        } tTopMost, tBottomMost, tCentreMost = {
            .nOffset = 0,
            .hwID = 0,
        };

        while(NULL != ptItem) {
            int32_t nY1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
        
            int32_t nDistance = nCentreLocation - nY1;
            
            if (nDistance < 0) {
                /* cross the centre */
                break;
            }

            if (nDistance < nMinimalDistance) {
                nMinimalDistance = nDistance;
                tCentreMost.hwID = ptItem->hwID;
                tCentreMost.nOffset = nTempOffset;
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
                                false,
                                true);  /* force to use ring mode */
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
        }

        /* with centre most item, we start to find the bottom most item*/
        tBottomMost = tCentreMost;
        nTempOffset = tBottomMost.nOffset;

        /* move to the centre most item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    tCentreMost.hwID,
                    false,
                    false);
        assert(NULL != ptItem);

        do {
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
                    false,
                    false);

            if (NULL == ptItem) {
                /* end of the list */
                break;
            }
            
            int32_t nY1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
            
            if (nY1 >= this.Runtime.tileList.tRegion.tSize.iHeight) {
                /* this item is not visible */
                break;
            }

            tBottomMost.nOffset = nTempOffset;
            tBottomMost.hwID = ptItem->hwID;

        } while(true);
        this.CalMidAligned.iBottomVisibleOffset = (int16_t)tBottomMost.nOffset;
        this.CalMidAligned.hwBottomVisibleItemID = tBottomMost.hwID;

        /* move to the centre most item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    tCentreMost.hwID,
                    false,
                    false);
        assert(NULL != ptItem);

        /* with centre most item, we start to find the top most item*/
        tTopMost = tCentreMost;
        nTempOffset = tTopMost.nOffset;
        do {
            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_PREVIOUS,
                    0,
                    false,
                    false);

            if (NULL == ptItem) {
                /* end of the list */
                break;
            }

            /* update nTempOffset */
            nTempOffset -= ptItem->tSize.iHeight 
                        + ptItem->Padding.chPrevious
                        + ptItem->Padding.chNext;

            int32_t nY1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
            int32_t nY2 = nY1 + ptItem->tSize.iHeight - 1;

            if (nY2 <= 0) {
                /* this item is not visible */
                break;
            }

            tTopMost.nOffset = nTempOffset;
            tTopMost.hwID = ptItem->hwID;

        } while(true);
        this.CalMidAligned.iTopVisibleOffset = (int16_t)tTopMost.nOffset;
        this.CalMidAligned.hwTopVisibleItemID = tTopMost.hwID;

    } else {
        /* find the first and last visible items */

        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            false,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        if (!this.tCFG.bDisableRingMode) {
            /* move the nOffset to the top invisible area */
            do {
                int32_t nStartY = nOffset 
                                + this.iStartOffset 
                                + ptItem->Padding.chPrevious;
                        
                if (nStartY <= 0) {
                    break;
                } 
                nOffset -= this.tCFG.nTotalLength;
            } while(true);
        }

        int32_t nTempOffset = nOffset;
        
        while(NULL != ptItem) {
            if (nTempOffset >= 0) {
                assert(this.tCFG.bDisableRingMode);
                break;
            }

            int32_t nY1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
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
                                false,
                                false);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
        }
        
        /* mark the first visible item on top */
        this.CalMidAligned.iTopVisibleOffset = (int16_t)nTempOffset;
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
                    false,
                    false);

            if (NULL == ptItem) {
                if (this.tCFG.bDisableRingMode) {
                    break;
                } else {
                    /* no valid item, return NULL */
                    ARM_PT_RETURN(NULL)
                }
            }
            
            int32_t nY1 = nTempOffset 
                        +   this.iStartOffset 
                        +   ptItem->Padding.chPrevious;
            
            if (nY1 >= this.Runtime.tileList.tRegion.tSize.iHeight) {
                /* this item is not visible */
                break;
            }
        } while(true);
    }


    /* start draw items */
    do {
        /* move to the top item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwTopVisibleItemID,
                    false,
                    false);

        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iTopVisibleOffset 
            + this.iStartOffset 
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
            <=  this.CalMidAligned.iTopVisibleOffset) {
            break;
        }

        /* resume local context */
        ptItem = this.Runtime.tWorkingArea.ptItem;

        /* update top visiable item id and offset */
        do {
            this.CalMidAligned.iTopVisibleOffset += ptItem->tSize.iHeight 
                                                  + ptItem->Padding.chPrevious
                                                  + ptItem->Padding.chNext;
            
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_NEXT,
                        0,
                        false,
                        false);

            this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;
        } while(0);

        /* move to the bottom item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwBottomVisibleItemID,
                    false,
                    false);
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iBottomVisibleOffset 
            + this.iStartOffset 
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
            <=  this.CalMidAligned.iTopVisibleOffset) {
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
                        false,
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


static
__arm_2d_list_work_area_t *__calculator_horizontal (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset,
                                bool bMidAligned)
{

    arm_2d_list_item_t *ptItem = NULL;

    /* reset the calculator */
    if (!this.Runtime.bIsRegCalInit) {
        this.Runtime.bIsRegCalInit = true;
        this.chState = 0;
        this.Runtime.tWorkingArea.tDirection = ARM_2D_LIST_HORIZONTAL;
        this.tCFG.bDisableStatusCheck = false;
    }

ARM_PT_BEGIN(this.chState)

    if (!__arm_2d_list_core_update_normal(ptThis, fnIterator, bMidAligned)) {
        ARM_PT_RETURN(NULL)
    }

    if (bMidAligned) {
        this.nOffset = this.Runtime.Selection.nOffset;
        nOffset = this.nOffset;
    }

    if (ARM_2D_ERR_NONE != __calculator_offset_update(ptThis, fnIterator, &nOffset, bMidAligned)) {
        /* no valid item, return NULL */
        ARM_PT_RETURN(NULL)
    }

    if (this.tCFG.nTotalLength) {
        nOffset = __arm_2d_list_safe_mod(nOffset, this.tCFG.nTotalLength, bMidAligned);
        if (!this.tCFG.bDisableRingMode) {
            if (nOffset >= 0) {
                nOffset -= this.tCFG.nTotalLength;
            }
        }
    }

    /* no ring mode */
    if (this.tCFG.bDisableRingMode && bMidAligned) {
        
        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            false,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        /* move the nOffset to the top invisible area */
        do {
            int32_t nStartX = nOffset 
                            +   this.iStartOffset 
                            +   ptItem->Padding.chPrevious;
                    
            if (nStartX <= 0) {
                break;
            } 
            nOffset -= this.tCFG.nTotalLength;
        } while(true);

        int32_t nTempOffset = nOffset;
        
        /* find the centre most item */
        int32_t nCentreLocation = this.Runtime.tileList.tRegion.tSize.iWidth >> 1;
        int32_t nMinimalDistance = nCentreLocation;

        struct {
            int32_t nOffset;
            uint16_t hwID;
        } tTopMost, tBottomMost, tCentreMost = {
            .nOffset = 0,
            .hwID = 0,
        };

        while(NULL != ptItem) {
            int32_t nX1 = nTempOffset 
                            +   this.iStartOffset 
                            +   ptItem->Padding.chPrevious;
        
            int32_t nDistance = nCentreLocation - nX1;
            
            if (nDistance < 0) {
                /* cross the centre */
                break;
            }

            if (nDistance < nMinimalDistance) {
                nMinimalDistance = nDistance;
                tCentreMost.hwID = ptItem->hwID;
                tCentreMost.nOffset = nTempOffset;
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
                                false,
                                true);  /* force to use ring mode */
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
        }

        /* with centre most item, we start to find the bottom most item*/
        tBottomMost = tCentreMost;
        nTempOffset = tBottomMost.nOffset;

        /* move to the centre most item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    tCentreMost.hwID,
                    false,
                    false);
        assert(NULL != ptItem);

        do {
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
                    false,
                    false);

            if (NULL == ptItem) {
                /* end of the list */
                break;
            }
            
            int32_t nX1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
            
            if (nX1 >= this.Runtime.tileList.tRegion.tSize.iWidth) {
                /* this item is not visible */
                break;
            }

            tBottomMost.nOffset = nTempOffset;
            tBottomMost.hwID = ptItem->hwID;

        } while(true);
        this.CalMidAligned.iBottomVisibleOffset = (int16_t)tBottomMost.nOffset;
        this.CalMidAligned.hwBottomVisibleItemID = tBottomMost.hwID;

        /* move to the centre most item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    tCentreMost.hwID,
                    false,
                    false);
        assert(NULL != ptItem);

        /* with centre most item, we start to find the top most item*/
        tTopMost = tCentreMost;
        nTempOffset = tTopMost.nOffset;
        do {
            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_PREVIOUS,
                    0,
                    false,
                    false);

            if (NULL == ptItem) {
                /* end of the list */
                break;
            }

            /* update nTempOffset */
            nTempOffset -= ptItem->tSize.iWidth 
                        + ptItem->Padding.chPrevious
                        + ptItem->Padding.chNext;

            int32_t nX1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
            int32_t nX2 = nX1 + ptItem->tSize.iWidth - 1;

            if (nX2 <= 0) {
                /* this item is not visible */
                break;
            }

            tTopMost.nOffset = nTempOffset;
            tTopMost.hwID = ptItem->hwID;

        } while(true);
        this.CalMidAligned.iTopVisibleOffset = (int16_t)tTopMost.nOffset;
        this.CalMidAligned.hwTopVisibleItemID = tTopMost.hwID;

    } else {
        /* find the first and last visible items */
        

        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            false,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        if (!this.tCFG.bDisableRingMode) {
            /* move the nOffset to the top invisible area */
            do {
                int32_t nStartX = nOffset 
                                +   this.iStartOffset 
                                +   ptItem->Padding.chPrevious;
                        
                if (nStartX <= 0) {
                    break;
                } 
                nOffset -= this.tCFG.nTotalLength;
            } while(true);
        }

        int32_t nTempOffset = nOffset;
        
        
        while(NULL != ptItem) {

            if (nTempOffset >= 0) {
                assert(this.tCFG.bDisableRingMode);
                break;
            }

            int32_t nX1 = nTempOffset 
                            +   this.iStartOffset 
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
                                false,
                                true);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
        }
        
        /* mark the first visible item on top */
        this.CalMidAligned.iTopVisibleOffset = (int16_t)nTempOffset;
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
                    false,
                    false);

            if (NULL == ptItem) {
                if (this.tCFG.bDisableRingMode) {
                    break;
                } else {
                    /* no valid item, return NULL */
                    ARM_PT_RETURN(NULL)
                }
            }
            
            int32_t nX1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
            
            if (nX1 >= this.Runtime.tileList.tRegion.tSize.iWidth) {
                /* this item is not visible */
                break;
            }
        } while(true);

    }

    /* start draw items */
    do {
        /* move to the top item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwTopVisibleItemID,
                    false,
                    false);

        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX 
            = this.CalMidAligned.iTopVisibleOffset 
            + this.iStartOffset 
            + ptItem->Padding.chPrevious;
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
            <=  this.CalMidAligned.iTopVisibleOffset) {
            break;
        }

        /* resume local context */
        ptItem = this.Runtime.tWorkingArea.ptItem;

        /* update top visiable item id and offset */
        do {
            this.CalMidAligned.iTopVisibleOffset += ptItem->tSize.iWidth 
                                                  + ptItem->Padding.chPrevious
                                                  + ptItem->Padding.chNext;
            
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_NEXT,
                        0,
                        false,
                        false);

            this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;
        } while(0);

        /* move to the bottom item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwBottomVisibleItemID,
                    false,
                    false);
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX 
            = this.CalMidAligned.iBottomVisibleOffset 
            + this.iStartOffset 
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
            <=  this.CalMidAligned.iTopVisibleOffset) {
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
                        false,
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

static
__arm_2d_list_work_area_t *
__calculator_fixed_sized_item_no_status_checking_vertical (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset,
                                bool bMidAligned)
{

    arm_2d_list_item_t *ptItem = NULL;

    /* reset the calculator */
    if (!this.Runtime.bIsRegCalInit) {
        this.Runtime.bIsRegCalInit = true;
        this.chState = 0;
        this.Runtime.tWorkingArea.tDirection = ARM_2D_LIST_VERTICAL;
        this.tCFG.bDisableStatusCheck = true;
    }

ARM_PT_BEGIN(this.chState)

    if (!__arm_2d_list_core_update_fixed_size_no_status_check(  ptThis, 
                                                                fnIterator,
                                                                bMidAligned)) {
        ARM_PT_RETURN(NULL)
    }

    if (bMidAligned) {
        this.nOffset = this.Runtime.Selection.nOffset;
        nOffset = this.nOffset;
    }

    if (ARM_2D_ERR_NONE != __calculator_offset_update(ptThis, fnIterator, &nOffset, bMidAligned)) {
        /* no valid item, return NULL */
        ARM_PT_RETURN(NULL)
    }

    if (this.tCFG.nTotalLength) {
        nOffset = __arm_2d_list_safe_mod(nOffset, this.tCFG.nTotalLength, bMidAligned);
        if (!this.tCFG.bDisableRingMode) {
            if (nOffset >= 0) {
                nOffset -= this.tCFG.nTotalLength;
            }
        }
    }

    /* no ring mode */
    if (this.tCFG.bDisableRingMode && bMidAligned) {
        
        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            true,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        /* move the nOffset to the top invisible area */
        do {
            int32_t nStartY = nOffset 
                            + this.iStartOffset 
                            + ptItem->Padding.chPrevious;
                    
            if (nStartY <= 0) {
                break;
            } 
            nOffset -= this.tCFG.nTotalLength;
        } while(true);

        int32_t nTempOffset = nOffset;

        /* find the centre most item */
        int32_t nCentreLocation = this.Runtime.tileList.tRegion.tSize.iHeight >> 1;
        int32_t nMinimalDistance = nCentreLocation;

        struct {
            int32_t nOffset;
            uint16_t hwID;
        } tTopMost, tBottomMost, tCentreMost = {
            .nOffset = 0,
            .hwID = 0,
        };

        while(NULL != ptItem) {
            int32_t nY1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
        
            int32_t nDistance = nCentreLocation - nY1;
            
            if (nDistance < 0) {
                /* cross the centre */
                break;
            }

            if (nDistance < nMinimalDistance) {
                nMinimalDistance = nDistance;
                //tCentreMost.ptItem = ptItem;
                tCentreMost.nOffset = nTempOffset;
                tCentreMost.hwID = ptItem->hwID;
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
                                true,
                                true);  /* force to use ring mode */
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
        }

        /* with centre most item, we start to find the bottom most item*/
        tBottomMost = tCentreMost;
        nTempOffset = tBottomMost.nOffset;

        /* move to the centre most item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    tCentreMost.hwID,
                    true,
                    false);
        assert(NULL != ptItem);

        do {
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
                    true,
                    false);

            if (NULL == ptItem) {
                /* end of the list */
                break;
            }
            
            int32_t nY1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
            
            if (nY1 >= this.Runtime.tileList.tRegion.tSize.iHeight) {
                /* this item is not visible */
                break;
            }

            tBottomMost.nOffset = nTempOffset;
            //tBottomMost.ptItem = ptItem;
            tBottomMost.hwID = ptItem->hwID;

        } while(true);
        this.CalMidAligned.iBottomVisibleOffset = (int16_t)tBottomMost.nOffset;
        this.CalMidAligned.hwBottomVisibleItemID = tBottomMost.hwID;

        /* move to the centre most item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    tCentreMost.hwID,
                    true,
                    false);
        assert(NULL != ptItem);

        /* with centre most item, we start to find the top most item*/
        tTopMost = tCentreMost;
        nTempOffset = tTopMost.nOffset;
        do {
            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_PREVIOUS,
                    0,
                    true,
                    false);

            if (NULL == ptItem) {
                /* end of the list */
                break;
            }

            /* update nTempOffset */
            nTempOffset -= ptItem->tSize.iHeight 
                        + ptItem->Padding.chPrevious
                        + ptItem->Padding.chNext;

            int32_t nY1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
            int32_t nY2 = nY1 + ptItem->tSize.iHeight - 1;

            if (nY2 <= 0) {
                /* this item is not visible */
                break;
            }

            tTopMost.nOffset = nTempOffset;
            //tTopMost.ptItem = ptItem;
            tTopMost.hwID = ptItem->hwID;

        } while(true);
        this.CalMidAligned.iTopVisibleOffset = (int16_t)tTopMost.nOffset;
        this.CalMidAligned.hwTopVisibleItemID = tTopMost.hwID;

    } else {
        /* find the first and last visible items */

        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            true,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        /* move the nOffset to the top invisible area */
        if (!this.tCFG.bDisableRingMode) {
            do {
                int32_t nStartY = nOffset 
                                +   this.iStartOffset 
                                +   ptItem->Padding.chPrevious;
                        
                if (nStartY <= 0) {
                    break;
                } 
                nOffset -= this.tCFG.nTotalLength;
            } while(true);
        }


        int32_t nTempOffset = nOffset;
        
        do {
            int32_t nCount = 0;

            if (nTempOffset < 0) {
                int16_t iItemActualHeight = ptItem->tSize.iHeight 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;

                int32_t nLength = ABS(nTempOffset) - this.iStartOffset;
                nCount = nLength / iItemActualHeight;
                nTempOffset += nCount * iItemActualHeight;
                
                do {
                    int32_t nY1 = nTempOffset 
                                    +   this.iStartOffset 
                                    +   ptItem->Padding.chPrevious;
                    int32_t nY2 = nY1 + ptItem->tSize.iHeight - 1;
                    
                    if (nY1 >= 0 || nY2 >=0) {
                        /* we find the top item */
                        break;
                    }
                    nTempOffset += iItemActualHeight;
                    nCount++;
                } while(true);
            } /*else {
                assert(this.tCFG.bDisableRingMode);
            } */
            
            uint16_t hwTempID = (ptItem->hwID + nCount) % this.tCFG.hwItemCount;
            
            
            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                                ptThis, 
                                fnIterator, 
                                __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                                hwTempID,
                                true,
                                false);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
            
        } while(0);

        /* mark the first visible item on top */
        this.CalMidAligned.iTopVisibleOffset = (int16_t)nTempOffset;
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
                    true,
                    false);

            if (NULL == ptItem) {
                if (this.tCFG.bDisableRingMode) {
                    break;
                } else {
                    /* no valid item, return NULL */
                    ARM_PT_RETURN(NULL)
                }
            }
            
            int32_t nY1 = nTempOffset 
                        +   this.iStartOffset 
                        +   ptItem->Padding.chPrevious;
            
            if (nY1 >= this.Runtime.tileList.tRegion.tSize.iHeight) {
                /* this item is not visible */
                break;
            }
        } while(true);
    }


    /* start draw items */
    do {
        /* move to the top item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwTopVisibleItemID,
                    true,
                    false);

        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iTopVisibleOffset 
            + this.iStartOffset 
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
            <=  this.CalMidAligned.iTopVisibleOffset) {
            break;
        }

        /* resume local context */
        ptItem = this.Runtime.tWorkingArea.ptItem;

        /* update top visiable item id and offset */
        do {
            this.CalMidAligned.iTopVisibleOffset += ptItem->tSize.iHeight 
                                                  + ptItem->Padding.chPrevious
                                                  + ptItem->Padding.chNext;
            
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_NEXT,
                        0,
                        true,
                        false);

            this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;
        } while(0);

        /* move to the bottom item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwBottomVisibleItemID,
                    true,
                    false);
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iY 
            = this.CalMidAligned.iBottomVisibleOffset 
            + this.iStartOffset 
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
            <=  this.CalMidAligned.iTopVisibleOffset) {
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
                        true,
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

static
__arm_2d_list_work_area_t *
__calculator_fixed_sized_item_no_status_checking_horizontal (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset,
                                bool bMidAligned)
{

    arm_2d_list_item_t *ptItem = NULL;

    /* reset the calculator */
    if (!this.Runtime.bIsRegCalInit) {
        this.Runtime.bIsRegCalInit = true;
        this.chState = 0;
        this.Runtime.tWorkingArea.tDirection = ARM_2D_LIST_HORIZONTAL;
        this.tCFG.bDisableStatusCheck = true;
    }

ARM_PT_BEGIN(this.chState)

    if (!__arm_2d_list_core_update_fixed_size_no_status_check(  ptThis, 
                                                                fnIterator,
                                                                bMidAligned)) {
        ARM_PT_RETURN(NULL)
    }

    if (bMidAligned) {
        this.nOffset = this.Runtime.Selection.nOffset;
        nOffset = this.nOffset;
    }

    if (ARM_2D_ERR_NONE != __calculator_offset_update(ptThis, fnIterator, &nOffset, bMidAligned)) {
        /* no valid item, return NULL */
        ARM_PT_RETURN(NULL)
    }

    if (this.tCFG.nTotalLength) {
        nOffset = __arm_2d_list_safe_mod(nOffset, this.tCFG.nTotalLength, bMidAligned);
        if (!this.tCFG.bDisableRingMode) {
            if (nOffset >= 0) {
                nOffset -= this.tCFG.nTotalLength;
            }
        }
    }

    /* no ring mode */
    if (this.tCFG.bDisableRingMode && bMidAligned) {
        
        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            true,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        /* move the nOffset to the top invisible area */
        do {
            int32_t nStartX = nOffset 
                            +   this.iStartOffset 
                            +   ptItem->Padding.chPrevious;
                    
            if (nStartX <= 0) {
                break;
            } 
            nOffset -= this.tCFG.nTotalLength;
        } while(true);

        int32_t nTempOffset = nOffset;

        /* find the centre most item */
        int32_t nCentreLocation = this.Runtime.tileList.tRegion.tSize.iWidth >> 1;
        int32_t nMinimalDistance = nCentreLocation;

        struct {
            int32_t nOffset;
            uint16_t hwID;
        } tTopMost, tBottomMost, tCentreMost = {
            .nOffset = 0,
            .hwID = 0,
        };

        while(NULL != ptItem) {
            int32_t nX1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
        
            int32_t nDistance = nCentreLocation - nX1;
            
            if (nDistance < 0) {
                /* cross the centre */
                break;
            }

            if (nDistance < nMinimalDistance) {
                nMinimalDistance = nDistance;
                tCentreMost.nOffset = nTempOffset;
                tCentreMost.hwID = ptItem->hwID;
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
                                true,
                                true);  /* force to use ring mode */
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
        }

        /* with centre most item, we start to find the bottom most item*/
        tBottomMost = tCentreMost;
        nTempOffset = tBottomMost.nOffset;

        /* move to the centre most item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    tCentreMost.hwID,
                    true,
                    false);
        assert(NULL != ptItem);

        do {
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
                    true,
                    false);

            if (NULL == ptItem) {
                /* end of the list */
                break;
            }
            
            int32_t nX1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
            
            if (nX1 >= this.Runtime.tileList.tRegion.tSize.iWidth) {
                /* this item is not visible */
                break;
            }

            tBottomMost.nOffset = nTempOffset;
            tBottomMost.hwID = ptItem->hwID;

        } while(true);
        this.CalMidAligned.iBottomVisibleOffset = (int16_t)tBottomMost.nOffset;
        this.CalMidAligned.hwBottomVisibleItemID = tBottomMost.hwID;

        /* move to the centre most item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    tCentreMost.hwID,
                    true,
                    false);
        assert(NULL != ptItem);

        /* with centre most item, we start to find the top most item*/
        tTopMost = tCentreMost;
        nTempOffset = tTopMost.nOffset;
        do {
            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_PREVIOUS,
                    0,
                    true,
                    false);

            if (NULL == ptItem) {
                /* end of the list */
                break;
            }

            /* update nTempOffset */
            nTempOffset -= ptItem->tSize.iWidth 
                        + ptItem->Padding.chPrevious
                        + ptItem->Padding.chNext;

            int32_t nX1 = nTempOffset 
                        + this.iStartOffset 
                        + ptItem->Padding.chPrevious;
            int32_t nX2 = nX1 + ptItem->tSize.iWidth - 1;

            if (nX2 <= 0) {
                /* this item is not visible */
                break;
            }

            tTopMost.nOffset = nTempOffset;
            tTopMost.hwID = ptItem->hwID;
        } while(true);
        this.CalMidAligned.iTopVisibleOffset = (int16_t)tTopMost.nOffset;
        this.CalMidAligned.hwTopVisibleItemID = tTopMost.hwID;

    } else {

        /* find the first and last visible items */

        /* move to the first item */
        ptItem = __arm_2d_list_core_get_item(   
                            ptThis, 
                            fnIterator, 
                            __ARM_2D_LIST_GET_FIRST_ITEM,
                            0,
                            true,
                            false);
        if (NULL == ptItem) {
            /* no valid item, return NULL */
            ARM_PT_RETURN(NULL)
        }
        
        if (!this.tCFG.bDisableRingMode) {
            /* move the nOffset to the top invisible area */
            do {
                int32_t nStartX = nOffset 
                                +   this.iStartOffset 
                                +   ptItem->Padding.chPrevious;
                        
                if (nStartX <= 0) {
                    break;
                } 
                nOffset -= this.tCFG.nTotalLength;
            } while(true);
        }

        int32_t nTempOffset = nOffset;
        
        do {
            int32_t nCount = 0;

            if (nTempOffset < 0) {
                int16_t iItemActualWidth = ptItem->tSize.iWidth 
                         + ptItem->Padding.chPrevious
                         + ptItem->Padding.chNext;
            
                int32_t nLength = ABS(nTempOffset) - this.iStartOffset;
                nCount = nLength / iItemActualWidth;
                nTempOffset += nCount * iItemActualWidth;

                do {
                    int32_t nX1 = nTempOffset 
                                    +   this.iStartOffset 
                                    +   ptItem->Padding.chPrevious;
                    int32_t nX2 = nX1 + ptItem->tSize.iWidth - 1;
                    
                    if (nX1 >= 0 || nX2 >=0) {
                        /* we find the top item */
                        break;
                    }
                    nTempOffset += iItemActualWidth;
                    nCount++;
                } while(true);
            } /* else {
                assert(this.tCFG.bDisableRingMode);
            } */
            
            uint16_t hwTempID = (ptItem->hwID + nCount) % this.tCFG.hwItemCount;
            
            /* move to the next */
            ptItem = __arm_2d_list_core_get_item(   
                                ptThis, 
                                fnIterator, 
                                __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                                hwTempID,
                                true,
                                false);
            if (NULL == ptItem) {
                /* no valid item, return NULL */
                ARM_PT_RETURN(NULL)
            }
            
        } while(0);

        /* mark the first visible item on top */
        this.CalMidAligned.iTopVisibleOffset = (int16_t)nTempOffset;
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
                    true,
                    false);

            if (NULL == ptItem) {
                if (this.tCFG.bDisableRingMode) {
                    break;
                } else {
                    /* no valid item, return NULL */
                    ARM_PT_RETURN(NULL)
                }
            }
            
            int32_t nX1 = nTempOffset 
                        +   this.iStartOffset 
                        +   ptItem->Padding.chPrevious;
            
            if (nX1 >= this.Runtime.tileList.tRegion.tSize.iWidth) {
                /* this item is not visible */
                break;
            }
        } while(true);
    }


    /* start draw items */
    do {
        /* move to the top item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwTopVisibleItemID,
                    true,
                    false);

        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX 
            = this.CalMidAligned.iTopVisibleOffset 
            + this.iStartOffset 
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
            <=  this.CalMidAligned.iTopVisibleOffset) {
            break;
        }

        /* resume local context */
        ptItem = this.Runtime.tWorkingArea.ptItem;

        /* update top visiable item id and offset */
        do {
            this.CalMidAligned.iTopVisibleOffset += ptItem->tSize.iWidth 
                                                  + ptItem->Padding.chPrevious
                                                  + ptItem->Padding.chNext;
            
            /* move to the top item */
            ptItem = __arm_2d_list_core_get_item(   
                        ptThis, 
                        fnIterator, 
                        __ARM_2D_LIST_GET_NEXT,
                        0,
                        true,
                        false);

            this.CalMidAligned.hwTopVisibleItemID = ptItem->hwID;
        } while(0);

        /* move to the bottom item */
        ptItem = __arm_2d_list_core_get_item(   
                    ptThis, 
                    fnIterator, 
                    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
                    this.CalMidAligned.hwBottomVisibleItemID,
                    true,
                    false);
        assert(NULL != ptItem);
        
        /* prepare working area */
        this.Runtime.tWorkingArea.ptItem = ptItem;
        this.Runtime.tWorkingArea.tRegion.tSize = ptItem->tSize;
        this.Runtime.tWorkingArea.tRegion.tLocation.iX 
            = this.CalMidAligned.iBottomVisibleOffset 
            + this.iStartOffset 
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
            <=  this.CalMidAligned.iTopVisibleOffset) {
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
                        true,
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

static
__arm_2d_list_work_area_t *
__calculator_mid_aligned_horizontal (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset
                            )
{
    return __calculator_horizontal(ptThis, fnIterator, nOffset, true);
}

arm_2d_i_list_region_calculator_t
ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_HORIZONTAL = {
    .fnCalculator = 
        &__calculator_mid_aligned_horizontal,
    .fnSelectionCompensation = &__selection_compensation_mid_aligned,
    .tDirection = ARM_2D_LIST_HORIZONTAL,
};

static
__arm_2d_list_work_area_t *
__calculator_mid_aligned_vertical (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset)
{
    return __calculator_vertical(ptThis, fnIterator, nOffset, true);
}

arm_2d_i_list_region_calculator_t
ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_VERTICAL = {
    .fnCalculator = 
        &__calculator_mid_aligned_vertical,
    .fnSelectionCompensation = &__selection_compensation_mid_aligned,
    .tDirection = ARM_2D_LIST_VERTICAL,
};

static
__arm_2d_list_work_area_t *
__calculator_mid_aligned_fixed_sized_item_no_status_checking_vertical (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset)
{
    return __calculator_fixed_sized_item_no_status_checking_vertical(
                                                                ptThis,
                                                                fnIterator,
                                                                nOffset,
                                                                true);
}

arm_2d_i_list_region_calculator_t
ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_FIXED_SIZED_ITEM_NO_STATUS_CHECK_VERTICAL = {
    .fnCalculator = 
        &__calculator_mid_aligned_fixed_sized_item_no_status_checking_vertical,
    .fnSelectionCompensation = &__selection_compensation_mid_aligned,
    .tDirection = ARM_2D_LIST_VERTICAL,
};

static
__arm_2d_list_work_area_t *
__calculator_mid_aligned_fixed_sized_item_no_status_checking_horizontal (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset)
{
    return __calculator_fixed_sized_item_no_status_checking_horizontal(
                                                                    ptThis,
                                                                    fnIterator,
                                                                    nOffset,
                                                                    true);
}

arm_2d_i_list_region_calculator_t
ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_FIXED_SIZED_ITEM_NO_STATUS_CHECK_HORIZONTAL = {
    .fnCalculator = 
        &__calculator_mid_aligned_fixed_sized_item_no_status_checking_horizontal,
    .fnSelectionCompensation = &__selection_compensation_mid_aligned,
    .tDirection = ARM_2D_LIST_HORIZONTAL,
};

static
__arm_2d_list_work_area_t * __calculator_normal_horizontal(
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset
                            )
{
    return __calculator_horizontal(ptThis, fnIterator, nOffset, false);
}

arm_2d_i_list_region_calculator_t
ARM_2D_LIST_CALCULATOR_NORMAL_HORIZONTAL = {
    .fnCalculator = 
        &__calculator_normal_horizontal,
    .tDirection = ARM_2D_LIST_HORIZONTAL,
};


static
__arm_2d_list_work_area_t * __calculator_normal_vertical (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset)
{
    return __calculator_vertical(ptThis, fnIterator, nOffset, false);
}

arm_2d_i_list_region_calculator_t
ARM_2D_LIST_CALCULATOR_NORMAL_VERTICAL = {
    .fnCalculator = 
        &__calculator_normal_vertical,
    .tDirection = ARM_2D_LIST_VERTICAL,
};

static
__arm_2d_list_work_area_t *
__calculator_normal_fixed_sized_item_no_status_checking_horizontal (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset)
{
    return __calculator_fixed_sized_item_no_status_checking_horizontal(
                                                                    ptThis,
                                                                    fnIterator,
                                                                    nOffset,
                                                                    false);
}

arm_2d_i_list_region_calculator_t
ARM_2D_LIST_CALCULATOR_NORMAL_FIXED_SIZED_ITEM_NO_STATUS_CHECK_HORIZONTAL = {
    .fnCalculator = 
        &__calculator_normal_fixed_sized_item_no_status_checking_horizontal,
    .tDirection = ARM_2D_LIST_HORIZONTAL,
};

static
__arm_2d_list_work_area_t *
__calculator_normal_fixed_sized_item_no_status_checking_vertical (
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset)
{
    return __calculator_fixed_sized_item_no_status_checking_vertical(
                                                                    ptThis,
                                                                    fnIterator,
                                                                    nOffset,
                                                                    false);
}

arm_2d_i_list_region_calculator_t
ARM_2D_LIST_CALCULATOR_NORMAL_FIXED_SIZED_ITEM_NO_STATUS_CHECK_VERTICAL = {
    .fnCalculator = 
        &__calculator_normal_fixed_sized_item_no_status_checking_vertical,
    .tDirection = ARM_2D_LIST_VERTICAL,
};


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

    if (this.tCFG.bDisableRingMode) {
        switch (tDirection) {
            default:
            case __ARM_2D_LIST_GET_ITEM_WITH_ID_WITHOUT_MOVE_POINTER:
                if (hwID >= this.tCFG.hwItemCount) {
                    /* out of range */
                    return NULL;
                }
                nIterationIndex = hwID;
                break;

            case __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER:
                if (hwID >= this.tCFG.hwItemCount) {
                    /* out of range */
                    return NULL;
                }
                this.Runtime.Iterator.Array.hwIndex = hwID;
                nIterationIndex = this.Runtime.Iterator.Array.hwIndex;
                break;

            case __ARM_2D_LIST_GET_PREVIOUS:
                if (this.Runtime.Iterator.Array.hwIndex) {
                    this.Runtime.Iterator.Array.hwIndex--;
                } else {
                    return NULL;
                }
                nIterationIndex = this.Runtime.Iterator.Array.hwIndex;
                break;

            case __ARM_2D_LIST_GET_NEXT:
                if ((this.Runtime.Iterator.Array.hwIndex + 1) >= this.tCFG.hwItemCount) {
                    return NULL;
                }
                this.Runtime.Iterator.Array.hwIndex++;
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
    } else {
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
