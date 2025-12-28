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
#define __GENERIC_LOADER_IMPLEMENT__

#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "arm_2d_generic_loader.h"
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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/


/*!
 *  \brief a method to load a specific part of an image
 *  \param[in] pTarget a reference of an user object
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] ptRegion the target region of the image
 *  \return intptr_t the address of a resource buffer which holds the content
 */
static
intptr_t __arm_generic_vres_asset_loader( uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        arm_2d_region_t *ptRegion);

/*!
 *  \brief a method to despose the buffer
 *  \param[in] pTarget a reference of an user object
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] pBuffer the target buffer
 */
static
void  __arm_generic_vres_buffer_deposer(  uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        intptr_t pBuffer );

static
arm_2d_err_t __arm_2d_generic_decode(   arm_generic_loader_t *ptThis, 
                                        arm_2d_region_t *ptRegion);

static
bool __arm_generic_decode_prepare(arm_generic_loader_t *ptThis);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t arm_generic_loader_init( arm_generic_loader_t *ptThis,
                                  arm_generic_loader_cfg_t *ptCFG)
{
    if (NULL == ptThis || NULL == ptCFG) {
        return ARM_2D_ERR_INVALID_PARAM;
    } else if (NULL == ptCFG->ptScene) {
        return ARM_2D_ERR_MISSING_PARAM;
    } else if ( (!ptCFG->bUseHeapForVRES)
             && (NULL == ptCFG->ptScene)) {
        return ARM_2D_ERR_MISSING_PARAM;
    }

    memset(ptThis, 0, sizeof(arm_generic_loader_t));

    this.tCFG = *ptCFG;

    this.vres.Load = &__arm_generic_vres_asset_loader;
    this.vres.Depose = &__arm_generic_vres_buffer_deposer;

    this.vres.tTile.tInfo.bIsRoot = true;
    this.vres.tTile.tInfo.bVirtualResource = true;

    if (this.tCFG.tColourInfo.chScheme == 0) {
        this.vres.tTile.tColourInfo.chScheme = ARM_2D_COLOUR;
    } else {
        this.vres.tTile.tColourInfo.chScheme = ptCFG->tColourInfo.chScheme;
    }
    this.vres.tTile.tInfo.bHasEnforcedColour = true;

    uint8_t chCurrentColourSize = (((ARM_2D_COLOUR & ARM_2D_COLOUR_SZ_msk)) >> ARM_2D_COLOUR_SZ_pos);
    if ( (this.vres.tTile.tColourInfo.u3ColourSZ > chCurrentColourSize)
      && (this.vres.tTile.tColourInfo.u3ColourSZ < ARM_2D_COLOUR_SZ_24BIT)) {
        this.tCFG.bUseHeapForVRES = true;
    }

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
        this.u5BitsPerPixel = nBitsPerPixel - 1;

    } while(0);

    this.bInitialized = true;

    /* try to decode */
    do {
        this.bErrorDetected = false;
        //this.Decoder.nPosition = 0;
        
        __arm_generic_decode_prepare(ptThis);

        if (NULL != this.tCFG.ImageIO.ptIO) {
            /* close low level IO */
            ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
                ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));
        }

        /* free scratch memory */
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, this.Decoder.pWorkMemory);
        this.Decoder.pWorkMemory = NULL;
        
        if (this.bErrorDetected) {
            return ARM_2D_ERR_IO_ERROR;
        }
    } while(0);

    return ARM_2D_ERR_NONE;
}


bool arm_generic_loader_io_seek(uintptr_t pTarget, 
                                int32_t offset, 
                                int32_t whence)
{
    arm_generic_loader_t *ptThis = (arm_generic_loader_t *)pTarget;

    if (NULL != this.tCFG.ImageIO.ptIO) {
        return ARM_2D_INVOKE(this.tCFG.ImageIO.ptIO->fnSeek, 
                    ARM_2D_PARAM(   this.tCFG.ImageIO.pTarget, 
                                    ptThis, 
                                    offset, 
                                    whence));
    }

    return false;
}

size_t arm_generic_loader_io_read (   uintptr_t pTarget,       
                                    uint8_t *pchBuffer,
                                    size_t tLength)
{
    arm_generic_loader_t *ptThis = (arm_generic_loader_t *)pTarget;

    size_t tResult = tLength;
    if (NULL != pchBuffer) {

        if (NULL != this.tCFG.ImageIO.ptIO) {
            tResult = ARM_2D_INVOKE(this.tCFG.ImageIO.ptIO->fnRead, 
                        ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis, pchBuffer, tLength));
        } else {
            tResult = 0;
        }
    }

    return tResult;
}

