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
 * Title:        #include "arm_2d_helper_scene.c"
 * Description:  Public header file for the scene service
 *
 * $Date:        22. June 2022
 * $Revision:    V.1.0.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

#define __ARM_2D_IMPL__
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "arm_2d_helper.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

/*============================ MACROS ========================================*/
#undef this
#define this        (*ptThis)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __scene_player_depose_all_scene(arm_2d_scene_player_t *ptThis)
{
    arm_2d_scene_t *ptScene = NULL;

    do {
        ARM_LIST_QUEUE_DEQUEUE(this.SceneFIFO.ptHead,
                               this.SceneFIFO.ptTail,
                               ptScene);
        if (NULL == ptScene) {
            break;
        }
        if (NULL != ptScene->fnDepose) {
            ptScene->fnDepose(ptScene);
        }
    } while(true);
}

ARM_NONNULL(1)
void arm_2d_user_scene_player_set_scenes(   arm_2d_scene_player_t *ptThis,
                                            arm_2d_scene_t *ptSceneList)
{
    assert(NULL != ptThis);

    __scene_player_depose_all_scene(ptThis);

    this.SceneFIFO.ptHead = ptSceneList;
    this.SceneFIFO.ptTail = ptSceneList;
    
    if (NULL == ptSceneList) {
        return ;
    }

    while(NULL != ptSceneList->ptNext) {
        ptSceneList = ptSceneList->ptNext;
    }

    this.SceneFIFO.ptTail = ptSceneList;
}

ARM_NONNULL(1)
void arm_2d_user_scene_player_append_scenes(arm_2d_scene_player_t *ptThis, 
                                            arm_2d_scene_t *ptSceneList)
{
    assert(NULL != ptThis);

    while (NULL != ptSceneList) {
        arm_2d_scene_t *ptNext = ptSceneList->ptNext;
        
        ARM_LIST_QUEUE_ENQUEUE(this.SceneFIFO.ptHead,
                               this.SceneFIFO.ptTail,
                               ptSceneList);

        ptSceneList = ptNext;
    }
}

ARM_NONNULL(1)
void arm_2d_user_scene_player_next_scene(arm_2d_scene_player_t *ptThis)
{
    assert(NULL != ptThis);

    this.Runtime.bNextSceneReq = true;
}

static void __arm_2d_user_scene_player_next_scene(arm_2d_scene_player_t *ptThis)
{
    arm_2d_scene_t *ptScene = NULL;
    this.Runtime.bNextSceneReq = false;
    
    do {
        ARM_LIST_QUEUE_DEQUEUE(this.SceneFIFO.ptHead,
                               this.SceneFIFO.ptTail,
                               ptScene);
        if (NULL == ptScene) {
            break;
        }
        if (NULL != ptScene->fnDepose) {
            ptScene->fnDepose(ptScene);
        }
    } while(false);
}

#define ARM_2D_USER_SCENE_PLAYER_TASK_RESET()                                   \
            do {this.Runtime.chState = START;} while(0)

ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_user_scene_player_task(arm_2d_scene_player_t *ptThis)
{
    assert(NULL != ptThis);

    arm_2d_scene_t *ptScene = this.SceneFIFO.ptHead;
    arm_fsm_rt_t tResult;
    
    enum {
        START = 0,
        DRAW_BACKGROUND_PREPARE,
        DRAW_BACKGROUND,
        DRAW_SCENE_PREPARE,
        DRAW_SCENE_START,
        DRAW_SCENE,
        POST_SCENE_CHECK,
    };
    
    switch (this.Runtime.chState) {
        case START:
            this.Runtime.chState++;
            // fall-through
            
        case DRAW_BACKGROUND_PREPARE:
            if (NULL == ptScene->fnBackground) {
                this.Runtime.chState = DRAW_SCENE_PREPARE;
                break;
            } else {
                ARM_2D_INVOKE(ptScene->fnOnBGStart, ptScene);
            
                ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(   
                    &this.use_as__arm_2d_helper_pfb_t,
                    ptScene->fnBackground);
                this.Runtime.chState = DRAW_BACKGROUND;
            }
            // fall-through
            
        case DRAW_BACKGROUND:
            tResult = arm_2d_helper_pfb_task(
                &this.use_as__arm_2d_helper_pfb_t, 
                NULL);
            if (tResult < 0) {
                ARM_2D_USER_SCENE_PLAYER_TASK_RESET();
                return tResult;
            } else if (arm_fsm_rt_cpl != tResult) {
                return tResult;
            }
            
            ARM_2D_INVOKE(ptScene->fnOnBGComplete, ptScene);
            
            this.Runtime.chState = DRAW_SCENE_PREPARE;
            // fall-through
            
        case DRAW_SCENE_PREPARE:
            if (NULL == ptScene->fnScene) {
                ARM_2D_USER_SCENE_PLAYER_TASK_RESET();
                return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
            }

            ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(   
                &this.use_as__arm_2d_helper_pfb_t,
                ptScene->fnScene);
            this.Runtime.chState = DRAW_SCENE_START;
            // fall-through
            
        case DRAW_SCENE_START:
            ARM_2D_INVOKE(ptScene->fnOnFrameStart, ptScene);
            this.Runtime.chState = DRAW_SCENE;
            // fall-through
            
        case DRAW_SCENE:
            tResult = arm_2d_helper_pfb_task(
                &this.use_as__arm_2d_helper_pfb_t, 
                ptScene->ptDirtyRegion);
            if (tResult < 0) {
                ARM_2D_USER_SCENE_PLAYER_TASK_RESET();
                return tResult;
            } else if (arm_fsm_rt_cpl != tResult) {
                return tResult;
            }
            this.Runtime.chState = POST_SCENE_CHECK;
            // fall-through
            
        case POST_SCENE_CHECK:
            ARM_2D_INVOKE(ptScene->fnOnFrameCPL, ptScene);

            if (this.Runtime.bNextSceneReq) {
                __arm_2d_user_scene_player_next_scene(ptThis);
                
                ARM_2D_USER_SCENE_PLAYER_TASK_RESET();
                return arm_fsm_rt_cpl;
            }

            this.Runtime.chState = DRAW_SCENE_START;
            break;

        default:
            ARM_2D_USER_SCENE_PLAYER_TASK_RESET();
            break;
    }
    
    return arm_fsm_rt_on_going;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif
