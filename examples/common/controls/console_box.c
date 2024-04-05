/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
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
#define __CONSOLE_BOX_IMPLEMENT__   1

#define __ARM_2D_HELPER_INHERIT__   1
#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "console_box.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

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

enum {
    REFRESH_MODE_NO_UPDATE,     /* nothing changed, no need to fresh */
    REFRESH_MODE_NEW_CHARS,     /* only refresh the new chars in the same line */
    REFRESH_MODE_NEW_LINES,     /* refresh new line(s) for line wrapping */
    REFRESH_MODE_WHOLE,         /* refresh the whole area */
};

enum {
    CONSOLE_BOX_DIRTY_REGION_START = 0,
    CONSOLE_BOX_DIRTY_REGION_DONE,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,3)
bool console_box_init(  console_box_t *ptThis,
                        arm_2d_scene_t *ptTargetScene,
                        console_box_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    assert(NULL!= ptCFG);

    memset(ptThis, 0, sizeof(console_box_t));

    if (!arm_2d_byte_fifo_init( &this.tConsoleFIFO,
                                ptCFG->pchConsoleBuffer,
                                ptCFG->hwConsoleBufferSize)) {
        return false;
    }

    if (!arm_2d_byte_fifo_init( &this.tInputFIFO,
                                ptCFG->pchInputBuffer,
                                ptCFG->hwInputBufferSize)) {
        this.bCFGNoInputFIFO = true;
    }

    this.ptTargetScene = ptTargetScene;

    this.ptFont = ptCFG->ptFont;
    if (NULL == this.ptFont) {
        this.ptFont = &ARM_2D_FONT_6x8.use_as__arm_2d_font_t;
    }

    this.tBoxSize = ptCFG->tBoxSize;
    if (0 == this.tBoxSize.iHeight || 0 == this.tBoxSize.iWidth) {
        return false;
    }

    this.tColor = ptCFG->tColor;

    do {
        this.Console.hwMaxColumn = this.tBoxSize.iWidth 
                                 / this.ptFont->tCharSize.iWidth;
        this.Console.hwMaxRow = this.tBoxSize.iHeight 
                              / this.ptFont->tCharSize.iHeight;

        if ((0 == this.Console.hwMaxColumn)
        ||  (0 == this.Console.hwMaxRow)) {
            return false;
        }
    } while(0);

    /* dirty region */

    if (ptCFG->bUseDirtyRegion) {

        arm_2d_scene_player_dynamic_dirty_region_init(&this.tDirtyRegion,
                                              this.ptTargetScene);

        if (NULL != this.ptTargetScene) {
            this.bCFGUseDirtyRegion = true;

        } else if (NULL != ptCFG->ppDirtyRegionList) {
            this.bCFGUseDirtyRegion = true;

            this.ppDirtyRegionList = ptCFG->ppDirtyRegionList;
            arm_2d_helper_pfb_append_dirty_regions_to_list(
                    this.ppDirtyRegionList,
                    &this.tDirtyRegion, 
                    1);
        }
    }

    return true;
}

ARM_NONNULL(1)
void console_box_depose( console_box_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (this.bCFGUseDirtyRegion) {

        arm_2d_scene_player_dynamic_dirty_region_depose(&this.tDirtyRegion, 
                                                this.ptTargetScene);

        if (NULL != this.ppDirtyRegionList) {

            arm_2d_helper_pfb_remove_dirty_regions_from_list(
                this.ppDirtyRegionList,
                &this.tDirtyRegion, 
                1);
        }
    }
}

static
bool __console_box_peek_utf8(arm_2d_byte_fifo_t *ptFIFO, uint32_t *pwUTF8)
{
    assert(NULL != ptFIFO);
    assert(NULL != pwUTF8);

    *pwUTF8 = 0;
    uint8_t *pchCode = (uint8_t *)pwUTF8;

    if (!arm_2d_byte_fifo_peek(ptFIFO, pchCode, true)) {
        /* reach end of the FIFO */
        return false;
    }

    int8_t chCodeLength = arm_2d_helper_get_utf8_byte_length(pchCode++);
    if (chCodeLength <= 0) {
        return false;
    }

    while(--chCodeLength) {
        if (!arm_2d_byte_fifo_peek(ptFIFO, pchCode, false)) {
            /* reach end of the FIFO unexpectedly */
            return false;
        }
        if ((*pchCode & 0xC0) != 0x80) { /* BYTE: 10xx-xxxx */
            /* illegal UTF8 */
            return false;
        }
        /* safe to peek */
        arm_2d_byte_fifo_peek(ptFIFO, pchCode++, true);
    }

    return true;
}

