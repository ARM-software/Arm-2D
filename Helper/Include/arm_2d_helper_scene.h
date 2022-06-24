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

typedef struct arm_2d_scene_t arm_2d_scene_t;

struct arm_2d_scene_t {
    arm_2d_scene_t *ptNext;
    arm_2d_region_list_item_t       *ptDirtyRegion;
    arm_2d_helper_draw_handler_t    *fnBackground;
    arm_2d_helper_draw_handler_t    *fnScene;
    void (*fnOnBGStart)(arm_2d_scene_t *ptThis);
    void (*fnOnBGComplete)(arm_2d_scene_t *ptThis);
    void (*fnOnFrameStart)(arm_2d_scene_t *ptThis);
    void (*fnOnFrameCPL)(arm_2d_scene_t *ptThis);
    void (*fnDepose)(arm_2d_scene_t *ptThis);
};

typedef struct arm_2d_scene_player_t {
    inherit(arm_2d_helper_pfb_t);
    
    ARM_PRIVATE(
        struct {
            arm_2d_scene_t *ptHead;
            arm_2d_scene_t *ptTail;
        } SceneFIFO;
        
        struct {
            uint8_t bNextSceneReq   : 1;
            uint8_t                 : 7;
            uint8_t chState;
        } Runtime;
    )
} arm_2d_scene_player_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void arm_2d_user_scene_player_set_scenes(   arm_2d_scene_player_t *ptThis,
                                            arm_2d_scene_t *ptSceneList);

extern
ARM_NONNULL(1)
void arm_2d_user_scene_player_append_scenes(arm_2d_scene_player_t *ptThis, 
                                            arm_2d_scene_t *ptSceneList);

extern
ARM_NONNULL(1)
void arm_2d_user_scene_player_next_scene(arm_2d_scene_player_t *ptThis);

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
