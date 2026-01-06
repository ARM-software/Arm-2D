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

#ifndef __WAVEFORM_VIEW_H__
#define __WAVEFORM_VIEW_H__

/*============================ INCLUDES ======================================*/
#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB) && defined(RTE_Acceleration_Arm_2D_Extra_Loader)

#include "arm_2d_helper.h"
#include "arm_2d_example_loaders.h"

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
#ifdef __WAVEFORM_VIEW_IMPLEMENT__
#   undef   __WAVEFORM_VIEW_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__WAVEFORM_VIEW_INHERIT__)
#   undef   __WAVEFORM_VIEW_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    WAVEFORM_SAMPLE_SIZE_BYTE = 0,
    WAVEFORM_SAMPLE_SIZE_HWORD,
    WAVEFORM_SAMPLE_SIZE_WORD,
};

typedef struct waveform_view_dirty_bin_t {
ARM_PRIVATE(
    struct {
        int16_t iY0;
        int16_t iY1;
    } Coverage[2];
    int16_t iWidth;
)
} waveform_view_dirty_bin_t;

typedef struct waveform_view_cfg_t {

    arm_2d_size_t tSize;

    uint16_t bUseHeapForVRES        : 1;
    uint16_t bShowShadow            : 1;
    uint16_t bUseDirtyRegion        : 1;
    uint16_t bUnsigned              : 1;
    uint16_t u2SampleSize           : 2;    /* WAVEFORM_SAMPLE_SIZE_xxxx */
    uint16_t                        : 2;
    uint16_t u5DotHeight            : 5;
    uint16_t __bFullUpdateReq       : 1;    /* please ignore this */
    uint16_t __bShowGradient        : 1;    /* please ignore this */
    uint16_t __bValid               : 1;    /* please ignore this */
    
    uint8_t                         : 8;
    uint8_t chDirtyRegionItemCount;
    waveform_view_dirty_bin_t *ptDirtyBins; 

    COLOUR_TYPE_T tBrushColour;
    COLOUR_TYPE_T tBackgroundColour;
    
    struct {
        const arm_loader_io_t *ptIO;
        uintptr_t pTarget;
    } IO;

    struct {
        int32_t nUpperLimit;
        int32_t nLowerLimit;
    } ChartScale;

    struct {
        COLOUR_TYPE_T tColour;
        arm_2d_alpha_samples_4pts_t tGradient;
    } Shadow;

    arm_2d_scene_t *ptScene;
} waveform_view_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct waveform_view_t waveform_view_t;

struct waveform_view_t {

    union {
        arm_2d_tile_t tTile;
        inherit(arm_generic_loader_t);
    };

ARM_PRIVATE(
    waveform_view_cfg_t tCFG;
    q16_t q16Scale;
    int16_t iDiagramHeight;
    int16_t iStartYOffset;

    struct {
        arm_2d_region_list_item_t tDirtyRegionItem;
        uint8_t chCurrentBin; 
        q16_t   q16BinWidth;
    } DirtyRegion;

    struct {
        q16_t q16UpperGradientRatio;
        q16_t q16LowerGradientRatio;
    } Shadow;

)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1, 2)
arm_2d_err_t waveform_view_init(waveform_view_t *ptThis,
                                waveform_view_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void waveform_view_depose( waveform_view_t *ptThis);

extern
ARM_NONNULL(1)
void waveform_view_on_load( waveform_view_t *ptThis);

extern
ARM_NONNULL(1)
void waveform_view_on_frame_start( waveform_view_t *ptThis, bool bUpdate);

extern
ARM_NONNULL(1)
void waveform_view_on_frame_complete( waveform_view_t *ptThis);

extern
ARM_NONNULL(1,2)
void waveform_view_show(waveform_view_t *ptThis,
                        const arm_2d_tile_t *ptTile,
                        const arm_2d_region_t *ptRegion,
                        bool bIsNewFrame);

extern
ARM_NONNULL(1)
arm_2d_err_t waveform_view_update_chart_scale(  waveform_view_t *ptThis,
                                                int32_t nUpperLimie, 
                                                int32_t nLowerLimit);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#else

#define waveform_view_init(...)                 ARM_2D_ERR_NOT_AVAILABLE
#define waveform_view_depose(...)
#define waveform_view_on_load(...)
#define waveform_view_on_frame_start(...)
#define waveform_view_on_frame_complete(...)

#endif 

#endif
