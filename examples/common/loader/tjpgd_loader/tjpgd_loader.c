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
#define __TJPGD_LOADER_IMPLEMENT__

#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "tjpgd_loader.h"
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
#endif

/*============================ MACROS ========================================*/


#undef this
#define this    (*ptThis)

#if JD_FASTDECODE == 2
#   define __WORKING_MEMORY_SIZE__   (3300 - 512 + JD_SZBUF + (6<<10))
#else
#   define __WORKING_MEMORY_SIZE__   (3300 - 512 + JD_SZBUF)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

extern
JRESULT restart (
	JDEC* jd,		/* Pointer to the decompressor object */
	uint16_t rstn	/* Expected restert sequense number */
);

extern
JRESULT mcu_output (
	JDEC* jd,			/* Pointer to the decompressor object */
	int (*outfunc)(JDEC*, void*, JRECT*),	/* RGB output function */
	unsigned int x,		/* MCU location in the image */
	unsigned int y		/* MCU location in the image */
);

extern
JRESULT mcu_load (
	JDEC* jd		/* Pointer to the decompressor object */
);

/*!
 *  \brief a method to load a specific part of an image
 *  \param[in] pTarget a reference of an user object
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] ptRegion the target region of the image
 *  \return intptr_t the address of a resource buffer which holds the content
 */
static
intptr_t __arm_tjpgd_vres_asset_loader( uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        arm_2d_region_t *ptRegion);

/*!
 *  \brief a method to despose the buffer
 *  \param[in] pTarget a reference of an user object
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] pBuffer the target buffer
 */
static
void  __arm_tjpgd_vres_buffer_deposer(  uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        intptr_t pBuffer );
static
bool   __arm_tjpgd_io_fopen(uintptr_t pTarget, arm_tjpgd_loader_t *ptLoader);

static
void   __arm_tjpgd_io_fclose(uintptr_t pTarget, arm_tjpgd_loader_t *ptLoader);

static
bool   __arm_tjpgd_io_fseek(uintptr_t pTarget, arm_tjpgd_loader_t *ptLoader, int32_t offset, int32_t whence);

static
size_t __arm_tjpgd_io_fread(uintptr_t pTarget, arm_tjpgd_loader_t *ptLoader, uint8_t *pchBuffer, size_t tSize);

static
JRESULT jd_decomp_rect (
	JDEC* jd,								/* Initialized decompression object */
	int (*outfunc)(JDEC*, void*, JRECT*),	/* RGB output function */
	uint8_t scale,							/* Output de-scaling factor (0 to 3) */
    arm_2d_region_t *ptRegion               /* Target Region inside the image */
);
/*============================ GLOBAL VARIABLES ==============================*/