static
bool __arm_generic_decode_prepare(arm_generic_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (!this.bInitialized) {
        return false;
    }

    this.bErrorDetected = false;

    /* allocate memory */
    do {
        if (this.tCFG.hwWorkingMemorySize > 0) {
            this.Decoder.pWorkMemory = 
                (void*)__arm_2d_allocate_scratch_memory(this.tCFG.hwWorkingMemorySize, 
                                                        4, 
                                                        ARM_2D_MEM_TYPE_FAST);
            if (NULL == this.Decoder.pWorkMemory) {
                this.bErrorDetected = true;
                break;
            }
        }     

        if (NULL != this.tCFG.ImageIO.ptIO) {
            /* open low level IO */
            if (!ARM_2D_INVOKE(this.tCFG.ImageIO.ptIO->fnOpen,
                    ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis))) {
                this.bErrorDetected = true;
                break;    
            }
        }

        if (ARM_2D_ERR_NONE != ARM_2D_INVOKE(this.tCFG.UserDecoder.fnDecoderInit, 
                                ARM_2D_PARAM(ptThis))) {
            this.bErrorDetected = true;
            break;
        }

    } while(0);

    if (this.bErrorDetected) {

        if (NULL != this.tCFG.ImageIO.ptIO) {
            /* close low level IO */
            ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
                ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));
        }

        if (NULL != this.Decoder.pWorkMemory) {
            __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, this.Decoder.pWorkMemory);
            this.Decoder.pWorkMemory = NULL;
        }

        return false;
    }

    return true;
}

ARM_NONNULL(1,2,3)
static
void __arm_generic_decode_partial(  arm_generic_loader_t *ptThis, 
                                    arm_2d_region_t *ptRegion,
                                    uint8_t *pchBuffer,
                                    uint16_t hwStrideInByte)
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
        this.hwTargetStrideInByte = hwStrideInByte;
        this.ImageBuffer.tRegion = *ptRegion;

        /* decoding */
        if (ARM_2D_ERR_NONE != __arm_2d_generic_decode( ptThis, 
                                                        ptRegion)) {

            this.ImageBuffer.pchBuffer = NULL;
            this.hwTargetStrideInByte = 0;
            this.bErrorDetected = true;
            break;    
        }

    } while(0);

    if (this.bErrorDetected) {

        if (NULL != this.tCFG.ImageIO.ptIO) {
            /* close low level IO */
            ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
                ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));
        }

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
void arm_generic_loader_on_load( arm_generic_loader_t *ptThis)
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
    
        if (!__arm_generic_decode_prepare(ptThis)) {
            break;
        }
        
    } while(0);


    if (this.bErrorDetected) {

        if (NULL != this.tCFG.ImageIO.ptIO) {
            /* close low level IO */
            ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
                ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));
        }

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
void arm_generic_loader_depose( arm_generic_loader_t *ptThis)
{
    assert(NULL != ptThis);

    if (!this.bInitialized) {
        return ;
    }

    if (NULL != this.tCFG.ImageIO.ptIO) {
        /* close low level IO */
        ARM_2D_INVOKE_RT_VOID(this.tCFG.ImageIO.ptIO->fnClose,
            ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis));
    }

    this.ImageBuffer.pchBuffer = NULL;

    if (NULL != this.Decoder.pWorkMemory) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_FAST, this.Decoder.pWorkMemory);
        this.Decoder.pWorkMemory = NULL;
    }
    if (NULL != this.ImageBuffer.pchBuffer) {
        __arm_2d_free_scratch_memory(this.tCFG.u2ScratchMemType, this.ImageBuffer.pchBuffer);
        this.ImageBuffer.pchBuffer = NULL;
    }


}

ARM_NONNULL(1)
void arm_generic_loader_on_frame_start( arm_generic_loader_t *ptThis)
{
    assert(NULL != ptThis);

    this.bIsNewFrame = true;

    if (!this.bInitialized) {
        return ;
    }

}