static 
void __console_box_remove_top_line(console_box_t *ptThis)
{
    arm_2d_byte_fifo_reset_peeked(&this.tConsoleFIFO);

    uint16_t hwColumn = 0;
    do {
        uint32_t wUTF8 = 0;
        if (!__console_box_peek_utf8(&this.tConsoleFIFO, &wUTF8)) {
            /* failed to get a complete UTF8 char */
            arm_2d_byte_fifo_get_all_peeked(&this.tConsoleFIFO);
            break;
        }

        if (wUTF8 == '\n') {
            /* successfully removed a line */
            arm_2d_byte_fifo_get_all_peeked(&this.tConsoleFIFO);
            break;
        }

        //bool bMoveToNextLine = false;

        switch ((char)wUTF8) {
            case '\r':
                hwColumn = 0;   /* move to start of the line */
                break;
            case '\b':
                if (hwColumn) { /* delete one byte */
                    hwColumn--;
                }
                break;
            case '\t':
                hwColumn += 4;
                hwColumn &= ~0x3;
                break;
            default:
                hwColumn++;
                break;
        }

        if (hwColumn >= this.Console.hwMaxColumn) {
            arm_2d_byte_fifo_reset_peeked(&this.tConsoleFIFO);
            /* detect line wrapping, this char belongs to the newline */
            break;
        }

        /* remove this UTF8 char and move to next one */
        arm_2d_byte_fifo_get_all_peeked(&this.tConsoleFIFO);

    } while(true);

    if (this.Console.hwCurrentRow) {
        this.Console.hwCurrentRow--;
    }

    /* refresh the whole console */
    this.u2RTRefreshMode = REFRESH_MODE_WHOLE;
}

ARM_NONNULL(1)
static 
void __console_box_force_to_write_console_fifo( console_box_t *ptThis, 
                                                uint8_t chInputChar)
{
    do {
        if (arm_2d_byte_fifo_enqueue(&this.tConsoleFIFO, chInputChar)) {
            /* check the byte written */
            bool bMoveToNextLine = false;

            switch (chInputChar) {
                case '\n':
                    bMoveToNextLine = true;
                    break;
                case '\r':
                #if 0   /* known issue: only use '\r' without '\n' will have letter-overlapping issue */
                    if (__byte_fifo_vomit(&this.tConsoleFIFO, this.Console.hwCurrentColumn + 1)) {
                        assert(false);
                    }
                #endif
                    this.Console.hwCurrentColumn = 0;   /* move to start of the line */
                    break;
                case '\b':
                    if (this.Console.hwCurrentColumn) { /* delete one byte */
                        this.Console.hwCurrentColumn--;
                    }
                    break;
                case '\t':
                    this.Console.hwCurrentColumn += 4;
                    this.Console.hwCurrentColumn &= ~0x3;
                    break;
                default:
                    this.Console.hwCurrentColumn++;
                    break;
            }

            if (this.Console.hwCurrentColumn >= this.Console.hwMaxColumn) {
                /* move to next line */
                bMoveToNextLine = true;
            }

            if (bMoveToNextLine) {
                this.Console.hwCurrentColumn = 0;
                this.Console.hwCurrentRow++;

                if (this.Console.hwCurrentRow >= this.Console.hwMaxRow) {
                    __console_box_remove_top_line(ptThis);
                } else if (this.u2RTRefreshMode < REFRESH_MODE_NEW_LINES) {
                    this.u2RTRefreshMode = REFRESH_MODE_NEW_LINES;
                }
            } else if (this.u2RTRefreshMode == REFRESH_MODE_NO_UPDATE){
                this.u2RTRefreshMode = REFRESH_MODE_NEW_CHARS;
            }

            return ;
        }
        /* the CONSOLE FIFO is FULL, we have to remove the top line */
        __console_box_remove_top_line(ptThis);

    } while(true);

}

