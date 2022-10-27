
/*
 * Copyright (C) 2010-2021 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        __arm_2d_copy_mirror_wrapper.inc
 * Description:  c code template for tile copying and mirroring
 *
 * $Date:        27. Oct 2022
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */


#ifndef __API_CMW_COLOUR
#   error You have to define __API_CMW_COLOUR before using this c template
#endif
#ifndef __API_CMW_COLOUR_UPPERCASE
#   error You have to define __API_CMW_COLOUR_UPPERCASE before using this c template
#endif
#ifndef __API_CMW_COLOUR_SZ
#   error You have to define __API_CMW_COLOUR_SZ before using this c template
#endif
#ifndef __API_CMW_INT_TYPE
#   error You have to define the __API_CMW_INT_TYPE before using this c template
#endif


/*----------------------------------------------------------------------------*
 * copy only                                                                  *
 *----------------------------------------------------------------------------*/
#define __API_MCM_COLOUR                     __API_CMW_COLOUR
#define __API_MCM_COLOUR_UPPERCASE           __API_CMW_COLOUR_UPPERCASE
#define __API_MCM_COLOUR_SZ                  __API_CMW_COLOUR_SZ
#define __API_MCM_INT_TYPE                   __API_CMW_INT_TYPE
#define __API_MCM_OP                         ARM_2D_OP_TILE_COPY
#define __API_MCM_OP_TYPE                    arm_2d_op_cp_t
#define __API_MCM_MIRROR_TYPE                only
#define __API_MCM_MIRROR_TYPE_UPPERCASE      ONLY

#define __API_MCM_OP_IDX                     __ARM_2D_OP_IDX_COPY
#define __API_MCM_IO                         __ARM_2D_IO_COPY

#include "__arm_2d_meta_copy_mirror.inc"


/*----------------------------------------------------------------------------*
 * copy with x-mirror                                                         *
 *----------------------------------------------------------------------------*/
#define __API_MCM_COLOUR                     __API_CMW_COLOUR
#define __API_MCM_COLOUR_UPPERCASE           __API_CMW_COLOUR_UPPERCASE
#define __API_MCM_COLOUR_SZ                  __API_CMW_COLOUR_SZ
#define __API_MCM_INT_TYPE                   __API_CMW_INT_TYPE
#define __API_MCM_OP                         ARM_2D_OP_TILE_COPY
#define __API_MCM_OP_TYPE                    arm_2d_op_cp_t
#define __API_MCM_MIRROR_TYPE                x_mirror
#define __API_MCM_MIRROR_TYPE_UPPERCASE      X_MIRROR

#define __API_MCM_OP_IDX                     __ARM_2D_OP_IDX_COPY_WITH
#define __API_MCM_IO                         __ARM_2D_IO_COPY_WITH

#include "__arm_2d_meta_copy_mirror.inc"


/*----------------------------------------------------------------------------*
 * copy with y-mirror                                                         *
 *----------------------------------------------------------------------------*/
#define __API_MCM_COLOUR                     __API_CMW_COLOUR
#define __API_MCM_COLOUR_UPPERCASE           __API_CMW_COLOUR_UPPERCASE
#define __API_MCM_COLOUR_SZ                  __API_CMW_COLOUR_SZ
#define __API_MCM_INT_TYPE                   __API_CMW_INT_TYPE
#define __API_MCM_OP                         ARM_2D_OP_TILE_COPY
#define __API_MCM_OP_TYPE                    arm_2d_op_cp_t
#define __API_MCM_MIRROR_TYPE                y_mirror
#define __API_MCM_MIRROR_TYPE_UPPERCASE      Y_MIRROR

#define __API_MCM_OP_IDX                     __ARM_2D_OP_IDX_COPY_WITH
#define __API_MCM_IO                         __ARM_2D_IO_COPY_WITH
#include "__arm_2d_meta_copy_mirror.inc"

/*----------------------------------------------------------------------------*
 * copy with xy-mirror                                                        *
 *----------------------------------------------------------------------------*/
#define __API_MCM_COLOUR                     __API_CMW_COLOUR
#define __API_MCM_COLOUR_UPPERCASE           __API_CMW_COLOUR_UPPERCASE
#define __API_MCM_COLOUR_SZ                  __API_CMW_COLOUR_SZ
#define __API_MCM_INT_TYPE                   __API_CMW_INT_TYPE
#define __API_MCM_OP                         ARM_2D_OP_TILE_COPY
#define __API_MCM_OP_TYPE                    arm_2d_op_cp_t
#define __API_MCM_MIRROR_TYPE                xy_mirror
#define __API_MCM_MIRROR_TYPE_UPPERCASE      XY_MIRROR

#define __API_MCM_OP_IDX                     __ARM_2D_OP_IDX_COPY_WITH
#define __API_MCM_IO                         __ARM_2D_IO_COPY_WITH
#include "__arm_2d_meta_copy_mirror.inc"


