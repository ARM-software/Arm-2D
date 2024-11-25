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
 * $Date:        25. Nov 2024
 * $Revision:    V.2.2.1
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
#include "./arm_2d_helper_shape.h"
#include "./arm_2d_helper_font.h"
#include "./arm_2d_helper_control.h"

//#include "./arm_2d_helper_map.h"


#include <stdlib.h>
#include <assert.h>

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wpadded"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 64
#endif


/* OOC header, please DO NOT modify  */
#ifdef __ARM_2D_HELPER_IMPLEMENT__
#   define __ARM_2D_IMPL__
#elif defined(__ARM_2D_HELPER_INHERIT__)
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"

/*!
 * \addtogroup Deprecated
 * @{
 */
#define arm_2d_draw_box              arm_2d_helper_draw_box
/*! @} */

/*!
 * \addtogroup gHelper 8 Helper Services
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


#define arm_2d_helper_time_elapsed(__timestamp_ptr)                             \
        arm_2d_helper_convert_ticks_to_ms(                                      \
            __arm_2d_helper_time_elapsed(__timestamp_ptr))

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


/*!
 * \brief initialize/implement a given film (arm_2d_helper_file_t) object 
 *        at compile-time.
 * \param[in] __sprites_tile the sprites tile
 * \param[in] __width the width of each frame
 * \param[in] __height the height of each frame
 * \param[in] __column the number of frames per row in the sprite tile
 * \param[in] __frame_count the total number of frames in the sprite tile
 * \param[in] __period the period per-frame
 * \note __period is used as a reference for applications. The helper service
 *       doesn't use it at all.
 */
#define impl_film(  __sprites_tile,                                             \
                    __width,                                                    \
                    __height,                                                   \
                    __column,                                                   \
                    __frame_count,                                              \
                    __period)                                                   \
    {                                                                           \
        .use_as__arm_2d_tile_t =                                                \
            impl_child_tile((__sprites_tile), 0, 0, (__width), (__height)),     \
        .hwColumn = (__column),                                                 \
        .hwFrameNum = (__frame_count),                                          \
        .hwPeriodPerFrame = (__period),                                         \
    }

/*============================ TYPES =========================================*/

/*!
 * \brief a helper class to represent a GIF-like resource
 */
typedef struct arm_2d_helper_film_t {
    implement(arm_2d_tile_t);                                                   /*!< derived from arm_2d_tile_t */
    uint16_t hwColumn;                                                          /*!< number of frames per row in a sprite tile */
    uint16_t hwFrameNum;                                                        /*!< the total number of frames */
    uint16_t hwPeriodPerFrame;                                                  /*!< the period per frame (optional, used as a reference) */
    uint16_t hwFrameIndex;                                                      /*!< the frame index used at runtime */
} arm_2d_helper_film_t;

struct __arm_2d_fifo_reader_pointer {
    uint16_t hwPointer;
    uint16_t hwDataAvailable;
};

typedef struct arm_2d_byte_fifo_t {
ARM_PROTECTED (
    uint8_t *pchBuffer;
    uint16_t hwSize;
    uint16_t hwTail;

    struct __arm_2d_fifo_reader_pointer tHead;
    struct __arm_2d_fifo_reader_pointer tPeek;
)
} arm_2d_byte_fifo_t;

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
 * \return int64_t the timestamp in ticks
 * \note you have to call arm_2d_helper_convert_ticks_to_ms() to convert the 
 *       the timestamp into milliseconds when required.
 */
extern
int64_t arm_2d_helper_get_system_timestamp(void);

/*!
 * \brief get the elapsed time since a reference timestamp
 * \param[in] plTimestamp the reference timestamp
 * \return int64_t the timestamp in ticks
 */
extern
ARM_NONNULL(1)
int64_t __arm_2d_helper_time_elapsed(int64_t *plTimestamp);

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
 * \brief get a new semaphore from host RTOS
 * \return uintptr_t a handler for the semaphore
 */
extern
uintptr_t arm_2d_port_new_semaphore(void);

/*!
 * \brief free a semaphore
 * \param[in] pSemaphore the target semaphore
 */
extern
void arm_2d_port_free_semaphore(uintptr_t pSemaphore);

/*!
 * \brief wait for a semaphore
 * \param[in] pSemaphore the target semaphore
 * \retval true we get the semaphore
 * \retval false we haven't get the sempahore
 */
extern 
bool arm_2d_port_wait_for_semaphore(uintptr_t pSemaphore);

/*!
 * \brief set a semaphore 
 * \param[in] pSemaphore the target semaphore
 */
extern
void arm_2d_port_set_semaphore(uintptr_t pSemaphore);

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

