/*
 * Copyright (C) 2010-2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        arm-2d_tile.c
 * Description:  Basic Tile operations
 *
 * $Date:        12. May 2025
 * $Revision:    V.1.5.1
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wempty-translation-unit"
#endif


#ifdef __ARM_2D_COMPILATION_UNIT
#undef __ARM_2D_COMPILATION_UNIT

#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"
#include "__arm_2d_paving.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wextra-semi-stmt"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif

/*----------------------------------------------------------------------------*
 * Code Template                                                              *
 *----------------------------------------------------------------------------*/

#define __API_COLOUR                c8bit
#define __API_INT_TYPE              uint8_t
#define __API_INT_TYPE_BIT_NUM      8

#include "__arm_2d_ll_copy.inc"


#define __API_COLOUR                rgb16
#define __API_INT_TYPE              uint16_t
#define __API_INT_TYPE_BIT_NUM      16

#include "__arm_2d_ll_copy.inc"


#define __API_COLOUR                rgb32
#define __API_INT_TYPE              uint32_t
#define __API_INT_TYPE_BIT_NUM      32

#include "__arm_2d_ll_copy.inc"

/*----------------------------------------------------------------------------*
 * Tile Operations                                                            *
 *----------------------------------------------------------------------------*/

/*
  HOW IT WORKS:

    Input Region 0
  +------------------------------------------------------+
  |                                                      |
  |                                                      |
  |                                                      |
  |                       +------------------------------+---------+
  |                       |                              |/////////|
  |                       |        Output Region         |/////////|
  |                       |                              |/////////|
  +-----------------------+------------------------------+/////////|
                          |////////////////////////////////////////|
                          |////////////////////////////////////////|
                          +----------------------------------------+
                                                   Input Region 1
 */
ARM_NONNULL(1,2)
bool arm_2d_region_intersect(   const arm_2d_region_t *ptRegionIn0,
                                const arm_2d_region_t *ptRegionIn1,
                                arm_2d_region_t *ptRegionOut)
{
    assert(ptRegionIn0 != NULL);
    assert(ptRegionIn1 != NULL);

    if (    (ptRegionIn0->tSize.iWidth == 0)
        ||  (ptRegionIn0->tSize.iHeight == 0)
        ||  (ptRegionIn1->tSize.iWidth == 0)
        ||  (ptRegionIn1->tSize.iHeight == 0)) {
        return false;
    }

    do {
        arm_2d_location_t tLocationIn0End = {
                                .iX = ptRegionIn0->tLocation.iX
                                    + ptRegionIn0->tSize.iWidth
                                    - 1,
                                .iY = ptRegionIn0->tLocation.iY
                                    + ptRegionIn0->tSize.iHeight
                                    - 1,
                            };

        arm_2d_location_t tLocationIn1End = {
                                .iX = ptRegionIn1->tLocation.iX
                                    + ptRegionIn1->tSize.iWidth
                                    - 1,
                                .iY = ptRegionIn1->tLocation.iY
                                    + ptRegionIn1->tSize.iHeight
                                    - 1,
                            };

        arm_2d_location_t tLocationOutStart = {
                                .iX = MAX(  ptRegionIn0->tLocation.iX,
                                            ptRegionIn1->tLocation.iX),

                                .iY = MAX(  ptRegionIn0->tLocation.iY,
                                            ptRegionIn1->tLocation.iY),
                            };

        arm_2d_location_t tLocationOutEnd = {
                                .iX = MIN(  tLocationIn0End.iX,
                                            tLocationIn1End.iX),
                                .iY = MIN(  tLocationIn0End.iY,
                                            tLocationIn1End.iY),
                            };

        if (    (tLocationOutStart.iX > tLocationOutEnd.iX)
            ||  (tLocationOutStart.iY > tLocationOutEnd.iY)) {
            return false;
        }

        if (NULL != ptRegionOut) {
            ptRegionOut->tLocation = tLocationOutStart;
            ptRegionOut->tSize.iWidth = tLocationOutEnd.iX
                                      - tLocationOutStart.iX
                                      + 1;

            ptRegionOut->tSize.iHeight = tLocationOutEnd.iY
                                       - tLocationOutStart.iY
                                       + 1;
        }
    } while(0);

    return true;
}

ARM_NONNULL(1,2,3)
arm_2d_region_t *arm_2d_region_get_minimal_enclosure(const arm_2d_region_t *ptInput0,
                                                     const arm_2d_region_t *ptInput1,
                                                     arm_2d_region_t *ptOutput)
{
    assert(NULL != ptInput0);
    assert(NULL != ptInput1);
    assert(NULL != ptOutput);

    do {
        if ((ptInput0->tSize.iWidth <= 0) || (ptInput0->tSize.iHeight <= 0)) {
            *ptOutput = *ptInput1;
            break;
        } else if ((ptInput1->tSize.iWidth <= 0) || (ptInput1->tSize.iHeight <= 0)) {
            *ptOutput = *ptInput0;
            break;
        }

        int16_t x00, y00, x01, y01, x10, y10, x11, y11;
        x00 = ptInput0->tLocation.iX;
        y00 = ptInput0->tLocation.iY;
        x10 = ptInput1->tLocation.iX;
        y10 = ptInput1->tLocation.iY;

        arm_2d_location_t tTopLeft = {
                .iX = MIN(x00, x10),
                .iY = MIN(y00, y10),
            };

        x01 = x00 + ptInput0->tSize.iWidth - 1;
        y01 = y00 + ptInput0->tSize.iHeight - 1;
        x11 = x10 + ptInput1->tSize.iWidth - 1;
        y11 = y10 + ptInput1->tSize.iHeight - 1; 
        
        arm_2d_location_t tBottomRight = {
                .iX = MAX(x01, x11),
                .iY = MAX(y01, y11),
            };

        ptOutput->tLocation = tTopLeft;
        ptOutput->tSize.iWidth = tBottomRight.iX - tTopLeft.iX + 1;
        ptOutput->tSize.iHeight = tBottomRight.iY - tTopLeft.iY + 1;
    } while(0);

    return ptOutput;
}

ARM_NONNULL(1)
arm_2d_region_t *arm_2d_create_region_from_corner_points(arm_2d_region_t *ptOutput, 
                                                         arm_2d_location_t tPointA, 
                                                         arm_2d_location_t tPointB)
{
    if (NULL == ptOutput) {
        return NULL;
    }

    arm_2d_region_t tRegionA = {
        .tLocation = tPointA,
        .tSize = {1,1},
    };

    arm_2d_region_t tRegionB = {
        .tLocation = tPointB,
        .tSize = {1,1},
    };

    return arm_2d_region_get_minimal_enclosure(&tRegionA, &tRegionB, ptOutput);
}

ARM_NONNULL(1,2)
bool arm_2d_is_point_inside_region( const arm_2d_region_t *ptRegion,
                                    const arm_2d_location_t *ptPoint)
{
    assert(ptRegion != NULL);
    assert(ptPoint != NULL);
    
    do {
        if          ((0 == ptRegion->tSize.iWidth) || (0 == ptRegion->tSize.iHeight)) {
            break;
        } else if   (ptPoint->iX < ptRegion->tLocation.iX) {
            break;
        } else if   (ptPoint->iY < ptRegion->tLocation.iY) {
            break;
        } else if   (ptPoint->iX >= ptRegion->tLocation.iX + ptRegion->tSize.iWidth) {
            break;
        } else if   (ptPoint->iY >= ptRegion->tLocation.iY + ptRegion->tSize.iHeight) {
            break;
        }
        
        return true;
    } while(0);

    return false;
}

