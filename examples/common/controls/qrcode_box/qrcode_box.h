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

#ifndef __QRCODE_BOX_H__
#define __QRCODE_BOX_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__common.h"
#include "./qrcodegen.h"

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
#ifdef __QRCODE_BOX_IMPLEMENT__
#   undef   __QRCODE_BOX_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__QRCODE_BOX_INHERIT__)
#   undef   __QRCODE_BOX_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


typedef struct qrcode_box_cfg_t {
    union {
        const char *pchString;
        const uint8_t *pchBinary;
    };
    uint16_t hwInputSize;
    
    uint16_t u6Version              : 6;    /*!< 1 ~ 40, 0 means auto */
    uint16_t u2ECCLevel             : 2;
    uint16_t bIsString              : 1;    /*!< string or binary */
    uint16_t                        : 5;
    uint16_t bIsValid               : 1;    /*!< please ignore this flag */
    uint16_t bScratchMemoryInHeap   : 1;    /*!< please ignore this flag */

    uint8_t *pchBuffer;             /*!< passing NULL means using heap to allocate memory */
    uint16_t hwQRCodeBufferSize;    /*!< at least qrcodegen_BUFFER_LEN_FOR_VERSION(version) * 2*/

    uint8_t chSquarePixelSize;

    
} qrcode_box_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct qrcode_box_t qrcode_box_t;

struct qrcode_box_t {
ARM_PRIVATE(
    qrcode_box_cfg_t tCFG;
    int16_t iQRCodePixelSize;
)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
arm_2d_err_t qrcode_box_init(   qrcode_box_t *ptThis,
                                qrcode_box_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void qrcode_box_depose( qrcode_box_t *ptThis);

extern
ARM_NONNULL(1)
void qrcode_box_on_load( qrcode_box_t *ptThis);

extern
ARM_NONNULL(1)
int16_t qrcode_box_get_size(qrcode_box_t *ptThis);

extern 
ARM_NONNULL(1)
void qrcode_box_show(   qrcode_box_t *ptThis,
                        const arm_2d_tile_t *ptTile, 
                        const arm_2d_region_t *ptRegion, 
                        COLOUR_INT tColour,
                        uint8_t chOpacity);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
