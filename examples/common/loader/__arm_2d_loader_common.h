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

#ifndef __ARM_2D_LOADER_COMMON_H__
#define __ARM_2D_LOADER_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d_helper.h"
#include <stdio.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#endif

#ifdef   __cplusplus
extern "C" {
#endif

/* OOC header, please DO NOT modify  */
#ifdef __ARM_2D_LOADER_IO_IMPLEMENT__
#   undef   __ARM_2D_LOADER_IO_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__ARM_2D_LOADER_IO_INHERIT__)
#   undef   __ARM_2D_LOADER_IO_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct arm_loader_io_t {

    bool        (*fnOpen)        (uintptr_t pTarget, void *ptLoader);
    void        (*fnClose)       (uintptr_t pTarget, void *ptLoader);
    bool        (*fnSeek)        (uintptr_t pTarget, void *ptLoader, int32_t offset, int32_t whence);
    intptr_t    (*fnGetPosition) (uintptr_t pTarget, void *ptLoader);
    size_t      (*fnRead)        (uintptr_t pTarget, void *ptLoader, uint8_t *pchBuffer, size_t tSize);
    void        (*fnOnFrameStart)(uintptr_t pTarget, void *ptLoader);

} arm_loader_io_t;

typedef struct arm_loader_io_file_t {
ARM_PRIVATE(
    const char *pchFilePath;
    FILE *phFile; 
)
} arm_loader_io_file_t;

typedef struct arm_loader_io_binary_t {
ARM_PRIVATE(
    size_t tPostion;
    union {
        uint8_t *pchBinary;
        uintptr_t nAddress;
    };
    size_t tSize;
)
} arm_loader_io_binary_t;

typedef arm_loader_io_binary_t arm_loader_io_rom_t;

typedef struct arm_loader_io_window_t {
ARM_PRIVATE(
    arm_2d_byte_fifo_t tInputFIFO;
    arm_2d_byte_fifo_t tWindow;
)
} arm_loader_io_window_t;

typedef struct arm_io_cacheline_t {
ARM_PRIVATE(
    struct arm_io_cacheline_t *ptNext;

    uint32_t u4LiftCount        : 4;
    uint32_t bHasPrefetchNext   : 1;
    uint32_t u27Address         : 27;

    uint32_t wWords[32 / sizeof(uint32_t)];
)
}arm_io_cacheline_t;

typedef struct arm_loader_io_cache_t {
    implement(arm_loader_io_rom_t);

ARM_PRIVATE(
    arm_io_cacheline_t *ptFree;
    bool bAllowsPrefetch;
    volatile bool bFinishedLoading;

    //uint32_t wMissed;
    //uint64_t dwMemoryAccess;
    
    struct {
        arm_io_cacheline_t *ptHead;
    } Ways[1];

    arm_io_cacheline_t *ptRecent;
    arm_io_cacheline_t * volatile ptLoading;
)
} arm_loader_io_cache_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern 
const arm_loader_io_t ARM_LOADER_IO_FILE;

extern 
const arm_loader_io_t ARM_LOADER_IO_BINARY;

extern 
const arm_loader_io_t ARM_LOADER_IO_ROM;

extern 
const arm_loader_io_t ARM_LOADER_IO_CACHE;

extern 
const arm_loader_io_t ARM_LOADER_IO_WINDOW;

/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * Invoke Interface                                                           *
 *----------------------------------------------------------------------------*/
extern
bool arm_loader_io_open(const arm_loader_io_t *ptIO, 
                        uintptr_t pTarget, 
                        void *ptLoader);

extern
void arm_loader_io_close(   const arm_loader_io_t *ptIO,
                            uintptr_t pTarget, 
                            void *ptLoader);

extern
void arm_loader_io_on_frame_start(  const arm_loader_io_t *ptIO,
                                    uintptr_t pTarget, 
                                    void *ptLoader);

extern
bool arm_loader_io_seek(const arm_loader_io_t *ptIO,
                        uintptr_t pTarget, 
                        void *ptLoader, 
                        int32_t offset, 
                        int32_t whence);

extern
intptr_t arm_loader_io_get_position(const arm_loader_io_t *ptIO,
                                    uintptr_t pTarget, 
                                    void *ptLoader);

extern
size_t arm_loader_io_read(  const arm_loader_io_t *ptIO,
                            uintptr_t pTarget, 
                            void *ptLoader, 
                            uint8_t *pchBuffer, 
                            size_t tSize);
/*----------------------------------------------------------------------------*
 * IO Class                                                                   *
 *----------------------------------------------------------------------------*/
extern 
ARM_NONNULL(1, 2)
arm_2d_err_t arm_loader_io_file_init(   arm_loader_io_file_t *ptThis, 
                                        const char *pchFilePath);

extern
ARM_NONNULL(1, 2)
arm_2d_err_t arm_loader_io_binary_init( arm_loader_io_binary_t *ptThis, 
                                        const uint8_t *pchBinary,
                                        size_t tSize);

/*
 * \note You can implement the following function to load external memory
 *
 *  __OVERRIDE_WEAK
 *  size_t __arm_loader_io_rom_memcpy(uintptr_t pObj, uint8_t *pchBuffer, uintptr_t nAddress, size_t tSize)
 *  {
 *      ...
 *      return tSize;
 *  }
 */
extern
ARM_NONNULL(1)
arm_2d_err_t arm_loader_io_rom_init( arm_loader_io_rom_t *ptThis, 
                                     uintptr_t nAddress,
                                     size_t tSize);

extern
ARM_NONNULL(1, 4)
arm_2d_err_t arm_loader_io_cache_init(  arm_loader_io_cache_t *ptThis, 
                                        uintptr_t nAddress,
                                        size_t tSize,
                                        arm_io_cacheline_t *ptCacheLines,
                                        uint_fast8_t chCachelineCount);

/*!
 * \brief initialize a double-buffered window
 * \param[in] ptThis the window object
 * \param[in] pchBuffer the buffer for the window
 * \param[in] hwSize the buffer size
 * \param[in] hwWindowSize the window size
 * \note the the buffer size (hwSize) should be at least 2x of the window size(hwWindowSize).
 * \return the error code for this initialization.
 */
extern
ARM_NONNULL(1)
arm_2d_err_t arm_loader_io_window_init( arm_loader_io_window_t *ptThis, 
                                        uint8_t *pchBuffer,
                                        uint16_t hwSize,
                                        uint16_t hwWindowSize);

extern
ARM_NONNULL(1,2)
void arm_loader_io_window_enqueue(  arm_loader_io_window_t *ptThis, 
                                    void *pBuffer, 
                                    size_t tSize);
#ifdef   __cplusplus
}
#endif

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif  /* __ARM_2D_LOADER_COMMON_H__ */
