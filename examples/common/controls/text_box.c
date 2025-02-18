/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
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
#define __TEXT_BOX_IMPLEMENT__

#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "text_box.h"
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
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static 
int32_t __c_string_io_read_utf8_char_handler_t(text_box_t *ptThis, 
                                            uintptr_t pObj, 
                                            uint8_t *pchBuffer,
                                            uint_fast16_t hwSize);

static 
int32_t __c_string_io_seek_handler_t(   text_box_t *ptThis, 
                                        uintptr_t pObj,
                                        int32_t nOffset,
                                        text_box_seek_whence_t enWhence);

/*============================ GLOBAL VARIABLES ==============================*/
const text_box_io_handler_t TEXT_BOX_IO_C_STRING_READER = {
    .fnGetChar = &__c_string_io_read_utf8_char_handler_t,
    .fnSeek = &__c_string_io_seek_handler_t,
};

/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
void text_box_init( text_box_t *ptThis,
                          text_box_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    memset(ptThis, 0, sizeof(text_box_t));
    this.tCFG = *ptCFG;


}

ARM_NONNULL(1)
void text_box_depose( text_box_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void text_box_on_load( text_box_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void text_box_on_frame_start( text_box_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void text_box_on_frame_complete( text_box_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void text_box_show( text_box_t *ptThis,
                    const arm_2d_tile_t *ptTile, 
                    const arm_2d_region_t *ptRegion,
                    __arm_2d_color_t tColour,
                    uint8_t chOpacity,
                    bool bIsNewFrame)
{
    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }

    assert(NULL!= ptThis);

    arm_2d_container(ptTile, __text_box, ptRegion) {
        arm_2d_draw_box(&__text_box, NULL, 1, GLCD_COLOR_BLUE, 128);
    }

    ARM_2D_OP_WAIT_ASYNC();
}

ARM_NONNULL(1,2)
text_box_c_str_reader_t *text_box_c_str_reader_init(
                                                text_box_c_str_reader_t *ptThis,
                                                const char *pchString,
                                                size_t tMaxLen)
{
    assert(NULL != ptThis);
    assert(NULL != pchString);

    memset(ptThis, 0, sizeof(text_box_c_str_reader_t));
    this.pchString = pchString;
    this.tSizeInByte = strnlen(pchString, tMaxLen);

    return ptThis;
}

static 
int32_t __c_string_io_read_utf8_char_handler_t( text_box_t *ptTextBox, 
                                                uintptr_t pObj, 
                                                uint8_t *pchBuffer,
                                                uint_fast16_t hwSize)
{
    text_box_c_str_reader_t *ptThis = (text_box_c_str_reader_t *)pObj;
    assert(NULL != ptThis);
    
    if (NULL == pchBuffer) {
        return -1;
    } else if (0 == hwSize) {
        return 0;
    } else if (this.tPosition >= this.tSizeInByte) {
        return 0;
    }

    uintptr_t wReadPosition = this.tPosition + (uintptr_t)this.pchString;

    size_t tLeftToRead = this.tSizeInByte - this.tPosition;
    hwSize = MIN(tLeftToRead, hwSize);

    memcpy(pchBuffer, (uint8_t *)wReadPosition, hwSize);
    this.tPosition += hwSize;

    return hwSize;
}
static 
int32_t __c_string_io_seek_handler_t(   text_box_t *ptTextBox, 
                                        uintptr_t pObj,
                                        int32_t nOffset,
                                        text_box_seek_whence_t enWhence)
{
    text_box_c_str_reader_t *ptThis = (text_box_c_str_reader_t *)pObj;
    assert(NULL != ptThis);
    int64_t lPosition = this.tPosition;
    switch (enWhence) {
        case TEXT_BOX_SEEK_SET:
            lPosition = nOffset;
            break;
        case TEXT_BOX_SEEK_CUR:
            lPosition = this.tPosition + nOffset;
            break;
        case TEXT_BOX_SEEK_END:
            lPosition = (this.tSizeInByte - 1) + nOffset;
            break;
        default:
            break;
    }

    if (lPosition < 0) {
        lPosition = 0;
    } else if (lPosition >= this.tSizeInByte) {
        lPosition = this.tSizeInByte;
    }
    this.tPosition = lPosition;

    return this.tPosition;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
