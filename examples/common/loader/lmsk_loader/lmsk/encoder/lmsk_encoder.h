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
#ifndef __LMSK_ENCODER_H__
#define __LMSK_ENCODER_H__   1

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../__lmsk_common.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef MAX
#   define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#   define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

#ifndef ABS
#   define ABS(x) ((x) > 0 ? (x) : -(x))
#endif

/*! 
 * \brief get the number of items in an given array
 */
#ifndef dimof
#   define dimof(__array)          (sizeof(__array)/sizeof(__array[0]))
#endif

/*============================ TYPES =========================================*/

typedef struct __arm_lmsk_line_out_t {
    struct __arm_lmsk_line_out_t *ptNext;
    struct __arm_lmsk_line_out_t *ptPrev;
    uint8_t *pchBuffer;
    uint8_t *pchSourceLine;
    size_t tSize;
    uint32_t wPosition;
    uint32_t wCRC;
} __arm_lmsk_line_out_t;

typedef struct __arm_lmsk_line_in_t {
    struct __arm_lmsk_line_in_t *ptNext;
    uint8_t *pchBuffer;
} __arm_lmsk_line_in_t;

typedef struct __arm_lmsk_output_t {
    arm_lmsk_header_t tHeader;

    uint8_t chPalette[32];

    struct {
        __arm_lmsk_line_out_t *ptHead;
        __arm_lmsk_line_out_t *ptTail;
    } Lines;

    uint32_t wDataSize;

    struct {
        uint32_t *pwReferences;
    } tLineIndexTable;

} __arm_lmsk_output_t;

typedef struct arm_lmsk_encoder_t {
    __arm_lmsk_output_t tOutput;

    struct {
        uint8_t *pchBuffer;
        int16_t iWidth;
        int16_t iHeight;
    } Mask;

    uint32_t wPosition;

} arm_lmsk_encoder_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

extern
arm_lmsk_encoder_t * arm_lmsk_encoder_init( arm_lmsk_encoder_t *ptThis, 
                                            uint8_t *pchMaskBuffer, 
                                            int16_t iWidth, 
                                            int16_t iHeight);

extern
__arm_lmsk_output_t *arm_lmsk_encode(   arm_lmsk_encoder_t *ptThis, 
                                        uint8_t chAlphaBits);

extern
int arm_lmsk_write_to_file(__arm_lmsk_output_t *ptThis, FILE *ptOut);

extern 
arm_lmsk_encoder_t * arm_lmsk_encoder_depose(arm_lmsk_encoder_t *ptThis);

#ifdef   __cplusplus
}
#endif

#endif