ARM_NONNULL(1)
bool console_box_putchar(console_box_t *ptThis, uint8_t chChar)
{
    assert(NULL!= ptThis);

    if (this.bCFGNoInputFIFO) {
        __console_box_force_to_write_console_fifo(ptThis, chChar);
        return true;
    }

    return arm_2d_byte_fifo_enqueue(&this.tInputFIFO, chChar);
}

ARM_NONNULL(1)
void console_box_clear_screen(console_box_t *ptThis)
{
    arm_irq_safe {
        this.bREQClearScreen = true;
    }
}

ARM_NONNULL(1)
bool console_box_on_frame_start(console_box_t *ptThis)
{
    assert(NULL != ptThis);
    bool bREQClearScreen = false;

    arm_irq_safe {
        if (this.bREQClearScreen) {
            bREQClearScreen = true;
            this.bREQClearScreen = false;
        }
    }

    /* clear screen */
    if (bREQClearScreen) {
        arm_2d_byte_fifo_drop_all(&this.tConsoleFIFO);
        this.u2RTRefreshMode = REFRESH_MODE_WHOLE;      /* refresh the whole screen */
        this.Console.hwCurrentColumn = 0;
        this.Console.hwCurrentRow = 0;
        this.Console.hwLastRow = 0;
        this.Console.hwLastColumn = 0;
    }

    do {
        if (this.bCFGNoInputFIFO) {
            break;
        }

        uint8_t chByte;
        if (!arm_2d_byte_fifo_dequeue(&this.tInputFIFO, &chByte)){
            /* nothing to read */
            break;
        }

        /* write to console buffer */
        __console_box_force_to_write_console_fifo(ptThis, chByte);

    } while(true);

    if (this.bCFGUseDirtyRegion) {
        arm_2d_size_t tCharSize = this.ptFont->tCharSize;

        this.u2RTOneTimeRefreshMode = this.u2RTRefreshMode;
        /* reset fresh mode */
        this.u2RTRefreshMode = REFRESH_MODE_NO_UPDATE;

        switch (this.u2RTOneTimeRefreshMode) {
            default:
            case REFRESH_MODE_NO_UPDATE:
                break;
            
            case REFRESH_MODE_NEW_CHARS: {
                    int32_t iCharDelta = (int32_t)this.Console.hwCurrentColumn 
                                       - (int32_t)this.Console.hwLastColumn;
                    
                    /* update size */
                    this.tReDrawRegion.tSize.iHeight = tCharSize.iHeight;
                    this.tReDrawRegion.tSize.iWidth = tCharSize.iWidth * ABS(iCharDelta);

                    /* update location */
                    if (iCharDelta > 0) {
                        this.tReDrawRegion.tLocation.iX 
                            = this.Console.hwLastColumn * tCharSize.iWidth;
                    } else {
                        this.tReDrawRegion.tLocation.iX 
                            = ((int32_t)this.Console.hwLastColumn + iCharDelta) 
                            * tCharSize.iWidth;
                    }

                    this.tReDrawRegion.tLocation.iY = this.Console.hwLastRow * tCharSize.iHeight;
                }
                break;

            case REFRESH_MODE_NEW_LINES:
                /* update location */
                this.tReDrawRegion.tLocation.iX = 0;
                this.tReDrawRegion.tLocation.iY = this.Console.hwLastRow * tCharSize.iHeight;

                /* this whole line */
                this.tReDrawRegion.tSize.iWidth = tCharSize.iWidth * this.Console.hwMaxColumn;
                this.tReDrawRegion.tSize.iHeight 
                    = tCharSize.iHeight 
                    * (this.Console.hwCurrentRow - this.Console.hwLastRow + 1);

                break;
            
            case REFRESH_MODE_WHOLE:
                /* update location */
                this.tReDrawRegion.tLocation.iX = 0;
                this.tReDrawRegion.tLocation.iY = 0;

                /* update size */
                this.tReDrawRegion.tSize = this.tBoxSize;
                break;

        }

        arm_2d_dynamic_dirty_region_on_frame_start(&this.tDirtyRegion, CONSOLE_BOX_DIRTY_REGION_START);

        this.Console.hwLastColumn = this.Console.hwCurrentColumn;
        this.Console.hwLastRow = this.Console.hwCurrentRow;
    }

    return this.u2RTOneTimeRefreshMode != REFRESH_MODE_NO_UPDATE;
}