const arm_tjpgd_loader_io_t ARM_TJPGD_LOADER_IO_FILE = {
    .fnOpen =   &__arm_tjpgd_io_fopen,
    .fnClose =  &__arm_tjpgd_io_fclose,
    .fnSeek =   &__arm_tjpgd_io_fseek,
    .fnRead =   &__arm_tjpgd_io_fread,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t arm_tjpgd_loader_init( arm_tjpgd_loader_t *ptThis,
                                    arm_tjpgd_loader_cfg_t *ptCFG)
{
    if (NULL == ptThis || NULL == ptCFG) {
        return ARM_2D_ERR_INVALID_PARAM;
    } else if (NULL == ptCFG->ptScene) {
        return ARM_2D_ERR_MISSING_PARAM;
    } else if ( (!ptCFG->bUseHeapForVRES)
             && (NULL == ptCFG->ptScene->ptPlayer)) {
        return ARM_2D_ERR_MISSING_PARAM;
    } else if (NULL == ptCFG->ImageIO.ptIO) {
        return ARM_2D_ERR_MISSING_PARAM;
    }

    memset(ptThis, 0, sizeof(arm_tjpgd_loader_t));

    this.tCFG = *ptCFG;

    this.vres.Load = &__arm_tjpgd_vres_asset_loader;
    this.vres.Depose = &__arm_tjpgd_vres_buffer_deposer;
    this.vres.tTile.tColourInfo.chScheme = ARM_2D_COLOUR;

    this.vres.tTile.tInfo.bIsRoot = true;
    this.vres.tTile.tInfo.bVirtualResource = true;
    this.vres.tTile.tInfo.bHasEnforcedColour = true;

    if (0 == this.vres.tTile.tColourInfo.chScheme) {
        /* invalid colour scheme, use default one */
        this.vres.tTile.tColourInfo.chScheme = ARM_2D_COLOUR;
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
                size_t nPixelPerByte = 1 << (3 - this.vres.tTile.tColourInfo.u3ColourSZ);
                //int16_t iOffset = ptRegion->tLocation.iX & (nPixelPerByte - 1);
                
                //uint32_t nBitsPerLine =  nBitsPerPixel * (iOffset + ptRegion->tSize.iWidth);
                //nBytesPerLine = (nBitsPerLine + 7) >> 3;
            }
        }

        this.u3PixelByteSize = nPixelSize;
        this.u5BitsPerPixel = nBitsPerPixel;

    } while(0);

    this.bInitialized = true;

    return ARM_2D_ERR_NONE;
}

static
size_t __arm_tjpgd_loader_in_func ( JDEC *ptDecoder,       
                                    uint8_t *pchBuffer,
                                    size_t nSize)
{
    arm_tjpgd_loader_t *ptThis = (arm_tjpgd_loader_t *)ptDecoder->device;

    size_t nResult = 0;
    if (NULL == pchBuffer) {
        nResult =  
            ARM_2D_INVOKE(this.tCFG.ImageIO.ptIO->fnSeek, 
                ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis, nSize, SEEK_CUR)) ? nSize : 0;
    } else {
        nResult = ARM_2D_INVOKE(this.tCFG.ImageIO.ptIO->fnRead, 
                    ARM_2D_PARAM(this.tCFG.ImageIO.pTarget, ptThis, pchBuffer, nSize));
    }

    this.Decoder.nPosition += nResult;
    return nResult;
}

static
int __arm_tjpgd_loader_write_to_full_framebuffer (      /* Returns 1 to continue, 0 to abort */
    JDEC* ptDecoder,        /* Decompression object */
    void* pSource,          /* Bitmap data to be output */
    JRECT* rect             /* Rectangle region of output image */
)
{
    arm_tjpgd_loader_t *ptThis = (arm_tjpgd_loader_t *)ptDecoder->device;

    /* Copy the output image rectangle to the frame buffer */
    uint8_t *pchSrc = (uint8_t *)pSource;
    uint8_t *pchDes = this.ImageBuffer.pchBuffer 
                    + this.u3PixelByteSize * rect->left
                    + rect->top * this.iTargetStrideInByte;                                                        
    int16_t iSourceStrideInByte = this.u3PixelByteSize * (rect->right - rect->left + 1);
    int16_t iTargetStrideInByte = this.iTargetStrideInByte;

    int16_t iHeight = rect->bottom - rect->top + 1;

    for (int_fast16_t y = 0; y < iHeight; y++) {
        memcpy(pchDes, pchSrc, iSourceStrideInByte);    /* Copy a line */

        pchSrc += iSourceStrideInByte; 
        pchDes += iTargetStrideInByte;                  /* Next line */
    }

    return 1;    /* Continue to decompress */
}

