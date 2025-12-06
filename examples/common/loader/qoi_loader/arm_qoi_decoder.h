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

#ifndef __ARM_QOI_DECODER_H__
#define __ARM_QOI_DECODER_H__

/*============================ INCLUDES ======================================*/

#ifdef __ARM_QOI_DECODER_CFG__
#   include __ARM_QOI_DECODER_CFG__
#else
#   include "arm_qoi_cfg.h"
#endif

#include "arm_2d_helper.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#   pragma clang diagnostic ignored "-Wvisibility"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/*============================ MACROS ========================================*/

/* OOC header, please DO NOT modify  */
#ifdef __QOI_DECODER_IMPLEMENT__
#   undef   __QOI_DECODER_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__QOI_DECODER_INHERIT__)
#   undef   __QOI_DECODER_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    ARM_QOI_DEC_FORMAT_CCCA8888 = 0,
    ARM_QOI_DEC_FORMAT_CCCN888,
    ARM_QOI_DEC_FORMAT_RGB565,
    ARM_QOI_DEC_FORMAT_GRAY8,

    ARM_QOI_DEC_FORMAT_CHN_B,
    ARM_QOI_DEC_FORMAT_CHN_G,
    ARM_QOI_DEC_FORMAT_CHN_R,
    ARM_QOI_DEC_FORMAT_MASK_ONLY,

    __ARM_QOI_DEC_FORMAT_VALID = ARM_QOI_DEC_FORMAT_MASK_ONLY,

    ARM_QOI_OP_INDEX    = 0x0,
    ARM_QOI_OP_DIFF     = 0x1,
    ARM_QOI_OP_LUMA     = 0x2,
    ARM_QOI_OP_RUN      = 0x3,
    ARM_QOI_OP_ID_RGB   = 0xFE,
    ARM_QOI_OP_ID_RGBA  = 0xFF,
};

typedef enum {
    ARM_QOI_CTX_REPORT_TOP_LEFT,
    ARM_QOI_CTX_REPORT_BOTTOM_RIGHT,
} arm_qoi_ctx_evt_t;

/*!
 * \brief the QOI header
 */
struct __arm_qoi_header {
    const char  chMagic[4];         //!< magic bytes "qoif"
    uint32_t    wWidth;             //!< image width in pixels (Big-endian)
    uint32_t    wHeight;            //!< image height in pixels (Big-endian)
    uint8_t     chChannels;         //!< 3 = RGB, 4 = RGBA, others are illegal
    uint8_t     chColourSpace;      //!< 0 = sRGB with linear alpha, 1 = all channels linear
};

typedef union __arm_qoi_rgba8888_t {
    struct {
        uint8_t chRed;
        uint8_t chGreen;
        uint8_t chBlue;
        uint8_t chAlpha;
    };
    uint8_t chChannel[4];
}__arm_qoi_rgba8888_t ;

typedef union __arm_qoi_pixel_t {
    uint32_t wValue;
    __arm_qoi_rgba8888_t tColour;
} __arm_qoi_pixel_t;

typedef union __arm_qoi_op_t {
    uint8_t chID;
    struct {
        uint8_t chParam : 6;
        uint8_t u2OP    : 2;
    };
}__arm_qoi_op_t;

typedef struct __arm_qoi_op_rgb_t {
    __arm_qoi_op_t tHead;       //!< 0xFE

    union {
        struct {
            uint8_t chRed;
            uint8_t chGreen;
            uint8_t chBlue;
        };
        uint8_t chRGB[3];
    };
} __arm_qoi_op_rgb_t;

typedef struct __arm_qoi_op_rgba_t {
    __arm_qoi_op_t tHead;       //!< 0xFF
    __arm_qoi_rgba8888_t tColour;
} __arm_qoi_op_rgba_t;

typedef union __arm_qoi_op_index_t {
    __arm_qoi_op_t tHead;       
    struct {
        uint8_t u6Index     : 6;    //!< index
        uint8_t             : 2;    //!< 0b00
    };
} __arm_qoi_op_index_t;

