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
 * $Date:        06. April 2023
 * $Revision:    V.1.6.2
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_H__
#define __ARM_2D_HELPER_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__arm_2d_helper_common.h"
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
 * \addtogroup Deprecated
 * @{
 */
#define arm_2d_draw_box              arm_2d_helper_draw_box
/*! @} */

/*!
 * \addtogroup gHelper 7 Helper Services
 * @{
 */
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

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
           __arm_2d_helper_time_liner_slider((__from),                          \
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

/*!
 * \brief initialize helper services
 */
extern
void arm_2d_helper_init(void);

/*!
 * \brief backend task for asynchronose mode
 */
extern
void arm_2d_helper_backend_task(void);

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

/*!
 * \brier colour intrapolation
 * \param[in] wFrom a 32bit colour (4 8bit colour channels) on the start
 * \param[in] wTo a 32bit colour (4 8bit colour channels) on the end
 * \param[in] nDistance the reference full distance between two end points
 * \param[in] nOffset the offset from the start
 * \return uint32_t 32bit colour
 */
extern
uint32_t __arm_2d_helper_colour_slider( uint32_t wFrom, 
                                        uint32_t wTo,
                                        int32_t nDistance,
                                        int32_t nOffset);

/*!
 * \brier draw a box with specified colour, border width and opacity
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] iBorderWidth the border width
 * \param[in] tColour the target colour
 * \param[in] chOpacity the opacity
 */
extern
void arm_2d_helper_draw_box( const arm_2d_tile_t *ptTarget,
                             const arm_2d_region_t *ptRegion,
                             int16_t iBorderWidth, 
                             COLOUR_INT tColour,
                             uint8_t chOpacity);


/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
