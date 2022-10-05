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
 * Description:  Public header file for list view related services
 *
 * $Date:        05. Oct 2022
 * $Revision:    V.0.7.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_LIST_H__
#define __ARM_2D_HELPER_LIST_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "arm_2d_helper_pfb.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wpadded"
#endif


/*!
 * \addtogroup gHelper 7 Helper Services
 * @{
 */
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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __arm_2d_list_core_t __arm_2d_list_core_t;


typedef struct arm_2d_list_item_t arm_2d_list_item_t;

/*! 
 * \brief runtime parameters passed to on-draw-list-view-item event handler
 */
typedef struct arm_2d_list_core_item_param_t {
    uint8_t     bIsChecked   : 1;       /*!< is this item checked */
    uint8_t     bIsSelected  : 1;       /*!< is this item seleteced */
    uint8_t     chOpacity;              /*!< opacity proposal */
    uint16_t    hwRatio;                /*!< other ratio proposal */
} arm_2d_list_core_item_param_t;

/*!
 * \brief the prototype of On-Drawing-List-View-Item event handler
 * 
 * \param[in] ptThis the current list view item object. 
 * \param[in] ptTile a tile for the virtual screen
 * \param[in] bIsNewFrame a flag indicate the starting of a new frame
 * \param[in] ptParam the runtime paramters
 * \return arm_fsm_rt_t the status of the FSM.  
 */
typedef arm_fsm_rt_t arm_2d_draw_list_core_item_handler_t( 
                                      arm_2d_list_item_t *ptThis,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_core_item_param_t *ptParam);

/*! 
 * \brief alignment 
 */
enum {
    ARM_2D_ALIGN_LEFT               = _BV(0),                                   /*!< align to left */
    ARM_2D_ALIGN_RIGHT              = _BV(1),                                   /*!< align to right */
    ARM_2D_ALIGN_TOP                = _BV(2),                                   /*!< align to top */
    ARM_2D_ALIGN_BOTTOM             = _BV(3),                                   /*!< align to bottom */
    
    ARM_2D_ALIGN_CENTRE             = 0,                                        /*!< align to centre */
    ARM_2D_ALIGN_CENTRE_ALIAS       = ARM_2D_ALIGN_LEFT                         /*!< align to centre */
                                    | ARM_2D_ALIGN_RIGHT
                                    | ARM_2D_ALIGN_TOP
                                    | ARM_2D_ALIGN_BOTTOM,

    ARM_2D_ALIGN_TOP_LEFT           = ARM_2D_ALIGN_TOP                          /*!< align to top left corner */
                                    | ARM_2D_ALIGN_LEFT,
    ARM_2D_ALIGN_TOP_RIGHT          = ARM_2D_ALIGN_TOP                          /*!< align to top right corner */
                                    | ARM_2D_ALIGN_RIGHT,
    ARM_2D_ALIGN_BOTTOM_LEFT        = ARM_2D_ALIGN_BOTTOM                       /*!< align to bottom left corner */
                                    | ARM_2D_ALIGN_LEFT,
    ARM_2D_ALIGN_BOTTOM_RIGHT       = ARM_2D_ALIGN_BOTTOM                       /*!< align to bottom right corner */
                                    | ARM_2D_ALIGN_RIGHT,
};

/*! 
 * \brief the list view item class
 */
struct arm_2d_list_item_t {

ARM_PROTECTED(
    arm_2d_list_item_t                         *ptNext;                    /*!< list item pointer */
    uint16_t hwID;                                                              /*!< the ID used by the list iterator */
    uint16_t                                        : 16;                       /*!< reserved */
)
    __arm_2d_list_core_t                            *ptListView;                /*!< the parent list view */
    union {
        uint16_t                                    hwAttribute;                /*!< 16bit attribute value */
        struct {
            uint16_t                                bIsEnabled  : 1;            /*!< whether this item is enabled or not */
            uint16_t                                bIsVisible  : 1;            /*!< visibility */
            uint16_t                                            : 2;            /*!< reserved */
            uint16_t                                u4Alignment : 4;            /*!< alignment: see ARM_2D_ALIGN_xxxx */
            uint16_t                                            : 8;            /*!< reserved */
        };
    };

    struct {
        int8_t chPrevious;                                                      /*!< padding between this item and the previous one */
        int8_t chNext;                                                          /*!< padding between this item and the next one */
    } Padding;

    struct {
        uint8_t chLeft;                                                         /*!< left margin */
        uint8_t chRight;                                                        /*!< right margin */
        uint8_t chTop;                                                          /*!< top margin */
        uint8_t chBottom;                                                       /*!< bottom margin */
    } Margin;
    arm_2d_size_t                                   tSize;                      /*!< the size of the item */

    uintptr_t                                       pTarget;                    /*!< user specified object */
    arm_2d_draw_list_core_item_handler_t           *fnOnDrawItem;               /*!< on-draw-list-view-item event handler */
};


/*!
 * \brief intructions for how to move the list view item iterator
 * \note For internal use only
 */
