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
size_t __arm_loader_io_window_read( uintptr_t pTarget, 
                                    void *ptLoader, 
                                    uint8_t *pchBuffer, 
                                    size_t tSize);

static
void __arm_loader_io_window_on_frame_start( uintptr_t pTarget, 
                                            void *ptLoader);

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

const arm_loader_io_t ARM_LOADER_IO_ROM = {
    .fnOpen =   &__arm_loader_io_binary_open,
    .fnClose =  &__arm_loader_io_binary_close,
    .fnSeek =   &__arm_loader_io_binary_seek,
    .fnRead =   &__arm_loader_io_rom_read,
};

const arm_loader_io_t ARM_LOADER_IO_WINDOW = {
    .fnOpen =           &__arm_loader_io_window_open,
    .fnClose =          &__arm_loader_io_window_close,
    .fnSeek =           &__arm_loader_io_window_seek,
    .fnRead =           &__arm_loader_io_window_read,
    .fnOnFrameStart =   &__arm_loader_io_window_on_frame_start,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Invoke Interface                                                           *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1, 3)
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

ARM_NONNULL(1, 3)
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

ARM_NONNULL(1, 3)
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

ARM_NONNULL(1, 3)
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

ARM_NONNULL(1, 3)
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


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
