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
#include <stdio.h>

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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

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
    this.vres.tTile.tColourInfo = this.tCFG.tColourInfo;
    this.vres.tTile.tRegion.tSize = this.tCFG.tSize;
    this.vres.tTile.tInfo.bIsRoot = true;
    this.vres.tTile.tInfo.bVirtualResource = true;
    this.vres.tTile.tInfo.bHasEnforcedColour = true;

    if (0 == this.vres.tTile.tColourInfo.chScheme) {
        /* invalid colour scheme, use default one */
        this.vres.tTile.tColourInfo.chScheme = ARM_2D_COLOUR;
    }

    this.bInitialized = true;

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
void arm_tjpgd_loader_depose( arm_tjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);

    if (!this.bInitialized) {
        return ;
    }

}

ARM_NONNULL(1)
void arm_tjpgd_loader_on_load( arm_tjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (!this.bInitialized) {
        return ;
    }

}

ARM_NONNULL(1)
void arm_tjpgd_loader_on_frame_start( arm_tjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);

    if (!this.bInitialized) {
        return ;
    }
}

ARM_NONNULL(1)
void arm_tjpgd_loader_on_frame_complete( arm_tjpgd_loader_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (!this.bInitialized) {
        return ;
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
    #if 0
        __disp_adapter0_vres_read_memory( 
                                        pObj, 
                                        (void *)pDes, 
                                        (uintptr_t)pSrc, 
                                        iPixelSize * iSourceWidth);
    #endif
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

    /* background load mode */
    do {
        if (ptVRES->tTile.tInfo.u3ExtensionID != ARM_2D_TILE_EXTENSION_VRES) {
            break;
        }

        assert ((uintptr_t)NULL != ptVRES->tTile.nAddress);

        //uintptr_t pSrc = __disp_adapter0_vres_get_asset_address(pTarget, ptVRES);
        uintptr_t pSrc = pTarget;
        uintptr_t pDes = (uintptr_t)ptVRES->tTile.nAddress;
        int16_t iTargetStride = ptVRES->tTile.tInfo.Extension.VRES.iTargetStride;
        int16_t iSourceStride = ptVRES->tTile.tRegion.tSize.iWidth;
    
        __arm_tjpgd_vres_asset_2dcopy(  pTarget, 
                                        ptVRES, 
                                        ptRegion, 
                                        pSrc, 
                                        pDes, 
                                        iTargetStride, 
                                        iSourceStride, 
                                        nPixelSize);

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
        //uintptr_t pSrc = __disp_adapter0_vres_get_asset_address(pTarget, ptVRES);
        uintptr_t pSrc = pTarget;
        uintptr_t pDes = (uintptr_t)pBuffer;
        int16_t iTargetStride = ptRegion->tSize.iWidth;
        int16_t iSourceStride = ptVRES->tTile.tRegion.tSize.iWidth;

        __arm_tjpgd_vres_asset_2dcopy(  pTarget, 
                                        ptVRES, 
                                        ptRegion, 
                                        pSrc, 
                                        pDes, 
                                        iTargetStride, 
                                        iSourceStride, 
                                        nPixelSize);
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

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
