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

#ifndef __ZJPGD_LOADER_H__
#define __ZJPGD_LOADER_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include <stdio.h>
#include "./zjpgd/zjpgd.h"

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
#ifdef __ZJPGD_LOADER_IMPLEMENT__
#   undef   __ZJPGD_LOADER_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__ZJPGD_LOADER_INHERIT__)
#   undef   __ZJPGD_LOADER_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define __arm_zjpgd_loader_add_reference_point1(__ZJPGD_LD_PTR,                 \
                                                __REF_PT)                       \
            arm_zjpgd_loader_add_reference_point_in_image((__ZJPGD_LD_PTR),     \
                                                          (__REF_PT))

#define __arm_zjpgd_loader_add_reference_point2(__ZJPGD_LD_PTR,                 \
                                                __IMG_LOCATION,                 \
                                                __REF_PT)                       \
            arm_zjpgd_loader_add_reference_point_on_canvas((__ZJPGD_LD_PTR),    \
                                                           (__IMG_LOCATION),    \
                                                           (__REF_PT))

#define __arm_zjpgd_loader_add_reference_point3(__ZJPGD_LD_PTR,                 \
                                                __TILE_PTR,                     \
                                                __IMG_LOCATION,                 \
                                                __REF_PT)                       \
    arm_zjpgd_loader_add_reference_point_on_virtual_screen( (__ZJPGD_LD_PTR),   \
                                                            (__TILE_PTR),       \
                                                            (__IMG_LOCATION),   \
                                                            (__REF_PT))

/*!
 * \brief add reference point for a given ZJPGDec Loader. 
 * 
 * \note prototype 1:
 *      arm_zjpgd_loader_add_reference_point(
 *                                      <ZJPGDec Loader pointer>,
 *                                      <Reference Point in the target image>);
 * 
 * \note prototype 2:
 *      arm_zjpgd_loader_add_reference_point(
 *                                      <ZJPGDec Loader pointer>,
 *                                      <Image Location on a canvas>
 *                                      <Reference Point on the same canvas>);
 *
 * \note prototype 3:
 *      arm_zjpgd_loader_add_reference_point(
 *                                      <ZJPGDec Loader pointer>,
 *                                      <The target tile pointer>
 *                                      <Image Location on the target tile>
 *                                      <Reference Point on the virtual screen>);
 */
#define arm_zjpgd_loader_add_reference_point(__ZJPGD_LD_PTR, ...)               \
            ARM_CONNECT2(   __arm_zjpgd_loader_add_reference_point,             \
                __ARM_VA_NUM_ARGS(__VA_ARGS__))((__ZJPGD_LD_PTR),               \
                                                 __VA_ARGS__)

/*============================ TYPES =========================================*/

typedef struct arm_zjpgd_context_t arm_zjpgd_context_t;

struct arm_zjpgd_context_t {
ARM_PRIVATE (
    arm_zjpgd_context_t *ptNext;
    zjd_ctx_t tSnapshot;

    arm_2d_location_t tLocation;
)
};

typedef enum {
    /*! slow, and only consume a small scratch memory. If you have multiple 
     *  arm_zjpgd_loader_t objects, each of them might occupy a scrach memory
     *  (Recommended)
     */         
    ARM_ZJPGD_MODE_PARTIAL_DECODED,                     

    /*! very slow, for each partial loading, always decode from the begining and 
     * release the scratch memory 
     */
    ARM_ZJPGD_MODE_PARTIAL_DECODED_TINY,

} arm_zjpgd_loader_work_mode_t;

typedef struct arm_zjpgd_loader_t arm_zjpgd_loader_t;

typedef struct arm_zjpgd_loader_io_t {

    bool   (*fnOpen)(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader);
    void   (*fnClose)(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader);
    bool   (*fnSeek)(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader, int32_t offset, int32_t whence);
    size_t (*fnRead)(uintptr_t pTarget, arm_zjpgd_loader_t *ptLoader, uint8_t *pchBuffer, size_t tSize);

} arm_zjpgd_loader_io_t;

typedef struct arm_zjpgd_loader_cfg_t {
    
    //arm_2d_size_t       tSize;

    uint8_t bUseHeapForVRES     : 1;
    uint8_t u2ScratchMemType    : 2;
    uint8_t u2WorkMode          : 2;
    uint8_t                     : 5;

    struct {
        const arm_zjpgd_loader_io_t *ptIO;
        uintptr_t pTarget;
    } ImageIO;

    arm_2d_scene_t *ptScene;
} arm_zjpgd_loader_cfg_t;

