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
 * Title:        #include "arm_2d_helper_scene.h"
 * Description:  Public header file for the scene service
 *
 * $Date:        15. March 2024
 * $Revision:    V.1.6.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_SCENE_H__
#define __ARM_2D_HELPER_SCENE_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d_helper_pfb.h"

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


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

/*!
 * \brief register / update the evtOnDrawNavigation event handler. You can use 
 *        this event to draw an ALWAY-TOP navigation bar or title during switching
 *        period.
 *
 * \param[in] __DISP_ADAPTER_PTR the target scene player
 * \param[in] __DRAW_HANDLER the event handler to draw the navigation bar and/or
 *            titles
 * \param[in] __USER_TARGET_PTR the address of an user specified object. If it 
 *            is NULL, ptThis will be used instead.
 * \param[in] ...  an optional dirty region list for the navigation layer. If 
 *            ommited, NULL is used.
 * \note if the optional dirty region list is omitted and the normal scene 
 *       doesn't cover the region of the content in the navigation layer, 
 *       you won't see the content. 
 * \return arm_2d_err_t the operation result
 */
#define arm_2d_scene_player_register_on_draw_navigation_event_handler(          \
                                                        __SCENE_PLAYER_PTR,     \
                                                        __DRAW_HANDLER,         \
                                                        __USER_TARGET_PTR,      \
                                                        ...)                    \
            __arm_2d_scene_player_register_on_draw_navigation_event_handler(    \
                                                        (__SCENE_PLAYER_PTR),   \
                                                        (__DRAW_HANDLER),       \
                                                        (__USER_TARGET_PTR),    \
                                                        (NULL,##__VA_ARGS__))

/*!
 * \brief configure the scene switching mode
 *
 * \param[in] __DISP_ADAPTER_PTR the target scene player
 * \param[in] __SWITCH_MODE a switching mode object
 * \param[in] ... an optional configurations for the switching
 */
#define arm_2d_scene_player_set_switching_mode(__SCENE_PLAYER_PTR,              \
                                               __SWITCH_MODE,                   \
                                               ...)                             \
            __arm_2d_scene_player_set_switching_mode((__SCENE_PLAYER_PTR),      \
                                                     &(__SWITCH_MODE),          \
                                                     (0,##__VA_ARGS__))

/*!
 * \brief register / update the evtBeforeSwitching event handler. You can use 
 *        this event to prepare next scenes.
 *
 * \param[in] ptThis the target scene player
 * \param[in] fnHandler the event handler
 * \param[in] ... optional, the address of an user specified object.
 * \return arm_2d_err_t the operation result
 */
#define arm_2d_scene_player_register_before_switching_event_handler(            \
                                                            __SCENE_PLAYER_PTR, \
                                                            __HANDLER,          \
                                                            ...)                \
            __arm_2d_scene_player_register_before_switching_event_handler(      \
                                                        (__SCENE_PLAYER_PTR),   \
                                                        (__HANDLER),            \
                                                        (NULL,##__VA_ARGS__))


/*============================ TYPES =========================================*/

/*!
 * \brief scene switching mode
 */
typedef enum {

    /* valid switching visual effects begin */
    ARM_2D_SCENE_SWITCH_CFG_NONE           = 0,                                 //!< no switching visual effect
    ARM_2D_SCENE_SWITCH_CFG_USER           = 1,                                 //!< user defined switching visual effect
    ARM_2D_SCENE_SWITCH_CFG_FADE_WHITE     = 2,                                 //!< fade in fade out (white)
    ARM_2D_SCENE_SWITCH_CFG_FADE_BLACK     = 3,                                 //!< fade in fade out (black)
    ARM_2D_SCENE_SWITCH_CFG_SLIDE_LEFT     = 4,                                 //!< slide left
    ARM_2D_SCENE_SWITCH_CFG_SLIDE_RIGHT,                                        //!< slide right
    ARM_2D_SCENE_SWITCH_CFG_SLIDE_UP,                                           //!< slide up
    ARM_2D_SCENE_SWITCH_CFG_SLIDE_DOWN,                                         //!< slide down
    ARM_2D_SCENE_SWITCH_CFG_ERASE_LEFT     = 8,                                 //!< erase to the right
    ARM_2D_SCENE_SWITCH_CFG_ERASE_RIGHT,                                        //!< erase to the left
    ARM_2D_SCENE_SWITCH_CFG_ERASE_UP,                                           //!< erase to the top
    ARM_2D_SCENE_SWITCH_CFG_ERASE_DOWN,                                         //!< erase to the bottom

    /* valid switching visual effects end */
    __ARM_2D_SCENE_SWITCH_CFG_VALID,                                            //!< For internal user only
    
    ARM_2D_SCENE_SWITCH_CFG_IGNORE_OLD_BG          = _BV(8),                    //!< ignore the background of the old scene
    ARM_2D_SCENE_SWITCH_CFG_IGNORE_OLD_SCEBE       = _BV(9),                    //!< ignore the old scene
    ARM_2D_SCENE_SWITCH_CFG_IGNORE_NEW_BG          = _BV(10),                   //!< ignore the background of the new scene
    ARM_2D_SCENE_SWITCH_CFG_IGNORE_NEW_SCEBE       = _BV(11),                   //!< ignore the new scene
    
    ARM_2D_SCENE_SWITCH_CFG_DEFAULT_BG_WHITE       = 0 << 12,                   //!< use white as default background
    ARM_2D_SCENE_SWITCH_CFG_DEFAULT_BG_BLACK       = 1 << 12,                   //!< use black as default background
    ARM_2D_SCENE_SWITCH_CFG_DEFAULT_BG_USER        = 2 << 12,                   //!< use user defined default background

    __ARM_2D_SCENE_SWTICH_CFG_IGNORE_msk           = 0x0F << 8,                 //!< For internal user only
    __ARM_2D_SCENE_SWTICH_CFG_IGNORE_pos           = 8,                         //!< For internal user only
    __ARM_2D_SCENE_SWTICH_CFG_DEFAULT_BG_msk       = 3 << 12,                   //!< For internal user only
    __ARM_2D_SCENE_SWTICH_CFG_DEFAULT_BG_pos       = 12,                        //!< For internal user only

} arm_2d_scene_player_switch_mode_t;



/*!
 * \brief an internal data structure for scene switching
 * 
 * \note Please do not use it.
 */
typedef union __arm_2d_helper_scene_switch_t {

    struct {
        uint8_t chMode;                                                         //!< the switch visual effect
        uint8_t bIgnoreOldSceneBG       : 1;                                    //!< when set, ignore the background of the old scene
        uint8_t bIgnoreOldScene         : 1;                                    //!< when set, ignore the old scene
        uint8_t bIgnoreNewSceneBG       : 1;                                    //!< when set, ignore the background of the new scene
        uint8_t bIgnoreNewScene         : 1;                                    //!< when set, ignore the new scene
        uint8_t u2DefaultBG             : 2;                                    //!< the default background
        uint8_t                         : 2;
    } Feature;
    uint16_t hwSetting;                                                         //!< the setting value

}__arm_2d_helper_scene_switch_t;


/*!
 * \brief scene switching mode descriptor
 */
typedef const struct {
    uint8_t                         chEffects;                                  //!< switching effects
    arm_2d_helper_draw_handler_t    *fnSwitchDrawer;                            //!< switching algorithm
} arm_2d_scene_switch_mode_t;


typedef struct arm_2d_scene_player_t arm_2d_scene_player_t;

/*!
 * \brief a class for describing scenes which are the combination of a
 *        background and a foreground with a dirty-region-list support
 * 
 */
typedef struct arm_2d_scene_t arm_2d_scene_t;
struct arm_2d_scene_t {
    arm_2d_scene_t *ptNext;                                                     //!< next scene
    arm_2d_scene_player_t *ptPlayer;                                            //!< points to the host scene player

    /*! \note Please do NOT use it unless it is necessary */
    arm_2d_helper_draw_handler_t    *fnBackground;

    arm_2d_region_list_item_t       *ptDirtyRegion;                             //!< dirty region list for the scene
    arm_2d_helper_draw_handler_t    *fnScene;                                   //!< the function pointer for the scene
  
    void (*fnOnBGStart)(arm_2d_scene_t *ptThis);                                //!< on-start-drawing-background event handler
    void (*fnOnBGComplete)(arm_2d_scene_t *ptThis);                             //!< on-complete-drawing-background event handler
    void (*fnOnFrameStart)(arm_2d_scene_t *ptThis);                             //!< on-frame-start event handler
    void (*fnOnFrameCPL)(arm_2d_scene_t *ptThis);                               //!< on-frame-complete event handler

    /*!
     * \note We can use this event to initialize/generate the new(next) scene
     */
    void (*fnBeforeSwitchOut)(arm_2d_scene_t *ptThis);                          //!< before-scene-switch-out event handler

    /*!
     * \note We use fnDepose to free the resources
     */
    void (*fnDepose)(arm_2d_scene_t *ptThis);                                   //!< on-scene-depose event handler
    struct {
        uint8_t bOnSwitchingIgnoreBG    : 1;                                    //!< ignore background during switching period
        uint8_t bOnSwitchingIgnoreScene : 1;                                    //!< ignore forground during switching period
    };
};

/*!
 * \brief the scene player event handler
 * 
 * \param[in] pTarget a user attached target address 
 * \param[in] ptPlayer the scene player 
 * \param[in] ptScene the old scene that is to be switched out
 */
typedef void arm_2d_scene_before_scene_switching_handler_t(
                                            void *pTarget,
                                            arm_2d_scene_player_t *ptPlayer,
                                            arm_2d_scene_t *ptScene);

/*!
 * \brief on low level render event 
 */
typedef struct arm_2d_scene_before_scene_switching_evt_t {
    arm_2d_scene_before_scene_switching_handler_t *fnHandler;                   //!< event handler function
    void *pTarget;                                                              //!< user attached target
} arm_2d_scene_before_scene_switching_evt_t;

/*!
 * \brief a class to manage scenes
 * 
 */
struct arm_2d_scene_player_t {
    inherit(arm_2d_helper_pfb_t);                                               //!< inherit from arm_2d_helper_pfb_t

    struct {
        uint32_t wMin;
        uint32_t wMax;
        uint64_t dwTotal;
        uint64_t dwRenderTotal;
        uint32_t wAverage;
        float fCPUUsage;
        uint16_t hwIterations;
        uint16_t hwFrameCounter;
        uint32_t wLCDLatency;
        int64_t lTimestamp;
    } Benchmark;

    ARM_PRIVATE(
        struct {
            arm_2d_scene_t *ptHead;                                             //!< points to the head of the FIFO
            arm_2d_scene_t *ptTail;                                             //!< points to the tail of the FIFO
        } SceneFIFO;                                                            //!< Scene FIFO
        
        struct {
            uint8_t bNextSceneReq           : 1;                                //!< a flag to request switching-to-the next-scene
            uint8_t bSwitchCPL              : 1;                                //!< indication of scene switching completion
            uint8_t bUpdateBG               : 1;                                //!< update the background of the current scene
            uint8_t                         : 1;
            uint8_t bCallOldSceneFrameCPL   : 1;                                //!< call the old scene frame complete event handler
            uint8_t bCallNewSceneFrameCPL   : 1;                                //!< call the new scene frame complete event handler
            uint8_t bCallOldSceneBGCPL      : 1;                                //!< call the old scene Background complete event handler
            uint8_t bCallNewSceneBGCPL      : 1;                                //!< call the new scene Background complete event handler
            
            uint8_t chState;                                                    //!< the state of the FSM used by runtime.
        } Runtime;                                                              //!< scene player runtime
        
        struct {
            arm_2d_scene_switch_mode_t *ptMode;                                 //!< the switching mode
            union {
                uint8_t chState;                                                //!< FSM state
                struct {
                    uint8_t chState;                                            //!< FSM state
                    uint8_t chOpacity;                                          //!< opacity of the cover
                    bool bIsFadeBlack;                                          //!< the colour of the cover
                }Fade;
                struct {
                    uint8_t chState;                                            //!< FSM state
                    arm_2d_tile_t tSceneWindow;                                 //!< scene window
                    arm_2d_tile_t tTemp;                                        //!< a temp tile
                    int16_t iOffset;                                            //!< erase offset
                }Erase;
                struct {
                    uint8_t chState;                                            //!< FSM state
                    arm_2d_tile_t tSceneWindow;                                 //!< scene window
                    int16_t iOffset;                                            //!< slide offset
                }Slide;
            };
            __arm_2d_helper_scene_switch_t tConfig;                             //!< the switching configuration
            uint16_t hwPeriod;                                                  //!< the switching should finish in specified millisecond
            int64_t lTimeStamp;
        }Switch;
        
        struct {
            /*!
             * \note We can use this event to initialize/generate the new(next) scene
             */
            arm_2d_scene_before_scene_switching_evt_t  evtBeforeSwitching;      //!< before-scene-switch-out event handler
        } Events;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_NONE;

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_USER;

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE;

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_FADE_BLACK;

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT;

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT;

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_UP;

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_DOWN;

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_LEFT;

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_RIGHT;

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_UP;

extern
arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_DOWN;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief flush the scene FIFO
 * 
 * \param[in] ptThis the target scene player
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_flush_fifo(arm_2d_scene_player_t *ptThis);

/*!
 * \brief append a set of scenes to a scene player
 *
 * \param[in] ptThis the target scene player
 * \param[in] ptScenes a scene array
 * \param[in] hwCount the number of scenes in the array
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_append_scenes(arm_2d_scene_player_t *ptThis, 
                                            arm_2d_scene_t *ptScenes,
                                            int_fast16_t hwCount);

/*!
 * \brief request updating the background of the current scene
 * \param[in] ptThis the target scene player
 */
ARM_NONNULL(1)
void arm_2d_scene_player_update_scene_background(arm_2d_scene_player_t *ptThis);

/*!
 * \brief request switching to the next scene safely
 * 
 * \param[in] ptThis the target scene player
 * 
 * \note Once received a request, the scene player will only switch to the
 *       next scene at the end of a frame. 
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_switch_to_next_scene(arm_2d_scene_player_t *ptThis);

/*!
 * \brief configure the scene switching mode
 *
 * \param[in] ptThis the target scene player
 * \param[in] ptMode a switching mode object
 * \param[in] hwSettings configurations for the switching
 */
extern
ARM_NONNULL(1)
void __arm_2d_scene_player_set_switching_mode(arm_2d_scene_player_t *ptThis,
                                              arm_2d_scene_switch_mode_t *ptMode,
                                              uint16_t hwSettings);
                                            
/*!
 * \brief read the current scene switching mode
 *
 * \param[in] ptThis the target scene player
 * \return uint16_t the current setting value for the scene switching mode
 */
extern
ARM_NONNULL(1)
uint16_t arm_2d_scene_player_get_switching_cfg(arm_2d_scene_player_t *ptThis);

/*!
 * \brief configure the scene switching period
 *
 * \param[in] ptThis the target scene player
 * \param[in] hwMS period in millisecond
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_set_switching_period(  arm_2d_scene_player_t *ptThis,
                                                uint_fast16_t hwMS);


/*!
 * \brief register / update the evtOnDrawNavigation event handler. You can use 
 *        this event to draw an ALWAY-TOP navigation bar or title during switching
 *        period.
 *
 * \param[in] ptThis the target scene player
 * \param[in] fnHandler the event handler to draw the navigation bar and/or titles
 * \param[in] pTarget the address of an user specified object. If it is NULL, 
 *            ptThis will be used instead.
 * \param[in] ptDirtyRegions a dirty region list for the navigation layer. 
 * \note if ptDirtyRegions is NULL and the normal scene doesn't cover the region
 *       of the content in the navigation layer, you won't see the content. 
 * \return arm_2d_err_t the operation result
 */
extern
ARM_NONNULL(1)
arm_2d_err_t __arm_2d_scene_player_register_on_draw_navigation_event_handler(
                                    arm_2d_scene_player_t *ptThis,
                                    arm_2d_helper_draw_handler_t *fnHandler,
                                    void *pTarget,
                                    arm_2d_region_list_item_t *ptDirtyRegions);

/*!
 * \brief hide the navigation layer
 * \param[in] ptThis an initialised scene player
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_hide_navigation_layer(arm_2d_scene_player_t *ptThis);

/*!
 * \brief show the navigation layer if there is a valid one
 * \param[in] ptThis an initialised scene player
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_show_navigation_layer(arm_2d_scene_player_t *ptThis);

/*!
 * \brief register / update the evtBeforeSwitching event handler. You can use 
 *        this event to prepare next scenes.
 *
 * \param[in] ptThis the target scene player
 * \param[in] fnHandler the event handler
 * \param[in] pTarget the address of an user specified object.
 * \return arm_2d_err_t the operation result
 */
extern
ARM_NONNULL(1)
arm_2d_err_t __arm_2d_scene_player_register_before_switching_event_handler(
                    arm_2d_scene_player_t *ptThis,
                    arm_2d_scene_before_scene_switching_handler_t *fnHandler,
                    void *pTarget
                );

/*!
 * \brief the scene player task function
 * 
 * \param[in] ptThis the target scene player
 * 
 * \note the event sequence of a scene:
 *       1. when fnBackground is valid
 *           - invoke fnOnBGStart when it is valid
 *           - invoke fnBackground
 *           - invoke fnOnBGComplete when it is valid
 *       2. invoke fnOnFrameStart when it is valid
 *       3. invoke fnScene
 *       4. invoke fnOnFrameCPL when it is valid
 *       5. Check bNextSceneReq
 *           - false (0), go back to step 2
 *           - true, invoke fnDepose when it is valid and switch to the next scene
 *          
 */
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_scene_player_task(arm_2d_scene_player_t *ptThis);

/*!
 * \brief append dirty regions to the a specified scene
 * \param[in] ptScene the target scene
 * \param[in] ptItems the dirty regions
 * \param[in] tCount the number of dirty regions
 * \retval true operation is successful
 * \retval false the operation is failed.
 */
extern
ARM_NONNULL(1,2)
bool arm_2d_scene_player_append_dirty_regions(arm_2d_scene_t *ptScene, 
                                              arm_2d_region_list_item_t *ptItems,
                                              size_t tCount);

/*!
 * \brief remove dirty regions from the a specified scene
 * \param[in] ptScene the target scene
 * \param[in] ptItems the dirty regions
 * \param[in] tCount the number of dirty regions
 * \retval true operation is successful
 * \retval false the operation is failed.
 */
extern
ARM_NONNULL(1,2)
bool arm_2d_scene_player_remove_dirty_regions(arm_2d_scene_t *ptScene, 
                                              arm_2d_region_list_item_t *ptItems,
                                              size_t tCount);

/*!
 * \brief get the current scene of a given scene player
 * \param[in] ptThis the target scene player
 * \return arm_2d_scene_t * the current scene
 */
extern
ARM_NONNULL(1)
arm_2d_scene_t * 
arm_2d_scene_player_get_the_current_scene(arm_2d_scene_player_t *ptThis);


/*-----------------------------------------------------------------------------*
 * Dynamic Dirty Region Helper Service                                         *
 *-----------------------------------------------------------------------------*/

/*!
 * \brief initialize a user dynamic dirty region
 * 
 * \param[in] ptThis the target region list item. If it is NULL, this function will
 *               allocate an object from the heap
 * \param[in] ptScene the target scene.
 * \return arm_2d_region_list_item_t* the target region list item
 */
extern
arm_2d_region_list_item_t *arm_2d_scene_player_dynamic_dirty_region_init(
                                            arm_2d_region_list_item_t *ptThis,
                                            arm_2d_scene_t *ptScene);

/*!
 * \brief depose a given user dynamic dirty region
 * 
 * \param[in] ptThis the target region list item.
 * \param[in] ptScene the target scene.
 */
extern
ARM_NONNULL(1)
void arm_2d_scene_player_dynamic_dirty_region_depose(
                                            arm_2d_region_list_item_t *ptThis,
                                            arm_2d_scene_t *ptScene);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
