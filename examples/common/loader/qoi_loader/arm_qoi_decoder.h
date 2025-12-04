/*
 * Copyright (c) 2009-2025 Arm Limited. All rights reserved.
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

#ifndef __ARM_QOI_DECODER_H__
#define __ARM_QOI_DECODER_H__

/*============================ INCLUDES ======================================*/

#ifdef __ARM_QOI_DECODER_CFG__
#   include __ARM_QOI_DECODER_CFG__
#else
#   include "arm_qoi_cfg.h"
#endif

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    ARM_QOI_DEC_FORMAT_GRAY8,
    ARM_QOI_DEC_FORMAT_MASK_ONLY,
    ARM_QOI_DEC_FORMAT_RGB565,
    ARM_QOI_DEC_FORMAT_CCCA8888,
};

typedef struct arm_qoi_dec_ctx_t {

} arm_qoi_dec_ctx_t;

typedef struct arm_qoi_dec_t {

    arm_2d_size_t tSize;

    uintptr_t pTarget;
} arm_qoi_dec_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef   __cplusplus
}
#endif

#endif
