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
#include "./app_cfg.h"
#include "./progress_bar_flowing.h"
#include "arm_2d.h"
#include "platform.h"

#if defined(__clang__)
#   pragma clang diagnostic push
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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern uint32_t SystemCoreClock;

/*============================ PROTOTYPES ====================================*/
__attribute__((nothrow)) 
extern int64_t clock(void);

/*============================ LOCAL VARIABLES ===============================*/
extern const uint8_t c_bmpWave[];
const arm_2d_tile_t c_tileWave = {
    .tRegion = {
        .tSize = {
            .iWidth = 34,
            .iHeight = 20
        },
    },
    .tInfo.bIsRoot = true,
    .phwBuffer = (uint16_t *)c_bmpWave,
};

ARM_NOINIT static int64_t s_lLastTime;
ARM_NOINIT static uint32_t s_wUnit;
static volatile bool s_bWaving = false;

/*============================ IMPLEMENTATION ================================*/


void progress_bar_flowing_init(void)
{
    s_lLastTime = clock();
    s_wUnit = (SystemCoreClock  / 1000) * PROGRESS_BAR_WAVE_SPEED;
}

void progress_bar_flowing_insert_1ms_handler(void)
{
    static uint_fast16_t s_hwCounter = 0;
    s_hwCounter++;
    if (s_hwCounter >= 3000) {      //!< every 2 sec
        s_hwCounter = 0;
        s_bWaving = true;
    }
}


void progress_bar_flowing_show(const arm_2d_tile_t *ptTarget, int_fast16_t iProgress)
{
    int_fast16_t iWidth = ptTarget->tRegion.tSize.iWidth * 3 >> 3;         //!< 3/8 Width
 
    ASSERT(NULL != ptTarget);
    ASSERT(iProgress <= 1000);
 
    arm_2d_region_t tBarRegion = {
        .tLocation = {
           .iX = (ptTarget->tRegion.tSize.iWidth - (int16_t)iWidth) / 2,
           .iY = (ptTarget->tRegion.tSize.iHeight - c_tileWave.tRegion.tSize.iHeight) / (int16_t)2,
        },
        .tSize = {
            .iWidth = (int16_t)iWidth,
            .iHeight = c_tileWave.tRegion.tSize.iHeight,
        },
    };
    
    //! draw a white box
    arm_2d_rgb16_fill_colour(ptTarget, &tBarRegion, 0xA63D);
    
    //! pave inter texture
    tBarRegion.tSize.iHeight-=2;
    tBarRegion.tSize.iWidth-=2;
    tBarRegion.tLocation.iX += 1;
    tBarRegion.tLocation.iY += 1;
    
    arm_2d_rgb16_fill_colour(ptTarget, &tBarRegion, GLCD_COLOR_WHITE);

    if (iProgress > 0) {
        //! calculate the width of the inner stripe 
        tBarRegion.tSize.iWidth = tBarRegion.tSize.iWidth * (int16_t)iProgress / 1000;
    } 
    
    //! draw the inner stripe
    arm_2d_rgb16_fill_colour(ptTarget, &tBarRegion, 0x968A);
    
    //! draw wave
    do {
        static int16_t s_iOffset = 0;
        arm_2d_region_t tInnerRegion = {
            .tSize = c_tileWave.tRegion.tSize,
            .tLocation = {
                .iX = - c_tileWave.tRegion.tSize.iWidth + s_iOffset,
            },
        };
        arm_2d_tile_t tileInnerSlot;
    
        //! generate a child tile for texture paving
        arm_2d_tile_generate_child(ptTarget, &tBarRegion, &tileInnerSlot, false);
    
        
        arm_2d_rgb16_tile_copy( &c_tileWave, 
                                &tileInnerSlot, 
                                &tInnerRegion, 
                                ARM_2D_CP_MODE_COPY);
        
        //! update offset
        do {
            int64_t lClocks = clock();
            int32_t nElapsed = (int32_t)((lClocks - s_lLastTime));
            
            if (nElapsed >= (int32_t)s_wUnit) {
                s_lLastTime = lClocks;
                
                if (s_bWaving) {
                    s_iOffset+=2;
                    if (s_iOffset >= tBarRegion.tSize.iWidth + c_tileWave.tRegion.tSize.iWidth) {
                        s_iOffset = 0;
                        s_bWaving = false;
                    }
                }
            }
        } while(0);
        

    } while(0);
    
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
