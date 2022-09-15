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

#ifndef __ARM_2D_DISP_ADAPTER%Instance%_H__
#define __ARM_2D_DISP_ADAPTER%Instance%_H__

#include "arm_2d.h"

#ifdef __RTE_ACCELERATION_ARM_2D_HELPER_DISP_ADAPTER%Instance%__

#include "arm_2d_helper_scene.h"
#include "__common.h"

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <o> Select the screen colour depth
//     <8=>     8 Bits
//     <16=>    16Bits
//     <32=>    32Bits
// <i> The colour depth of your screen
#ifndef __DISP%Instance%_CFG_COLOUR_DEPTH__
#   define __DISP%Instance%_CFG_COLOUR_DEPTH__              16
#endif

// <o>Width of the screen <8-32767>
// <i> The width of your screen
// <i> Default: 320
#ifndef __DISP%Instance%_CFG_SCEEN_WIDTH__
#   define __DISP%Instance%_CFG_SCEEN_WIDTH__               320
#endif

// <o>Height of the screen <8-32767>
// <i> The height of your screen
// <i> Default: 240
#ifndef __DISP%Instance%_CFG_SCEEN_HEIGHT__
#   define __DISP%Instance%_CFG_SCEEN_HEIGHT__              240
#endif

// <o>Width of the PFB block
// <i> The width of your PFB block size used in disp%Instance%
#ifndef __DISP%Instance%_CFG_PFB_BLOCK_WIDTH__
#   define __DISP%Instance%_CFG_PFB_BLOCK_WIDTH__           320
#endif

// <o>Height of the PFB block
// <i> The height of your PFB block size used in disp%Instance%
#ifndef __DISP%Instance%_CFG_PFB_BLOCK_HEIGHT__
#   define __DISP%Instance%_CFG_PFB_BLOCK_HEIGHT__          240
#endif

// <o>PFB Block Count <1-65535>
// <i> The number of blocks in the PFB pool.
#ifndef __DISP%Instance%_CFG_PFB_HEAP_SIZE__
#   define __DISP%Instance%_CFG_PFB_HEAP_SIZE__             1
#endif

// <o>Number of iterations <0-2000>
// <i> run number of iterations before calculate the FPS.
#ifndef __DISP%Instance%_CFG_ITERATION_CNT__
#   define __DISP%Instance%_CFG_ITERATION_CNT__             30
#endif

// <q>Disable the default scene
// <i> Remove the default scene for this display adapter. We highly recommend you to disable the default scene when creating real applications.
#ifndef __DISP%Instance%_CFG_DISABLE_DEFAULT_SCENE__
#   define __DISP%Instance%_CFG_DISABLE_DEFAULT_SCENE__     0
#endif

// <q>Enable the virtual resource helper service
// <i> Introduce a helper service for loading virtual resources.
// <i> This feature is disabled by default.
#ifndef __DISP%Instance%_CFG_VIRTUAL_RESOURCE_HELPER__
#   define __DISP%Instance%_CFG_VIRTUAL_RESOURCE_HELPER__   0
#endif

// <q>Use heap to allocate buffer in the virtual resource helper service
// <i> Use malloc and free in the virtual resource helper service. When disabled, a static buffer in the size of current display adapter PFB will be used. 
// <i> This feature is disabled by default.
#ifndef __DISP%Instance%_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__
#   define __DISP%Instance%_CFG_USE_HEAP_FOR_VIRTUAL_RESOURCE_HELPER__   0
#endif
// <<< end of configuration section >>>


/*============================ MACROFIED FUNCTIONS ===========================*/

#if __DISP%Instance%_CFG_VIRTUAL_RESOURCE_HELPER__
#define disp_adapter%Instance%_impl_vres(__COLOUR_FORMAT, __WIDTH, __HEIGHT,...)\
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
    .Load       = &__disp_adapter%Instance%_vres_asset_loader,                  \
    .Depose     = &__disp_adapter%Instance%_vres_buffer_deposer,                \
    __VA_ARGS__                                                                 \
}
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
ARM_NOINIT
extern
arm_2d_scene_player_t DISP%Instance%_ADAPTER;

/*============================ PROTOTYPES ====================================*/

extern
void disp_adapter%Instance%_init(void);

extern
arm_fsm_rt_t disp_adapter%Instance%_task(void);


#if __DISP%Instance%_CFG_VIRTUAL_RESOURCE_HELPER__
/*!
 *  \brief a method to load a specific part of an image
 *  \note It is NOT an API for users to call, plese leave it alone
 *
 *  \param[in] pTarget a reference of an user object 
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] ptRegion the target region of the image
 *  \return intptr_t the address of a resource buffer which holds the content
 */
intptr_t __disp_adapter%Instance%_vres_asset_loader   (
                                                uintptr_t pTarget, 
                                                arm_2d_vres_t *ptVRES, 
                                                arm_2d_region_t *ptRegion);
    
/*!
 *  \brief a method to despose the buffer
 *  \note It is NOT an API for users to call, plese leave it alone
 *
 *  \param[in] pTarget a reference of an user object 
 *  \param[in] ptVRES a reference of this virtual resource
 *  \param[in] pBuffer the target buffer
 */
void __disp_adapter%Instance%_vres_buffer_deposer (  uintptr_t pTarget, 
                                                            arm_2d_vres_t *ptVRES, 
                                                            intptr_t pBuffer );

/*!
 * \brief A user implemented function to return the address for specific asset
 *        stored in external memory, e.g. SPI Flash
 * \note You MUST provide an implementation when 
 *       __DISP%Instance%_CFG_VIRTUAL_RESOURCE_HELPER__ is enabled(1)
 *
 * \param[in] pObj an pointer of user defined object, it is used for OOC
 * \note You can ignore pObj if you don't care/don't use OOC 
 *
 * \param[in] ptVRES the target virtual resource object
 * \return uintptr_t the address of the target asset in external memory
 */
extern
uintptr_t __disp_adapter%Instance%_vres_get_asset_address(
                                                        uintptr_t pObj,
                                                        arm_2d_vres_t *ptVRES);

/*!
 * \brief A user implemented function to copy content from external memory
 *        (e.g. SPI Flash) to a local buffer with specified address and size.
 *
 * \note You MUST provide an implementation when 
 *       __DISP%Instance%_CFG_VIRTUAL_RESOURCE_HELPER__ is enabled(1)
 *
 * \param[in] pObj an pointer of user defined object, it is used for OOC
 * \note You can ignore pObj if you don't care/don't use OOC 
 *
 * \param[in] pBuffer the address of the local buffer
 * \param[in] pAddress the address in the external memory
 * \param[in] nSizeInByte number of bytes to read
 */
extern
void __disp_adapter%Instance%_vres_read_memory( intptr_t pObj, 
                                                void *pBuffer,
                                                uintptr_t pAddress,
                                                size_t nSizeInByte);

#endif

#endif

#endif
