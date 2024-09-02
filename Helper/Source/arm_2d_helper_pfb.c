/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
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
 * Title:        #include "arm_2d_helper_pfb.c"
 * Description:  the pfb helper service source code
 *
 * $Date:        19. July 2024
 * $Revision:    V.1.11.3
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/
#define __ARM_2D_IMPL__

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "arm_2d_helper.h"
#include "__arm_2d_impl.h"

#if defined(__PERF_COUNTER__)
#   include "perf_counter.h"
#else
#   include <time.h>
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#   pragma clang diagnostic ignored "-Wundef"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wconditional-uninitialized"
#   pragma clang diagnostic ignored "-Wunreachable-code-return"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wunreachable-code"
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#   pragma clang diagnostic ignored "-Wswitch-default"
#   pragma clang diagnostic ignored "-Wunreachable-code-break"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress=1296,64,1,177,191
#endif

/*============================ MACROS ========================================*/

#undef  this
#define this            (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

ARM_NONNULL(1)
static void __arm_2d_helper_dirty_region_pool_free(
                                            arm_2d_helper_pfb_t *ptThis,
                                            arm_2d_region_list_item_t *ptItem);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * PFB Helper                                                                 *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1)
arm_2d_pfb_t *__arm_2d_helper_pfb_new(arm_2d_helper_pfb_t *ptThis)
{
    assert(NULL != ptThis);
    
    arm_2d_pfb_t *ptPFB = NULL;

    arm_irq_safe {
        if (this.Adapter.chFreePFBCount) {
            this.Adapter.chFreePFBCount--;
            ARM_LIST_STACK_POP(this.Adapter.ptFreeList, ptPFB);
        }
    }

    if (NULL != ptPFB) {
        ARM_2D_LOG_INFO(
            HELPER_PFB, 
            2, 
            "PFB Pool", 
            "Allocate a new PFB [%p], %d is left",
            ptPFB,
            this.Adapter.chFreePFBCount
        );

        ptPFB->ptPFBHelper = ptThis;
    } else {
         ARM_2D_LOG_WARNING(
            HELPER_PFB, 
            2, 
            "PFB Pool", 
            "Failed to allocate a PFB from pool, %d is left",
            this.Adapter.chFreePFBCount
        );
    }

    return ptPFB;
}

ARM_NONNULL(1)
void __arm_2d_helper_pfb_free(arm_2d_helper_pfb_t *ptThis, arm_2d_pfb_t *ptPFB)
{
    assert(NULL != ptThis);
    
    if (NULL == ptPFB) {
        return ;
    }

    arm_irq_safe {
        ARM_LIST_STACK_PUSH(this.Adapter.ptFreeList, ptPFB);
        this.Adapter.chFreePFBCount++;
    }
    
    /* set event */
    if  (   (NULL == this.tCFG.Dependency.evtOnLowLevelSyncUp.fnHandler)
        &&  ((uintptr_t)NULL != this.Adapter.pFPBPoolAvailable)) {
        arm_2d_port_set_semaphore(this.Adapter.pFPBPoolAvailable);
    }

    ARM_2D_LOG_INFO(
        HELPER_PFB, 
        2, 
        "PFB Pool", 
        "Free a PFB to the pool [%p], %d is left",
        ptPFB,
        this.Adapter.chFreePFBCount
    );
}

ARM_NONNULL(1,2)
arm_2d_err_t arm_2d_helper_pfb_init(arm_2d_helper_pfb_t *ptThis, 
                                    arm_2d_helper_pfb_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    
    arm_2d_helper_init();
    
    memset(ptThis, 0, sizeof(this));
    this.tCFG = *ptCFG;
    
    if (NULL == this.tCFG.Dependency.evtOnLowLevelRendering.fnHandler) {
        return ARM_2D_ERR_MISSING_PARAM;
    }
    
    this.Adapter.wPFBPixelCount 
        = this.tCFG.FrameBuffer.tFrameSize.iWidth
        * this.tCFG.FrameBuffer.tFrameSize.iHeight;

    /* make sure the width of the pfb fulfill the pixel-width-alignment 
     * requirement 
     */
    do {
        int_fast8_t chPixelWidthAlignMask 
                = (1 << this.tCFG.FrameBuffer.u3PixelWidthAlign) - 1;
        uint32_t wTotalPixelCount = this.Adapter.wPFBPixelCount;
        int_fast8_t chPixelHeightAlignMask 
                = (1 << this.tCFG.FrameBuffer.u3PixelHeightAlign) - 1;

        if ((0 == chPixelWidthAlignMask) && (0 == chPixelHeightAlignMask)) {
            break;
        }

        /* Align both Width and Height */
        if (chPixelWidthAlignMask && chPixelHeightAlignMask) {

            int_fast16_t iWidth 
                = this.tCFG.FrameBuffer.tFrameSize.iWidth & ~chPixelWidthAlignMask;
            int_fast16_t iHeight 
                = this.tCFG.FrameBuffer.tFrameSize.iHeight & ~chPixelHeightAlignMask;

            if (0 == iWidth) {
                int_fast16_t iWidthAlign = (chPixelWidthAlignMask + 1);
                if (wTotalPixelCount < iWidthAlign) {
                    return ARM_2D_ERR_INSUFFICIENT_RESOURCE;
                }
                iWidth = iWidthAlign;
            }

            if (0 == iHeight) {
                int_fast16_t iHeightAlign = (chPixelHeightAlignMask + 1);
                if (wTotalPixelCount < iHeightAlign) {
                    return ARM_2D_ERR_INSUFFICIENT_RESOURCE;
                }
                iHeight = iHeightAlign;
            }

            if (iWidth * iHeight > wTotalPixelCount) {
                return ARM_2D_ERR_INSUFFICIENT_RESOURCE;
            }

            /* update frame size */
            this.tCFG.FrameBuffer.tFrameSize.iWidth = iWidth;
            this.tCFG.FrameBuffer.tFrameSize.iHeight = iHeight;

        /* Align Width Only */
        } else if (chPixelWidthAlignMask) {

            int_fast16_t iWidth 
                = this.tCFG.FrameBuffer.tFrameSize.iWidth & ~chPixelWidthAlignMask;
            
            if (0 == iWidth) {
                int_fast16_t iWidthAlign = (chPixelWidthAlignMask + 1);
                if (wTotalPixelCount < iWidthAlign) {
                    return ARM_2D_ERR_INSUFFICIENT_RESOURCE;
                }
                iWidth = iWidthAlign;
            }

            /* update frame size */
            this.tCFG.FrameBuffer.tFrameSize.iWidth = iWidth;
            this.tCFG.FrameBuffer.tFrameSize.iHeight 
                = wTotalPixelCount 
                / this.tCFG.FrameBuffer.tFrameSize.iWidth;

        /* Align Height Only */
        } else if (chPixelHeightAlignMask) {
            
            int_fast16_t iHeight 
                = this.tCFG.FrameBuffer.tFrameSize.iHeight & ~chPixelHeightAlignMask;

            if (0 == iHeight) {
                int_fast16_t iHeightAlign = (chPixelHeightAlignMask + 1);
                if (wTotalPixelCount < iHeightAlign) {
                    return ARM_2D_ERR_INSUFFICIENT_RESOURCE;
                }
                iHeight = iHeightAlign;
            }

            /* update frame size */
            this.tCFG.FrameBuffer.tFrameSize.iHeight = iHeight;
            this.tCFG.FrameBuffer.tFrameSize.iWidth 
                = wTotalPixelCount 
                / this.tCFG.FrameBuffer.tFrameSize.iHeight;
        }
    } while(0);

    if (NULL == this.tCFG.Dependency.evtOnLowLevelSyncUp.fnHandler) {
        /* use default semaphore */
        this.Adapter.pFPBPoolAvailable = arm_2d_port_new_semaphore();
    }

    /* initialize internal dirty region pool*/
    do {
        uint_fast8_t chCount = this.tCFG.DirtyRegion.chCount;
        arm_2d_region_list_item_t *ptRegion = this.tCFG.DirtyRegion.ptRegions;
        if (    (0 == chCount) ||  (NULL == ptRegion)) {
            this.Adapter.bIsDirtyRegionOptimizationEnabled = false;
            break;
        }

        do {
            memset(ptRegion, 0, sizeof(arm_2d_region_list_item_t) );
            ptRegion->bFromInternalPool = true;

            __arm_2d_helper_dirty_region_pool_free(ptThis, ptRegion);
            ptRegion++;
        } while(--chCount);

        this.Adapter.bIsDirtyRegionOptimizationEnabled = true;
    } while(0);

    // perform validation
    do {
        uint_fast8_t n = this.tCFG.FrameBuffer.u8PFBNum;
        arm_2d_pfb_t *ptItem = this.tCFG.FrameBuffer.ptPFBs;
        uint32_t wBufferSize = this.tCFG.FrameBuffer.u24BufferSize;
        arm_2d_color_info_t tColourFormat = {
            .u7ColourFormat = this.tCFG.FrameBuffer.u7ColourFormat,
        };

        // handle alignments
        wBufferSize += __alignof__(arm_2d_pfb_t) - 1;
        wBufferSize &= ~(__alignof__(arm_2d_pfb_t) - 1);
        
        if (0 == n || NULL == ptItem) {
            return ARM_2D_ERR_MISSING_PARAM;
        } else if ( (0 == this.tCFG.FrameBuffer.tFrameSize.iHeight)
                 || (0 == this.tCFG.FrameBuffer.tFrameSize.iWidth)
                 || (0 == wBufferSize)) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
        
        this.Adapter.chFreePFBCount = 0;
        // add PFBs to pool
        do {
            ptItem->tTile = (arm_2d_tile_t) {
                .tRegion = {
                    .tSize = this.tCFG.FrameBuffer.tFrameSize,
                },
                .tInfo.bIsRoot = true,
                .pchBuffer = (uint8_t *)((uintptr_t)ptItem + sizeof(arm_2d_pfb_t)),
            };
            
            // update buffer size
            ptItem->u24Size = wBufferSize;

            // save colour format
            ptItem->u7ColourFormat = tColourFormat.u7ColourFormat;

            __arm_2d_helper_pfb_free(ptThis, ptItem);
            
            // update pointer
            ptItem = (arm_2d_pfb_t *)(  (uintptr_t)ptItem 
                                     +  wBufferSize
                                     +  sizeof(arm_2d_pfb_t));
        } while(--n);
    
    } while(0);

    this.Adapter.bFirstIteration = true;
    
    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
void arm_2d_helper_pfb_deinit(arm_2d_helper_pfb_t *ptThis) 
{
    assert(NULL != ptThis);
    arm_2d_port_free_semaphore(this.Adapter.pFPBPoolAvailable);
}

ARM_NONNULL(1)
arm_2d_region_t arm_2d_helper_pfb_get_display_area(arm_2d_helper_pfb_t *ptThis)
{
    assert(NULL != ptThis);
    
    return this.tCFG.tDisplayArea;
}

ARM_NONNULL(1)
arm_2d_size_t arm_2d_helper_pfb_get_pfb_size(arm_2d_helper_pfb_t *ptThis)
{
    assert(NULL != ptThis);
    
    return this.tCFG.FrameBuffer.tFrameSize;
}

ARM_NONNULL(1)
arm_2d_location_t arm_2d_helper_pfb_get_absolute_location(  
                                                    arm_2d_tile_t *ptTile, 
                                                    arm_2d_location_t tLocation)
{
    assert(NULL != ptTile);
    
    return arm_2d_get_absolute_location(ptTile, tLocation, true);
}

ARM_NONNULL(1)
void arm_2d_helper_ignore_low_level_flush(arm_2d_helper_pfb_t *ptThis)
{
    assert(NULL != ptThis);
    this.Adapter.bIgnoreLowLevelFlush = true;
}

ARM_NONNULL(1)
void arm_2d_helper_resume_low_level_flush(arm_2d_helper_pfb_t *ptThis)
{
    assert(NULL != ptThis);
    this.Adapter.bIgnoreLowLevelFlush = false;
}


ARM_NONNULL(1)
void arm_2d_helper_hide_navigation_layer(arm_2d_helper_pfb_t *ptThis)
{
    assert(NULL != ptThis);
    this.Adapter.bHideNavigationLayer = true;
}

ARM_NONNULL(1)
void arm_2d_helper_show_navigation_layer(arm_2d_helper_pfb_t *ptThis)
{
    assert(NULL != ptThis);
    this.Adapter.bHideNavigationLayer = false;
}

ARM_NONNULL(1)
void __arm_2d_helper_pfb_enable_drawing_canvas_colour(arm_2d_helper_pfb_t *ptThis,
                                                      arm_2d_colour_t tColour)
{
    assert(NULL != ptThis);

    arm_irq_safe {
        this.Adapter.bIgnoreCanvasColour = false;
        this.Adapter.tCanvas = tColour;
    }
}

ARM_NONNULL(1)
void __arm_2d_helper_pfb_disable_drawing_canvas_colour(arm_2d_helper_pfb_t *ptThis)
{
    assert(NULL != ptThis);
    this.Adapter.bIgnoreCanvasColour = true;
}

ARM_NONNULL(1)
void arm_2d_helper_pfb_flush(arm_2d_helper_pfb_t *ptThis)
{
    assert(NULL != ptThis);

    arm_2d_pfb_t *ptPFB = NULL;
    
    /* atomic access of this.Adapter.pfFlushing */
    arm_irq_safe {
        if (NULL != this.Adapter.ptFlushing) {
            arm_exit_irq_safe;
        }

        ARM_LIST_QUEUE_DEQUEUE( this.Adapter.FlushFIFO.ptHead, 
                                this.Adapter.FlushFIFO.ptTail, 
                                ptPFB);

        if (NULL != ptPFB) {
            this.Adapter.ptFlushing = ptPFB;
            ptPFB->ptPFBHelper = ptThis;
        }
    }

    if (NULL != ptPFB) {
        if (    (NULL == this.tCFG.Dependency.evtOnLowLevelRendering.fnHandler)
           ||   this.Adapter.bIgnoreLowLevelFlush) {
           __arm_2d_helper_pfb_report_rendering_complete(ptThis, ptPFB);
        } else {

            ARM_2D_LOG_INFO(
                HELPER_PFB, 
                0, 
                "FLUSH_PFB", 
                "Request Flushing PFB [%p] , x=%d y=%d w=%d h=%d",
                (void *)ptPFB, 
                ptPFB->tTile.tRegion.tLocation.iX,
                ptPFB->tTile.tRegion.tLocation.iY,
                ptPFB->tTile.tRegion.tSize.iWidth,
                ptPFB->tTile.tRegion.tSize.iHeight
            );


            // call handler
            (*this.tCFG.Dependency.evtOnLowLevelRendering.fnHandler)(
                            this.tCFG.Dependency.evtOnLowLevelRendering.pTarget,
                            ptPFB,
                            ptPFB->bIsNewFrame);
        }
    }
}

static 
void __arm_2d_helper_enqueue_pfb(arm_2d_helper_pfb_t *ptThis)
{
    arm_irq_safe {
        ARM_LIST_QUEUE_ENQUEUE( this.Adapter.FlushFIFO.ptHead, 
                                this.Adapter.FlushFIFO.ptTail, 
                                this.Adapter.ptCurrent);
    }
    
    arm_2d_helper_pfb_flush(ptThis);
}

ARM_NONNULL(1)
void __arm_2d_helper_pfb_report_rendering_complete( arm_2d_helper_pfb_t *ptThis,
                                                    arm_2d_pfb_t *ptPFB)
{
    assert(NULL != ptThis);

    /* note: in fact, user can only pass either NULL or this.Adapter.ptFlushing
     *       to the ptPFB. This makes ptPFB useless. We only keep it for 
     *       backward compatible.
     */

    arm_irq_safe {
        if (NULL == ptPFB) {
            ptPFB = this.Adapter.ptFlushing;
            this.Adapter.ptFlushing = NULL;
        } else if (ptPFB == this.Adapter.ptFlushing) {
            this.Adapter.ptFlushing = NULL;
        } else {
            /* this should not happen */
            assert(false);
        }
    }

    if (NULL == ptPFB) {
        assert(false);  /* this should not happen */
        return ;
    }

    ARM_2D_LOG_INFO(
        HELPER_PFB, 
        0, 
        "REPORT_RENDERING_COMPLETE", 
        "Flushing PFB [%p] is complete, x=%d y=%d w=%d h=%d",
        (void *)ptPFB, 
        ptPFB->tTile.tRegion.tLocation.iX,
        ptPFB->tTile.tRegion.tLocation.iY,
        ptPFB->tTile.tRegion.tSize.iWidth,
        ptPFB->tTile.tRegion.tSize.iHeight
    );

    ptPFB->tTile.tRegion.tLocation = (arm_2d_location_t) {0,0};
    
    __arm_2d_helper_pfb_free(ptThis, ptPFB);
    
    arm_2d_helper_pfb_flush(ptThis);
}

static
void __arm_2d_helper_low_level_rendering(arm_2d_helper_pfb_t *ptThis)
{

    assert(NULL != this.tCFG.Dependency.evtOnLowLevelRendering.fnHandler);
    assert(NULL != this.Adapter.ptCurrent);

    // update location info
    this.Adapter.ptCurrent->tTile.tRegion.tLocation = (arm_2d_location_t) {
        .iX = this.Adapter.tScanOffset.iX
            + this.Adapter.tTargetRegion.tLocation.iX,
        .iY = this.Adapter.tScanOffset.iY
            + this.Adapter.tTargetRegion.tLocation.iY,
    };

    
    arm_2d_color_info_t tColourFormat = {
        .u7ColourFormat = this.tCFG.FrameBuffer.u7ColourFormat,
    };

    /* swap the low and high byte for RGB565 and RGB16 */
    if ((this.tCFG.FrameBuffer.bSwapRGB16)
    &&  (tColourFormat.u3ColourSZ == ARM_2D_M_COLOUR_SZ_16BIT)) {
        arm_2d_helper_swap_rgb16( this.Adapter.ptCurrent->tTile.phwBuffer, 
                                    get_tile_buffer_pixel_count(
                                        this.Adapter.ptCurrent->tTile));
    }

    /* handle screen rotation event */
    if (NULL != this.tCFG.Dependency.evtBeforeFlushing.fnHandler) {
        /* get a scratch PFB */
        arm_2d_pfb_t *ptScratchPFB 
            = __arm_2d_helper_pfb_new(ptThis);

        assert(NULL != ptScratchPFB);
        if (this.tCFG.Dependency.evtBeforeFlushing.fnHandler(
                                    this.tCFG.Dependency.evtBeforeFlushing.pTarget,
                                    this.Adapter.ptCurrent, 
                                    ptScratchPFB)) {
            /* the new content is stored in the ptScratchPFB */
            /* free the old PFB */
            __arm_2d_helper_pfb_free(ptThis,this.Adapter.ptCurrent);
            /* points to the new output */
            this.Adapter.ptCurrent = ptScratchPFB;
        } else {
             /* free the scratch PFB */
            __arm_2d_helper_pfb_free(ptThis, ptScratchPFB);
        }
    }

    __arm_2d_helper_enqueue_pfb(ptThis);

    this.Adapter.bFirstIteration = false;

}


__STATIC_INLINE 
bool __when_dirty_region_list_is_empty(arm_2d_helper_pfb_t *ptThis)
{
    /* check whether has already been switched to the navigation dirty 
     * region list 
     */
    if (this.Adapter.bNoAdditionalDirtyRegionList) {
        // no dirty region is available
        this.Adapter.bFirstIteration = true;
        
        ARM_2D_LOG_INFO(
            HELPER_PFB, 
            2, 
            "Get Next Dirty Region", 
            "No more dirty regions."
        );

        return false;
    } else if ( (NULL != this.tCFG.Dependency.Navigation.evtOnDrawing.fnHandler)
           &&   (NULL != this.tCFG.Dependency.Navigation.ptDirtyRegion)
           &&   (!this.Adapter.bHideNavigationLayer)) {
        
        /* switch to navigation dirty region list */
        this.Adapter.ptDirtyRegion = this.tCFG.Dependency.Navigation.ptDirtyRegion;
        this.Adapter.bNoAdditionalDirtyRegionList = true;
        
        this.Adapter.bIsRegionChanged = true;

        ARM_2D_LOG_INFO(
            HELPER_PFB, 
            2, 
            "Get Next Dirty Region", 
            "Get dirty region item from the navigation list [%p]",
            this.Adapter.ptDirtyRegion
        );

        return true;
    } else {
        // no dirty region is available
        this.Adapter.bFirstIteration = true;
        
        ARM_2D_LOG_INFO(
            HELPER_PFB, 
            2, 
            "Get Next Dirty Region", 
            "No more dirty regions."
        );

        return false;
    }
}

static bool __arm_2d_helper_pfb_get_next_dirty_region(arm_2d_helper_pfb_t *ptThis)
{

    if (this.Adapter.bIsUsingOptimizedDirtyRegionList) {
        

        if (NULL == this.Adapter.ptDirtyRegion->ptInternalNext) {
            /* finished or empty */
            this.Adapter.bIsUsingOptimizedDirtyRegionList = false;
        #if 0
            this.Adapter.ptDirtyRegion = this.Adapter.OptimizedDirtyRegions.ptOriginalList;

            ARM_2D_LOG_INFO(
                HELPER_PFB, 
                2, 
                "Get Next Dirty Region", 
                "Reach the end of the optimized working list, use the original dirty region list [%p]",
                this.Adapter.ptDirtyRegion
            );

            if (NULL == this.Adapter.ptDirtyRegion) {
                // no dirty region is available
                this.Adapter.bFirstIteration = true;
                
                return false;
            } else {
                this.Adapter.bIsRegionChanged = true;
            }

            return true;
        #else
            this.Adapter.ptDirtyRegion = NULL;
            // no dirty region is available
            this.Adapter.bFirstIteration = true;

            ARM_2D_LOG_INFO(
                HELPER_PFB, 
                2, 
                "Get Next Dirty Region", 
                "Reach the end of the optimized working list.",
                this.Adapter.ptDirtyRegion
            );

            return false;
        #endif
        } else {
            this.Adapter.ptDirtyRegion
                = this.Adapter.ptDirtyRegion->ptInternalNext;

            this.Adapter.bIsRegionChanged = true;

            ARM_2D_LOG_INFO(
                HELPER_PFB, 
                2, 
                "Get Next Dirty Region", 
                "Get a dirty region[%p] from the optimized working list, x=%d y=%d w=%d h=%d",
                this.Adapter.ptDirtyRegion,
                this.Adapter.ptDirtyRegion->tRegion.tLocation.iX,
                this.Adapter.ptDirtyRegion->tRegion.tLocation.iY,
                this.Adapter.ptDirtyRegion->tRegion.tSize.iWidth,
                this.Adapter.ptDirtyRegion->tRegion.tSize.iHeight
            );

            return true;
        }
    }

    if (NULL == this.Adapter.ptDirtyRegion) {
        return __when_dirty_region_list_is_empty(ptThis);
    } 
    
    if (this.Adapter.ptDirtyRegion->bUpdated) {
        /* refresh this dirty region item again */
        this.Adapter.bIsRegionChanged = true;

        ARM_2D_LOG_INFO(
            HELPER_PFB, 
            2, 
            "Get Next Dirty Region", 
            "Get a dynamic dirty region item [%p] in the normal way",
            this.Adapter.ptDirtyRegion
        );

    } else {
        this.Adapter.ptDirtyRegion = this.Adapter.ptDirtyRegion->ptNext;
        
        if (NULL == this.Adapter.ptDirtyRegion) {
            // reach last item in a chain
            return __when_dirty_region_list_is_empty(ptThis);
        } else {
            this.Adapter.bIsRegionChanged = true;
        }

        ARM_2D_LOG_INFO(
            HELPER_PFB, 
            2, 
            "Get Next Dirty Region", 
            "Get a dirty region item [%p] in the normal way",
            this.Adapter.ptDirtyRegion
        );

    }


    
    return true;
}

/*----------------------------------------------------------------------------*
 * Optimized Dirty Regions                                                    *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1)
void arm_2d_helper_pfb_enable_dirty_region_optimization(
                                        arm_2d_helper_pfb_t *ptThis,
                                        arm_2d_region_list_item_t *ptRegions,
                                        uint_fast8_t chCount)
{
    assert(NULL != ptThis);
    arm_irq_safe {
        this.Adapter.bEnableDirtyRegionOptimizationRequest = true;
        this.Adapter.bDisableDirtyRegionOptimizationRequest = false;
    }

    if ((0 == chCount) || (NULL == ptRegions)) {
        return ;
    }

    do {
        memset(ptRegions, 0, sizeof(arm_2d_region_list_item_t) );
        ptRegions->bFromInternalPool = true;

        __arm_2d_helper_dirty_region_pool_free(ptThis, ptRegions);
        ptRegions++;
    } while(--chCount);
}

ARM_NONNULL(1)
void arm_2d_helper_pfb_disable_dirty_region_optimization(
                                                arm_2d_helper_pfb_t *ptThis)
{
    assert(NULL != ptThis);

    arm_irq_safe {
        this.Adapter.bEnableDirtyRegionOptimizationRequest = false;
        this.Adapter.bDisableDirtyRegionOptimizationRequest = true;
    }
}


ARM_NONNULL(1)
static void __arm_2d_helper_dirty_region_pool_free(
                                            arm_2d_helper_pfb_t *ptThis,
                                            arm_2d_region_list_item_t *ptItem)
{
    assert(NULL != ptThis);
    if (NULL == ptItem) {
        return ;
    } else if (!ptItem->bFromInternalPool) {
        return ;
    }

    if (ptItem->bFromHeap) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, ptItem);

        ARM_2D_LOG_INFO(DIRTY_REGION_OPTIMISATION, 
                    2, 
                    "Dirty Region Pool", 
                    "Free Dirty Region Item [%p] to HEAP", 
                    (void *)ptItem);
    } else {

        arm_irq_safe {
            /* PUSH item to the pool in STACK-style */
            ptItem->ptInternalNext = this.Adapter.OptimizedDirtyRegions.ptFreeList;
            this.Adapter.OptimizedDirtyRegions.ptFreeList = ptItem;
            this.Adapter.OptimizedDirtyRegions.iFreeCount++;
            assert(this.Adapter.OptimizedDirtyRegions.iFreeCount > 0);
        }

        ARM_2D_LOG_INFO(DIRTY_REGION_OPTIMISATION, 
                    2, 
                    "Dirty Region Pool", 
                    "Free Dirty Region Item [%p], %d items available", 
                    (void *)ptItem,
                    this.Adapter.OptimizedDirtyRegions.iFreeCount);
    }
}

