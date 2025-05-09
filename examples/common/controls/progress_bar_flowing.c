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

/*============================ INCLUDES ======================================*/
#include "./progress_bar_flowing.h"
#include "./__common.h"
#include "arm_2d.h"

#include <assert.h>
#include "arm_2d_helper.h"

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
#endif

/*============================ MACROS ========================================*/

#ifndef PROGRESS_BAR_WAVE_SPEED
#   define PROGRESS_BAR_WAVE_SPEED     15
#endif

#ifndef PROGRESS_BAR_REPEAT_INTERVAL
#   define PROGRESS_BAR_REPEAT_INTERVAL 3000
#endif

#define MASK_HEIGHT   20

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

extern const arm_2d_tile_t c_tileWaveLineMask;

ARM_NOINIT static int64_t s_lLastTime;
ARM_NOINIT static int64_t s_lWavingTime;
ARM_NOINIT static uint32_t s_wUnit;
static volatile bool s_bWaving = false;

/*============================ IMPLEMENTATION ================================*/


void progress_bar_flowing_init(void)
{
    s_lLastTime = arm_2d_helper_get_system_timestamp();
    s_wUnit = (uint32_t)arm_2d_helper_convert_ms_to_ticks(PROGRESS_BAR_WAVE_SPEED);
    s_lWavingTime = s_lLastTime + arm_2d_helper_convert_ms_to_ticks(3000);
}

void __progress_bar_flowing_show(   const arm_2d_tile_t *ptTarget,
                                    const arm_2d_region_t *ptRegion,
                                    int_fast16_t iProgress,
                                    bool bIsNewFrame, 
                                    COLOUR_INT tBarColour,
                                    COLOUR_INT tPitchColour,
                                    COLOUR_INT tBoarderColour)
{
    static int16_t s_iOffset = 0;
 
    if (-1 == (intptr_t)ptTarget) {
        ptTarget = arm_2d_get_default_frame_buffer();
    }

    assert(NULL != ptTarget);
    if (iProgress > 1000) {
        iProgress = 0;
    }
 
    int_fast16_t iWidth = 0;
    int_fast16_t iHeight;
    arm_2d_region_t tTargetRegion = {.tSize = ptTarget->tRegion.tSize,};
    if (NULL == ptRegion) {
        ptRegion = &tTargetRegion;
        iWidth = ptRegion->tSize.iWidth * 3 >> 3;         //!< 3/8 Width
        iHeight = MASK_HEIGHT;
    } else {
        iWidth = ptRegion->tSize.iWidth;
        iHeight = ptRegion->tSize.iHeight;
    }

    arm_2d_region_t tBarRegion = {
        .tLocation = {
           .iX = ptRegion->tLocation.iX + (ptRegion->tSize.iWidth - (int16_t)iWidth) / 2,
           .iY = ptRegion->tLocation.iY + (ptRegion->tSize.iHeight - (int16_t)iHeight) / (int16_t)2,
        },
        .tSize = {
            .iWidth = (int16_t)iWidth,
            .iHeight = (int16_t)iHeight,
        },
    };

    //! update offset
    if (bIsNewFrame) {
        int64_t lClocks = arm_2d_helper_get_system_timestamp();
        int32_t nElapsed = (int32_t)((lClocks - s_lLastTime));

        if (lClocks >= s_lWavingTime) {
            s_lWavingTime = lClocks + arm_2d_helper_convert_ms_to_ticks(PROGRESS_BAR_REPEAT_INTERVAL);
            s_bWaving = true;
        }

        if (nElapsed >= (int32_t)s_wUnit) {
            s_lLastTime = lClocks;
            
            if (s_bWaving) {
                s_iOffset+=2;
                if (s_iOffset >= tBarRegion.tSize.iWidth + c_tileWaveLineMask.tRegion.tSize.iWidth) {
                    s_iOffset = 0;
                    s_bWaving = false;
                }
            }
        }
    }

    if (!arm_2d_helper_pfb_is_region_being_drawing(ptTarget, &tBarRegion, NULL)) {
        return ;
    }
    
    //! draw a white box
    arm_2d_helper_draw_box(ptTarget, &tBarRegion, 1, tBoarderColour, 255);

    ARM_2D_OP_WAIT_ASYNC();
    
    //! pave inter texture
    tBarRegion.tSize.iHeight-=2;
    tBarRegion.tSize.iWidth-=2;
    tBarRegion.tLocation.iX += 1;
    tBarRegion.tLocation.iY += 1;

    if (iProgress >= 0) {
        //! calculate the width of the inner stripe 
        tBarRegion.tSize.iWidth = tBarRegion.tSize.iWidth * (int16_t)iProgress / 1000;
    } 
    
    //! draw the inner stripe
    arm_2d_fill_colour( ptTarget, 
                        &tBarRegion, 
                        //__RGB(0x94, 0xd2, 0x52) /*0x968A */
                        tBarColour
                        );
    ARM_2D_OP_WAIT_ASYNC();
    //! draw wave
    do {
        
        arm_2d_region_t tInnerRegion = {
            .tSize = { 
                .iWidth = c_tileWaveLineMask.tRegion.tSize.iWidth,
                .iHeight = iHeight,
            },
            .tLocation = {
                .iX = - c_tileWaveLineMask.tRegion.tSize.iWidth + s_iOffset,
            },
        };
        arm_2d_tile_t tileInnerSlot;
    
        //! generate a child tile for texture paving
        arm_2d_tile_generate_child(ptTarget, &tBarRegion, &tileInnerSlot, false);

        arm_2d_fill_colour_with_horizontal_line_mask(   &tileInnerSlot,
                                                        &tInnerRegion,
                                                        &c_tileWaveLineMask,
                                                        (__arm_2d_color_t) {tPitchColour});
        
        ARM_2D_OP_WAIT_ASYNC();

    } while(0);
    
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
