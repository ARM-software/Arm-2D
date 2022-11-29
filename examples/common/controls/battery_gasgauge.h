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

#ifndef __BATTERY_GASGAUGE_H__
#define __BATTERY_GASGAUGE_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__common.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/*============================ MACROS ========================================*/

/* OOC header, please DO NOT modify  */
#ifdef __BATTERY_GASGAUGE_IMPLEMENT__
#   undef   __BATTERY_GASGAUGE_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__BATTERY_GASGAUGE_INHERIT__)
#   undef   __BATTERY_GASGAUGE_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/

#define GLCD_COLOR_NIXIE_TUBE       __RGB(0xFF, 0xA5, 0x00)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    BATTERY_STATUS_DISCHARGING = -1,
    BATTERY_STATUS_IDLE = 0,
    BATTERY_STATUS_CHARGING = 1,
} battery_status_t;

typedef struct battery_nixie_tube_t {
ARM_PRIVATE(
    int64_t lTimeStamp;

    uint16_t hwGasGauge;
    uint8_t chBoarderOpacity;
    uint8_t chBarOpacity;
    
    uint8_t bBoarderFlashing    : 1;
    uint8_t bFlashingBar        : 1;
    
    battery_status_t tStatus;
)
} battery_nixie_tube_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
void battery_gasgauge_nixie_tube_init(battery_nixie_tube_t *ptThis);


extern
ARM_NONNULL(1,2)
void battery_gasgauge_nixie_tube_show(  battery_nixie_tube_t *ptThis,
                                        const arm_2d_tile_t *ptTile, 
                                        const arm_2d_region_t *ptRegion,
                                        uint16_t hwGasgauge,
                                        battery_status_t tStatus,
                                        bool bIsNewFrame);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