ARM_NONNULL(1)
static 
arm_2d_region_list_item_t *__arm_2d_helper_dirty_region_pool_new(
                                            arm_2d_helper_pfb_t *ptThis)
{
    assert(NULL != ptThis);
    arm_2d_region_list_item_t *ptItem = NULL;
    bool bFromHeap = false;
    arm_irq_safe {
        ptItem = this.Adapter.OptimizedDirtyRegions.ptFreeList;
        if (NULL != ptItem) {
            /* POP a dirty region item from the free list in STACK-style */
            this.Adapter.OptimizedDirtyRegions.ptFreeList = ptItem->ptInternalNext;
            this.Adapter.OptimizedDirtyRegions.iFreeCount--;
            assert(this.Adapter.OptimizedDirtyRegions.iFreeCount >= 0);
        }
    }

    if (NULL == ptItem) {
        /* try to borrow items from the heap */
        ptItem = (arm_2d_region_list_item_t *)
                    __arm_2d_allocate_scratch_memory(
                        sizeof(arm_2d_region_list_item_t),
                        __alignof__(arm_2d_region_list_item_t),
                        ARM_2D_MEM_TYPE_FAST);
        bFromHeap = true;
    }

    if (NULL != ptItem) {
        memset(ptItem, 0, sizeof(arm_2d_region_list_item_t));
        ptItem->bFromInternalPool = true;
        ptItem->bFromHeap = bFromHeap;

        if (bFromHeap) {
            ARM_2D_LOG_INFO(
                DIRTY_REGION_OPTIMISATION, 
                2, 
                "Dirty Region Pool", 
                "Allocate a new dirty region item [%p] from HEAP as POOL is empty", 
                (void *)ptItem);
        } else {
            ARM_2D_LOG_INFO(
                DIRTY_REGION_OPTIMISATION, 
                2, 
                "Dirty Region Pool", 
                "Allocate a new dirty region item [%p], %d item left", 
                (void *)ptItem,
                this.Adapter.OptimizedDirtyRegions.iFreeCount);
        }
    } else {
        ARM_2D_LOG_INFO(
            DIRTY_REGION_OPTIMISATION, 
            2, 
            "Dirty Region Pool", 
            "Failed to allocate dirty region item", 
            (void *)ptItem);
    }

    return ptItem;
}

ARM_NONNULL(1)
static 
void __arm_2d_helper_free_dirty_region_working_list(arm_2d_helper_pfb_t *ptThis, 
                                                    arm_2d_region_list_item_t *ptList)
{
    assert(NULL != ptThis);

    /* free the optimized dirty region list*/
    if (NULL != ptList) {
        
        do {
            arm_2d_region_list_item_t *ptNextItem = ptList->ptInternalNext;
            __arm_2d_helper_dirty_region_pool_free(ptThis, ptList);

            ptList = ptNextItem;
        } while(NULL != ptList);

        this.Adapter.OptimizedDirtyRegions.ptWorkingList = NULL;
    }
}

ARM_NONNULL(1,2)
static 
void __arm_2d_helper_add_item_to_weighted_dirty_region_list(  
                                                arm_2d_region_list_item_t **pptList,
                                                arm_2d_region_list_item_t *ptItem)
{
    assert(NULL != pptList);
    assert(NULL != ptItem);
    arm_2d_region_list_item_t **pptListOrigin = pptList;
    ARM_2D_UNUSED(pptListOrigin);

    if (NULL != *pptList) {
        uint32_t wItemPixelCount = ptItem->tRegion.tSize.iHeight 
                                 * ptItem->tRegion.tSize.iWidth;

        do {
            arm_2d_region_list_item_t *ptItemInList = (*pptList);
            uint32_t wPixelCount = ptItemInList->tRegion.tSize.iHeight 
                                * ptItemInList->tRegion.tSize.iWidth;
            
            if (wPixelCount <= wItemPixelCount) {
                /* add item to the list */
                ptItem->ptInternalNext = (*pptList);
                *pptList = ptItem;

                ARM_2D_LOG_INFO(
                    DIRTY_REGION_OPTIMISATION, 
                    3, 
                    "ADD_ITEM_TO_DIRTY_REGION_LIST", 
                    "Insert item [%p] to the list [%p]", 
                    (void *)ptItem,
                    (void *)pptListOrigin
                );

                return;
            }

            pptList = &(ptItemInList->ptInternalNext);
        } while(NULL != (*pptList));
    }

    ARM_2D_LOG_INFO(
        DIRTY_REGION_OPTIMISATION, 
        3, 
        "ADD_ITEM_TO_DIRTY_REGION_LIST", 
        "Append item [%p] to the list [%p]", 
        (void *)ptItem,
        (void *)pptListOrigin
    );

    /* add item to the list */
    ptItem->ptInternalNext = (*pptList);
    *pptList = ptItem;
}

ARM_NONNULL(1,2)
static 
arm_2d_region_list_item_t * __arm_2d_helper_remove_item_from_weighted_dirty_region_list(  
                                                arm_2d_region_list_item_t **pptList,
                                                arm_2d_region_list_item_t *ptItem)
{
    assert(NULL != pptList);
    assert(NULL != ptItem);
    arm_2d_region_list_item_t **pptListOrigin = pptList;
    ARM_2D_UNUSED(pptListOrigin);

    bool bFindItem = false;

    while(NULL != (*pptList)) {

        /* find item */
        if ((*pptList) == ptItem) {
            (*pptList) = ptItem->ptInternalNext;
            ptItem->ptInternalNext = NULL;

            ARM_2D_LOG_INFO(
                DIRTY_REGION_OPTIMISATION, 
                3, 
                "REMOTE_ITEM_FROM_DIRTY_REGION_LIST", 
                "Remove item [%p] from the list [%p]", 
                (void *)ptItem,
                (void *)pptListOrigin
            );

            bFindItem = true;
            break ;
        }

        pptList = &((*pptList)->ptInternalNext);
    }

    if (!bFindItem) {
        ARM_2D_LOG_INFO(
            DIRTY_REGION_OPTIMISATION, 
            3, 
            "REMOTE_ITEM_FROM_DIRTY_REGION_LIST", 
            "Cannot find the item [%p] from the list [%p]", 
            (void *)ptItem,
            (void *)pptListOrigin
        );
    }

    return ptItem;
}

ARM_NONNULL(1,2,3)
static 
arm_2d_region_t * __arm_2d_remove_overlapped_region_vertically (
                                         const arm_2d_region_t *ptOriginal,
                                         const arm_2d_region_t *ptOverlap,
                                         arm_2d_region_t *ptResidual)
{
    assert(NULL != ptOriginal);
    assert(NULL != ptOverlap);
    assert(NULL != ptResidual);

    *ptResidual = *ptOriginal;

    if (ptOriginal->tLocation.iY == ptOverlap->tLocation.iY) {
        /*
         *
         *    +----------------------------+
         *    |                            |
         *    |  Original                  |
         *    |  +------------------+      |
         *    |  |//////Overlap/////|      |
         *    +--+------------------+------+
         *       |                  |
         *       |                  |
         *       +------------------+
         */

        ptResidual->tLocation.iY += ptOverlap->tSize.iHeight;
        ptResidual->tSize.iHeight -= ptOverlap->tSize.iHeight;
    } else {
        /*       Original
         *       +------------------+
         *       |                  |
         *    +--+------------------+------+
         *    |  |//////Overlap/////|      |
         *    |  |//////////////////|      |
         *    |  +------------------+      |
         *    |                            |
         *    +----------------------------+
         */
        ptResidual->tSize.iHeight -= ptOverlap->tSize.iHeight;
    }

    return ptResidual;
}



ARM_NONNULL(1,2,3)
static 
arm_2d_region_t * __arm_2d_remove_overlapped_region_horizontally (
                                         const arm_2d_region_t *ptOriginal,
                                         const arm_2d_region_t *ptOverlap,
                                         arm_2d_region_t *ptResidual)
{
    assert(NULL != ptOriginal);
    assert(NULL != ptOverlap);
    assert(NULL != ptResidual);

    *ptResidual = *ptOriginal;

    if (ptOriginal->tLocation.iX == ptOverlap->tLocation.iX) {
        /*
         *
         *     +-------------------+
         *     |     Original      | 
         *     |     +-------------+-------+
         *     |     |/////////////|       |
         *     |     |/////Overlap/|       |
         *     |     +-------------+-------+
         *     |                   |
         *     +-------------------+
         */

        ptResidual->tLocation.iX += ptOverlap->tSize.iWidth;
        ptResidual->tSize.iWidth -= ptOverlap->tSize.iWidth;
    } else {
        /*
         *
         *         +-------------------+
         * Original|                   | 
         *   +-----+-----------+       |
         *   |     |///////////|       |
         *   |     |///Overlap/|       |
         *   +-----+-----------+       |
         *         |                   |
         *         +-------------------+
         */

        ptResidual->tSize.iWidth -= ptOverlap->tSize.iWidth;
    }

    return ptResidual;
}