#if defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__
#define __va_list    va_list
#endif

ARM_NONNULL(1,2)
int console_box_printf(console_box_t *ptThis, const char *format, ...)
{
    int real_size, n;
    char s_chBuffer[128];
    char *pchSrc = s_chBuffer;

    __va_list ap;
    va_start(ap, format);
        real_size = vsnprintf(s_chBuffer, sizeof(s_chBuffer)-1, format, ap);
    va_end(ap);
    real_size = MIN(sizeof(s_chBuffer)-1, real_size);
    s_chBuffer[real_size] = '\0';
    n = real_size;
    
    do {
        if (!console_box_putchar(ptThis, *pchSrc++)) {
            break;
        }
    } while(--n);

    return real_size;
}



ARM_NONNULL(1)
void console_box_show(  console_box_t *ptThis,
                        const arm_2d_tile_t *ptTile, 
                        const arm_2d_region_t *ptRegion, 
                        bool bIsNewFrame,
                        uint8_t chOpacity)
{
    assert(NULL!= ptThis);

    arm_2d_container(ptTile, __console_box, ptRegion) {

        /* example code: flash a 50x50 red box in the centre */
        arm_2d_align_centre(__console_box_canvas, this.tBoxSize) {

            if (this.bCFGUseDirtyRegion) {
            
                switch (arm_2d_dynamic_dirty_region_wait_next(
                                                        &this.tDirtyRegion)) {
                    case CONSOLE_BOX_DIRTY_REGION_START:
                        if (    this.u2RTOneTimeRefreshMode 
                            !=  REFRESH_MODE_NO_UPDATE) {
                            this.tReDrawRegion.tLocation.iX 
                                += __centre_region.tLocation.iX;
                            this.tReDrawRegion.tLocation.iY 
                                += __centre_region.tLocation.iY;

                            arm_2d_dynamic_dirty_region_update(
                                                &this.tDirtyRegion,
                                                &__console_box,
                                                &this.tReDrawRegion,
                                                CONSOLE_BOX_DIRTY_REGION_DONE);

                        } else {
                            arm_2d_dynamic_dirty_region_change_user_region_index_only(
                                &this.tDirtyRegion,
                                CONSOLE_BOX_DIRTY_REGION_DONE);
                        }
                        break;
                    case CONSOLE_BOX_DIRTY_REGION_DONE:
                        break;
                    default:    /* 0xFF */
                        break;
                }
            
            }

            /* draw text at the top-left corner */
            arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)&__console_box);
            arm_lcd_text_set_font(this.ptFont);
            arm_lcd_text_set_draw_region(&__centre_region);
            arm_lcd_text_set_colour(this.tColor, GLCD_COLOR_BLACK);

            /* force all char use the same with in display */
            arm_using(bool bOriginal = arm_lcd_text_force_char_use_same_with(true),
                      /* on leave resume the original configuration */
                      arm_lcd_text_force_char_use_same_with(bOriginal)) {

                uint32_t wUTF8;
                arm_2d_byte_fifo_reset_peeked(&this.tConsoleFIFO);
                do {
                    if (!__console_box_peek_utf8(&this.tConsoleFIFO, &wUTF8)) {
                        break;
                    }

                    arm_lcd_putchar((const char *)&wUTF8);
                } while(true);
            }
            

            /* make sure the operation is complete */
            arm_2d_op_wait_async(NULL);
        }

    }

    arm_2d_op_wait_async(NULL);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
