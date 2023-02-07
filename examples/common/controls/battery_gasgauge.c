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

#define arm_2d_pixel_from_brga8888  arm_2d_pixel_brga8888_to_gray8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#define arm_2d_pixel_from_brga8888  arm_2d_pixel_brga8888_to_rgb565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#define arm_2d_pixel_from_brga8888

#else
#   error Unsupported colour depth!
#endif


#define __NIXIE_BOARDER_OPA_MAX         (255 - 64)

#define __LIQUID_BOARDER_OPA_MAX        (128)

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

extern
const arm_2d_tile_t c_tileSinWaveMask;

extern 
const arm_2d_tile_t c_tileGlassReflectionWMask;

extern
const arm_2d_tile_t c_tileGlassReflectionNMask;

extern
const arm_2d_tile_t c_tileLightingA4Mask;

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
    hwGasgauge = MIN(1000, hwGasgauge);

    if (bIsNewFrame){
        this.hwGasGauge = hwGasgauge;
        this.tStatus = tStatus;
    }

    if (bIsNewFrame) {
        if (BATTERY_STATUS_CHARGING == this.tStatus) {
            this.chBoarderOpacity = __NIXIE_BOARDER_OPA_MAX;
            this.bBoarderFlashing = false;
            
            int32_t iResult;
            arm_2d_helper_time_cos_slider(0, __NIXIE_BOARDER_OPA_MAX, 2000, 0, &iResult, &this.lTimeStamp);
            this.chBarOpacity = (uint8_t)iResult;
            this.bFlashingBar = true;
        } else {
            this.bFlashingBar = false;

            /* smaller than 10% */
            if (this.hwGasGauge < 100 
            &&  this.tStatus == BATTERY_STATUS_DISCHARGING) {
                int32_t iResult;
                arm_2d_helper_time_cos_slider(__NIXIE_BOARDER_OPA_MAX, 0, 2000, 0, &iResult, &this.lTimeStamp);
                this.chBoarderOpacity = (uint8_t)iResult;
                this.bBoarderFlashing = true;
            } else {
                this.lTimeStamp = 0;
                this.chBoarderOpacity = __NIXIE_BOARDER_OPA_MAX;
                this.bBoarderFlashing = false;
            }
        }
    }

    arm_2d_container(ptTile, __battery, ptRegion) {

        /* draw battery boarder */
        arm_2d_align_centre(__battery_canvas,
                            c_tileBatteryBoarder1Mask.tRegion.tSize) {

            arm_2d_fill_colour_with_mask_and_opacity(   
                                            &__battery,
                                            &__centre_region,
                                            &c_tileBatteryBoarder1Mask,
                                            (__arm_2d_color_t){GLCD_COLOR_NIXIE_TUBE},
                                            this.chBoarderOpacity);
            arm_2d_op_wait_async(NULL);
        }

        /* draw gas gauge grade*/
        do {
            arm_2d_size_t tInnerSize 
                = c_tileBatteryGasGaugeGradeBoarderMask.tRegion.tSize;
            tInnerSize.iHeight = 80;
            uint16_t  hwLevel = 1000;
            bool bDrawTheTopBar = true;
            arm_2d_align_centre( __battery_canvas, tInnerSize) {
                for (int n = 0; n < 5; n++) {
                    uint8_t chOpacity = 0;
                    hwLevel -= 200;
                    if (this.hwGasGauge > hwLevel) {
                        if (BATTERY_STATUS_CHARGING == this.tStatus) {
                            chOpacity = __NIXIE_BOARDER_OPA_MAX;
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
                                            &__battery,
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
            arm_2d_align_centre( __battery_canvas, tInnerSize) {
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
                                            &__battery,
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
    
    this.iWaveOffset[0] = -c_tileSinWaveMask.tRegion.tSize.iWidth;
    this.iWaveOffset[1] = -(c_tileSinWaveMask.tRegion.tSize.iWidth >> 1);
}

ARM_NONNULL(1)
void battery_gasgauge_liquid_show(  battery_liquid_t *ptThis,
                                    const arm_2d_tile_t *ptTile,
                                    const arm_2d_region_t *ptRegion,
                                    uint16_t hwGasgauge,
                                    battery_status_t tStatus,
                                    bool bIsNewFrame)
{
    hwGasgauge = MIN(1000, hwGasgauge);

    if (bIsNewFrame){
        this.hwGasGauge = hwGasgauge;
        this.tStatus = tStatus;

        if (arm_2d_helper_is_time_out(10, &this.lTimeStamp[1])) {
            if (this.iWaveOffset[0] == 0) {
                this.iWaveOffset[0] = -c_tileSinWaveMask.tRegion.tSize.iWidth + 1;
            } else {
                this.iWaveOffset[0]++;
            }
            
            if (this.iWaveOffset[1] == -c_tileSinWaveMask.tRegion.tSize.iWidth) {
                this.iWaveOffset[1] = -1;
            } else {
                this.iWaveOffset[1]--;
            }
        }
    }

    
    /* calculate the colour */
    COLOUR_INT tColour;

    switch (this.tStatus) {
        case BATTERY_STATUS_CHARGING:
            if (this.hwGasGauge > 800) {
                tColour = arm_2d_pixel_from_brga8888(
                                            __arm_2d_helper_colour_slider(
                                                GLCD_COLOR_NIXIE_TUBE_RGB32, 
                                                __RGB32(0, 0xFF, 0),
                                                200,
                                                this.hwGasGauge - 800));
            } else {
                tColour = GLCD_COLOR_NIXIE_TUBE;
            }
            break;
        case BATTERY_STATUS_IDLE:
            tColour = GLCD_COLOR_LIGHT_GREY;
            break;
        case BATTERY_STATUS_DISCHARGING:
            tColour = arm_2d_pixel_from_brga8888( 
                                            __arm_2d_helper_colour_slider(
                                                __RGB32(0xFF, 0, 0), 
                                                __RGB32(0, 0xFF, 0),
                                                1000,
                                                this.hwGasGauge));
        break;
    }

    if (bIsNewFrame) {
        if (BATTERY_STATUS_CHARGING == this.tStatus) {
            this.chBoarderOpacity = __LIQUID_BOARDER_OPA_MAX;
            this.bBoarderFlashing = false;
            
            int32_t iResult;
            arm_2d_helper_time_cos_slider(  0,
                                            255 - 64,
                                            2000,
                                            0,
                                            &iResult,
                                            &this.lTimeStamp[0]);
            this.chChargingMarkOpacity = (uint8_t)iResult;
        } else {
            /* smaller than 10% */
            if (this.hwGasGauge < 100 
            &&  this.tStatus == BATTERY_STATUS_DISCHARGING) { 
                int32_t iResult;
                arm_2d_helper_time_cos_slider(  __LIQUID_BOARDER_OPA_MAX,
                                                0,
                                                2000,
                                                0,
                                                &iResult,
                                                &this.lTimeStamp[0]);
                this.chBoarderOpacity = (uint8_t)iResult;
                this.bBoarderFlashing = true;
            } else {
                this.lTimeStamp[0] = 0;
                this.chBoarderOpacity = __LIQUID_BOARDER_OPA_MAX;
                this.bBoarderFlashing = false;
            }
        }
    }
    
    arm_2d_container(ptTile, __battery, ptRegion) {
        /* draw battery boarder */
        arm_2d_align_centre(__battery_canvas,
                            c_tileBatteryBoarder1Mask.tRegion.tSize) {

            arm_2d_fill_colour_with_mask_and_opacity(   
                                            &__battery,
                                            &__centre_region,
                                            &c_tileBatteryBoarder1Mask,
                                            (__arm_2d_color_t){GLCD_COLOR_WHITE},
                                            this.chBoarderOpacity);

            arm_2d_container(&__battery, __inner_container, &__centre_region,
                             //8,9,16,10
                             .chLeft = 9,
                             .chRight = 9,
                             .chTop = 16,
                             .chBottom = 10
                             ) {

                arm_2d_region_t tWaterLevelRegion = __inner_container_canvas;
                /* calculate the water level */
                tWaterLevelRegion.tLocation.iY 
                    += tWaterLevelRegion.tSize.iHeight 
                    -  tWaterLevelRegion.tSize.iHeight 
                    *  this.hwGasGauge / 1000;

                /* draw background wave */
                do {
                    arm_2d_region_t tWaveRegion = tWaterLevelRegion;
                    tWaveRegion.tLocation.iY 
                        = tWaterLevelRegion.tLocation.iY
                        - c_tileSinWaveMask.tRegion.tSize.iHeight;
                    tWaveRegion.tLocation.iX = this.iWaveOffset[1];
                    tWaveRegion.tSize = c_tileSinWaveMask.tRegion.tSize;
                    
                    arm_2d_fill_colour_with_mask_and_opacity(
                            &__inner_container,
                            &tWaveRegion,
                            &c_tileSinWaveMask,
                            (__arm_2d_color_t) {tColour},
                            96
                        );
                    arm_2d_op_wait_async(NULL);
                    
                    tWaveRegion.tLocation.iX 
                        += c_tileSinWaveMask.tRegion.tSize.iWidth;

                    arm_2d_fill_colour_with_mask_and_opacity(
                            &__inner_container,
                            &tWaveRegion,
                            &c_tileSinWaveMask,
                            (__arm_2d_color_t) {tColour},
                            96
                        );
                    arm_2d_op_wait_async(NULL);
                } while(0);

                /* draw foreground wave */
                do {
                    arm_2d_region_t tWaveRegion = tWaterLevelRegion;
                    tWaveRegion.tLocation.iY 
                        = tWaterLevelRegion.tLocation.iY
                        - c_tileSinWaveMask.tRegion.tSize.iHeight;
                    tWaveRegion.tLocation.iX = this.iWaveOffset[0];
                    tWaveRegion.tSize = c_tileSinWaveMask.tRegion.tSize;
                    
                    arm_2d_fill_colour_with_mask_and_opacity(
                            &__inner_container,
                            &tWaveRegion,
                            &c_tileSinWaveMask,
                            (__arm_2d_color_t) {tColour},
                            255
                        );
                    arm_2d_op_wait_async(NULL);
                    
                    tWaveRegion.tLocation.iX 
                        += c_tileSinWaveMask.tRegion.tSize.iWidth;

                    arm_2d_fill_colour_with_mask_and_opacity(
                            &__inner_container,
                            &tWaveRegion,
                            &c_tileSinWaveMask,
                            (__arm_2d_color_t) {tColour},
                            255
                        );
                    arm_2d_op_wait_async(NULL);
                } while(0);

                arm_2d_fill_colour_with_opacity(
                                &__inner_container, 
                                &tWaterLevelRegion,
                                (__arm_2d_color_t) {tColour},
                                255);
                arm_2d_op_wait_async(NULL);
             
                if (BATTERY_STATUS_CHARGING == this.tStatus) {
                    arm_2d_align_centre(__inner_container_canvas, 
                                        c_tileLightingA4Mask.tRegion.tSize) {
                        arm_2d_fill_colour_with_a4_mask_and_opacity(
                            &__inner_container,
                            &__centre_region,
                            &c_tileLightingA4Mask,
                            (__arm_2d_color_t){GLCD_COLOR_YELLOW},
                            this.chChargingMarkOpacity);
                        arm_2d_op_wait_async(NULL);
                    }
                }
            }

            arm_2d_container(&__battery, __glass_face, &__centre_region,
                            8, 8, 12, 8
                            ) {

                /* draw the glass reflection (narrow) on the left side */
                arm_2d_align_left(  __glass_face_canvas, 
                                    c_tileGlassReflectionNMask.tRegion.tSize) {
                    arm_2d_fill_colour_with_mask_and_opacity(
                        &__glass_face,
                        &__left_region,
                        &c_tileGlassReflectionNMask,
                        (__arm_2d_color_t){GLCD_COLOR_WHITE},
                        128 );

                    arm_2d_op_wait_async(NULL);
                }
                
                /* draw the glass reflection (wide) on the right side */
                arm_2d_align_right( __glass_face_canvas, 
                                    c_tileGlassReflectionWMask.tRegion.tSize) {
                    arm_2d_fill_colour_with_mask_and_opacity(
                        &__glass_face,
                        &__right_region,
                        &c_tileGlassReflectionWMask,
                        (__arm_2d_color_t){GLCD_COLOR_WHITE},
                        128 + 32);

                    arm_2d_op_wait_async(NULL);
                }
            }
        }
    }

}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