ARM_NONNULL(1,2)
static 
void __arm_2d_helper_update_dirty_region_working_list(  
                                                arm_2d_helper_pfb_t *ptThis, 
                                                arm_2d_region_list_item_t *ptItem,
                                                bool bEncounterDynamicDirtyRegion)
{
    assert(NULL != ptThis);
    assert(NULL != ptItem);

    do {
        arm_2d_region_list_item_t *ptDirtyRegion = ptItem;

        if (bEncounterDynamicDirtyRegion) {

            ARM_2D_LOG_INFO(
                DIRTY_REGION_OPTIMISATION, 
                1, 
                "UPDATE_WORKING_LIST", 
                "Encounter Dynamic Dirty Region [%p]", 
                (void *)ptItem
            );

            ptDirtyRegion = __arm_2d_helper_dirty_region_pool_new(ptThis);
            if (NULL == ptDirtyRegion) {
                this.Adapter.bFailedToOptimizeDirtyRegion = true;

                ARM_2D_LOG_WARNING(
                    DIRTY_REGION_OPTIMISATION, 
                    1, 
                    "UPDATE_WORKING_LIST", 
                    "Failed to allocate dirty region item for dynamic dirty region [%p]", 
                    (void *)ptItem
                );

            } else {
                ptDirtyRegion->tRegion = ptItem->tRegion;
            }
        }

        if (this.Adapter.bFailedToOptimizeDirtyRegion) {
            __arm_2d_helper_dirty_region_pool_free(ptThis, ptDirtyRegion);
            return ;
        }

        /* add dirty region to the candidate list */
        __arm_2d_helper_add_item_to_weighted_dirty_region_list(
            &this.Adapter.OptimizedDirtyRegions.ptCandidateList,
            ptDirtyRegion
        );
    } while(0);

label_start_process_candidate:
    /* the process loop */
    do {
        arm_2d_region_list_item_t *ptCandidate = NULL;

label_move_to_next_candidate:
        do {
            ptCandidate 
                = this.Adapter.OptimizedDirtyRegions.ptCandidateList;
            
            if (NULL == ptCandidate) {
                break;
            }

            arm_2d_region_list_item_t *ptWorking
                = this.Adapter.OptimizedDirtyRegions.ptWorkingList;

            /* remove the candidate from the list */
            __arm_2d_helper_remove_item_from_weighted_dirty_region_list(
                &this.Adapter.OptimizedDirtyRegions.ptCandidateList,
                ptCandidate
            );

            ARM_2D_LOG_INFO(
                DIRTY_REGION_OPTIMISATION, 
                1, 
                "UPDATE_WORKING_LIST", 
                "Get one candidate [%p], x:%d y:%d w:%d h:%d", 
                (void *)ptCandidate,
                ptCandidate->tRegion.tLocation.iX,
                ptCandidate->tRegion.tLocation.iY,
                ptCandidate->tRegion.tSize.iWidth,
                ptCandidate->tRegion.tSize.iHeight
            );

            uint32_t wCandidatePixelCount = ptCandidate->tRegion.tSize.iHeight
                                            * ptCandidate->tRegion.tSize.iWidth;
            while(NULL != ptWorking) {
                arm_2d_region_list_item_t *ptNextWorking = ptWorking->ptInternalNext;
                arm_2d_region_t tEnclosureArea;

                ARM_2D_LOG_INFO(
                    DIRTY_REGION_OPTIMISATION, 
                    2, 
                    "UPDATE_WORKING_LIST", 
                    "Try an exiting dirty region in working list [%p], x:%d y:%d w:%d h:%d", 
                    (void *)ptWorking,
                    ptWorking->tRegion.tLocation.iX,
                    ptWorking->tRegion.tLocation.iY,
                    ptWorking->tRegion.tSize.iWidth,
                    ptWorking->tRegion.tSize.iHeight
                );

                int_fast8_t chResult = arm_2d_is_region_inside_target(&ptCandidate->tRegion,
                                                                      &ptWorking->tRegion);

                if (1 == chResult) {
                    /* the candidate is inside the working region,
                     * remove the candidate from the list 
                     */

                    ARM_2D_LOG_INFO(
                        DIRTY_REGION_OPTIMISATION, 
                        3, 
                        "UPDATE_WORKING_LIST", 
                        "The candidate [%p] is inside the working region [%p], drop it directly", 
                        (void *)ptCandidate,
                        (void *)ptWorking
                    );

                #if 0
                    __arm_2d_helper_remove_item_from_weighted_dirty_region_list(
                        &this.Adapter.OptimizedDirtyRegions.ptCandidateList,
                        ptCandidate
                    );
                #endif
                    /* free the working item */
                    __arm_2d_helper_dirty_region_pool_free(ptThis, ptCandidate);

                    goto label_move_to_next_candidate;
                } else if (-1 == chResult) {
                    /* remove the working item from the list */

                    ARM_2D_LOG_INFO(
                        DIRTY_REGION_OPTIMISATION, 
                        3, 
                        "UPDATE_WORKING_LIST", 
                        "The working region [%p] is inside the candidate region [%p], remove the working region from the working list", 
                        (void *)ptWorking, 
                        (void *)ptCandidate
                    );

                    __arm_2d_helper_remove_item_from_weighted_dirty_region_list(
                        &this.Adapter.OptimizedDirtyRegions.ptWorkingList,
                        ptWorking
                    );

                    /* free the working item */
                    __arm_2d_helper_dirty_region_pool_free(ptThis, ptWorking);

                    goto label_move_to_next_working;
                } else {
                    
                    /* has overlap */
                    arm_2d_region_get_minimal_enclosure(&ptWorking->tRegion, 
                                                        &ptCandidate->tRegion,
                                                        &tEnclosureArea);
                    uint32_t wWorkingItemPixelCount 
                                        = ptWorking->tRegion.tSize.iWidth
                                        * ptWorking->tRegion.tSize.iHeight;

                    uint32_t wPixelsRegionEnclosure = tEnclosureArea.tSize.iHeight
                                                    * tEnclosureArea.tSize.iWidth;


                    ARM_2D_LOG_INFO(
                        DIRTY_REGION_OPTIMISATION, 
                        3, 
                        "UPDATE_WORKING_LIST", 
                        "Get the minimal enclosure region x=%d y%d w=%d h=%d", 
                        tEnclosureArea.tLocation.iX,
                        tEnclosureArea.tLocation.iY,
                        tEnclosureArea.tSize.iWidth,
                        tEnclosureArea.tSize.iHeight
                    );

                    if (wPixelsRegionEnclosure <= (wCandidatePixelCount + wWorkingItemPixelCount + 100)) {
                        /* we only refresh the enclosure region to save time */
                        
                        ARM_2D_LOG_INFO(
                            DIRTY_REGION_OPTIMISATION, 
                            4, 
                            "UPDATE_WORKING_LIST", 
                            "Use the minimal enclosure region and free the candidates[%p] and working one [%p]", 
                            (void *)ptCandidate,
                            (void *)ptWorking
                        );

                        /* remove the working item from the list */
                        __arm_2d_helper_remove_item_from_weighted_dirty_region_list(
                            &this.Adapter.OptimizedDirtyRegions.ptWorkingList,
                            ptWorking
                        );
                        /* free the working item */
                        __arm_2d_helper_dirty_region_pool_free(ptThis, ptWorking);

                        /* remove the candidate from the list */
                    #if 0
                        __arm_2d_helper_remove_item_from_weighted_dirty_region_list(
                            &this.Adapter.OptimizedDirtyRegions.ptCandidateList,
                            ptCandidate
                        );
                    #endif
                        /* free the candidate */
                        __arm_2d_helper_dirty_region_pool_free(ptThis, ptCandidate);

                        arm_2d_region_list_item_t *ptDirtyRegion = __arm_2d_helper_dirty_region_pool_new(ptThis);
                        if (NULL == ptDirtyRegion) {
                            this.Adapter.bFailedToOptimizeDirtyRegion = true;
                            return ;
                        }
                        /* initialize the newly allocated dirty region item */
                        ptDirtyRegion->tRegion = tEnclosureArea;

                        /* update candidate list */
                        __arm_2d_helper_add_item_to_weighted_dirty_region_list(
                            &this.Adapter.OptimizedDirtyRegions.ptCandidateList,
                            ptDirtyRegion
                        );

                        ARM_2D_LOG_INFO(
                            DIRTY_REGION_OPTIMISATION, 
                            4, 
                            "UPDATE_WORKING_LIST", 
                            "Generate a new candidate[%p] with the enclosure region and add it to the candidate list", 
                            (void *)ptDirtyRegion
                        );

                        /* re-start processing candidate list from the beginning */
                        goto label_start_process_candidate;
                    }


                    /* remove common part */
                    do {
                        arm_2d_region_t tOverlapped, tResidual;
                        arm_2d_region_t *ptCResidual = NULL;
                        arm_2d_region_t *ptWResidual = NULL;
                        
                        if (!arm_2d_region_intersect(&ptWorking->tRegion, 
                                                    &ptCandidate->tRegion,
                                                    &tOverlapped)) {
                            break;
                        }

                        ARM_2D_LOG_INFO(
                            DIRTY_REGION_OPTIMISATION, 
                            4, 
                            "UPDATE_WORKING_LIST", 
                            "The candidate overlaps with the working region, x=%d y=%d w=%d h=%d", 
                            tOverlapped.tLocation.iX,
                            tOverlapped.tLocation.iY,
                            tOverlapped.tSize.iWidth,
                            tOverlapped.tSize.iHeight
                        );

                        int16_t iCX0 = ptCandidate->tRegion.tLocation.iX;
                        int16_t iCX1 = ptCandidate->tRegion.tLocation.iX
                                     + ptCandidate->tRegion.tSize.iWidth
                                     - 1;
                        int16_t iCY0 = ptCandidate->tRegion.tLocation.iY;
                        int16_t iCY1 = ptCandidate->tRegion.tLocation.iY
                                     + ptCandidate->tRegion.tSize.iHeight
                                     - 1;

                        
                        int16_t iWX0 = ptWorking->tRegion.tLocation.iX;
                        int16_t iWX1 = ptWorking->tRegion.tLocation.iX
                                     + ptWorking->tRegion.tSize.iWidth
                                     - 1;
                        int16_t iWY0 = ptWorking->tRegion.tLocation.iY;
                        int16_t iWY1 = ptWorking->tRegion.tLocation.iY
                                     + ptWorking->tRegion.tSize.iHeight
                                     - 1;

                        bool bXCinW = false;
                        bool bXWinC = false;
                        bool bYCinW = false;
                        bool bYWinC = false;

                        if (iCX0 < iWX0) {
                            if (iCX1 >= iWX1) {
                                /*
                                 *    [CCCCCCCCCCCCCCCCCCC]
                                 *           [WWWWWWWWWWWW]
                                 */
                                bXWinC = true;
                            }
                        } else if (iCX0 > iWX0) {
                            if (iCX1 <= iWX1) {
                                /*
                                 *               [CCCCCCCC]
                                 *           [WWWWWWWWWWWW]
                                 */
                                bXCinW = true;
                            }
                        } else {
                            if (iCX1 <= iWX1) {
                                /*
                                 *           [CCCCCCCC]
                                 *           [WWWWWWWWWWWW]
                                 */
                                bXCinW = true;
                            } else {
                                /*
                                 *           [CCCCCCCCCCCCCCCC]
                                 *           [WWWWWWWWWWWW]
                                 */
                                bXWinC = true;
                            }
                        }

                        if (iCY0 < iWY0) {
                            if (iCY1 >= iWY1) {
                                /*
                                 *    [CCCCCCCCCCCCCCCCCCC]
                                 *           [WWWWWWWWWWWW]
                                 */
                                bYWinC = true;
                            }
                        } else if (iCY0 > iWY0) {
                            if (iCY1 <= iWY1) {
                                /*
                                 *               [CCCCCCCC]
                                 *           [WWWWWWWWWWWW]
                                 */
                                bYCinW = true;
                            }
                        } else {
                            if (iCY1 <= iWY1) {
                                /*
                                 *           [CCCCCCCC]
                                 *           [WWWWWWWWWWWW]
                                 */
                                bYCinW = true;
                            } else {
                                /*
                                 *           [CCCCCCCCCCCCCCCC]
                                 *           [WWWWWWWWWWWW]
                                 */
                                bYWinC = true;
                            }
                        }

                        if (bXCinW) {
                            if (!bYWinC) {
                                /* 
                                 * we don't care bYCinW, as (bYCinW == true) has 
                                 * been filtered before, i.e. the Candidate region
                                 * is inside the Working region.
                                 * 
                                 * remove overlapped region from candidate region
                                 * in Y direction
                                 * 
                                 *    Working
                                 *    +----------------------------+
                                 *    |                            |
                                 *    |  Candidate                 |
                                 *    |  +------------------+      |
                                 *    |  |//////Overlap/////|      |
                                 *    +--+------------------+------+
                                 *       |                  |
                                 *       |                  |
                                 *       +------------------+
                                 */

                                ptCResidual = 
                                    __arm_2d_remove_overlapped_region_vertically(
                                                        &(ptCandidate->tRegion),
                                                        &tOverlapped,
                                                        &tResidual);


                            } else if (iCX0 == iWX0 || iCX1 == iWX1) {
                                /* the candidate region sticks to the right side
                                 * or left side of the working region 
                                 *
                                 *       Candidate
                                 *       +------------------+
                                 *       |                  |   Working
                                 *       +------------------+------+
                                 *       |//////////////////|      |
                                 *       |//////Overlap/////|      |
                                 *       |//////////////////|      |
                                 *       +------------------+------+
                                 *       |                  |
                                 *       |                  |
                                 *       +------------------+
                                 */
                                ptWResidual = 
                                    __arm_2d_remove_overlapped_region_horizontally(
                                                        &(ptWorking->tRegion),
                                                        &tOverlapped,
                                                        &tResidual);

                            }
                        } else if (bXWinC) {

                            if (!bYCinW) {
                                /* 
                                 * we don't care bYWinC, as (bYWinC == true) has 
                                 * been filtered before, i.e. the Candidate region
                                 * is inside the Working region.
                                 * 
                                 * remove overlapped region from working region
                                 * in Y direction
                                 * 
                                 *    Candidate
                                 *    +----------------------------+
                                 *    |                            |
                                 *    |  Working                   |
                                 *    |  +------------------+      |
                                 *    |  |//////Overlap/////|      |
                                 *    +--+------------------+------+
                                 *       |                  |
                                 *       |                  |
                                 *       +------------------+
                                 */
                                
                                ptWResidual = 
                                    __arm_2d_remove_overlapped_region_vertically(
                                                        &(ptWorking->tRegion),
                                                        &tOverlapped,
                                                        &tResidual);

                            } else if (iCX0 == iWX0 || iCX1 == iWX1) {
                                /* the candidate region sticks to the right side
                                 * or left side of the working region 
                                 *
                                 *
                                 *       Working
                                 *       +------------------+
                                 *       |                  |   Candidate
                                 *       +------------------+------+
                                 *       |//////////////////|      |
                                 *       |//////Overlap/////|      |
                                 *       |//////////////////|      |
                                 *       +------------------+------+
                                 *       |                  |
                                 *       |                  |
                                 *       +------------------+
                                 */

                                ptCResidual = 
                                    __arm_2d_remove_overlapped_region_horizontally(
                                                        &(ptCandidate->tRegion),
                                                        &tOverlapped,
                                                        &tResidual);
                            }

                        } else if (bYCinW) {
                            if (!bXWinC) {
                                /* 
                                 * we don't care bXCinW, as (bXCinW == true) has 
                                 * been filtered before, i.e. the Candidate region
                                 * is inside the Working region.
                                 * 
                                 * remove overlapped region from candidate region
                                 * in X direction
                                 *     
                                 *     Working
                                 *     +-------------------+
                                 *     |     Candidate     | 
                                 *     |     +-------------+-------+
                                 *     |     |/////////////|       |
                                 *     |     |/////Overlap/|       |
                                 *     |     +-------------+-------+
                                 *     |                   |
                                 *     +-------------------+
                                 */
                                ptCResidual = 
                                    __arm_2d_remove_overlapped_region_horizontally(
                                                        &(ptCandidate->tRegion),
                                                        &tOverlapped,
                                                        &tResidual);

                            } else if (iCY0 == iWY0 || iCY1 == iWY1) {
                                /* the candidate region sticks to the top 
                                 * or bottom of the working region 
                                 *
                                 *    Candidate          
                                 *    +--+------------------+------+
                                 *    |  |//////////////////|      |
                                 *    |  |//////Overlap/////|      |
                                 *    |  |//////////////////|      |
                                 *    +--+------------------+------+
                                 *       |                  |
                                 *       |                  |
                                 *       +------------------+
                                 *       Working
                                 */
                                ptWResidual = 
                                    __arm_2d_remove_overlapped_region_vertically(
                                                        &(ptWorking->tRegion),
                                                        &tOverlapped,
                                                        &tResidual);
                            }
                        } else if (bYWinC) {
                            if (!bXCinW) {
                                /* 
                                 * we don't care bXWinC, as (bXWinC == true) has 
                                 * been filtered before, i.e. the Candidate region
                                 * is inside the Working region.
                                 * 
                                 * remove overlapped region from working region
                                 * in X direction
                                 *     
                                 *     Candidate
                                 *     +-------------------+
                                 *     |     Working       | 
                                 *     |     +-------------+-------+
                                 *     |     |/////////////|       |
                                 *     |     |/////Overlap/|       |
                                 *     |     +-------------+-------+
                                 *     |                   |
                                 *     +-------------------+
                                 */
                                ptWResidual = 
                                    __arm_2d_remove_overlapped_region_horizontally(
                                                        &(ptWorking->tRegion),
                                                        &tOverlapped,
                                                        &tResidual);
                            } else if (iCY0 == iWY0 || iCY1 == iWY1) {
                                /* the Working region sticks to the top 
                                 * or bottom of the working region 
                                 *
                                 *    Working          
                                 *    +--+------------------+------+
                                 *    |  |//////////////////|      |
                                 *    |  |//////Overlap/////|      |
                                 *    |  |//////////////////|      |
                                 *    +--+------------------+------+
                                 *       |                  |
                                 *       |                  |
                                 *       +------------------+
                                 *       Candidate
                                 */
                                ptCResidual = 
                                    __arm_2d_remove_overlapped_region_vertically(
                                                        &(ptCandidate->tRegion),
                                                        &tOverlapped,
                                                        &tResidual);

                            }
                        }


                        if (NULL != ptCResidual) {
                            /* the candidate is inside the working region horizontally or vertically,
                             */

                            ARM_2D_LOG_INFO(
                                DIRTY_REGION_OPTIMISATION, 
                                3, 
                                "UPDATE_WORKING_LIST", 
                                "The candidate [%p] is overlap with working region [%p] and inside the working region in X axis or Y axis, free the original candidate and add the shrunk one into candidate list", 
                                (void *)ptCandidate,
                                (void *)ptWorking
                            );

                        #if 0
                            __arm_2d_helper_remove_item_from_weighted_dirty_region_list(
                                &this.Adapter.OptimizedDirtyRegions.ptCandidateList,
                                ptCandidate
                            );
                        #endif
                            /* free the candidate */
                            __arm_2d_helper_dirty_region_pool_free(ptThis, ptCandidate);

                            arm_2d_region_list_item_t *ptDirtyRegion = __arm_2d_helper_dirty_region_pool_new(ptThis);
                            if (NULL == ptDirtyRegion) {
                                assert( false == ptCandidate->bFromInternalPool);

                                /* suppose nothing happened */
                                break;
                            }
                            /* initialize the newly allocated dirty region item */
                            ptDirtyRegion->tRegion = *ptCResidual;

                            /* update candidate list */
                            __arm_2d_helper_add_item_to_weighted_dirty_region_list(
                                &this.Adapter.OptimizedDirtyRegions.ptCandidateList,
                                ptDirtyRegion
                            );

                            goto label_start_process_candidate;
                        }

                        if (NULL != ptWResidual) {
                            /* the working region is inside the candidate region horizontally or vertically,
                             * remove the working region from the list 
                             */
                            ARM_2D_LOG_INFO(
                                DIRTY_REGION_OPTIMISATION, 
                                3, 
                                "UPDATE_WORKING_LIST", 
                                "The working [%p] is overlap with overlap region [%p] and inside the candidate region in X axis or Y axis, remove the working region and add the shrunk one into candidate list", 
                                (void *)ptWorking, 
                                (void *)ptCandidate
                            );

                            __arm_2d_helper_remove_item_from_weighted_dirty_region_list(
                                &this.Adapter.OptimizedDirtyRegions.ptWorkingList,
                                ptWorking
                            );

                            /* free the working item */
                            __arm_2d_helper_dirty_region_pool_free(ptThis, ptWorking);

                            arm_2d_region_list_item_t *ptDirtyRegion = __arm_2d_helper_dirty_region_pool_new(ptThis);
                            if (NULL == ptDirtyRegion) {
                                assert( false == ptWorking->bFromInternalPool);

                                /* add it back */
                                __arm_2d_helper_add_item_to_weighted_dirty_region_list(
                                    &this.Adapter.OptimizedDirtyRegions.ptWorkingList,
                                    ptWorking
                                );

                                /* suppose nothing happened */
                                break;
                            }

                            ptDirtyRegion->tRegion = *ptWResidual;

                            /* update candidate list */
                            __arm_2d_helper_add_item_to_weighted_dirty_region_list(
                                &this.Adapter.OptimizedDirtyRegions.ptCandidateList,
                                ptDirtyRegion
                            );

                            goto label_move_to_next_working;
                        }

                    } while(0);

                }
label_move_to_next_working:
                /* no overlap */
                ptWorking = ptNextWorking;  /* get the next dirty region in the working list */
            }

            /* default: no way to optimize the candidate,
             *          add the candidate to the working list
             */

            ARM_2D_LOG_INFO(
                DIRTY_REGION_OPTIMISATION, 
                1, 
                "UPDATE_WORKING_LIST", 
                "Add the candidate[%p] to the working list", 
                (void *)ptCandidate
            );

            __arm_2d_helper_add_item_to_weighted_dirty_region_list(
                &this.Adapter.OptimizedDirtyRegions.ptWorkingList,
                ptCandidate
            );


            /* process next candidate */
            //ptCandidate = ptNextCandiate;
        } while(true);

        if (NULL == ptCandidate) {
            break;  /* all candidates are processed */
        }
    } while(true);

    ARM_2D_LOG_INFO(
        DIRTY_REGION_OPTIMISATION, 
        1, 
        "UPDATE_WORKING_LIST", 
        "All candidates are processed."
    );

}


/*! \brief begin a iteration of drawing and request a frame buffer from 
 *         low level display driver.
 *  \param[in] ptThis the PFB helper control block
 *  \param[in] ptTargetRegion the address of the target region in the LCD
 *         passing NULL means we want to draw the whole LCD.
 *  \retval NULL the display driver is not ready
 *  \retval (intptr_t)-1 the display driver want to ignore this drawing 
 *          (maybe because the target area is out of the LCD)
 *  \retval non-null a tile which contains the (partial) frame buffer
 */