ARM_NONNULL(1,2)
int_fast8_t arm_2d_is_region_inside_target(const arm_2d_region_t *ptRegion,
                                           const arm_2d_region_t *ptTarget)
{
    do {
        arm_2d_location_t tTopLeft = ptRegion->tLocation;
        arm_2d_location_t tBottomRight = {
            .iX = ptRegion->tLocation.iX + ptRegion->tSize.iWidth - 1,
            .iY = ptRegion->tLocation.iY + ptRegion->tSize.iHeight - 1,
        };

        if (    arm_2d_is_point_inside_region(ptTarget, &tTopLeft)
            &&  arm_2d_is_point_inside_region(ptTarget, &tBottomRight)) {
            return 1;
        }
    } while(0);

    do {
        arm_2d_location_t tTopLeft = ptTarget->tLocation;
        arm_2d_location_t tBottomRight = {
            .iX = ptTarget->tLocation.iX + ptTarget->tSize.iWidth - 1,
            .iY = ptTarget->tLocation.iY + ptTarget->tSize.iHeight - 1,
        };

        if (    arm_2d_is_point_inside_region(ptRegion, &tTopLeft)
            &&  arm_2d_is_point_inside_region(ptRegion, &tBottomRight)) {
            return -1;
        }
    } while(0);

    return 0;
}


ARM_NONNULL(1)
const arm_2d_tile_t *__arm_2d_tile_get_1st_derived_child_or_root(
                                            const arm_2d_tile_t *ptTile,
                                            arm_2d_region_t *ptValidRegion,
                                            arm_2d_location_t *ptOffset,
                                            arm_2d_tile_t **ppFirstDerivedChild,
                                            bool bQuitWhenFindFirstDerivedChild)
{
    assert(NULL != ptTile);

    arm_2d_region_t tValidRegion = ptTile->tRegion;

    if (NULL != ppFirstDerivedChild) {
        *ppFirstDerivedChild = NULL;        /* initialise */
    }

    if (NULL != ptOffset) {
        ptOffset->iX = 0;
        ptOffset->iY = 0;
    }

    if (arm_2d_is_root_tile(ptTile)) {
        if (NULL != ptValidRegion) {
            *ptValidRegion = tValidRegion;
        }
        return ptTile;
    }

    do {
        if (ptTile->tInfo.bDerivedResource) {
            if (NULL != ppFirstDerivedChild) {
                /* ensure this is the first derived child */
                if (NULL == *ppFirstDerivedChild) {
                    *ppFirstDerivedChild = (arm_2d_tile_t *)ptTile;
                }
            }
            
            if (bQuitWhenFindFirstDerivedChild) {
                break;
            }
        }

        //! get parent
        ptTile = (const arm_2d_tile_t *)ptTile->ptParent;
        if (NULL == ptTile) {
            break;
        }
        
        /*! \note Calculate the relative position between valid region and
         *!       the tile's original region. Usually, the tile's region
         *!       is inside the parent tile, but when the tile's location is
         *!       out of the parent's region with one or more negative axies,
         *!       the offset will be non-zero.
         *!       The offset is used to indicate the tile's view, and the
         *!       valid region is seen as inside the tile's region.
         *!
         *!   Figure: What's the meaning of offset location
         *!
         *!   The special case, where the child tile has a negative coordinates,
         *!   hence, the offset is (a,b) **as if** the valid region is inside
         *!   the child tile.
         *!
         *!    (-a,-b) Child Tile
         *!       +------------------------------------+
         *!       |///(0,0) Parent Tile ///////////////|
         *!       |/////+------------------------------+---------+
         *!       |/////|                              |         |
         *!       |/////|       Valid Region           |         |
         *!       |/////|                              |         |
         *!       +-----+------------------------------+         |
         *!             |                                        |
         *!             |                                        |
         *!             +----------------------------------------+
         *!
         */
        if (NULL != ptOffset) {
            arm_2d_location_t tOffset = tValidRegion.tLocation;
            tOffset.iX = MAX(0, -tOffset.iX);
            tOffset.iY = MAX(0, -tOffset.iY);

            ptOffset->iX += tOffset.iX;
            ptOffset->iY += tOffset.iY;
        }

        /*! calculate the valid range in parent tile
         *!
         *! \note the location of the parent tile is used to indicate its
         *!       relative location between the it and its parent.
         *!       when calculate the valid range in parent, we have to assume
         *!       that the location is always (0,0)
         *!
         */
        arm_2d_region_t tParentRegion = {
            .tSize = ptTile->tRegion.tSize,
        };

        if (arm_2d_is_root_tile(ptTile)) {
            /* root tile can has offset */
            tParentRegion.tLocation = ptTile->tRegion.tLocation;
        }

        /*! make sure the output region is valid */
        if (!arm_2d_region_intersect(   &tParentRegion,
                                        &tValidRegion,
                                        &tValidRegion)) {
            /* out of range */
            return NULL;
        }

        if (arm_2d_is_root_tile(ptTile)) {
            break;
        }

        tValidRegion.tLocation.iX += ptTile->tRegion.tLocation.iX;
        tValidRegion.tLocation.iY += ptTile->tRegion.tLocation.iY;

    } while(true);

    if (NULL != ptValidRegion) {
        *ptValidRegion = tValidRegion;
    }

    return ptTile;
}



