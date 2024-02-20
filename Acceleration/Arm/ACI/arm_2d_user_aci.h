/*
 * Copyright (C) 2010-2024 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        arm_2d_user_aci.h
 * Description:  Public header file template for adding ACI acceleration
 *
 * $Date:        24 Aug 2023
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */

#if !defined(__ARM_2D_USER_ACI_H__) && __ARM_2D_HAS_ACI__
#define __ARM_2D_USER_ACI_H__

/*============================ INCLUDES ======================================*/
/*
 * NOTE: You can modify this header file for your ACI acceleration.
 */



/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/




/*============================ DISABLE DEFAULT HELIUM IMPLEMENTATION =========*/

#if defined(__ARM_2D_IMPLEMENT_HELIUM__) && __ARM_2D_IMPLEMENT_HELIUM__


#define __arm_2d_impl_rgb565_colour_filling_with_opacity    \
                 __arm_2d_impl_rgb565_colour_filling_with_opacity_orig

#define __arm_2d_impl_rgb565_colour_filling_mask_opacity \
                  __arm_2d_impl_rgb565_colour_filling_mask_opacity_orig

#define __arm_2d_impl_rgb565_tile_copy_opacity \
                  __arm_2d_impl_rgb565_tile_copy_opacity_orig

#define __arm_2d_impl_rgb565_tile_copy_colour_keying_opacity \
                __arm_2d_impl_rgb565_tile_copy_colour_keying_opacity_orig

#define __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy \
                __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_orig

#define __arm_2d_impl_rgb565_src_msk_1h_des_msk_fill \
                __arm_2d_impl_rgb565_src_msk_1h_des_msk_fill_orig

#define __arm_2d_impl_rgb565_colour_filling_mask_opacity_transform \
                __arm_2d_impl_rgb565_colour_filling_mask_opacity_transform_orig

#define __arm_2d_impl_rgb565_colour_filling_a2_mask_opacity \
                __arm_2d_impl_rgb565_colour_filling_a2_mask_opacity_orig

#define __arm_2d_impl_rgb565_colour_filling_a4_mask \
                __arm_2d_impl_rgb565_colour_filling_a4_mask_orig

#define __arm_2d_impl_rgb565_des_msk_copy \
                __arm_2d_impl_rgb565_des_msk_copy_orig

#define __arm_2d_impl_rgb565_transform_with_opacity \
                __arm_2d_impl_rgb565_transform_with_opacity_orig

#define __arm_2d_impl_rgb565_transform_with_src_mask \
                __arm_2d_impl_rgb565_transform_with_src_mask_orig

#define __arm_2d_impl_rgb565_transform \
                __arm_2d_impl_rgb565_transform_orig

#define __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_x_mirror \
                __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_x_mirror_orig

#define __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_y_mirror \
                __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_y_mirror_orig

#define __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_xy_mirror \
                __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_xy_mirror_orig



#endif

#endif                          /* end of __ARM_2D_USER_ACI_H__ */