static 
arm_2d_tile_t * __arm_2d_helper_pfb_drawing_iteration_begin(
                                    arm_2d_helper_pfb_t *ptThis,
                                    arm_2d_region_list_item_t *ptDirtyRegions)
{
label_iteration_begin_start:

    if (this.Adapter.bIsDryRun) {
        /* 
         * NOTE: If this is a dry run, no need to allocate PFB again.
         */
        
        if (!this.Adapter.bIsDirtyRegionOptimizationEnabled) {

            this.Adapter.bIsDryRun = false;
            /* refresh the first dirty region as people might update dirty region list 
            * in the dry run
            */
            if (NULL == this.Adapter.ptDirtyRegion) {
                this.Adapter.bIsRegionChanged = true;
            } else if (this.Adapter.ptDirtyRegion->bUpdated || this.Adapter.bEncounterDynamicDirtyRegion) {
                this.Adapter.bIsRegionChanged = true;
            }

        } else {
            if (NULL == this.Adapter.ptDirtyRegion) {
                /* dry run is finished */
                this.Adapter.bIsRegionChanged = true; 
            }
        }

        assert(NULL != this.Adapter.ptCurrent); /* this should not happen */

        if (NULL == this.Adapter.ptCurrent) {   /* but just in case */
            
            ARM_2D_LOG_WARNING(
                HELPER_PFB, 
                1, 
                "Iteration Begin", 
                "In the dry run, ptCurrent is set to NULL. Try to allocate a new PFB from the pool...\r\n"
                "Entering this branch, usually means something went wrong. "
            );

             arm_irq_safe {
                /* allocating pfb only when the number of free PFB blocks is larger than
                * the reserved threashold
                */
                if (this.Adapter.chFreePFBCount > this.tCFG.FrameBuffer.u4PoolReserve) {
                    this.Adapter.ptCurrent = __arm_2d_helper_pfb_new(ptThis);
                }
            }
            
            if (NULL == this.Adapter.ptCurrent) {
                // no resource left
                return NULL;
            }
        }

    } else {
        this.Adapter.ptCurrent = NULL;

        ARM_2D_LOG_INFO(
            HELPER_PFB, 
            1, 
            "Iteration Begin", 
            "Try to allocate a new PFB from the pool..."
        );

        arm_irq_safe {
            /* allocating pfb only when the number of free PFB blocks is larger than
            * the reserved threashold
            */
            if (this.Adapter.chFreePFBCount > this.tCFG.FrameBuffer.u4PoolReserve) {
                this.Adapter.ptCurrent = __arm_2d_helper_pfb_new(ptThis);
            }
        }
        
        if (NULL == this.Adapter.ptCurrent) {
            // no resource left
            return NULL;
        }

        this.Adapter.ptCurrent->bIsNewFrame = this.Adapter.bFirstIteration;
    }

    assert(NULL != this.Adapter.ptCurrent);

    arm_2d_tile_t *ptPartialFrameBuffer = &(this.Adapter.ptCurrent->tTile);

    if (this.Adapter.bFirstIteration) {

        if (this.Adapter.bIsDryRun) {

            ARM_2D_LOG_INFO(
                DIRTY_REGION_OPTIMISATION, 
                1, 
                "Iteration Begin", 
                "Reset to the start of a frame in dry run mode"
            );

            if (    this.tCFG.FrameBuffer.bDebugDirtyRegions
                ||  this.Adapter.bFailedToOptimizeDirtyRegion) {
                /* In dirty region debug mode (or failed to optimize the dirty regions), 
                 * we have to refresh the whole screen instead of the dirty regions
                 */
                this.Adapter.bIsDryRun = false;             /* clear the DryRun flag */

                /* NOTE: due to the dry run, it is not the first iteration of a frame 
                *       but this flag is set to true when all dirty regions are visited, 
                *       so we have to clear it manually.
                * */
                this.Adapter.bFirstIteration = false;       
                this.Adapter.ptDirtyRegion = NULL;          /* refresh the whole screen */

                if (this.tCFG.FrameBuffer.bDebugDirtyRegions) {
                    /* in debug mode, if there is nothing to refresh, we should ignore this frame*/
                    if (NULL == this.Adapter.OptimizedDirtyRegions.ptWorkingList) {
                        ARM_2D_LOG_INFO(
                            DIRTY_REGION_OPTIMISATION, 
                            1, 
                            "Iteration Begin", 
                            "No valid dirty region in the list, this frame SHOULD BE ignored"
                        );
                        
                    #if 1
                        /* free pfb */
                        arm_irq_safe {
                            __arm_2d_helper_pfb_free(ptThis, this.Adapter.ptCurrent);
                            this.Adapter.ptCurrent = NULL;
                        }

                        /* reset flag */
                        this.Adapter.bFirstIteration = true;
                        this.Adapter.bIsDryRun = false;

                        // out of lcd 
                        return (arm_2d_tile_t *)-1;
                    #endif
                    }
                }
            } else {

                /* Use the optimized dirty region working list for refresh
                 */
                this.Adapter.ptDirtyRegion = this.Adapter.OptimizedDirtyRegions.ptWorkingList;
                this.Adapter.bIsUsingOptimizedDirtyRegionList = (NULL != this.Adapter.ptDirtyRegion);

                if (NULL == this.Adapter.ptDirtyRegion) {
                    assert(NULL != ptDirtyRegions);

                    ARM_2D_LOG_INFO(
                        DIRTY_REGION_OPTIMISATION, 
                        1, 
                        "Iteration Begin", 
                        "No valid dirty region in the list, ignore this frame"
                    );

                    /* free pfb */
                    arm_irq_safe {
                        __arm_2d_helper_pfb_free(ptThis, this.Adapter.ptCurrent);
                        this.Adapter.ptCurrent = NULL;
                    }

                    /* reset flag */
                    this.Adapter.bFirstIteration = true;
                    this.Adapter.bIsDryRun = false;

                    // out of lcd 
                    return (arm_2d_tile_t *)-1;
                } else {

                    ARM_2D_LOG_INFO(
                        DIRTY_REGION_OPTIMISATION, 
                        1, 
                        "Iteration Begin", 
                        "Get the first dirty region[%p] from the working list , x=%d y=%d w=%d h=%d", 
                        this.Adapter.ptDirtyRegion,
                        this.Adapter.ptDirtyRegion->tRegion.tLocation.iX,
                        this.Adapter.ptDirtyRegion->tRegion.tLocation.iY,
                        this.Adapter.ptDirtyRegion->tRegion.tSize.iWidth,
                        this.Adapter.ptDirtyRegion->tRegion.tSize.iHeight
                    );
                }
            }
        } else {

            ARM_2D_LOG_INFO(
                HELPER_PFB, 
                1, 
                "Iteration Begin", 
                "The start of a frame in normal mode, get the first dirty region in ptDirtyRegions [%p] ",
                ptDirtyRegions
            );

            this.Adapter.ptDirtyRegion = ptDirtyRegions;
        }
        
        if (NULL == this.Adapter.ptDirtyRegion) {
            /* since we draw the whole frame, no need to take the additional 
             * dirty region list into consideration 
             */
            this.Adapter.bNoAdditionalDirtyRegionList = true;
        } else {
            /* reset flag */
            this.Adapter.bNoAdditionalDirtyRegionList = false;
        }

        this.Adapter.bIsRegionChanged = true;
    }

    /* Fetch a dirty region and adjust its shape according to the alignment 
     * requirements 
     */
    do {
        if (this.Adapter.bIsRegionChanged) {
            this.Adapter.bIsRegionChanged = false;

            if (NULL != this.Adapter.ptDirtyRegion) {

                if (this.Adapter.ptDirtyRegion->bUpdated) {
                    /* clear the region updated flag
                     * NOTE: Please never set bUpdate of a const dirty region
                     *       item to true
                     */
                    this.Adapter.ptDirtyRegion->bUpdated = false;
                    this.Adapter.bEncounterDynamicDirtyRegion = true;

                    ARM_2D_LOG_INFO(
                        HELPER_PFB, 
                        1, 
                        "Iteration Begin", 
                        "The target dirty region [%p] is a dynamic dirty region.",
                        this.Adapter.ptDirtyRegion
                    );
                } else {
                    this.Adapter.bEncounterDynamicDirtyRegion = false;

                    ARM_2D_LOG_INFO(
                        HELPER_PFB, 
                        1, 
                        "Iteration Begin", 
                        "The target dirty region [%p] is a normal dirty region.",
                        this.Adapter.ptDirtyRegion
                    );
                }

                if (this.Adapter.bFirstIteration 
                &&  !this.Adapter.ptDirtyRegion->bIgnore
                &&  this.Adapter.bEncounterDynamicDirtyRegion) {
                    /* Because the only first dirty region is a dynamic region,
                     * there won't be any valid region information, we simply
                     * ignore it to start the dry run
                     */
                    this.Adapter.tTargetRegion = this.tCFG.tDisplayArea;

                    ARM_2D_LOG_INFO(
                        HELPER_PFB, 
                        1, 
                        "Iteration Begin", 
                        "The target dirty region [%p] is the first dynamic dirty region, we ignore its region and start a dry run...",
                        this.Adapter.ptDirtyRegion
                    );

                } else if ( (this.Adapter.ptDirtyRegion->bIgnore)
                            // calculate the valid region
                        ||  (!arm_2d_region_intersect(
                                    &this.tCFG.tDisplayArea, 
                                    &(this.Adapter.ptDirtyRegion->tRegion), 
                                    &this.Adapter.tTargetRegion))) {

                    ARM_2D_LOG_INFO(
                        HELPER_PFB, 
                        1, 
                        "Iteration Begin", 
                        "The dirty region item flags to be ignored[%d] or there is no overlapping with the display area [%p] ",
                        this.Adapter.ptDirtyRegion->bIgnore,
                        this.Adapter.ptDirtyRegion
                    );

                    /*----------------------------------------------------------------*
                     * the code segment for try next dirty region or end early: BEGIN *
                     *----------------------------------------------------------------*/
                    if (__arm_2d_helper_pfb_get_next_dirty_region(ptThis)) {
                        /* note, this function updates the 
                         * this.Adapter.bIsRegionChanged flag
                         */

                        continue;    //!< try next region
                    }

                    if (this.Adapter.bIsDryRun) {

                        ARM_2D_LOG_INFO(
                            HELPER_PFB, 
                            1, 
                            "Iteration Begin", 
                            "It appears we can end dry run earlier."
                        );

                        goto label_iteration_begin_start;
                    }

                    if (this.tCFG.FrameBuffer.bDebugDirtyRegions) {
                        ARM_2D_LOG_INFO(
                            HELPER_PFB, 
                            1, 
                            "Iteration Begin", 
                            "No valid dirty region in the list, this frame SHOULD BE ignored, but as we are in dirty region debug mode, we will continue...";
                        );

                        this.Adapter.tTargetRegion = this.tCFG.tDisplayArea;
                        this.Adapter.ptDirtyRegion = NULL;
                        this.Adapter.bNoAdditionalDirtyRegionList = true;

                    } else {
                        /* free pfb */
                        arm_irq_safe {
                            __arm_2d_helper_pfb_free(ptThis, this.Adapter.ptCurrent);
                            this.Adapter.ptCurrent = NULL;
                        }

                        assert(false == this.Adapter.bIsDryRun);
                        assert(this.Adapter.bFirstIteration);

                        // out of lcd 
                        return (arm_2d_tile_t *)-1;
                        /*--------------------------------------------------------------*
                        * the code segment for try next dirty region or end early: END *
                        *--------------------------------------------------------------*/
                    }
                }

            } else {
                this.Adapter.tTargetRegion = this.tCFG.tDisplayArea;
            }

            /* update this.Adapter.tTargetRegion to fulfill the pixel width
             * alignment request 
             */
            if (this.tCFG.FrameBuffer.u3PixelWidthAlign) {
                uint_fast8_t chPixelWidthAlignMask 
                    = (1 << this.tCFG.FrameBuffer.u3PixelWidthAlign)-1;
                
                int_fast16_t iX = this.Adapter.tTargetRegion.tLocation.iX;
                this.Adapter.tTargetRegion.tLocation.iX 
                    &= ~chPixelWidthAlignMask;
                
                this.Adapter.tTargetRegion.tSize.iWidth 
                    += iX - this.Adapter.tTargetRegion.tLocation.iX;
                this.Adapter.tTargetRegion.tSize.iWidth 
                    = ( this.Adapter.tTargetRegion.tSize.iWidth 
                      + chPixelWidthAlignMask) 
                    & ~chPixelWidthAlignMask;
            }

            /* update this.Adapter.tTargetRegion to fulfill the pixel height
             * alignment request 
             */
            if (this.tCFG.FrameBuffer.u3PixelHeightAlign) {
                uint_fast8_t chPixelHeightAlignMask 
                    = (1 << this.tCFG.FrameBuffer.u3PixelHeightAlign)-1;
                
                int_fast16_t iY = this.Adapter.tTargetRegion.tLocation.iY;
                this.Adapter.tTargetRegion.tLocation.iY 
                    &= ~chPixelHeightAlignMask;
                
                this.Adapter.tTargetRegion.tSize.iHeight 
                    += iY - this.Adapter.tTargetRegion.tLocation.iY;
                this.Adapter.tTargetRegion.tSize.iHeight
                    = ( this.Adapter.tTargetRegion.tSize.iHeight 
                      + chPixelHeightAlignMask) 
                    & ~chPixelHeightAlignMask;
            }


            /* update PFB Size */
            if (this.tCFG.FrameBuffer.bDisableDynamicFPBSize) {
                // reset adapter frame size
                this.Adapter.tFrameSize = this.tCFG.FrameBuffer.tFrameSize;

            } else {
                uint32_t wTargetPixelCount 
                    = this.Adapter.tTargetRegion.tSize.iWidth
                    * this.Adapter.tTargetRegion.tSize.iHeight;
                
                uint32_t wPFBPixelCount = this.Adapter.wPFBPixelCount;
                        
                if (    (wTargetPixelCount <= wPFBPixelCount)
                   ||   (   this.Adapter.tTargetRegion.tSize.iWidth 
                        <   wPFBPixelCount)) {
                    // redefine the shape of PFB
                    
                    this.Adapter.tFrameSize.iWidth 
                        = this.Adapter.tTargetRegion.tSize.iWidth;
                    
                    int32_t nHeight = (int32_t)
                        (   wPFBPixelCount 
                        /   (uint32_t)this.Adapter.tTargetRegion.tSize.iWidth);
                
                    nHeight = MIN(nHeight, this.Adapter.tTargetRegion.tSize.iHeight);

                    if (this.tCFG.FrameBuffer.u3PixelHeightAlign) {
                        uint_fast8_t chPixelHeightAlignMask 
                            = (1 << this.tCFG.FrameBuffer.u3PixelHeightAlign)-1;

                        
                        nHeight &= ~chPixelHeightAlignMask;

                        if (0 == nHeight) {
                            // reset adapter frame size
                            this.Adapter.tFrameSize = this.tCFG.FrameBuffer.tFrameSize;
                        } else {
                            this.Adapter.tFrameSize.iHeight = (int16_t)nHeight;
                        } 

                    } else {
                        this.Adapter.tFrameSize.iHeight = (int16_t)nHeight;
                    }

                } else {
                    // reset adapter frame size
                    this.Adapter.tFrameSize = this.tCFG.FrameBuffer.tFrameSize;
                }
            } 

        }
        break;
    } while(true);
    
    /* - this.Adapter.tTargetRegion: 
     *       the region to draw, it could be a dirty region, or the full screen
     *
     * - this.Adapter.tFrameSize:
     *       The adjusted PFB size.
     * 
     * - this.Adapter.tScanOffset:
     *       the relative location inside the this.Adapter.tTargetRegion. It is 
     *       used to scan the this.Adapter.tTargetRegion with the size stored 
     *       in this.Adapter.tFrameSize. The this.Adapter.tPFBTile ( the region
     *       returned ) is generated based on this region.
     */
    
    arm_2d_region_t tVirtualScreenRegion = {
        .tSize = this.tCFG.tDisplayArea.tSize,
        .tLocation = {
            .iX = - (   this.Adapter.tTargetRegion.tLocation.iX 
                    +   this.Adapter.tScanOffset.iX),
            .iY = - (   this.Adapter.tTargetRegion.tLocation.iY 
                    +   this.Adapter.tScanOffset.iY),
        },
    };
    
    /* adjust the framebuffer actual size */
    ptPartialFrameBuffer->tRegion.tSize.iWidth 
        = MIN(  this.Adapter.tFrameSize.iWidth, 
                this.Adapter.tTargetRegion.tSize.iWidth 
            -   this.Adapter.tScanOffset.iX);
    ptPartialFrameBuffer->tRegion.tSize.iHeight 
        = MIN(  this.Adapter.tFrameSize.iHeight, 
                this.Adapter.tTargetRegion.tSize.iHeight 
            -   this.Adapter.tScanOffset.iY);
    
    /* generate a child tile based on the tVirtualScreenRegion */
    arm_2d_tile_generate_child( ptPartialFrameBuffer, 
                                &tVirtualScreenRegion, 
                                &this.Adapter.tPFBTile, 
                                false);

    do {
        if (this.tCFG.FrameBuffer.bDebugDirtyRegions && this.Adapter.ptDirtyRegion == NULL) {
            /*
             * NOTE: In dirty region debug mode, even when there is no dirty region available,
             *       we should also flush the whole screen for a clear debugging view.
             */
            this.Adapter.bIsDryRun = false;
            break;
        }

        if (this.Adapter.bIsDirtyRegionOptimizationEnabled) {
            bool bFirstIteration = this.Adapter.bFirstIteration;

            /* check whether we need a dry run */
            if (bFirstIteration && NULL != ptDirtyRegions) {
                if (!this.Adapter.bIsDryRun) {
                    this.Adapter.bIsDryRun = true;

                    ARM_2D_LOG_INFO(
                        HELPER_PFB, 
                        1, 
                        "Iteration Begin", 
                        "--------------------Dry Run Begin--------------------"
                    );

                } else {
                    /* starting the real drawing after the dry run */
                    this.Adapter.bIsDryRun = false;

                    /* NOTE: due to the dry run, it is not the first iteration of a frame 
                    *       but this flag is set to true when all dirty regions are visited, 
                    *       so we have to clear it manually.
                    */
                    this.Adapter.bFirstIteration = false;

                    ARM_2D_LOG_INFO(
                        HELPER_PFB, 
                        1, 
                        "Iteration Begin", 
                        "--------------------Dry Run End ---------------------\r\n"
                    );
                }
            }

            if (this.Adapter.bIsDryRun) {
                /* if the dirty region list isn't empty, to support the services that
                * need a dry run to update dirty region (e.g. dynamic dirty regions
                * and transform helper etc), we will do a dry run for the first 
                * iteration. 
                * 
                * To achieve that, we need to modify the this.Adapter.tPFBTile to  
                * ensure that there is no valid region between the root tile and
                * the child tile, i.e draw nothing. 
                */
                this.Adapter.tPFBTile.tRegion.tLocation.iX
                    = ptPartialFrameBuffer->tRegion.tSize.iWidth;
                
                assert(NULL != this.Adapter.ptDirtyRegion);
                if (!this.Adapter.bFailedToOptimizeDirtyRegion) {

                    if (!(bFirstIteration && this.Adapter.bEncounterDynamicDirtyRegion)) {
                        /* NOTE: we won't take the first dynamic dirty region into the 
                        * consideration as there is no valid value 
                        */

                        ARM_2D_LOG_INFO(
                            DIRTY_REGION_OPTIMISATION, 
                            1, 
                            "Iteration Begin", 
                            "Update dirty region working list with the new one [%p], dynamic? [%s]",
                            this.Adapter.ptDirtyRegion,
                            this.Adapter.bEncounterDynamicDirtyRegion ? "true" : "false"
                        );

                        __arm_2d_helper_update_dirty_region_working_list(
                                                ptThis, 
                                                this.Adapter.ptDirtyRegion,
                                                this.Adapter.bEncounterDynamicDirtyRegion);
                    } else {
                        ARM_2D_LOG_INFO(
                            DIRTY_REGION_OPTIMISATION, 
                            1, 
                            "Iteration Begin", 
                            "As the dynamic dirty region is the first available one in the list, we will not add it to the working list in the first iteration."
                        );
                    }
                    /* reset flag */
                    this.Adapter.bEncounterDynamicDirtyRegion = false;
                }

            }
        } else {
            /* check whether we need a dry run */
            if (this.Adapter.bFirstIteration && NULL != ptDirtyRegions) {
                this.Adapter.bIsDryRun = true;

                /* if the dirty region list isn't empty, to support the services that
                * need a dry run to update dirty region (e.g. dynamic dirty regions
                * and transform helper etc), we will do a dry run for the first 
                * iteration. 
                * 
                * To achieve that, we need to modify the this.Adapter.tPFBTile to  
                * ensure that there is no valid region between the root tile and
                * the child tile, i.e draw nothing. 
                */
                this.Adapter.tPFBTile.tRegion.tLocation.iX
                    = ptPartialFrameBuffer->tRegion.tSize.iWidth;
                
                ARM_2D_LOG_INFO(
                    HELPER_PFB, 
                    0, 
                    "Iteration Begin", 
                    "Start Dry-run"
                );
            }
        }
    } while(0);

    if (!this.tCFG.FrameBuffer.bDoNOTUpdateDefaultFrameBuffer) {
        // update default frame buffer
        arm_2d_set_default_frame_buffer(&this.Adapter.tPFBTile);
    }

    arm_with(arm_2d_tile_t, &this.Adapter.tPFBTile, ptTile) {

        /* mark the virtual screen */
        ptTile->tInfo.bVirtualScreen = true;
        /* update the colour info */
        ptTile->tInfo.tColourInfo.u7ColourFormat 
            = this.tCFG.FrameBuffer.u7ColourFormat;

        //! attach PFB info to tile extension field
        ptTile->tInfo.u3ExtensionID = ARM_2D_TILE_EXTENSION_PFB;
        ptTile->tInfo.Extension.PFB.bIsDryRun = this.Adapter.bIsDryRun;
        ptTile->tInfo.Extension.PFB.bIsNewFrame = this.Adapter.bIsNewFrame;
    } 

    return (arm_2d_tile_t *)&(this.Adapter.tPFBTile);
}



/*! \brief end a drawing iteration and decide wether a new iteration is required
 *         or not based on the return value
 *  \param[in] ptThis the PFB control block
 *  \retval true a new iteration is required
 *  \retval false no more iteration is required
 */
static 
bool __arm_2d_helper_pfb_drawing_iteration_end(arm_2d_helper_pfb_t *ptThis)
{
    if (this.Adapter.bIsDryRun) {

        ARM_2D_LOG_INFO(
            HELPER_PFB, 
            1, 
            "Iteration End", 
            "End iteration in dry run mode"
        );


        /* NOTE: if this is a dry run, we don't have to invoke low level rendering
         *       process.
         */

        this.Adapter.bFirstIteration = false;

        if (this.Adapter.bIsDirtyRegionOptimizationEnabled) {
            __arm_2d_helper_pfb_get_next_dirty_region(ptThis);
        }

        return true;
    }

    ARM_2D_LOG_INFO(
        HELPER_PFB, 
        1, 
        "Iteration End", 
        "End iteration in normal mode. Send PFB to display driver..."
    );

    __arm_2d_helper_low_level_rendering(ptThis);
    
    arm_2d_tile_t *ptPartialFrameBuffer = &(this.Adapter.ptCurrent->tTile);
    
    if (!this.tCFG.FrameBuffer.bDoNOTUpdateDefaultFrameBuffer) {
        // update default frame buffer
        arm_2d_set_default_frame_buffer(NULL);
    }

    this.Adapter.tScanOffset.iX 
        += ptPartialFrameBuffer->tRegion.tSize.iWidth;

    if (    this.Adapter.tScanOffset.iX 
        >=  this.Adapter.tTargetRegion.tSize.iWidth) {

        this.Adapter.tScanOffset.iX = 0;

        this.Adapter.tScanOffset.iY 
            += ptPartialFrameBuffer->tRegion.tSize.iHeight;
        
        if (    this.Adapter.tScanOffset.iY 
            >=  this.Adapter.tTargetRegion.tSize.iHeight) {
            // finished
            this.Adapter.tScanOffset.iY = 0;

            return __arm_2d_helper_pfb_get_next_dirty_region(ptThis);
            
        }
    }

    return true;
}

#if     defined(__PERF_COUNTER__)                                               \
    &&  defined(__PERF_CNT_USE_RTOS__)                                          \
    &&  __PER_COUNTER_VER__ >= 20204ul
static task_cycle_info_t s_tPerfCounters[__ARM_2D_PERFC_COUNT - 1];

__WEAK
void *__arm_2d_helper_perf_counter_get(arm_2d_perfc_type_t tType)
{
    if (ARM_2D_PERFC_RENDER == tType) {
        return &s_tPerfCounters[tType];
    }
    
    return NULL;
}

__WEAK void __arm_2d_helper_perf_counter_start( int64_t *plTimestamp, 
                                                arm_2d_perfc_type_t tType)
{
    assert(NULL != plTimestamp);
    if (ARM_2D_PERFC_RENDER == tType) {
        start_task_cycle_counter(&s_tPerfCounters[tType]);
    } else {
        *plTimestamp = arm_2d_helper_get_system_timestamp();
    }
}

