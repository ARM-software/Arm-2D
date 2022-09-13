/*
 * Copyright (C) 2022 Arm Limited or its affiliates. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        #include "arm_2d_helper.h"
 * Description:  Public header file for the all helper services
 *
 * $Date:        13. Sept 2022
 * $Revision:    V.1.2.2
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_H__
#define __ARM_2D_HELPER_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./arm_2d_helper_pfb.h"
#include "./arm_2d_helper_scene.h"
#include "./arm_2d_disp_adapters.h"
#include <stdlib.h>
#include <assert.h>

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wunused-function"
#endif


/*!
 * \addtogroup gHelper 7 Helper Services
 * @{
 */
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __declare_tile(__name)                                                  \
            extern const arm_2d_tile_t __name;

/*!
 * \brief declare a tile
 * \param __name the name of the tile
 */
#define declare_tile(__name)            __declare_tile(__name)

/*!
 * \brief declare a tile
 * \param __name the name of the tile
 */
#define dcl_tile(__name)                declare_tile(__name)

/*!
 * \brief declare a framebuffer
 * \param __name the name of the framebuffer
 */
#define dcl_fb(__name)                  declare_tile(__name)

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __impl_fb(__NAME, __WIDTH, __HEIGHT, __TYPE, ...)                       \
            ARM_NOINIT static __TYPE                                            \
                __NAME##Buffer[(__WIDTH) * (__HEIGHT)];                         \
            const arm_2d_tile_t __NAME = {                                      \
                .tRegion = {                                                    \
                    .tSize = {(__WIDTH), (__HEIGHT)},                           \
                },                                                              \
                .tInfo.bIsRoot = true,                                          \
                .pchBuffer = (uint8_t *)__NAME##Buffer,                         \
                __VA_ARGS__                                                     \
            };

/*!
 * \brief implement a framebuffer 
 * \param __NAME the name of the framebuffer
 * \param __WIDTH the width
 * \param __HEIGHT the height
 * \param __TYPE the type of the pixel
 * \param ... an optional initialisation for other members of arm_2d_tile_t
 */
#define impl_fb(__NAME, __WIDTH, __HEIGHT, __TYPE, ...)                         \
            __impl_fb(__NAME, __WIDTH, __HEIGHT, __TYPE, ##__VA_ARGS__)

/*!
 * \brief implement a framebuffer and allocate it from the heap
 * \note  the framebuffer will be freed automatically when run out of the code
 *        body. 
 * \param __NAME the name of the framebuffer
 * \param __WIDTH the width
 * \param __HEIGHT the height
 * \param __TYPE the type of the pixel
 */
#define impl_heap_fb(__tile_name, __width, __height, __colour_type)             \
    arm_using(                                                                  \
        arm_2d_tile_t __tile_name = {                                           \
            .pchBuffer = malloc((__width) * (__height) * sizeof(__colour_type)),\
        },                                                                      \
        ({__tile_name.tRegion.tSize.iWidth = (__width);                         \
         __tile_name.tRegion.tSize.iHeight = (__height);                        \
         __tile_name.tInfo.bIsRoot = true;                                      \
         assert(NULL != __tile_name.pchBuffer);                                 \
        }),                                                                     \
        ({ arm_2d_op_wait_async(NULL); free(__tile_name.phwBuffer); }) )

/*!
 * \brief calculate the number of pixels in a given tile
 * \param __NAME the target tile
 * \return uint32_t the number of pixels
 */
#define get_tile_buffer_pixel_count(__NAME)                                     \
            (uint32_t)(     (__NAME.tRegion.tSize.iWidth)                       \
                        *   (__NAME.tRegion.tSize.iHeight))

/*!
 * \brief calculate the size of a given framebuffer
 * \param __NAME the target tile
 * \return uint32_t the buffer size
 */
#define get_tile_buffer_size(__NAME, __TYPE)                                    \
            (get_2d_layer_buffer_pixel_count(__NAME) * sizeof(TYPE))


/*!
 * \brief implement a child tile for a given parent
 * \param __PARENT the parent tile
 * \param __X the x offset in the parent region
 * \param __Y the y offset in the parent region
 * \param __WIDTH the width of the new tile
 * \param __HEIGHT the height of the new tile
 * \param ... an optional initialisation for other members of arm_2d_tile_t
 */
#define impl_child_tile(__PARENT, __X, __Y, __WIDTH, __HEIGHT, ...) {           \
        .tRegion = {                                                            \
            .tLocation = {                                                      \
                .iX = (__X),                                                    \
                .iY = (__Y),                                                    \
            },                                                                  \
            .tSize = {                                                          \
                .iWidth = (__WIDTH),                                            \
                .iHeight = (__HEIGHT),                                          \
            },                                                                  \
        },                                                                      \
        .tInfo.bIsRoot = false,                                                 \
        .tInfo.bDerivedResource = true,                                         \
        .ptParent = (arm_2d_tile_t *)&(__PARENT),                               \
        __VA_ARGS__                                                             \
    }

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_centre2(__region, __size)                                \
    for (arm_2d_region_t __centre_region = {                                    \
            .tSize = (__size),                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__size).iWidth)  >> 1,\
                .iY = ((__region).tSize.iHeight - (__size).iHeight)>> 1,\
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_centre3(__region, __width, __height)                     \
    for (arm_2d_region_t __centre_region = {                                    \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__width))  >> 1,              \
                .iY = ((__region).tSize.iHeight - (__height))>> 1,              \
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )    

#define arm_2d_align_centre(...)                                                \
            ARM_CONNECT2(   __arm_2d_align_centre,                              \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief set an alarm with given period and check the status
 *
 * \param[in] wMS a time period in millisecond
 * \param[in] ... an optional timestamp holder
 *
 * \return bool whether it is timeout
 */
#define arm_2d_helper_is_time_out(__MS, ...)                                    \
    __arm_2d_helper_is_time_out((__MS), (NULL, ##__VA_ARGS__))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief set an alarm with given period and check the status
 * 
 * \param[in] wMS a time period in millisecond
 * \param[in] plTimestamp a pointer points to an int64_t integer, if NULL is 
 *            passed, an static local variable inside the function will be used
 * \return bool whether it is timeout or not
 */
__STATIC_INLINE 
bool __arm_2d_helper_is_time_out(uint32_t wMS, int64_t *plTimestamp)
{
    int64_t lTimestamp = arm_2d_helper_get_system_timestamp();
    static int64_t s_lTimestamp = 0;
    if (NULL == plTimestamp) {
        plTimestamp = &s_lTimestamp;
    }

    if (0 == *plTimestamp) {
        *plTimestamp = arm_2d_helper_convert_ms_to_ticks(wMS);
        *plTimestamp += lTimestamp;
        
        return false;
    }

    if (lTimestamp >= *plTimestamp) {
        *plTimestamp = arm_2d_helper_convert_ms_to_ticks(wMS) + lTimestamp;
        return true;
    }

    return false;
}

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