ARM_NONNULL(1,2)
void arm_2d_normal_root_tile_copy(  const arm_2d_tile_t *ptSource, 
                                    arm_2d_tile_t *ptTarget, 
                                    const arm_2d_region_t *ptTargetRegion,
                                    uint_fast8_t chBytesPerPixel)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);
    assert(ptSource->tInfo.bIsRoot);
    assert(ptTarget->tInfo.bIsRoot);
    assert(false == ptSource->tInfo.bVirtualResource);
    assert(false == ptSource->tInfo.bVirtualResource);

    assert(chBytesPerPixel > 0);

    arm_2d_region_t tTargetRegion = {
        .tSize = ptTarget->tRegion.tSize,
    };

    /* get a valid tTargetRegion */
    if (NULL == ptTargetRegion) {
        ptTargetRegion = &tTargetRegion;
    } else {
        if (!arm_2d_region_intersect(&tTargetRegion, ptTargetRegion, &tTargetRegion)) {
            /* out of region */
            return ;
        }
    }

    int16_t iHeight = MIN(ptSource->tRegion.tSize.iHeight, tTargetRegion.tSize.iHeight);
    int16_t iWidth = MIN(ptSource->tRegion.tSize.iWidth, tTargetRegion.tSize.iWidth);

    uint32_t wCopyStrideInByte = iWidth * chBytesPerPixel;
    uint32_t wSourceStrideInByte = ptSource->tRegion.tSize.iWidth * chBytesPerPixel;
    uint32_t wTargetStrideInByte = ptTarget->tRegion.tSize.iWidth * chBytesPerPixel;

    /* offset target address */
    uintptr_t pTarget = ptTarget->nAddress 
                      + tTargetRegion.tLocation.iY * wTargetStrideInByte
                      + tTargetRegion.tLocation.iX * chBytesPerPixel;

    /* offset source address */
    uintptr_t pSource = ptSource->nAddress;
    if (ptTargetRegion->tLocation.iY < 0) {
        pSource += (-ptTargetRegion->tLocation.iY) * wSourceStrideInByte;
    }
    if (ptTargetRegion->tLocation.iX < 0) {
        pSource += (-ptTargetRegion->tLocation.iX) * chBytesPerPixel;
    }

    switch (chBytesPerPixel) {
        case 3:
        case 1:
            for (int_fast16_t y = 0; y < iHeight; y++) {

                memcpy((void *)pTarget, (void *)pSource, wCopyStrideInByte);
        
                pSource += wSourceStrideInByte;
                pTarget += wTargetStrideInByte;
            }
            break;
        case 2:
            for (int_fast16_t y = 0; y < iHeight; y++) {

                memcpy((uint16_t *)pTarget, (uint16_t *)pSource, wCopyStrideInByte);
        
                pSource += wSourceStrideInByte;
                pTarget += wTargetStrideInByte;
            }
            break;

        case 4:
            for (int_fast16_t y = 0; y < iHeight; y++) {

                memcpy((uint32_t *)pTarget, (uint32_t *)pSource, wCopyStrideInByte);
        
                pSource += wSourceStrideInByte;
                pTarget += wTargetStrideInByte;
            }
            break;
        default:
            break;
    }
}


static
int __arm_tjpgd_loader_write_to_vres_framebuffer (      /* Returns 1 to continue, 0 to abort */
    JDEC* ptDecoder,        /* Decompression object */
    void* pSource,          /* Bitmap data to be output */
    JRECT* rect             /* Rectangle region of output image */
)
{
    arm_tjpgd_loader_t *ptThis = (arm_tjpgd_loader_t *)ptDecoder->device;

    ARM_2D_UNUSED(rect);
    arm_2d_region_t tBlockRegion = this.Decoder.tBlockRegion;

    assert(tBlockRegion.tLocation.iX == rect->left);
    assert(tBlockRegion.tLocation.iY == rect->top);
    assert(tBlockRegion.tSize.iWidth == rect->right - rect->left + 1);
    assert(tBlockRegion.tSize.iHeight == rect->bottom - rect->top + 1);

    /* create source tile */
    arm_2d_tile_t tSourceTile = {
        .tInfo.bIsRoot = true,
        .tRegion = {
            .tSize = tBlockRegion.tSize,
        },
        .nAddress = (uintptr_t)pSource,
    };

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
        arm_2d_region_t tTempRegion1 = {
            .tSize = tTargetRegion.tSize,
        };

        if (!arm_2d_region_intersect(&tTempRegion1, &tBlockRegion, &tTempRegion1)) {
            return 1;
        }

        arm_2d_region_t tTempRegion2 = {
            .tSize = tTargetTile.tRegion.tSize,
        };
        arm_2d_region_intersect(&tTempRegion2, &tBlockRegion, &tTempRegion2);

        tBlockRegion.tSize.iWidth -= tTempRegion2.tSize.iWidth - tTempRegion1.tSize.iWidth;
    }

    arm_2d_normal_root_tile_copy(&tSourceTile, &tTargetTile, &tBlockRegion, this.u3PixelByteSize);

    return 1;    /* Continue to decompress */
}