__WEAK int32_t __arm_2d_helper_perf_counter_stop(int64_t *plTimestamp,
                                                 arm_2d_perfc_type_t tType)
{
    ARM_2D_UNUSED(plTimestamp);
    
    assert(NULL != plTimestamp);
    
    if (ARM_2D_PERFC_RENDER == tType) {
        return (int32_t)stop_task_cycle_counter(&s_tPerfCounters[tType]);
    }

    return (int32_t)(   (int64_t)arm_2d_helper_get_system_timestamp() 
                    -   (int64_t)*plTimestamp);
}

#else

__WEAK
void *__arm_2d_helper_perf_counter_get(arm_2d_perfc_type_t tType)
{
    ARM_2D_UNUSED(tType);
    return NULL;
}

__WEAK void __arm_2d_helper_perf_counter_start( int64_t *plTimestamp, 
                                                arm_2d_perfc_type_t tType)
{
    ARM_2D_UNUSED(tType);

    assert(NULL != plTimestamp);
    *plTimestamp = arm_2d_helper_get_system_timestamp();
}

__WEAK int32_t __arm_2d_helper_perf_counter_stop(int64_t *plTimestamp,
                                                 arm_2d_perfc_type_t tType)
{
    ARM_2D_UNUSED(tType);
    
    assert(NULL != plTimestamp);
    return (int32_t)(   (int64_t)arm_2d_helper_get_system_timestamp() 
                    -   (int64_t)*plTimestamp);
}

#endif



ARM_NONNULL(1,3)
arm_2d_err_t arm_2d_helper_pfb_update_dependency(
                            arm_2d_helper_pfb_t *ptThis, 
                            uint_fast8_t chMask,
                            const arm_2d_helper_pfb_dependency_t *ptDependency)
{
    assert(NULL != ptThis);
    assert(NULL != ptDependency);
    
    arm_irq_safe {
        if (chMask & ARM_2D_PFB_DEPEND_ON_LOW_LEVEL_RENDERING) {
            this.tCFG.Dependency.evtOnLowLevelRendering 
                = ptDependency->evtOnLowLevelRendering;
        }

        if (chMask & ARM_2D_PFB_DEPEND_ON_DRAWING) {
            this.tCFG.Dependency.evtOnDrawing 
                = ptDependency->evtOnDrawing;
        }

        if (chMask & ARM_2D_PFB_DEPEND_ON_LOW_LEVEL_SYNC_UP) {
            this.tCFG.Dependency.evtOnLowLevelSyncUp 
                = ptDependency->evtOnLowLevelSyncUp;
        }
        
        if (chMask & ARM_2D_PFB_DEPEND_ON_EACH_FRAME_CPL) {
            this.tCFG.Dependency.evtOnEachFrameCPL
                = ptDependency->evtOnEachFrameCPL;
        }

        if (chMask & ARM_2D_PFB_DEPEND_ON_NAVIGATION) {
            this.tCFG.Dependency.Navigation = ptDependency->Navigation;
        }
    }
    
    return ARM_2D_ERR_NONE;
}



arm_fsm_rt_t arm_2d_helper_pfb_task(arm_2d_helper_pfb_t *ptThis, 
                                    arm_2d_region_list_item_t *ptDirtyRegions) 
{   
    assert(NULL != ptThis);
    assert(NULL != this.tCFG.Dependency.evtOnDrawing.fnHandler);
    arm_fsm_rt_t tResult;

ARM_PT_BEGIN(this.Adapter.chPT)

#if     defined(__PERF_COUNTER__)                                               \
    &&  defined(__PERF_CNT_USE_RTOS__)                                          \
    &&  __PER_COUNTER_VER__ >= 20204ul

    if (!perfc_check_task_stack_canary_safe()) {
        /* init the cycle counter for the current task */
        init_task_cycle_counter();
        
        /* initialize the cross tasks task-cycle-info object */
        init_task_cycle_info(&s_tPerfCounters[ARM_2D_PERFC_RENDER]);
        
        static task_cycle_info_agent_t s_tAgent;
        register_task_cycle_agent(&s_tPerfCounters[ARM_2D_PERFC_RENDER],
                                  &s_tAgent);
    }
#endif

    ARM_2D_LOG_INFO(
        HELPER_PFB, 
        0, 
        "PFB TASK", 
        "Start a new frame.."
    );

    this.Statistics.nTotalCycle = 0;
    this.Statistics.nRenderingCycle = 0;
    this.Adapter.bIsNewFrame = true;

    arm_irq_safe {
        if (this.Adapter.bEnableDirtyRegionOptimizationRequest) {
            this.Adapter.bEnableDirtyRegionOptimizationRequest = false;
            this.Adapter.bIsDirtyRegionOptimizationEnabled = true;
        } else if (this.Adapter.bDisableDirtyRegionOptimizationRequest) {
            this.Adapter.bDisableDirtyRegionOptimizationRequest = false;
            this.Adapter.bIsDirtyRegionOptimizationEnabled = false;
        }
    }

    /* initialize the OptimizedDirtyRegions service */
    if (this.Adapter.bIsDirtyRegionOptimizationEnabled) {
        this.Adapter.OptimizedDirtyRegions.ptOriginalList = ptDirtyRegions;
        this.Adapter.bIsUsingOptimizedDirtyRegionList = false;              /* this must be false here */
        this.Adapter.bFailedToOptimizeDirtyRegion = false;

        assert(NULL == this.Adapter.OptimizedDirtyRegions.ptWorkingList);
        assert(NULL == this.Adapter.OptimizedDirtyRegions.ptCandidateList);
    }

    __arm_2d_helper_perf_counter_start(&this.Statistics.lTimestamp,
                                       ARM_2D_PERFC_DRIVER); 
    do {
        /* begin of the drawing iteration, 
         * try to request the tile of frame buffer
         */
        do {
        
            /*! \note In deep embedded applications, a LCD usually is connected 
             *       via a serial interface to save pins, hence the bandwidth 
             *       is limited and the FPS is low due to the bandwidth.
             *       To overcome this issue, some partial-flushing schemes are 
             *       used, such as:
             *       - Dirty Region based partial-flushing
             *       - Flush the known and fixed small area that is updated 
             *         frequently based on the application scenarios. 
             *       
             *       It is worth emphasizing that as we are using partial 
             *       flushing scheme, which means for a given frame, we only 
             *       update those changed area(s) but not the complete frame,
             *       using the term frame per sec (FPS) might confuse people,
             *       hence, we decide to introduce a NEW term called update per
             *       sec (UPS) to avoid this confusion. It reflects what people
             *       feel when looking at the LCD but not necessarily means
             *       the rate that a complete frame is flushed into LCD.  
             *       
             *       In Arm-2D:
             *       - FPS is a sub-set of UPS. 
             *       - UPS forcus on how people feel and FPS is sticks to the 
             *         concept of (full) frame per sec.              
             */
        
            ARM_2D_LOG_INFO(
                HELPER_PFB, 
                0, 
                "PFB TASK", 
                "Iteration Begin, request a PFB..."
            );
            // request to draw the whole LCD
            this.Adapter.ptFrameBuffer = 
                __arm_2d_helper_pfb_drawing_iteration_begin( 
                                                        ptThis, 
                                                        ptDirtyRegions);
            if (NULL == this.Adapter.ptFrameBuffer) {
                ARM_2D_LOG_INFO(
                    HELPER_PFB, 
                    0, 
                    "PFB TASK", 
                    "No PFB is available, waiting..."
                );
                if (NULL != this.tCFG.Dependency.evtOnLowLevelSyncUp.fnHandler){
                     // wait until lcd is ready
                    (*this.tCFG.Dependency.evtOnLowLevelSyncUp.fnHandler)(
                        this.tCFG.Dependency.evtOnLowLevelSyncUp.pTarget
                    );
                } else if ((uintptr_t)NULL != this.Adapter.pFPBPoolAvailable) {
                    arm_2d_port_wait_for_semaphore(
                                                this.Adapter.pFPBPoolAvailable);
                }
                continue;
            } else if (-1 == (intptr_t)this.Adapter.ptFrameBuffer) {
                /* display driver wants to end the drawing */

                ARM_2D_LOG_INFO(
                    HELPER_PFB, 
                    0, 
                    "PFB TASK", 
                    "End the frame drawing."
                );

                goto label_pfb_task_rt_cpl;
            }
        } while(NULL == this.Adapter.ptFrameBuffer);

        /* LCD Latency includes the time of waiting for a PFB block */
        this.Statistics.nRenderingCycle 
            += __arm_2d_helper_perf_counter_stop(&this.Statistics.lTimestamp,
                                                 ARM_2D_PERFC_DRIVER);

        ARM_2D_LOG_INFO(
            HELPER_PFB, 
            0, 
            "PFB TASK", 
            "Get a PFB"
        );

    ARM_PT_ENTRY()
        
        ARM_2D_LOG_INFO(
            HELPER_PFB, 
            0, 
            "PFB TASK", 
            "Call on-drawing-event-handler [%p]( user-obj: %p), IsNewFrame[%s]",
            this.tCFG.Dependency.evtOnDrawing.fnHandler,
            this.tCFG.Dependency.evtOnDrawing.pTarget,
            this.Adapter.bIsNewFrame ? "true" : "false"
        );

        __arm_2d_helper_perf_counter_start( &this.Statistics.lTimestamp,
                                            ARM_2D_PERFC_RENDER);
        
        if (!this.Adapter.bIgnoreCanvasColour) {

            arm_2d_color_info_t tColourFormat = {
                .u7ColourFormat = this.tCFG.FrameBuffer.u7ColourFormat,
            };

            arm_2d_helper_fill_tile_colour( this.Adapter.ptFrameBuffer,
                                            tColourFormat,
                                            this.Adapter.tCanvas);

        }

        /* draw all the gui elements on target frame buffer */
        tResult = this.tCFG.Dependency.evtOnDrawing.fnHandler(
                                        this.tCFG.Dependency.evtOnDrawing.pTarget,
                                        this.Adapter.ptFrameBuffer,
                                        this.Adapter.bIsNewFrame);

        // just in case some one forgot to do this...
        arm_2d_op_wait_async(NULL);

        this.Statistics.nTotalCycle += 
            __arm_2d_helper_perf_counter_stop(  &this.Statistics.lTimestamp,
                                                ARM_2D_PERFC_RENDER); 

        ARM_2D_LOG_INFO(
            HELPER_PFB, 
            0, 
            "PFB TASK", 
            "on-drawing-event-handler return [%d]",
            tResult
        );

        if (arm_fsm_rt_on_going == tResult) {
    ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going)
        } else if (tResult < 0) {
            // error was reported
    ARM_PT_GOTO_PREV_ENTRY(tResult)
        } else if (arm_fsm_rt_wait_for_obj == tResult) {
    ARM_PT_GOTO_PREV_ENTRY(tResult)
        } else { 
    ARM_PT_YIELD(arm_fsm_rt_on_going)
        }

        /* draw navigation layer */
        if (    (NULL != this.tCFG.Dependency.Navigation.evtOnDrawing.fnHandler)
            &&  (!this.Adapter.bHideNavigationLayer)) {
    ARM_PT_ENTRY()
            __arm_2d_helper_perf_counter_start( &this.Statistics.lTimestamp,
                                                ARM_2D_PERFC_RENDER); 

            ARM_2D_LOG_INFO(
                HELPER_PFB, 
                0, 
                "PFB TASK", 
                "draw navigation layer [%p]( user-obj: %p), IsNewFrame[%s]",
                this.tCFG.Dependency.Navigation.evtOnDrawing.fnHandler,
                this.tCFG.Dependency.Navigation.evtOnDrawing.pTarget,
                this.Adapter.bIsNewFrame ? "true" : "false"
            );

            tResult = this.tCFG.Dependency.Navigation.evtOnDrawing.fnHandler(
                    this.tCFG.Dependency.Navigation.evtOnDrawing.pTarget,
                    this.Adapter.ptFrameBuffer,
                    this.Adapter.bIsNewFrame);
            arm_2d_op_wait_async(NULL);
            
            this.Statistics.nTotalCycle += 
                __arm_2d_helper_perf_counter_stop(
                                                &this.Statistics.lTimestamp,
                                                ARM_2D_PERFC_RENDER); 

            ARM_2D_LOG_INFO(
                HELPER_PFB, 
                0, 
                "PFB TASK", 
                "draw navigation layer return [%d]",
                tResult
            );

        if (arm_fsm_rt_on_going == tResult) {
        ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going)
            } else if (tResult < 0) {
                // error was reported
        ARM_PT_GOTO_PREV_ENTRY(tResult)
            } else if (arm_fsm_rt_wait_for_obj == tResult) {
        ARM_PT_GOTO_PREV_ENTRY(tResult)
            } else { 
        ARM_PT_YIELD(arm_fsm_rt_on_going)
            }
        }
        
        /* draw dirty regions */
        if (this.tCFG.FrameBuffer.bDebugDirtyRegions && !this.Adapter.bIsDryRun) {


            if (this.Adapter.bIsDirtyRegionOptimizationEnabled) {
                arm_2d_region_list_item_t *ptRegionListItem 
                    = this.Adapter.OptimizedDirtyRegions.ptWorkingList;

                while(NULL != ptRegionListItem) {
                    if (!ptRegionListItem->bIgnore) {
                        arm_2d_helper_draw_box( this.Adapter.ptFrameBuffer, 
                                                &ptRegionListItem->tRegion, 
                                                1,  
                                                GLCD_COLOR_RED, 255);
                    }
                    
                    ptRegionListItem = ptRegionListItem->ptInternalNext;
                }
            } else {
                arm_2d_region_list_item_t *ptRegionListItem = ptDirtyRegions;
            
                while(NULL != ptRegionListItem) {
                    if (!ptRegionListItem->bIgnore) {

                        arm_2d_helper_draw_box( this.Adapter.ptFrameBuffer, 
                                                &ptRegionListItem->tRegion, 
                                                1,  
                                                GLCD_COLOR_GREEN, 255);
                    }
                    
                    ptRegionListItem = ptRegionListItem->ptNext;
                }
            }
        }

        this.Adapter.bIsNewFrame = false;
        __arm_2d_helper_perf_counter_start( &this.Statistics.lTimestamp,
                                            ARM_2D_PERFC_DRIVER); 
    } while(__arm_2d_helper_pfb_drawing_iteration_end(ptThis));
    
label_pfb_task_rt_cpl:    
    this.Statistics.nRenderingCycle 
        += __arm_2d_helper_perf_counter_stop(   &this.Statistics.lTimestamp,
                                                ARM_2D_PERFC_DRIVER); 

ARM_PT_END()

    if (this.Adapter.bIsDirtyRegionOptimizationEnabled) {
        ARM_2D_LOG_INFO(
            DIRTY_REGION_OPTIMISATION, 
            0, 
            "PFB TASK", 
            "Free the working list and the candidate list."
        );

#if 0 /* reserved for debug only */
        do {
            arm_2d_region_list_item_t *ptRegionListItem = ptDirtyRegions;
            
            while(NULL != ptRegionListItem) {
                if (!ptRegionListItem->bIgnore) {
                    ARM_2D_LOG_INFO(
                        DIRTY_REGION_OPTIMISATION, 
                        0, 
                        "PFB TASK", 
                        "Original Dirty Region [%p], x=%d\ty=%d\t w=%d\th%d",
                        (void *)ptRegionListItem,
                        ptRegionListItem->tRegion.tLocation.iX,
                        ptRegionListItem->tRegion.tLocation.iY,
                        ptRegionListItem->tRegion.tSize.iWidth,
                        ptRegionListItem->tRegion.tSize.iHeight
                    );
                }
                
                ptRegionListItem = ptRegionListItem->ptNext;
            }

            if (this.Adapter.bIsDirtyRegionOptimizationEnabled) {
                ptRegionListItem = this.Adapter.OptimizedDirtyRegions.ptWorkingList;

                while(NULL != ptRegionListItem) {
                    if (!ptRegionListItem->bIgnore) {
                        ARM_2D_LOG_INFO(
                            DIRTY_REGION_OPTIMISATION, 
                            0, 
                            "PFB TASK", 
                            "Optimized Dirty Region [%p], x=%d\ty=%d\t w=%d\th%d",
                            (void *)ptRegionListItem,
                            ptRegionListItem->tRegion.tLocation.iX,
                            ptRegionListItem->tRegion.tLocation.iY,
                            ptRegionListItem->tRegion.tSize.iWidth,
                            ptRegionListItem->tRegion.tSize.iHeight
                        );
                    }
                    
                    ptRegionListItem = ptRegionListItem->ptInternalNext;
                }
            }
        } while(0);
#endif

        /* free working list */
        __arm_2d_helper_free_dirty_region_working_list(
                                ptThis, 
                                this.Adapter.OptimizedDirtyRegions.ptWorkingList);
        
        /* free candidate list */
        __arm_2d_helper_free_dirty_region_working_list(
                                ptThis, 
                                this.Adapter.OptimizedDirtyRegions.ptCandidateList);

        if (this.Adapter.bFailedToOptimizeDirtyRegion) {
            this.Adapter.bIsDirtyRegionOptimizationEnabled = false;

            ARM_2D_LOG_WARNING(
                DIRTY_REGION_OPTIMISATION, 
                0, 
                "PFB TASK", 
                "As we failed to optimize dirty regions due to insufficient dirty region items in the pool, we have to turn of the optimization service."
            );
        }
    }

    /* invoke the On Each Frame Complete Event */
    ARM_2D_INVOKE(  this.tCFG.Dependency.evtOnEachFrameCPL.fnHandler,
                    this.tCFG.Dependency.evtOnEachFrameCPL.pTarget);

    ARM_2D_LOG_INFO(
        HELPER_PFB, 
        0, 
        "PFB TASK", 
        "Drawing frame complete.\r\n\r\n"
    );

    return arm_fsm_rt_cpl;
}

/*----------------------------------------------------------------------------*
 * 3FB (Direct Mode) Helper                                                   *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1)
static
void __arm_2d_helper_3fb_dma_copy(  arm_2d_helper_3fb_t *ptThis,
                                    void *pObj,
                                    uintptr_t pnSource,
                                    uintptr_t pnTarget,
                                    uint32_t nDataItemCount,
                                    uint_fast8_t chDataItemSize)
{
    ARM_2D_UNUSED(pObj);
    
    memcpy((void *)pnTarget, (const void *)pnSource, nDataItemCount * chDataItemSize);

    arm_2d_helper_3fb_report_dma_copy_complete(ptThis);
}

static
bool __arm_2d_helper_3fb_dma_2d_copy(   arm_2d_helper_3fb_t *ptThis, 
                                        void *pObj,
                                        uintptr_t pnSource,
                                        uint32_t wSourceStride,
                                        uintptr_t pnTarget,
                                        uint32_t wTargetStride,
                                        int16_t iWidth,
                                        int16_t iHeight,
                                        uint_fast8_t chBytePerPixel ) 
{
    ARM_2D_UNUSED(ptThis);
    ARM_2D_UNUSED(pObj);
    assert(iWidth * chBytePerPixel <= wSourceStride);
    assert(iWidth * chBytePerPixel <= wTargetStride);

    /* 2D copy */
    for (int_fast16_t i = 0; i < iHeight; i++) {
        memcpy( (void *)pnTarget, (void *)pnSource, iWidth * chBytePerPixel );

        pnSource += wSourceStride;
        pnTarget += wTargetStride;
    }

    return true;
}

ARM_NONNULL(1,2)
void arm_2d_helper_3fb_init(arm_2d_helper_3fb_t *ptThis, 
                            const arm_2d_helper_3fb_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert((uintptr_t)NULL != ptCFG->pnAddress[0]);
    assert((uintptr_t)NULL != ptCFG->pnAddress[1]);
    assert((uintptr_t)NULL != ptCFG->pnAddress[2]);
    assert(0 == (ptCFG->chPixelBits & 0x7));

    memset(ptThis, 0, sizeof(arm_2d_helper_3fb_t));

    this.tCFG = *ptCFG;
    uint32_t wFrameBufferSize = this.tCFG.tScreenSize.iHeight 
                                    * this.tCFG.tScreenSize.iWidth 
                                    * (this.tCFG.chPixelBits >> 3);


    memset((void *)this.tCFG.pnAddress[0], 0, wFrameBufferSize);
    memset((void *)this.tCFG.pnAddress[1], 0, wFrameBufferSize);
    memset((void *)this.tCFG.pnAddress[2], 0, wFrameBufferSize);

    this.Runtime.tState[0] = ARM_3FB_STATE_READY_TO_DRAW;
    this.Runtime.u2ReadyToDraw = 0;

    this.Runtime.tState[1] = ARM_3FB_STATE_DRAWING;
    this.Runtime.u2Drawing = 1;

    this.Runtime.tState[2] = ARM_3FB_STATE_FLUSHING;
    this.Runtime.u2Flushing = 2;

    this.Runtime.u2ReadyToFlush = ARM_2D_3FB_INVALID_IDX;

    this.Runtime.tSemaphore = arm_2d_port_new_semaphore();

    if (NULL == this.tCFG.evtOn2DCopy.fnHandler) {
        this.tCFG.evtOn2DCopy.fnHandler = __arm_2d_helper_3fb_dma_2d_copy;
    }
    if (NULL == this.tCFG.evtOnDMACopy.fnHandler) {
        this.tCFG.evtOnDMACopy.fnHandler = __arm_2d_helper_3fb_dma_copy;
    }
}

