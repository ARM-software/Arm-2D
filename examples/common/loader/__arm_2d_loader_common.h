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

#include "arm_2d.h"
#include <stdio.h>

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

    bool   (*fnOpen)    (uintptr_t pTarget, void *ptLoader);
    void   (*fnClose)   (uintptr_t pTarget, void *ptLoader);
    bool   (*fnSeek)    (uintptr_t pTarget, void *ptLoader, int32_t offset, int32_t whence);
    size_t (*fnRead)    (uintptr_t pTarget, void *ptLoader, uint8_t *pchBuffer, size_t tSize);

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

/*============================ GLOBAL VARIABLES ==============================*/

extern 
const arm_loader_io_t ARM_LOADER_IO_FILE;

extern 
const arm_loader_io_t ARM_LOADER_IO_BINARY;

extern 
const arm_loader_io_t ARM_LOADER_IO_ROM;

/*============================ PROTOTYPES ====================================*/

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
arm_2d_err_t arm_loader_io_rom_init( arm_loader_io_binary_t *ptThis, 
                                     uintptr_t nAddress,
                                     size_t tSize);
#ifdef   __cplusplus
}
#endif

#endif  /* __ARM_2D_LOADER_COMMON_H__ */
