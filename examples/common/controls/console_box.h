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

#ifndef __CONSOLE_BOX_H__
#define __CONSOLE_BOX_H__

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
#ifdef __CONSOLE_BOX_IMPLEMENT__
#   undef   __CONSOLE_BOX_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__CONSOLE_BOX_INHERIT__)
#   undef   __CONSOLE_BOX_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


typedef struct console_box_cfg_t {
    arm_2d_size_t tBoxSize;
    struct {
        char *chFIFOBuffer;
        size_t tSize;
    } FIFO;
    const arm_2d_font_t *ptFont;
} console_box_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct console_box_t console_box_t;

struct console_box_t {

ARM_PRIVATE(
    console_box_cfg_t tCFG;
)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void console_box_init( console_box_t *ptThis,
                          console_box_cfg_t *ptCFG);

extern
ARM_NONNULL(1)
void console_box_depose( console_box_t *ptThis);

extern
ARM_NONNULL(1)
void console_box_show( console_box_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion, 
                            bool bIsNewFrame,
                            uint8_t chOpacity);


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
