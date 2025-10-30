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

/*============================ INCLUDES ======================================*/
#define __ZJPGD_LOADER_IMPLEMENT__

#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "zjpgd_loader.h"
#include <assert.h>
#include <string.h>

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
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif

/*============================ MACROS ========================================*/


#undef this
#define this    (*ptThis)


#define __WORKING_MEMORY_SIZE__     ZJD_MEM_POOL_SZ

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    JDEC_CONTEXT_START,
    JDEC_CONTEXT_PREVIOUS_START,
    JDEC_CONTEXT_PREVIOUS_FRAME_START,
    JDEC_CONTEXT_PREVIOUS_LINE,
    JDEC_CONTEXT_CURRENT,
};

/*============================ PROTOTYPES ====================================*/


/*!
 *  \brief a method to load a specific part of an image
 *  \param[in] pTarget a reference of an user object
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] ptRegion the target region of the image
 *  \return intptr_t the address of a resource buffer which holds the content
 */
static
intptr_t __arm_zjpgd_vres_asset_loader( uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        arm_2d_region_t *ptRegion);

/*!
 *  \brief a method to despose the buffer
 *  \param[in] pTarget a reference of an user object
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] pBuffer the target buffer
 */
static
void  __arm_zjpgd_vres_buffer_deposer(  uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        intptr_t pBuffer );
static
bool   __arm_zjpgd_io_fopen(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader);

static
void   __arm_zjpgd_io_fclose(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader);

static
bool   __arm_zjpgd_io_fseek(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader, int32_t offset, int32_t whence);

static
size_t __arm_zjpgd_io_fread(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader, uint8_t *pchBuffer, size_t tSize);

static
bool   __arm_zjpgd_io_binary_open(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader);

static
void   __arm_zjpgd_io_binary_close(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader);

static
bool   __arm_zjpgd_io_binary_seek(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader, int32_t offset, int32_t whence);

static
size_t __arm_zjpgd_io_binary_read(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader, uint8_t *pchBuffer, size_t tSize);


static
zjd_res_t __arm_2d_zjpgd_decode(arm_zjpgd_loader_t *ptThis, arm_2d_region_t *ptRegion, bool bUseContext);

static
bool __arm_zjpgd_decode_prepare(arm_zjpgd_loader_t *ptThis);

ARM_NONNULL(1,2)
static
void __arm_zjpgd_save_context_to(   arm_zjpgd_loader_t *ptThis, 
                                    arm_zjpgd_context_t *ptContext,
                                    int16_t iX,
                                    int16_t iY);

ARM_NONNULL(1,2)
static
void __arm_zjpgd_context_add_to_list(   arm_zjpgd_context_t **ppList, 
                                        arm_zjpgd_context_t *ptItem,
                                        arm_2d_size_t tBlockSize,
                                        bool bBackward);
/*============================ GLOBAL VARIABLES ==============================*/

const arm_zjpgd_loader_io_t ARM_ZJPGD_IO_FILE_LOADER = {
    .fnOpen =   &__arm_zjpgd_io_fopen,
    .fnClose =  &__arm_zjpgd_io_fclose,
    .fnSeek =   &__arm_zjpgd_io_fseek,
    .fnRead =   &__arm_zjpgd_io_fread,
};

const arm_zjpgd_loader_io_t ARM_ZJPGD_IO_BINARY_LOADER = {
    .fnOpen =   &__arm_zjpgd_io_binary_open,
    .fnClose =  &__arm_zjpgd_io_binary_close,
    .fnSeek =   &__arm_zjpgd_io_binary_seek,
    .fnRead =   &__arm_zjpgd_io_binary_read,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t arm_zjpgd_loader_init( arm_zjpgd_loader_t *ptThis,
                                    arm_zjpgd_loader_cfg_t *ptCFG)
{
    if (NULL == ptThis || NULL == ptCFG) {
        return ARM_2D_ERR_INVALID_PARAM;
    } else if (NULL == ptCFG->ptScene) {
        return ARM_2D_ERR_MISSING_PARAM;
    } else if ( (!ptCFG->bUseHeapForVRES)
             && (NULL == ptCFG->ptScene)) {
        return ARM_2D_ERR_MISSING_PARAM;
    } else if (NULL == ptCFG->ImageIO.ptIO) {
        return ARM_2D_ERR_MISSING_PARAM;
    }

    memset(ptThis, 0, sizeof(arm_zjpgd_loader_t));

    this.tCFG = *ptCFG;

    this.vres.Load = &__arm_zjpgd_vres_asset_loader;
    this.vres.Depose = &__arm_zjpgd_vres_buffer_deposer;

    this.vres.tTile.tInfo.bIsRoot = true;
    this.vres.tTile.tInfo.bVirtualResource = true;

    this.Decoder.u3ZJDOutputColourFormat = ZJD_FORMAT;

    if (this.tCFG.tColourInfo.chScheme != 0) {
        /* use user specified colour format */
        switch (this.tCFG.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_GRAY8:
                this.Decoder.u3ZJDOutputColourFormat = ZJD_GRAYSCALE;
                this.vres.tTile.tColourInfo.chScheme = this.tCFG.tColourInfo.chScheme;
                break;
            case ARM_2D_COLOUR_RGB565:
                this.Decoder.u3ZJDOutputColourFormat = ZJD_RGB565;
                this.vres.tTile.tColourInfo.chScheme = this.tCFG.tColourInfo.chScheme;
                break;
            case ARM_2D_COLOUR_BGRA8888:
                this.Decoder.u3ZJDOutputColourFormat = ZJD_BGRA8888;
                this.vres.tTile.tColourInfo.chScheme = this.tCFG.tColourInfo.chScheme;
                break;
            default:
                this.vres.tTile.tColourInfo.chScheme = ARM_2D_COLOUR;
                break;
        }
    } else {
        this.vres.tTile.tColourInfo.chScheme = ARM_2D_COLOUR;
    }
    this.vres.tTile.tInfo.bHasEnforcedColour = true;

    do {
        size_t nPixelSize = sizeof(COLOUR_INT);
        //uint32_t nBytesPerLine = ptRegion->tSize.iWidth * nPixelSize;
        size_t nBitsPerPixel = sizeof(COLOUR_INT) << 3;
    
        if (0 != this.vres.tTile.tColourInfo.chScheme) {
            nBitsPerPixel = (1 << this.vres.tTile.tColourInfo.u3ColourSZ);
            if (this.vres.tTile.tColourInfo.u3ColourSZ >= 3) {
                if (this.vres.tTile.tColourInfo.u3ColourSZ == ARM_2D_COLOUR_SZ_24BIT) {
                    nPixelSize = 3;
                } else {
                    nPixelSize = (1 << (this.vres.tTile.tColourInfo.u3ColourSZ - 3));
                }
                //nBytesPerLine = ptRegion->tSize.iWidth * nPixelSize;
            } else {
                /* for A1, A2 and A4 */
                //size_t nPixelPerByte = 1 << (3 - this.vres.tTile.tColourInfo.u3ColourSZ);
                //int16_t iOffset = ptRegion->tLocation.iX & (nPixelPerByte - 1);
                
                //uint32_t nBitsPerLine =  nBitsPerPixel * (iOffset + ptRegion->tSize.iWidth);
                //nBytesPerLine = (nBitsPerLine + 7) >> 3;
            }
        }

        this.u3PixelByteSize = nPixelSize;
        this.u5BitsPerPixel = nBitsPerPixel;

    } while(0);

    this.bInitialized = true;

    /* try to decode */
    do {
        this.bErrorDetected = false;
        //this.Decoder.nPosition = 0;
        
        __arm_zjpgd_decode_prepare(ptThis);

        /* close low level IO */
        ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
            ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));

        /* free scratch memory */
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, this.Decoder.pWorkMemory);
        this.Decoder.pWorkMemory = NULL;
        
        if (this.bErrorDetected) {
            return ARM_2D_ERR_IO_ERROR;
        }
    } while(0);

    return ARM_2D_ERR_NONE;
}