ARM_NONNULL(1)
void arm_2d_helper_3fb_report_dma_copy_complete(arm_2d_helper_3fb_t *ptThis)
{
    this.Runtime.bFBCopyComplete = true;
    arm_2d_port_set_semaphore(this.Runtime.tSemaphore);
}



ARM_NONNULL(1)
void * arm_2d_helper_3fb_get_flush_pointer(arm_2d_helper_3fb_t *ptThis)
{
    assert(NULL != ptThis);

    uint_fast8_t chFlushingIndex;
    uint_fast8_t chReadyToFlushIndex;
    uint_fast8_t chReadyToDrawIndex;

    arm_irq_safe {
        chFlushingIndex = this.Runtime.u2Flushing;
        chReadyToFlushIndex = this.Runtime.u2ReadyToFlush;
        chReadyToDrawIndex = this.Runtime.u2ReadyToDraw;
    }

    if (    ARM_2D_3FB_INVALID_IDX != chReadyToFlushIndex
        &&  ARM_2D_3FB_INVALID_IDX == chReadyToDrawIndex) {
        /* a new fb is ready to flush and the ready-to-draw slot is available */
        
        arm_irq_safe {
            this.Runtime.tState[chFlushingIndex]        = ARM_3FB_STATE_READY_TO_DRAW;
            this.Runtime.u2ReadyToDraw = chFlushingIndex;

            this.Runtime.tState[chReadyToFlushIndex]    = ARM_3FB_STATE_FLUSHING;
            this.Runtime.u2Flushing = chReadyToFlushIndex;

            this.Runtime.u2ReadyToFlush = ARM_2D_3FB_INVALID_IDX;
        }
    }

    return (void *)this.tCFG.pnAddress[this.Runtime.u2Flushing];
}

ARM_NONNULL(1)
static uintptr_t 
__arm_2d_helper_3fb_get_drawing_pointer(arm_2d_helper_3fb_t *ptThis, bool bIsNewFrame)
{
    assert(NULL != ptThis);

    uint_fast8_t chDrawingIndex;
    uint_fast8_t chReadyToDrawIndex;


    arm_irq_safe {
        chDrawingIndex = this.Runtime.u2Drawing;
        chReadyToDrawIndex = this.Runtime.u2ReadyToDraw;
    }

    uintptr_t pnAddress = this.tCFG.pnAddress[chDrawingIndex];
    bool bPrepareForCopy = false;

    if (bIsNewFrame) {
        arm_irq_safe {

            /* drawing: no */
            if (ARM_2D_3FB_INVALID_IDX == this.Runtime.u2Drawing) {
                /* ensure ready-to-draw: available */
                assert(ARM_2D_3FB_INVALID_IDX != chReadyToDrawIndex);

                uint_fast8_t chIndex = chReadyToDrawIndex;

                /* update FB status */
                this.Runtime.tState[chIndex] = ARM_3FB_STATE_DRAWING;

                /* update pointer */
                this.Runtime.u2ReadyToDraw = ARM_2D_3FB_INVALID_IDX;
                this.Runtime.u2Drawing = chIndex;

                pnAddress = this.tCFG.pnAddress[chIndex];
                arm_exit_irq_safe;   /* jump out */
            } /* drawing: yes */

            /* check whether Ready-To-Flush is empty */
            if (this.Runtime.u2ReadyToFlush == ARM_2D_3FB_INVALID_IDX) {
                
                /* ensure ready-to-draw: available */
                assert(ARM_2D_3FB_INVALID_IDX != chReadyToDrawIndex);
                this.Runtime.tState[chDrawingIndex] = ARM_3FB_STATE_COPYING_AS_SOURCE;
                this.Runtime.tState[chReadyToDrawIndex] = ARM_3FB_STATE_COPYING_AS_TARGET;
                this.Runtime.u2ReadyToDraw = ARM_2D_3FB_INVALID_IDX;
                this.Runtime.u2Drawing = ARM_2D_3FB_INVALID_IDX;

                bPrepareForCopy = true;
            } 
        }

        if (bPrepareForCopy) {

            this.Runtime.bFBCopyComplete = false;
            __arm_2d_helper_3fb_dma_copy(ptThis, 
                                        this.tCFG.evtOnDMACopy.pObj,
                                        this.tCFG.pnAddress[chDrawingIndex],
                                        this.tCFG.pnAddress[chReadyToDrawIndex],
                                        this.tCFG.tScreenSize.iWidth * this.tCFG.tScreenSize.iHeight,
                                        this.tCFG.chPixelBits >> 3);

            while(true) {
                if (this.Runtime.bFBCopyComplete) {
                    break;
                }
                while(!arm_2d_port_wait_for_semaphore(this.Runtime.tSemaphore));
            }

            /* update state and pointers */
            arm_irq_safe {

                this.Runtime.tState[chDrawingIndex] = ARM_3FB_STATE_READY_TO_FLUSH;
                this.Runtime.u2ReadyToFlush = chDrawingIndex;

                this.Runtime.tState[chReadyToDrawIndex] = ARM_3FB_STATE_DRAWING;
                this.Runtime.u2Drawing = chReadyToDrawIndex;

                pnAddress = this.tCFG.pnAddress[chReadyToDrawIndex];
            }
        }
    }

    return pnAddress;
}



ARM_NONNULL(1,2)
bool __arm_2d_helper_3fb_draw_bitmap( arm_2d_helper_3fb_t *ptThis, 
                                      const arm_2d_pfb_t *ptPFB)
{

    assert(NULL != ptThis);
    assert(NULL != ptPFB);
    assert(0 == (this.tCFG.chPixelBits & 0x07));

    //uint_fast8_t chBytesPerPixel = this.tCFG.chPixelBits >> 3;
    int16_t iLCDWidth = this.tCFG.tScreenSize.iWidth;
    int16_t iLCDHeight = this.tCFG.tScreenSize.iHeight;
    uintptr_t pnAddress = __arm_2d_helper_3fb_get_drawing_pointer(ptThis, ptPFB->bIsNewFrame);

    uint_fast8_t chBytePerPixel = this.tCFG.chPixelBits >> 3;
    uint32_t wLCDStrideInByte = chBytePerPixel * iLCDWidth;

    int16_t iX = ptPFB->tTile.tRegion.tLocation.iX;
    int16_t iY = ptPFB->tTile.tRegion.tLocation.iY;
    int16_t iPFBWidth = ptPFB->tTile.tRegion.tSize.iWidth;
    int16_t iPFBHeight = ptPFB->tTile.tRegion.tSize.iHeight;
    uint32_t wPFBStrideInByte = chBytePerPixel * iPFBWidth;

    /* calculate source and target address */
    uintptr_t pnTarget = pnAddress + (iY * iLCDWidth + iX) * chBytePerPixel;
    uintptr_t pnSource = ptPFB->tTile.nAddress;

    /* region clipping */
    int16_t iHeight = MIN(iPFBHeight, iLCDHeight);
    int16_t iWidth = MIN(iPFBWidth, iLCDWidth);

    return this.tCFG.evtOn2DCopy.fnHandler(ptThis,
                                            this.tCFG.evtOnDMACopy.pObj,
                                            pnSource,
                                            wPFBStrideInByte,
                                            pnTarget,
                                            wLCDStrideInByte,
                                            iWidth,
                                            iHeight,
                                            chBytePerPixel
                                        );
}


/*----------------------------------------------------------------------------*
 * PFB Rotate Screen 90 PFB Helper                                            *
 *----------------------------------------------------------------------------*/

__WEAK 
void __arm_2d_rotate_90_c8bit(  uint8_t * __restrict pchOrigin,
                                uint8_t * __restrict pchOutput,
                                int_fast16_t iOriginWidth,
                                int_fast16_t iOriginHeight,
                                int_fast16_t iOutputWidth)
{
    assert(iOriginHeight <= iOutputWidth);

    int_fast16_t iOriginY = iOriginHeight - 1;
    uint8_t * __restrict pchDesColumnStart = pchOutput + iOriginY;
    do {
        uint8_t * __restrict pchSrcLine = pchOrigin;
        
        /* select a column in target buffer */
        uint8_t * __restrict pchDesColumn = pchDesColumnStart--;
        
        int_fast16_t iOriginX = iOriginWidth;
        do {
            *pchDesColumn = *pchSrcLine++;
            pchDesColumn += iOutputWidth;
        } while(--iOriginX);

        pchOrigin += iOriginWidth;
    } while(iOriginY--);
}

__WEAK 
void __arm_2d_rotate_90_rgb16(  uint16_t * __restrict phwOrigin,
                                uint16_t * __restrict phwOutput,
                                int_fast16_t iOriginWidth,
                                int_fast16_t iOriginHeight,
                                int_fast16_t iOutputWidth)
{
    assert(iOriginHeight <= iOutputWidth);

#if 0 /* We keep this prototype algorihtm for ease of understanding */
    for (int16_t iOriginY = 0; iOriginY < iOriginHeight; iOriginY++) {
        uint16_t * __restrict phwSrcLine = phwOrigin;
        
        /* select a column in target buffer */
        uint16_t * __restrict phwDesColumn = phwOutput + (iOriginHeight - iOriginY - 1);
        
        for (int16_t iOriginX = 0; iOriginX < iOriginWidth; iOriginX++) {
            *phwDesColumn = *phwSrcLine++;
            
            phwDesColumn += iOutputWidth;
        }

        phwOrigin += iOriginWidth;
    }
#else

    if ((0 == (iOriginWidth & 0x3)) && (0 == (iOutputWidth & 0x3))) {
        int_fast16_t iOriginY = iOriginHeight;
        uint16_t * __restrict phwDesColumnStart = phwOutput + (iOriginHeight - 1);

        do {
            uint16_t * __restrict phwSrcLine[4] = {
                phwOrigin + iOriginWidth * 0,
                phwOrigin + iOriginWidth * 1,
                phwOrigin + iOriginWidth * 2,
                phwOrigin + iOriginWidth * 3,
            };

            /* select a column in target buffer */
            uint16_t * __restrict phwDesColumn = phwDesColumnStart;
            phwDesColumnStart -= 4;

            int_fast16_t iOriginX = iOriginWidth;
            do {
                uint64_t * __restrict pdw4PixelStride = (uint64_t * __restrict)(phwDesColumn - 3);
                register uint64_t dw4PixelStride;

                dw4PixelStride = *phwSrcLine[0]++;
                dw4PixelStride <<= 16;
                dw4PixelStride |= *phwSrcLine[1]++;
                dw4PixelStride <<= 16;
                dw4PixelStride |= *phwSrcLine[2]++;
                dw4PixelStride <<= 16;
                dw4PixelStride |= *phwSrcLine[3]++;
                
                *pdw4PixelStride = dw4PixelStride;
                
                phwDesColumn += iOutputWidth;
            } while(--iOriginX);

            phwOrigin += iOriginWidth * 4;

            iOriginY -= 4;
        } while(iOriginY);

    } else {
        int_fast16_t iOriginY = iOriginHeight - 1;
        uint16_t * __restrict phwDesColumnStart = phwOutput + iOriginY;
        do {
            uint16_t * __restrict phwSrcLine = phwOrigin;
            
            /* select a column in target buffer */
            uint16_t * __restrict phwDesColumn = phwDesColumnStart--;
            
            int_fast16_t iOriginX = iOriginWidth;
            do {
                *phwDesColumn = *phwSrcLine++;
                phwDesColumn += iOutputWidth;
            } while(--iOriginX);

            phwOrigin += iOriginWidth;
        } while(iOriginY--);
    }

#endif
}

__WEAK 
void __arm_2d_rotate_90_rgb32(  uint32_t * __restrict pwOrigin,
                                uint32_t * __restrict pwOutput,
                                int_fast16_t iOriginWidth,
                                int_fast16_t iOriginHeight,
                                int_fast16_t iOutputWidth)
{
    assert(iOriginHeight <= iOutputWidth);

    int_fast16_t iOriginY = iOriginHeight - 1;
    uint32_t * __restrict pwDesColumnStart = pwOutput + iOriginY;
    do {
        uint32_t * __restrict pwSrcLine = pwOrigin;
        
        /* select a column in target buffer */
        uint32_t * __restrict pwDesColumn = pwDesColumnStart--;
        
        int_fast16_t iOriginX = iOriginWidth;
        do {
            *pwDesColumn = *pwSrcLine++;
            pwDesColumn += iOutputWidth;
        } while(--iOriginX);

        pwOrigin += iOriginWidth;
    } while(iOriginY--);
}

ARM_NONNULL(1,2,3)
arm_2d_pfb_t * __arm_2d_helper_pfb_rotate90_c8bit(  arm_2d_pfb_t *ptOrigin, 
                                                    arm_2d_pfb_t *ptScratch,
                                                    const arm_2d_size_t *ptScreenSize)
{
    assert(NULL != ptOrigin);
    assert(NULL != ptScratch);

    uint8_t * __restrict pchOrigin = (uint8_t * __restrict)ptOrigin->tTile.pchBuffer;
    uint8_t * __restrict pchOutput = (uint8_t * __restrict)ptScratch->tTile.pchBuffer;

    int16_t iWidth = ptOrigin->tTile.tRegion.tSize.iWidth;
    int16_t iHeight = ptOrigin->tTile.tRegion.tSize.iHeight;

    __arm_2d_rotate_90_c8bit(pchOrigin, pchOutput, iWidth, iHeight, iHeight);

    *ptScratch = *ptOrigin;
    ptScratch->tTile.pchBuffer = (uint8_t *)pchOutput;
    ptScratch->ptNext = NULL;

    ptScratch->tTile.tRegion.tSize.iWidth = iHeight;
    ptScratch->tTile.tRegion.tSize.iHeight = iWidth;

    arm_2d_location_t tNewLocation = {
        ptScreenSize->iHeight - (ptOrigin->tTile.tRegion.tLocation.iY + iHeight - 1) - 1,
        ptOrigin->tTile.tRegion.tLocation.iX,
    };

    ptScratch->tTile.tRegion.tLocation = tNewLocation;

    return ptScratch;
}

ARM_NONNULL(1,2,3)
arm_2d_pfb_t * __arm_2d_helper_pfb_rotate90_rgb16(  arm_2d_pfb_t *ptOrigin, 
                                                    arm_2d_pfb_t *ptScratch,
                                                    const arm_2d_size_t *ptScreenSize)
{
    assert(NULL != ptOrigin);
    assert(NULL != ptScratch);

    uint16_t * __restrict phwOrigin = (uint16_t * __restrict)ptOrigin->tTile.phwBuffer;
    uint16_t * __restrict phwOutput = (uint16_t * __restrict)ptScratch->tTile.phwBuffer;

    int16_t iWidth = ptOrigin->tTile.tRegion.tSize.iWidth;
    int16_t iHeight = ptOrigin->tTile.tRegion.tSize.iHeight;

    __arm_2d_rotate_90_rgb16(phwOrigin, phwOutput, iWidth, iHeight, iHeight);

    *ptScratch = *ptOrigin;
    ptScratch->tTile.phwBuffer = (uint16_t *)phwOutput;
    ptScratch->ptNext = NULL;

    ptScratch->tTile.tRegion.tSize.iWidth = iHeight;
    ptScratch->tTile.tRegion.tSize.iHeight = iWidth;

    arm_2d_location_t tNewLocation = {
        ptScreenSize->iHeight - (ptOrigin->tTile.tRegion.tLocation.iY + iHeight - 1) - 1,
        ptOrigin->tTile.tRegion.tLocation.iX,
    };

    ptScratch->tTile.tRegion.tLocation = tNewLocation;

    return ptScratch;
}

ARM_NONNULL(1,2,3)
arm_2d_pfb_t * __arm_2d_helper_pfb_rotate90_rgb32(  arm_2d_pfb_t *ptOrigin, 
                                                    arm_2d_pfb_t *ptScratch,
                                                    const arm_2d_size_t *ptScreenSize)
{
    assert(NULL != ptOrigin);
    assert(NULL != ptScratch);

    uint32_t * __restrict pwOrigin = (uint32_t * __restrict)ptOrigin->tTile.pwBuffer;
    uint32_t * __restrict pwOutput = (uint32_t * __restrict)ptScratch->tTile.pwBuffer;

    int16_t iWidth = ptOrigin->tTile.tRegion.tSize.iWidth;
    int16_t iHeight = ptOrigin->tTile.tRegion.tSize.iHeight;

    __arm_2d_rotate_90_rgb32(pwOrigin, pwOutput, iWidth, iHeight, iHeight);

    *ptScratch = *ptOrigin;
    ptScratch->tTile.pwBuffer = (uint32_t *)pwOutput;
    ptScratch->ptNext = NULL;

    ptScratch->tTile.tRegion.tSize.iWidth = iHeight;
    ptScratch->tTile.tRegion.tSize.iHeight = iWidth;

    arm_2d_location_t tNewLocation = {
        ptScreenSize->iHeight - (ptOrigin->tTile.tRegion.tLocation.iY + iHeight - 1) - 1,
        ptOrigin->tTile.tRegion.tLocation.iX,
    };

    ptScratch->tTile.tRegion.tLocation = tNewLocation;

    return ptScratch;
}



/*----------------------------------------------------------------------------*
 * PFB Rotate Screen 270 PFB Helper                                           *
 *----------------------------------------------------------------------------*/

__WEAK 
void __arm_2d_rotate_270_c8bit(  uint8_t * __restrict pchOrigin,
                                uint8_t * __restrict pchOutput,
                                int_fast16_t iOriginWidth,
                                int_fast16_t iOriginHeight,
                                int_fast16_t iOutputWidth)
{
    assert(iOriginHeight <= iOutputWidth);

    uintptr_t uColoumOffset =  iOutputWidth * (iOriginWidth - 1);
    uint8_t * __restrict pchDesColumnStart = pchOutput + uColoumOffset;

    int_fast16_t iOriginY = iOriginHeight;
    do {
        uint8_t * __restrict pchSrcLine = pchOrigin;

        /* select a column in target buffer */
        uint8_t * __restrict pchDesColumn = pchDesColumnStart++;;

        int_fast16_t iOriginX = iOriginWidth;
        do {
            *pchDesColumn = *pchSrcLine++;
            pchDesColumn -= iOutputWidth;
        } while(--iOriginX);

        pchOrigin += iOriginWidth;
    } while(--iOriginY);
}

__WEAK 
void __arm_2d_rotate_270_rgb16(  uint16_t * __restrict phwOrigin,
                                uint16_t * __restrict phwOutput,
                                int_fast16_t iOriginWidth,
                                int_fast16_t iOriginHeight,
                                int_fast16_t iOutputWidth)
{
    assert(iOriginHeight <= iOutputWidth);

#if 0 /* We keep this prototype algorihtm for ease of understanding */
    uintptr_t uColoumOffset =  iOutputWidth * (iOriginWidth - 1);
    
    for (int16_t iOriginY = 0; iOriginY < iOriginHeight; iOriginY++) {
        uint16_t * __restrict phwSrcLine = phwOrigin;
        
        /* select a column in target buffer */
        uint16_t * __restrict phwDesColumn = phwOutput + iOriginY + uColoumOffset;
        
        for (int16_t iOriginX = 0; iOriginX < iOriginWidth; iOriginX++) {
            *phwDesColumn = *phwSrcLine++;
            
            phwDesColumn -= iOutputWidth;
        }

        phwOrigin += iOriginWidth;
    }
#else

    uintptr_t uColoumOffset =  iOutputWidth * (iOriginWidth - 1);
    uint16_t * __restrict phwDesColumnStart = phwOutput + uColoumOffset;

    int_fast16_t iOriginY = iOriginHeight;
    do {
        uint16_t * __restrict phwSrcLine = phwOrigin;

        /* select a column in target buffer */
        uint16_t * __restrict phwDesColumn = phwDesColumnStart++;;

        int_fast16_t iOriginX = iOriginWidth;
        do {
            *phwDesColumn = *phwSrcLine++;
            phwDesColumn -= iOutputWidth;
        } while(--iOriginX);

        phwOrigin += iOriginWidth;
    } while(--iOriginY);

#endif
}

__WEAK 
void __arm_2d_rotate_270_rgb32(  uint32_t * __restrict pwOrigin,
                                uint32_t * __restrict pwOutput,
                                int_fast16_t iOriginWidth,
                                int_fast16_t iOriginHeight,
                                int_fast16_t iOutputWidth)
{
    assert(iOriginHeight <= iOutputWidth);

    uintptr_t uColoumOffset =  iOutputWidth * (iOriginWidth - 1);
    uint32_t * __restrict pwDesColumnStart = pwOutput + uColoumOffset;

    int_fast16_t iOriginY = iOriginHeight;
    do {
        uint32_t * __restrict pwSrcLine = pwOrigin;

        /* select a column in target buffer */
        uint32_t * __restrict pwDesColumn = pwDesColumnStart++;;

        int_fast16_t iOriginX = iOriginWidth;
        do {
            *pwDesColumn = *pwSrcLine++;
            pwDesColumn -= iOutputWidth;
        } while(--iOriginX);

        pwOrigin += iOriginWidth;
    } while(--iOriginY);

}

