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
 * Title:        #include "arm_2d_helper_list.h"
 * Description:  Public header file for list core related services
 *
 * $Date:        5. July 2024
 * $Revision:    V.1.1.7
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_LIST_H__
#define __ARM_2D_HELPER_LIST_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "__arm_2d_helper_common.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/*============================ MACROS ========================================*/

/* OOC header, please DO NOT modify  */
#ifdef __ARM_2D_HELPER_LIST_VIEW_IMPLEMENT__
#   undef   __ARM_2D_HELPER_LIST_VIEW_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__ARM_2D_HELPER_LIST_VIEW_INHERIT__)
#   undef __ARM_2D_HELPER_LIST_VIEW_INHERIT__
#   define  __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"

/*! 
 *  \addtogroup Deprecated
 *  @{
 */
#define ARM_2D_LIST_VIEW_CALCULATOR_MIDDLE_ALIGNED_VERTICAL                     \
            ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_VERTICAL

#define ARM_2D_LIST_VIEW_CALCULATOR_MIDDLE_ALIGNED_HORIZONTAL                   \
            ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_HORIZONTAL

/*! @} */

/*!
 * \addtogroup gHelper 8 Helper Services
 * @{
 */
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __arm_2d_list_core_t __arm_2d_list_core_t;


typedef struct arm_2d_list_item_t arm_2d_list_item_t;

/*! 
 * \brief runtime parameters passed to on-draw-list-item event handler
 */
typedef struct arm_2d_list_item_param_t {
    uint8_t     bIsChecked   : 1;       /*!< is this item checked */
    uint8_t     bIsSelected  : 1;       /*!< is this item seleteced */
    uint8_t     chOpacity;              /*!< opacity proposal */
    uint16_t    hwRatio;                /*!< other ratio proposal */
} arm_2d_list_item_param_t;

/*!
 * \brief the prototype of On-Drawing-List-Item event handler
 * 
 * \param[in] ptThis the current list core item object. 
 * \param[in] ptTile a tile for the virtual screen
 * \param[in] bIsNewFrame a flag indicate the starting of a new frame
 * \param[in] ptParam the runtime paramters
 * \return arm_fsm_rt_t the status of the FSM.  
 */
typedef arm_fsm_rt_t arm_2d_draw_list_item_handler_t( 
                                      arm_2d_list_item_t *ptThis,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam);

/*! 
 * \brief the list core item class
 */
struct arm_2d_list_item_t {

ARM_PROTECTED(
    arm_2d_list_item_t                              *ptNext;                    /*!< list item pointer */
)

    uint16_t hwID;                                                              /*!< the ID used by the list iterator */

    union {
        uint16_t                                    hwAttribute;                /*!< 16bit attribute value */
        struct {
            uint16_t                                bIsEnabled  : 1;            /*!< whether this item is enabled or not */
            uint16_t                                bIsVisible  : 1;            /*!< visibility */
            uint16_t                                bIsReadOnly : 1;            /*!< indicate whether this item is readonly or not */
            uint16_t                                            : 1;            /*!< reserved */
            uint16_t                                u4Alignment : 4;            /*!< alignment: see ARM_2D_ALIGN_xxxx */
            uint16_t                                            : 8;            /*!< reserved */
        };
    };
    __arm_2d_list_core_t                            *ptListView;                /*!< the parent list core */
    
    struct {
        int8_t chPrevious;                                                      /*!< padding between this item and the previous one */
        int8_t chNext;                                                          /*!< padding between this item and the next one */
    } Padding;

    arm_2d_margin_t Margin;
    arm_2d_size_t                                   tSize;                      /*!< the size of the item */

    uintptr_t                                       pTarget;                    /*!< user specified object */
    arm_2d_draw_list_item_handler_t                *fnOnDrawItem;               /*!< on-draw-list-view-item event handler */
};


/*!
 * \brief intructions for how to move the list core item iterator
 * \note For internal use only
 */
typedef enum {
    __ARM_2D_LIST_GET_ITEM_WITH_ID_WITHOUT_MOVE_POINTER = 0,
    __ARM_2D_LIST_GET_ITEM_AND_MOVE_POINTER,
    __ARM_2D_LIST_GET_PREVIOUS,
    __ARM_2D_LIST_GET_NEXT,

    __ARM_2D_LIST_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER,
    __ARM_2D_LIST_GET_FIRST_ITEM,
    __ARM_2D_LIST_GET_CURRENT,
    __ARM_2D_LIST_GET_LAST_ITEM_WITHOUT_MOVE_POINTER,
    __ARM_2D_LIST_GET_LAST_ITEM,
} arm_2d_list_iterator_dir_t;

/*!
 * \brief the list core interator prototype
 *
 * \param[in] ptThis the target list core object
 * \param[in] tDirection the direction for fetching a list item.
 * \param[in] hwID the ID of the target item
 * \return arm_2d_list_item_t* a list item
 */
typedef arm_2d_list_item_t *__arm_2d_list_item_iterator(
                                        __arm_2d_list_core_t *ptThis,
                                        arm_2d_list_iterator_dir_t tDirection,
                                        uint_fast16_t hwID
                                    );

typedef enum {
    ARM_2D_LIST_VERTICAL,
    ARM_2D_LIST_HORIZONTAL,
} arm_2d_list_dir_t;

/*!
 *  \brief the target working area for one list core item
 */
typedef struct __arm_2d_list_work_area_t {
    arm_2d_list_item_t         *ptItem;                                         /*!< the target item */
    arm_2d_region_t             tRegion;                                        /*!< the target region on the list */
    arm_2d_list_item_param_t    tParam;                                         /*!< paramters for the target item */
    arm_2d_list_dir_t           tDirection;                                     /*!< list direction (only region calculator knows ) */
} __arm_2d_list_work_area_t;


/*!
 * \brief the list region calculaor prototype
 * \param[in] ptThis the target list core object
 * \param[in] iOffset the offset in the list core
 * \return __arm_2d_list_core_target_area_t* the working area for a target list core item
 */
typedef
__arm_2d_list_work_area_t *__arm_2d_list_region_calculator_t(
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_item_iterator *fnIterator,
                                int32_t nOffset
                            );

/*!
 *  \brief list core configuration structure
 */
typedef struct __arm_2d_list_core_cfg_t {
    arm_2d_size_t tListSize;                                                    /*!< the size of the list */
    __arm_2d_list_item_iterator             *fnIterator;                        /*!< the item iterator */
    __arm_2d_list_region_calculator_t       *fnCalculator;                      /*!< the region calculator */
    arm_2d_draw_list_item_handler_t         *fnOnDrawListItemBackground;        /*!< the On-Draw-List-Item-Background event handler */
    arm_2d_helper_draw_handler_t            *fnOnDrawListBackground;            /*!< the On-Draw-List-Background event handler */
    arm_2d_helper_draw_handler_t            *fnOnDrawListCover;                 /*!< the On-Draw-List-Cover event handler */
    int32_t nTotalLength;                                                       /*!< the total length of the list in pixel, 0 means update later */
    arm_2d_list_item_t                      *ptItems;                           /*!< an optional pointer for items (array/list) */
    uint16_t hwItemSizeInByte;                                                  /*!< the size of the item (in byte) */
    uint16_t hwItemCount;                                                       /*!< the total number of items, 0 means update later */
    uint16_t hwSwitchingPeriodInMs;                                             /*!< A constant period (in ms) for switching item, zero means using default value */

    uint16_t bDisableRingMode               : 1;                                /*!< whether disable ring mode */
    uint16_t bDisableStatusCheck            : 1;                                /*!< whether disable the item status */

} __arm_2d_list_core_cfg_t;

/*!
 * \brief the list core class
 */
struct __arm_2d_list_core_t {

ARM_PROTECTED(
    __arm_2d_list_core_cfg_t            tCFG;                                   /*!< list core configuration */
)

    struct {
    
        ARM_PROTECTED(
            arm_2d_tile_t                   tileTarget;                         /*!< the target draw area */
            arm_2d_tile_t                   tileList;                           /*!< the target tile for the list */
            __arm_2d_list_work_area_t       tWorkingArea;                       /*!< the working area */
            uint8_t                         bIsRegCalInit;                      /*!< indicate whether the region calcluator is initialized or not */
            union {
                struct {
                    uint16_t hwIndex;                                           /*!< array iterator index */
                } Array;                                                        /*!< array iterator */
                /* put other iterator structure here 
                 * ...
                 */
            } Iterator;                                                         /*!< iterator control block */
        )

        ARM_PRIVATE(
            arm_2d_tile_t                   tileItem;                           /*!< the target tile for list items */
            int64_t                         lPeriod;                            /*!< time to run target distance */
            int64_t                         lTimestamp;                         /*!< timestamp used by animation */
            int32_t                         nOffset;                            /*!< list offset */
            int32_t                         nStartOffset;                       /*!< the start offset */
            int32_t                         nTargetOffset;                      /*!< the target list offset */
            uint16_t                        hwSelection;                        /*!< item selection */
            uint8_t                         chState;                            /*!< state used by list core task */
            uint8_t                         bIsMoving   : 1;                    /*!< a flag to indicate whether the list is moving */
            uint8_t                         bNeedRedraw : 1;                    /*!< a flag to indicate whether a redraw is requested, this is a sticky flag */
            uint8_t                                     : 6;                    /*!< reserved */
            struct {
                int16_t iSteps;                                                 /*!< steps to move */
                int32_t nFinishInMs;                                            /*!< finish in ms */
            } MoveReq;
        )

    } Runtime;                                                                  /*!< list runtime */

ARM_PROTECTED(

    uint8_t chState;
    bool bListSizeChanged;
    int16_t iStartOffset;
    int32_t nOffset;

    union {
        struct {

            int16_t iTopVisiableOffset;
            uint16_t hwTopVisibleItemID;
            
            int16_t iBottomVisibleOffset;
            uint16_t hwBottomVisibleItemID;
        } CalMidAligned;
    };
)

};


/*============================ GLOBAL VARIABLES ==============================*/

/*!
 *  \brief a list calculator for vertical lists, which puts selected item
 *         in the centre of the target list
 */
extern 
__arm_2d_list_region_calculator_t 
    ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_VERTICAL;

/*!
 *  \brief a list calculator for horizontal lists, which puts selected item
 *         in the centre of the target list
 */
extern
__arm_2d_list_region_calculator_t
    ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_HORIZONTAL;
    
/*!
 *  \brief a list calculator for vertical lists, which puts selected item
 *         in the centre of the target list, item size is fixed and no status 
 *         checking (i.e. visible or enabled).
 */
extern
__arm_2d_list_region_calculator_t
ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_FIXED_SIZED_ITEM_NO_STATUS_CHECK_VERTICAL;

/*!
 *  \brief a list calculator for horizontal lists, which puts selected item
 *         in the centre of the target list, item size is fixed and no status 
 *         checking (i.e. visible or enabled).
 */
extern
__arm_2d_list_region_calculator_t
ARM_2D_LIST_CALCULATOR_MIDDLE_ALIGNED_FIXED_SIZED_ITEM_NO_STATUS_CHECK_HORIZONTAL;

/*!
 *  \brief a list iterator for the list that stores items in an array
 */
extern __arm_2d_list_item_iterator ARM_2D_LIST_ITERATOR_ARRAY;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief initialize a target list core object
 * \param[in] ptThis the target list core object
 * \param[in] ptCFG the user specified configuration
 * \return arm_2d_err_t the operation result
 */
extern
ARM_NONNULL(1,2)
arm_2d_err_t __arm_2d_list_core_init(   __arm_2d_list_core_t *ptThis,
                                        __arm_2d_list_core_cfg_t *ptCFG);

extern
ARM_NONNULL(1)
/*!
 * \brief get the size of the target list
 * 
 * \param[in] ptThis the target list core object
 * \return arm_2d_size_t the list size
 */
arm_2d_size_t __arm_2d_list_core_get_list_size(__arm_2d_list_core_t *ptThis);