/*!
 * \brief a user class for user defined control
 */

struct arm_zjpgd_loader_t {
    inherit_ex(arm_2d_vres_t, vres);

ARM_PRIVATE(
    arm_zjpgd_loader_cfg_t tCFG;

    arm_2d_color_info_t tColourInfo;
    
    struct {
        uint8_t *pchBuffer;
        size_t tSize;
        arm_2d_region_t tRegion;
    } ImageBuffer;

    struct {
        zjd_t tZDEC;
        void *pWorkMemory;
        //size_t nPosition;
        arm_2d_region_t tBlockRegion;
        arm_2d_region_t tDrawRegion;

        uint8_t bContextInitialized : 1;
        uint8_t bIsNewLine          : 1;

        //struct {
        //    size_t nPostion;
        //    uintptr_t pBuffer;
        //    size_t tSize;
        //} PreviousRead;
    } Decoder;

    arm_zjpgd_context_t tContext[5];

    struct {
        arm_zjpgd_context_t *ptCandidates;
        arm_zjpgd_context_t *ptList;
    } Reference;

    uint16_t u3PixelByteSize        : 3;
    uint16_t u5BitsPerPixel         : 5;
    uint16_t                        : 5;
    uint16_t bIsNewFrame            : 1;
    uint16_t bErrorDetected         : 1;
    uint16_t bInitialized           : 1;

    int16_t iTargetStrideInByte;

)
};

typedef struct arm_zjpgd_io_file_loader_t {
ARM_PRIVATE(
    const char *pchFilePath;
    FILE *phFile; 
)
} arm_zjpgd_io_file_loader_t;

typedef struct arm_zjpgd_io_binary_loader_t {
ARM_PRIVATE(
    size_t tPostion;
    uint8_t *pchBinary;
    size_t tSize;
)
} arm_zjpgd_io_binary_loader_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern 
const arm_zjpgd_loader_io_t ARM_ZJPGD_IO_FILE_LOADER;

extern 
const arm_zjpgd_loader_io_t ARM_ZJPGD_IO_BINARY_LOADER;

/*============================ PROTOTYPES ====================================*/
extern
ARM_NONNULL(1)
arm_2d_err_t arm_zjpgd_loader_init( arm_zjpgd_loader_t *ptThis,
                                    arm_zjpgd_loader_cfg_t *ptCFG);

extern
ARM_NONNULL(1)
void arm_zjpgd_loader_depose( arm_zjpgd_loader_t *ptThis);

extern
ARM_NONNULL(1)
void arm_zjpgd_loader_on_load( arm_zjpgd_loader_t *ptThis);

extern
ARM_NONNULL(1)
void arm_zjpgd_loader_on_frame_start( arm_zjpgd_loader_t *ptThis);

extern
ARM_NONNULL(1)
void arm_zjpgd_loader_on_frame_complete( arm_zjpgd_loader_t *ptThis);

extern
ARM_NONNULL(1)
arm_2d_err_t arm_zjpgd_loader_add_reference_point_in_image( 
                                            arm_zjpgd_loader_t *ptThis, 
                                            arm_2d_location_t tLocation);
extern
ARM_NONNULL(1)
arm_2d_err_t arm_zjpgd_loader_add_reference_point_on_canvas(
                                            arm_zjpgd_loader_t *ptThis,
                                            arm_2d_location_t tImageLocation,
                                            arm_2d_location_t tReferencePoint);

extern
ARM_NONNULL(1,2)
arm_2d_err_t arm_zjpgd_loader_add_reference_point_on_virtual_screen(
                            arm_zjpgd_loader_t *ptThis,
                            arm_2d_tile_t *ptTile,
                            arm_2d_location_t tImageLocationOnTile,
                            arm_2d_location_t tReferencePointOnVirtualScreen);

extern
ARM_NONNULL(1, 2)
arm_2d_err_t arm_zjpgd_io_file_loader_init(arm_zjpgd_io_file_loader_t *ptThis, 
                                           const char *pchFilePath);

extern
ARM_NONNULL(1, 2)
arm_2d_err_t arm_zjpgd_io_binary_loader_init(arm_zjpgd_io_binary_loader_t *ptThis, 
                                             const uint8_t *pchBinary,
                                             size_t tSize);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