ARM_NONNULL(1)
const arm_2d_tile_t *__arm_2d_tile_get_virtual_screen_or_root(
                                        const arm_2d_tile_t *ptTile,
                                        arm_2d_region_t *ptValidRegion,
                                        arm_2d_location_t *ptOffset,
                                        const arm_2d_tile_t **ppVirtualScreen,
                                        bool bQuitWhenFindVirtualScreen)
{
    assert(NULL != ptTile);

    arm_2d_region_t tValidRegion = ptTile->tRegion;

    if (NULL != ppVirtualScreen) {
        *ppVirtualScreen = NULL;        /* initialise */
    }

    if (NULL != ptOffset) {
        ptOffset->iX = 0;
        ptOffset->iY = 0;
    }

    if (arm_2d_is_root_tile(ptTile)) {
        if (NULL != ptValidRegion) {
            *ptValidRegion = tValidRegion;
        }
        return ptTile;
    }

    if (ptTile->tInfo.bVirtualScreen) {
        if (NULL != ppVirtualScreen) {
            /* ensure this is the first virtual screen */
            if (NULL == *ppVirtualScreen) {
                *ppVirtualScreen = (arm_2d_tile_t *)ptTile;
            }
        }
        
        if (bQuitWhenFindVirtualScreen) {

            if (NULL != ptValidRegion) {
                *ptValidRegion = tValidRegion;
                ptValidRegion->tLocation.iX = 0;
                ptValidRegion->tLocation.iY = 0;
            }
            return ptTile;
        }
    }

    do {

        //! get parent
        ptTile = (const arm_2d_tile_t *)ptTile->ptParent;
        if (NULL == ptTile) {
            break;
        }
        
        /*! \note Calculate the relative position between valid region and
         *!       the tile's original region. Usually, the tile's region
         *!       is inside the parent tile, but when the tile's location is
         *!       out of the parent's region with one or more negative axies,
         *!       the offset will be non-zero.
         *!       The offset is used to indicate the tile's view, and the
         *!       valid region is seen as inside the tile's region.
         *!
         *!   Figure: What's the meaning of offset location
         *!
         *!   The special case, where the child tile has a negative coordinates,
         *!   hence, the offset is (a,b) **as if** the valid region is inside
         *!   the child tile.
         *!
         *!    (-a,-b) Child Tile
         *!       +------------------------------------+
         *!       |///(0,0) Parent Tile ///////////////|
         *!       |/////+------------------------------+---------+
         *!       |/////|                              |         |
         *!       |/////|       Valid Region           |         |
         *!       |/////|                              |         |
         *!       +-----+------------------------------+         |
         *!             |                                        |
         *!             |                                        |
         *!             +----------------------------------------+
         *!
         */
        if (NULL != ptOffset) {
            arm_2d_location_t tOffset = tValidRegion.tLocation;
            tOffset.iX = MAX(0, -tOffset.iX);
            tOffset.iY = MAX(0, -tOffset.iY);

            ptOffset->iX += tOffset.iX;
            ptOffset->iY += tOffset.iY;
        }

        /*! calculate the valid range in parent tile
         *!
         *! \note the location of the parent tile is used to indicate its
         *!       relative location between the it and its parent.
         *!       when calculate the valid range in parent, we have to assume
         *!       that the location is always (0,0)
         *!
         */
        arm_2d_region_t tParentRegion = {
            .tSize = ptTile->tRegion.tSize,
        };

        if (arm_2d_is_root_tile(ptTile)) {
            /* root tile can has offset */
            tParentRegion.tLocation = ptTile->tRegion.tLocation;
        }

        /*! make sure the output region is valid */
        if (!arm_2d_region_intersect(   &tParentRegion,
                                        &tValidRegion,
                                        &tValidRegion)) {
            /* out of range */
            return NULL;
        }

        if (ptTile->tInfo.bVirtualScreen) {
            if (NULL != ppVirtualScreen) {
                /* ensure this is the first virtual screen */
                if (NULL == *ppVirtualScreen) {
                    *ppVirtualScreen = (arm_2d_tile_t *)ptTile;
                }
            }
            
            if (bQuitWhenFindVirtualScreen) {
                break;
            }
        }

        if (arm_2d_is_root_tile(ptTile)) {
            break;
        }

        tValidRegion.tLocation.iX += ptTile->tRegion.tLocation.iX;
        tValidRegion.tLocation.iY += ptTile->tRegion.tLocation.iY;

    } while(true);

    if (NULL != ptValidRegion) {
        *ptValidRegion = tValidRegion;
    }

    return ptTile;
}

ARM_NONNULL(1)
const arm_2d_tile_t *__arm_2d_tile_get_root(const arm_2d_tile_t *ptTile,
                                            arm_2d_region_t *ptValidRegion,
                                            arm_2d_location_t *ptOffset,
                                            arm_2d_tile_t **ppFirstDerivedChild)
{
    return __arm_2d_tile_get_1st_derived_child_or_root( ptTile, 
                                                        ptValidRegion, 
                                                        ptOffset, 
                                                        ppFirstDerivedChild, 
                                                        false);
}

/*
  HOW IT WORKS:

   Root Tile (Output Tile)
  +------------------------------------------------------------------------+
  |     ... ...                                                            |
  |                                                                        |
  |                   Parent Tile                                          |
  |                 +------------------------------------+                 |
  |                 |        Child Tile                  |                 |
  |                 |     +------------------------------+---------+       |
  |                 |     |                              |/////////|       |
  |                 |     |       Valid Region           |/////////|       |
  |                 |     |                              |/////////|       |
  |                 +-----+------------------------------+/////////|       |
  |                       |////////////////////////////////////////|       |
  |                       |////////////////////////////////////////|       |
  |                       +----------------------------------------+       |
  |                                                                        |
  +------------------------------------------------------------------------+
 */
ARM_NONNULL(1)
const arm_2d_tile_t *arm_2d_tile_get_root(  const arm_2d_tile_t *ptTile,
                                            arm_2d_region_t *ptValidRegion,
                                            arm_2d_location_t *ptOffset)
{
    return __arm_2d_tile_get_1st_derived_child_or_root( ptTile, 
                                                        ptValidRegion, 
                                                        ptOffset, 
                                                        NULL, 
                                                        false);
}

ARM_NONNULL(1)
arm_2d_err_t arm_2d_target_tile_is_new_frame(const arm_2d_tile_t *ptTarget)
{
    const arm_2d_tile_t *ptScreen = NULL;
    arm_2d_err_t tResult = ARM_2D_ERR_INVALID_PARAM;
    do {
        if (NULL == __arm_2d_tile_get_virtual_screen_or_root(   ptTarget,
                                                                NULL,
                                                                NULL,
                                                                &ptScreen,
                                                                true)) {
            break;
        }

        tResult = ARM_2D_RT_TRUE;
        if ((ptScreen->tInfo.u3ExtensionID != ARM_2D_TILE_EXTENSION_PFB)) {
            break;
        }
        if (ptScreen->tInfo.Extension.PFB.bIsNewFrame) {
            break;
        }

        return ARM_2D_RT_FALSE;
    } while(0);

    return tResult;
}


ARM_NONNULL(1,2)
arm_2d_cmp_t arm_2d_tile_width_compare( const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptReference)
{
    assert(ptTarget != NULL);
    assert(ptReference != NULL);
    arm_2d_region_t tTargetRegion;
    arm_2d_region_t tReferenceRegion;
    
    ptTarget = arm_2d_tile_get_root(ptTarget, &tTargetRegion, NULL);
    ptReference = arm_2d_tile_get_root(ptReference, &tReferenceRegion, NULL);
    
    if (NULL == ptTarget) {
        if (NULL != ptReference) {
            return ARM_2D_CMP_SMALLER;
        }
        return ARM_2D_CMP_EQUALS;
    } else if (NULL == ptReference) {
        return ARM_2D_CMP_LARGER;
    }
    
    if (tTargetRegion.tSize.iWidth > tReferenceRegion.tSize.iWidth) {
        return ARM_2D_CMP_LARGER;
    } else if (tTargetRegion.tSize.iWidth < tReferenceRegion.tSize.iWidth) {
        return ARM_2D_CMP_SMALLER;
    }
    
    return ARM_2D_CMP_EQUALS;
}


ARM_NONNULL(1,2)
arm_2d_cmp_t arm_2d_tile_height_compare(const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptReference)
{
    assert(ptTarget != NULL);
    assert(ptReference != NULL);
    arm_2d_region_t tTargetRegion;
    arm_2d_region_t tReferenceRegion;
    
    ptTarget = arm_2d_tile_get_root(ptTarget, &tTargetRegion, NULL);
    ptReference = arm_2d_tile_get_root(ptReference, &tReferenceRegion, NULL);
    
    if (NULL == ptTarget) {
        if (NULL != ptReference) {
            return ARM_2D_CMP_SMALLER;
        }
        return ARM_2D_CMP_EQUALS;
    } else if (NULL == ptReference) {
        return ARM_2D_CMP_LARGER;
    }
    
    if (tTargetRegion.tSize.iHeight > tReferenceRegion.tSize.iHeight) {
        return ARM_2D_CMP_LARGER;
    } else if (tTargetRegion.tSize.iHeight < tReferenceRegion.tSize.iHeight) {
        return ARM_2D_CMP_SMALLER;
    }
    
    return ARM_2D_CMP_EQUALS;
}