/*!
 * \brier move to the next frame of a given film
 * \param[in] ptThis the target film
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_film_next_frame(arm_2d_helper_film_t *ptThis);

/*!
 * \brier reset the frame index to zero
 * \param[in] ptThis the target film
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_film_reset(arm_2d_helper_film_t *ptThis);

/*!
 * \brier reset the frame index to a specified value and wrap around if the
 *        index number is out of range.
 * \param[in] ptThis the target film
 * \param[in] nIndex the given index
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_film_set_frame(arm_2d_helper_film_t *ptThis, int32_t nIndex);

/*----------------------------------------------------------------------------*
 * FIFO Helper Service                                                        *
 *----------------------------------------------------------------------------*/

/*!
 * \brief initialize a given byte fifo
 * \param[in] ptThis the target FIFO control block
 * \param[in] pBuffer a buffer for storing the data
 * \param[in] hwSize the buffer size
 * \retval false Illegal parameters
 * \retval true the target FIFO is initialized
 */
extern
ARM_NONNULL(1,2)
bool arm_2d_byte_fifo_init( arm_2d_byte_fifo_t *ptThis, 
                            void *pBuffer, 
                            uint16_t hwSize);

/*!
 * \brief Empty a given byte fifo
 * \param[in] ptThis the target byte fifo
 */
extern
ARM_NONNULL(1)
void arm_2d_byte_fifo_drop_all( arm_2d_byte_fifo_t *ptThis);

/*!
 * \brief write a byte to a given fifo
 * \param[in] ptThis the target FIFO control block
 * \param[in] chChar the target byte
 * \retval false the FIFO is FULL
 * \retval true operation is successful
 */
extern
ARM_NONNULL(1)
bool arm_2d_byte_fifo_enqueue(arm_2d_byte_fifo_t *ptThis, uint8_t chChar);

/*!
 * \brief read a byte from a given fifo
 * \param[in] ptThis the target FIFO control block
 * \param[in] pchChar a buffer to store the byte, NULL means drop a byte
 * \retval false the FIFO is EMPTY
 * \retval true operation is successful
 */
extern
ARM_NONNULL(1)
bool arm_2d_byte_fifo_dequeue(arm_2d_byte_fifo_t *ptThis, uint8_t *pchChar);

/*!
 * \brief peek a byte continuously from a given fifo
 * \param[in] ptThis the target FIFO control block
 * \param[in] pchChar a buffer to store the byte, NULL means drop a byte
 * \retval false the FIFO is EMPTY
 * \retval true operation is successful
 */
extern
ARM_NONNULL(1)
bool arm_2d_byte_fifo_peek( arm_2d_byte_fifo_t *ptThis, 
                            uint8_t *pchChar, 
                            bool bMovePointer);

/*!
 * \brief drop all peeked byte from a given fifo
 * \param[in] ptThis the target FIFO control block
 * \return none
 */
extern
ARM_NONNULL(1)
void arm_2d_byte_fifo_get_all_peeked(arm_2d_byte_fifo_t *ptThis);

/*!
 * \brief reset the peek pointer, so you can restart from the beginning to peek.
 * \param[in] ptThis the target FIFO control block
 * \return none
 */
ARM_NONNULL(1)
void arm_2d_byte_fifo_reset_peeked(arm_2d_byte_fifo_t *ptThis);

/*----------------------------------------------------------------------------*
 * Misc                                                                       *
 *----------------------------------------------------------------------------*/

/*!
 * \brief swap the high and low bytes for each rgb16 pixel
 *
 * \param[in] phwBuffer the pixel buffer
 * \note the phwBuffer MUST aligned to half-word addresses
 *
 * \param[in] wSize the number of pixels
 */
extern
void arm_2d_helper_swap_rgb16(uint16_t *phwBuffer, uint32_t wCount);

extern
ARM_NONNULL(1)
/*!
 * \brief fill the target tile with a specified colour
 * 
 * \param[in] ptTile the target tile
 * \param[in] tColourFormat the colour format of the target tile
 * \param[in] tColour the target colour
 */
void arm_2d_helper_fill_tile_colour(const arm_2d_tile_t *ptTile, 
                                    arm_2d_color_info_t tColourFormat,
                                    arm_2d_colour_t tColour);

#if __ARM_2D_HELPER_CFG_LAYOUT_DEBUG_MODE__
extern
ARM_NONNULL(1)
void __arm_2d_helper_layout_debug_print_label(const arm_2d_tile_t *ptTile, 
                                              arm_2d_region_t *ptRegion,
                                              const char *pchString);
#endif

/*! @} */

/*============================ INCLUDES ======================================*/
#include "arm_2d_helper_built_in.h"

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_ARM_COMPILER_5__
#pragma diag_warning 64
#endif

#undef __ARM_2D_HELPER_IMPLEMENT__
#undef __ARM_2D_HELPER_INHERIT__


#ifdef   __cplusplus
}
#endif

#endif
