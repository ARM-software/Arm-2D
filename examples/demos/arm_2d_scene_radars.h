/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
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

#ifndef __ARM_2D_SCENE_RADARS_H__
#define __ARM_2D_SCENE_RADARS_H__

/*============================ INCLUDES ======================================*/

#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include "arm_2d_helper.h"
#include "arm_2d_example_controls.h"

#if defined(RTE_Acceleration_Arm_2D_Extra_Loader)
#   include "arm_2d_example_loaders.h"
#endif

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wpadded"
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wpadded"
#endif

/*============================ MACROS ========================================*/

#ifndef ARM_2D_DEMO_RADAR_COLOUR
#   define ARM_2D_DEMO_RADAR_COLOUR                     GLCD_COLOR_NIXIE_TUBE
#endif

#ifndef ARM_2D_DEMO_RADAR_SCAN_SECTOR_COLOUR
#   define ARM_2D_DEMO_RADAR_SCAN_SECTOR_COLOUR         GLCD_COLOR_RED
#endif

#ifndef ARM_2D_DEMO_RADAR_BOGEY_COLOUR
#   define ARM_2D_DEMO_RADAR_BOGEY_COLOUR               ARM_2D_DEMO_RADAR_SCAN_SECTOR_COLOUR
#endif

#ifndef ARM_2D_DEMO_RADAR_SHOW_ANIMATION
#   define ARM_2D_DEMO_RADAR_SHOW_ANIMATION             0
#endif

#ifndef ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION
#   define ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION      0
#endif

#ifndef ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION
#   define ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION 0
#endif

/* NOTE: Please do NOT modify the following macro validation 
 */

#if !defined(RTE_Acceleration_Arm_2D_Extra_QOI_Loader)
#   undef ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION
#   define ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION      0
#endif

#if !defined(RTE_Acceleration_Arm_2D_Extra_zhRGB565_Loader)
#   undef ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION
#   define ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION 0
#endif

#if !defined(ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION)                           \
 && !ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION                                    \
 && !ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION
#   define ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION      1
#endif

#if !defined(RTE_Acceleration_Arm_2D_Extra_ZJpgDec_Loader)
#   undef ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION
#   define ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION      0
#endif


#if ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION
#   undef ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION
#   undef ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION
#   define ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION      0
#   define ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION 0
#endif

#if ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION
#   undef ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION
#   undef ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION
#   define ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION      0
#   define ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION 0
#endif

#if ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION
#   undef ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION
#   undef ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION
#   define ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION      0
#   define ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION      0
#endif

#if !ARM_2D_DEMO_RADAR_SHOW_ANIMATION
#   undef ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION
#   undef ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION
#   undef ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION
#   define ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION      0
#   define ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION      0
#   define ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION 0
#endif

/* OOC header, please DO NOT modify  */
#ifdef __USER_SCENE_RADARS_IMPLEMENT__
#   define __ARM_2D_IMPL__
#endif
#ifdef __USER_SCENE_RADARS_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"

/*============================ MACROFIED FUNCTIONS ===========================*/

/*!
 * \brief initalize scene_radars and add it to a user specified scene player
 * \param[in] __DISP_ADAPTER_PTR the target display adapter (i.e. scene player)
 * \param[in] ... this is an optional parameter. When it is NULL, a new 
 *            user_scene_radars_t will be allocated from HEAP and freed on
 *            the deposing event. When it is non-NULL, the life-cycle is managed
 *            by user.
 * \return user_scene_radars_t* the user_scene_radars_t instance
 */
#define arm_2d_scene_radars_init(__DISP_ADAPTER_PTR, ...)                    \
            __arm_2d_scene_radars_init((__DISP_ADAPTER_PTR), (NULL, ##__VA_ARGS__))

/*============================ TYPES =========================================*/

enum {
    FILM_IDX_TOP_LEFT,
    FILM_IDX_BOTTOM_RIGHT,
    __FILM_COUNT,
};

typedef struct __radar_bogey_t {
    implement(dynamic_nebula_particle_t);
    int16_t iAngle;
    int16_t iNewAngle;
    int16_t iDistance;

    uint8_t chOpacity;
    uint8_t u2State                 : 2;
    uint8_t u2NextState             : 2;
    uint8_t bAllowUpdateLocation    : 1;
    uint8_t bIsLocationUpdated      : 1;
    uint8_t                         : 2;

    arm_2d_location_t tDetectedPos;

    arm_2d_helper_dirty_region_item_t tDirtyRegionItem;
} __radar_bogey_t;

/*!
 * \brief a user class for scene radars
 */
typedef struct user_scene_radars_t user_scene_radars_t;

struct user_scene_radars_t {
    implement(arm_2d_scene_t);                                                  //! derived from class: arm_2d_scene_t

ARM_PRIVATE(
    /* place your private member here, following two are examples */
#if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
    int64_t lTimestamp[4];
#else
    int64_t lTimestamp[2];
#endif
    bool bUserAllocated;
    uint8_t chPT;
    uint8_t chRadarIndex;

    spin_zoom_widget_t tScanSector;

    dynamic_nebula_t    tNebula;
    __radar_bogey_t     tBogeys[6];

    foldable_panel_t    tScreen;

#if ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION \
||  ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION \
|| ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION
    struct {
    #if ARM_2D_DEMO_RADAR_USE_JPG_FOR_ANIMATION
        arm_zjpgd_loader_t tLoader;
    #elif ARM_2D_DEMO_RADAR_USE_QOI_FOR_ANIMATION
        arm_qoi_loader_t tLoader;
    #elif ARM_2D_DEMO_RADAR_USE_ZHRGB565_FOR_ANIMATION
        arm_zhrgb565_loader_t tLoader;
    #endif
        union {
            arm_loader_io_file_t tFile;
            arm_loader_io_binary_t tBinary;
            arm_loader_io_rom_t tROM;
        } LoaderIO;
    } tAnimation[__FILM_COUNT];
#endif

#if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
    struct {
        arm_2d_helper_film_t    tHelper;
        spin_zoom_widget_t      tSector;
    } tFilm[__FILM_COUNT];
#endif

)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

ARM_NONNULL(1)
extern
user_scene_radars_t *__arm_2d_scene_radars_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_radars_t *ptScene);

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#undef __USER_SCENE_RADARS_IMPLEMENT__
#undef __USER_SCENE_RADARS_INHERIT__

#ifdef   __cplusplus
}
#endif

#endif

#endif