static
int __arm_zjpgd_loader_in_func (zjd_t *ptDecoder,       
                                uint8_t *pchBuffer,
                                uint32_t wAddress,
                                int nLength)
{
    arm_zjpgd_loader_t *ptThis = (arm_zjpgd_loader_t *)ptDecoder->arg;

    ARM_2D_INVOKE(this.tCFG.ImageIO.ptIO->fnSeek, 
                    ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis, wAddress, SEEK_SET));

    int nResult = nLength;
    if (NULL != pchBuffer) {

        /* record previous read */
        //this.Decoder.PreviousRead.nPostion = this.Decoder.nPosition;
        //this.Decoder.PreviousRead.pBuffer = (uintptr_t)pchBuffer;
        //this.Decoder.PreviousRead.tSize = nLength;

        nResult = ARM_2D_INVOKE(this.tCFG.ImageIO.ptIO->fnRead, 
                    ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis, pchBuffer, nLength));
        //this.Decoder.nPosition += nResult;
    }

    
    return nResult;
}

static
int __arm_zjpgd_loader_write_to_vres_framebuffer (      /* Returns 1 to continue, 0 to abort */
    zjd_t* ptDecoder,           /* Decompression object */
    zjd_rect_t* rect,           /* Rectangle region of output image */
    void* pSource               /* Bitmap data to be output */
)
{
    arm_zjpgd_loader_t *ptThis = (arm_zjpgd_loader_t *)ptDecoder->arg;

    ARM_2D_LOG_INFO(
        CONTROLS, 
        2, 
        "ZJPGDec", 
        "Decoded Block at x=%d, y=%d...",
        rect->x,
        rect->y
    );

    /* save context */
    if (this.tCFG.u2WorkMode == ARM_ZJPGD_MODE_PARTIAL_DECODED) {

        if (!this.Decoder.bContextInitialized) {
            if (0 == rect->x && 0 == rect->y) {
                this.Decoder.bContextInitialized = true;
                zjd_save(ptDecoder, &this.tContext[JDEC_CONTEXT_START].tSnapshot);


                this.tContext[JDEC_CONTEXT_PREVIOUS_START]          = this.tContext[JDEC_CONTEXT_START];
                this.tContext[JDEC_CONTEXT_PREVIOUS_FRAME_START]    = this.tContext[JDEC_CONTEXT_START];
                this.tContext[JDEC_CONTEXT_PREVIOUS_LINE]           = this.tContext[JDEC_CONTEXT_START];
                this.tContext[JDEC_CONTEXT_CURRENT]                 = this.tContext[JDEC_CONTEXT_START];
            }
        }

        /* scan reference candidates */
        arm_zjpgd_context_t *ptCandiate = NULL;
        arm_irq_safe {
            ptCandiate = this.Reference.ptCandidates;
            if (NULL != ptCandiate) {
                if (ptCandiate->tLocation.iX == rect->x && ptCandiate->tLocation.iY == rect->y) {
                    /* save reference point */
                    __arm_zjpgd_save_context_to(ptThis, ptCandiate, rect->x, rect->y);

                    /* remove it from candidate list */
                    ARM_LIST_STACK_POP(this.Reference.ptCandidates, ptCandiate);
                    assert(NULL != ptCandiate);
                } else {
                    ptCandiate = NULL;
                }
            }
        }
        if (NULL != ptCandiate) {
            __arm_zjpgd_context_add_to_list(&this.Reference.ptList, 
                                            ptCandiate, 
                                            this.Decoder.tBlockRegion.tSize,
                                            true);
        }

        if (this.bIsNewFrame) {
            this.bIsNewFrame = false;

            ARM_2D_LOG_INFO(
                CONTROLS, 
                3, 
                "ZJPGDec", 
                "Encounter a new frame, Save context to slot [JDEC_CONTEXT_PREVIOUS_FRAME_START]"
            );

            __arm_zjpgd_save_context_to(ptThis, &this.tContext[JDEC_CONTEXT_PREVIOUS_FRAME_START], rect->x, rect->y);
        }

        if (    ((rect->x + rect->w) >= (this.Decoder.tDrawRegion.tLocation.iX + this.Decoder.tDrawRegion.tSize.iWidth))
           &&   ((rect->y + rect->h) >= (this.Decoder.tDrawRegion.tLocation.iY + this.Decoder.tDrawRegion.tSize.iHeight))) {
            /* reach the end of the draw region */

            ARM_2D_LOG_INFO(
                CONTROLS, 
                3, 
                "ZJPGDec", 
                "Save context to slot [JDEC_CONTEXT_CURRENT]"
            );
            __arm_zjpgd_save_context_to(ptThis, &this.tContext[JDEC_CONTEXT_CURRENT], rect->x, rect->y);     

        }

        if (rect->x <= this.Decoder.tDrawRegion.tLocation.iX) {

            /* the start of a line */
            ARM_2D_LOG_INFO(
                CONTROLS, 
                3, 
                "ZJPGDec", 
                "Save context to slot [JDEC_CONTEXT_PREVIOUS_LINE]"
            );
            __arm_zjpgd_save_context_to(ptThis, &this.tContext[JDEC_CONTEXT_PREVIOUS_LINE], rect->x, rect->y);     
        }

        if ((rect->x <= this.Decoder.tDrawRegion.tLocation.iX) 
        &&  (rect->y <= this.Decoder.tDrawRegion.tLocation.iY)) {

            /* start of the target region */
            ARM_2D_LOG_INFO(
                CONTROLS, 
                3, 
                "ZJPGDec", 
                "Save context to slot [JDEC_CONTEXT_PREVIOUS_START]"
            );
            __arm_zjpgd_save_context_to(ptThis, &this.tContext[JDEC_CONTEXT_PREVIOUS_START], rect->x, rect->y);     
        }

    }

    arm_2d_region_t tBlockRegion = {
        .tLocation = {
            .iX = rect->x,
            .iY = rect->y,
        },

        .tSize = {
            .iWidth = rect->w,
            .iHeight = rect->h,
        },
    };

    /* create source tile */
    arm_2d_tile_t tSourceTile = {
        .tInfo.bIsRoot = true,
        .tRegion = {
            .tSize = tBlockRegion.tSize,
        },
        .nAddress = (uintptr_t)pSource,
    };

    if (this.tCFG.bInverseColour) {
        size_t tPixelCount = tBlockRegion.tSize.iHeight * tBlockRegion.tSize.iWidth;

        switch (this.u3PixelByteSize) {
            case 1: {
                    /* cheat with compiler using __RESTRICT */
                    uint8_t * __RESTRICT pchSrc = (uint8_t *)pSource;
                    uint8_t * __RESTRICT pchDes = (uint8_t *)pSource;

                    do {
                        *pchDes++ = ~*pchSrc++;
                    } while(--tPixelCount);
                }
                break;
            case 2: {
                    /* cheat with compiler using __RESTRICT */
                    uint16_t * __RESTRICT phwSrc = (uint16_t *)pSource;
                    uint16_t * __RESTRICT phwDes = (uint16_t *)pSource;

                    do {
                        *phwDes++ = ~*phwSrc++;
                    } while(--tPixelCount);
                }
                break;
            case 4: {
                    /* cheat with compiler using __RESTRICT */
                    uint32_t * __RESTRICT pwSrc = (uint32_t *)pSource;
                    uint32_t * __RESTRICT pwDes = (uint32_t *)pSource;

                    do {
                        *pwDes++ = ~*pwSrc++;
                    } while(--tPixelCount);
                }
                break;
            default:
                break;
        }
    }

    arm_2d_region_t tTargetRegion = this.ImageBuffer.tRegion;

    arm_2d_tile_t tTargetTile = {
        .tInfo.bIsRoot = true,
        .tRegion = {
            .tSize = {
                .iHeight = tTargetRegion.tSize.iHeight,
                .iWidth = this.iTargetStrideInByte / this.u3PixelByteSize,
            }
        },
        .nAddress = (uintptr_t)this.ImageBuffer.pchBuffer,
    };

    tBlockRegion.tLocation.iX -= tTargetRegion.tLocation.iX;
    tBlockRegion.tLocation.iY -= tTargetRegion.tLocation.iY;

    /* ugly fix for background image loading mode
     * when tTargetRegion.tSize.iWidth < tTargetTile.tRegion.tSize.iWidth
     * the arm_2d_normal_root_tile_copy might write pixels outside the 
     * tTargetRegion
     */
    if (tTargetRegion.tSize.iWidth < tTargetTile.tRegion.tSize.iWidth) {

        if (tBlockRegion.tLocation.iX < 0) {
            int16_t iActualWidth = tBlockRegion.tSize.iWidth + tBlockRegion.tLocation.iX;
            iActualWidth = MIN(iActualWidth, tTargetRegion.tSize.iWidth);
            tBlockRegion.tSize.iWidth = -tBlockRegion.tLocation.iX + iActualWidth;
        } else if (0 == tBlockRegion.tLocation.iX){
            tBlockRegion.tSize.iWidth = MIN(tBlockRegion.tSize.iWidth, tTargetRegion.tSize.iWidth);
        } else {
            int16_t iRight = tBlockRegion.tLocation.iX + tBlockRegion.tSize.iWidth - 1;
            int16_t iTargetRight = 0 + tTargetRegion.tSize.iWidth - 1;
            iRight = MIN(iRight, iTargetRight);
            
            tBlockRegion.tSize.iWidth = iRight - tBlockRegion.tLocation.iX + 1;
        }
    }

    arm_2d_sw_normal_root_tile_copy(&tSourceTile, &tTargetTile, &tBlockRegion, this.u3PixelByteSize);

    return 1;    /* Continue to decompress */
}