ARM_NONNULL(1,2)
arm_2d_cmp_t arm_2d_tile_shape_compare(   const arm_2d_tile_t *ptTarget,
                                            const arm_2d_tile_t *ptReference)
{
    assert(ptTarget != NULL);
    assert(ptReference != NULL);
    arm_2d_region_t tTargetRegion;
    arm_2d_region_t tReferenceRegion;
    
    ptTarget = arm_2d_tile_get_root(ptTarget, &tTargetRegion, NULL);
    ptReference = arm_2d_tile_get_root(ptReference, &tReferenceRegion, NULL);
    
    if (NULL == ptTarget) {
        if (NULL != ptReference) {
            return ARM_2D_CMP_SMALLER;
        }
        return ARM_2D_CMP_EQUALS;
    } else if (NULL == ptReference) {
        return ARM_2D_CMP_LARGER;
    }
    
    if (tTargetRegion.tSize.iWidth < tReferenceRegion.tSize.iWidth) {
        return ARM_2D_CMP_SMALLER;
    }
    
    if (tTargetRegion.tSize.iHeight < tReferenceRegion.tSize.iHeight) {
        return ARM_2D_CMP_SMALLER;
    }
    
    if (    (tTargetRegion.tSize.iWidth == tReferenceRegion.tSize.iWidth)
       &&   (tTargetRegion.tSize.iHeight == tReferenceRegion.tSize.iHeight)) {
        return ARM_2D_CMP_EQUALS;
    }
    
    return ARM_2D_CMP_LARGER;
}


ARM_NONNULL(1,2)
const arm_2d_tile_t * arm_2d_tile_get_absolute_location(
                                               const arm_2d_tile_t *ptTile, 
                                               arm_2d_location_t *ptLocation)
{
    
    assert(NULL != ptTile);
    assert(NULL != ptLocation);
    
    ptLocation->iX = 0;
    ptLocation->iY = 0;

    *ptLocation = arm_2d_get_absolute_location(ptTile, *ptLocation, false);
    
    return ptTile;
}

ARM_NONNULL(1,2,3)
arm_2d_region_t *arm_2d_tile_region_diff(   const arm_2d_tile_t *ptTarget,
                                            const arm_2d_tile_t *ptReference,
                                            arm_2d_region_t *ptBuffer)
{
    assert(NULL != ptTarget);
    assert(NULL != ptReference);
    assert(NULL != ptBuffer);
    
    //! get the absolute location
    arm_2d_location_t tTargetAbsoluteLocaton, tReferenceAbsoluteLocation;
    
    ptBuffer->tSize.iWidth  = ptTarget->tRegion.tSize.iWidth 
                            - ptReference->tRegion.tSize.iWidth;
    ptBuffer->tSize.iHeight = ptTarget->tRegion.tSize.iHeight 
                            - ptReference->tRegion.tSize.iHeight;
   
    ptTarget = arm_2d_tile_get_absolute_location(ptTarget, &tTargetAbsoluteLocaton);
    ptReference = arm_2d_tile_get_absolute_location(ptReference, &tReferenceAbsoluteLocation);
    
    if (ptTarget != ptReference) {
        //! they don't have the same root
        return NULL;
    }
    
    ptBuffer->tLocation.iX  = tTargetAbsoluteLocaton.iX 
                            - tReferenceAbsoluteLocation.iX;
    ptBuffer->tLocation.iY  = tTargetAbsoluteLocaton.iY 
                            - tReferenceAbsoluteLocation.iY;

    return ptBuffer;
}

/*
  HOW IT WORKS:

   Parent Tile (Are NOT necessarily a ROOT tile )
  +------------------------------------------------------+
  |                                                      |
  |                                                      |
  |                         Target Region                |
  |                       +------------------------------+---------+
  |                       |                              |/////////|
  |                       |    New Child Tile (Output)   |/////////|
  |                       |                              |/////////|
  +-----------------------+------------------------------+/////////|
                          |////////////////////////////////////////|
                          |////////////////////////////////////////|
                          +----------------------------------------+
 */
ARM_NONNULL(1,2,3)
arm_2d_tile_t *arm_2d_tile_generate_child(
                                        const arm_2d_tile_t *ptParentTile,
                                        const arm_2d_region_t *ptRegion,
                                        arm_2d_tile_t *ptOutput,
                                        bool bClipRegion)
{
    assert(NULL != ptParentTile);
    assert(NULL != ptRegion);
    assert(NULL != ptOutput);

    memset(ptOutput, 0, sizeof(arm_2d_tile_t));
    ptOutput->tRegion = *ptRegion;

    arm_2d_region_t tParentRegion = {
        .tSize = ptParentTile->tRegion.tSize,
    };

    if (bClipRegion) {
        if (!arm_2d_region_intersect(   &tParentRegion,
                                        &(ptOutput->tRegion),
                                        &(ptOutput->tRegion)
                                        )) {
            /* out of range */
            return NULL;
        }
    } else {
        /*! \note since you are allowed to generate a child tile whose region is
         *!       bigger than its parent, so we don't have to clip it, see **note**
         *!       below.
         */
        if (!arm_2d_region_intersect(   &tParentRegion,
                                        &(ptOutput->tRegion),
                                        NULL //&(ptOutput->tRegion) //!< **note**
                                        )) {
            /* out of range */
            return NULL;
        }
    }
    
    ptOutput->tInfo = ptParentTile->tInfo;
    ptOutput->tInfo.bIsRoot = false;
    ptOutput->tInfo.bVirtualScreen = false;
    
    /* user has to manually set this flag to true to indicate a derived resource */
    ptOutput->tInfo.bDerivedResource = false;

    ptOutput->ptParent = (arm_2d_tile_t *)ptParentTile;

    return ptOutput;
}

ARM_NONNULL(1)
arm_2d_location_t arm_2d_get_absolute_location( const arm_2d_tile_t *ptTile, 
                                                arm_2d_location_t tLocation,
                                                bool bOnVirtualScreen)
{
    assert(NULL != ptTile);
    if (bOnVirtualScreen) {
        while( !ptTile->tInfo.bIsRoot && !ptTile->tInfo.bVirtualScreen ) {
            tLocation.iX += ptTile->tRegion.tLocation.iX;
            tLocation.iY += ptTile->tRegion.tLocation.iY;
            
            ptTile = ptTile->ptParent;

            assert(NULL != ptTile);
        }
    } else {
        while( !ptTile->tInfo.bIsRoot) {
            tLocation.iX += ptTile->tRegion.tLocation.iX;
            tLocation.iY += ptTile->tRegion.tLocation.iY;
            
            ptTile = ptTile->ptParent;

            assert(NULL != ptTile);
        }
    }
    
    return tLocation;
}


ARM_NONNULL(1,2)
void arm_2d_sw_normal_root_tile_copy(   const arm_2d_tile_t *ptSource, 
                                        arm_2d_tile_t *ptTarget, 
                                        const arm_2d_region_t *ptTargetRegion,
                                        uint_fast8_t chBytesPerPixel)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);
    assert(ptSource->tInfo.bIsRoot);
    assert(ptTarget->tInfo.bIsRoot);
    assert(false == ptSource->tInfo.bVirtualResource);
    assert(false == ptSource->tInfo.bVirtualResource);

    assert(chBytesPerPixel > 0);

    arm_2d_region_t tTargetRegion = {
        .tSize = ptTarget->tRegion.tSize,
    };

    /* get a valid tTargetRegion */
    if (NULL == ptTargetRegion) {
        ptTargetRegion = &tTargetRegion;
    } else {
        if (!arm_2d_region_intersect(&tTargetRegion, ptTargetRegion, &tTargetRegion)) {
            /* out of region */
            return ;
        }
    }

    int16_t iHeight = MIN(ptSource->tRegion.tSize.iHeight, tTargetRegion.tSize.iHeight);
    int16_t iWidth = MIN(ptSource->tRegion.tSize.iWidth, tTargetRegion.tSize.iWidth);

    uint32_t wCopyStrideInByte = iWidth * chBytesPerPixel;
    uint32_t wSourceStrideInByte = ptSource->tRegion.tSize.iWidth * chBytesPerPixel;
    uint32_t wTargetStrideInByte = ptTarget->tRegion.tSize.iWidth * chBytesPerPixel;

    /* offset target address */
    uintptr_t pTarget = ptTarget->nAddress 
                      + tTargetRegion.tLocation.iY * wTargetStrideInByte
                      + tTargetRegion.tLocation.iX * chBytesPerPixel;

    /* offset source address */
    uintptr_t pSource = ptSource->nAddress;
    if (ptTargetRegion->tLocation.iY < 0) {
        pSource += (-ptTargetRegion->tLocation.iY) * wSourceStrideInByte;
    }
    if (ptTargetRegion->tLocation.iX < 0) {
        pSource += (-ptTargetRegion->tLocation.iX) * chBytesPerPixel;
    }

    switch (chBytesPerPixel) {
        case 3:
        case 1:
            for (int_fast16_t y = 0; y < iHeight; y++) {

                memcpy((void *)pTarget, (void *)pSource, wCopyStrideInByte);
        
                pSource += wSourceStrideInByte;
                pTarget += wTargetStrideInByte;
            }
            break;
        case 2:
            for (int_fast16_t y = 0; y < iHeight; y++) {

                memcpy((uint16_t *)pTarget, (uint16_t *)pSource, wCopyStrideInByte);
        
                pSource += wSourceStrideInByte;
                pTarget += wTargetStrideInByte;
            }
            break;

        case 4:
            for (int_fast16_t y = 0; y < iHeight; y++) {

                memcpy((uint32_t *)pTarget, (uint32_t *)pSource, wCopyStrideInByte);
        
                pSource += wSourceStrideInByte;
                pTarget += wTargetStrideInByte;
            }
            break;
        default:
            break;
    }
}

/*----------------------------------------------------------------------------*
 * Copy/Fill tile to destination with Mirroring                               *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_copy(arm_2d_op_cp_t *ptOP,
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_cp_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_GRAY8:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));
    
    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_C8BIT;
    
    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_copy(arm_2d_op_cp_t *ptOP,
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_cp_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_RGB565:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));
    
    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_RGB16;
    
    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_copy(arm_2d_op_cp_t *ptOP,
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint32_t wMode)
{

    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_cp_t, ptOP);

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
    arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(ptSource, NULL, NULL);
    if (NULL == ptSourceRoot) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }
    if (ptSourceRoot->tInfo.bHasEnforcedColour) {
        switch (ptSourceRoot->tInfo.tColourInfo.chScheme) {
            case ARM_2D_COLOUR_CCCN888:
            case ARM_2D_COLOUR_CCCA8888:
                /* code */
                break;
            
            default:
                return (arm_fsm_rt_t)ARM_2D_ERR_NOT_SUPPORT;
                //break;
        }
    }
#endif

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));
    
    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_RGB32;
    
    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    uint32_t wMode = this.wMode;

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
        __arm_2d_impl_c8bit_copy_mirror(ptTask->Param.tCopy.tSource.pBuffer,
                                        ptTask->Param.tCopy.tSource.iStride,
                                        ptTask->Param.tCopy.tTarget.pBuffer,
                                        ptTask->Param.tCopy.tTarget.iStride,
                                        &ptTask->Param.tCopy.tCopySize,
                                        wMode);
    } else {
    
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
        arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(this.Source.ptTile, NULL, NULL);
        assert(NULL != ptSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_to_gray8(
                                        ptTask->Param.tCopy.tSource.pBuffer,
                                        ptTask->Param.tCopy.tSource.iStride,
                                        ptTask->Param.tCopy.tTarget.pBuffer,
                                        ptTask->Param.tCopy.tTarget.iStride,
                                        &ptTask->Param.tCopy.tCopySize);
        } else
    #endif

        {
        __arm_2d_impl_c8bit_copy(   ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize);
        }
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    uint32_t wMode = this.wMode;

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
        __arm_2d_impl_rgb16_copy_mirror(ptTask->Param.tCopy.tSource.pBuffer,
                                        ptTask->Param.tCopy.tSource.iStride,
                                        ptTask->Param.tCopy.tTarget.pBuffer,
                                        ptTask->Param.tCopy.tTarget.iStride,
                                        &ptTask->Param.tCopy.tCopySize,
                                        wMode);
    } else {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
        arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(this.Source.ptTile, NULL, NULL);
        assert(NULL != ptSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_to_rgb565(
                                        ptTask->Param.tCopy.tSource.pBuffer,
                                        ptTask->Param.tCopy.tSource.iStride,
                                        ptTask->Param.tCopy.tTarget.pBuffer,
                                        ptTask->Param.tCopy.tTarget.iStride,
                                        &ptTask->Param.tCopy.tCopySize);
        } else
    #endif
        {
        __arm_2d_impl_rgb16_copy(   ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize);
        }
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    uint32_t wMode = this.wMode;

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
        __arm_2d_impl_rgb32_copy_mirror(ptTask->Param.tCopy.tSource.pBuffer,
                                        ptTask->Param.tCopy.tSource.iStride,
                                        ptTask->Param.tCopy.tTarget.pBuffer,
                                        ptTask->Param.tCopy.tTarget.iStride,
                                        &ptTask->Param.tCopy.tCopySize,
                                        wMode);
    } else {
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
        arm_2d_tile_t *ptSourceRoot = arm_2d_tile_get_root(this.Source.ptTile, NULL, NULL);
        assert(NULL != ptSourceRoot);
        if (ARM_2D_COLOUR_CCCA8888 == ptSourceRoot->tInfo.tColourInfo.chScheme) {
            __arm_2d_impl_ccca8888_to_cccn888(
                                        ptTask->Param.tCopy.tSource.pBuffer,
                                        ptTask->Param.tCopy.tSource.iStride,
                                        ptTask->Param.tCopy.tTarget.pBuffer,
                                        ptTask->Param.tCopy.tTarget.iStride,
                                        &ptTask->Param.tCopy.tCopySize);
        } else
    #endif
        {
            __arm_2d_impl_rgb32_copy(   ptTask->Param.tCopy.tSource.pBuffer,
                                        ptTask->Param.tCopy.tSource.iStride,
                                        ptTask->Param.tCopy.tTarget.pBuffer,
                                        ptTask->Param.tCopy.tTarget.iStride,
                                        &ptTask->Param.tCopy.tCopySize);
        }
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    uint32_t wMode = this.wMode;

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

        __arm_2d_impl_c8bit_fill_mirror(
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
                                wMode);

    
    } else {
        __arm_2d_impl_c8bit_fill(   
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize);
    }
        
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    uint32_t wMode = this.wMode;

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

        __arm_2d_impl_rgb16_fill_mirror(
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
                                wMode);

    
    } else {
        __arm_2d_impl_rgb16_fill(   
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize);
    }
        
    return arm_fsm_rt_cpl;
}


arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    uint32_t wMode = this.wMode;

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
    
        __arm_2d_impl_rgb32_fill_mirror(
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
                                wMode);
    
    } else {
        __arm_2d_impl_rgb32_fill(   
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize);
    }
        
    return arm_fsm_rt_cpl;
}

