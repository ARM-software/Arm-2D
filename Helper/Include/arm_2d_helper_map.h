/*
 * Copyright (C) 2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        #include "arm_2d_helper_map.h"
 * Description:  Public header file for the map service
 *
 * $Date:        01. August 2023
 * $Revision:    V.0.5.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_MAP_H__
#define __ARM_2D_HELPER_MAP_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d.h"
#include "__arm_2d_helper_common.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

/*!
 * \addtogroup gHelper 7 Helper Services
 * @{
 */

/* OOC header, please DO NOT modify  */
#ifdef __ARM_2D_HELPER_MAP_IMPLEMENT__
#   undef   __ARM_2D_HELPER_MAP_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__ARM_2D_HELPER_MAP_INHERIT__)
#   undef __ARM_2D_HELPER_MAP_INHERIT__
#   define  __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct arm_2d_helper_map_t arm_2d_helper_map_t;

/*!
 * \brief the cell for storing user defined content.
 */
typedef struct arm_2d_helper_map_cell_t {
    uintptr_t pUserObj;
} arm_2d_helper_map_cell_t;

/*!
 * \brief the structure for coordinate 
 */
typedef struct arm_2d_helper_map_coordinate_t{
    int64_t lX;
    int64_t lY;
} arm_2d_helper_map_coordinate_t;

typedef struct arm_2d_helper_map_grid_t {
    uint16_t hwColumn;
    uint16_t hwRow;
} arm_2d_helper_map_grid_t;

/*!
 * \brief the prototype for on-cell-loading event handler
 */
typedef arm_2d_helper_map_cell_t * arm_2d_cell_loader(
                                    arm_2d_helper_map_t *ptHelper,
                                    void *pObj,
                                    arm_2d_helper_map_coordinate_t *ptOffset);

/*!
 * \brief the on-cell-loading event
 */
typedef struct arm_2d_cell_load_evt_t {
    arm_2d_cell_loader *fnHandler;
    void *pTarget;   
} arm_2d_cell_load_evt_t;

/*!
 * \brief the prototype for on-cell-unload event handler
 */
typedef arm_2d_helper_map_cell_t * arm_2d_cell_unloader(
                                    arm_2d_helper_map_t *ptHelper,
                                    void *pObj,
                                    arm_2d_helper_map_coordinate_t *ptOffset,
                                    arm_2d_helper_map_cell_t *ptCell);

/*!
 * \brief the on-cell-unloading event
 */
typedef struct arm_2d_cell_unload_evt_t {
    arm_2d_cell_unloader *fnHandler;
    void *pTarget;
} arm_2d_cell_unload_evt_t;

/*!
 * \brief useful params for drawing each cell
 */
typedef struct arm_2d_helper_map_cell_param_t {
    arm_2d_tile_t                   *ptTile;                    //!< the tile for drawing
    arm_2d_helper_map_coordinate_t  *ptOffset;                  //!< the coordinate of the cell
    arm_2d_location_t               tAwayFromBoxCentre;         //!< the offset from the centre of the map window
    arm_2d_size_t                   tMapSize;                   //!< the size of the map window
    q15_t                           q15DistanceRatio;           //!< the ratio of the distance from the centre of the map window
    uint8_t                         chOpacity;                  //!< the map opacity used as a reference
    uint8_t                         : 8;                        //!< padding 
} arm_2d_helper_map_cell_param_t;

/*!
 * \brief the prototype for on-cell-draw event handler
 */
typedef arm_2d_helper_map_cell_t * arm_2d_cell_drawer(
                                    arm_2d_helper_map_t *ptHelper,
                                    void *pObj,
                                    arm_2d_helper_map_cell_t *ptCell,
                                    arm_2d_helper_map_cell_param_t *ptParam);

/*!
 * \brief the on-cell-draw event
 */
typedef struct arm_2d_cell_draw_evt_t {
    arm_2d_cell_drawer *fnHandler;
    void *pTarget;
} arm_2d_cell_draw_evt_t;


/*!
 * \brief the configuration for the map service
 */
typedef struct arm_2d_helper_map_cfg_t {
    struct {
        arm_2d_helper_map_grid_t tGrid;
        arm_2d_size_t CellSize;
    } VisibleWindow;

    /* the number of cells must be (hwColumn + 1) * (hwRow + 1) */
    arm_2d_helper_map_cell_t *ptCells;
    uint32_t wCellCount;
    
    struct {
        arm_2d_cell_load_evt_t      evtOnCellLoading;       //!< cell loading event
        arm_2d_cell_draw_evt_t      evtOnCellDrawing;       //!< cell drawing event
        arm_2d_cell_unload_evt_t    evtOnCellUnloading;     //!< cell unloading event
    } Dependency;
} arm_2d_helper_map_cfg_t;


/*!
 * \brief the class of the helper service for loading big map
 */
struct arm_2d_helper_map_t {

ARM_PRIVATE(
    arm_2d_helper_map_cfg_t tCFG;

    arm_2d_tile_t tMap;
    arm_2d_helper_map_coordinate_t tCoordinate;
    
    struct {
        arm_2d_helper_map_grid_t tStart;
        arm_2d_helper_map_grid_t tCurrent;
    } tCell;
)

} ;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief initialize a map helper service 
 * \param[in] ptThis the target map helper object
 * \param[in] ptCFG the configuration
 * \retval NULL Error happened during initialization
 * \retval !NULL the initialized helper service object
 */
extern
ARM_NONNULL(1,2)
arm_2d_helper_map_t *__arm_2d_helper_map_init(  arm_2d_helper_map_t *ptThis, 
                                                arm_2d_helper_map_cfg_t *ptCFG);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