static
bool __arm_zjpgd_decode_prepare(arm_zjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (!this.bInitialized) {
        return false;
    }

    this.bErrorDetected = false;
    //this.Decoder.nPosition = 0;

    /* allocate memory */
    do {
        this.Decoder.pWorkMemory = (void*)__arm_2d_allocate_scratch_memory(__WORKING_MEMORY_SIZE__, 4, ARM_2D_MEM_TYPE_FAST);
        if (NULL == this.Decoder.pWorkMemory) {
            this.bErrorDetected = true;
            break;
        } 

        /* open low level IO */
        if (!ARM_2D_INVOKE(this.tCFG.ImageIO.ptIO->fnOpen,
                ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis))) {
            this.bErrorDetected = true;
            break;    
        }

        zjd_cfg_t tCFG = {
            .arg = ptThis,
            .outfmt = this.Decoder.u3ZJDOutputColourFormat,
            .buf = this.Decoder.pWorkMemory,
            .buflen = __WORKING_MEMORY_SIZE__,
            .ifunc = __arm_zjpgd_loader_in_func,
            .ofunc = __arm_zjpgd_loader_write_to_vres_framebuffer,
        };
        if (ZJD_OK != zjd_init( &this.Decoder.tZDEC, 
                                &tCFG)) {
            this.bErrorDetected = true;
            break;
        }

        this.Decoder.bContextInitialized = false;

        /* update image size */
        this.vres.tTile.tRegion.tSize.iHeight = this.Decoder.tZDEC.height;
        this.vres.tTile.tRegion.tSize.iWidth = this.Decoder.tZDEC.width;

        /* reset context */
        memset(&this.tContext[0], 0, sizeof(this.tContext[0]));

    } while(0);

    if (this.bErrorDetected) {

        /* close low level IO */
        ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
            ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));

        if (NULL != this.Decoder.pWorkMemory) {
            __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, this.Decoder.pWorkMemory);
            this.Decoder.pWorkMemory = NULL;
        }

        return false;
    }

    return true;
}