#define ARM_2D_CP_MODE_ONLY                 ARM_2D_CP_MODE_COPY
#define ARM_2D_COLOUR_C8BIT                 ARM_2D_COLOUR_8BIT

#define __arm_2d_impl_c8bit_copy_only       __arm_2d_impl_c8bit_copy
#define __arm_2d_impl_rgb16_copy_only       __arm_2d_impl_rgb16_copy
#define __arm_2d_impl_rgb32_copy_only       __arm_2d_impl_rgb32_copy

#define arm_2dp_c8bit_tile_copy_with_only   arm_2dp_c8bit_tile_copy_only
#define arm_2dp_rgb16_tile_copy_with_only   arm_2dp_rgb16_tile_copy_only
#define arm_2dp_rgb32_tile_copy_with_only   arm_2dp_rgb32_tile_copy_only


#define __arm_2d_impl_c8bit_fill_only       __arm_2d_impl_c8bit_fill
#define __arm_2d_impl_rgb16_fill_only       __arm_2d_impl_rgb16_fill
#define __arm_2d_impl_rgb32_fill_only       __arm_2d_impl_rgb32_fill

#define arm_2dp_c8bit_tile_fill_with_only   arm_2dp_c8bit_tile_fill_only
#define arm_2dp_rgb16_tile_fill_with_only   arm_2dp_rgb16_tile_fill_only
#define arm_2dp_rgb32_tile_fill_with_only   arm_2dp_rgb32_tile_fill_only


#define __API_CMW_COLOUR                     c8bit
#define __API_CMW_COLOUR_UPPERCASE           C8BIT
#define __API_CMW_COLOUR_SZ                  ARM_2D_COLOUR_SZ_8BIT
#define __API_CMW_INT_TYPE                   uint8_t

#include "__arm_2d_copy_mirror_wrapper.inc"


#define __API_CMW_COLOUR                     rgb16
#define __API_CMW_COLOUR_UPPERCASE           RGB16
#define __API_CMW_COLOUR_SZ                  ARM_2D_COLOUR_SZ_16BIT
#define __API_CMW_INT_TYPE                   uint16_t

#include "__arm_2d_copy_mirror_wrapper.inc"


#define __API_CMW_COLOUR                     rgb32
#define __API_CMW_COLOUR_UPPERCASE           RGB32
#define __API_CMW_COLOUR_SZ                  ARM_2D_COLOUR_SZ_32BIT
#define __API_CMW_INT_TYPE                   uint32_t

#include "__arm_2d_copy_mirror_wrapper.inc"


/*----------------------------------------------------------------------------*
 * Copy/Fill with colour-keying and Mirroring                                 *
 *----------------------------------------------------------------------------*/

