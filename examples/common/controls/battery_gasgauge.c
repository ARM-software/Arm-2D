/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
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
#define __BATTERY_GASGAUGE_IMPLEMENT__

#include "./battery_gasgauge.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
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
#   pragma clang diagnostic ignored "-Wgnu-statement-expression-from-macro-expansion"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif


#define __BOARDER_OPA_MAX       (255 - 64)

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/


extern
const arm_2d_tile_t c_tileBatteryBoarder1Mask;

extern
const arm_2d_tile_t c_tileBatteryGasGaugeGradeBoarderMask;

extern
const arm_2d_tile_t c_tileBatteryGasGaugeBlockMask;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1)
void battery_gasgauge_nixie_tube_init(battery_nixie_tube_t *ptThis)
{
    assert(NULL != ptThis);
    memset(ptThis, 0, sizeof(battery_nixie_tube_t));
}

ARM_NONNULL(1)
void battery_gasgauge_nixie_tube_show(  battery_nixie_tube_t *ptThis,
                                        const arm_2d_tile_t *ptTile,
                                        const arm_2d_region_t *ptRegion,
                                        uint16_t hwGasgauge,
                                        battery_status_t tStatus,
                                        bool bIsNewFrame)
{
    arm_2d_region_t tDrawRegion = {0};
    arm_2d_tile_t tDrawTile;
    hwGasgauge = MIN(1000, hwGasgauge);

    if (bIsNewFrame){
        this.hwGasGauge = hwGasgauge;
        this.tStatus = tStatus;
    }

    if (NULL == ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
        if (NULL == ptTile) {
            return ;
        }
    }

    if (NULL == ptRegion) {
        tDrawRegion.tSize = ptTile->tRegion.tSize;
        ptRegion = (const arm_2d_region_t *)&tDrawRegion;
    }

    ptTile = arm_2d_tile_generate_child(ptTile, ptRegion, &tDrawTile, false);
    if (NULL == ptTile) {
        return ;
    }
    
    if (bIsNewFrame) {
        if (BATTERY_STATUS_CHARGING == this.tStatus) {
            this.chBoarderOpacity = __BOARDER_OPA_MAX;
            this.bBoarderFlashing = false;
            
            int32_t iResult;
            arm_2d_helper_time_cos_slider(0, __BOARDER_OPA_MAX, 2000, 0, &iResult, &this.lTimeStamp);
            this.chBarOpacity = (uint8_t)iResult;
            this.bFlashingBar = true;
        } else {
            this.bFlashingBar = false;

            /* smaller than 10% */
            if (this.hwGasGauge < 100) { 
                int32_t iResult;
                arm_2d_helper_time_cos_slider(__BOARDER_OPA_MAX, 0, 2000, 0, &iResult, &this.lTimeStamp);
                this.chBoarderOpacity = (uint8_t)iResult;
                this.bBoarderFlashing = true;
            } else {
                this.lTimeStamp = 0;
                this.chBoarderOpacity = __BOARDER_OPA_MAX;
                this.bBoarderFlashing = false;
            }
        }
    }

    arm_2d_canvas(ptTile, __canvas) {
        /* draw battery boarder */
        arm_2d_align_centre( __canvas, c_tileBatteryBoarder1Mask.tRegion.tSize) {

            arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTile,
                                            &__centre_region,
                                            &c_tileBatteryBoarder1Mask,
                                            (__arm_2d_color_t){GLCD_COLOR_NIXIE_TUBE},
                                            this.chBoarderOpacity);
        }

        /* draw gas gauge grade*/
        do {
            arm_2d_size_t tInnerSize 
                = c_tileBatteryGasGaugeGradeBoarderMask.tRegion.tSize;
            tInnerSize.iHeight = 80;
            uint16_t  hwLevel = 1000;
            bool bDrawTheTopBar = true;
            arm_2d_align_centre( __canvas, tInnerSize) {
                for (int n = 0; n < 5; n++) {
                    uint8_t chOpacity = 0;
                    hwLevel -= 200;
                    if (this.hwGasGauge > hwLevel) {
                        if (BATTERY_STATUS_CHARGING == this.tStatus) {
                            chOpacity = __BOARDER_OPA_MAX;
                        } else {
                            chOpacity = (uint8_t)MIN(255, this.hwGasGauge -  hwLevel);
                        }
                        if (bDrawTheTopBar) {
                            bDrawTheTopBar = false;
                            if (this.bFlashingBar) {
                                chOpacity = MIN(this.chBarOpacity, chOpacity);
                            }
                        }
                    }
                    
                    if (this.bBoarderFlashing) {
                        chOpacity = MIN(this.chBoarderOpacity, chOpacity);
                    }
                    
                    arm_2d_fill_colour_with_mask_and_opacity(
                                            ptTile,
                                            &__centre_region,
                                            &c_tileBatteryGasGaugeGradeBoarderMask,
                                            (__arm_2d_color_t){GLCD_COLOR_NIXIE_TUBE},
                                            chOpacity);
                    
                    arm_2d_op_wait_async(NULL);
                    __centre_region.tLocation.iY += 16;
                    
                }
            }
        } while(0);

        /* draw gas gauge block*/
        do {
            arm_2d_size_t tInnerSize 
                = c_tileBatteryGasGaugeBlockMask.tRegion.tSize;
            tInnerSize.iHeight = 80;
            uint16_t  hwLevel = 1000;
            arm_2d_align_centre( __canvas, tInnerSize) {
                for (int n = 0; n < 5; n++) {
                    uint8_t chOpacity = 0;
                    hwLevel -= 200;

                    if (this.hwGasGauge > hwLevel) {
                        chOpacity = (uint8_t)MIN(255, this.hwGasGauge -  hwLevel);
                    }
                    
                    if (this.bBoarderFlashing) {
                        chOpacity = MIN(this.chBoarderOpacity, chOpacity);
                    }

                    __centre_region.tLocation.iY += 4;
                    
                    arm_2d_fill_colour_with_mask_and_opacity(
                                            ptTile,
                                            &__centre_region,
                                            &c_tileBatteryGasGaugeBlockMask,
                                            (__arm_2d_color_t){GLCD_COLOR_NIXIE_TUBE},
                                            chOpacity);
                    
                    arm_2d_op_wait_async(NULL);
                    __centre_region.tLocation.iY += 12;

                }
            }
        } while(0);
    }
}



ARM_NONNULL(1)
void battery_gasgauge_liquid_init(battery_liquid_t *ptThis)
{
    assert(NULL != ptThis);
    memset(ptThis, 0, sizeof(battery_liquid_t));
}

ARM_NONNULL(1)
void battery_gasgauge_liquid_show(  battery_liquid_t *ptThis,
                                    const arm_2d_tile_t *ptTile,
                                    const arm_2d_region_t *ptRegion,
                                    uint16_t hwGasgauge,
                                    battery_status_t tStatus,
                                    bool bIsNewFrame)
{
    arm_2d_region_t tDrawRegion = {0};
    arm_2d_tile_t tDrawTile;
    hwGasgauge = MIN(1000, hwGasgauge);

    if (bIsNewFrame){
        this.hwGasGauge = hwGasgauge;
        this.tStatus = tStatus;
    }

    if (NULL == ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
        if (NULL == ptTile) {
            return ;
        }
    }

    if (NULL == ptRegion) {
        tDrawRegion.tSize = ptTile->tRegion.tSize;
        ptRegion = (const arm_2d_region_t *)&tDrawRegion;
    }

    ptTile = arm_2d_tile_generate_child(ptTile, ptRegion, &tDrawTile, false);
    if (NULL == ptTile) {
        return ;
    }

    

}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
