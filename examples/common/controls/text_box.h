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

#ifndef __TEXT_BOX_H__
#define __TEXT_BOX_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__common.h"

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
#ifdef __TEXT_BOX_IMPLEMENT__
#   undef   __TEXT_BOX_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__TEXT_BOX_INHERIT__)
#   undef   __TEXT_BOX_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __text_box_line_info_t __text_box_line_info_t;

struct __text_box_line_info_t {
ARM_PRIVATE(
    __text_box_line_info_t *ptNext;

    int32_t nLineNo;
    int32_t nStartPosition;

    uint16_t hwActive;
    uint16_t bEndNaturally  : 1;
    uint16_t                : 15;

    uint16_t hwBrickCount;
    int16_t iLineWidth;
    int16_t iPureCharWidth;
    uint16_t hwByteCount;
    
    q16_t q16PixelsPerBlank;
)
} ;

typedef __text_box_line_info_t __text_box_scratch_mem_t;

typedef enum {
    TEXT_BOX_SEEK_SET,
    TEXT_BOX_SEEK_CUR,
    TEXT_BOX_SEEK_END
} text_box_seek_whence_t;

typedef enum {
    TEXT_BOX_LINE_ALIGN_LEFT,
    TEXT_BOX_LINE_ALIGN_RIGHT,
    TEXT_BOX_LINE_ALIGN_JUSTIFIED,
} text_box_line_alignment_t;

typedef struct text_box_t text_box_t;

typedef int32_t text_box_io_read_handler_t(text_box_t *ptThis, 
                                        uintptr_t pObj, 
                                        uint8_t *pchBuffer,
                                        uint_fast16_t hwSize);
typedef int32_t text_box_io_seek_handler_t( text_box_t *ptThis, 
                                            uintptr_t pObj,
                                            int32_t nOffset,
                                            text_box_seek_whence_t enWhence);

typedef bool text_box_io_eof_handler_t(text_box_t *ptThis, uintptr_t pObj);

typedef struct text_box_io_handler_t {
    text_box_io_read_handler_t  *fnGetChar;
    text_box_io_seek_handler_t  *fnSeek;
    text_box_io_eof_handler_t   *fnEOF;
} text_box_io_handler_t;

typedef struct text_box_io_text_stream_reader_t {
    const text_box_io_handler_t *ptIO;
    uintptr_t pTarget;
} text_box_io_text_stream_reader_t;

typedef struct text_box_cfg_t {
    const arm_2d_font_t *ptFont;
    float fScale;

    text_box_io_text_stream_reader_t tStreamIO;
    arm_2d_scene_t *ptScene;

#if 0
    __text_box_scratch_mem_t *ptScratchMemory;
    uint16_t                  hwScratchMemoryCount;
#endif

    uint8_t u2LineAlign         : 2;                                            //!< please use enum in text_box_line_alignment_t
    uint8_t                     : 4;
    uint8_t bUseDirtyRegions    : 1;

    int8_t chSpaceBetweenParagraph;
    
} text_box_cfg_t;

/*!
 * \brief a user class for user defined control
 */

struct text_box_t {

ARM_PRIVATE(
    text_box_cfg_t tCFG;

    struct {
        int32_t nLine;
        int32_t nPosition;
    } Start;

    //struct {
        int32_t nTargetStartLineReq;
        uint16_t bUpdateReq         : 1;
        uint16_t                    : 15;
    //} Request;

    int16_t iLineWidth;

    __text_box_line_info_t tCurrentLine;

    arm_2d_helper_dirty_region_item_t tDirtyRegionItem[1];

#if 0
    __text_box_scratch_mem_t *ptFreeList;
    __text_box_scratch_mem_t *ptLineCache;
    uint16_t hwLineCacheCount;
#endif
)
    /* place your public member here */
    
};


typedef struct text_box_c_str_reader_t {
ARM_PRIVATE(
    const char *pchString;
    int32_t nSizeInByte;
    int32_t nPosition;
)
} text_box_c_str_reader_t;

/*============================ GLOBAL VARIABLES ==============================*/
extern
const text_box_io_handler_t TEXT_BOX_IO_C_STRING_READER;
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void text_box_init( text_box_t *ptThis,
                          text_box_cfg_t *ptCFG);
extern
ARM_NONNULL(1)
void text_box_depose( text_box_t *ptThis);

extern
ARM_NONNULL(1)
void text_box_on_load( text_box_t *ptThis);

extern
ARM_NONNULL(1)
void text_box_on_frame_start( text_box_t *ptThis);

extern
ARM_NONNULL(1)
void text_box_on_frame_complete( text_box_t *ptThis);

extern
ARM_NONNULL(1)
void text_box_show( text_box_t *ptThis,
                    const arm_2d_tile_t *ptTile, 
                    const arm_2d_region_t *ptRegion,
                    __arm_2d_color_t tColour,
                    uint8_t chOpacity,
                    bool bIsNewFrame);

/*!
 * \brief force update the text box.
 * \param[in] ptThis the target text box object
 */
extern
ARM_NONNULL(1)
void text_box_update(text_box_t *ptThis);

extern
ARM_NONNULL(1)
int32_t text_box_set_start_line(text_box_t *ptThis, int32_t iStartLine);

extern
ARM_NONNULL(1)
int32_t text_box_get_start_line(text_box_t *ptThis);

extern
ARM_NONNULL(1,2)
text_box_c_str_reader_t *text_box_c_str_reader_init(
                                                text_box_c_str_reader_t *ptThis,
                                                const char *pchString,
                                                int32_t nMaxLen);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
