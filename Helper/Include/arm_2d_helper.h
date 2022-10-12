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
 * $Date:        13. Oct 2022
 * $Revision:    V.1.3.2
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
#include "./arm_2d_helper_list.h"

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
#define __impl_fb(__name, __width, __height, __type, ...)                       \
            ARM_NOINIT static __type                                            \
                __name##Buffer[(__width) * (__height)];                         \
            const arm_2d_tile_t __name = {                                      \
                .tRegion = {                                                    \
                    .tSize = {(__width), (__height)},                           \
                },                                                              \
                .tInfo.bIsRoot = true,                                          \
                .pchBuffer = (uint8_t *)__name##Buffer,                         \
                __VA_ARGS__                                                     \
            }

/*!
 * \brief implement a framebuffer 
 * \param __name the name of the framebuffer
 * \param __width the width
 * \param __height the height
 * \param __type the type of the pixel
 * \param ... an optional initialisation for other members of arm_2d_tile_t
 */
#define impl_fb(__name, __width, __height, __type, ...)                         \
            __impl_fb(__name, __width, __height, __type, ##__VA_ARGS__)

/*!
 * \brief implement a framebuffer and allocate it from the heap
 * \note  the framebuffer will be freed automatically when run out of the code
 *        body. 
 * \param __tile_name the name of the framebuffer
 * \param __width the width
 * \param __height the height
 * \param __colour_type the type of the pixel
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
 * \param __name the target tile
 * \return uint32_t the number of pixels
 */
#define get_tile_buffer_pixel_count(__name)                                     \
            (uint32_t)(     (__name.tRegion.tSize.iWidth)                       \
                        *   (__name.tRegion.tSize.iHeight))

/*!
 * \brief calculate the size of a given framebuffer
 * \param __name the target tile
 * \param __type the pixel type
 * \return uint32_t the buffer size
 */
#define get_tile_buffer_size(__name, __type)                                    \
            (get_2d_layer_buffer_pixel_count(__name) * sizeof(__type))


/*!
 * \brief implement a child tile for a given parent
 * \param __parent the parent tile
 * \param __x the x offset in the parent region
 * \param __y the y offset in the parent region
 * \param __width the width of the new tile
 * \param __height the height of the new tile
 * \param ... an optional initialisation for other members of arm_2d_tile_t
 */
#define impl_child_tile(__parent, __x, __y, __width, __height, ...) {           \
        .tRegion = {                                                            \
            .tLocation = {                                                      \
                .iX = (__x),                                                    \
                .iY = (__y),                                                    \
            },                                                                  \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
        },                                                                      \
        .tInfo.bIsRoot = false,                                                 \
        .tInfo.bDerivedResource = true,                                         \
        .ptParent = (arm_2d_tile_t *)&(__parent),                               \
        __VA_ARGS__                                                             \
    }

/*----------------------------------------------------------------------------*
 * Helper Macros for Alignment                                                *
 *----------------------------------------------------------------------------*/
 
