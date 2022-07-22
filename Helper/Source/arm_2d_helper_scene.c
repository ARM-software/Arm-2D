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
 * $Date:        18. July 2022
 * $Revision:    V.1.0.1
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

#define __ARM_2D_IMPL__
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "arm_2d_helper.h"
#include "../../examples/common/controls/__common.h"

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

/*-----------------------------------------------------------------------------*
 * Scene Management                                                            *
 *-----------------------------------------------------------------------------*/

static void __scene_player_depose_all_scene(arm_2d_scene_player_t *ptThis)
{
    arm_2d_scene_t *ptScene = NULL;

    do {
        arm_irq_safe {
            ARM_LIST_QUEUE_DEQUEUE(this.SceneFIFO.ptHead,
                                   this.SceneFIFO.ptTail,
                                   ptScene);
        }
        if (NULL == ptScene) {
            break;
        }
        if (NULL != ptScene->fnDepose) {
            ptScene->fnDepose(ptScene);
        }
    } while(true);
}

ARM_NONNULL(1)
void arm_2d_scene_player_flush_fifo(arm_2d_scene_player_t *ptThis)
{
    assert(NULL != ptThis);

    __scene_player_depose_all_scene(ptThis);

    arm_irq_safe {
        this.SceneFIFO.ptHead = NULL;
        this.SceneFIFO.ptTail = NULL;
    }
}

ARM_NONNULL(1)
void arm_2d_scene_player_append_scenes( arm_2d_scene_player_t *ptThis, 
                                        arm_2d_scene_t *ptScenes,
                                        int_fast16_t hwCount)
{
    assert(NULL != ptThis);
    assert(NULL != ptScenes);
    assert(hwCount > 0);
    
    do {
        arm_2d_scene_t *ptScene = ptScenes++;
        arm_irq_safe {
            ARM_LIST_QUEUE_ENQUEUE(this.SceneFIFO.ptHead,
                                   this.SceneFIFO.ptTail,
                                   ptScene);
        }
    } while(--hwCount);
}


/*-----------------------------------------------------------------------------*
 * Scene Switching                                                             *
 *-----------------------------------------------------------------------------*/

ARM_NONNULL(1)
void arm_2d_scene_player_switch_to_next_scene(arm_2d_scene_player_t *ptThis)
{
    assert(NULL != ptThis);

    this.Runtime.bNextSceneReq = true;
}

static void __arm_2d_scene_player_next_scene(arm_2d_scene_player_t *ptThis)
{
    arm_2d_scene_t *ptScene = NULL;
    this.Runtime.bNextSceneReq = false;
    
    do {
        arm_irq_safe {
            ARM_LIST_QUEUE_DEQUEUE(this.SceneFIFO.ptHead,
                                   this.SceneFIFO.ptTail,
                                   ptScene);
        }
        if (NULL == ptScene) {
            break;
        }
        if (NULL != ptScene->fnDepose) {
            ptScene->fnDepose(ptScene);
        }
    } while(false);
}

ARM_NONNULL(1)
void arm_2d_scene_player_set_switching_mode(arm_2d_scene_player_t *ptThis,
                                            uint_fast16_t hwSettings)
{
    assert(NULL != ptThis);
    
    /* valid input */
    assert(     (   (hwSettings & __ARM_2D_SCENE_SWTICH_MODE_DEFAULT_BG_msk) 
                >>  __ARM_2D_SCENE_SWTICH_MODE_DEFAULT_BG_pos) 
            <   3);
            
    this.Switch.tConfig.hwSetting = hwSettings;
}

ARM_NONNULL(1)
uint16_t arm_2d_scene_player_get_switching_mode(arm_2d_scene_player_t *ptThis)
{
    assert(NULL != ptThis);
    return this.Switch.tConfig.hwSetting;
}

ARM_NONNULL(1)
void arm_2d_scene_player_set_switching_period(  arm_2d_scene_player_t *ptThis,
                                                uint_fast16_t hwMS)
{
    assert(NULL != ptThis);
    this.Switch.hwPeriod = MAX(hwMS, 100);
}

__WEAK
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_user)
{
    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    ARM_2D_UNUSED(bIsNewFrame);


    /* doing nothing at all */
    this.Runtime.bSwitchCPL = true;
    
    return arm_fsm_rt_cpl;
}

__WEAK
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_user_default_background)
{
    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    
    arm_2d_op_wait_async(NULL);
    return arm_fsm_rt_cpl;
}