static
void __arm_zjpgd_decode_fully(arm_zjpgd_loader_t *ptThis, 
                              arm_2d_region_t *ptRegion,
                              uint8_t *pchBuffer,
                              int16_t iStrideInByte)
{
    assert(NULL != ptThis);
    assert(NULL != ptRegion);
    
    if (!this.bInitialized) {
        return ;
    }

    this.bErrorDetected = false;
    //this.Decoder.nPosition = 0;

    /* allocate memory */
    do {
    
        //uint8_t chAlign = 0;
//        if (3 == this.u3PixelByteSize) {
//            chAlign = 1;
//        } else {
//            chAlign = this.u3PixelByteSize;
//        }

        if (!__arm_zjpgd_decode_prepare(ptThis)) {
            break;
        }


        this.ImageBuffer.pchBuffer = pchBuffer;
        this.iTargetStrideInByte = iStrideInByte;
        this.ImageBuffer.tRegion = *ptRegion;

        /* decoding */
        if (ZJD_OK != __arm_2d_zjpgd_decode(ptThis, ptRegion, false)) {

            this.ImageBuffer.pchBuffer = NULL;
            this.iTargetStrideInByte = 0;
            this.bErrorDetected = true;
            break;    
        }

        this.ImageBuffer.pchBuffer = NULL;
        this.iTargetStrideInByte = 0;

        /* close low level IO */
        ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
            ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));

        /* free scratch memory */
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, this.Decoder.pWorkMemory);
        this.Decoder.pWorkMemory = NULL;
        
    } while(0);

    if (this.bErrorDetected) {

        /* close low level IO */
        ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
            ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));

        if (NULL != this.Decoder.pWorkMemory) {
            __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, this.Decoder.pWorkMemory);
            this.Decoder.pWorkMemory = NULL;
        }
        if (NULL != this.ImageBuffer.pchBuffer) {
            __arm_2d_free_scratch_memory(this.tCFG.u2ScratchMemType, this.ImageBuffer.pchBuffer);
            this.ImageBuffer.pchBuffer = NULL;
        }
    }
}

ARM_NONNULL(1,2,3)
static
void __arm_zjpgd_decode_partial(arm_zjpgd_loader_t *ptThis, 
                                arm_2d_region_t *ptRegion,
                                uint8_t *pchBuffer,
                                int16_t iStrideInByte)
{
    assert(NULL != ptThis);
    assert(NULL != ptRegion);
    assert(NULL != pchBuffer);
    
    if (!this.bInitialized || this.bErrorDetected) {
        return ;
    }

    /* allocate memory */
    do {
    
//        uint8_t chAlign = 0;
//        if (3 == this.u3PixelByteSize) {
//            chAlign = 1;
//        } else {
//            chAlign = this.u3PixelByteSize;
//        }

        this.ImageBuffer.pchBuffer = pchBuffer;
        this.iTargetStrideInByte = iStrideInByte;
        this.ImageBuffer.tRegion = *ptRegion;

        /* decoding */
        if (ZJD_OK != __arm_2d_zjpgd_decode(ptThis, ptRegion, true)) {

            this.ImageBuffer.pchBuffer = NULL;
            this.iTargetStrideInByte = 0;
            this.bErrorDetected = true;
            break;    
        }

    } while(0);

    if (this.bErrorDetected) {

        /* close low level IO */
        ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
            ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));

        if (NULL != this.Decoder.pWorkMemory) {
            __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, this.Decoder.pWorkMemory);
            this.Decoder.pWorkMemory = NULL;
        }
        if (NULL != this.ImageBuffer.pchBuffer) {
            __arm_2d_free_scratch_memory(this.tCFG.u2ScratchMemType, this.ImageBuffer.pchBuffer);
            this.ImageBuffer.pchBuffer = NULL;
        }
    }
}

ARM_NONNULL(1)
void arm_zjpgd_loader_on_load( arm_zjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (!this.bInitialized) {
        return ;
    }

    this.bErrorDetected = false;
    //this.Decoder.nPosition = 0;

    /* allocate memory */
    do {
        //uint8_t chAlign = 0;
//        if (3 == this.u3PixelByteSize) {
//            chAlign = 1;
//        } else {
//            chAlign = this.u3PixelByteSize;
//        }
    
        if (!__arm_zjpgd_decode_prepare(ptThis)) {
            break;
        }

        if (ARM_ZJPGD_MODE_PARTIAL_DECODED != this.tCFG.u2WorkMode) {
            /* close low level IO */
            ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
                ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));

            /* free scratch memory */
            __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, this.Decoder.pWorkMemory);
            this.Decoder.pWorkMemory = NULL;
        }
        
    } while(0);

    if (this.bErrorDetected) {

        /* close low level IO */
        ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
            ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));

        if (NULL != this.Decoder.pWorkMemory) {
            __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, this.Decoder.pWorkMemory);
            this.Decoder.pWorkMemory = NULL;
        }
        if (NULL != this.ImageBuffer.pchBuffer) {
            __arm_2d_free_scratch_memory(this.tCFG.u2ScratchMemType, this.ImageBuffer.pchBuffer);
            this.ImageBuffer.pchBuffer = NULL;
        }
    }

}

ARM_NONNULL(1)
void arm_zjpgd_loader_depose( arm_zjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);

    if (!this.bInitialized) {
        return ;
    }

    if (ARM_ZJPGD_MODE_PARTIAL_DECODED == this.tCFG.u2WorkMode) {
        /* close low level IO */
        ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
            ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));

        this.ImageBuffer.pchBuffer = NULL;
    } else if (ARM_ZJPGD_MODE_PARTIAL_DECODED_TINY == this.tCFG.u2WorkMode) {
        assert(NULL == this.ImageBuffer.pchBuffer);
    }

    if (NULL != this.Decoder.pWorkMemory) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, this.Decoder.pWorkMemory);
        this.Decoder.pWorkMemory = NULL;
    }
    if (NULL != this.ImageBuffer.pchBuffer) {
        __arm_2d_free_scratch_memory(this.tCFG.u2ScratchMemType, this.ImageBuffer.pchBuffer);
        this.ImageBuffer.pchBuffer = NULL;
    }

    /* free the candidates list */
    do {
        arm_zjpgd_context_t *ptItem = NULL;

        arm_irq_safe {
            ARM_LIST_STACK_POP(this.Reference.ptCandidates, ptItem);
        }

        if (NULL == ptItem) {
            break;
        }

        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, ptItem);
    } while(1);

    /* free the reference working list */
    do {
        arm_zjpgd_context_t *ptItem = NULL;

        arm_irq_safe {
            ARM_LIST_STACK_POP(this.Reference.ptList, ptItem);
        }

        if (NULL == ptItem) {
            break;
        }

        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, ptItem);
    } while(1);

}

