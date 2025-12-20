/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
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

/*============================ INCLUDES ======================================*/
#define __ARM_2D_LOADER_IO_IMPLEMENT__
#include "./__arm_2d_loader_common.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#endif

/*============================ MACROS ========================================*/
#undef this
#define this (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static
bool __arm_loader_io_fopen(uintptr_t pTarget, void *ptLoader);

static
void __arm_loader_io_fclose(uintptr_t pTarget, void *ptLoader);

static
bool __arm_loader_io_fseek( uintptr_t pTarget, 
                            void *ptLoader, 
                            int32_t offset, 
                            int32_t whence);

static
size_t __arm_loader_io_fread(   uintptr_t pTarget, 
                                void *ptLoader, 
                                uint8_t *pchBuffer, 
                                size_t tSize);

static
bool __arm_loader_io_binary_open(uintptr_t pTarget, void *ptLoader);

static
void __arm_loader_io_binary_close(uintptr_t pTarget, void *ptLoader);

static
bool __arm_loader_io_binary_seek(   uintptr_t pTarget, 
                                    void *ptLoader, 
                                    int32_t offset, 
                                    int32_t whence);

static
size_t __arm_loader_io_binary_read( uintptr_t pTarget, 
                                    void *ptLoader, 
                                    uint8_t *pchBuffer, 
                                    size_t tSize);

/*============================ GLOBAL VARIABLES ==============================*/

const arm_loader_io_t ARM_LOADER_IO_FILE = {
    .fnOpen =   &__arm_loader_io_fopen,
    .fnClose =  &__arm_loader_io_fclose,
    .fnSeek =   &__arm_loader_io_fseek,
    .fnRead =   &__arm_loader_io_fread,
};

const arm_loader_io_t ARM_LOADER_IO_BINARY= {
    .fnOpen =   &__arm_loader_io_binary_open,
    .fnClose =  &__arm_loader_io_binary_close,
    .fnSeek =   &__arm_loader_io_binary_seek,
    .fnRead =   &__arm_loader_io_binary_read,
};
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


/*----------------------------------------------------------------------------*
 * IO                                                                         *
 *----------------------------------------------------------------------------*/

static
bool __file_exists(const char *path, const char *pchMode) 
{
    FILE *ph = fopen(path, pchMode);
    if (ph) {
        fclose(ph);
        return true;
    }
    return false;
}

ARM_NONNULL(1, 2)
arm_2d_err_t arm_loader_io_file_init(   arm_loader_io_file_t *ptThis, 
                                        const char *pchFilePath)
{
    if (NULL == ptThis || NULL == pchFilePath) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__file_exists(pchFilePath, "rb")) {
        return ARM_2D_ERR_IO_ERROR;
    }

    memset(ptThis, 0, sizeof(arm_loader_io_file_t));
    this.pchFilePath = pchFilePath;

    return ARM_2D_ERR_NONE;
}

static
bool __arm_loader_io_fopen(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_file_t *ptThis = (arm_loader_io_file_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);

    assert(NULL != ptThis);
    assert(NULL != this.pchFilePath);

    this.phFile = fopen(this.pchFilePath, "rb");
    if (NULL == this.phFile) {
        return false;
    }

    return true;
}

static
void __arm_loader_io_fclose(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_file_t *ptThis = (arm_loader_io_file_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    fclose(this.phFile);
    this.phFile = NULL;
}

static
bool __arm_loader_io_fseek( uintptr_t pTarget, 
                            void *ptLoader, 
                            int32_t offset, 
                            int32_t whence)
{
    arm_loader_io_file_t *ptThis = (arm_loader_io_file_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    return fseek(this.phFile, offset, whence) == 0;
}

static
size_t __arm_loader_io_fread(   uintptr_t pTarget, 
                                void *ptLoader, 
                                uint8_t *pchBuffer, 
                                size_t tSize)
{
    arm_loader_io_file_t *ptThis = (arm_loader_io_file_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    return fread(pchBuffer, 1, tSize, this.phFile);
}


ARM_NONNULL(1, 2)
arm_2d_err_t arm_loader_io_binary_init( arm_loader_io_binary_t *ptThis, 
                                        const uint8_t *pchBinary,
                                        size_t tSize)
{
    if (NULL == ptThis || NULL == pchBinary || 0 == tSize) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    memset(ptThis, 0, sizeof(arm_loader_io_binary_t));
    this.pchBinary = (uint8_t *)pchBinary;
    this.tSize = tSize;

    return ARM_2D_ERR_NONE;
}

static
bool __arm_loader_io_binary_open(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_binary_t *ptThis = (arm_loader_io_binary_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);
    assert(NULL != this.pchBinary);
    assert(this.tSize > 0);

    this.tPostion = 0;

    return true;
}

static
void __arm_loader_io_binary_close(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_binary_t *ptThis = (arm_loader_io_binary_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    ARM_2D_UNUSED(ptThis);

}

static
bool __arm_loader_io_binary_seek(uintptr_t pTarget, void *ptLoader, int32_t offset, int32_t whence)
{
    arm_loader_io_binary_t *ptThis = (arm_loader_io_binary_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);
    assert(NULL != this.pchBinary);
    assert(this.tSize > 0);

    switch (whence) {
        case SEEK_SET:
            if (offset < 0 || offset >= (int32_t)this.tSize) {
                return false;
            }
            this.tPostion = offset;
            break;
        
        case SEEK_END:
            if (offset > 0 || (-offset >= (int32_t)this.tSize)) {
                return false;
            }
            this.tPostion = this.tSize + offset - 1;
            break;
        
        case SEEK_CUR:
            if (offset > 0) {
                if ((this.tPostion + offset) >= this.tSize) {
                    return false;
                }
                this.tPostion += offset;
            } else if (offset < 0) {
                size_t tABSOffset = -offset;
                if ((this.tPostion < tABSOffset)) {
                    return false;
                }
                this.tPostion -= tABSOffset;
            }
            break;
        default:
            return false;
    }

    return true;

}

static
size_t __arm_loader_io_binary_read(uintptr_t pTarget, void *ptLoader, uint8_t *pchBuffer, size_t tSize)
{
    arm_loader_io_binary_t *ptThis = (arm_loader_io_binary_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);
    assert(NULL != this.pchBinary);
    assert(this.tSize > 0);

    if (NULL == pchBuffer || 0 == tSize) {
        return 0;
    }

    if (this.tPostion >= this.tSize) {
        return 0;
    }
    size_t iSizeToRead = this.tSize - this.tPostion;
    iSizeToRead = MIN(iSizeToRead, tSize);

    memcpy(pchBuffer, this.pchBinary + this.tPostion, iSizeToRead);

    this.tPostion += iSizeToRead;

    return iSizeToRead;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