ARM_NONNULL(1,2,3)
arm_2d_pfb_t * __arm_2d_helper_pfb_rotate270_c8bit(  arm_2d_pfb_t *ptOrigin, 
                                                    arm_2d_pfb_t *ptScratch,
                                                    const arm_2d_size_t *ptScreenSize)
{
    assert(NULL != ptOrigin);
    assert(NULL != ptScratch);

    uint8_t * __restrict pchOrigin = (uint8_t * __restrict)ptOrigin->tTile.pchBuffer;
    uint8_t * __restrict pchOutput = (uint8_t * __restrict)ptScratch->tTile.pchBuffer;

    int16_t iWidth = ptOrigin->tTile.tRegion.tSize.iWidth;
    int16_t iHeight = ptOrigin->tTile.tRegion.tSize.iHeight;

    __arm_2d_rotate_270_c8bit(pchOrigin, pchOutput, iWidth, iHeight, iHeight);

    *ptScratch = *ptOrigin;
    ptScratch->tTile.pchBuffer = (uint8_t *)pchOutput;
    ptScratch->ptNext = NULL;

    ptScratch->tTile.tRegion.tSize.iWidth = iHeight;
    ptScratch->tTile.tRegion.tSize.iHeight = iWidth;

    arm_2d_location_t tNewLocation = {
        ptOrigin->tTile.tRegion.tLocation.iY,
        ptScreenSize->iWidth - (ptOrigin->tTile.tRegion.tLocation.iX + iWidth - 1) - 1,
    };

    ptScratch->tTile.tRegion.tLocation = tNewLocation;

    return ptScratch;
}

ARM_NONNULL(1,2,3)
arm_2d_pfb_t * __arm_2d_helper_pfb_rotate270_rgb16(  arm_2d_pfb_t *ptOrigin, 
                                                    arm_2d_pfb_t *ptScratch,
                                                    const arm_2d_size_t *ptScreenSize)
{
    assert(NULL != ptOrigin);
    assert(NULL != ptScratch);

    uint16_t * __restrict phwOrigin = (uint16_t * __restrict)ptOrigin->tTile.phwBuffer;
    uint16_t * __restrict phwOutput = (uint16_t * __restrict)ptScratch->tTile.phwBuffer;

    int16_t iWidth = ptOrigin->tTile.tRegion.tSize.iWidth;
    int16_t iHeight = ptOrigin->tTile.tRegion.tSize.iHeight;

    __arm_2d_rotate_270_rgb16(phwOrigin, phwOutput, iWidth, iHeight, iHeight);

    *ptScratch = *ptOrigin;
    ptScratch->tTile.phwBuffer = (uint16_t *)phwOutput;
    ptScratch->ptNext = NULL;

    ptScratch->tTile.tRegion.tSize.iWidth = iHeight;
    ptScratch->tTile.tRegion.tSize.iHeight = iWidth;

    arm_2d_location_t tNewLocation = {
        ptOrigin->tTile.tRegion.tLocation.iY,
        ptScreenSize->iWidth - (ptOrigin->tTile.tRegion.tLocation.iX + iWidth - 1) - 1,
    };

    ptScratch->tTile.tRegion.tLocation = tNewLocation;

    return ptScratch;
}

ARM_NONNULL(1,2,3)
arm_2d_pfb_t * __arm_2d_helper_pfb_rotate270_rgb32(  arm_2d_pfb_t *ptOrigin, 
                                                    arm_2d_pfb_t *ptScratch,
                                                    const arm_2d_size_t *ptScreenSize)
{
    assert(NULL != ptOrigin);
    assert(NULL != ptScratch);

    uint32_t * __restrict pwOrigin = (uint32_t * __restrict)ptOrigin->tTile.pwBuffer;
    uint32_t * __restrict pwOutput = (uint32_t * __restrict)ptScratch->tTile.pwBuffer;

    int16_t iWidth = ptOrigin->tTile.tRegion.tSize.iWidth;
    int16_t iHeight = ptOrigin->tTile.tRegion.tSize.iHeight;

    __arm_2d_rotate_270_rgb32(pwOrigin, pwOutput, iWidth, iHeight, iHeight);

    *ptScratch = *ptOrigin;
    ptScratch->tTile.pwBuffer = (uint32_t *)pwOutput;
    ptScratch->ptNext = NULL;

    ptScratch->tTile.tRegion.tSize.iWidth = iHeight;
    ptScratch->tTile.tRegion.tSize.iHeight = iWidth;

    arm_2d_location_t tNewLocation = {
        ptOrigin->tTile.tRegion.tLocation.iY,
        ptScreenSize->iWidth - (ptOrigin->tTile.tRegion.tLocation.iX + iWidth - 1) - 1,
    };

    ptScratch->tTile.tRegion.tLocation = tNewLocation;

    return ptScratch;
}


/*----------------------------------------------------------------------------*
 * PFB Rotate Screen 180 PFB Helper                                           *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1,2,3)
arm_2d_pfb_t * __arm_2d_helper_pfb_rotate180_c8bit(  arm_2d_pfb_t *ptOrigin, 
                                                    arm_2d_pfb_t *ptScratch,
                                                    const arm_2d_size_t *ptScreenSize)
{
    assert(NULL != ptOrigin);
    assert(NULL != ptScratch);

    uint8_t * __restrict pchOrigin = (uint8_t * __restrict)ptOrigin->tTile.pchBuffer;
    uint8_t * __restrict pchOutput = (uint8_t * __restrict)ptScratch->tTile.pchBuffer;

    int16_t iWidth = ptOrigin->tTile.tRegion.tSize.iWidth;
    int16_t iHeight = ptOrigin->tTile.tRegion.tSize.iHeight;

    __arm_2d_impl_c8bit_copy_xy_mirror(
        pchOrigin,
        iWidth,
        pchOutput,
        iWidth,
        (arm_2d_size_t []){
            {iWidth, iHeight},
        }
    );

    *ptScratch = *ptOrigin;
    ptScratch->tTile.pchBuffer = (uint8_t *)pchOutput;
    ptScratch->ptNext = NULL;

    arm_2d_location_t tNewLocation = {
        ptScreenSize->iWidth - (ptOrigin->tTile.tRegion.tLocation.iX + iWidth - 1) - 1,
        ptScreenSize->iHeight - (ptOrigin->tTile.tRegion.tLocation.iY + iHeight - 1) - 1,
    };

    ptScratch->tTile.tRegion.tLocation = tNewLocation;

    return ptScratch;
}

ARM_NONNULL(1,2,3)
arm_2d_pfb_t * __arm_2d_helper_pfb_rotate180_rgb16(  arm_2d_pfb_t *ptOrigin, 
                                                    arm_2d_pfb_t *ptScratch,
                                                    const arm_2d_size_t *ptScreenSize)
{
    assert(NULL != ptOrigin);
    assert(NULL != ptScratch);

    uint16_t * __restrict phwOrigin = (uint16_t * __restrict)ptOrigin->tTile.phwBuffer;
    uint16_t * __restrict phwOutput = (uint16_t * __restrict)ptScratch->tTile.phwBuffer;

    int16_t iWidth = ptOrigin->tTile.tRegion.tSize.iWidth;
    int16_t iHeight = ptOrigin->tTile.tRegion.tSize.iHeight;

    __arm_2d_impl_rgb16_copy_xy_mirror(
        phwOrigin,
        iWidth,
        phwOutput,
        iWidth,
        (arm_2d_size_t []){
            {iWidth, iHeight},
        }
    );

    *ptScratch = *ptOrigin;
    ptScratch->tTile.phwBuffer = (uint16_t *)phwOutput;
    ptScratch->ptNext = NULL;

    arm_2d_location_t tNewLocation = {
        ptScreenSize->iWidth - (ptOrigin->tTile.tRegion.tLocation.iX + iWidth - 1) - 1,
        ptScreenSize->iHeight - (ptOrigin->tTile.tRegion.tLocation.iY + iHeight - 1) - 1,
    };

    ptScratch->tTile.tRegion.tLocation = tNewLocation;

    return ptScratch;
}

ARM_NONNULL(1,2,3)
arm_2d_pfb_t * __arm_2d_helper_pfb_rotate180_rgb32( arm_2d_pfb_t *ptOrigin, 
                                                    arm_2d_pfb_t *ptScratch,
                                                    const arm_2d_size_t *ptScreenSize)
{
    assert(NULL != ptOrigin);
    assert(NULL != ptScratch);

    uint32_t * __restrict pwOrigin = (uint32_t * __restrict)ptOrigin->tTile.pwBuffer;
    uint32_t * __restrict pwOutput = (uint32_t * __restrict)ptScratch->tTile.pwBuffer;

    int16_t iWidth = ptOrigin->tTile.tRegion.tSize.iWidth;
    int16_t iHeight = ptOrigin->tTile.tRegion.tSize.iHeight;

    __arm_2d_impl_rgb32_copy_xy_mirror(
        pwOrigin,
        iWidth,
        pwOutput,
        iWidth,
        (arm_2d_size_t []){
            {iWidth, iHeight},
        }
    );

    *ptScratch = *ptOrigin;
    ptScratch->tTile.pwBuffer = (uint32_t *)pwOutput;
    ptScratch->ptNext = NULL;

    arm_2d_location_t tNewLocation = {
        ptScreenSize->iWidth - (ptOrigin->tTile.tRegion.tLocation.iX + iWidth - 1) - 1,
        ptScreenSize->iHeight - (ptOrigin->tTile.tRegion.tLocation.iY + iHeight - 1) - 1,
    };

    ptScratch->tTile.tRegion.tLocation = tNewLocation;

    return ptScratch;
}

/*----------------------------------------------------------------------------*
 * API for Dirty Region List                                                  *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1)
bool arm_2d_dirty_region_item_ignore_set(arm_2d_region_list_item_t *ptThis, bool bIgnore)
{
    bool bOrigin = false;

    do {
        if (NULL == ptThis) {
            assert(false);
            break;
        }

        arm_irq_safe {
            bOrigin = this.bIgnore;
            this.bIgnore = !!bIgnore;
        }
    } while(0);

    return bOrigin;
}

ARM_NONNULL(1)
bool arm_2d_dirty_region_item_ignore_get(arm_2d_region_list_item_t *ptThis)
{
    bool bIgnore = false;

    do {
        if (NULL == ptThis) {
            assert(false);
            break;
        }

        bIgnore = this.bIgnore;
    } while(0);

    return bIgnore;
}

ARM_NONNULL(1,2)
bool arm_2d_helper_pfb_append_dirty_regions_to_list(
                                arm_2d_region_list_item_t **ppDirtyRegionList, 
                                arm_2d_region_list_item_t *ptItems,
                                size_t tCount)
{

    do {
        if (NULL == ptItems || 0 == tCount || NULL == ppDirtyRegionList) {
            break;
        }

        while(NULL != (*ppDirtyRegionList)) {
            ppDirtyRegionList = &((*ppDirtyRegionList)->ptNext);
        }

        /* add dirty region items to the list */
        (*ppDirtyRegionList) = ptItems;
        while(--tCount) {
            ptItems->ptNext = ptItems + 1;
            ptItems++;
        }
        ptItems->ptNext = NULL;

        return true;
    } while(0);

    return false;
}

ARM_NONNULL(1,2)
bool arm_2d_helper_pfb_remove_dirty_regions_from_list(
                                    arm_2d_region_list_item_t **ppDirtyRegionList, 
                                    arm_2d_region_list_item_t *ptItems,
                                    size_t tCount)
{

    do {
        if (NULL == ptItems || 0 == tCount || NULL == ppDirtyRegionList) {
            break;
        }

        do {
            /* search and remove item */

            while(NULL != (*ppDirtyRegionList)) {

                /* remove the dirty region from the user dirty region list */
                if ((*ppDirtyRegionList) == ptItems) {
                    (*ppDirtyRegionList) = ptItems->ptNext;
                    ptItems->ptNext = NULL;
                    break;
                }

                ppDirtyRegionList = &((*ppDirtyRegionList)->ptNext);
            }

            /* next item */
            ptItems++;
        } while(--tCount);

        return true;
    } while(0);

    return false;
}

arm_2d_region_list_item_t *arm_2d_dynamic_dirty_region_init(
                                arm_2d_region_list_item_t *ptThis)
{
    bool bFromHeap = false;
    if (NULL == ptThis) {
        /* allocate one dirty region from heap */
        arm_2d_region_list_item_t *ptThis 
            = (arm_2d_region_list_item_t *)__arm_2d_allocate_scratch_memory(
                    sizeof(arm_2d_region_list_item_t),
                    __alignof__(arm_2d_region_list_item_t),
                    ARM_2D_MEM_TYPE_UNSPECIFIED
                );
        bFromHeap = true;
    }

    do {
        if (NULL == ptThis) {
            break;
        }

        memset(ptThis, 0, sizeof(arm_2d_region_list_item_t));
        this.bFromHeap = bFromHeap;

        this.u12KEY = 0xACE;    /* set key */

    } while(0);

    return ptThis;
}


ARM_NONNULL(1)
void arm_2d_dynamic_dirty_region_depose(
                                arm_2d_region_list_item_t *ptThis)
{
    assert(NULL != ptThis);

    if (NULL == ptThis) {
        return ;
    }

    if (this.u12KEY != 0xACE) {
        /* invalid key */
        return ;
    }

    /* destroy the key */
    this.u12KEY = 0;

    /* free the memory */
    if (this.bFromHeap) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptThis);
    }
}

ARM_NONNULL(1)
void arm_2d_dynamic_dirty_region_on_frame_start(
                                            arm_2d_region_list_item_t *ptThis,
                                            uint8_t chUserRegionIndex)
{
    enum {
        START = 0,
        WAIT_HANDSHAKE,
        DONE,
    };

    if (NULL == ptThis) {
        assert(false);
        return ;
    }

    this.bIgnore = false;

    /* set the flag for handshaking with the PFB helper service */
    this.bUpdated = true;

    if (0xFF != chUserRegionIndex) {
        /* reset the user region index */
        this.chUserRegionIndex = chUserRegionIndex;
    }

    /* reset the state machine */
    this.u2UpdateState = START;
}

ARM_NONNULL(1)
uint_fast8_t arm_2d_dynamic_dirty_region_wait_next(
                                            arm_2d_region_list_item_t *ptThis)
{
    uint_fast8_t chUserRegionIndex = 0xFF;
    enum {
        START = 0,
        WAIT_HANDSHAKE,
        DONE,
    };

    do {
        if (NULL == ptThis) {
            assert(false);
            break ;
        }

        switch(this.u2UpdateState) {
            case START:
                /* return the next index for user */
                chUserRegionIndex = this.chUserRegionIndex;
                this.u2UpdateState = DONE;
                this.bIgnore = true;
                break;
            case WAIT_HANDSHAKE:
                if (!this.bUpdated) {
                    /* return the next index for user */
                    chUserRegionIndex = this.chUserRegionIndex;
                    this.u2UpdateState = DONE;
                    this.bIgnore = true;
                }
                break;
            default:
            case DONE:
                break;
        }
    } while(0);

    return chUserRegionIndex;
}

ARM_NONNULL(1)
void arm_2d_dynamic_dirty_region_change_user_region_index_only(
                                            arm_2d_region_list_item_t *ptThis,
                                            uint8_t chNextUserIndex)
{
    if (NULL == ptThis) {
        assert(false);
        return ;
    }

    if (0xFF == chNextUserIndex) {
        this.chUserRegionIndex = 0;
    } else {
        this.chUserRegionIndex = chNextUserIndex;
    }
}



ARM_NONNULL(1)
static
void __arm_2d_dynamic_dirty_region_update(  arm_2d_region_list_item_t *ptThis,
                                            arm_2d_tile_t *ptTarget,
                                            arm_2d_region_t *ptRegion,
                                            uint8_t chNextUserIndex,
                                            bool bIsAbsoluteRegion)
{
    enum {
        START = 0,
        WAIT_HANDSHAKE,
        DONE,
    };

    if (NULL == ptThis) {
        assert(false);
        return ;
    }

    this.bIgnore = false;
    this.bUpdated = true;

    /* use 0xFF as a DONE signal */
    if (0xFF == chNextUserIndex) {
        /* reset the user region index */
        this.chUserRegionIndex = 0;

        /* reset the state machine */
        this.u2UpdateState = DONE;

        return ;
    } 

    if (NULL == ptRegion) {
        assert(false);
        return ;
    }

    /* update region */
    this.tRegion = *ptRegion;

#if 0
    if (NULL == ptTarget) {
        ptTarget = arm_2d_get_default_frame_buffer();
    }
#endif

    if (NULL != ptTarget && !bIsAbsoluteRegion) {
        /* get the absolute region */
        this.tRegion.tLocation 
            = arm_2d_helper_pfb_get_absolute_location(  ptTarget, 
                                                        this.tRegion.tLocation);
    }

    this.chUserRegionIndex = chNextUserIndex;
    this.u2UpdateState = WAIT_HANDSHAKE;

}

void arm_2d_dynamic_dirty_region_update(  arm_2d_region_list_item_t *ptThis,
                                            arm_2d_tile_t *ptTarget,
                                            arm_2d_region_t *ptRegion,
                                            uint8_t chNextUserIndex)
{
    __arm_2d_dynamic_dirty_region_update(   ptThis, 
                                            ptTarget, 
                                            ptRegion, 
                                            chNextUserIndex, 
                                            false);
}

/*----------------------------------------------------------------------------*
 * Dirty Region Helper                                                        *
 *----------------------------------------------------------------------------*/

enum {
    DIRTY_REGION_HELPER_START = 0,
    DIRTY_REGION_HELEPR_CHECK_NEXT_ITEM,
    DIRTY_REGION_HELPER_UPDATE_NEW_REGION,
    DIRTY_REGION_HELPER_DONE,
};

ARM_NONNULL(1,2)
void arm_2d_helper_dirty_region_init(
                                arm_2d_helper_dirty_region_t *ptThis,
                                arm_2d_region_list_item_t **ppDirtyRegionList)
{
    assert(NULL != ptThis);
    assert(NULL != ppDirtyRegionList);
    
    memset(ptThis, 0, sizeof(arm_2d_helper_dirty_region_t));
    this.ppDirtyRegionList = ppDirtyRegionList;

    arm_2d_dynamic_dirty_region_init(&this.tDirtyRegion);

    this.tDefaultItem.bIgnore = true;

    arm_2d_helper_pfb_append_dirty_regions_to_list( ppDirtyRegionList, 
                                                    &this.tDirtyRegion, 
                                                    1);

}

ARM_NONNULL(1,2)
void arm_2d_helper_dirty_region_add_items(
                                arm_2d_helper_dirty_region_t *ptThis,
                                arm_2d_helper_dirty_region_item_t *ptItems,
                                uint_fast16_t hwCount)
{
    assert(NULL != ptThis);

    if (NULL == ptItems || 0 == hwCount) {
        return ;
    }

    arm_2d_helper_dirty_region_item_t **ppList = &this.tDefaultItem.ptNext;

    /* add items to the list */
    arm_2d_helper_dirty_region_item_t *ptItem = ptItems;
    do {
        memset(ptItem, 0, sizeof(arm_2d_helper_dirty_region_item_t));

        ptItem->ptHelper = ptThis;
        ptItem->ptNext = *ppList;
        ptItem->u16Key = 0xCAFE;        /* add key for security */
        ptItem->bIgnore = true;

        *ppList = ptItem;

        ptItem ++;
    } while(--hwCount);

}

ARM_NONNULL(1,2)
void arm_2d_helper_dirty_region_remove_items(
                                arm_2d_helper_dirty_region_t *ptThis,
                                arm_2d_helper_dirty_region_item_t *ptItems,
                                uint_fast16_t hwCount)
{
    assert(NULL != ptThis);

    if (NULL == ptItems || 0 == hwCount) {
        return ;
    }

    arm_2d_helper_dirty_region_item_t **pptItem = &this.tDefaultItem.ptNext;
    if (NULL == *pptItem) {
        /* nothing in the list */
        return ;
    }

    /* find the item in the list*/
    do {
        if (*pptItem == ptItems) {
            break;
        }

        pptItem = &((*pptItem)->ptNext);
    } while(NULL != *pptItem);

    if (NULL == *pptItem) {
        /* cannot find the ptItems in the list */
        return ;
    }
    
    /* remove an array of items */
    *pptItem = ptItems[hwCount - 1].ptNext;
    ptItems[hwCount].ptNext = NULL;
}

ARM_NONNULL(1)
arm_2d_helper_dirty_region_item_t * arm_2d_helper_dirty_region_depose(
                                        arm_2d_helper_dirty_region_t *ptThis)
{
    assert(NULL != ptThis);
    arm_2d_region_list_item_t **ppDirtyRegionList = this.ppDirtyRegionList;
    
    if (NULL != ppDirtyRegionList) {
        arm_2d_helper_pfb_remove_dirty_regions_from_list(   ppDirtyRegionList,
                                                            &this.tDirtyRegion,
                                                            1);
    }

    arm_2d_dynamic_dirty_region_depose(&this.tDirtyRegion);

    return this.tDefaultItem.ptNext;
}