/*----------------------------------------------------------------------------*
 * fill only                                                                  *
 *----------------------------------------------------------------------------*/

#define __API_MFM_COLOUR                     __API_CMW_COLOUR
#define __API_MFM_COLOUR_UPPERCASE           __API_CMW_COLOUR_UPPERCASE
#define __API_MFM_COLOUR_SZ                  __API_CMW_COLOUR_SZ
#define __API_MFM_INT_TYPE                   __API_CMW_INT_TYPE
#define __API_MFM_OP                         ARM_2D_OP_TILE_FILL
#define __API_MFM_OP_TYPE                    arm_2d_op_cp_t
#define __API_MFM_MIRROR_TYPE                only
#define __API_MFM_MIRROR_TYPE_UPPERCASE      ONLY
#define __API_MFM_OP_IDX                     __ARM_2D_OP_IDX_FILL
#define __API_MFM_IO_COPY                   __ARM_2D_IO_COPY
#define __API_MFM_IO_FILL                   __ARM_2D_IO_FILL

#include "__arm_2d_meta_fill_mirror.inc"


/*----------------------------------------------------------------------------*
 * fill with x-miorro                                                         *
 *----------------------------------------------------------------------------*/
#define __API_MFM_COLOUR                     __API_CMW_COLOUR
#define __API_MFM_COLOUR_UPPERCASE           __API_CMW_COLOUR_UPPERCASE
#define __API_MFM_COLOUR_SZ                  __API_CMW_COLOUR_SZ
#define __API_MFM_INT_TYPE                   __API_CMW_INT_TYPE
#define __API_MFM_OP                         ARM_2D_OP_TILE_FILL
#define __API_MFM_OP_TYPE                    arm_2d_op_cp_t
#define __API_MFM_MIRROR_TYPE                x_mirror
#define __API_MFM_MIRROR_TYPE_UPPERCASE      X_MIRROR
#define __API_MFM_OP_IDX                     __ARM_2D_OP_IDX_FILL_WITH
#define __API_MFM_IO_COPY                   __ARM_2D_IO_COPY_WITH
#define __API_MFM_IO_FILL                   __ARM_2D_IO_FILL_WITH

#include "__arm_2d_meta_fill_mirror.inc"


/*----------------------------------------------------------------------------*
 * fill with y-miorro                                                         *
 *----------------------------------------------------------------------------*/
#define __API_MFM_COLOUR                     __API_CMW_COLOUR
#define __API_MFM_COLOUR_UPPERCASE           __API_CMW_COLOUR_UPPERCASE
#define __API_MFM_COLOUR_SZ                  __API_CMW_COLOUR_SZ
#define __API_MFM_INT_TYPE                   __API_CMW_INT_TYPE
#define __API_MFM_OP                         ARM_2D_OP_TILE_FILL
#define __API_MFM_OP_TYPE                    arm_2d_op_cp_t
#define __API_MFM_MIRROR_TYPE                y_mirror
#define __API_MFM_MIRROR_TYPE_UPPERCASE      Y_MIRROR
#define __API_MFM_OP_IDX                     __ARM_2D_OP_IDX_FILL_WITH
#define __API_MFM_IO_COPY                   __ARM_2D_IO_COPY_WITH
#define __API_MFM_IO_FILL                   __ARM_2D_IO_FILL_WITH

#include "__arm_2d_meta_fill_mirror.inc"


/*----------------------------------------------------------------------------*
 * fill with xy-miorro                                                        *
 *----------------------------------------------------------------------------*/
#define __API_MFM_COLOUR                     __API_CMW_COLOUR
#define __API_MFM_COLOUR_UPPERCASE           __API_CMW_COLOUR_UPPERCASE
#define __API_MFM_COLOUR_SZ                  __API_CMW_COLOUR_SZ
#define __API_MFM_INT_TYPE                   __API_CMW_INT_TYPE
#define __API_MFM_OP                         ARM_2D_OP_TILE_FILL
#define __API_MFM_OP_TYPE                    arm_2d_op_cp_t
#define __API_MFM_MIRROR_TYPE                xy_mirror
#define __API_MFM_MIRROR_TYPE_UPPERCASE      XY_MIRROR
#define __API_MFM_OP_IDX                     __ARM_2D_OP_IDX_FILL_WITH
#define __API_MFM_IO_COPY                   __ARM_2D_IO_COPY_WITH
#define __API_MFM_IO_FILL                   __ARM_2D_IO_FILL_WITH

#include "__arm_2d_meta_fill_mirror.inc"


#undef __API_CMW_COLOUR_SZ
#undef __API_CMW_COLOUR_UPPERCASE
#undef __API_CMW_COLOUR
#undef __API_CMW_INT_TYPE