/*! \brief copy source tile to destination tile and use destination tile as 
 *!        background. When encountering specified mask colour, the background
 *!        pixel should be used, otherwise the foreground pixel from source tile
 *!        is used. 
 *!         
 *! \note  All color formats which using 8bits per pixel are treated equally.
 *!
 */

ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_c8bit_tile_copy_with_colour_keying(
                                            arm_2d_op_cp_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource, 
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint8_t chMaskColour,
                                            uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_cp_cl_key_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = 
        &ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_C8BIT;
    
    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.chColour = chMaskColour;
    
    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

/*! \brief copy source tile to destination tile and use destination tile as 
 *!        background. When encountering specified mask colour, the background
 *!        pixel should be used, otherwise the foreground pixel from source tile
 *!        is used. 
 *!         
 *! \note  All color formats which using 16bits per pixel are treated equally.
 *! 
 *! \note  alpha channel is not handled, i.e. rgba5551
 */

ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb16_tile_copy_with_colour_keying(
                                            arm_2d_op_cp_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource, 
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint16_t hwMaskColour,
                                            uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_cp_cl_key_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = 
        &ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_RGB16;
    
    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.hwColour = hwMaskColour;
    
    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

/*! \brief copy source tile to destination tile and use destination tile as 
 *!        background. When encountering specified mask colour, the background
 *!        pixel should be used, otherwise the foreground pixel from source tile
 *!        is used. 
 *! 
 *! \note  All color formats which using 32bits per pixel are treated equally.
 *! 
 *! \note  alpha channel is not handled.
 */
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb32_tile_copy_with_colour_keying(
                                            arm_2d_op_cp_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource, 
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint32_t wMaskColour,
                                            uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_cp_cl_key_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = 
        &ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_RGB32;
    
    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.wColour = wMaskColour;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


arm_fsm_rt_t __arm_2d_c8bit_sw_tile_copy_with_colour_keying(
                                            __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_cl_key_t, ptTask->ptOP)
    
    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    uint32_t wMode = this.wMode;
    
    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

        __arm_2d_impl_c8bit_cl_key_copy_mirror(
                                            ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &ptTask->Param.tCopy.tCopySize,
                                            wMode,
                                            this.hwColour);

    } else {
        __arm_2d_impl_c8bit_cl_key_copy(   
                                            ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &ptTask->Param.tCopy.tCopySize,
                                            this.hwColour);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb16_sw_tile_copy_with_colour_keying(
                                            __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_cl_key_t, ptTask->ptOP)
    
    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    uint32_t wMode = this.wMode;
    
    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

        __arm_2d_impl_rgb16_cl_key_copy_mirror(
                                            ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &ptTask->Param.tCopy.tCopySize,
                                            wMode,
                                            this.hwColour);

    } else {
        __arm_2d_impl_rgb16_cl_key_copy(   
                                            ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &ptTask->Param.tCopy.tCopySize,
                                            this.hwColour);
    }

    return arm_fsm_rt_cpl;
}


arm_fsm_rt_t __arm_2d_rgb32_sw_tile_copy_with_colour_keying(
                                            __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_cl_key_t, ptTask->ptOP)
    
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    uint32_t wMode = this.wMode;

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

        __arm_2d_impl_rgb32_cl_key_copy_mirror(
                                            ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &ptTask->Param.tCopy.tCopySize,
                                            wMode,
                                            this.wColour);
    } else {
        __arm_2d_impl_rgb32_cl_key_copy(   
                                            ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &ptTask->Param.tCopy.tCopySize,
                                            this.wColour);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_c8bit_sw_tile_fill_with_colour_keying( 
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_cl_key_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    uint32_t wMode = this.wMode;

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
        __arm_2d_impl_c8bit_cl_key_fill_mirror(
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
                                wMode,
                                this.hwColour);
    } else {
        __arm_2d_impl_c8bit_cl_key_fill(
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
                                this.hwColour);
    }

        
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb16_sw_tile_fill_with_colour_keying( 
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_cl_key_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    uint32_t wMode = this.wMode;

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {
        __arm_2d_impl_rgb16_cl_key_fill_mirror(
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
                                wMode,
                                this.hwColour);
    } else {
        __arm_2d_impl_rgb16_cl_key_fill(
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
                                this.hwColour);
    }

        
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb32_sw_tile_fill_with_colour_keying( 
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_cp_cl_key_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    uint32_t wMode = this.wMode;

    if (wMode & (ARM_2D_CP_MODE_Y_MIRROR | ARM_2D_CP_MODE_X_MIRROR)) {

        __arm_2d_impl_rgb32_cl_key_fill_mirror(
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
                                wMode,
                                this.wColour);

    } else {

        __arm_2d_impl_rgb32_cl_key_fill(
                                ptTask->Param.tFill.tSource.pBuffer,
                                ptTask->Param.tFill.tSource.iStride,
                                &ptTask->Param.tFill.tSource.tValidRegion.tSize,
                                ptTask->Param.tFill.tTarget.pBuffer,
                                ptTask->Param.tFill.tTarget.iStride,
                                &ptTask->Param.tFill.tTarget.tValidRegion.tSize,
                                this.wColour);

    }

        
    return arm_fsm_rt_cpl;
}



#define __arm_2d_impl_c8bit_cl_key_copy_only       __arm_2d_impl_c8bit_cl_key_copy
#define __arm_2d_impl_rgb16_cl_key_copy_only       __arm_2d_impl_rgb16_cl_key_copy
#define __arm_2d_impl_rgb32_cl_key_copy_only       __arm_2d_impl_rgb32_cl_key_copy

#define __arm_2d_c8bit_sw_tile_copy_with_colour_keying_only __arm_2d_c8bit_sw_tile_copy_with_colour_keying_and_only
#define __arm_2d_rgb16_sw_tile_copy_with_colour_keying_only __arm_2d_rgb16_sw_tile_copy_with_colour_keying_and_only
#define __arm_2d_rgb32_sw_tile_copy_with_colour_keying_only __arm_2d_rgb32_sw_tile_copy_with_colour_keying_and_only

#define arm_2dp_c8bit_tile_copy_with_colour_keying_and_only   arm_2dp_c8bit_tile_copy_with_colour_keying_only
#define arm_2dp_rgb16_tile_copy_with_colour_keying_and_only   arm_2dp_rgb16_tile_copy_with_colour_keying_only
#define arm_2dp_rgb32_tile_copy_with_colour_keying_and_only   arm_2dp_rgb32_tile_copy_with_colour_keying_only

#define __arm_2d_impl_c8bit_cl_key_fill_only       __arm_2d_impl_c8bit_cl_key_fill
#define __arm_2d_impl_rgb16_cl_key_fill_only       __arm_2d_impl_rgb16_cl_key_fill
#define __arm_2d_impl_rgb32_cl_key_fill_only       __arm_2d_impl_rgb32_cl_key_fill


#define __arm_2d_c8bit_sw_tile_fill_with_colour_keying_only __arm_2d_c8bit_sw_tile_fill_with_colour_keying_and_only
#define __arm_2d_rgb16_sw_tile_fill_with_colour_keying_only __arm_2d_rgb16_sw_tile_fill_with_colour_keying_and_only
#define __arm_2d_rgb32_sw_tile_fill_with_colour_keying_only __arm_2d_rgb32_sw_tile_fill_with_colour_keying_and_only

#define arm_2dp_c8bit_tile_fill_with_colour_keying_and_only   arm_2dp_c8bit_tile_fill_with_colour_keying_only
#define arm_2dp_rgb16_tile_fill_with_colour_keying_and_only   arm_2dp_rgb16_tile_fill_with_colour_keying_only
#define arm_2dp_rgb32_tile_fill_with_colour_keying_and_only   arm_2dp_rgb32_tile_fill_with_colour_keying_only

#define __API_CKMW_COLOUR                     c8bit
#define __API_CKMW_COLOUR_UPPERCASE           C8BIT
#define __API_CKMW_COLOUR_SZ                  ARM_2D_COLOUR_SZ_8BIT
#define __API_CKMW_INT_TYPE                   uint8_t

#include "__arm_2d_copy_with_colour_keying_and_mirror_wrapper.inc"


#define __API_CKMW_COLOUR                     rgb16
#define __API_CKMW_COLOUR_UPPERCASE           RGB16
#define __API_CKMW_COLOUR_SZ                  ARM_2D_COLOUR_SZ_16BIT
#define __API_CKMW_INT_TYPE                   uint16_t

#include "__arm_2d_copy_with_colour_keying_and_mirror_wrapper.inc"


#define __API_CKMW_COLOUR                     rgb32
#define __API_CKMW_COLOUR_UPPERCASE           RGB32
#define __API_CKMW_COLOUR_SZ                  ARM_2D_COLOUR_SZ_32BIT
#define __API_CKMW_INT_TYPE                   uint32_t

#include "__arm_2d_copy_with_colour_keying_and_mirror_wrapper.inc"


/*----------------------------------------------------------------------------*
 * Low Level IO Interfaces                                                    *
 *----------------------------------------------------------------------------*/
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_C8BIT, __arm_2d_c8bit_sw_tile_copy);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_RGB16, __arm_2d_rgb16_sw_tile_copy);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_RGB32, __arm_2d_rgb32_sw_tile_copy);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_ONLY_C8BIT, __arm_2d_c8bit_sw_tile_copy_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_ONLY_RGB16, __arm_2d_rgb16_sw_tile_copy_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_ONLY_RGB32, __arm_2d_rgb32_sw_tile_copy_only);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_X_MIRROR_C8BIT, __arm_2d_c8bit_sw_tile_copy_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_X_MIRROR_RGB16, __arm_2d_rgb16_sw_tile_copy_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_X_MIRROR_RGB32, __arm_2d_rgb32_sw_tile_copy_x_mirror);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_Y_MIRROR_C8BIT, __arm_2d_c8bit_sw_tile_copy_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_Y_MIRROR_RGB16, __arm_2d_rgb16_sw_tile_copy_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_Y_MIRROR_RGB32, __arm_2d_rgb32_sw_tile_copy_y_mirror);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_XY_MIRROR_C8BIT, __arm_2d_c8bit_sw_tile_copy_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_XY_MIRROR_RGB16, __arm_2d_rgb16_sw_tile_copy_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_XY_MIRROR_RGB32, __arm_2d_rgb32_sw_tile_copy_xy_mirror);



__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_ONLY_C8BIT, __arm_2d_c8bit_sw_tile_fill_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_ONLY_RGB16, __arm_2d_rgb16_sw_tile_fill_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_ONLY_RGB32, __arm_2d_rgb32_sw_tile_fill_only);


__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_X_MIRROR_C8BIT, __arm_2d_c8bit_sw_tile_fill_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_X_MIRROR_RGB16, __arm_2d_rgb16_sw_tile_fill_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_X_MIRROR_RGB32, __arm_2d_rgb32_sw_tile_fill_x_mirror);


__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_Y_MIRROR_C8BIT, __arm_2d_c8bit_sw_tile_fill_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_Y_MIRROR_RGB16, __arm_2d_rgb16_sw_tile_fill_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_Y_MIRROR_RGB32, __arm_2d_rgb32_sw_tile_fill_y_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_XY_MIRROR_C8BIT, __arm_2d_c8bit_sw_tile_fill_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_XY_MIRROR_RGB16, __arm_2d_rgb16_sw_tile_fill_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_XY_MIRROR_RGB32, __arm_2d_rgb32_sw_tile_fill_xy_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_C8BIT, __arm_2d_c8bit_sw_tile_fill);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_RGB16, __arm_2d_rgb16_sw_tile_fill);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_RGB32, __arm_2d_rgb32_sw_tile_fill);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_C8BIT, 
                __arm_2d_c8bit_sw_tile_copy_with_colour_keying);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_RGB16, 
                __arm_2d_rgb16_sw_tile_copy_with_colour_keying);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_RGB32, 
                __arm_2d_rgb32_sw_tile_copy_with_colour_keying);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_ONLY_C8BIT, 
                __arm_2d_c8bit_sw_tile_copy_with_colour_keying_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_ONLY_RGB16, 
                __arm_2d_rgb16_sw_tile_copy_with_colour_keying_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_ONLY_RGB32, 
                __arm_2d_rgb32_sw_tile_copy_with_colour_keying_only);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_X_MIRROR_C8BIT, 
                __arm_2d_c8bit_sw_tile_copy_with_colour_keying_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_X_MIRROR_RGB16, 
                __arm_2d_rgb16_sw_tile_copy_with_colour_keying_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_X_MIRROR_RGB32, 
                __arm_2d_rgb32_sw_tile_copy_with_colour_keying_and_x_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_Y_MIRROR_C8BIT, 
                __arm_2d_c8bit_sw_tile_copy_with_colour_keying_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_Y_MIRROR_RGB16, 
                __arm_2d_rgb16_sw_tile_copy_with_colour_keying_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_Y_MIRROR_RGB32, 
                __arm_2d_rgb32_sw_tile_copy_with_colour_keying_and_y_mirror);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_XY_MIRROR_C8BIT, 
                __arm_2d_c8bit_sw_tile_copy_with_colour_keying_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_XY_MIRROR_RGB16, 
                __arm_2d_rgb16_sw_tile_copy_with_colour_keying_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_XY_MIRROR_RGB32, 
                __arm_2d_rgb32_sw_tile_copy_with_colour_keying_and_xy_mirror);