/*!
 * \brief show a given list core
 * \param[in] ptThis the target list core object
 * \param[in] ptTarget the target framebuffer
 * \param[in] ptRegion the target region
 * \param[in] bIsNewFrame a flag to indicate whether current iteration is the 
 *            first one of a new frame.
 * \return arm_fsm_rt_t the fsm status
 */
extern
ARM_NONNULL(1,2)
arm_fsm_rt_t __arm_2d_list_core_show(   __arm_2d_list_core_t *ptThis,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        bool bIsNewFrame);

/*!
 * \brief request to move selection with specified steps
 * \param[in] ptThis the target list core object
 * \param[in] iSteps number of steps, here negative value means move to previous
 *            items and positive value means move to next items
 * \note for current stage, ring mode is permanently enabled.
 * \param[in] nFinishInMs 
 *              - (nFinishInMs > 0) the list should turn to those
 *                  steps in specified time (ms)
 *              - (nFinishInMs < 0) use the configuration passed at the  
 *                  initialisation stage.
 *              - (nFinishInMs == 0) do not change current configuration
 */
extern
ARM_NONNULL(1)
void __arm_2d_list_core_move_request(   __arm_2d_list_core_t *ptThis, 
                                        int16_t iSteps,
                                        int32_t nFinishInMs);

/*!
 * \brief move selection with specified pixel offset
 * \param[in] ptThis the target list core object
 * \param[in] iOffset number of pixels, here negative value means move to previous
 *            items and positive value means move to next items
 * \note for current stage, ring mode is permanently enabled.
 *
 * \return arm_2d_err_t the operation result
 */
extern
ARM_NONNULL(1)
void __arm_2d_list_core_move_offset(__arm_2d_list_core_t *ptThis, 
                                    int16_t iOffset);

/*!
 * \brief get the currently selected item id
 * 
 * \param[in] ptThis the target list core object
 * \return uint16_t the item ID
 */
extern
ARM_NONNULL(1)
uint16_t __arm_2d_list_core_get_selected_item_id(__arm_2d_list_core_t *ptThis);

/*!
 * \brief get the currently selected item
 * 
 * \param[in] ptThis the target list core object
 * \return arm_2d_list_item_t* the selected item
 */
extern
ARM_NONNULL(1)
arm_2d_list_item_t *__arm_2d_list_core_get_selected_item(__arm_2d_list_core_t *ptThis);

/*!
 * \brief check whether the list need a redraw
 * 
 * \param[in] ptThis the target list core object
 * \param[in] bAutoreset a flag to indicate whether we have to clear the redraw flag 
 *                   automatically when calling this function.
 * \return true the list need a redraw
 * \return false the list has no change.
 */
extern
ARM_NONNULL(1)
bool __arm_2d_list_core_need_redraw(__arm_2d_list_core_t *ptThis, bool bAutoreset);

/*!
 * \brief check whether the list is moving its items
 * 
 * \param[in] ptThis the target list core object
 * \return true the list is moving
 * \return false the list has no change.
 */
extern
ARM_NONNULL(1)
bool __arm_2d_list_core_is_list_moving(__arm_2d_list_core_t *ptThis);

extern
ARM_NONNULL(1,2)
/*!
 * \brief get list item with a specified direction instruction
 * 
 * \param[in] ptThis the target list core object
 * \param[in] fnIterator a list iterator
 * \param[in] tDirection the direction instruction
 * \param[in] hwID the item ID (when required by the direction instruction)
 * \param[in] bIgnoreStatusCheck whether ignore the status checking
 * \param[in] bForceRingMode whether force to enable ring mode anyway
 * \return arm_2d_list_item_t* 
 */
arm_2d_list_item_t *__arm_2d_list_core_get_item(
                        __arm_2d_list_core_t *ptThis,
                        __arm_2d_list_item_iterator *fnIterator,
                        arm_2d_list_iterator_dir_t tDirection,
                        uint16_t hwID,
                        bool bIgnoreStatusCheck,
                        bool bForceRingMode);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