ARM_NONNULL(1)
void arm_zjpgd_loader_on_frame_start( arm_zjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);

    this.bIsNewFrame = true;

    if (!this.bInitialized) {
        return ;
    }

}

ARM_NONNULL(1)
void arm_zjpgd_loader_on_frame_complete( arm_zjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (!this.bInitialized) {
        return ;
    }

}


/*!
 *  \brief a method to load a specific part of an image
 *  \param[in] pTarget a reference of an user object
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] ptRegion the target region of the image
 *  \return intptr_t the address of a resource buffer which holds the content
 */
static
intptr_t __arm_zjpgd_vres_asset_loader( uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        arm_2d_region_t *ptRegion)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptVRES);
    arm_zjpgd_loader_t *ptThis = (arm_zjpgd_loader_t *)ptVRES;

    if (!this.bInitialized) {
        return (intptr_t)NULL;
    }

    COLOUR_INT *pBuffer = NULL;

#if 0
    size_t nPixelSize = sizeof(COLOUR_INT);
    size_t tBufferSize = 0;
    uint32_t nBytesPerLine = ptRegion->tSize.iWidth * nPixelSize;
    size_t nBitsPerPixel = sizeof(COLOUR_INT) << 3;

    if (0 != ptVRES->tTile.tColourInfo.chScheme) {
        nBitsPerPixel = (1 << ptVRES->tTile.tColourInfo.u3ColourSZ);
        if (ptVRES->tTile.tColourInfo.u3ColourSZ >= 3) {
            nPixelSize = (1 << (ptVRES->tTile.tColourInfo.u3ColourSZ - 3));
            nBytesPerLine = ptRegion->tSize.iWidth * nPixelSize;
        } else {
            /* for A1, A2 and A4 */
            size_t nPixelPerByte = 1 << (3 - ptVRES->tTile.tColourInfo.u3ColourSZ);
            int16_t iOffset = ptRegion->tLocation.iX & (nPixelPerByte - 1);
            
            uint32_t nBitsPerLine =  nBitsPerPixel * (iOffset + ptRegion->tSize.iWidth);
            nBytesPerLine = (nBitsPerLine + 7) >> 3;
        }
    }
#else
    size_t nPixelSize = this.u3PixelByteSize;
    size_t tBufferSize = 0;
    size_t nBitsPerPixel = this.u5BitsPerPixel;
    uint32_t nBytesPerLine = ptRegion->tSize.iWidth * nPixelSize;

    if (0 != ptVRES->tTile.tColourInfo.chScheme) {
        //nBitsPerPixel = (1 << ptVRES->tTile.tColourInfo.u3ColourSZ);
        if (ptVRES->tTile.tColourInfo.u3ColourSZ >= 3) {
            //nPixelSize = (1 << (ptVRES->tTile.tColourInfo.u3ColourSZ - 3));
            nBytesPerLine = ptRegion->tSize.iWidth * nPixelSize;
        } else {
            /* for A1, A2 and A4 */
            size_t nPixelPerByte = 1 << (3 - ptVRES->tTile.tColourInfo.u3ColourSZ);
            int16_t iOffset = ptRegion->tLocation.iX & (nPixelPerByte - 1);
            
            uint32_t nBitsPerLine =  nBitsPerPixel * (iOffset + ptRegion->tSize.iWidth);
            nBytesPerLine = (nBitsPerLine + 7) >> 3;
        }
    }
#endif

    /* background load mode */
    do {
        if (ptVRES->tTile.tInfo.u3ExtensionID != ARM_2D_TILE_EXTENSION_VRES) {
            break;
        }

        assert ((uintptr_t)NULL != ptVRES->tTile.nAddress);
        int16_t iTargetStride = ptVRES->tTile.tInfo.Extension.VRES.iTargetStride;

        switch(this.tCFG.u2WorkMode) {
            case ARM_ZJPGD_MODE_PARTIAL_DECODED:
                __arm_zjpgd_decode_partial( ptThis, 
                                            ptRegion, 
                                            ptVRES->tTile.pchBuffer, 
                                            iTargetStride * this.u3PixelByteSize );
                break;
            case ARM_ZJPGD_MODE_PARTIAL_DECODED_TINY:
                __arm_zjpgd_decode_fully(   ptThis, 
                                            ptRegion, 
                                            ptVRES->tTile.pchBuffer, 
                                            iTargetStride * this.u3PixelByteSize );
                break;
        }

        return ptVRES->tTile.nAddress;
    } while(0);

    /* default condition */
    tBufferSize = ptRegion->tSize.iHeight * nBytesPerLine;
    
    if (this.tCFG.bUseHeapForVRES) {
        pBuffer = __arm_2d_allocate_scratch_memory(tBufferSize, nPixelSize, this.tCFG.u2ScratchMemType);
        assert(NULL != pBuffer);

        if (NULL == pBuffer) {
            return (intptr_t)NULL;
        }
    } else if (NULL != this.tCFG.ptScene->ptPlayer) {
        arm_2d_pfb_t *ptPFB = __arm_2d_helper_pfb_new(&(this.tCFG.ptScene->ptPlayer->use_as__arm_2d_helper_pfb_t));
        assert(NULL != ptPFB);
        
        assert(ptPFB->u24Size >= tBufferSize);
        
        if (tBufferSize > ptPFB->u24Size) {
            return (intptr_t)NULL;
        }
        pBuffer = (COLOUR_INT *)((uintptr_t)ptPFB + sizeof(arm_2d_pfb_t));
    } else {
        return (intptr_t)NULL;
    }

    /* load content into the buffer */
    if (nBitsPerPixel < 8) {
    #if 0
        /* A1, A2 and A4 support */
        uintptr_t pSrc = __disp_adapter0_vres_get_asset_address(pTarget, ptVRES);
        uintptr_t pDes = (uintptr_t)pBuffer;
        
        uint32_t iBitsperLineInSource = ptVRES->tTile.tRegion.tSize.iWidth * nBitsPerPixel;
        int16_t iSourceStride = (int16_t)((uint32_t)(iBitsperLineInSource + 7) >> 3);

        /* calculate offset */
        pSrc += (ptRegion->tLocation.iY * iSourceStride);
        pSrc += (ptRegion->tLocation.iX * nBitsPerPixel) >> 3;
        
        for (int_fast16_t y = 0; y < ptRegion->tSize.iHeight; y++) {
            __disp_adapter0_vres_read_memory(   pTarget, 
                                                (void *)pDes, 
                                                (uintptr_t)pSrc, 
                                                nBytesPerLine);

            pDes += nBytesPerLine;
            pSrc += iSourceStride;
        }
    #else
        /* unsupport */
        assert(false);
    #endif
    } else {

        int16_t iTargetStride = ptRegion->tSize.iWidth;

        switch(this.tCFG.u2WorkMode) {
            case ARM_ZJPGD_MODE_PARTIAL_DECODED:
                __arm_zjpgd_decode_partial( ptThis, 
                                            ptRegion, 
                                            (uint8_t *)pBuffer, 
                                            iTargetStride * this.u3PixelByteSize );
                break;
            case ARM_ZJPGD_MODE_PARTIAL_DECODED_TINY:
                __arm_zjpgd_decode_fully(   ptThis, 
                                            ptRegion, 
                                            (uint8_t *)pBuffer, 
                                            iTargetStride * this.u3PixelByteSize );
                break;
        }

    } while(0);
    
    return (intptr_t)pBuffer;
}

/*!
*  \brief a method to despose the buffer
*  \param[in] pTarget a reference of an user object
*  \param[in] ptVRES a reference of this virtual resource
*  \param[in] pBuffer the target buffer
*/
static
void  __arm_zjpgd_vres_buffer_deposer(  uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        intptr_t pBuffer )
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptVRES);
    arm_zjpgd_loader_t *ptThis = (arm_zjpgd_loader_t *)ptVRES;

    if (!this.bInitialized) {
        return ;
    }

    if (this.tCFG.bUseHeapForVRES) {
        if ((intptr_t)NULL != pBuffer) {
            __arm_2d_free_scratch_memory(this.tCFG.u2ScratchMemType, (void *)pBuffer);
        }
    } else {

        if ((intptr_t)NULL == pBuffer) {
            return ;
        }
        
        arm_2d_pfb_t *ptPFB = (arm_2d_pfb_t *)((uintptr_t)pBuffer - sizeof(arm_2d_pfb_t));
        __arm_2d_helper_pfb_free(&(this.tCFG.ptScene->ptPlayer->use_as__arm_2d_helper_pfb_t), 
                                 ptPFB);
    }
}

/*----------------------------------------------------------------------------*
 * Context                                                                    *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1,2)
static
void __arm_zjpgd_context_add_to_list(   arm_zjpgd_context_t **ppList, 
                                        arm_zjpgd_context_t *ptItem,
                                        arm_2d_size_t tBlockSize,
                                        bool bBackward)
{
    assert(NULL != ppList);
    assert(NULL != ptItem);

    arm_2d_location_t tItemLocation = {
        .iX = ptItem->tLocation.iX & ~(tBlockSize.iWidth - 1),
        .iY = ptItem->tLocation.iY & ~(tBlockSize.iHeight - 1),
    };

    ptItem->tLocation = tItemLocation;

    while(NULL != *ppList) {

        arm_2d_location_t tTempLocation = {
            .iX = (*ppList)->tLocation.iX & ~(tBlockSize.iWidth - 1),
            .iY = (*ppList)->tLocation.iY & ~(tBlockSize.iHeight - 1),
        };

        if ((tTempLocation.iX == tItemLocation.iX)
        &&  (tTempLocation.iY == tItemLocation.iY)) {
            /* free the item */
            __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, ptItem);
            return ;
        }

        if (bBackward) {
            if (tTempLocation.iY > tItemLocation.iY) {
                goto label_next_item;
            }else if (  (tItemLocation.iY == tTempLocation.iY)
                    &&  (tTempLocation.iX > tItemLocation.iX)) {
                goto label_next_item;
            } /*if ( (tTempLocation.iY < tItemLocation.iY) 
                  || (   (tItemLocation.iY == tTempLocation.iY)
                     &&  (tTempLocation.iX < tItemLocation.iX))) */
            {

                /* insert here */
                arm_irq_safe {
                    ARM_LIST_INSERT_AFTER((*ppList), ptItem);
                }
                return ;
            }
        } else {
            if (tItemLocation.iY > tTempLocation.iY) {
                goto label_next_item;
            } else if ( (tItemLocation.iY == tTempLocation.iY)
                    &&  (tItemLocation.iX > tTempLocation.iX)) {
                goto label_next_item;
            } /*if ( (tItemLocation.iY < tTempLocation.iY) 
                  || (   (tItemLocation.iY == tTempLocation.iY)
                     &&  (tItemLocation.iX < tTempLocation.iX))) */
            {
                /* insert here */
                arm_irq_safe {
                    ARM_LIST_INSERT_AFTER((*ppList), ptItem);
                }
                return ;
            }
        }

label_next_item:
        ppList = &((*ppList)->ptNext);
    }

    /* insert to the tail */
    arm_irq_safe {
        ARM_LIST_INSERT_AFTER((*ppList), ptItem);
    }
}