__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_C8BIT, 
                __arm_2d_c8bit_sw_tile_fill_with_colour_keying);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_RGB16, 
                __arm_2d_rgb16_sw_tile_fill_with_colour_keying);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_RGB32, 
                __arm_2d_rgb32_sw_tile_fill_with_colour_keying);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_ONLY_C8BIT, 
                __arm_2d_c8bit_sw_tile_fill_with_colour_keying_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_ONLY_RGB16, 
                __arm_2d_rgb16_sw_tile_fill_with_colour_keying_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_ONLY_RGB32, 
                __arm_2d_rgb32_sw_tile_fill_with_colour_keying_only);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_X_MIRROR_C8BIT, 
                __arm_2d_c8bit_sw_tile_fill_with_colour_keying_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_X_MIRROR_RGB16, 
                __arm_2d_rgb16_sw_tile_fill_with_colour_keying_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_X_MIRROR_RGB32, 
                __arm_2d_rgb32_sw_tile_fill_with_colour_keying_and_x_mirror);


__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_Y_MIRROR_C8BIT, 
                __arm_2d_c8bit_sw_tile_fill_with_colour_keying_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_Y_MIRROR_RGB16, 
                __arm_2d_rgb16_sw_tile_fill_with_colour_keying_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_Y_MIRROR_RGB32, 
                __arm_2d_rgb32_sw_tile_fill_with_colour_keying_and_y_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_XY_MIRROR_C8BIT, 
                __arm_2d_c8bit_sw_tile_fill_with_colour_keying_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_XY_MIRROR_RGB16, 
                __arm_2d_rgb16_sw_tile_fill_with_colour_keying_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_XY_MIRROR_RGB32, 
                __arm_2d_rgb32_sw_tile_fill_with_colour_keying_and_xy_mirror);


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
            .bAllowEnforcedColour   = true,
    #endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_C8BIT),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
            .bAllowEnforcedColour   = true,
    #endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_RGB16),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
            .bAllowEnforcedColour   = true,
    #endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_RGB32),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_RGB32),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_ONLY_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
            .bAllowEnforcedColour   = true,
    #endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_ONLY_C8BIT),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_ONLY_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
            .bAllowEnforcedColour   = true,
    #endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_ONLY_RGB16),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_ONLY_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
    #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__ && defined(RTE_Acceleration_Arm_2D_Alpha_Blending)
            .bAllowEnforcedColour   = true,
    #endif
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_ONLY_RGB32),
            .ptFillLike = NULL,
        },
    },
};



const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_X_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_X_MIRROR_C8BIT),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_X_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_X_MIRROR_RGB16),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_X_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_X_MIRROR_RGB32),
            .ptFillLike = NULL,
        },
    },
};



const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_Y_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_Y_MIRROR_C8BIT),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_Y_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_Y_MIRROR_RGB16),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_Y_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_Y_MIRROR_RGB32),
            .ptFillLike = NULL,
        },
    },
};



const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_XY_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_XY_MIRROR_C8BIT),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_XY_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_XY_MIRROR_RGB16),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_XY_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_XY_MIRROR_RGB32),
            .ptFillLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_ONLY_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_ONLY,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_ONLY_C8BIT),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_ONLY_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_ONLY_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_ONLY,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_ONLY_RGB16),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_ONLY_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_ONLY_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_ONLY,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_ONLY_RGB32),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_ONLY_RGB32),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_X_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_X_MIRROR,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_X_MIRROR_C8BIT),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_X_MIRROR_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_X_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_X_MIRROR,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_X_MIRROR_RGB16),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_X_MIRROR_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_X_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_X_MIRROR,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_X_MIRROR_RGB32),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_X_MIRROR_RGB32),
        },
    },
};



const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_Y_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_Y_MIRROR,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_Y_MIRROR_C8BIT),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_Y_MIRROR_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_Y_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_Y_MIRROR,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_Y_MIRROR_RGB16),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_Y_MIRROR_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_Y_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_Y_MIRROR,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_Y_MIRROR_RGB32),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_Y_MIRROR_RGB32),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_XY_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_XY_MIRROR,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_XY_MIRROR_C8BIT),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_XY_MIRROR_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_XY_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_XY_MIRROR,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_XY_MIRROR_RGB16),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_XY_MIRROR_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_XY_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_XY_MIRROR,
        
        .LowLevelIO = {
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_XY_MIRROR_RGB32),
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_XY_MIRROR_RGB32),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_C8BIT),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_RGB16),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_RGB32),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_RGB32),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_ONLY_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_ONLY_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_ONLY_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_ONLY_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_ONLY_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_ONLY_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_ONLY_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_ONLY_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_ONLY_RGB32),
        },
    },
};
  

const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_X_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_X_MIRROR_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_X_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_X_MIRROR_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_X_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_X_MIRROR_RGB32),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_Y_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_Y_MIRROR_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_Y_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_Y_MIRROR_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_Y_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_Y_MIRROR_RGB32),
        },
    },
};



const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_XY_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_XY_MIRROR_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_XY_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_XY_MIRROR_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_XY_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_XY_MIRROR_RGB32),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_ONLY_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_ONLY_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_ONLY_C8BIT),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_ONLY_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_ONLY_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_ONLY_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_ONLY_RGB16),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_ONLY_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_ONLY_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_ONLY_WITH_COLOUR_KEYING,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_ONLY_RGB32),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_ONLY_RGB32),
        },
    },
};
  

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_AND_X_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_X_MIRROR_C8BIT),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_X_MIRROR_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_AND_X_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_X_MIRROR_RGB16),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_X_MIRROR_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_AND_X_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_X_MIRROR_RGB32),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_X_MIRROR_RGB32),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_AND_Y_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_Y_MIRROR_C8BIT),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_Y_MIRROR_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_AND_Y_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_Y_MIRROR_RGB16),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_Y_MIRROR_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_AND_Y_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_Y_MIRROR_RGB32),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_Y_MIRROR_RGB32),
        },
    },
};



const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_AND_XY_MIRROR_C8BIT = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_XY_MIRROR_C8BIT),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_XY_MIRROR_C8BIT),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_AND_XY_MIRROR_RGB16 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB16,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_XY_MIRROR_RGB16),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_XY_MIRROR_RGB16),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_COLOUR_KEYING_AND_XY_MIRROR_RGB32 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB32,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_COLOUR_KEYING_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_AND_XY_MIRROR_RGB32),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_AND_XY_MIRROR_RGB32),
        },
    },
};



#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_warning 174,177,188,68,513,144
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif

