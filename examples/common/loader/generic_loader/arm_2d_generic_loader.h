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

#ifndef __ARM_2D_GENERIC_LOADER_H__
#define __ARM_2D_GENERIC_LOADER_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include <stdio.h>
#include "../__arm_2d_loader_common.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/*============================ MACROS ========================================*/

/* OOC header, please DO NOT modify  */
#ifdef __GENERIC_LOADER_IMPLEMENT__
#   undef   __GENERIC_LOADER_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__GENERIC_LOADER_INHERIT__)
#   undef   __GENERIC_LOADER_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct arm_generic_loader_t arm_generic_loader_t;

typedef struct arm_generic_loader_cfg_t {

    uint16_t bUseHeapForVRES    : 1;
    uint16_t bBlendWithBG       : 1;    
    uint16_t u2ScratchMemType   : 2;
    uint16_t                    : 5;
    uint16_t                    : 8;

    uint16_t hwWorkingMemorySize;

    arm_2d_color_info_t tColourInfo;
    COLOUR_TYPE_T tBackgroundColour;

    struct {
        const arm_loader_io_t *ptIO;
        uintptr_t pTarget;
    } ImageIO;

    struct {
        arm_2d_err_t (*fnDecoderInit)(arm_generic_loader_t *ptThis);
        arm_2d_err_t (*fnDecode)(arm_generic_loader_t *ptThis,
                                 arm_2d_region_t *ptROI,
                                 uint8_t *pchBuffer,
                                 uint32_t iTargetStrideInByte,
                                 uint_fast8_t chBitsPerPixel);
    } UserDecoder;

    arm_2d_scene_t *ptScene;
} arm_generic_loader_cfg_t;

/*!
 * \brief a user class for user defined control
 */

struct arm_generic_loader_t {
    inherit_ex(arm_2d_vres_t, vres);
    arm_generic_loader_cfg_t tCFG;

ARM_PROTECTED(
    
    struct {
        uint8_t *pchBuffer;
        size_t tSize;
        arm_2d_region_t tRegion;
    } ImageBuffer;


    uint16_t u3PixelByteSize        : 3;
    uint16_t u5BitsPerPixel         : 5;
    uint16_t                        : 4;
    uint16_t                        : 1;
    uint16_t bIsNewFrame            : 1;
    uint16_t bErrorDetected         : 1;
    uint16_t bInitialized           : 1;

    uint16_t hwTargetStrideInByte;

)
    struct {
        void *pWorkMemory;

        arm_2d_region_t tDrawRegion;
    } Decoder;
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern
ARM_NONNULL(1)
arm_2d_err_t arm_generic_loader_init(   arm_generic_loader_t *ptThis,
                                    arm_generic_loader_cfg_t *ptCFG);

extern
ARM_NONNULL(1)
void arm_generic_loader_depose( arm_generic_loader_t *ptThis);

extern
ARM_NONNULL(1)
void arm_generic_loader_on_load( arm_generic_loader_t *ptThis);

extern
ARM_NONNULL(1)
void arm_generic_loader_on_frame_start( arm_generic_loader_t *ptThis);

extern
ARM_NONNULL(1)
void arm_generic_loader_on_frame_complete( arm_generic_loader_t *ptThis);

extern
bool arm_generic_loader_io_seek(uintptr_t pTarget, 
                                int32_t offset, 
                                int32_t whence);

extern
size_t arm_generic_loader_io_read ( uintptr_t pTarget,       
                                    uint8_t *pchBuffer,
                                    size_t tLength);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
