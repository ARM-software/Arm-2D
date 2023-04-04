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
#include <stdio.h>
#include "platform.h"
#include "example_gui.h"
#include "arm_extra_controls.h"
#include "arm_2d_helper.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif

/*============================ MACROS ========================================*/
#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileHelium           c_tileHeliumGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileHelium           c_tileHeliumRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileHelium           c_tileHeliumCCCA8888
#else
#   error Unsupported colour depth!
#endif

#ifndef __VIRTUAL_RESOURCE_DEMO_USE_HEAP__
#   define __VIRTUAL_RESOURCE_DEMO_USE_HEAP__       0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct {
    intptr_t pResource;
} resource_loader_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileHelium ;


/*============================ PROTOTYPES ====================================*/
/*!
 *  \brief a method to load a specific part of an image
 *  \param[in] pTarget a reference of an user object 
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] ptRegion the target region of the image
 *  \return intptr_t the address of a resource buffer which holds the content
 */
static intptr_t __vres_asset_loader   (uintptr_t pTarget, 
                                    arm_2d_vres_t *ptVRES, 
                                    arm_2d_region_t *ptRegion);
    
/*!
 *  \brief a method to despose the buffer
 *  \param[in] pTarget a reference of an user object 
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] pBuffer the target buffer
 */
static void __vres_buffer_deposer (  uintptr_t pTarget, 
                                arm_2d_vres_t *ptVRES, 
                                intptr_t pBuffer );
                            
/*============================ LOCAL VARIABLES ===============================*/

static resource_loader_t s_tLoader = {
    .pResource = (intptr_t)&c_tileHelium,
};

#define impl_vres(__COLOUR_FORMAT, __WIDTH, __HEIGHT, ...)                      \
{                                                                               \
    .tTile = {                                                                  \
        .tRegion = {                                                            \
            .tSize = {                                                          \
                .iWidth = (__WIDTH),                                            \
                .iHeight =(__HEIGHT),                                           \
            },                                                                  \
        },                                                                      \
        .tInfo = {                                                              \
            .bIsRoot = true,                                                    \
            .bHasEnforcedColour = true,                                         \
            .bVirtualResource = true,                                           \
            .tColourInfo = {                                                    \
                .chScheme = (__COLOUR_FORMAT),                                  \
            },                                                                  \
        },                                                                      \
    },                                                                          \
    .Load       = &__vres_asset_loader,                                         \
    .Depose     = &__vres_buffer_deposer,                                       \
    __VA_ARGS__                                                                 \
}

static arm_2d_vres_t s_tBigImage = 
    impl_vres(   
        ARM_2D_COLOUR, 
        320, 
        256, 
        .pTarget = (uintptr_t)&s_tLoader
    );

static
const arm_2d_tile_t c_tChildImage = {
    .tRegion = {
        .tLocation = {
            .iX = 160,
            .iY = 128,
        },
        .tSize = {
            .iWidth = 160,
            .iHeight = 128,
        },
    },
    .tInfo = {
        .bIsRoot = false,
        .bDerivedResource = true,
    },
    .ptParent = (arm_2d_tile_t *)&s_tBigImage.tTile,
};

/*============================ IMPLEMENTATION ================================*/


void arm_2d_helper_vres_read_memory(intptr_t pObj, 
                                    COLOUR_INT *pBuffer,
                                    uintptr_t pAddress,
                                    size_t nSizeInByte)
{
    ARM_2D_UNUSED(pObj);
    memcpy(pBuffer, (void * const)pAddress, nSizeInByte);
}

uintptr_t arm_2d_helper_vres_get_asset_address(uintptr_t pObj,
                                               arm_2d_vres_t *ptVRES)
{
    ARM_2D_UNUSED(ptVRES);
    resource_loader_t *ptLoader = (resource_loader_t *)pObj;
    arm_2d_tile_t *ptTile = (arm_2d_tile_t *)ptLoader->pResource;
    
    return ptTile->nAddress;
}


extern
void arm_2d_helper_vres_read_memory(intptr_t pObj, 
                                    COLOUR_INT *pBuffer,
                                    uintptr_t pAddress,
                                    size_t nSizeInByte);

