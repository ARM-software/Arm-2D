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

#ifndef __ARM_2D_SCENE_HISTOGRAM_H__
#define __ARM_2D_SCENE_HISTOGRAM_H__

/*============================ INCLUDES ======================================*/

#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include "arm_2d.h"

#include "arm_2d_helper_scene.h"
#include "arm_2d_example_controls.h"

#if defined(RTE_Acceleration_Arm_2D_Extra_TJpgDec_Loader)
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

/* OOC header, please DO NOT modify  */
#ifdef __USER_SCENE_HISTOGRAM_IMPLEMENT__
#   undef __USER_SCENE_HISTOGRAM_IMPLEMENT__
#   define __ARM_2D_IMPL__
#endif
#include "arm_2d_utils.h"


#ifndef ARM_2D_SCENE_HISTOGRAM_USE_JPG
#   define ARM_2D_SCENE_HISTOGRAM_USE_JPG       0
#endif

#if !defined(RTE_Acceleration_Arm_2D_Extra_TJpgDec_Loader)
#   undef  ARM_2D_SCENE_HISTOGRAM_USE_JPG
#   define ARM_2D_SCENE_HISTOGRAM_USE_JPG       0
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/

/*!
 * \brief initalize scene_histogram and add it to a user specified scene player
 * \param[in] __DISP_ADAPTER_PTR the target display adapter (i.e. scene player)
 * \param[in] ... this is an optional parameter. When it is NULL, a new 
 *            user_scene_histogram_t will be allocated from HEAP and freed on
 *            the deposing event. When it is non-NULL, the life-cycle is managed
 *            by user.
 * \return user_scene_histogram_t* the user_scene_histogram_t instance
 */
#define arm_2d_scene_histogram_init(__DISP_ADAPTER_PTR, ...)                    \
            __arm_2d_scene_histogram_init((__DISP_ADAPTER_PTR), (NULL, ##__VA_ARGS__))

/*============================ TYPES =========================================*/
/*!
 * \brief a user class for scene histogram
 */
typedef struct user_scene_histogram_t user_scene_histogram_t;

struct user_scene_histogram_t {
    implement(arm_2d_scene_t);                                                  //! derived from class: arm_2d_scene_t

ARM_PRIVATE(
    /* place your private member here, following two are examples */
    int64_t lTimestamp[2];
    uint8_t bUserAllocated                      : 1;
    uint8_t bIsDirtyRegionOptimizationEnabled   : 1;

    histogram_t tHistogram;
    histogram_bin_item_t tBins[14];

    struct {
        int16_t iBuffer[14];
        uint16_t hwPointer; 
    } WindowFIFO; 

#if ARM_2D_SCENE_HISTOGRAM_USE_JPG
    arm_tjpgd_loader_t tJPGBackground;
    union {
        arm_tjpgd_io_file_loader_t tFile;
        arm_tjpgd_io_binary_loader_t tBinary;
    } LoaderIO;
#endif

)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
user_scene_histogram_t *__arm_2d_scene_histogram_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_histogram_t *ptScene);

extern
ARM_NONNULL(1)
void user_scene_histogram_enqueue_new_value(user_scene_histogram_t *ptThis, 
                                            int_fast16_t iValue);
#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif

#endif