ARM_NONNULL(1)
void arm_generic_loader_on_frame_complete( arm_generic_loader_t *ptThis)
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
intptr_t __arm_generic_vres_asset_loader( uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        arm_2d_region_t *ptRegion)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptVRES);
    arm_generic_loader_t *ptThis = (arm_generic_loader_t *)ptVRES;

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
    size_t nBitsPerPixel = this.u5BitsPerPixel + 1;
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

        __arm_generic_decode_partial( ptThis, 
                                    ptRegion, 
                                    ptVRES->tTile.pchBuffer, 
                                    (uint32_t)iTargetStride * this.u3PixelByteSize );

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

    if (this.tCFG.bBlendWithBG) {
        switch(nBitsPerPixel) {
            case 1:
                if (this.tCFG.tBackgroundColour.chColour & 0x01) {
                    memset( pBuffer,  0xFF, tBufferSize);
                } else {
                    memset( pBuffer,  0x00, tBufferSize);
                }
                break;
            case 2: {
                    uint8_t chColour = 0;
                    switch (this.tCFG.tBackgroundColour.chColour & 0x3) {
                        case 0:
                            break;
                        case 1:
                            chColour = 0x55;
                            break;
                        case 2:
                            chColour = 0xAA;
                            break;
                        case 3:
                            chColour = 0xFF;
                            break;
                    }
                    memset( pBuffer,  chColour, tBufferSize);
                }
                break;
            case 4:
                memset( pBuffer, 
                        (   (this.tCFG.tBackgroundColour.chColour & 0xF)
                        |   ((this.tCFG.tBackgroundColour.chColour & 0xF) << 4)),
                        tBufferSize);
            case 8:
                memset(pBuffer, this.tCFG.tBackgroundColour.chColour, tBufferSize);
                break;
            case 16: {
                    uint16_t *phwSource = (uint16_t *)pBuffer;
                    size_t tPixels = ptRegion->tSize.iWidth * ptRegion->tSize.iHeight;
                    do {
                        *phwSource++ = this.tCFG.tBackgroundColour.hwColour;
                    } while(--tPixels);
                }
                break;
            case 24: {
                    uint8_t *pchSource = (uint8_t *)pBuffer;
                    size_t tPixels = ptRegion->tSize.iWidth * ptRegion->tSize.iHeight;
                    do {
                        *pchSource++ = this.tCFG.tBackgroundColour.chChannels[0];
                        *pchSource++ = this.tCFG.tBackgroundColour.chChannels[1];
                        *pchSource++ = this.tCFG.tBackgroundColour.chChannels[2];
                    } while(--tPixels);
                }
                break;
            case 32: {
                    uint32_t *pwSource = (uint32_t *)pBuffer;
                    size_t tPixels = ptRegion->tSize.iWidth * ptRegion->tSize.iHeight;
                    do {
                        *pwSource++ = this.tCFG.tBackgroundColour.wColour;
                    } while(--tPixels);
                }
                break;
            default:
                assert(false);
                break;
        }
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

        uint16_t hwTargetStride = ptRegion->tSize.iWidth;

        __arm_generic_decode_partial(   ptThis, 
                                        ptRegion, 
                                        (uint8_t *)pBuffer, 
                                        hwTargetStride * this.u3PixelByteSize );

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
void  __arm_generic_vres_buffer_deposer(  uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        intptr_t pBuffer )
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptVRES);
    arm_generic_loader_t *ptThis = (arm_generic_loader_t *)ptVRES;

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
 * GENERIC Extension                                                          *
 *----------------------------------------------------------------------------*/

static
arm_2d_err_t  __arm_2d_generic_decode (
    arm_generic_loader_t *ptThis,
    arm_2d_region_t *ptRegion)          /* Target Region inside the image */

{
    arm_2d_err_t tResult = ARM_2D_ERR_NONE;
    
    this.Decoder.tDrawRegion = (arm_2d_region_t){
        .tSize = this.vres.tTile.tRegion.tSize,
    };

    if (NULL != ptRegion) {

        ARM_2D_LOG_INFO(
            CONTROLS, 
            0, 
            "GENERIC",
            "\r\nGENERICec Decoding...\r\n"
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
                "GENERIC", 
                "[SKIP] The interested region is outside of the image: width=%d, height=%d.",
                this.Decoder.tDrawRegion.tSize.iWidth,
                this.Decoder.tDrawRegion.tSize.iHeight
            );

            /* nothing to load */
            return ARM_2D_ERR_NONE;
        }

        ARM_2D_LOG_INFO(
            CONTROLS, 
            1, 
            "GENERIC", 
            "Decoding Region: x=%d, y=%d, width=%d, height=%d",
            this.Decoder.tDrawRegion.tLocation.iX,
            this.Decoder.tDrawRegion.tLocation.iY,
            this.Decoder.tDrawRegion.tSize.iWidth,
            this.Decoder.tDrawRegion.tSize.iHeight
        );
    }

    ARM_2D_LOG_INFO(
        CONTROLS, 
        1, 
        "GENERIC", 
        "Decoding...\r\n"
        "---------------------------------------------"
    );

   tResult = this.tCFG.UserDecoder.fnDecode( ptThis, 
                                        &this.Decoder.tDrawRegion,
                                        this.ImageBuffer.pchBuffer,
                                        this.hwTargetStrideInByte, 
                                        this.u5BitsPerPixel + 1);

    ARM_2D_LOG_INFO(
        CONTROLS, 
        1, 
        "GENERIC", 
        "End of Decoding...\r\n"
        "==============================================================\r\n"
    );


	return tResult;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
