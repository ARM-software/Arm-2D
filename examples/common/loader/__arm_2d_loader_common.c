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
#define __ARM_2D_HELPER_INHERIT__
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
intptr_t __arm_loader_io_file_get_position(uintptr_t pTarget, void *ptLoader);

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
intptr_t __arm_loader_io_binary_get_position(uintptr_t pTarget, void *ptLoader);

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

static
size_t __arm_loader_io_rom_read(uintptr_t pTarget, 
                                void *ptLoader, 
                                uint8_t *pchBuffer, 
                                size_t tSize);

static
bool __arm_loader_io_window_open(uintptr_t pTarget, void *ptLoader);

static
void __arm_loader_io_window_close(uintptr_t pTarget, void *ptLoader);

static
bool __arm_loader_io_window_seek(   uintptr_t pTarget, 
                                    void *ptLoader, 
                                    int32_t offset, 
                                    int32_t whence);

static
intptr_t __arm_loader_io_window_get_position(uintptr_t pTarget, void *ptLoader);

static
size_t __arm_loader_io_window_read( uintptr_t pTarget, 
                                    void *ptLoader, 
                                    uint8_t *pchBuffer, 
                                    size_t tSize);

static
void __arm_loader_io_window_on_frame_start( uintptr_t pTarget, 
                                            void *ptLoader);

static
bool __arm_loader_io_cache_open(uintptr_t pTarget, void *ptLoader);

static
void __arm_loader_io_cache_close(uintptr_t pTarget, void *ptLoader);

static
bool __arm_loader_io_cache_seek(uintptr_t pTarget, 
                                void *ptLoader, 
                                int32_t offset, 
                                int32_t whence);

static
size_t __arm_loader_io_cache_read(  uintptr_t pTarget, 
                                    void *ptLoader, 
                                    uint8_t *pchBuffer, 
                                    size_t tSize);
/*============================ GLOBAL VARIABLES ==============================*/

const arm_loader_io_t ARM_LOADER_IO_FILE = {
    .fnOpen =           &__arm_loader_io_fopen,
    .fnClose =          &__arm_loader_io_fclose,
    .fnSeek =           &__arm_loader_io_fseek,
    .fnGetPosition =    &__arm_loader_io_file_get_position,
    .fnRead =           &__arm_loader_io_fread,
};

const arm_loader_io_t ARM_LOADER_IO_BINARY= {
    .fnOpen =           &__arm_loader_io_binary_open,
    .fnClose =          &__arm_loader_io_binary_close,
    .fnSeek =           &__arm_loader_io_binary_seek,
    .fnGetPosition =    &__arm_loader_io_binary_get_position,
    .fnRead =           &__arm_loader_io_binary_read,
};

const arm_loader_io_t ARM_LOADER_IO_ROM = {
    .fnOpen =           &__arm_loader_io_binary_open,
    .fnClose =          &__arm_loader_io_binary_close,
    .fnSeek =           &__arm_loader_io_binary_seek,
    .fnGetPosition =    &__arm_loader_io_binary_get_position,
    .fnRead =           &__arm_loader_io_rom_read,
};

const arm_loader_io_t ARM_LOADER_IO_CACHE = {
    .fnOpen =           &__arm_loader_io_cache_open,
    .fnClose =          &__arm_loader_io_cache_close,
    .fnSeek =           &__arm_loader_io_cache_seek,
    .fnGetPosition =    &__arm_loader_io_binary_get_position,
    .fnRead =           &__arm_loader_io_cache_read,
};

