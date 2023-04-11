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

#ifndef __SHAPE_ROUND_CORNER_BOX_H__
#define __SHAPE_ROUND_CORNER_BOX_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#if __GLCD_CFG_COLOUR_DEPTH__ == 8
#   define COLOUR_INT               uint8_t
#elif __GLCD_CFG_COLOUR_DEPTH__ == 16
#   define COLOUR_INT               uint16_t
#elif __GLCD_CFG_COLOUR_DEPTH__ == 32
#   define COLOUR_INT               uint32_t
#else
#   error Unsupported colour depth!
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


extern
void draw_round_corner_box( const arm_2d_tile_t *ptTarget, 
                            const arm_2d_region_t *ptRegion,
                            COLOUR_INT tColour,
                            uint8_t chOpacity,
                            bool bIsNewFrame);


typedef struct {
    uint8_t chLeft;
    uint8_t chTop;
    uint8_t chRight;
    uint8_t chBottom;
} arm_2d_border_opacity_t;

typedef struct {
    uint8_t chTopLeft;
    uint8_t chTopRight;
    uint8_t chBottomLeft;
    uint8_t chBottomRight;
} arm_2d_corner_opacity_t;

extern
void draw_round_corner_border(  const arm_2d_tile_t *ptTarget,
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tColour,
                                arm_2d_border_opacity_t Opacity,
                                arm_2d_corner_opacity_t CornerOpacity);
#ifdef   __cplusplus
}
#endif

#endif