ARM_NONNULL(1)
void arm_2d_helper_dirty_region_on_frame_start( 
                                        arm_2d_helper_dirty_region_t *ptThis)
{
    assert(NULL != ptThis);

    this.chUpdateLifeCycle++;
    this.ptCurrent = &this.tDefaultItem;

    arm_2d_dynamic_dirty_region_on_frame_start(&this.tDirtyRegion, 
                                                DIRTY_REGION_HELPER_START);
}


ARM_NONNULL(1,2,3)
void arm_2d_helper_dirty_region_update_item(
                                        arm_2d_helper_dirty_region_t *ptHelper,
                                        arm_2d_helper_dirty_region_item_t *ptThis,
                                        const arm_2d_tile_t *ptTargetTile,
                                        const arm_2d_region_t *ptVisibleArea,
                                        const arm_2d_region_t *ptNewRegion)
{
    assert(NULL != ptThis);
    assert(NULL != ptHelper);
    assert(NULL != ptTargetTile);

    if (ptHelper->chUpdateLifeCycle == this.chUpdateLifeCycle) {
        /* already updated */
        return ;
    }
    this.chUpdateLifeCycle = ptHelper->chUpdateLifeCycle;

    if (NULL == ptNewRegion) {
        this.bIgnore = true;

        return ;
    }
    this.bIgnore = false;
    this.bOnlyUpdateMinimalEnclosure = false;

    do {
        arm_2d_region_t tNewRegion = *ptNewRegion;

        /* apply region patch */
        do {
            /* note: tRegionPatch stores the patch values (deltas) */

            tNewRegion.tSize.iWidth += this.tRegionPatch.tSize.iWidth;
            tNewRegion.tSize.iHeight += this.tRegionPatch.tSize.iHeight;

            tNewRegion.tLocation.iX += this.tRegionPatch.tLocation.iX;
            tNewRegion.tLocation.iY += this.tRegionPatch.tLocation.iY;
        } while(0);

        //! get absolute location
        tNewRegion.tLocation = arm_2d_helper_pfb_get_absolute_location(
                                                (arm_2d_tile_t *)ptTargetTile, 
                                                tNewRegion.tLocation);
        if (NULL != ptVisibleArea) {

            arm_2d_region_t tTargetRegion = *ptVisibleArea;
            tTargetRegion.tLocation = arm_2d_helper_pfb_get_absolute_location(
                                                (arm_2d_tile_t *)ptTargetTile, 
                                                tTargetRegion.tLocation);

            if (!arm_2d_region_intersect(   &tNewRegion, 
                                            &tTargetRegion,
                                            &tNewRegion)) {
                this.bIgnore = true;
                break;
            }
        }

        /* keep the old region */
        this.tOldRegion = this.tNewRegion;

        /* update the new region */
        this.tNewRegion = tNewRegion;

        /* region optimization */
        arm_2d_region_t tOverlapArea, tEnclosureArea;
        if (this.bForceToUseMinimalEnclosure) {
            arm_2d_region_get_minimal_enclosure(&this.tNewRegion, 
                                                &this.tOldRegion,
                                                &tEnclosureArea);

            /* we only refresh the enclosure region to save time */
            this.tEnclosureArea = tEnclosureArea;
            this.bOnlyUpdateMinimalEnclosure = true;
            break;

        } else if (arm_2d_region_intersect( &this.tOldRegion, 
                                            &this.tNewRegion,
                                            &tOverlapArea)) {
            /* the new region overlaps with the old region */
            uint32_t wPixelsRegion0 = this.tOldRegion.tSize.iHeight
                                    * this.tOldRegion.tSize.iWidth;
            uint32_t wPixelsRegion1 = this.tNewRegion.tSize.iHeight
                                    * this.tNewRegion.tSize.iWidth;


            arm_2d_region_get_minimal_enclosure(&this.tNewRegion, 
                                                &this.tOldRegion,
                                                &tEnclosureArea);
            
            uint32_t wPixelsRegionEnclosure 
                = tEnclosureArea.tSize.iHeight
                * tEnclosureArea.tSize.iWidth;

            if (wPixelsRegionEnclosure 
            <   (wPixelsRegion0 + wPixelsRegion1)) {
                /* we only refresh the enclosure region to save time */
                this.tEnclosureArea = tEnclosureArea;
                this.bOnlyUpdateMinimalEnclosure = true;

                break;
            }
        }
    } while(0);

}

ARM_NONNULL(1,2)
void __arm_2d_helper_dirty_region_update_dirty_regions2(
                                        arm_2d_helper_dirty_region_t *ptThis,
                                        const arm_2d_tile_t *ptTargetTile,
                                        const arm_2d_region_t *ptVisibleArea,
                                        const arm_2d_region_t *ptNewRegion)
{
    assert(NULL != ptThis);
    assert(NULL != ptTargetTile);

    /* update the first item for simplicity */
    arm_2d_helper_dirty_region_update_item( ptThis, 
                                            &this.tDefaultItem, 
                                            ptTargetTile, 
                                            ptVisibleArea, 
                                            ptNewRegion);

    __arm_2d_helper_dirty_region_update_dirty_regions(ptThis, ptTargetTile);
}

ARM_NONNULL(1,2)
void __arm_2d_helper_dirty_region_update_dirty_regions(
                                        arm_2d_helper_dirty_region_t *ptThis,
                                        const arm_2d_tile_t *ptTargetTile)
{
    assert(NULL != ptThis);
    assert(NULL != ptTargetTile);

    switch(arm_2d_dynamic_dirty_region_wait_next(&this.tDirtyRegion)) {
        case DIRTY_REGION_HELPER_START:
        case DIRTY_REGION_HELEPR_CHECK_NEXT_ITEM:
            if (NULL != this.ptCurrent) {
                /* skip ignored or suspended items */
                do {
                    if (!this.ptCurrent->bIgnore 
                     && !this.ptCurrent->bSuspendUpdate) {
                        break;
                    }
                    this.ptCurrent = this.ptCurrent->ptNext;
                } while(NULL != this.ptCurrent);

                if (NULL == this.ptCurrent) {
                    arm_2d_dynamic_dirty_region_change_user_region_index_only(
                                                    &this.tDirtyRegion, 
                                                    DIRTY_REGION_HELPER_DONE);
                    break;
                }

                this.ptCurrent->bIgnore = true;

                if (this.ptCurrent->bOnlyUpdateMinimalEnclosure) {
                    /* we only update the minimal enclosure region */
                    __arm_2d_dynamic_dirty_region_update(
                                        &this.tDirtyRegion,
                                        NULL, 
                                        &this.ptCurrent->tEnclosureArea,
                                        DIRTY_REGION_HELEPR_CHECK_NEXT_ITEM,
                                        true);             
                    this.ptCurrent = this.ptCurrent->ptNext;                    /* move to next */
                } else {
                    __arm_2d_dynamic_dirty_region_update( 
                                        &this.tDirtyRegion,
                                        NULL, 
                                        &this.ptCurrent->tOldRegion,
                                        DIRTY_REGION_HELPER_UPDATE_NEW_REGION,
                                        true);
                }
            } else {
                arm_2d_dynamic_dirty_region_change_user_region_index_only(
                                                    &this.tDirtyRegion, 
                                                    DIRTY_REGION_HELPER_DONE);
            }
            break;
        case DIRTY_REGION_HELPER_UPDATE_NEW_REGION:
            __arm_2d_dynamic_dirty_region_update( &this.tDirtyRegion,
                                                NULL, 
                                                &this.ptCurrent->tNewRegion,
                                                DIRTY_REGION_HELEPR_CHECK_NEXT_ITEM,
                                                true);
            this.ptCurrent = this.ptCurrent->ptNext;                            /* move to next */

            break;
        case DIRTY_REGION_HELPER_DONE:
            break;
        default: /* 0xFF */
            break;
    }

}

ARM_NONNULL(1)
bool arm_2d_helper_dirty_region_item_force_to_use_minimal_enclosure(
                                        arm_2d_helper_dirty_region_item_t *ptThis,
                                        bool bEnable)
{
    bool bOrigin = false;
    assert(NULL != ptThis);

    arm_irq_safe {
        bOrigin = this.bForceToUseMinimalEnclosure;
        this.bForceToUseMinimalEnclosure = bEnable;
    }

    return bOrigin;
}

ARM_NONNULL(1)
bool arm_2d_helper_dirty_region_force_to_use_minimal_enclosure(
                                        arm_2d_helper_dirty_region_t *ptThis,
                                        bool bEnable)
{
    bool bOrigin = false;
    assert(NULL != ptThis);

    return arm_2d_helper_dirty_region_item_force_to_use_minimal_enclosure(
                                                            &this.tDefaultItem,
                                                            bEnable);
}

ARM_NONNULL(1)
bool arm_2d_helper_dirty_region_item_suspend_update(
                                        arm_2d_helper_dirty_region_item_t *ptThis,
                                        bool bEnable)
{
    bool bOrigin = false;
    assert(NULL != ptThis);

    arm_irq_safe {
        bOrigin = this.bSuspendUpdate;
        this.bSuspendUpdate = bEnable;
    }

    return bOrigin;
}

ARM_NONNULL(1)
bool arm_2d_helper_dirty_region_suspend_update(
                                        arm_2d_helper_dirty_region_t *ptThis,
                                        bool bEnable)
{
    bool bOrigin = false;
    assert(NULL != ptThis);

    return arm_2d_helper_dirty_region_item_suspend_update(  &this.tDefaultItem,
                                                            bEnable);
}

/*----------------------------------------------------------------------------*
 * Dirty Region Transform Helper                                              *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1,2,3)
void arm_2d_helper_dirty_region_transform_init(
                                  arm_2d_helper_dirty_region_transform_t *ptThis,
                                  arm_2d_helper_dirty_region_t *ptHelper,
                                  arm_2d_op_t *ptTransformOP,
                                  float fAngleStep,
                                  float fScaleStep)
{
    assert(NULL != ptThis);
    assert(NULL != ptTransformOP);
    assert(NULL != ptHelper);

    memset(ptThis, 0, sizeof(arm_2d_helper_dirty_region_transform_t));

    this.ptTransformOP = ptTransformOP;
    this.Angle.fStep = fAngleStep;
    this.Scale.fStep = fScaleStep;
    this.Scale.fValue = 1.0f;
    this.fScale = 1.0f;

    this.bNeedUpdate = true;
    this.ptHelper = ptHelper;

    arm_2d_helper_dirty_region_add_items(ptHelper, &this.tItem, 1);
}

ARM_NONNULL(1)
void arm_2d_helper_dirty_region_transform_depose(arm_2d_helper_dirty_region_transform_t *ptThis)
{
    assert(NULL != ptThis);

    arm_2d_helper_dirty_region_remove_items(this.ptHelper,
                                            &this.tItem, 
                                            1);
}

ARM_NONNULL(1)
void arm_2d_helper_dirty_region_transform_update(
                                    arm_2d_helper_dirty_region_transform_t *ptThis,
                                    const arm_2d_region_t *ptCanvas,
                                    bool bIsNewFrame)
{
    assert(NULL != ptThis);
    ARM_2D_UNUSED(ptCanvas);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_tile_t *ptTarget = (arm_2d_tile_t *)this.ptTransformOP->Target.ptTile;
    if (NULL == ptTarget) {
        return ;
    }

    assert(NULL != ptTarget->ptParent);

    if (NULL != ptCanvas) {
        arm_2d_region_t tCanvasInTarget = *ptCanvas;

        /* move canvas to the target tile (calculate its relative location 
         * in the target tile) 
         */
        tCanvasInTarget.tLocation.iX -= ptTarget->tRegion.tLocation.iX;
        tCanvasInTarget.tLocation.iY -= ptTarget->tRegion.tLocation.iY;
        
        arm_2d_helper_dirty_region_update_item(
                                        this.ptHelper,
                                        &this.tItem,
                                        ptTarget,
                                        &tCanvasInTarget,
                                        (this.ptTransformOP->Target.ptRegion));
    } else {
        arm_2d_helper_dirty_region_update_item(
                                        this.ptHelper,
                                        &this.tItem,
                                        ptTarget,
                                        NULL,
                                        (this.ptTransformOP->Target.ptRegion));
    }
}


ARM_NONNULL(1)
void arm_2d_helper_dirty_region_transform_on_frame_start(
                                arm_2d_helper_dirty_region_transform_t *ptThis)
{
    assert(NULL != ptThis);

    arm_2d_helper_dirty_region_item_suspend_update(&this.tItem, !this.bNeedUpdate);

    /* make it thread safe */
    arm_irq_safe {

        if (this.bNeedUpdate) {
            this.bNeedUpdate = false;
            this.fAngle = this.Angle.fValue;
            this.fScale = this.Scale.fValue;
        }
    }
}

ARM_NONNULL(1)
void arm_2d_helper_dirty_region_transform_force_update(
                                arm_2d_helper_dirty_region_transform_t *ptThis)
{
    assert(NULL != ptThis);
    this.bNeedUpdate = true;
}

ARM_NONNULL(1)
bool arm_2d_helper_dirty_region_transform_force_to_use_minimal_enclosure(
                                arm_2d_helper_dirty_region_transform_t *ptThis,
                                bool bEnable)
{
    assert(NULL != ptThis);

    return arm_2d_helper_dirty_region_item_force_to_use_minimal_enclosure(
                                                        &this.tItem,
                                                        bEnable);
}

ARM_NONNULL(1)
bool arm_2d_helper_dirty_region_transform_suspend_update(
                                arm_2d_helper_dirty_region_transform_t *ptThis,
                                bool bEnable)
{
    assert(NULL != ptThis);

    return arm_2d_helper_dirty_region_item_suspend_update(
                                                        &this.tItem,
                                                        bEnable);
}


ARM_NONNULL(1)
void arm_2d_helper_dirty_region_transform_update_value(  
                                arm_2d_helper_dirty_region_transform_t *ptThis,
                                float fAngle,
                                float fScale)
{
    assert(NULL != ptThis);
    
    float fDelta = 0.0f;

    arm_irq_safe {
        float fDelta = this.Angle.fValue - fAngle;
        fDelta = ABS(fDelta);

        if (fDelta >= this.Angle.fStep) {
            this.Angle.fValue = fAngle;
            this.bNeedUpdate = true;
        }
    }

    arm_irq_safe {
        fDelta = this.Scale.fValue - fScale;
        fDelta = ABS(fDelta);

        if (fDelta >= this.Scale.fStep) {
            this.Scale.fValue = fAngle;
            this.bNeedUpdate = true;
        }
    }
}



/*----------------------------------------------------------------------------*
 * Transform Helper (Deprecated)                                              *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1,2,5)
void arm_2d_helper_transform_init(arm_2d_helper_transform_t *ptThis,
                                  arm_2d_op_t *ptTransformOP,
                                  float fAngleStep,
                                  float fScaleStep,
                                  arm_2d_region_list_item_t **ppDirtyRegionList)
{
    assert(NULL != ptThis);
    assert(NULL != ptTransformOP);
    assert(NULL != ppDirtyRegionList);

    memset(ptThis, 0, sizeof(arm_2d_helper_transform_t));

    this.ptTransformOP = ptTransformOP;
    this.Angle.fStep = fAngleStep;
    this.Scale.fStep = fScaleStep;
    this.Scale.fValue = 1.0f;
    this.fScale = 1.0f;

    this.bNeedUpdate = true;

    arm_2d_helper_dirty_region_init(&this.tHelper, ppDirtyRegionList);
}

ARM_NONNULL(1)
void arm_2d_helper_transform_depose(arm_2d_helper_transform_t *ptThis)
{
    assert(NULL != ptThis);

    arm_2d_helper_dirty_region_depose(&this.tHelper);

}

ARM_NONNULL(1)
void arm_2d_helper_transform_update_dirty_regions(
                                    arm_2d_helper_transform_t *ptThis,
                                    const arm_2d_region_t *ptCanvas,
                                    bool bIsNewFrame)
{
    assert(NULL != ptThis);
    ARM_2D_UNUSED(ptCanvas);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_tile_t *ptTarget = (arm_2d_tile_t *)this.ptTransformOP->Target.ptTile;
    if (NULL == ptTarget) {
        return ;
    }

    assert(NULL != ptTarget->ptParent);

    if (NULL != ptCanvas) {
        arm_2d_region_t tCanvasInTarget = *ptCanvas;

        /* move canvas to the target tile (calculate its relative location in the target tile) */
        tCanvasInTarget.tLocation.iX -= ptTarget->tRegion.tLocation.iX;
        tCanvasInTarget.tLocation.iY -= ptTarget->tRegion.tLocation.iY;

        arm_2d_helper_dirty_region_update_dirty_regions(&this.tHelper,
                                                        ptTarget,
                                                        &tCanvasInTarget,
                                                        (this.ptTransformOP->Target.ptRegion));
    } else {
        arm_2d_helper_dirty_region_update_dirty_regions(&this.tHelper,
                                                        ptTarget,
                                                        NULL,
                                                        (this.ptTransformOP->Target.ptRegion));
    }
}


ARM_NONNULL(1)
void arm_2d_helper_transform_on_frame_begin(arm_2d_helper_transform_t *ptThis)
{
    assert(NULL != ptThis);

    /* make it thread safe */
    arm_irq_safe {
        if (this.bNeedUpdate) {
            this.bNeedUpdate = false;

            this.fAngle = this.Angle.fValue;
            this.fScale = this.Scale.fValue;

            arm_2d_helper_dirty_region_on_frame_start(&this.tHelper);
        }
    }
}

ARM_NONNULL(1)
void arm_2d_helper_transform_force_update(arm_2d_helper_transform_t *ptThis)
{
    assert(NULL != ptThis);
    this.bNeedUpdate = true;
}

ARM_NONNULL(1)
bool arm_2d_helper_transform_force_to_use_minimal_enclosure(
                                            arm_2d_helper_transform_t *ptThis,
                                            bool bEnable)
{
    assert(NULL != ptThis);

    return arm_2d_helper_dirty_region_force_to_use_minimal_enclosure(
                                                                &this.tHelper,
                                                                bEnable);
}

ARM_NONNULL(1)
bool arm_2d_helper_transform_suspend_update(
                                            arm_2d_helper_transform_t *ptThis,
                                            bool bEnable)
{
    assert(NULL != ptThis);

    return arm_2d_helper_dirty_region_suspend_update(&this.tHelper, bEnable);
}


ARM_NONNULL(1)
void arm_2d_helper_transform_update_value(  arm_2d_helper_transform_t *ptThis,
                                            float fAngle,
                                            float fScale)
{
    assert(NULL != ptThis);
    
    float fDelta = 0.0f;

    arm_irq_safe {
        float fDelta = this.Angle.fValue - fAngle;
        fDelta = ABS(fDelta);

        if (fDelta >= this.Angle.fStep) {
            this.Angle.fValue = fAngle;
            this.bNeedUpdate = true;
        }
    }

    arm_irq_safe {
        fDelta = this.Scale.fValue - fScale;
        fDelta = ABS(fDelta);

        if (fDelta >= this.Scale.fStep) {
            this.Scale.fValue = fAngle;
            this.bNeedUpdate = true;
        }
    }
}

/*----------------------------------------------------------------------------*
 * Misc                                                                       *
 *----------------------------------------------------------------------------*/

bool arm_2d_helper_pfb_is_region_being_drawing(
                                    const arm_2d_tile_t *ptTarget,
                                    const arm_2d_region_t *ptRegion,
                                    const arm_2d_tile_t **ppVirtualScreen)
{
    bool bResult = false;
    arm_2d_tile_t tTempTile;

    if (NULL == ptTarget || (-1 == (intptr_t)ptTarget)) {
        ptTarget = arm_2d_get_default_frame_buffer();
    }

    if (NULL != ptRegion && NULL != ptTarget) {
        ptTarget = arm_2d_tile_generate_child(ptTarget, ptRegion, &tTempTile, false);
    }

    do {
        if (NULL == ptTarget) {
            break;
        }

        if (NULL != __arm_2d_tile_get_virtual_screen_or_root(   
                                                    ptTarget, 
                                                    NULL, 
                                                    NULL, 
                                                    (const arm_2d_tile_t **)ppVirtualScreen, 
                                                    false)) {
            bResult = true;
        }
    } while(0);

    return bResult;
}

bool arm_2d_helper_pfb_is_region_active(const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        bool bConsiderDryRun)
{
    const arm_2d_tile_t *ptScreen = NULL;

    bool bResult = arm_2d_helper_pfb_is_region_being_drawing(   ptTarget, 
                                                                ptRegion, 
                                                                &ptScreen);

    do {
        if (bResult) {
            break;
        }

        if (NULL == ptScreen) {
            break;
        }

        if ((ptScreen->tInfo.u3ExtensionID != ARM_2D_TILE_EXTENSION_PFB)) {
            break;
        }
        if (!ptScreen->tInfo.Extension.PFB.bIsNewFrame) {
            if (!(bConsiderDryRun && ptScreen->tInfo.Extension.PFB.bIsDryRun)) {
                break;
            }
        }

        bResult = true;

    } while(0);


    return bResult;
}