static
void __arm_tjpgd_decode_fully(arm_tjpgd_loader_t *ptThis, 
                              arm_2d_region_t *ptRegion,
                              uint8_t *pchBuffer,
                              int16_t iStrideInByte)
{
    assert(NULL != ptThis);

    assert(NULL != ptThis);
    
    if (!this.bInitialized) {
        return ;
    }

    this.bErrorDetected = false;
    this.Decoder.nPosition = 0;

    /* allocate memory */
    do {
        uint8_t chAlign = 0;
        if (3 == this.u3PixelByteSize) {
            chAlign = 1;
        } else {
            chAlign = this.u3PixelByteSize;
        }

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

        if (JDR_OK != jd_prepare(&this.Decoder.tJDEC, 
                                    &__arm_tjpgd_loader_in_func, 
                                    this.Decoder.pWorkMemory, 
                                    __WORKING_MEMORY_SIZE__, 
                                    ptThis)) {
            this.bErrorDetected = true;
            break;
        }

        /* update image size */
        this.vres.tTile.tRegion.tSize.iHeight = this.Decoder.tJDEC.height;
        this.vres.tTile.tRegion.tSize.iWidth = this.Decoder.tJDEC.width;
        
        /* allocate scratch memory */
        if (NULL == ptRegion) {
            this.iTargetStrideInByte = this.vres.tTile.tRegion.tSize.iWidth * this.u3PixelByteSize;
            size_t tSize =this.vres.tTile.tRegion.tSize.iHeight * this.vres.tTile.tRegion.tSize.iWidth * this.u3PixelByteSize;
            this.ImageBuffer.pchBuffer = __arm_2d_allocate_scratch_memory(tSize, chAlign, this.tCFG.u2ScratchMemType);

            if (NULL == this.ImageBuffer.pchBuffer) {
                this.bErrorDetected = true;
                break ;
            } else {
                this.ImageBuffer.tSize = tSize;
            }

            /* decoding */
            if (JDR_OK != jd_decomp_rect( &this.Decoder.tJDEC, 
                                          __arm_tjpgd_loader_write_to_full_framebuffer, 
                                          0,
                                          NULL)) {
                this.bErrorDetected = true;
                break;    
            }
        } else {
            this.ImageBuffer.pchBuffer = pchBuffer;
            this.iTargetStrideInByte = iStrideInByte;
            this.ImageBuffer.tRegion = *ptRegion;

            /* decoding */
            if (JDR_OK != jd_decomp_rect(   &this.Decoder.tJDEC, 
                                            __arm_tjpgd_loader_write_to_vres_framebuffer, 
                                            0,
                                            ptRegion)) {

                this.ImageBuffer.pchBuffer = NULL;
                this.iTargetStrideInByte = 0;
                this.bErrorDetected = true;
                break;    
            }

            this.ImageBuffer.pchBuffer = NULL;
            this.iTargetStrideInByte = 0;
        }

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

ARM_NONNULL(1)
void arm_tjpgd_loader_on_load( arm_tjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (!this.bInitialized) {
        return ;
    }

    this.bErrorDetected = false;
    this.Decoder.nPosition = 0;

    /* allocate memory */
    do {
        uint8_t chAlign = 0;
        if (3 == this.u3PixelByteSize) {
            chAlign = 1;
        } else {
            chAlign = this.u3PixelByteSize;
        }
    
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

        if (JDR_OK != jd_prepare(&this.Decoder.tJDEC, 
                                    &__arm_tjpgd_loader_in_func, 
                                    this.Decoder.pWorkMemory, 
                                    __WORKING_MEMORY_SIZE__, 
                                    ptThis)) {
            this.bErrorDetected = true;
            break;
        }

        /* update image size */
        this.vres.tTile.tRegion.tSize.iHeight = this.Decoder.tJDEC.height;
        this.vres.tTile.tRegion.tSize.iWidth = this.Decoder.tJDEC.width;

        /* decode now */
        if (ARM_TJPGD_MODE_FULLY_DECODED_ONCE == this.tCFG.u2WorkMode) {
            size_t tSize =this.vres.tTile.tRegion.tSize.iHeight * this.vres.tTile.tRegion.tSize.iWidth * this.u3PixelByteSize;

            this.ImageBuffer.pchBuffer = __arm_2d_allocate_scratch_memory(tSize, chAlign, this.tCFG.u2ScratchMemType);

            if (NULL == this.ImageBuffer.pchBuffer) {
                this.bErrorDetected = true;
                break ;
            } else {
                this.ImageBuffer.tSize = tSize;
            }

            this.iTargetStrideInByte = this.vres.tTile.tRegion.tSize.iWidth * this.u3PixelByteSize;

            /* decoding */
            if (JDR_OK != jd_decomp( &this.Decoder.tJDEC, 
                            __arm_tjpgd_loader_write_to_full_framebuffer, 
                            0)) {

                this.bErrorDetected = true;
                break;    
            }

            
        }

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

ARM_NONNULL(1)
void arm_tjpgd_loader_depose( arm_tjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);

    if (!this.bInitialized) {
        return ;
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

ARM_NONNULL(1)
void arm_tjpgd_loader_on_frame_start( arm_tjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);

    if (!this.bInitialized) {
        return ;
    }

    if (ARM_TJPGD_MODE_FULLY_DECODED_EACH_FRAME == this.tCFG.u2WorkMode) {
        __arm_tjpgd_decode_fully(ptThis, NULL, NULL, 0);
    }
}

ARM_NONNULL(1)
void arm_tjpgd_loader_on_frame_complete( arm_tjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (!this.bInitialized) {
        return ;
    }

    if (ARM_TJPGD_MODE_FULLY_DECODED_EACH_FRAME == this.tCFG.u2WorkMode) {
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

static 
void __arm_tjpgd_vres_asset_2dcopy( uintptr_t pObj,
                                        arm_2d_vres_t *ptVRES,
                                        arm_2d_region_t *ptRegion,
                                        uintptr_t pSrc,
                                        uintptr_t pDes,
                                        int16_t iTargetStride,
                                        int16_t iSourceStride,
                                        int16_t iPixelSize)
{
    assert(NULL != ptRegion);
    assert(NULL != ptVRES);

    int16_t iSourceWidth = ptRegion->tSize.iWidth;
    int16_t iSourceHeight = ptRegion->tSize.iHeight;

    /* calculate offset */
    pSrc += (ptRegion->tLocation.iY * iSourceStride + ptRegion->tLocation.iX) * iPixelSize;
    
    for (int_fast16_t y = 0; y < iSourceHeight; y++) {

        memcpy((void *)pDes, (const void *)pSrc, iPixelSize * iSourceWidth);

        pDes += iTargetStride * iPixelSize;
        pSrc += iSourceStride * iPixelSize;
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
intptr_t __arm_tjpgd_vres_asset_loader( uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        arm_2d_region_t *ptRegion)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptVRES);
    arm_tjpgd_loader_t *ptThis = (arm_tjpgd_loader_t *)ptVRES;

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

        if (    (ARM_TJPGD_MODE_FULLY_DECODED_ONCE == this.tCFG.u2WorkMode)
          ||    (ARM_TJPGD_MODE_FULLY_DECODED_EACH_FRAME == this.tCFG.u2WorkMode)) {
            uintptr_t pSrc = (uintptr_t)this.ImageBuffer.pchBuffer;
        
            uintptr_t pDes = (uintptr_t)ptVRES->tTile.nAddress;
            
            int16_t iSourceStride = ptVRES->tTile.tRegion.tSize.iWidth;
        
            __arm_tjpgd_vres_asset_2dcopy(  pTarget, 
                                            ptVRES, 
                                            ptRegion, 
                                            pSrc, 
                                            pDes, 
                                            iTargetStride, 
                                            iSourceStride, 
                                            nPixelSize);
        } else if (ARM_TJPGD_MODE_PARTIAL_DECODED_TINY == this.tCFG.u2WorkMode) {

            __arm_tjpgd_decode_fully(   ptThis, 
                                        ptRegion, 
                                        ptVRES->tTile.pchBuffer, 
                                        iTargetStride * this.u3PixelByteSize );

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
    } else {
        arm_2d_pfb_t *ptPFB = __arm_2d_helper_pfb_new(&DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t);
        assert(NULL != ptPFB);
        
        assert(ptPFB->u24Size >= tBufferSize);
        
        if (tBufferSize > ptPFB->u24Size) {
            return (intptr_t)NULL;
        }
        pBuffer = (COLOUR_INT *)((uintptr_t)ptPFB + sizeof(arm_2d_pfb_t));
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
        if (    (ARM_TJPGD_MODE_FULLY_DECODED_ONCE == this.tCFG.u2WorkMode)
          ||    (ARM_TJPGD_MODE_FULLY_DECODED_EACH_FRAME == this.tCFG.u2WorkMode)) {
            uintptr_t pSrc = (uintptr_t)this.ImageBuffer.pchBuffer;
        

            uintptr_t pDes = (uintptr_t)pBuffer;
            
            int16_t iSourceStride = ptVRES->tTile.tRegion.tSize.iWidth;

            __arm_tjpgd_vres_asset_2dcopy(  pTarget, 
                                            ptVRES, 
                                            ptRegion, 
                                            pSrc, 
                                            pDes, 
                                            iTargetStride, 
                                            iSourceStride, 
                                            nPixelSize);
        } else if (ARM_TJPGD_MODE_PARTIAL_DECODED_TINY == this.tCFG.u2WorkMode) {

            __arm_tjpgd_decode_fully(ptThis, ptRegion, (uint8_t *)pBuffer, iTargetStride * this.u3PixelByteSize );

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
void  __arm_tjpgd_vres_buffer_deposer(  uintptr_t pTarget,
                                        arm_2d_vres_t *ptVRES,
                                        intptr_t pBuffer )
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptVRES);
    arm_tjpgd_loader_t *ptThis = (arm_tjpgd_loader_t *)ptVRES;

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
 * TJpgDec Extension                                                          *
 *----------------------------------------------------------------------------*/

static
JRESULT jd_decomp_rect (
	JDEC* jd,								/* Initialized decompression object */
	int (*outfunc)(JDEC*, void*, JRECT*),	/* RGB output function */
	uint8_t scale,							/* Output de-scaling factor (0 to 3) */
    arm_2d_region_t *ptRegion               /* Target Region inside the image */
)
{
    arm_tjpgd_loader_t *ptThis = (arm_tjpgd_loader_t *)jd->device;

	int16_t x, y, mx, my;
	uint16_t rst, rsc;
	JRESULT rc;

    arm_2d_region_t tDrawRegion = {
        .tSize = {
            .iWidth = jd->width,
            .iHeight = jd->height,
        },
    };

    if (NULL != ptRegion) {
        if (!arm_2d_region_intersect(&tDrawRegion, ptRegion, &tDrawRegion)) {
            /* nothing to load */
            return JDR_OK;
        }
    }

	if (scale > (JD_USE_SCALE ? 3 : 0)) {
        return JDR_PAR;
    }
	jd->scale = scale;

	mx = jd->msx * 8; my = jd->msy * 8;			/* Size of the MCU (pixel) */

    this.Decoder.tBlockRegion = (arm_2d_region_t){
        .tSize = {
            .iHeight = my,
            .iWidth = mx,
        }
    };

	jd->dcv[2] = jd->dcv[1] = jd->dcv[0] = 0;	/* Initialize DC values */
	rst = rsc = 0;

	rc = JDR_OK;
	for (y = 0; y < jd->height; y += my) {		/* Vertical loop of MCUs */
        this.Decoder.tBlockRegion.tLocation.iY = y;

		for (x = 0; x < jd->width; x += mx) {	/* Horizontal loop of MCUs */
            this.Decoder.tBlockRegion.tLocation.iX = x;

			if (jd->nrst && rst++ == jd->nrst) {	/* Process restart interval if enabled */
				rc = restart(jd, rsc++);
				if (rc != JDR_OK) return rc;
				rst = 1;
			}
			rc = mcu_load(jd);					/* Load an MCU (decompress huffman coded stream, dequantize and apply IDCT) */
			if (rc != JDR_OK) return rc;

            if (!arm_2d_region_intersect(&this.Decoder.tBlockRegion, &tDrawRegion, NULL)) {

                if (    this.Decoder.tBlockRegion.tLocation.iY 
                   >=   (tDrawRegion.tLocation.iY + tDrawRegion.tSize.iHeight)) {
                    /* lower than the bottom of the draw region, terminate the job earlier */
                    return rc;
                }
                continue;
            }

			rc = mcu_output(jd, outfunc, x, y);	/* Output the MCU (YCbCr to RGB, scaling and output) */
			if (rc != JDR_OK) return rc;
		}
	}

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

extern
ARM_NONNULL(1, 2)
arm_2d_err_t arm_tjpgd_io_file_init(arm_tjpgd_io_file_t *ptThis, 
                                    const char *pchFilePath)
{
    if (NULL == ptThis || NULL == pchFilePath) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__file_exists(pchFilePath, "rb")) {
        return ARM_2D_ERR_IO_ERROR;
    }

    memset(ptThis, 0, sizeof(arm_tjpgd_io_file_t));
    this.pchFilePath = pchFilePath;

    return ARM_2D_ERR_NONE;
}

static
bool   __arm_tjpgd_io_fopen(uintptr_t pTarget, arm_tjpgd_loader_t *ptLoader)
{
    arm_tjpgd_io_file_t *ptThis = (arm_tjpgd_io_file_t *)pTarget;
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
void   __arm_tjpgd_io_fclose(uintptr_t pTarget, arm_tjpgd_loader_t *ptLoader)
{
    arm_tjpgd_io_file_t *ptThis = (arm_tjpgd_io_file_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    fclose(this.phFile);
    this.phFile = NULL;
}

static
bool   __arm_tjpgd_io_fseek(uintptr_t pTarget, arm_tjpgd_loader_t *ptLoader, int32_t offset, int32_t whence)
{
    arm_tjpgd_io_file_t *ptThis = (arm_tjpgd_io_file_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    return fseek(this.phFile, offset, whence) == 0;
}

static
size_t __arm_tjpgd_io_fread(uintptr_t pTarget, arm_tjpgd_loader_t *ptLoader, uint8_t *pchBuffer, size_t tSize)
{
    arm_tjpgd_io_file_t *ptThis = (arm_tjpgd_io_file_t *)pTarget;
    ARM_2D_UNUSED(ptLoader);
    assert(NULL != ptThis);

    return fread(pchBuffer, 1, tSize, this.phFile);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