ARM_NONNULL(1)
arm_2d_err_t arm_zjpgd_loader_add_reference_point_in_image( arm_zjpgd_loader_t *ptThis, 
                                                            arm_2d_location_t tLocation)
{
    assert(NULL != ptThis);
    if (!this.bInitialized 
    ||  this.bErrorDetected 
    ||  this.tCFG.u2WorkMode != ARM_ZJPGD_MODE_PARTIAL_DECODED) {
        return ARM_2D_ERR_INVALID_STATUS;
    }

    arm_2d_region_t tImageRegion = {
        .tSize = {
            .iWidth = this.Decoder.tZDEC.width,
            .iHeight = this.Decoder.tZDEC.height,
        },
    };

    if (!arm_2d_is_point_inside_region(&tImageRegion, &tLocation)) {
        return ARM_2D_ERR_OUT_OF_REGION;
    }

    arm_zjpgd_context_t *ptItem = 
        __arm_2d_allocate_scratch_memory(   sizeof(arm_zjpgd_context_t), 
                                            __alignof__(arm_zjpgd_context_t),
                                            ARM_2D_MEM_TYPE_FAST);

    if (NULL == ptItem) {
        return ARM_2D_ERR_INSUFFICIENT_RESOURCE;
    }

    memset(ptItem, 0, sizeof(arm_zjpgd_context_t));

    ptItem->tLocation = tLocation;

    arm_2d_size_t tBlockSize = {
        .iWidth = this.Decoder.tZDEC.msx * 8,
        .iHeight = this.Decoder.tZDEC.msy * 8,	
    };

    __arm_zjpgd_context_add_to_list(&this.Reference.ptCandidates, 
                                    ptItem, 
                                    tBlockSize,
                                    false);

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
arm_2d_err_t arm_zjpgd_loader_add_reference_point_on_canvas(
                                            arm_zjpgd_loader_t *ptThis,
                                            arm_2d_location_t tImageLocation,
                                            arm_2d_location_t tReferencePoint)
{
    tReferencePoint.iX -= tImageLocation.iX;
    tReferencePoint.iY -= tImageLocation.iY;

    return arm_zjpgd_loader_add_reference_point_in_image(ptThis, 
                                                         tReferencePoint);
}

ARM_NONNULL(1,2)
arm_2d_err_t arm_zjpgd_loader_add_reference_point_on_virtual_screen(
                            arm_zjpgd_loader_t *ptThis,
                            arm_2d_tile_t *ptTile,
                            arm_2d_location_t tImageLocationOnTile,
                            arm_2d_location_t tReferencePointOnVirtualScreen)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);

    return arm_zjpgd_loader_add_reference_point_on_canvas(  
            ptThis, 
            arm_2d_get_absolute_location(ptTile, tImageLocationOnTile, true),
            tReferencePointOnVirtualScreen);
}



ARM_NONNULL(1)
static
bool __arm_2d_zjpgd_is_context_before_the_target_region(arm_zjpgd_context_t *ptContext, 
                                                        arm_2d_location_t tLocation,
                                                        arm_2d_size_t tBlockSize)
{
    assert(NULL != ptContext);

    if ((ptContext->tLocation.iY <= tLocation.iY)
    &&  (ptContext->tLocation.iX <= tLocation.iX)) {
        return true;
    } else if ((ptContext->tLocation.iY + tBlockSize.iHeight) <= tLocation.iY) {
        return true;
    }
    
    return false;
}

ARM_NONNULL(1,2)
static
void __arm_zjpgd_save_context_to(   arm_zjpgd_loader_t *ptThis, 
                                    arm_zjpgd_context_t *ptContext,
                                    int16_t iX,
                                    int16_t iY)
{
    assert(NULL != ptThis);
    assert(NULL != ptContext);

    zjd_save(&this.Decoder.tZDEC, &ptContext->tSnapshot);
    ptContext->tLocation.iX = iX;
    ptContext->tLocation.iY = iY;
}

/*----------------------------------------------------------------------------*
 * ZJPGDec Extension                                                          *
 *----------------------------------------------------------------------------*/

