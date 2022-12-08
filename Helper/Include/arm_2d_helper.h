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
 * $Date:        8. Dec 2022
 * $Revision:    V.1.6.0
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

#define arm_2d_canvas(__tile_ptr, __region_name)                             \
            arm_using(arm_2d_region_t __region_name = {0},                      \
                        { __region_name.tSize = (__tile_ptr)->tRegion.tSize;},  \
                        {__region_name = __region_name;})

#define arm_2d_layout(__region)                                                 \
        arm_using(arm_2d_region_t __layout = (__region))

#define ____item_line_horizontal2(__width, __height)                            \
    for (arm_2d_region_t __item_region = {                                      \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
            .tLocation = __layout.tLocation,                                    \
        },                                                                      \
        *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;                      \
         ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;                   \
         arm_2d_op_wait_async(NULL)                                             \
        )

#define __item_line_horizontal(...)                                             \
            ARM_CONNECT2(   ____item_line_horizontal,                           \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_left2(__region, __size)                              \
            arm_using(                                                          \
                arm_2d_region_t __top_left_region,                              \
                {                                                               \
                    __top_left_region.tSize.iWidth = (__size).iWidth;           \
                    __top_left_region.tSize.iHeight = (__size).iHeight;         \
                    __top_left_region.tLocation = (__region).tLocation;         \
                },                                                              \
                { __top_left_region = __top_left_region;})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_left3(__region, __width, __height)                   \
            arm_using(                                                          \
                    arm_2d_region_t __top_left_region,                          \
                    {                                                           \
                        __top_left_region.tSize.iWidth = (__width);             \
                        __top_left_region.tSize.iHeight = (__height);           \
                        __top_left_region.tLocation = (__region).tLocation;     \
                    },                                                          \
                    { __top_left_region = __top_left_region;})

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
            arm_using(                                                          \
                arm_2d_region_t __top_centre_region,                            \
                {                                                               \
                    __top_centre_region.tSize.iWidth = (__size).iWidth;         \
                    __top_centre_region.tSize.iHeight = (__size).iHeight;       \
                    __top_centre_region.tLocation = (__region).tLocation;       \
                    __top_centre_region.tLocation.iX                            \
                        += ((__region).tSize.iWidth - (__size).iWidth) >> 1;    \
                },                                                              \
                { __top_centre_region = __top_centre_region;})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_centre3(__region, __width, __height)                 \
            arm_using(                                                          \
                    arm_2d_region_t __top_centre_region,                        \
                    {                                                           \
                        __top_centre_region.tSize.iWidth = (__width);           \
                        __top_centre_region.tSize.iHeight = (__height);         \
                        __top_centre_region.tLocation = (__region).tLocation;   \
                        __top_centre_region.tLocation.iX                        \
                            += ((__region).tSize.iWidth - (__width)) >> 1;      \
                    },                                                          \
                    { __top_centre_region = __top_centre_region;})

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
            arm_using(                                                          \
                arm_2d_region_t __top_right_region,                             \
                {                                                               \
                    __top_right_region.tSize.iWidth = (__size).iWidth;          \
                    __top_right_region.tSize.iHeight = (__size).iHeight;        \
                    __top_right_region.tLocation = (__region).tLocation;        \
                    __top_right_region.tLocation.iX                             \
                        += ((__region).tSize.iWidth - (__size).iWidth);         \
                },                                                              \
                { __top_right_region = __top_right_region;})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_right3(__region, __width, __height)                  \
            arm_using(                                                          \
                    arm_2d_region_t __top_right_region,                         \
                    {                                                           \
                        __top_right_region.tSize.iWidth = (__width);            \
                        __top_right_region.tSize.iHeight = (__height);          \
                        __top_right_region.tLocation = (__region).tLocation;    \
                        __top_right_region.tLocation.iX                         \
                            += ((__region).tSize.iWidth - (__width));           \
                    },                                                          \
                    { __top_right_region = __top_right_region;})

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
            arm_using(                                                          \
                arm_2d_region_t __mid_left_region,                              \
                {                                                               \
                    __mid_left_region.tSize.iWidth = (__size).iWidth;           \
                    __mid_left_region.tSize.iHeight = (__size).iHeight;         \
                    __mid_left_region.tLocation = (__region).tLocation;         \
                    __mid_left_region.tLocation.iY                              \
                        += ((__region).tSize.iHeight - (__size).iHeight) >> 1;  \
                },                                                              \
                { __mid_left_region = __mid_left_region;})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_mid_left3(__region, __width, __height)                   \
            arm_using(                                                          \
                    arm_2d_region_t __mid_left_region,                          \
                    {                                                           \
                        __mid_left_region.tSize.iWidth = (__width);             \
                        __mid_left_region.tSize.iHeight = (__height);           \
                        __mid_left_region.tLocation = (__region).tLocation;     \
                        __mid_left_region.tLocation.iY                          \
                            += ((__region).tSize.iHeight - (__height)) >> 1;    \
                    },                                                          \
                    { __mid_left_region = __mid_left_region;})

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
            arm_using(                                                          \
                arm_2d_region_t __centre_region,                                \
                {                                                               \
                    __centre_region.tSize.iWidth = (__size).iWidth;             \
                    __centre_region.tSize.iHeight = (__size).iHeight;           \
                    __centre_region.tLocation = (__region).tLocation;           \
                    __centre_region.tLocation.iX                                \
                        += ((__region).tSize.iWidth - (__size).iWidth)  >> 1;   \
                    __centre_region.tLocation.iY                                \
                        += ((__region).tSize.iHeight - (__size).iHeight)>> 1;   \
                },                                                              \
                { __centre_region = __centre_region;})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_centre3(__region, __width, __height)                     \
            arm_using(  arm_2d_region_t __centre_region,                        \
                        {                                                       \
                            __centre_region.tSize.iWidth = (__width);           \
                            __centre_region.tSize.iHeight = (__height);         \
                            __centre_region.tLocation = (__region).tLocation;   \
                            __centre_region.tLocation.iX                        \
                                += ((__region).tSize.iWidth - (__width))  >> 1; \
                            __centre_region.tLocation.iY                        \
                                += ((__region).tSize.iHeight - (__height))>> 1; \
                        },                                                      \
                        { __centre_region = __centre_region;})

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
            arm_using(                                                          \
                arm_2d_region_t __mid_right_region,                             \
                {                                                               \
                    __mid_right_region.tSize.iWidth = (__size).iWidth;          \
                    __mid_right_region.tSize.iHeight = (__size).iHeight;        \
                    __mid_right_region.tLocation = (__region).tLocation;        \
                    __mid_right_region.tLocation.iX                             \
                        += ((__region).tSize.iWidth - (__size).iWidth);         \
                    __mid_right_region.tLocation.iY                             \
                        += ((__region).tSize.iHeight - (__size).iHeight) >> 1;  \
                },                                                              \
                { __mid_right_region = __mid_right_region;})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_mid_right3(__region, __width, __height)                  \
            arm_using(                                                          \
                arm_2d_region_t __mid_right_region,                             \
                {                                                               \
                    __mid_right_region.tSize.iWidth = (__width);                \
                    __mid_right_region.tSize.iHeight = (__height);              \
                    __mid_right_region.tLocation = (__region).tLocation;        \
                    __mid_right_region.tLocation.iX                             \
                        += ((__region).tSize.iWidth - (__width));               \
                    __mid_right_region.tLocation.iY                             \
                        += ((__region).tSize.iHeight - (__height)) >> 1;        \
                },                                                              \
                { __mid_right_region = __mid_right_region;})

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
            arm_using(                                                          \
                arm_2d_region_t __bottom_left_region,                           \
                {                                                               \
                    __bottom_left_region.tSize.iWidth = (__size).iWidth;        \
                    __bottom_left_region.tSize.iHeight = (__size).iHeight;      \
                    __bottom_left_region.tLocation = (__region).tLocation;      \
                    __bottom_left_region.tLocation.iY                           \
                        += ((__region).tSize.iHeight - (__size).iHeight);       \
                },                                                              \
                { __bottom_left_region = __bottom_left_region;})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_left3(__region, __width, __height)                \
            arm_using(                                                          \
                arm_2d_region_t __bottom_left_region,                           \
                {                                                               \
                    __bottom_left_region.tSize.iWidth = (__width);              \
                    __bottom_left_region.tSize.iHeight = (__height);            \
                    __bottom_left_region.tLocation = (__region).tLocation;      \
                    __bottom_left_region.tLocation.iY                           \
                        += ((__region).tSize.iHeight - (__height));             \
                },                                                              \
                { __bottom_left_region = __bottom_left_region;})

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
            arm_using(                                                          \
                arm_2d_region_t __bottom_centre_region,                         \
                {                                                               \
                    __bottom_centre_region.tSize.iWidth = (__size).iWidth;      \
                    __bottom_centre_region.tSize.iHeight = (__size).iHeight;    \
                    __bottom_centre_region.tLocation = (__region).tLocation;    \
                    __bottom_centre_region.tLocation.iX                         \
                        += ((__region).tSize.iWidth - (__size).iWidth)  >> 1;   \
                    __bottom_centre_region.tLocation.iY                         \
                        += ((__region).tSize.iHeight - (__size).iHeight);       \
                },                                                              \
                { __bottom_centre_region = __bottom_centre_region;})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_centre3(__region, __width, __height)              \
            arm_using(                                                          \
                arm_2d_region_t __bottom_centre_region,                         \
                {                                                               \
                    __bottom_centre_region.tSize.iWidth = (__width);            \
                    __bottom_centre_region.tSize.iHeight = (__height);          \
                    __bottom_centre_region.tLocation = (__region).tLocation;    \
                    __bottom_centre_region.tLocation.iX                         \
                        += ((__region).tSize.iWidth - (__width))  >> 1;         \
                    __bottom_centre_region.tLocation.iY                         \
                        += ((__region).tSize.iHeight - (__height));             \
                },                                                              \
                { __bottom_centre_region = __bottom_centre_region;})

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
            arm_using(                                                          \
                arm_2d_region_t __bottom_right_region,                          \
                {                                                               \
                    __bottom_right_region.tSize.iWidth = (__size).iWidth;       \
                    __bottom_right_region.tSize.iHeight = (__size).iHeight;     \
                    __bottom_right_region.tLocation = (__region).tLocation;     \
                    __bottom_right_region.tLocation.iX                          \
                        += ((__region).tSize.iWidth - (__size).iWidth);         \
                    __bottom_right_region.tLocation.iY                          \
                        += ((__region).tSize.iHeight - (__size).iHeight);       \
                },                                                              \
                { __bottom_right_region = __bottom_right_region;})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_right3(__region, __width, __height)               \
            arm_using(                                                          \
                arm_2d_region_t __bottom_right_region,                          \
                {                                                               \
                    __bottom_right_region.tSize.iWidth = (__width);             \
                    __bottom_right_region.tSize.iHeight = (__height);           \
                    __bottom_right_region.tLocation = (__region).tLocation;     \
                    __bottom_right_region.tLocation.iX                          \
                        += ((__region).tSize.iWidth - (__width));               \
                    __bottom_right_region.tLocation.iY                          \
                        += ((__region).tSize.iHeight - (__height));             \
                },                                                              \
                { __bottom_right_region = __bottom_right_region;})

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
    ({  static int64_t arm_2d_safe_name(s_lTimestamp);                          \
        __arm_2d_helper_is_time_out(arm_2d_helper_convert_ms_to_ticks(__ms),    \
        (&arm_2d_safe_name(s_lTimestamp),##__VA_ARGS__));})


/*!
 * \brief calculate the stroke of a liner slider based on time
 *
 * \param[in] __from the start of the slider
 * \param[in] __to the end of the slider
 * \param[in] __ms a given period (ms) in which the slider should finish the 
 *            whole stroke
 * \param[out] __stroke_ptr the address of an int32_t stroke variable
 * \param[in] ... an optional address of a timestamp variable, if you omit it,
 *             NULL will be passed, and the code that call this funtion will not
 *              be reentrant.
 * \retval true the slider has finished the whole stroke
 * \retval false the slider hasn't reach the target end
 */
#define arm_2d_helper_time_liner_slider( __from,                                \
                                         __to,                                  \
                                         __ms,                                  \
                                         __stroke_ptr,                          \
                                         ...)                                   \
           ({static int64_t arm_2d_safe_name(s_lTimestamp);                     \
           __arm_2d_helper_time_line_slider((__from),                           \
                                            (__to),                             \
           arm_2d_helper_convert_ms_to_ticks(__ms),                             \
                                            (__stroke_ptr),                     \
           (&arm_2d_safe_name(s_lTimestamp),##__VA_ARGS__));})

/*!
 * \brief calculate the stroke of a cosine slider based on time
 *
 * \param[in] __from the start of the slider
 * \param[in] __to the end of the slider
 * \param[in] __ms a given period (ms) in which the slider should finish the 
 *            whole stroke
 * \param[in] __phase the phase offset
 * \param[out] __stroke_ptr the address of an int32_t stroke variable
 * \param[in] ... an optional address of a timestamp variable, if you omit it,
 *             NULL will be passed, and the code that call this funtion will not
 *              be reentrant.
 * \retval true the slider has finished the whole stroke
 * \retval false the slider hasn't reach the target end
 */
#define arm_2d_helper_time_cos_slider( __from,                                  \
                                       __to,                                    \
                                       __ms,                                    \
                                       __phase,                                 \
                                       __stroke_ptr,                            \
                                       ...)                                     \
           ({static int64_t arm_2d_safe_name(s_lTimestamp);                     \
           __arm_2d_helper_time_cos_slider((__from),                            \
                                            (__to),                             \
           arm_2d_helper_convert_ms_to_ticks(__ms),                             \
                                            (__phase),                          \
                                            (__stroke_ptr),                     \
           (&arm_2d_safe_name(s_lTimestamp),##__VA_ARGS__));})

/*!
 * \brief calculate the stroke of a cosine slider(0~pi) based on time
 *
 * \param[in] __from the start of the slider
 * \param[in] __to the end of the slider
 * \param[in] __ms a given period (ms) in which the slider should finish the 
 *            whole stroke
 * \param[out] __stroke_ptr the address of an int32_t stroke variable
 * \param[in] ... an optional address of a timestamp variable, if you omit it,
 *             NULL will be passed, and the code that call this funtion will not
 *              be reentrant.
 * \retval true the slider has finished the whole stroke
 * \retval false the slider hasn't reach the target end
 */
#define arm_2d_helper_time_half_cos_slider( __from,                             \
                                       __to,                                    \
                                       __ms,                                    \
                                       __stroke_ptr,                            \
                                       ...)                                     \
           ({static int64_t arm_2d_safe_name(s_lTimestamp);                     \
           __arm_2d_helper_time_half_cos_slider((__from),                       \
                                            (__to),                             \
           arm_2d_helper_convert_ms_to_ticks(__ms),                             \
                                            (__stroke_ptr),                     \
           (&arm_2d_safe_name(s_lTimestamp),##__VA_ARGS__));})

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


__STATIC_INLINE
uint8_t arm_2d_helper_alpha_mix(uint_fast8_t chAlpha1, 
                                                uint_fast8_t chAlpha2)
{
    chAlpha1 = MIN(255, chAlpha1);
    chAlpha2 = MIN(255, chAlpha2);
    return (uint8_t)((chAlpha1 == 255) ? chAlpha2 
                             : ((chAlpha2 == 255) ? chAlpha1 
                                                  : (chAlpha1 * chAlpha2 >> 8)));
}

/*!
 * \brief initialize helper services
 */
extern
void arm_2d_helper_init(void);

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
 * \param[in] lPeriod a time period in ticks
 * \param[in] plTimestamp a pointer points to an int64_t integer, if NULL is 
 *            passed, an static local variable inside the function will be used
 * \return bool whether it is timeout or not
 */
ARM_NONNULL(2)
extern
bool __arm_2d_helper_is_time_out(int64_t lPeriod, int64_t *plTimestamp);

/*!
 * \brief calculate the stroke of a liner slider based on time
 *
 * \param[in] nFrom the start of the slider
 * \param[in] nTo the end of the slider
 * \param[in] lPeriod a given period in which the slider should finish the whole
 *            stroke
 * \param[out] pnStroke the address of an int32_t stroke variable
 * \param[in] plTimestamp the address of a timestamp variable, if you pass NULL
 *            the code that call this funtion will not be reentrant.
 * \retval true the slider has finished the whole stroke
 * \retval false the slider hasn't reach the target end
 */
ARM_NONNULL(4,5)
extern
bool __arm_2d_helper_time_liner_slider( int32_t nFrom, 
                                        int32_t nTo, 
                                        int64_t lPeriod,
                                        int32_t *pnStroke,
                                        int64_t *plTimestamp);

/*!
 * \brief calculate the stroke of a cosine slider (0~pi) based on time
 *
 * \param[in] nFrom the start of the slider
 * \param[in] nTo the end of the slider
 * \param[in] lPeriod a given period in which the slider should finish the whole
 *            stroke
 * \param[out] pnStroke the address of an int32_t stroke variable
 * \param[in] plTimestamp the address of a timestamp variable, if you pass NULL
 *            the code that call this funtion will not be reentrant.
 * \retval true the slider has finished the whole stroke
 * \retval false the slider hasn't reach the target end
 */
ARM_NONNULL(4,5)
extern
bool __arm_2d_helper_time_half_cos_slider(  int32_t nFrom, 
                                            int32_t nTo, 
                                            int64_t lPeriod,
                                            int32_t *pnStroke,
                                            int64_t *plTimestamp);

/*!
 * \brief calculate the stroke of a consine slider (0~2pi) based on time
 *
 * \param[in] nFrom the start of the slider
 * \param[in] nTo the end of the slider
 * \param[in] lPeriod a given period in which the slider should finish the whole
 *            stroke
 * \param[in] lPhase the phase offset
 * \param[out] pnStroke the address of an int32_t stroke variable
 * \param[in] plTimestamp the address of a timestamp variable, if you pass NULL
 *            the code that call this funtion will not be reentrant.
 * \retval true the slider has finished the whole stroke
 * \retval false the slider hasn't reach the target end
 */
ARM_NONNULL(5,6)
extern
bool __arm_2d_helper_time_cos_slider(   int32_t nFrom, 
                                        int32_t nTo, 
                                        int64_t lPeriod,
                                        float fPhase,
                                        int32_t *pnStroke,
                                        int64_t *plTimestamp);




/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
