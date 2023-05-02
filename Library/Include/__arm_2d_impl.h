/*
 * Copyright (C) 2010-2022 Arm Limited or its affiliates. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        __arm_2d_impl.h
 * Description:  header files for internal users or professional developers
 *
 * $Date:        02. May 2023
 * $Revision:    V.1.3.11
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_ACCELERATION_H__
#   define __ARM_2D_ACCELERATION_H__      1

/*============================ INCLUDES ======================================*/

#include "arm_2d.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wpadded"
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 174,177,188,68,513,144,64
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pe301
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-value"
#endif

/*============================ MACROS ========================================*/
#undef this
#define this            (*ptThis)

#undef OP_CORE
#define OP_CORE         this.use_as__arm_2d_op_core_t

#define ARM_2D_IMPL(__TYPE, ...)                                                \
            __TYPE *ptThis = (__TYPE *)(NULL,##__VA_ARGS__);                    \
            if (NULL == ptThis) {                                               \
                ptThis = (__TYPE *)&ARM_2D_CTRL.DefaultOP;                      \
            }

/*============================ MACROFIED FUNCTIONS ===========================*/

#define ARM_2D_TRY_ACCELERATION(__ID, __FUNC_PROTOTYPE, ...)                    \
        if (    (NULL != OP_CORE.ptOp->Info.LowLevelIO.IO[__ID]->HW)            \
            &&  (NULL != OP_CORE.ptOp->Info.LowLevelIO.IO[__ID])) {             \
            tResult =                                                           \
            (*(__FUNC_PROTOTYPE *)OP_CORE.ptOp->Info.LowLevelIO.IO[__ID]->HW)(  \
                                        ptTask,                                 \
                                        ##__VA_ARGS__);                         \
        }

#define ARM_2D_RUN_DEFAULT(__ID, __FUNC_PROTOTYPE, ...)                         \
        if (    (NULL != OP_CORE.ptOp->Info.LowLevelIO.IO[__ID]->SW)            \
            &&  (NULL != OP_CORE.ptOp->Info.LowLevelIO.IO[__ID])) {             \
            tResult =                                                           \
            (*(__FUNC_PROTOTYPE *)OP_CORE.ptOp->Info.LowLevelIO.IO[__ID]->SW)(  \
                                        ptTask,                                 \
                                        ##__VA_ARGS__);                         \
        } else {                                                                \
            tResult = (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;                     \
        }
    
#define __ARM_2D_PIXEL_BLENDING_GRAY8(__SRC_ADDR, __DES_ADDR, __TRANS)          \
            do {                                                                \
                uint16_t hwOPA = 256 - (__TRANS);                               \
                const uint8_t *pchSrc = (uint8_t *)(__SRC_ADDR);                \
                uint8_t *pchDes = (uint8_t *)(__DES_ADDR);                      \
                                                                                \
                *pchDes = ((uint16_t)( ((uint16_t)(*pchSrc++) * hwOPA)          \
                                     + ((uint16_t)(*pchDes) * (__TRANS))        \
                                     ) >> 8);                                   \
            } while(0)

#define __ARM_2D_PIXEL_BLENDING_RGB565(__SRC_ADDR, __DES_ADDR, __TRANS)         \
            do {                                                                \
                uint16_t hwOPA = 256 - (__TRANS);                               \
                __arm_2d_color_fast_rgb_t tSrcPix, tTargetPix;                  \
                uint16_t *phwTargetPixel = (__DES_ADDR);                        \
                __arm_2d_rgb565_unpack(*(__SRC_ADDR), &tSrcPix);                \
                __arm_2d_rgb565_unpack(*phwTargetPixel, &tTargetPix);           \
                                                                                \
                for (int i = 0; i < 3; i++) {                                   \
                    uint16_t        hwTemp =                                    \
                        (uint16_t) (tSrcPix.BGRA[i] * hwOPA) +                  \
                        (tTargetPix.BGRA[i] * (__TRANS));                       \
                    tTargetPix.BGRA[i] = (uint16_t) (hwTemp >> 8);              \
                }                                                               \
                                                                                \
                /* pack merged stream */                                        \
                *phwTargetPixel = __arm_2d_rgb565_pack(&tTargetPix);            \
            } while(0)
            
#define __ARM_2D_PIXEL_BLENDING_CCCN888(__SRC_ADDR, __DES_ADDR, __TRANS)        \
            do {                                                                \
                uint16_t hwOPA = 256 - (__TRANS);                               \
                /* do not change alpha */                                       \
                uint_fast8_t ARM_2D_SAFE_NAME(n) = sizeof(uint32_t) - 1;        \
                const uint8_t *pchSrc = (uint8_t *)(__SRC_ADDR);                \
                uint8_t *pchDes = (uint8_t *)(__DES_ADDR);                      \
                                                                                \
                do {                                                            \
                    *pchDes = ( ((uint_fast16_t)(*pchSrc++) * hwOPA)            \
                              + ((uint_fast16_t)(*pchDes) * (__TRANS))          \
                              ) >> 8;                                           \
                     pchDes++;                                                  \
                } while(--ARM_2D_SAFE_NAME(n));                                 \
            } while(0)
            
            
            
#define __ARM_2D_PIXEL_BLENDING_OPA_GRAY8(__SRC_ADDR, __DES_ADDR, __OPA)        \
            do {                                                                \
                uint16_t hwTrans = 256 - (__OPA);                               \
                const uint8_t *pchSrc = (uint8_t *)(__SRC_ADDR);                \
                uint8_t *pchDes = (uint8_t *)(__DES_ADDR);                      \
                                                                                \
                *pchDes = ((uint16_t)( ((uint16_t)(*pchSrc++) * (__OPA))        \
                                     + ((uint16_t)(*pchDes) * hwTrans)          \
                                     ) >> 8);                                   \
            } while(0)

#define __ARM_2D_PIXEL_BLENDING_OPA_RGB565(__SRC_ADDR, __DES_ADDR, __OPA)       \
            do {                                                                \
                uint16_t hwTrans = 256 - (__OPA);                               \
                __arm_2d_color_fast_rgb_t tSrcPix, tTargetPix;                  \
                uint16_t *phwTargetPixel = (__DES_ADDR);                        \
                __arm_2d_rgb565_unpack(*(__SRC_ADDR), &tSrcPix);                \
                __arm_2d_rgb565_unpack(*phwTargetPixel, &tTargetPix);           \
                                                                                \
                for (int i = 0; i < 3; i++) {                                   \
                    uint16_t        hwTemp =                                    \
                        (uint16_t) (tSrcPix.BGRA[i] * (__OPA)) +                \
                        (tTargetPix.BGRA[i] * hwTrans);                         \
                    tTargetPix.BGRA[i] = (uint16_t) (hwTemp >> 8);              \
                }                                                               \
                                                                                \
                /* pack merged stream */                                        \
                *phwTargetPixel = __arm_2d_rgb565_pack(&tTargetPix);            \
            } while(0)
            
#define __ARM_2D_PIXEL_BLENDING_OPA_CCCN888(__SRC_ADDR, __DES_ADDR, __OPA)      \
            do {                                                                \
                uint16_t hwTrans = 256 - (__OPA);                               \
                /* do not change alpha */                                       \
                uint_fast8_t ARM_2D_SAFE_NAME(n) = sizeof(uint32_t) - 1;        \
                const uint8_t *pchSrc = (uint8_t *)(__SRC_ADDR);                \
                uint8_t *pchDes = (uint8_t *)(__DES_ADDR);                      \
                                                                                \
                do {                                                            \
                    *pchDes = ( ((uint_fast16_t)(*pchSrc++) * (__OPA))          \
                              + ((uint_fast16_t)(*pchDes) * hwTrans)            \
                              ) >> 8;                                           \
                     pchDes++;                                                  \
                } while(--ARM_2D_SAFE_NAME(n));                                 \
            } while(0)

#define __ARM_2D_PIXEL_AVERAGE_RGB565(__PIXEL_IN, __ALPHA)                      \
    do {                                                                        \
        __arm_2d_color_fast_rgb_t tTempColour;                                  \
        __arm_2d_rgb565_unpack((__PIXEL_IN), &tTempColour);                     \
        tPixel.R += tTempColour.R * (__ALPHA);                                  \
        tPixel.G += tTempColour.G * (__ALPHA);                                  \
        tPixel.B += tTempColour.B * (__ALPHA);                                  \
    } while(0)


#define __ARM_2D_PIXEL_AVERAGE_CCCN888(__PIXEL_IN, __ALPHA)                     \
    do {                                                                        \
        arm_2d_color_rgb888_t tTempColour = {.tValue = (__PIXEL_IN)};           \
        tPixel.R += tTempColour.u8R * (__ALPHA);                                \
        tPixel.G += tTempColour.u8G * (__ALPHA);                                \
        tPixel.B += tTempColour.u8B * (__ALPHA);                                \
    } while(0)
    
#define __ARM_2D_PIXEL_AVERAGE_GRAY8(__PIXEL_IN, __ALPHA)                       \
    do {                                                                        \
        tPixel += (uint16_t)(__PIXEL_IN) * (uint16_t)(__ALPHA);                 \
    } while(0)

/*============================ TYPES =========================================*/

typedef struct __arm_2d_point_adj_alpha_t{
    struct {
        arm_2d_location_t tOffset;
        uint_fast8_t chAlpha;
    }tMatrix[4];
} __arm_2d_point_adj_alpha_t;

/*!
 * \brief private enum for mask validation 
 */
enum {
    __ARM_2D_MASK_ALLOW_8in32 = _BV(0),     //!< allow 8in32 mode
    __ARM_2D_MASK_ALLOW_A2    = _BV(1),     //!< allow A2
    __ARM_2D_MASK_ALLOW_A4    = _BV(2),     //!< allow A4
    __ARM_2D_MASK_ALLOW_A8    = _BV(3),     //!< allow A8
};


//! \name Operation Index: used for logging and debugging purpose
//! @{
enum {
    /*------------ arm-2d operation idx begin --------------*/
    __ARM_2D_OP_IDX_BARRIER,
    __ARM_2D_OP_IDX_SYNC = __ARM_2D_OP_IDX_BARRIER,
    
    __ARM_2D_OP_IDX_COPY,
    __ARM_2D_OP_IDX_COPY_ONLY,
    __ARM_2D_OP_IDX_COPY_WITH_X_MIRROR,
    __ARM_2D_OP_IDX_COPY_WITH_Y_MIRROR,
    __ARM_2D_OP_IDX_COPY_WITH_XY_MIRROR,
    
    __ARM_2D_OP_IDX_FILL_ONLY,
    __ARM_2D_OP_IDX_FILL_WITH_X_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_Y_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_XY_MIRROR,
    
    __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING,
    __ARM_2D_OP_IDX_COPY_ONLY_WITH_COLOUR_KEYING,
    __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_X_MIRROR,
    __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_Y_MIRROR,
    __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_XY_MIRROR,

    __ARM_2D_OP_IDX_FILL_ONLY_WITH_COLOUR_KEYING,
    __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_X_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_Y_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_XY_MIRROR,


    __ARM_2D_OP_IDX_COPY_WITH_OPACITY,
    __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_OPACITY,

    __ARM_2D_OP_IDX_COPY_WITH_MASKS,
    __ARM_2D_OP_IDX_COPY_WITH_MASKS_ONLY,
    __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_X_MIRROR,
    __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_Y_MIRROR,
    __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_XY_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_MASKS_ONLY,
    __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_X_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_Y_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_XY_MIRROR,
    
    __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK,
    __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_ONLY,
    __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_X_MIRROR,
    __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_Y_MIRROR,
    __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_XY_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_ONLY,
    __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_X_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_Y_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_XY_MIRROR,

    __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK,
    __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_ONLY,
    __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_X_MIRROR,
    __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_Y_MIRROR,
    __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_XY_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_ONLY,
    __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_X_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_Y_MIRROR,
    __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_XY_MIRROR,

    __ARM_2D_OP_IDX_FILL_COLOUR,
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_OPACITY,

    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A2_MASK,
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A4_MASK,
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A8_MASK,
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK 
        = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A8_MASK,
    
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_REPEAT,
    
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A2_MASK_AND_OPACITY,
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A4_MASK_AND_OPACITY,
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A8_MASK_AND_OPACITY,
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_OPACITY 
        = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_A8_MASK_AND_OPACITY,

    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_OPACITY_AND_REPEAT,
    
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_AND_TRANFORM,
    __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_OPACITY_AND_TRANFORM,

    __ARM_2D_OP_IDX_DRAW_POINT,
    __ARM_2D_OP_IDX_DRAW_PATTERN,

    __ARM_2D_OP_IDX_COLOUR_FORMAT_CONVERSION,
    
    __ARM_2D_OP_IDX_TRANSFORM_ONLY,
    __ARM_2D_OP_IDX_TRANSFORM_ONLY_WITH_OPACITY,
    
    /* note: we delibrately avoid using the following defintion to avoid 
     *       confusion between transform-with-colour-keying-and-opacity (in the 
     *       older version, it is called transform-with-opacity) and 
     *       transform-only-with-opacity (it should be called 
     *       transform-with-opacity if we don't care about the backward
     *       compatibility in the later version).
    __ARM_2D_OP_IDX_TRANSFORM_WITH_OPACITY 
        = __ARM_2D_OP_IDX_TRANSFORM_ONLY_WITH_OPACITY,
    */
    
    __ARM_2D_OP_IDX_TRANSFORM_WITH_COLOUR_KEYING,
    __ARM_2D_OP_IDX_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY,

    //__ARM_2D_OP_IDX_TRANSFORM_WITH_MASKS,                                     //!< todo in v1.xx
    __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK,                                 
    //__ARM_2D_OP_IDX_TRANSFORM_WITH_TARGET_MASK,                               //!< todo in v1.xx

    //__ARM_2D_OP_IDX_TRANSFORM_WITH_MASKS_AND_OPACITY,                         //!< todo in v1.xx
    __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK_AND_OPACITY,
    //__ARM_2D_OP_IDX_TRANSFORM_WITH_TARGET_MASK_AND_OPACITY,                   //!< todo in v1.xx
    /*------------ cmsisi-2d operation idx end --------------*/
};
//! @}


typedef struct __arm_2d_sub_task_t __arm_2d_sub_task_t;


typedef arm_fsm_rt_t __arm_2d_io_func_t(__arm_2d_sub_task_t *ptTask);

typedef struct __arm_2d_low_level_io_t {
    __arm_2d_io_func_t *SW;
    __arm_2d_io_func_t *HW;
} __arm_2d_low_level_io_t;


typedef struct __arm_2d_tile_param_t {
    void *              pBuffer;
    int32_t             nOffset;
    int16_t             iStride;
    arm_2d_color_info_t tColour;
    
    uint8_t             bInvalid            : 1;
    uint8_t             bDerivedResource    : 1;
    uint8_t                                 : 6;
    
    arm_2d_region_t     tValidRegion;
    arm_2d_tile_t      *ptDerivedResource;
} __arm_2d_tile_param_t;

typedef struct __arm_2d_param_copy_t {
    __arm_2d_tile_param_t tSource;
    __arm_2d_tile_param_t tTarget;
    arm_2d_size_t       tCopySize;
} __arm_2d_param_copy_t;

typedef struct __arm_2d_param_copy_msk_t {
    implement(__arm_2d_param_copy_t);
    
    __arm_2d_tile_param_t tSrcMask;
    __arm_2d_tile_param_t tDesMask;
} __arm_2d_param_copy_msk_t;

typedef struct __arm_2d_param_copy_orig_t {
    implement(__arm_2d_param_copy_t);
    
    __arm_2d_tile_param_t tOrigin;
    
} __arm_2d_param_copy_orig_t;

typedef struct __arm_2d_param_copy_orig_msk_t {
    implement(__arm_2d_param_copy_orig_t);
    
    __arm_2d_tile_param_t tOrigMask;
    __arm_2d_tile_param_t tDesMask;
    
} __arm_2d_param_copy_orig_msk_t;

typedef struct __arm_2d_param_fill_t {
    __arm_2d_tile_param_t tSource;
    __arm_2d_tile_param_t tTarget;
} __arm_2d_param_fill_t;

typedef struct __arm_2d_param_fill_msk_t {
    implement(__arm_2d_param_fill_t);
    
    __arm_2d_tile_param_t tSrcMask;
    __arm_2d_tile_param_t tDesMask;
} __arm_2d_param_fill_msk_t;

typedef struct __arm_2d_param_fill_orig_t {
    implement(__arm_2d_param_fill_t);
    
    __arm_2d_tile_param_t tOrigin;
    
} __arm_2d_param_fill_orig_t;

struct __arm_2d_sub_task_t{
ARM_PRIVATE(
    __arm_2d_sub_task_t         *ptNext;
    
    arm_2d_op_core_t            *ptOP;
    
    uint8_t         chLowLeveIOIndex;                                           //!< the type of IO interface
    uint8_t         bIsCPL              : 1;
    uint8_t                             : 7;
    uint16_t                            : 16;
    
    union {
        __arm_2d_tile_param_t           tTileProcess;
        
        __arm_2d_param_copy_t           tCopy;
        __arm_2d_param_copy_msk_t       tCopyMask;
        __arm_2d_param_copy_orig_t      tCopyOrig;                              //!< for transform
        __arm_2d_param_copy_orig_msk_t  tCopyOrigMask;                          //!< for transform with masks
        
        __arm_2d_param_fill_t           tFill;
        __arm_2d_param_fill_msk_t       tFillMask;
        __arm_2d_param_fill_orig_t      tFillOrig;
    }Param;
)};


struct __arm_2d_op_control {
ARM_PRIVATE(
    __arm_2d_sub_task_t   *ptFreeList;
    struct {
        __arm_2d_sub_task_t   *ptHead;
        __arm_2d_sub_task_t   *ptTail;
    } TaskFIFO;
    
    struct {
        arm_2d_op_core_t   *ptHead;
        arm_2d_op_core_t   *ptTail;
    } OPFIFO;
    
    uint16_t                hwFreeCount;
    uint16_t                hwTaskCount;
    uint16_t                hwBookCount;
    uint16_t                            : 16;
    
    arm_2d_tile_t           *ptDefaultFrameBuffer;
    
    union {
        arm_2d_op_t                         tBasic;
        arm_2d_op_fill_cl_t                 tFillColour;
        arm_2d_op_fill_cl_msk_t             tFillColourMask;
        arm_2d_op_fill_cl_opc_t             tFillColourOpacity;
        arm_2d_op_src_t                     tWithSource;
        
        arm_2d_op_alpha_t                   tAlpha;
        arm_2d_op_alpha_cl_key_t            tAlphaColourKeying;
        arm_2d_op_alpha_fill_cl_msk_opc_t   tAlphaFillColourMaskOpacity;
        arm_2d_op_cp_msk_t                  tCopyMasks;
        
        arm_2d_op_drw_patn_t                tDrawPattern;
        arm_2d_op_trans_t                   tTransform;
        arm_2d_op_trans_opa_t               tTransformOpacity;
        arm_2d_op_trans_msk_opa_t           tTransformMaskOpacity;
        
        arm_2d_op_msk_t                     tBasicMask;
        arm_2d_op_src_msk_t                 tSourceMask;
        arm_2d_op_src_orig_msk_t            tSourceOrigMask;
    } DefaultOP;
)};



/*============================ GLOBAL VARIABLES ==============================*/
extern 
const struct __arm_2d_io_table __ARM_2D_IO_TABLE;

extern struct __arm_2d_op_control ARM_2D_CTRL;

/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * Pipeline                                                                   *
 *----------------------------------------------------------------------------*/
extern
arm_fsm_rt_t __arm_2d_op_invoke(arm_2d_op_core_t *ptOP);

extern 
/*! \brief sync up with operation 
 *! \retval true operation is busy
 *! \retval false operation isn't busy
 */
bool __arm_2d_op_acquire(arm_2d_op_core_t *ptOP);

/*! \note This API should be called by both arm_2d_task and hardware 
 *!       accelerator to indicate the completion of a sub task
 */
extern
void __arm_2d_notify_sub_task_cpl(__arm_2d_sub_task_t *ptTask, 
                                    arm_fsm_rt_t tResult,
                                    bool bFromHW);

extern 
arm_fsm_rt_t __arm_2d_op_frontend_op_decoder(arm_2d_op_core_t *ptThis);


extern
arm_fsm_rt_t __arm_2d_op_depose(arm_2d_op_core_t *ptThis, 
                                arm_fsm_rt_t tResult);

extern 
void __arm_2d_sub_task_depose(arm_2d_op_core_t *ptOP);
/*----------------------------------------------------------------------------*
 * Utilities                                                                  *
 *----------------------------------------------------------------------------*/
extern
__arm_2d_point_adj_alpha_t 
__arm_2d_point_get_adjacent_alpha_fp(arm_2d_point_float_t *ptPoint);

extern
__arm_2d_point_adj_alpha_t 
__arm_2d_point_get_adjacent_alpha_q16(arm_2d_point_fx_t *ptPoint);

extern
arm_2d_err_t  __arm_mask_validate(  const arm_2d_tile_t *ptSource,
                                    const arm_2d_tile_t *ptSrcMask,
                                    const arm_2d_tile_t *ptTarget,
                                    const arm_2d_tile_t *ptDesMask,
                                    uint32_t wMode);

extern
bool __arm_2d_valid_mask(   const arm_2d_tile_t *ptAlpha, 
                            uint_fast8_t chAllowMask);

extern
ARM_NONNULL(1,2)
const arm_2d_tile_t *__arm_2d_tile_get_1st_derived_child_or_root(
                                            const arm_2d_tile_t *ptTile,
                                            arm_2d_region_t *ptValidRegion,
                                            arm_2d_location_t *ptOffset,
                                            arm_2d_tile_t **ppFirstDerivedChild,
                                            bool bQuitWhenFindFirstDerivedChild);

extern
ARM_NONNULL(1,2)
const arm_2d_tile_t *__arm_2d_tile_get_root(const arm_2d_tile_t *ptTile,
                                            arm_2d_region_t *ptValidRegion,
                                            arm_2d_location_t *ptOffset,
                                            arm_2d_tile_t **ppFirstDerivedChild);

extern
void *__arm_2d_allocate_scratch_memory( uint32_t wSize, 
                                        uint_fast8_t nAlign,
                                        arm_2d_mem_type_t tType);

/*----------------------------------------------------------------------------*
 * Default Software Implementations                                           *
 *----------------------------------------------------------------------------*/
        
extern 
arm_fsm_rt_t __arm_2d_sw_draw_point(__arm_2d_sub_task_t *ptTask);      

extern 
arm_fsm_rt_t __arm_2d_c8bit_sw_draw_pattern( __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb16_sw_draw_pattern( __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb32_sw_draw_pattern( __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill( __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill( __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill( __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy_only(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy_only(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy_only(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy_x_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy_x_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy_x_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy_y_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy_y_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy_y_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy_xy_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy_xy_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy_xy_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill_only(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill_only(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill_only(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill_x_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill_x_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill_x_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill_y_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill_y_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill_y_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill_xy_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill_xy_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill_xy_mirror(  __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_masks(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_masks(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_masks(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_masks(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_masks(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_masks(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_masks_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_masks_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_masks_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_masks_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_masks_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_masks_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_masks_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_masks_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_masks_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_masks_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_masks_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_masks_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_masks_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_masks_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_masks_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_masks_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_masks_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_masks_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_masks_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_masks_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_masks_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_masks_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_masks_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_masks_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_src_mask(
                                                __arm_2d_sub_task_t *ptTask);


extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_src_mask_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_src_mask_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_src_mask_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_src_mask_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_src_mask_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_src_mask_only(
                                                __arm_2d_sub_task_t *ptTask);


extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_src_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_src_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_src_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_src_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_src_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_src_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);


extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_src_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_src_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_src_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_src_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_src_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_src_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);


extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_src_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_src_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_src_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_src_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_src_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_src_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);


extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_des_mask(
                                                __arm_2d_sub_task_t *ptTask);


extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_des_mask_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_des_mask_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_des_mask_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_des_mask_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_des_mask_only(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_des_mask_only(
                                                __arm_2d_sub_task_t *ptTask);


extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_des_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_des_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_des_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_des_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_des_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_des_mask_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);


extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_des_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_des_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_des_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_des_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_des_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_des_mask_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);


extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_des_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_fill_with_des_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_des_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_fill_with_des_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_des_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_fill_with_des_mask_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);


extern 
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy_with_colour_keying(
                                                __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy_with_colour_keying(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy_with_colour_keying(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy_with_colour_keying_only(
                                                __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy_with_colour_keying_only(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy_with_colour_keying_only(
                                        __arm_2d_sub_task_t *ptTask);


extern 
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy_with_colour_keying_and_x_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy_with_colour_keying_and_x_mirror(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy_with_colour_keying_and_x_mirror(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy_with_colour_keying_and_y_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy_with_colour_keying_and_y_mirror(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy_with_colour_keying_and_y_mirror(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy_with_colour_keying_and_xy_mirror(
                                                __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy_with_colour_keying_and_xy_mirror(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy_with_colour_keying_and_xy_mirror(
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill_with_colour_keying( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill_with_colour_keying( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill_with_colour_keying( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill_with_colour_keying_only( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill_with_colour_keying_only( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill_with_colour_keying_only( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill_with_colour_keying_and_x_mirror( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill_with_colour_keying_and_x_mirror( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill_with_colour_keying_and_x_mirror( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill_with_colour_keying_and_y_mirror( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill_with_colour_keying_and_y_mirror( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill_with_colour_keying_and_y_mirror( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill_with_colour_keying_and_xy_mirror( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill_with_colour_keying_and_xy_mirror( 
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill_with_colour_keying_and_xy_mirror( 
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_c8bit_sw_colour_filling(__arm_2d_sub_task_t *ptTask);
       
extern 
arm_fsm_rt_t __arm_2d_rgb16_sw_colour_filling(__arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb32_sw_colour_filling(__arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_a2_mask(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_a2_mask(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_a2_mask(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_a4_mask(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_a4_mask(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_a4_mask(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_mask(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_mask(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_mask(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_a2_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_a2_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_a2_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_a4_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_a4_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_a4_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_mask_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_mask_opacity_and_transform(
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_mask_opacity_and_transform(
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_mask_opacity_and_transform(
                                        __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_opacity(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_opacity(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_opacity(__arm_2d_sub_task_t *ptTask);
 

extern
arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_colour_keying_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);
                                        
extern
arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_colour_keying_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);
                                        
extern
arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_colour_keying_and_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask);

extern 
arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_opacity(
                                        __arm_2d_sub_task_t *ptTask);
extern
arm_fsm_rt_t __arm_2d_sw_convert_colour_to_gray8(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_sw_convert_colour_to_rgb565(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_sw_convert_colour_to_rgb888(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_transform_with_colour_keying(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_transform_with_colour_keying(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_transform_with_colour_keying(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_gray8_sw_transform_only(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_rgb565_sw_transform_only(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t __arm_2d_cccn888_sw_transform_only(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_gray8_sw_transform_with_colour_keying_and_opacity(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_rgb565_sw_transform_with_colour_keying_and_opacity(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_cccn888_sw_transform_with_colour_keying_and_opacity(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_gray8_sw_transform_only_with_opacity(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_rgb565_sw_transform_only_with_opacity(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_cccn888_sw_transform_only_with_opacity(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_gray8_sw_transform_with_src_mask(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_rgb565_sw_transform_with_src_mask(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_cccn888_sw_transform_with_src_mask(__arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_gray8_sw_transform_with_src_mask_and_opacity(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_rgb565_sw_transform_with_src_mask_and_opacity(
                                                __arm_2d_sub_task_t *ptTask);

extern
arm_fsm_rt_t 
__arm_2d_cccn888_sw_transform_with_src_mask_and_opacity(
                                                __arm_2d_sub_task_t *ptTask);
/*========================== POST INCLUDES ===================================*/
#include "__arm_2d_direct.h"

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_warning 174,177,188,68,513,144,64
#elif __IS_COMPILER_IAR__
#   pragma diag_warning=pe111
#endif

#ifdef   __cplusplus
}
#endif

#endif