typedef union __arm_qoi_op_diff_t {
    __arm_qoi_op_t tHead;       
    struct {
        uint8_t u2DB        : 2;    //!< blue diff      0:-2, 1:-1, 2:0, 3: 1 
        uint8_t u2DG        : 2;    //!< green diff     0:-2, 1:-1, 2:0, 3: 1 
        uint8_t u2DR        : 2;    //!< red diff       0:-2, 1:-1, 2:0, 3: 1 
        uint8_t             : 2;    //!< 0b01
    };
}__arm_qoi_op_diff_t;

typedef union __arm_qoi_op_luma_t {
    __arm_qoi_op_t tHead;       
    struct {
        int8_t s6DG         : 6;    //!< green diff
        int8_t              : 2;    //!< 0b10
        int8_t s4DBmDG      : 4;    //!< db - dg
        int8_t s4DRmDG      : 4;    //!< dr - dg
    };
} __arm_qoi_op_luma_t;

typedef union __arm_qoi_op_run_t {
    __arm_qoi_op_t tHead;       
    struct {
        uint8_t u6Run       : 6;    //!< run count
        uint8_t             : 2;    //!< 0b11
    };
} __arm_qoi_op_run_t;

typedef struct arm_qoi_dec_t arm_qoi_dec_t;

typedef struct arm_qoi_dec_ctx_t {
ARM_PRIVATE(
    arm_qoi_dec_t *ptQOIDec;

    __arm_qoi_pixel_t tHashTable[64];
    __arm_qoi_pixel_t tPrevious;
    
    size_t tPixelDecoded;
    size_t tPosition;

    uint16_t hwTail;
    uint16_t hwHead;
    uint16_t hwSize;

    uint8_t chRunCount;
)
} arm_qoi_dec_ctx_t;



typedef bool __arm_qoi_io_seek_t(uintptr_t pTarget, int32_t offset, int32_t whence);

typedef size_t __arm_qoi_io_read_t (uintptr_t pTarget,       
                                    uint8_t *pchBuffer,
                                    size_t tLength);

typedef void __arm_qoi_ctx_report_t(uintptr_t pTarget,
                                    arm_qoi_dec_t *ptDecoder,
                                    arm_2d_location_t tLocation,
                                    arm_qoi_dec_ctx_t *ptContext,
                                    arm_qoi_ctx_evt_t tEvent);

typedef struct arm_qoi_cfg_t {
    uint8_t *pchWorkingMemory;
    uint16_t hwSize;
    uint8_t chOutputColourFormat;
    uint8_t bPreBlendBGColour   : 1;        //!< this option is only valid when the output colour format is gray8, rgb565 or cccn888

    struct {
        __arm_qoi_io_seek_t     *fnSeek;
        __arm_qoi_io_read_t     *fnRead;
        __arm_qoi_ctx_report_t  *fnReport;
    } IO;

    COLOUR_TYPE_T tBackgroundColour;        //!< this option is only valid when the output colour format is gray8, rgb565 or cccn888
} arm_qoi_cfg_t;

struct arm_qoi_dec_t{
    arm_2d_size_t tSize;
    uintptr_t pTarget;

ARM_PRIVATE(
    arm_qoi_cfg_t tCFG;
    arm_qoi_dec_ctx_t *ptWorking;

    uint8_t bValid;
    int8_t chErrorCode;
)
} ;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1,2)
arm_2d_err_t arm_qoi_decoder_init(arm_qoi_dec_t *ptThis, arm_qoi_cfg_t *ptCFG);

extern
ARM_NONNULL(1,2)
arm_qoi_dec_ctx_t *arm_qoi_dec_save_context(arm_qoi_dec_t *ptThis, 
                                            arm_qoi_dec_ctx_t *ptContextOut);

extern
ARM_NONNULL(1,2)
arm_2d_err_t arm_qoi_dec_resume_context(arm_qoi_dec_t *ptThis, 
                                        arm_qoi_dec_ctx_t *ptContextOut);

extern
ARM_NONNULL(1,2)
arm_2d_err_t arm_qoi_decode(arm_qoi_dec_t *ptThis,
                            void *pTarget,
                            arm_2d_region_t *ptTargetRegion);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
