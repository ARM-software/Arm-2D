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

#ifndef __CRT_SCREEN_H__
#define __CRT_SCREEN_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__common.h"
#include "./image_box.h"

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
#ifdef __CRT_SCREEN_IMPLEMENT__
#   undef   __CRT_SCREEN_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__CRT_SCREEN_INHERIT__)
#   undef   __CRT_SCREEN_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


typedef struct crt_screen_cfg_t {
    arm_2d_scene_t *ptScene;

    image_box_cfg_t *ptImageBoxCFG;

    COLOUR_TYPE_T tScanBarColour;
    uint8_t bShowScanningEffect     : 1;
    uint8_t bStrongNoise            : 1;
    uint8_t                         : 6;
    uint8_t chWhiteNoiseRatio;
    uint8_t chNoiseLasts;

    struct {
        uint8_t chBarHeight;
        uint8_t chOpacity;
        uint16_t hwPeriodInMs;
    }ScanBar[2];

} crt_screen_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct crt_screen_t crt_screen_t;

struct crt_screen_t {

    implement(image_box_t);

ARM_PRIVATE(

    crt_screen_cfg_t tCRTCFG;
    arm_2d_tile_t tWhiteNoise;

    struct {
        int64_t lTimestamp;
        int16_t iYOffset;
        arm_2d_helper_dirty_region_item_t tDirtyRegionItem;
    } ScanBar[2];
    uint8_t chWhiteNoiseVisibleFrameCounter;

)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1,2)
void crt_screen_init(   crt_screen_t *ptThis,
                        crt_screen_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void crt_screen_depose( crt_screen_t *ptThis);

extern
ARM_NONNULL(1)
void crt_screen_on_load( crt_screen_t *ptThis);

extern
ARM_NONNULL(1)
void crt_screen_on_frame_start( crt_screen_t *ptThis);

extern
ARM_NONNULL(1)
void crt_screen_on_frame_complete( crt_screen_t *ptThis);

extern
ARM_NONNULL(1)
void crt_screen_show(   crt_screen_t *ptThis,
                        const arm_2d_tile_t *ptTile, 
                        const arm_2d_region_t *ptRegion,
                        uint8_t chOpacity,
                        bool bIsNewFrame);

extern
ARM_NONNULL(1)
bool crt_screen_is_showing_noise( crt_screen_t *ptThis);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