/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_left2(__region, __size)                              \
    for (arm_2d_region_t __top_left_region = {                                  \
            .tSize = (__size),                                                  \
            .tLocation = {                                                      \
                .iX = 0,                                                        \
                .iY = 0,                                                        \
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
#define __arm_2d_align_top_left3(__region, __width, __height)                   \
    for (arm_2d_region_t __top_left_region = {                                  \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
            .tLocation = {                                                      \
                .iX = 0,                                                        \
                .iY = 0,                                                        \
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         top-left alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __arm_2d_align_top_left(__region, __size) {
 *              code body that can use __top_left_region
 *          }
 * 
 * \note prototype 2:
 *          __arm_2d_align_top_left(__region, __width, __height) {
 *              code body that can use __top_left_region
 *          }
 *          
 */
#define arm_2d_align_top_left(...)                                              \
            ARM_CONNECT2(   __arm_2d_align_top_left,                            \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_centre2(__region, __size)                            \
    for (arm_2d_region_t __top_centre_region = {                                \
            .tSize = (__size),                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__size).iWidth)  >> 1,        \
                .iY = 0,                                                        \
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
#define __arm_2d_align_top_centre3(__region, __width, __height)                 \
    for (arm_2d_region_t __top_centre_region = {                                \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__width))  >> 1,              \
                .iY = 0,                                                        \
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         top-central alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_top_centre(__region, __size) {
 *              code body that can use __top_centre_region
 *          }
 * 
 * \note prototype 2:
 *          arm_2d_align_top_centre(__region, __width, __height) {
 *              code body that can use __top_centre_region
 *          }
 *          
 */
#define arm_2d_align_top_centre(...)                                            \
            ARM_CONNECT2(   __arm_2d_align_top_centre,                          \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_right2(__region, __size)                             \
    for (arm_2d_region_t __top_right_region = {                                 \
            .tSize = (__size),                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__size).iWidth),              \
                .iY = 0,                                                        \
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
#define __arm_2d_align_top_right3(__region, __width, __height)                  \
    for (arm_2d_region_t __top_right_region = {                                 \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__width)),                    \
                .iY = 0,                                                        \
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         top-right alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __arm_2d_align_top_right(__region, __size) {
 *              code body that can use __top_right_region
 *          }
 * 
 * \note prototype 2:
 *          __arm_2d_align_top_right(__region, __width, __height) {
 *              code body that can use __top_right_region
 *          }
 *          
 */
#define arm_2d_align_top_right(...)                                             \
            ARM_CONNECT2(   __arm_2d_align_top_right,                           \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)




/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_mid_left2(__region, __size)                              \
    for (arm_2d_region_t __mid_left_region = {                                  \
            .tSize = (__size),                                                  \
            .tLocation = {                                                      \
                .iX = 0,                                                        \
                .iY = ((__region).tSize.iHeight - (__size).iHeight)>> 1,        \
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
#define __arm_2d_align_mid_left3(__region, __width, __height)                   \
    for (arm_2d_region_t __mid_left_region = {                                  \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
            .tLocation = {                                                      \
                .iX = 0,                                                        \
                .iY = ((__region).tSize.iHeight - (__height))>> 1,              \
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         middle-left alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_mid_left(__region, __size) {
 *              code body that can use __mid_left_region
 *          }
 * 
 * \note prototype 2:
 *          arm_2d_align_mid_left(__region, __width, __height) {
 *              code body that can use __mid_left_region
 *          }
 *          
 */
#define arm_2d_align_mid_left(...)                                              \
            ARM_CONNECT2(   __arm_2d_align_mid_left,                            \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_centre2(__region, __size)                                \
    for (arm_2d_region_t __centre_region = {                                    \
            .tSize = (__size),                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__size).iWidth)  >> 1,        \
                .iY = ((__region).tSize.iHeight - (__size).iHeight)>> 1,        \
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

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         central alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_centre(__region, __size) {
 *              code body that can use __centre_region
 *          }
 * 
 * \note prototype 2:
 *          arm_2d_align_centre(__region, __width, __height) {
 *              code body that can use __centre_region
 *          }
 *          
 */
#define arm_2d_align_centre(...)                                                \
            ARM_CONNECT2(   __arm_2d_align_centre,                              \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_mid_right2(__region, __size)                             \
    for (arm_2d_region_t __mid_right_region = {                                 \
            .tSize = (__size),                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__size).iWidth),              \
                .iY = ((__region).tSize.iHeight - (__size).iHeight)>> 1,        \
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
#define __arm_2d_align_mid_right3(__region, __width, __height)                  \
    for (arm_2d_region_t __mid_right_region = {                                 \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__width)),                    \
                .iY = ((__region).tSize.iHeight - (__height))>> 1,              \
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         middle-right alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_mid_right(__region, __size) {
 *              code body that can use __mid_right_region
 *          }
 * 
 * \note prototype 2:
 *          arm_2d_align_mid_right(__region, __width, __height) {
 *              code body that can use __mid_right_region
 *          }
 *          
 */
#define arm_2d_align_mid_right(...)                                             \
            ARM_CONNECT2(   __arm_2d_align_mid_right,                           \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_left2(__region, __size)                           \
    for (arm_2d_region_t __bottom_left_region = {                               \
            .tSize = (__size),                                                  \
            .tLocation = {                                                      \
                .iX = 0,                                                        \
                .iY = ((__region).tSize.iHeight - (__size).iHeight),            \
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
#define __arm_2d_align_bottom_left3(__region, __width, __height)                \
    for (arm_2d_region_t __bottom_left_region = {                               \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
            .tLocation = {                                                      \
                .iX = 0,                                                        \
                .iY = ((__region).tSize.iHeight - (__height)),                  \
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         bottom-left alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __arm_2d_align_bottom_left(__region, __size) {
 *              code body that can use __bottom_left_region
 *          }
 * 
 * \note prototype 2:
 *          __arm_2d_align_bottom_left(__region, __width, __height) {
 *              code body that can use __bottom_left_region
 *          }
 *          
 */
#define arm_2d_align_bottom_left(...)                                           \
            ARM_CONNECT2(   __arm_2d_align_bottom_left,                         \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_centre2(__region, __size)                         \
    for (arm_2d_region_t __bottom_centre_region = {                             \
            .tSize = (__size),                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__size).iWidth)  >> 1,        \
                .iY = ((__region).tSize.iHeight - (__size).iHeight),            \
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
#define __arm_2d_align_bottom_centre3(__region, __width, __height)              \
    for (arm_2d_region_t __bottom_centre_region = {                             \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__width))  >> 1,              \
                .iY = ((__region).tSize.iHeight - (__height)),                  \
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         bottom-central alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_bottom_centre(__region, __size) {
 *              code body that can use __bottom_centre_region
 *          }
 * 
 * \note prototype 2:
 *          arm_2d_align_bottom_centre(__region, __width, __height) {
 *              code body that can use __bottom_centre_region
 *          }
 *          
 */
#define arm_2d_align_bottom_centre(...)                                         \
            ARM_CONNECT2(   __arm_2d_align_bottom_centre,                       \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_right2(__region, __size)                          \
    for (arm_2d_region_t __bottom_right_region = {                              \
            .tSize = (__size),                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__size).iWidth),              \
                .iY = ((__region).tSize.iHeight - (__size).iHeight),            \
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
#define __arm_2d_align_bottom_right3(__region, __width, __height)               \
    for (arm_2d_region_t __bottom_right_region = {                              \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
            .tLocation = {                                                      \
                .iX = ((__region).tSize.iWidth - (__width)),                    \
                .iY = ((__region).tSize.iHeight - (__height)),                  \
            },                                                                  \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         bottom-right alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __arm_2d_align_bottom_right(__region, __size) {
 *              code body that can use __bottom_right_region
 *          }
 * 
 * \note prototype 2:
 *          __arm_2d_align_bottom_right(__region, __width, __height) {
 *              code body that can use __bottom_right_region
 *          }
 *          
 */
#define arm_2d_align_bottom_right(...)                                          \
            ARM_CONNECT2(   __arm_2d_align_bottom_right,                        \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief set an alarm with given period and check the status
 *
 * \param[in] __ms a time period in millisecond
 * \param[in] ... an optional timestamp holder
 *
 * \return bool whether it is timeout
 */
#define arm_2d_helper_is_time_out(__ms, ...)                                    \
    __arm_2d_helper_is_time_out((__ms), (NULL, ##__VA_ARGS__))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


/*! 
 * \brief convert ticks of a reference timer to millisecond 
 *
 * \param[in] lTick the tick count
 * \return int64_t the millisecond
 */
extern
int64_t arm_2d_helper_convert_ticks_to_ms(int64_t lTick);

/*! 
 * \brief convert millisecond into ticks of the reference timer 
 *
 * \param[in] wMS the target time in millisecond
 * \return int64_t the ticks
 */
extern
int64_t arm_2d_helper_convert_ms_to_ticks(uint32_t wMS);

/*!
 * \brief get the reference clock frequency
 * \return uint32_t the frequency
 */
extern
uint32_t arm_2d_helper_get_reference_clock_frequency(void);

/*!
 * \brief get the current system stamp from the reference clock
 * 
 * \return int64_t the timestamp in ticks (no overflow issue)
 * \note you have to call arm_2d_helper_convert_ticks_to_ms() to convert the 
 *       the timestamp into milliseconds when required.
 */
extern
int64_t arm_2d_helper_get_system_timestamp(void);


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