static
zjd_res_t  __arm_2d_zjpgd_decode (
    arm_zjpgd_loader_t *ptThis,
    arm_2d_region_t *ptRegion,              /* Target Region inside the image */
    bool bUseContex                         /* whether use context */
)
{
    int16_t mx, my;

    zjd_t *jd = &this.Decoder.tZDEC;
    zjd_res_t rc = ZJD_OK;

    this.Decoder.tDrawRegion = (arm_2d_region_t){
        .tSize = {
            .iWidth = jd->width,
            .iHeight = jd->height,
        },
    };

#define __CHECK_CONTEXT(__CONTEXT_NAME)                                         \
        ARM_2D_LOG_INFO(                                                        \
            CONTROLS,                                                           \
            1,                                                                  \
            "ZJPGD",                                                            \
            "Check the context [" #__CONTEXT_NAME "] location: x=%d, y=%d...",  \
            this.tContext[__CONTEXT_NAME].tLocation.iX,                         \
            this.tContext[__CONTEXT_NAME].tLocation.iY                          \
        );                                                                      \
                                                                                \
        if (__arm_2d_zjpgd_is_context_before_the_target_region(                 \
                                        &this.tContext[__CONTEXT_NAME],         \
                                        this.Decoder.tDrawRegion.tLocation,     \
                                        this.Decoder.tBlockRegion.tSize)) {     \
                                                                                \
            ARM_2D_LOG_INFO(                                                    \
                CONTROLS,                                                       \
                2,                                                              \
                "ZJPGD",                                                        \
                "The the context [" #__CONTEXT_NAME                             \
                "] location is right before the target location!"               \
            );                                                                  \
                                                                                \
            /* use previous start point */                                      \
            this.tContext[JDEC_CONTEXT_CURRENT] = this.tContext[__CONTEXT_NAME];\
            break;                                                              \
        }

    if (NULL != ptRegion) {

        ARM_2D_LOG_INFO(
            CONTROLS, 
            0, 
            "ZJPGDec",
            "\r\nZJPGDec Decoding...\r\n"
            "==============================================================\r\n"
            "Interested Region: x=%d, y=%d, width=%d, height=%d",
            ptRegion->tLocation.iX,
            ptRegion->tLocation.iY,
            ptRegion->tSize.iWidth,
            ptRegion->tSize.iHeight
        );

        if (!arm_2d_region_intersect(&this.Decoder.tDrawRegion, ptRegion, &this.Decoder.tDrawRegion)) {

            ARM_2D_LOG_INFO(
                CONTROLS, 
                1, 
                "ZJPGDec", 
                "[SKIP] The interested region is outside of the image: width=%d, height=%d.",
                this.Decoder.tDrawRegion.tSize.iWidth,
                this.Decoder.tDrawRegion.tSize.iHeight
            );

            /* nothing to load */
            return ZJD_OK;
        }

        ARM_2D_LOG_INFO(
            CONTROLS, 
            1, 
            "ZJPGDec", 
            "Decoding Region: x=%d, y=%d, width=%d, height=%d",
            this.Decoder.tDrawRegion.tLocation.iX,
            this.Decoder.tDrawRegion.tLocation.iY,
            this.Decoder.tDrawRegion.tSize.iWidth,
            this.Decoder.tDrawRegion.tSize.iHeight
        );
    }

    mx = jd->msx * 8; my = jd->msy * 8;                                         /* Size of the MCU (pixel) */

    this.Decoder.tBlockRegion = (arm_2d_region_t){
        .tSize = {
            .iHeight = my,
            .iWidth = mx,
        }
    };

    if (bUseContex && this.Decoder.bContextInitialized) {

        do {

            ARM_2D_LOG_INFO(
                CONTROLS, 
                1, 
                "ZJPGDec", 
                "Check the current location: x=%d, y=%d...",
                this.tContext[JDEC_CONTEXT_CURRENT].tLocation.iX,
                this.tContext[JDEC_CONTEXT_CURRENT].tLocation.iY
            );

            if (__arm_2d_zjpgd_is_context_before_the_target_region(
                                            &this.tContext[JDEC_CONTEXT_CURRENT],
                                            this.Decoder.tDrawRegion.tLocation,
                                            this.Decoder.tBlockRegion.tSize)) {

                ARM_2D_LOG_INFO(
                    CONTROLS, 
                    2, 
                    "ZJPGDec", 
                    "The current location is right before the target location!"
                );

                /* next position */
                break;
            }

            __CHECK_CONTEXT(JDEC_CONTEXT_PREVIOUS_LINE);
            __CHECK_CONTEXT(JDEC_CONTEXT_PREVIOUS_START);
            

            ARM_2D_LOG_INFO(
                CONTROLS, 
                1, 
                "ZJPGDec", 
                "Check reference points..."
            );

            /* scan reference points */
            arm_zjpgd_context_t *ptReference = this.Reference.ptList;
            bool bFindReferencePoint = false;

            while(NULL != ptReference) {

                ARM_2D_LOG_INFO(
                    CONTROLS, 
                    2, 
                    "ZJPGDec", 
                    "Reference Point: x=%d, y=%d...",
                    ptReference->tLocation.iX,
                    ptReference->tLocation.iY
                );

                if (__arm_2d_zjpgd_is_context_before_the_target_region(
                                        ptReference,
                                        this.Decoder.tDrawRegion.tLocation,
                                        this.Decoder.tBlockRegion.tSize)) {
                    ARM_2D_LOG_INFO(
                        CONTROLS, 
                        3, 
                        "ZJPGDec", 
                        "The reference point is right before the target location!"
                    );
                    
                    /* find the reference point */
                    this.tContext[JDEC_CONTEXT_CURRENT] = *ptReference;
                    bFindReferencePoint = true;
                    break;
                }

                ptReference = ptReference->ptNext;
            }
            if (bFindReferencePoint) {
                break;
            }

            __CHECK_CONTEXT(JDEC_CONTEXT_PREVIOUS_FRAME_START);

            ARM_2D_LOG_INFO(
                CONTROLS, 
                1, 
                "ZJPGDec", 
                "Failed to find any context...Reset to the start."
            );

            /* use the very start point*/
            this.tContext[JDEC_CONTEXT_CURRENT] = this.tContext[JDEC_CONTEXT_START];
            this.tContext[JDEC_CONTEXT_PREVIOUS_START] = this.tContext[JDEC_CONTEXT_START];

        } while(0);

        ARM_2D_LOG_INFO(
            CONTROLS, 
            1, 
            "ZJPGDec", 
            "Decoding...\r\n"
            "---------------------------------------------"
        );

        /* resume context */

        this.Decoder.tBlockRegion.tLocation = this.tContext[JDEC_CONTEXT_CURRENT].tLocation;

        zjd_scan(jd, &this.tContext[JDEC_CONTEXT_CURRENT].tSnapshot, (const zjd_rect_t *)&this.Decoder.tDrawRegion);


    } else {
        zjd_scan(jd, NULL, (const zjd_rect_t *)&this.Decoder.tDrawRegion);
    }

    ARM_2D_LOG_INFO(
        CONTROLS, 
        1, 
        "ZJPGDec", 
        "End of Decoding...\r\n"
        "==============================================================\r\n"
    );

	return rc;
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
arm_2d_err_t arm_zjpgd_io_file_loader_init(arm_zjpgd_io_file_loader_t *ptThis, 
                                           const char *pchFilePath)
{
    if (NULL == ptThis || NULL == pchFilePath) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__file_exists(pchFilePath, "rb")) {
        return ARM_2D_ERR_IO_ERROR;
    }

    memset(ptThis, 0, sizeof(arm_zjpgd_io_file_loader_t));
    this.pchFilePath = pchFilePath;

    return ARM_2D_ERR_NONE;
}

static
bool __arm_zjpgd_io_fopen(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader)
{
    arm_zjpgd_io_file_loader_t *ptThis = (arm_zjpgd_io_file_loader_t *)pTarget;
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
void __arm_zjpgd_io_fclose(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader)
{
    arm_zjpgd_io_file_loader_t *ptThis = (arm_zjpgd_io_file_loader_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    fclose(this.phFile);
    this.phFile = NULL;
}

static
bool __arm_zjpgd_io_fseek(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader, int32_t offset, int32_t whence)
{
    arm_zjpgd_io_file_loader_t *ptThis = (arm_zjpgd_io_file_loader_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    return fseek(this.phFile, offset, whence) == 0;
}

static
size_t __arm_zjpgd_io_fread(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader, uint8_t *pchBuffer, size_t tSize)
{
    arm_zjpgd_io_file_loader_t *ptThis = (arm_zjpgd_io_file_loader_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    return fread(pchBuffer, 1, tSize, this.phFile);
}



extern
ARM_NONNULL(1, 2)
arm_2d_err_t arm_zjpgd_io_binary_loader_init(arm_zjpgd_io_binary_loader_t *ptThis, 
                                             const uint8_t *pchBinary,
                                             size_t tSize)
{
    if (NULL == ptThis || NULL == pchBinary || 0 == tSize) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    memset(ptThis, 0, sizeof(arm_zjpgd_io_binary_loader_t));
    this.pchBinary = (uint8_t *)pchBinary;
    this.tSize = tSize;

    return ARM_2D_ERR_NONE;
}

static
bool __arm_zjpgd_io_binary_open(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader)
{
    arm_zjpgd_io_binary_loader_t *ptThis = (arm_zjpgd_io_binary_loader_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);
    assert(NULL != this.pchBinary);
    assert(this.tSize > 0);

    this.tPostion = 0;

    return true;
}

static
void __arm_zjpgd_io_binary_close(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader)
{
    arm_zjpgd_io_binary_loader_t *ptThis = (arm_zjpgd_io_binary_loader_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    ARM_2D_UNUSED(ptThis);

}

static
bool __arm_zjpgd_io_binary_seek(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader, int32_t offset, int32_t whence)
{
    arm_zjpgd_io_binary_loader_t *ptThis = (arm_zjpgd_io_binary_loader_t *)pTarget;
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
size_t __arm_zjpgd_io_binary_read(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader, uint8_t *pchBuffer, size_t tSize)
{
    arm_zjpgd_io_binary_loader_t *ptThis = (arm_zjpgd_io_binary_loader_t *)pTarget;
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
