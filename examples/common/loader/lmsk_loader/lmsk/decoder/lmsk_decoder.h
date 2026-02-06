/*
 * Copyright (c) 2009-2026 Arm Limited. All rights reserved.
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
#ifndef __LMSK_DECODER_H__
#define __LMSK_DECODER_H__   1

#ifdef __ARM_LMSK_DECODER_CFG__
#   include __ARM_LMSK_DECODER_CFG__
#else
#   include "arm_lmsk_cfg.h"
#endif


#include "../__lmsk_common.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct arm_lmsk_decoder_cfg_t {
    struct {
        bool (*fnSeek)(     uintptr_t pTarget, int32_t offset);
        size_t (*fnRead) (  intptr_t pTarget,      
                            uint8_t *pchBuffer,
                            size_t tLength);
        uintptr_t pTarget;
    } IO;
} arm_lmsk_decoder_cfg_t;

typedef struct arm_lmsk_decoder_t {
    arm_lmsk_decoder_cfg_t tCFG;

    arm_lsmk_setting_t tSetting;
    uint8_t chPalette[32];

    uint32_t bValid         : 1;

    uint8_t chLastFloorIndex;
    uint16_t hwCurrentFloor;
    uint16_t hwNextFloor;
    uint32_t wFloorStart;

} arm_lmsk_decoder_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

extern
int arm_lmsk_decoder_init(  arm_lmsk_decoder_t *ptThis, 
                            arm_lmsk_decoder_cfg_t *ptCFG);


extern
int arm_lmsk_decode(arm_lmsk_decoder_t *ptThis,
                    int16_t iX,
                    int16_t iY,
                    int16_t iWidth,
                    int16_t iHeight,
                    uint8_t *pchBuffer,
                    uint32_t wTargetStride);


#ifdef   __cplusplus
}
#endif

#endif

