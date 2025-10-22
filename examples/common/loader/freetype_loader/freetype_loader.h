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

#ifndef __FREETYPE_LOADER_H__
#define __FREETYPE_LOADER_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"

#include <ft2build.h>
#include FT_FREETYPE_H

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
#ifdef __FREETYPE_LOADER_IMPLEMENT__
#   undef   __FREETYPE_LOADER_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__FREETYPE_LOADER_INHERIT__)
#   undef   __FREETYPE_LOADER_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