extern 
uintptr_t arm_2d_helper_vres_get_asset_address(uintptr_t pObj,
                                               arm_2d_vres_t *ptVRES);
                                               
/*!
 *  \brief a method to load a specific part of an image
 *  \param[in] pTarget a reference of an user object 
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] ptRegion the target region of the image
 *  \return intptr_t the address of a resource buffer which holds the content
 */
static intptr_t __vres_asset_loader   (  uintptr_t pObj, 
                                    arm_2d_vres_t *ptVRES, 
                                    arm_2d_region_t *ptRegion)
{
    
    size_t tBufferSize = ptRegion->tSize.iHeight * ptRegion->tSize.iWidth * sizeof(COLOUR_INT);
    COLOUR_INT *pBuffer = NULL;

#if __VIRTUAL_RESOURCE_DEMO_USE_HEAP__
    pBuffer = malloc(tBufferSize);
    assert(NULL != pBuffer);
    
    if (NULL == pBuffer) {
        return (intptr_t)NULL;
    }
#else
    static COLOUR_INT s_tImageBuffer[PFB_BLOCK_WIDTH * PFB_BLOCK_HEIGHT];
    pBuffer = s_tImageBuffer;
    assert(sizeof(s_tImageBuffer) >= tBufferSize);
    
    if (tBufferSize > sizeof(s_tImageBuffer)) {
        return (intptr_t)NULL;
    }
#endif
    /* load content into the buffer */
    /* this part of code is just simple a demo, you should implement your own */
    do {
        
        COLOUR_INT *pSrc = 
            (COLOUR_INT *)arm_2d_helper_vres_get_asset_address(pObj, ptVRES);
        COLOUR_INT *pDes = pBuffer;
        int16_t iSourceStride = ptVRES->tTile.tRegion.tSize.iWidth;
        int16_t iTargetStride = ptRegion->tSize.iWidth;
        /* calculate offset */
        pSrc += ptRegion->tLocation.iY * iSourceStride + ptRegion->tLocation.iX;
        
        for (int_fast16_t y = 0; y < ptRegion->tSize.iHeight; y++) {
            arm_2d_helper_vres_read_memory( pObj, 
                                            pDes, 
                                            (uintptr_t)pSrc, 
                                            sizeof(COLOUR_INT) * iTargetStride);
            
            pDes += iTargetStride;
            pSrc += iSourceStride;
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
static void __vres_buffer_deposer (  uintptr_t pTarget, 
                                arm_2d_vres_t *ptVRES, 
                                intptr_t pBuffer )
{
#if __VIRTUAL_RESOURCE_DEMO_USE_HEAP__
    resource_loader_t *ptLoader = (resource_loader_t *)pTarget;
    
    if ((intptr_t)NULL != pBuffer) {
        free((void *)pBuffer);
    }
#else
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptVRES);
    ARM_2D_UNUSED(pBuffer);
#endif
}


void example_gui_init(void)
{
    arm_extra_controls_init();
    
}


void example_gui_do_events(void)
{

}


__WEAK 
void example_gui_on_refresh_evt_handler(const arm_2d_tile_t *ptFrameBuffer)
{
     ARM_2D_UNUSED(ptFrameBuffer);
}


void example_gui_refresh(const arm_2d_tile_t *ptTile, bool bIsNewFrame)
{
    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_NAVY);

    arm_2d_canvas(ptTile, __canvas) {
        arm_2d_align_centre(__canvas, s_tBigImage.tTile.tRegion.tSize) {
            arm_2d_tile_copy(   &s_tBigImage.tTile,     /* source tile */
                                ptTile,          /* target frame buffer */
                                &__centre_region, 
                                ARM_2D_CP_MODE_COPY);

            arm_2d_op_wait_async(NULL);

            arm_2d_tile_copy(   &c_tChildImage,         /* source tile */
                                ptTile,          /* target frame buffer */
                                &__centre_region, 
                                ARM_2D_CP_MODE_XY_MIRROR);
        }

        arm_2d_op_wait_async(NULL);

        busy_wheel2_show(ptTile, bIsNewFrame);
        //spinning_wheel_show(ptFrameBuffer, bIsNewFrame);
    }
    
    example_gui_on_refresh_evt_handler(ptTile);
    
    arm_2d_op_wait_async(NULL);
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