const arm_loader_io_t ARM_LOADER_IO_WINDOW = {
    .fnOpen =           &__arm_loader_io_window_open,
    .fnClose =          &__arm_loader_io_window_close,
    .fnSeek =           &__arm_loader_io_window_seek,
    .fnGetPosition =    &__arm_loader_io_window_get_position,
    .fnRead =           &__arm_loader_io_window_read,
    .fnOnFrameStart =   &__arm_loader_io_window_on_frame_start,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Invoke Interface                                                           *
 *----------------------------------------------------------------------------*/


bool arm_loader_io_open(const arm_loader_io_t *ptIO, 
                        uintptr_t pTarget, 
                        void *ptLoader)
{
    ARM_2D_UNUSED(ptLoader);
    if (NULL == ptIO) {
        return true;
    }

    if (NULL == ptIO->fnOpen) {
        return true;
    }

    return ptIO->fnOpen(pTarget,ptLoader);
}


void arm_loader_io_close(   const arm_loader_io_t *ptIO,
                            uintptr_t pTarget, 
                            void *ptLoader)
{
    ARM_2D_UNUSED(ptLoader);
    if (NULL == ptIO) {
        return ;
    }

    ARM_2D_INVOKE_RT_VOID(ptIO->fnClose, 
        ARM_2D_PARAM(pTarget,
                    ptLoader));
}

intptr_t arm_loader_io_get_position(const arm_loader_io_t *ptIO,
                                    uintptr_t pTarget, 
                                    void *ptLoader)
{
    ARM_2D_UNUSED(ptLoader);
    if (NULL == ptIO) {
        return -1;
    }

    return ARM_2D_INVOKE(ptIO->fnGetPosition, 
            ARM_2D_PARAM(pTarget,
                        ptLoader));
}

bool arm_loader_io_seek(const arm_loader_io_t *ptIO,
                        uintptr_t pTarget, 
                        void *ptLoader, 
                        int32_t offset, 
                        int32_t whence)
{
    ARM_2D_UNUSED(ptLoader);
    if (NULL == ptIO) {
        return true;
    }

    if (NULL == ptIO->fnSeek) {
        return true;
    }

    return ptIO->fnSeek(pTarget, ptLoader, offset, whence);
}


size_t arm_loader_io_read(  const arm_loader_io_t *ptIO,
                            uintptr_t pTarget, 
                            void *ptLoader, 
                            uint8_t *pchBuffer, 
                            size_t tSize)
{
    ARM_2D_UNUSED(ptLoader);
    if (NULL == ptIO) {
        return 0;
    }

    return ARM_2D_INVOKE(   ptIO->fnRead, 
            ARM_2D_PARAM(   pTarget, 
                            ptLoader, 
                            pchBuffer, 
                            tSize));
}


void arm_loader_io_on_frame_start(  const arm_loader_io_t *ptIO,
                                    uintptr_t pTarget, 
                                    void *ptLoader)
{
    ARM_2D_UNUSED(ptLoader);
    if (NULL == ptIO) {
        return ;
    }

    ARM_2D_INVOKE_RT_VOID(ptIO->fnOnFrameStart, 
        ARM_2D_PARAM(   pTarget,
                        ptLoader));
}

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
intptr_t __arm_loader_io_file_get_position(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_file_t *ptThis = (arm_loader_io_file_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    return (intptr_t)ftell(this.phFile);
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

ARM_NONNULL(1)
arm_2d_err_t arm_loader_io_rom_init( arm_loader_io_binary_t *ptThis, 
                                     uintptr_t nAddress,
                                     size_t tSize)
{
    if (NULL == ptThis || 0 == tSize) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    memset(ptThis, 0, sizeof(arm_loader_io_binary_t));
    this.nAddress = nAddress;
    this.tSize = tSize;

    return ARM_2D_ERR_NONE;
}

static
bool __arm_loader_io_binary_open(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_binary_t *ptThis = (arm_loader_io_binary_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);
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
intptr_t __arm_loader_io_binary_get_position(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_binary_t *ptThis = (arm_loader_io_binary_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    ARM_2D_UNUSED(ptThis);
    return (intptr_t)this.tPostion;
}

static
bool __arm_loader_io_binary_seek(uintptr_t pTarget, void *ptLoader, int32_t offset, int32_t whence)
{
    arm_loader_io_binary_t *ptThis = (arm_loader_io_binary_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);
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

__WEAK 
__attribute__((noinline))
size_t __arm_loader_io_rom_memcpy(uintptr_t pObj, uint8_t *pchBuffer, uintptr_t nAddress, size_t tSize)
{
    ARM_2D_UNUSED(pObj);

    memcpy(pchBuffer, (uint8_t *)nAddress, tSize);

    return tSize;
} 

static
size_t __arm_loader_io_rom_read(uintptr_t pTarget, void *ptLoader, uint8_t *pchBuffer, size_t tSize)
{
    arm_loader_io_binary_t *ptThis = (arm_loader_io_binary_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);
    assert(this.tSize > 0);

    if (NULL == pchBuffer || 0 == tSize) {
        return 0;
    }

    if (this.tPostion >= this.tSize) {
        return 0;
    }
    size_t iSizeToRead = this.tSize - this.tPostion;
    iSizeToRead = MIN(iSizeToRead, tSize);

    iSizeToRead = __arm_loader_io_rom_memcpy(   pTarget, 
                                                pchBuffer, 
                                                this.nAddress + this.tPostion, 
                                                iSizeToRead);

    this.tPostion += iSizeToRead;

    return iSizeToRead;
}


ARM_NONNULL(1)
arm_2d_err_t arm_loader_io_window_init( arm_loader_io_window_t *ptThis, 
                                        uint8_t *pchBuffer,
                                        uint16_t hwSize,
                                        uint16_t hwWindowSize)
{
    if (    NULL == ptThis 
       ||   0 == hwSize 
       ||   0 == hwWindowSize
       ||   hwSize < (hwWindowSize * 2)) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    memset(ptThis, 0, sizeof(arm_loader_io_window_t));
    size_t tInputBufferSize = hwSize - hwWindowSize;

    if (!arm_2d_byte_fifo_init(&this.tInputFIFO, pchBuffer, tInputBufferSize)) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    pchBuffer += tInputBufferSize;

    if (!arm_2d_byte_fifo_init(&this.tWindow, pchBuffer, hwWindowSize)) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    return ARM_2D_ERR_NONE;
}

static
void __arm_loader_io_window_on_frame_start(uintptr_t pTarget, void *ptLoader)
{
    ARM_2D_UNUSED(ptLoader);
    arm_loader_io_window_t *ptThis = (arm_loader_io_window_t *)pTarget;

    assert(NULL != ptThis);

    bool bFinished = false;
    arm_irq_safe do {
        uint16_t hwDataAvailable = arm_2d_byte_fifo_get_item_count(&this.tInputFIFO);
        uint16_t hwWindowDepth = arm_2d_byte_fifo_get_capcity(&this.tWindow);

        if (hwDataAvailable < hwWindowDepth) {
            break;
        }

        /* move to the peek pointer */
        arm_2d_byte_fifo_peek_seek( &this.tInputFIFO, 
                                    hwDataAvailable - hwWindowDepth,
                                    SEEK_SET);

        /* copy the data to window buffer directly */
        uint8_t *pchBuffer = this.tWindow.pchBuffer;
        do {
            size_t tAcutalRead = arm_2d_byte_fifo_peek_bytes(
                                    &this.tInputFIFO,
                                    pchBuffer,
                                    hwWindowDepth);
            hwWindowDepth -= tAcutalRead;
            pchBuffer += tAcutalRead;
        } while(hwWindowDepth);
        
        /* update the window info */
        this.tWindow.hwTail = 0;
        this.tWindow.tHead.hwPointer = 0;
        this.tWindow.tHead.hwDataAvailable = this.tWindow.hwSize;
        this.tWindow.tPeek.hwPointer = 0;
        this.tWindow.tPeek.hwDataAvailable = this.tWindow.hwSize;

        /* empty the input buffer */
        arm_2d_byte_fifo_drop_all(&this.tInputFIFO);

        bFinished = true;
    } while(0);

    if (bFinished) {
        return ;
    }

    do {
        uint8_t chByte;
        if (!arm_2d_byte_fifo_dequeue(&this.tInputFIFO, &chByte)) {
            break;
        }

        arm_2d_byte_fifo_squeeze(&this.tWindow, chByte);
    } while(true);
}

ARM_NONNULL(1,2)
void arm_loader_io_window_enqueue(  arm_loader_io_window_t *ptThis, 
                                    void *pBuffer, 
                                    size_t tSize)
{
    assert(NULL != ptThis);

    if (NULL == pBuffer || 0 == tSize) {
        return ;
    }

    uint8_t *pchSrc = (uint8_t *)pBuffer;
    uint16_t hwFIFODepth = arm_2d_byte_fifo_get_capcity(&this.tInputFIFO);
    if (tSize >= hwFIFODepth) {
        arm_2d_byte_fifo_drop_all(&this.tInputFIFO);
        tSize -= hwFIFODepth;
        arm_irq_safe {
            memcpy( this.tInputFIFO.pchBuffer, 
                    pchSrc + tSize,
                    hwFIFODepth);

            this.tInputFIFO.hwTail = 0;
            this.tInputFIFO.tHead.hwPointer = 0;
            this.tInputFIFO.tHead.hwDataAvailable = hwFIFODepth;
            this.tInputFIFO.tPeek.hwPointer = 0;
            this.tInputFIFO.tPeek.hwDataAvailable = hwFIFODepth;
        }
        return ;
    }

    do {
        arm_2d_byte_fifo_squeeze(&this.tInputFIFO, *pchSrc++);
    } while(--tSize);
}

static
bool __arm_loader_io_window_open(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_window_t *ptThis = (arm_loader_io_window_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    return true;
}

static
void __arm_loader_io_window_close(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_window_t *ptThis = (arm_loader_io_window_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    ARM_2D_UNUSED(ptThis);

}

static
intptr_t __arm_loader_io_window_get_position(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_window_t *ptThis = (arm_loader_io_window_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    ARM_2D_UNUSED(ptThis);

    return this.tWindow.tHead.hwDataAvailable 
         - this.tWindow.tPeek.hwDataAvailable;
}

static
bool __arm_loader_io_window_seek(uintptr_t pTarget, void *ptLoader, int32_t offset, int32_t whence)
{
    arm_loader_io_window_t *ptThis = (arm_loader_io_window_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    if (arm_2d_byte_fifo_peek_seek(&this.tWindow, offset, whence) < 0) {
        return false;
    }

    return true;
}

static
size_t __arm_loader_io_window_read(uintptr_t pTarget, void *ptLoader, uint8_t *pchBuffer, size_t tSize)
{
    arm_loader_io_window_t *ptThis = (arm_loader_io_window_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    if (NULL == pchBuffer || 0 == tSize) {
        return 0;
    }

    return arm_2d_byte_fifo_peek_bytes(&this.tWindow, pchBuffer, tSize);
}


__STATIC_INLINE 
void __arm_loader_io_cacheline_free(arm_loader_io_cache_t *ptThis,
                                    arm_io_cacheline_t *ptCacheLine)
{
    arm_irq_safe {
        if (this.ptRecent == ptCacheLine) {
            this.ptRecent = NULL;
        }
        ARM_LIST_STACK_PUSH(this.ptFree, ptCacheLine);
    }
}

ARM_NONNULL(1, 4)
arm_2d_err_t arm_loader_io_cache_init(  arm_loader_io_cache_t *ptThis, 
                                        uintptr_t nAddress,
                                        size_t tSize,
                                        arm_io_cacheline_t *ptCacheLines,
                                        uint_fast8_t chCachelineCount)
{
    if (NULL == ptThis || 0 == tSize || NULL == ptCacheLines || 0 == chCachelineCount) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    memset(ptThis, 0, sizeof(arm_loader_io_cache_t));
    this.use_as__arm_loader_io_rom_t.nAddress = nAddress;
    this.use_as__arm_loader_io_rom_t.tSize = tSize;

    this.bAllowsPrefetch = (chCachelineCount > 1);

    do {
        __arm_loader_io_cacheline_free(ptThis, ptCacheLines++);
    } while(--chCachelineCount);

    return ARM_2D_ERR_NONE;
}

static
bool __arm_loader_io_cache_open(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_cache_t *ptThis = (arm_loader_io_cache_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);
    assert(this.use_as__arm_loader_io_rom_t.tSize > 0);

    this.use_as__arm_loader_io_rom_t.tPostion = 0;
    this.ptRecent = NULL;
    
    assert(NULL == this.ptLoading);

    return true;
}

static
void __arm_loader_io_cache_close(uintptr_t pTarget, void *ptLoader)
{
    arm_loader_io_cache_t *ptThis = (arm_loader_io_cache_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    ARM_2D_UNUSED(ptThis);
    
    /* wait all loading complete */
    while(!this.bFinishedLoading);

    if (NULL != this.ptLoading) {
        __arm_loader_io_cacheline_free(ptThis, this.ptLoading);
        this.ptLoading = NULL;
    }
    
    /* free all cacheline */
    arm_foreach(this.Ways) {
        arm_io_cacheline_t *ptItem = _->ptHead;

        while(NULL != ptItem) {
            arm_io_cacheline_t *ptNextItem = ptItem->ptNext;
            __arm_loader_io_cacheline_free(ptThis, ptItem);

            ptItem = ptNextItem;
        }

        _->ptHead = NULL;
    }
}

static
arm_io_cacheline_t *__arm_loader_io_search_cacheline(arm_loader_io_cache_t *ptThis, uintptr_t wAddress)
{
    assert(NULL != ptThis);

    wAddress >>= 5;

    if (NULL != this.ptRecent) {

        if (wAddress == this.ptRecent->u27Address) {
            /* cache hit */
            this.ptRecent->u4LiftCount = 0xF;
            return this.ptRecent;
        }
    }

    /* search cacheline */
    arm_foreach(this.Ways) {
        
        arm_io_cacheline_t **pptItem = &(_->ptHead);

        while(NULL != (*pptItem)) {
            if (wAddress == (*pptItem)->u27Address) {
                /* cache hit */
                (*pptItem)->u4LiftCount = 0xF; /* reset life count */
                return (*pptItem);
            }

            if (this.bFinishedLoading && NULL != this.ptLoading) {
                assert(this.ptLoading->u27Address != ((*pptItem)->u27Address));

                if (this.ptLoading->u27Address < ((*pptItem)->u27Address)) {
                    arm_irq_safe {
                        this.ptLoading->ptNext = (*pptItem);
                        (*pptItem) = this.ptLoading;
                        this.ptLoading = NULL;
                    }
                    continue;
                }
            }

            if ((wAddress < (*pptItem)->u27Address) 
             && (this.ptLoading == NULL)) {
                return NULL;
            }

            if ((*pptItem)->u4LiftCount) {
                (*pptItem)->u4LiftCount--;

                arm_irq_safe {
                    pptItem = &((*pptItem)->ptNext);
                }
            } else {
                arm_io_cacheline_t *ptDeadOne = *pptItem;

                arm_irq_safe {
                    /* remove the dead one from the chain */
                    (*pptItem) = ptDeadOne->ptNext;
                }

                __arm_loader_io_cacheline_free(ptThis, ptDeadOne);
            }
        }

        if (this.bFinishedLoading && NULL != this.ptLoading) {
            arm_irq_safe {
                this.ptLoading->ptNext = (*pptItem);
                (*pptItem) = this.ptLoading;
                this.ptLoading = NULL;
            }

            if (wAddress == (*pptItem)->u27Address) {
                /* cache hit */
                (*pptItem)->u4LiftCount = 0xF; /* reset life count */
                return (*pptItem);
            }
        }
    }

    return NULL;
}

static
bool __arm_loader_io_cache_is_busy(arm_loader_io_cache_t *ptThis)
{
    bool bBusy = false;

    arm_irq_safe {
        bBusy = (NULL != this.ptLoading);
    }

    return bBusy;
}

ARM_NONNULL(1)
void arm_loader_io_cache_report_load_complete(arm_loader_io_cache_t *ptThis)
{
    assert(NULL != ptThis);
    assert(NULL != this.ptLoading);

    arm_io_cacheline_t *ptItem = this.ptLoading;

    ptItem->u4LiftCount = 0xF;
    ptItem->bHasPrefetchNext = false;

    arm_irq_safe {
        this.bFinishedLoading = true;
    }

}


__WEAK
__attribute__((noinline))
void __arm_loader_io_cache_request_load_memory( arm_loader_io_cache_t *ptThis,
                                                uintptr_t wAddress,
                                                uint32_t *pwBuffer,
                                                size_t tNumberOfWords)
{
    memcpy(pwBuffer, (uint8_t *)wAddress, tNumberOfWords * sizeof(uint32_t));

    arm_loader_io_cache_report_load_complete(ptThis);
}

static
void __arm_loader_io_cache_preload(arm_loader_io_cache_t *ptThis, uintptr_t wAddress)
{
    assert(NULL != ptThis);

    if (__arm_loader_io_cache_is_busy(ptThis)) {
        /* busy */
        return ;
    }

    arm_io_cacheline_t *ptVictim = NULL;

    wAddress >>= 5;

    /* find a victim */
    if (NULL != this.ptFree) {

        //arm_irq_safe {
            ARM_LIST_STACK_POP(this.ptFree, ptVictim);
        //}

    } else {
        /* find a dead cacheline  */
        uint_fast8_t chMinLifeCount = 0xFF;
        arm_io_cacheline_t **pptVictim = NULL;
         
        arm_foreach(this.Ways) {
            arm_io_cacheline_t **pptItem = &_->ptHead;

            while(NULL != *pptItem) {
                if ((*pptItem)->u27Address == wAddress) {
                    return ;
                }
                if (0 == (*pptItem)->u4LiftCount) {
                    chMinLifeCount = 0;
                    pptVictim = pptItem;
                } else if ((*pptItem)->u4LiftCount <= chMinLifeCount) {
                    chMinLifeCount = (*pptItem)->u4LiftCount;
                    pptVictim = pptItem;
                }

                pptItem = &((*pptItem)->ptNext);
            }
        }

        ptVictim = (*pptVictim);
        /* remove it from the chain */
        (*pptVictim) = (*pptVictim)->ptNext;

    }

label_finish_searching:
    assert(NULL != ptVictim);

    arm_irq_safe {
        if (this.ptRecent == ptVictim) {
            this.ptRecent = NULL;
        }
    }

    ptVictim->u27Address = wAddress;
    ptVictim->ptNext = NULL;

    this.bFinishedLoading = false;
    this.ptLoading = ptVictim;

    wAddress = (wAddress << 5) + this.use_as__arm_loader_io_rom_t.nAddress;

    __arm_loader_io_cache_request_load_memory(  ptThis, 
                                                wAddress, 
                                                ptVictim->wWords, 
                                                dimof(ptVictim->wWords));

    return ;
}


static
bool __arm_loader_io_cache_seek(uintptr_t pTarget, void *ptLoader, int32_t offset, int32_t whence)
{
    arm_loader_io_cache_t *ptThis = (arm_loader_io_cache_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    bool bResult = __arm_loader_io_binary_seek(pTarget, ptLoader, offset, whence);

    /* we use this address to preload content */
    uintptr_t wAddress = this.use_as__arm_loader_io_rom_t.tPostion;
    
    arm_io_cacheline_t *ptHit = __arm_loader_io_search_cacheline(ptThis, wAddress);
    if (NULL != ptHit) {
        this.ptRecent = ptHit;
    } else {
        /* cache miss, request a preload */
        __arm_loader_io_cache_preload(ptThis, wAddress);
    }

    return bResult;
}

static
size_t __arm_loader_io_cache_read(  uintptr_t pTarget, 
                                    void *ptLoader, 
                                    uint8_t *pchBuffer, 
                                    size_t tSize)
{
    arm_loader_io_cache_t *ptThis = (arm_loader_io_cache_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);
    assert(this.use_as__arm_loader_io_rom_t.tSize > 0);
    
    if (NULL == pchBuffer || 0 == tSize) {
        return 0;
    }

    if (this.use_as__arm_loader_io_rom_t.tPostion >= this.use_as__arm_loader_io_rom_t.tSize) {
        return 0;
    }
    size_t iSizeToRead = this.use_as__arm_loader_io_rom_t.tSize 
                       - this.use_as__arm_loader_io_rom_t.tPostion;
    iSizeToRead = MIN(iSizeToRead, tSize);

    uintptr_t wAddress = this.use_as__arm_loader_io_rom_t.tPostion;
    //this.dwMemoryAccess++;

    /* read cacheline */
    do {
        arm_io_cacheline_t *ptHit = NULL;

        do {
            ptHit = __arm_loader_io_search_cacheline(ptThis, wAddress);
            if (NULL == ptHit) {
                //this.wMissed++;
                /* cache miss, request a preload */
                __arm_loader_io_cache_preload(ptThis, wAddress);

                /* wait until the cacheline is ready */
                //while (__arm_loader_io_cache_is_busy(ptThis));
                //assert(NULL == this.ptLoading);
                while(!this.bFinishedLoading);
            }
        } while(NULL == ptHit);

        this.ptRecent = ptHit;

        if (!ptHit->bHasPrefetchNext && this.bAllowsPrefetch) {
            ptHit->bHasPrefetchNext = true;
            uintptr_t nNextAddress = (ptHit->u27Address + 1) << 5;
            __arm_loader_io_cache_preload(ptThis, nNextAddress);
        }

        uintptr_t wOffset = wAddress - (ptHit->u27Address << 5);
        size_t wDataAvailable = sizeof(ptHit->wWords) - wOffset;

        iSizeToRead = MIN(iSizeToRead, wDataAvailable);

        memcpy(pchBuffer, (uint8_t *)ptHit->wWords + wOffset, iSizeToRead );
    } while(0);   


    this.use_as__arm_loader_io_rom_t.tPostion += iSizeToRead;

    return iSizeToRead;

}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
