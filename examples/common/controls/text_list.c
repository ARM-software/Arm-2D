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
#define __SIMPLE_LIST_INHERIT__
#define __TEXT_LIST_IMPLEMENT__
#define __ARM_2D_HELPER_LIST_VIEW_INHERIT__

#include "./text_list.h"

#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

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
#   pragma clang diagnostic ignored "-Wformat-nonliteral"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif


#undef this
#define this        (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__
#   efine __va_list    va_list
#endif


static 
arm_fsm_rt_t __arm_2d_text_list_draw_list_core_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    text_list_t *ptThis = (text_list_t *)ptItem->ptListView;
    __simple_list_cfg_t *ptCFG = &this.use_as____simple_list_t.tSimpleListCFG;

    ARM_2D_UNUSED(ptItem);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    arm_2d_canvas(ptTile, __top_container) {
        arm_lcd_text_set_font(ptCFG->ptFont);

        arm_lcd_text_set_colour(ptCFG->tFontColour, ptCFG->tBackgroundColour);
        arm_lcd_text_set_display_mode(ARM_2D_DRW_PATN_MODE_COPY);
        
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_scale(0.0f);
        arm_lcd_text_set_opacity(255);

        /* print numbers */
        __simple_list_item_printf(
                &this.use_as____simple_list_t,
                 &__top_container,
                 this.use_as____simple_list_t.tSimpleListCFG.tTextAlignment,
                 this.tCFG.pchFormatString,
                 text_list_get_item_string(ptThis, ptItem->hwID));
    
        arm_lcd_text_set_target_framebuffer(NULL);
    }
    
    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1,2)
arm_2d_err_t text_list_init(  text_list_t *ptThis, 
                        text_list_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(0 != ptCFG->hwStringCount);
    assert(NULL != ptCFG->ptStrings);

    arm_2d_err_t tResult =  __simple_list_init(&this.use_as____simple_list_t,
                            &ptCFG->use_as____simple_list_cfg_t);

    if (ARM_2D_ERR_NONE != tResult) {
        return tResult;
    }

    /* provide the default drawing method */
    if (NULL == ptCFG->use_as____simple_list_cfg_t.fnOnDrawListItem ) {
        __simple_list_set_draw_list_item_handler(
                                    &this.use_as____simple_list_t,
                                    &__arm_2d_text_list_draw_list_core_item);
    }

    static const char c_chDefaultFormatString[] = {"%s"};

    this.tCFG.hwStringCount = ptCFG->hwStringCount;
    this.tCFG.ptString = ptCFG->ptStrings;

    if (NULL == this.tCFG.ptString || 0 == this.tCFG.hwStringCount) {
        return ARM_2D_ERR_INVALID_PARAM;
    } 

    this.tCFG.pchFormatString = ptCFG->pchFormatString;

    if (NULL == this.tCFG.pchFormatString) {
        /* use the default format string */
        this.tCFG.pchFormatString = c_chDefaultFormatString;
    }

    return tResult;
}


ARM_NONNULL(1)
void text_list_depose(text_list_t *ptThis)
{
    assert(NULL != ptThis);

    __simple_list_depose(&this.use_as____simple_list_t);
}


ARM_NONNULL(1)
void text_list_on_frame_start(text_list_t *ptThis)
{
    assert(NULL != ptThis);

    __simple_list_on_frame_start(&this.use_as____simple_list_t);
}

ARM_NONNULL(1,2)
arm_fsm_rt_t text_list_show(  text_list_t *ptThis,
                                const arm_2d_tile_t *ptTile, 
                                const arm_2d_region_t *ptRegion, 
                                bool bIsNewFrame)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);

    return __simple_list_show(&this.use_as____simple_list_t,
                            ptTile,
                            ptRegion,
                            bIsNewFrame);
}


ARM_NONNULL(1)
void text_list_move_selection( text_list_t *ptThis,
                                int16_t iSteps,
                                int32_t nFinishInMs)
{
    __simple_list_move_selection( &this.use_as____simple_list_t, 
                                iSteps, 
                                nFinishInMs);
}

ARM_NONNULL(1)
uint16_t text_list_get_selected_item_id(text_list_t *ptThis)
{
    return __simple_list_get_selected_item_id(&this.use_as____simple_list_t);
}


ARM_NONNULL(1)
__disp_string_t text_list_get_item_string(text_list_t *ptThis, uint_fast16_t hwItemID)
{
    static const __disp_string_t c_strDefaultString = "";
    assert(NULL != ptThis);
    if (hwItemID >= this.use_as____simple_list_t.tSimpleListCFG.hwCount) {
        return c_strDefaultString;
    }

    __disp_string_t tString = this.tCFG.ptString[hwItemID];
    if (NULL == tString) {
        return c_strDefaultString;
    }

    return tString;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
