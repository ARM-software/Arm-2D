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

#ifndef __TEXT_LIST_H__
#define __TEXT_LIST_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "arm_2d_helper_list.h"
#include "__simple_list.h"

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
#ifdef __TEXT_LIST_IMPLEMENT__
#   undef   __TEXT_LIST_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__TEXT_LIST_INHERIT__)
#   undef   __TEXT_LIST_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef char * __disp_string_t ;

typedef struct text_list_cfg_t {

    implement(__simple_list_cfg_t);

    const char *pchFormatString;
    __disp_string_t *ptStrings;

} text_list_cfg_t;

typedef struct text_list_t {
    implement(__simple_list_t);

    struct {
        const char *pchFormatString;
        __disp_string_t *ptString;
    } tTextListCFG;
} text_list_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1,2)
arm_2d_err_t text_list_init(  text_list_t *ptThis, 
                                text_list_cfg_t *ptCFG);

extern
ARM_NONNULL(1)
void text_list_depose(text_list_t *ptThis);

extern
ARM_NONNULL(1,2)
arm_fsm_rt_t text_list_show(  text_list_t *ptThis,
                                const arm_2d_tile_t *ptTile, 
                                const arm_2d_region_t *ptRegion, 
                                bool bIsNewFrame);

extern
ARM_NONNULL(1)
void text_list_move_selection( text_list_t *ptThis,
                                        int16_t iSteps,
                                        int32_t nFinishInMs);

extern 
ARM_NONNULL(1)
void text_list_on_frame_start(text_list_t *ptThis);


extern
ARM_NONNULL(1)
uint16_t text_list_get_selected_item_id(text_list_t *ptThis);

extern
ARM_NONNULL(1)
__disp_string_t text_list_get_item_string(text_list_t *ptThis, uint_fast16_t hwItemID);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
