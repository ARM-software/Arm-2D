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

typedef int32_t q16_t;

typedef struct __arm_lmsk_floor_context_t {
    int16_t iCurrent;
    int16_t iNext;
    uint32_t wFloorStart;
    uint8_t chLastFloorIndex;
} __arm_lmsk_floor_context_t;

typedef struct arm_lmsk_decoder_cfg_t {

    struct {
    #if __ARM_LMSK_USE_LOADER_IO__
        bool (*fnSeek)(     uintptr_t pTarget, int32_t offset);
        size_t (*fnRead) (  intptr_t pTarget,      
                            uint8_t *pchBuffer,
                            size_t tLength);
    #endif
        __arm_lmsk_floor_context_t *
               (*fnSearchFloorContext)( intptr_t pTarget, int16_t iY);
        void (*fnReportFloorContext)(   intptr_t pTarget, 
                                        const __arm_lmsk_floor_context_t *ptContext);
        uintptr_t pTarget;
    } IO;

#if !__ARM_LMSK_USE_LOADER_IO__
    const uint8_t *pchLMSKSource;
#endif
} arm_lmsk_decoder_cfg_t;



typedef struct arm_lmsk_decoder_t {
    arm_lmsk_decoder_cfg_t tCFG;

#if !__ARM_LMSK_USE_LOADER_IO__
    size_t nPosition;
#endif

    uint8_t chPalette[32];

    arm_lsmk_setting_t tSetting;
    uint16_t u15Repeat          : 15;
    uint16_t bValid             : 1;

    uint32_t wTagFetchBuffer;
    uint8_t chTagFetchByteLeft;
    uint8_t                     : 8;
    uint16_t                    : 16;

    __arm_lmsk_floor_context_t tFloorContext;

#if __ARM_LMSK_LSB_COMPENSATION_POLICY__ == 2
    q16_t   q16LSBCompensation;
#endif

    q16_t   q16Gradient;
    int16_t iGradientSteps;
    int16_t iGradientStepIndex;
    uint8_t chGradientFromAlpha;
    uint8_t chGradientToAlpha;

    uint8_t chPrevious;
    uint8_t chState;

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

