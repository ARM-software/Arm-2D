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
 * $Date:        22. June 2022
 * $Revision:    V.1.0.0
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
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*!
 * \brief a class for describing scenes which are the combination of a
 *        background and a foreground with a dirty-region-list support
 * 
 */
typedef struct arm_2d_scene_t arm_2d_scene_t;
struct arm_2d_scene_t {
    arm_2d_scene_t *ptNext;                                                     //!< next scene
    arm_2d_region_list_item_t       *ptDirtyRegion;                             //!< dirty region list for the foreground 
    arm_2d_helper_draw_handler_t    *fnBackground;                              //!< the function pointer for the background 
    arm_2d_helper_draw_handler_t    *fnScene;                                   //!< the function pointer for the foreground
    void (*fnOnBGStart)(arm_2d_scene_t *ptThis);                                //!< on-start-drawing-background event handler
    void (*fnOnBGComplete)(arm_2d_scene_t *ptThis);                             //!< on-complete-drawing-background event handler
    void (*fnOnFrameStart)(arm_2d_scene_t *ptThis);                             //!< on-frame-start event handler
    void (*fnOnFrameCPL)(arm_2d_scene_t *ptThis);                               //!< on-frame-complete event handler
    void (*fnDepose)(arm_2d_scene_t *ptThis);                                   //!< on-scene-depose event handler 
    /*!
     * \note We use fnDepose to free the resources
     */
};

/*!
 * \brief a class to manage scenes
 * 
 */
typedef struct arm_2d_scene_player_t {
    inherit(arm_2d_helper_pfb_t);                                               //!< inherit from arm_2d_helper_pfb_t
    
    ARM_PRIVATE(
        struct {
            arm_2d_scene_t *ptHead;                                             //!< points to the head of the FIFO
            arm_2d_scene_t *ptTail;                                             //!< points to the tail of the FIFO
        } SceneFIFO;                                                            //!< Scene FIFO
        
        struct {
            uint8_t bNextSceneReq   : 1;                                        //!< a flag to request switching-to-the next-scene
            uint8_t                 : 7;
            uint8_t chState;                                                    //!< the state of the FSM used by runtime.
        } Runtime;                                                              //!< scene player runtime
    )
} arm_2d_scene_player_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief register a list of scenes to a scene player
 * 
 * \param[in] ptThis the target scene player
 * \param[in] ptSceneList the first node of a scene list
 * 
 * \note When you register new scene array to a scene player, existing scenes 
 *       (if any) will be deposed one by one by the scene player first. This is
 *       different from appending scene(s), as this operation is replacing the 
 *       old (existing) scene with new ones.
 */
extern
ARM_NONNULL(1)
void arm_2d_user_scene_player_set_scenes(   arm_2d_scene_player_t *ptThis,
                                            arm_2d_scene_t *ptSceneList);

/*!
 * \brief append a list of scenes to a scene player
 *
 * \param[in] ptThis the target scene player
 * \param[in] ptSceneList the first node of a scene list
 */
extern
ARM_NONNULL(1)
void arm_2d_user_scene_player_append_scenes(arm_2d_scene_player_t *ptThis, 
                                            arm_2d_scene_t *ptSceneList);

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
void arm_2d_user_scene_player_next_scene(arm_2d_scene_player_t *ptThis);

/*!
 * \brief the scene player task function
 * 
 * \param[in] ptThis the target scene player
 * 
 * \note the event sequence of a scene:
 *       1. when fnBackground is not NULL
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
arm_fsm_rt_t arm_2d_user_scene_player_task(arm_2d_scene_player_t *ptThis);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
