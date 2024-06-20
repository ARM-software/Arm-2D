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

#ifndef __CONSOLE_BOX_H__
#define __CONSOLE_BOX_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d_helper.h"

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
#ifdef __CONSOLE_BOX_IMPLEMENT__
#   undef   __CONSOLE_BOX_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__CONSOLE_BOX_INHERIT__)
#   undef   __CONSOLE_BOX_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct arm_2d_font_t arm_2d_font_t;

typedef struct console_box_cfg_t {
    arm_2d_size_t tBoxSize;

    uint8_t *pchInputBuffer;
    uint8_t *pchConsoleBuffer;
    uint16_t hwConsoleBufferSize;
    uint16_t hwInputBufferSize;

    const arm_2d_font_t *ptFont;
    COLOUR_INT tColor;

    bool bUseDirtyRegion;

    arm_2d_region_list_item_t **ppDirtyRegionList;
} console_box_cfg_t;

/*!
 * \brief a user class for user defined control
 */
typedef struct console_box_t console_box_t;

struct console_box_t {

ARM_PRIVATE(

    const arm_2d_font_t *ptFont;
    arm_2d_size_t tBoxSize;

    arm_2d_byte_fifo_t tInputFIFO;
    arm_2d_byte_fifo_t tConsoleFIFO;

    struct {
        uint16_t hwMaxColumn;
        uint16_t hwMaxRow;
        uint16_t hwLastRow;
        uint16_t hwLastColumn;
        uint16_t hwCurrentRow;
        uint16_t hwCurrentColumn;
        uint16_t hwDirtyRegionRow;
    } Console;

    arm_2d_scene_t                  *ptTargetScene;
    arm_2d_region_t                 tReDrawRegion;
    arm_2d_region_list_item_t       tDirtyRegion;
    arm_2d_region_list_item_t       **ppDirtyRegionList;

    COLOUR_INT tColor;
    uint8_t bCFGNoInputFIFO         : 1;        /* Configuration: There is no FIFO for input */
    uint8_t bCFGUseDirtyRegion      : 1;        /* Configuration: Use Dirty Region to improve performance */
    uint8_t u2RTRefreshMode         : 2;        /* Runtime: Refresh Mode */
    uint8_t u2RTOneTimeRefreshMode  : 2;        /* Runtime: the refresh mode for one frame */
    uint8_t bREQClearScreen         : 1;        /* User Request: Clear the screen (asynchronously) */
    uint8_t                         : 1;
)

};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1,3)
bool console_box_init(  console_box_t *ptThis,
                        arm_2d_scene_t *ptTargetScene,
                        console_box_cfg_t *ptCFG);

extern
ARM_NONNULL(1)
void console_box_depose( console_box_t *ptThis);

extern
ARM_NONNULL(1)
void console_box_show(  console_box_t *ptThis,
                        const arm_2d_tile_t *ptTile, 
                        const arm_2d_region_t *ptRegion, 
                        bool bIsNewFrame,
                        uint8_t chOpacity);

/*!
 * \brief the on-frame-start event handler
 * \param[in] ptThis the target console box 
 * \retval false no content changed
 * \retval true the console is updated.
 */
extern
ARM_NONNULL(1)
bool console_box_on_frame_start(console_box_t *ptThis);

extern
ARM_NONNULL(1,2)
int console_box_printf(console_box_t *ptThis, const char *format, ...);

extern
ARM_NONNULL(1)
bool console_box_putchar(console_box_t *ptThis, uint8_t chChar);

extern
ARM_NONNULL(1)
void console_box_clear_screen(console_box_t *ptThis);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