typedef enum {
    __ARM_2D_LIST_VIEW_GET_ITEM_WITH_ID_WITHOUT_MOVE_POINTER = 0,
    __ARM_2D_LIST_VIEW_GET_ITEM_AND_MOVE_POINTER,
    __ARM_2D_LIST_VIEW_GET_PREVIOUS,
    __ARM_2D_LIST_VIEW_GET_NEXT,

    __ARM_2D_LIST_VIEW_GET_FIRST_ITEM_WITHOUT_MOVE_POINTER,
    __ARM_2D_LIST_VIEW_GET_FIRST_ITEM,
    __ARM_2D_LIST_VIEW_GET_CURRENT,
    __ARM_2D_LIST_VIEW_GET_LAST_ITEM_WITHOUT_MOVE_POINTER,
    __ARM_2D_LIST_VIEW_GET_LAST_ITEM,
} arm_2d_list_iterator_dir_t;

/*!
 * \brief the list view interator prototype
 *
 * \param[in] ptThis the target list view object
 * \param[in] tDirection the direction for fetching a list item.
 * \param[in] hwID the ID of the target item
 * \return arm_2d_list_item_t* a list view item
 */
typedef arm_2d_list_item_t *__arm_2d_list_core_item_iterator(
                                        __arm_2d_list_core_t *ptThis,
                                        arm_2d_list_iterator_dir_t tDirection,
                                        uint_fast16_t hwID
                                    );

/*!
 *  \brief the target working area for one list view item
 */
typedef struct __arm_2d_list_core_work_area_t {
    arm_2d_list_item_t        *ptItem;                                     /*!< the target item */
    arm_2d_region_t                 tRegion;                                    /*!< the target region on the list */
    arm_2d_list_core_item_param_t   tParam;                                     /*!< paramters for the target item */
} __arm_2d_list_core_work_area_t;


/*!
 * \brief the list region calculaor prototype
 * \param[in] ptThis the target list view object
 * \param[in] iOffset the offset in the list view
 * \return __arm_2d_list_core_target_area_t* the working area for a target list view item
 */
typedef
__arm_2d_list_core_work_area_t *__arm_2d_list_core_region_calculator_t(
                                __arm_2d_list_core_t *ptThis,
                                __arm_2d_list_core_item_iterator *fnIterator,
                                int32_t nOffset
                            );

/*!
 *  \brief list view configuration structure
 */
typedef struct __arm_2d_list_core_cfg_t {
    arm_2d_size_t tListSize;                                                    /*!< the size of the list */
    __arm_2d_list_core_item_iterator        *fnIterator;                        /*!< the item iterator */
    __arm_2d_list_core_region_calculator_t  *fnCalculator;                      /*!< the region calculator */
    arm_2d_draw_list_core_item_handler_t    *fnOnDrawItemBackground;            /*!< the On-Draw-List-View-Item-Background event handler */
    arm_2d_helper_draw_handler_t            *fnOnDrawListViewBackground;        /*!< the On-Draw-List-View-Background event handler */
    arm_2d_helper_draw_handler_t            *fnOnDrawListViewCover;             /*!< the On-Draw-List-View-Cover event handler */
    uint16_t hwSwitchingPeriodInMs;                                             /*!< A constant period (in ms) for switching item, zero means using default value */
    uint16_t hwItemCount;                                                       /*!< the total number of items, 0 means update later */
    int32_t nTotalLength;                                                       /*!< the total lenght of the list, 0 means update later */
} __arm_2d_list_core_cfg_t;

/*!
 * \brief the list view class
 */
struct __arm_2d_list_core_t {

ARM_PROTECTED(
    __arm_2d_list_core_cfg_t            tCFG;                               /*!< list view configuration */
)

    struct {
    
        ARM_PROTECTED(
            arm_2d_tile_t                   tileTarget;                         /*!< the target draw area */
            arm_2d_tile_t                   tileList;                           /*!< the target tile for the list */
            __arm_2d_list_core_work_area_t  tWorkingArea;                       /*!< the working area */
        )

        ARM_PRIVATE(
            arm_2d_tile_t                   tileItem;                           /*!< the target tile for list items */

            uint16_t                        hwSelection;                        /*!< item selection */
            int32_t                         nPeriod;                            /*!< time to run target distance */
            uint64_t                        lTimestamp;                         /*!< timestamp used by animation */
            int32_t                         nOffset;                            /*!< list offset */
            int32_t                         nStartOffset;                       /*!< the start offset */
            int32_t                         nTargetOffset;                      /*!< the target list offset */
            uint8_t                         chState;                            /*!< state used by list view task */
        )

    } Runtime;                                                                  /*!< list runtime */

ARM_PROTECTED(
    union {
        struct {
            uint8_t chState;
            bool bListHeightChanged;
            int16_t iStartOffset;
            int32_t nOffset;
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
 *  \brief a list view calculator for vertical lists, which puts selected item
 *         in the centre of the target list
 */
extern 
__arm_2d_list_core_region_calculator_t 
    ARM_2D_LIST_VIEW_CALCULATOR_MIDDLE_ALIGNED_VERTICAL;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief initialize a target list view object
 * \param[in] ptThis the target list view object
 * \param[in] ptCFG the user specified configuration
 * \return arm_2d_err_t the operation result
 */
extern
ARM_NONNULL(1,2)
arm_2d_err_t __arm_2d_list_core_init(   __arm_2d_list_core_t *ptThis,
                                        __arm_2d_list_core_cfg_t *ptCFG);

/*!
 * \brief show a given list view
 * \param[in] ptThis the target list view object
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

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
