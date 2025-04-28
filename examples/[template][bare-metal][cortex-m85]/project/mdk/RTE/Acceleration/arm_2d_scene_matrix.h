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

#ifndef __ARM_2D_SCENE_MATRIX_H__
#define __ARM_2D_SCENE_MATRIX_H__

/*============================ INCLUDES ======================================*/

#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include "arm_2d_helper.h"
#include "arm_2d_example_controls.h"

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
#ifdef __USER_SCENE_MATRIX_IMPLEMENT__
#   define __ARM_2D_IMPL__
#endif
#ifdef __USER_SCENE_MATRIX_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"

#ifndef MATRIX_LETTER_TRAIN_USE_BLUR
#   define MATRIX_LETTER_TRAIN_USE_BLUR           0
#endif


#ifndef MATRIX_LETTER_TRAIN_FAR_STAGE_COUNT
#   define MATRIX_LETTER_TRAIN_FAR_STAGE_COUNT    30
#endif

#ifndef MATRIX_LETTER_TRAIN_MID_STAGE_COUNT
#   define MATRIX_LETTER_TRAIN_MID_STAGE_COUNT    30
#endif

#ifndef MATRIX_LETTER_TRAIN_NEAR_STAGE_COUNT
#   define MATRIX_LETTER_TRAIN_NEAR_STAGE_COUNT   10
#endif

#if !MATRIX_LETTER_TRAIN_USE_BLUR
#   undef MATRIX_LETTER_TRAIN_FAR_STAGE_COUNT
#   define MATRIX_LETTER_TRAIN_FAR_STAGE_COUNT      0
#endif

#define MATRIX_LETTER_TRAIN_COUNT                   \
        (   MATRIX_LETTER_TRAIN_FAR_STAGE_COUNT     \
        +   MATRIX_LETTER_TRAIN_MID_STAGE_COUNT     \
        +   MATRIX_LETTER_TRAIN_NEAR_STAGE_COUNT    )

/*============================ MACROFIED FUNCTIONS ===========================*/

/*!
 * \brief initalize scene_matrix and add it to a user specified scene player
 * \param[in] __DISP_ADAPTER_PTR the target display adapter (i.e. scene player)
 * \param[in] ... this is an optional parameter. When it is NULL, a new 
 *            user_scene_matrix_t will be allocated from HEAP and freed on
 *            the deposing event. When it is non-NULL, the life-cycle is managed
 *            by user.
 * \return user_scene_matrix_t* the user_scene_matrix_t instance
 */
#define arm_2d_scene_matrix_init(__DISP_ADAPTER_PTR, ...)                       \
            __arm_2d_scene_matrix_init( (__DISP_ADAPTER_PTR),                   \
                                        (NULL, ##__VA_ARGS__))

/*============================ TYPES =========================================*/

typedef struct {
    arm_2d_region_t tRegion;

    uint32_t u2Stage            : 2;
    uint32_t                    : 6;
    uint32_t u8RandomSeed       : 8;
    uint32_t u16NumberOfChars   : 16;

    


} __letter_train_t;


/*!
 * \brief a user class for scene matrix
 */
typedef struct user_scene_matrix_t user_scene_matrix_t;

struct user_scene_matrix_t {
    implement(arm_2d_scene_t);                                                  //! derived from class: arm_2d_scene_t

ARM_PRIVATE(
    /* place your private member here, following two are examples */
    int64_t lTimestamp[2];
    bool bUserAllocated;

#if MATRIX_LETTER_TRAIN_USE_BLUR
    arm_2d_filter_iir_blur_descriptor_t tBlurOP;
#endif

    __letter_train_t tTrains[MATRIX_LETTER_TRAIN_COUNT];
)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

ARM_NONNULL(1)
extern
user_scene_matrix_t *__arm_2d_scene_matrix_init(
                                        arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_matrix_t *ptScene);

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#undef __USER_SCENE_MATRIX_IMPLEMENT__
#undef __USER_SCENE_MATRIX_INHERIT__

#ifdef   __cplusplus
}
#endif

#endif

#endif