static 
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_default_background)
{
    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    
    switch(this.Switch.tConfig.Feature.u2DefaultBG) {
    case 0: /* ARM_2D_SCENE_SWITCH_MODE_DEFAULT_BG_WHITE */
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
        break;
    case 1: /* ARM_2D_SCENE_SWITCH_MODE_DEFAULT_BG_BLACK */
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_BLACK);
        break;
    case 2: /* ARM_2D_SCENE_SWITCH_MODE_DEFAULT_BG_USER */
    case 3:
        __pfb_draw_scene_mode_user_default_background(pTarget, ptTile, bIsNewFrame);
        break;
    }

    arm_2d_op_wait_async(NULL);
    return arm_fsm_rt_cpl;
}


IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_fade)
{
    enum {
        START = 0,
        FADE_IN,
        FADE_OUT,
    };

    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    arm_2d_scene_t *ptScene = NULL;

    bool bIgnoreBG = true;
    bool bIgnoreScene = true;
    
    __arm_2d_color_t tColour    = this.Switch.Fade.bIsFadeBlack 
                                ?   (__arm_2d_color_t){GLCD_COLOR_BLACK}
                                :   (__arm_2d_color_t){GLCD_COLOR_WHITE};

    /* internal statemachine */
    if (bIsNewFrame) {
        int32_t nElapsed;
        int32_t lTimeStamp = arm_2d_helper_get_system_timestamp();
        uint_fast16_t hwOpacity;
        switch (this.Switch.chState) {
            case START:
                this.Switch.Fade.chOpacity = 0;
                this.Switch.lTimeStamp = lTimeStamp;
                this.Switch.chState++;
                //break;
            case FADE_IN:
                nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                hwOpacity = (256ul * (int64_t)nElapsed / arm_2d_helper_convert_ms_to_ticks(this.Switch.hwPeriod >> 1));
                
                this.Switch.Fade.chOpacity = MIN(255, hwOpacity);
                if (this.Switch.Fade.chOpacity >= 255) {
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.chState = FADE_OUT;
                }
                break;
            case FADE_OUT:
                nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                hwOpacity = (256ul * (int64_t)nElapsed / arm_2d_helper_convert_ms_to_ticks(this.Switch.hwPeriod >> 1));
                
                this.Switch.Fade.chOpacity = 255 - MIN(255, hwOpacity);
                if (this.Switch.Fade.chOpacity == 0) {
                    this.Runtime.bSwitchCPL = true;
                }
                break;
        }
    }

    assert(NULL != this.SceneFIFO.ptHead);

    if (FADE_IN == this.Switch.chState) {
        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead;

        bIgnoreBG = this.Switch.tConfig.Feature.bIgnoreOldSceneBG;
        bIgnoreScene = this.Switch.tConfig.Feature.bIgnoreOldScene;
        
    } else if (NULL != this.SceneFIFO.ptHead) {
        /* draw the new scene background */
        ptScene = this.SceneFIFO.ptHead->ptNext;

        bIgnoreBG = this.Switch.tConfig.Feature.bIgnoreNewSceneBG;
        bIgnoreScene = this.Switch.tConfig.Feature.bIgnoreNewScene;
    }

    if (NULL == ptScene || (bIgnoreBG && bIgnoreScene)) {
        __pfb_draw_scene_mode_default_background(pTarget, ptTile, bIsNewFrame);
    } else {
        if (!bIgnoreBG) {
            ARM_2D_INVOKE( ptScene->fnOnBGStart, ptScene);
            ARM_2D_INVOKE( ptScene->fnBackground, pTarget, ptTile, bIsNewFrame);
            ARM_2D_INVOKE( ptScene->fnOnBGComplete, ptScene);
        }
        if (!bIgnoreScene) {
            ARM_2D_INVOKE( ptScene->fnOnFrameStart, ptScene);
            ARM_2D_INVOKE( ptScene->fnScene, pTarget, ptTile, bIsNewFrame);
            ARM_2D_INVOKE( ptScene->fnOnFrameCPL, ptScene);
        }
    }

    arm_2d_fill_colour_with_opacity(ptTile,
                                    NULL,
                                    tColour,
                                    this.Switch.Fade.chOpacity);

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}


IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_slide)
{
    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    ARM_2D_UNUSED(bIsNewFrame);

    /* todo */
    this.Runtime.bSwitchCPL = true;
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}


IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_erase)
{
    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    ARM_2D_UNUSED(bIsNewFrame);

    /* todo */
    this.Runtime.bSwitchCPL = true;
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

/*-----------------------------------------------------------------------------*
 * Misc                                                                        *
 *-----------------------------------------------------------------------------*/
#define ARM_2D_USER_SCENE_PLAYER_TASK_RESET()                                   \
            do {this.Runtime.chState = START;} while(0)

ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_scene_player_task(arm_2d_scene_player_t *ptThis)
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
        
        SWITCH_SCENE_PREPARE,
        SWITCH_SCENE,
        SWITCH_SCENE_POST,
    };
    
    switch (this.Runtime.chState) {
        case START:
            if (NULL == ptScene) {
                //! no scene available
                return arm_fsm_rt_cpl;
            }
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
            
                if (    (    ARM_2D_SCENE_SWITCH_MODE_NONE 
                        ==   this.Switch.tConfig.Feature.chMode)
                   ||   (   this.Switch.tConfig.Feature.chMode
                        >=  __ARM_2D_SCENE_SWITCH_MODE_VALID)) {
                   /* no or unsupported visual effect for switching*/
                    this.Runtime.chState = SWITCH_SCENE_POST;
                } else {
                    this.Runtime.chState = SWITCH_SCENE_PREPARE;
                    /* also return arm_fsm_rt_cpl to indicate the completion of a frame */
                }
            } else {
                this.Runtime.chState = DRAW_SCENE_START;
            }

            /* return arm_fsm_rt_cpl to indicate a end of a frame */
            return arm_fsm_rt_cpl;

        default:
            ARM_2D_USER_SCENE_PLAYER_TASK_RESET();
            break;
        
        case SWITCH_SCENE_PREPARE: {
                arm_2d_helper_draw_handler_t *fnSwitchDrawer = NULL;

                assert(     this.Switch.tConfig.Feature.chMode 
                        !=  ARM_2D_SCENE_SWITCH_MODE_NONE);
                
                switch (this.Switch.tConfig.Feature.chMode) {
                    default:
                    case ARM_2D_SCENE_SWITCH_MODE_NONE:
                    case ARM_2D_SCENE_SWITCH_MODE_USER:
                        /* use user defined switching drawer */
                        fnSwitchDrawer = __pfb_draw_scene_mode_user;
                        break;

                    case ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE:
                        this.Switch.Fade.bIsFadeBlack = false;
                        /* fade in fade out */
                        fnSwitchDrawer = __pfb_draw_scene_mode_fade;
                        break;
                    case ARM_2D_SCENE_SWITCH_MODE_FADE_BLACK:
                        this.Switch.Fade.bIsFadeBlack = true;
                        /* fade in fade out */
                        fnSwitchDrawer = __pfb_draw_scene_mode_fade;
                        break;

                    case ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT:
                    case ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT:
                    case ARM_2D_SCENE_SWITCH_MODE_SLIDE_UP:
                    case ARM_2D_SCENE_SWITCH_MODE_SLIDE_DOWN:
                        /* slide effect */
                        fnSwitchDrawer = __pfb_draw_scene_mode_slide;
                        break;

                    case ARM_2D_SCENE_SWITCH_MODE_ERASE_LEFT:
                    case ARM_2D_SCENE_SWITCH_MODE_ERASE_RIGHT:
                    case ARM_2D_SCENE_SWITCH_MODE_ERASE_UP:
                    case ARM_2D_SCENE_SWITCH_MODE_ERASE_DOWN:
                        /* erase effect */
                        fnSwitchDrawer = __pfb_draw_scene_mode_erase;
                        break;
                }
                
                /* update drawer */
                ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(
                    &this.use_as__arm_2d_helper_pfb_t,
                    fnSwitchDrawer,
                    ptThis);
                
                /* reset flag */
                this.Runtime.bSwitchCPL = false;
                
                /* validate parameters */
                this.Switch.hwPeriod = MAX(100, this.Switch.hwPeriod);
                
                /* reset switch FSM */
                this.Switch.chState = START;
                this.Runtime.chState = SWITCH_SCENE;
            }
            //break;
        case SWITCH_SCENE:
            tResult = arm_2d_helper_pfb_task(&this.use_as__arm_2d_helper_pfb_t, 
                                             NULL);
            if (tResult < 0) {
                ARM_2D_USER_SCENE_PLAYER_TASK_RESET();
                return tResult;
            } else if (arm_fsm_rt_cpl != tResult) {
                return tResult;
            }

            if (this.Runtime.bSwitchCPL) {
                this.Runtime.chState = SWITCH_SCENE_POST;
            }

            /* return arm_fsm_rt_cpl to indicate a end of a frame */
            return arm_fsm_rt_cpl;
        
        case SWITCH_SCENE_POST:
            __arm_2d_scene_player_next_scene(ptThis);
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
