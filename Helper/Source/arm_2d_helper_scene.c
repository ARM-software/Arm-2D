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
 * $Date:        15. Aug 2022
 * $Revision:    V.1.3.3
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
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wunused-value"
#endif

/*============================ MACROS ========================================*/
#undef this
#define this        (*ptThis)


#ifndef __ARM_2D_CFG_HELPER_SWITCH_FADE_USE_SIN__
#   define __ARM_2D_CFG_HELPER_SWITCH_FADE_USE_SIN__        0
#endif
#ifndef __ARM_2D_CFG_HELPER_SWITCH_MIN_PERIOD__
#   define __ARM_2D_CFG_HELPER_SWITCH_MIN_PERIOD__          200
#endif

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
        ptScene->ptPlayer = ptThis;
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
    this.Switch.hwPeriod = MAX(hwMS, __ARM_2D_CFG_HELPER_SWITCH_MIN_PERIOD__);
}

__WEAK
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_user)
{
    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);

    /* doing nothing at all */
    this.Runtime.bSwitchCPL = true;
    
    return arm_fsm_rt_cpl;
}

__WEAK
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_user_default_background)
{
    //arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(pTarget);
    
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


#define SCENE_SWITCH_RESET_FSM()    do {this.Switch.chState = START;} while(0)

IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_fade)
{
    enum {
        START = 0,
        FADE_IN,
        KEEP,
        FADE_OUT,
    };

    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    arm_2d_scene_t *ptScene = NULL;

    bool bIgnoreBG = true;
    bool bIgnoreScene = true;
    
    __arm_2d_color_t tColour    = this.Switch.Fade.bIsFadeBlack 
                                ?   (__arm_2d_color_t){GLCD_COLOR_BLACK}
                                :   (__arm_2d_color_t){GLCD_COLOR_WHITE};

    uint16_t hwKeepPeriod = MIN(this.Switch.hwPeriod / 3, 500);
    
    /* internal statemachine */
    if (bIsNewFrame) {
        int32_t nElapsed;
        int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();
        uint_fast16_t hwOpacity;
        switch (this.Switch.chState) {
            case START:
                this.Switch.Fade.chOpacity = 0;
                this.Switch.lTimeStamp = lTimeStamp;
                this.Switch.chState++;
                //break;
            case FADE_IN:
                nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                
            #if __ARM_2D_CFG_HELPER_SWITCH_FADE_USE_SIN__
                hwOpacity = (900ul * (int64_t)nElapsed 
                                   / arm_2d_helper_convert_ms_to_ticks(
                                        (this.Switch.hwPeriod - hwKeepPeriod) 
                                            >> 1));
                                            
                hwOpacity = (uint_fast16_t)
                            (256.0f 
                                * arm_sin_f32(ARM_2D_ANGLE(
                                    (float)hwOpacity / 10.0f)));
            #else
                hwOpacity = (uint_fast16_t)(256ul * (int64_t)nElapsed 
                                   / arm_2d_helper_convert_ms_to_ticks(
                                        (this.Switch.hwPeriod - hwKeepPeriod) 
                                            >> 1));
            #endif
                this.Switch.Fade.chOpacity = MIN(255, hwOpacity);
                if (this.Switch.Fade.chOpacity >= 255) {
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.chState = KEEP;
                }
                break;
            case KEEP:
                nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                if (nElapsed >= arm_2d_helper_convert_ms_to_ticks(hwKeepPeriod)) {
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.chState = FADE_OUT;
                }
                break;
            case FADE_OUT:
                nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                
            #if __ARM_2D_CFG_HELPER_SWITCH_FADE_USE_SIN__
                hwOpacity = (900ul * (int64_t)nElapsed 
                                   / arm_2d_helper_convert_ms_to_ticks(
                                        (this.Switch.hwPeriod - hwKeepPeriod) 
                                            >> 1));
                                            
                hwOpacity = (uint_fast16_t)
                            (256.0f 
                                * arm_sin_f32(ARM_2D_ANGLE(
                                    (float)hwOpacity / 10.0f)));
            #else
                hwOpacity = (uint_fast16_t)(256ul * (int64_t)nElapsed 
                                   / arm_2d_helper_convert_ms_to_ticks(
                                        (this.Switch.hwPeriod - hwKeepPeriod) 
                                            >> 1));
            #endif
                
                this.Switch.Fade.chOpacity = 255 - MIN(255, hwOpacity);
                if (this.Switch.Fade.chOpacity == 0) {
                    this.Runtime.bSwitchCPL = true;
                    SCENE_SWITCH_RESET_FSM();
                }
                break;
        }
    }

    do {
        if (KEEP == this.Switch.chState) {
            arm_2d_fill_colour( ptTile,
                                NULL,
                                tColour.tValue);
            break;
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

        if (NULL != ptScene) {
            bIgnoreBG = ptScene->bOnSwitchingIgnoreBG || bIgnoreBG;
            bIgnoreScene = ptScene->bOnSwitchingIgnoreScene || bIgnoreScene;
        }

        if (NULL == ptScene || (bIgnoreBG && bIgnoreScene)) {
            __pfb_draw_scene_mode_default_background(pTarget, ptTile, bIsNewFrame);
        } else {
            if (!bIgnoreBG) {
                ARM_2D_INVOKE( ptScene->fnOnBGStart, ptScene);
                ARM_2D_INVOKE( ptScene->fnBackground, ptScene, ptTile, bIsNewFrame);
                ARM_2D_INVOKE( ptScene->fnOnBGComplete, ptScene);
            }
            if (!bIgnoreScene) {
                ARM_2D_INVOKE( ptScene->fnOnFrameStart, ptScene);
                ARM_2D_INVOKE( ptScene->fnScene, ptScene, ptTile, bIsNewFrame);
                ARM_2D_INVOKE( ptScene->fnOnFrameCPL, ptScene);
            }
        }

        arm_2d_fill_colour_with_opacity(ptTile,
                                        NULL,
                                        tColour,
                                        this.Switch.Fade.chOpacity);
    } while(0);
    
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}


static 
void __draw_erase_scene(arm_2d_scene_player_t *ptThis,
                        arm_2d_scene_t *ptScene,
                        const arm_2d_tile_t *ptTile,
                        bool bIsNewFrame,
                        bool bIgnoreBG,
                        bool bIgnoreScene)
{
    ARM_2D_UNUSED(ptThis);

    if (NULL != ptScene) {
        bIgnoreBG = ptScene->bOnSwitchingIgnoreBG && bIgnoreBG;
        bIgnoreScene = ptScene->bOnSwitchingIgnoreScene && bIgnoreScene;
    }

    if (!bIgnoreBG) {
        ARM_2D_INVOKE( ptScene->fnOnBGStart, ptScene);
        ARM_2D_INVOKE( ptScene->fnBackground, ptScene, ptTile, bIsNewFrame);
        ARM_2D_INVOKE( ptScene->fnOnBGComplete, ptScene);
    }
    if (!bIgnoreScene) {
        ARM_2D_INVOKE( ptScene->fnOnFrameStart, ptScene);
        ARM_2D_INVOKE( ptScene->fnScene, ptScene, ptTile, bIsNewFrame);
        ARM_2D_INVOKE( ptScene->fnOnFrameCPL, ptScene);
    }
    arm_2d_op_wait_async(NULL);
}

IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_erase)
{
    enum {
        START = 0,
        ERASE,
    };

    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    arm_2d_scene_t *ptScene = NULL;
    
    int16_t iTargetDistance = 0;
    int16_t iOffset = 0;
    
    switch(this.Switch.tConfig.Feature.chMode) {
        case ARM_2D_SCENE_SWITCH_MODE_ERASE_LEFT:
        case ARM_2D_SCENE_SWITCH_MODE_ERASE_RIGHT:
            iTargetDistance = ptTile->tRegion.tSize.iWidth;
            break;
        case ARM_2D_SCENE_SWITCH_MODE_ERASE_UP:
        case ARM_2D_SCENE_SWITCH_MODE_ERASE_DOWN:
            iTargetDistance = ptTile->tRegion.tSize.iHeight;
            break;
        default:
            assert(false);      /* this should not happen */
    }

    
    /* internal statemachine */
    if (bIsNewFrame) {
        int32_t nElapsed;
        int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();

        switch (this.Switch.chState) {
            case START:
                this.Switch.lTimeStamp = lTimeStamp;
                this.Switch.Erase.iOffset = 0;
                this.Switch.chState++;
                //break;
            case ERASE:
                nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                
                this.Switch.Erase.iOffset = ((int64_t)iTargetDistance * (int64_t)nElapsed 
                           / arm_2d_helper_convert_ms_to_ticks( 
                                (this.Switch.hwPeriod) >> 1));
                this.Switch.Erase.iOffset = MIN(iTargetDistance, this.Switch.Erase.iOffset);

                if (this.Switch.Erase.iOffset >= iTargetDistance) {
                    this.Runtime.bSwitchCPL = true;
                    SCENE_SWITCH_RESET_FSM();
                }
                break;
        }
    }
    iOffset = this.Switch.Erase.iOffset;
    
    /* handle default background */
    do {
        bool bIgnoreBG;
        bool bIgnoreScene;
        bool bDrawDefaultBG = false;
        
        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead;

        if (NULL != ptScene) {
            bIgnoreBG = this.Switch.tConfig.Feature.bIgnoreOldSceneBG;
            bIgnoreScene = this.Switch.tConfig.Feature.bIgnoreOldScene;
            bIgnoreBG = ptScene->bOnSwitchingIgnoreBG || bIgnoreBG;
            bIgnoreScene = ptScene->bOnSwitchingIgnoreScene || bIgnoreScene;
            
            if (bIgnoreBG && bIgnoreScene) {
                bDrawDefaultBG = true;
            }
        } else {
            bDrawDefaultBG = true;
        }

        if (NULL != this.SceneFIFO.ptHead) {
            /* draw the new scene background */
            ptScene = this.SceneFIFO.ptHead->ptNext;
            
            bIgnoreBG = this.Switch.tConfig.Feature.bIgnoreNewSceneBG;
            bIgnoreScene = this.Switch.tConfig.Feature.bIgnoreNewScene;
            bIgnoreBG = ptScene->bOnSwitchingIgnoreBG || bIgnoreBG;
            bIgnoreScene = ptScene->bOnSwitchingIgnoreScene || bIgnoreScene;

            if (bIgnoreBG && bIgnoreScene) {
                bDrawDefaultBG = true;
            }
        } else {
            bDrawDefaultBG = true;
        }

        if (bDrawDefaultBG) {
            __pfb_draw_scene_mode_default_background(pTarget, ptTile, bIsNewFrame);
        }
    } while(0);

    do {
        if (NULL == this.SceneFIFO.ptHead) {
            break;
        }
        /* generate new tile for old scene */

        arm_2d_region_t tWindow = {
            .tSize = ptTile->tRegion.tSize,
        };
        
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_MODE_ERASE_LEFT:
                tWindow.tSize.iWidth -= iOffset;
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Erase.tTemp, 
                                            false);

                tWindow.tSize.iWidth = iTargetDistance;
                arm_2d_tile_generate_child( &this.Switch.Erase.tTemp, 
                                            &tWindow, 
                                            &this.Switch.Erase.tSceneWindow, 
                                            false);
                break;
            case ARM_2D_SCENE_SWITCH_MODE_ERASE_RIGHT:
                tWindow.tSize.iWidth -= iOffset;
                tWindow.tLocation.iX += iOffset;

                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Erase.tTemp, 
                                            false);
                                            
                tWindow.tSize.iWidth = iTargetDistance;
                tWindow.tLocation.iX = -iOffset;
                arm_2d_tile_generate_child( &this.Switch.Erase.tTemp, 
                                            &tWindow, 
                                            &this.Switch.Erase.tSceneWindow, 
                                            false);
                break;
            case ARM_2D_SCENE_SWITCH_MODE_ERASE_UP:
                tWindow.tSize.iHeight -= iOffset;
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Erase.tTemp, 
                                            false);

                tWindow.tSize.iHeight = iTargetDistance;
                arm_2d_tile_generate_child( &this.Switch.Erase.tTemp, 
                                            &tWindow, 
                                            &this.Switch.Erase.tSceneWindow, 
                                            false);
                break;

            case ARM_2D_SCENE_SWITCH_MODE_ERASE_DOWN:
                tWindow.tSize.iHeight -= iOffset;
                tWindow.tLocation.iY += iOffset;

                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Erase.tTemp, 
                                            false);
                                            
                tWindow.tSize.iHeight = iTargetDistance;
                tWindow.tLocation.iY = -iOffset;
                arm_2d_tile_generate_child( &this.Switch.Erase.tTemp, 
                                            &tWindow, 
                                            &this.Switch.Erase.tSceneWindow, 
                                            false);
                break;

            default:
                assert(false);      /* this should not happen */
        }

        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead;

        __draw_erase_scene( ptThis, 
                            ptScene, 
                            &this.Switch.Erase.tSceneWindow, 
                            bIsNewFrame,
                            this.Switch.tConfig.Feature.bIgnoreOldSceneBG,
                            this.Switch.tConfig.Feature.bIgnoreOldScene);
        


        /* generate new tile for new scene */
        if (NULL == ptScene->ptNext) {
            break;
        }
        
        tWindow = (arm_2d_region_t){ .tSize = ptTile->tRegion.tSize };
        
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_MODE_ERASE_LEFT:
                tWindow.tSize.iWidth = iOffset;
                tWindow.tLocation.iX = iTargetDistance - iOffset;

                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Erase.tTemp, 
                                            false);
                                            
                tWindow.tSize.iWidth = iTargetDistance;
                tWindow.tLocation.iX = -(iTargetDistance - iOffset);
                arm_2d_tile_generate_child( &this.Switch.Erase.tTemp, 
                                            &tWindow, 
                                            &this.Switch.Erase.tSceneWindow, 
                                            false);
                break;

            case ARM_2D_SCENE_SWITCH_MODE_ERASE_RIGHT:
                tWindow.tSize.iWidth = iOffset;
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Erase.tTemp, 
                                            false);

                tWindow.tSize.iWidth = iTargetDistance;
                arm_2d_tile_generate_child( &this.Switch.Erase.tTemp, 
                                            &tWindow, 
                                            &this.Switch.Erase.tSceneWindow, 
                                            false);
                break;

            case ARM_2D_SCENE_SWITCH_MODE_ERASE_UP:
                tWindow.tSize.iHeight = iOffset;
                tWindow.tLocation.iY = iTargetDistance - iOffset;

                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Erase.tTemp, 
                                            false);
                                            
                tWindow.tSize.iHeight = iTargetDistance;
                tWindow.tLocation.iY = -(iTargetDistance - iOffset);
                arm_2d_tile_generate_child( &this.Switch.Erase.tTemp, 
                                            &tWindow, 
                                            &this.Switch.Erase.tSceneWindow, 
                                            false);
                break;

            case ARM_2D_SCENE_SWITCH_MODE_ERASE_DOWN:
                tWindow.tSize.iHeight = iOffset;
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Erase.tTemp, 
                                            false);

                tWindow.tSize.iHeight = iTargetDistance;
                arm_2d_tile_generate_child( &this.Switch.Erase.tTemp, 
                                            &tWindow, 
                                            &this.Switch.Erase.tSceneWindow, 
                                            false);
                break;
            default:
                assert(false);      /* this should not happen */
        }

        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead->ptNext;
        
        
        __draw_erase_scene( ptThis, 
                            ptScene, 
                            &this.Switch.Erase.tSceneWindow, 
                            bIsNewFrame,
                            this.Switch.tConfig.Feature.bIgnoreNewSceneBG,
                            this.Switch.tConfig.Feature.bIgnoreNewScene);
        
    } while(0);

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}



IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_slide)
{
    enum {
        START = 0,
        SLIDE,
    };

    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    arm_2d_scene_t *ptScene = NULL;
    
    int16_t iTargetDistance = 0;
    //int16_t iOffset = 0;
    
    switch(this.Switch.tConfig.Feature.chMode) {
        case ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT:
        case ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT:
            iTargetDistance = ptTile->tRegion.tSize.iWidth;
            break;
        case ARM_2D_SCENE_SWITCH_MODE_SLIDE_UP:
        case ARM_2D_SCENE_SWITCH_MODE_SLIDE_DOWN:
            iTargetDistance = ptTile->tRegion.tSize.iHeight;
            break;
        default:
            assert(false);      /* this should not happen */
    }

    
    /* internal statemachine */
    if (bIsNewFrame) {
        int32_t nElapsed;
        int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();

        switch (this.Switch.chState) {
            case START:
                this.Switch.lTimeStamp = lTimeStamp;
                this.Switch.Slide.iOffset = 0;
                this.Switch.chState++;
                //break;
            case SLIDE:
                nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                
                this.Switch.Slide.iOffset = ((int64_t)iTargetDistance * (int64_t)nElapsed 
                           / arm_2d_helper_convert_ms_to_ticks( 
                                (this.Switch.hwPeriod) >> 1));
                this.Switch.Slide.iOffset = MIN(iTargetDistance, this.Switch.Slide.iOffset);

                if (this.Switch.Slide.iOffset >= iTargetDistance) {
                    this.Runtime.bSwitchCPL = true;
                    SCENE_SWITCH_RESET_FSM();
                }
                break;
        }
    }

    /* handle default background */
    do {
        bool bIgnoreBG;
        bool bIgnoreScene;
        bool bDrawDefaultBG = false;
        
        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead;

        if (NULL != ptScene) {
            bIgnoreBG = this.Switch.tConfig.Feature.bIgnoreOldSceneBG;
            bIgnoreScene = this.Switch.tConfig.Feature.bIgnoreOldScene;
            bIgnoreBG = ptScene->bOnSwitchingIgnoreBG || bIgnoreBG;
            bIgnoreScene = ptScene->bOnSwitchingIgnoreScene || bIgnoreScene;
            
            if (bIgnoreBG && bIgnoreScene) {
                bDrawDefaultBG = true;
            }
        } else {
            bDrawDefaultBG = true;
        }

        if (NULL != this.SceneFIFO.ptHead) {
            /* draw the new scene background */
            ptScene = this.SceneFIFO.ptHead->ptNext;
            
            bIgnoreBG = this.Switch.tConfig.Feature.bIgnoreNewSceneBG;
            bIgnoreScene = this.Switch.tConfig.Feature.bIgnoreNewScene;
            bIgnoreBG = ptScene->bOnSwitchingIgnoreBG || bIgnoreBG;
            bIgnoreScene = ptScene->bOnSwitchingIgnoreScene || bIgnoreScene;

            if (bIgnoreBG && bIgnoreScene) {
                bDrawDefaultBG = true;
            }
        } else {
            bDrawDefaultBG = true;
        }

        if (bDrawDefaultBG) {
            __pfb_draw_scene_mode_default_background(pTarget, ptTile, bIsNewFrame);
        }
    } while(0);

    do {
        if (NULL == this.SceneFIFO.ptHead) {
            break;
        }
        /* generate new tile for old scene */

        arm_2d_region_t tWindow = {
            .tSize = ptTile->tRegion.tSize,
        };
        
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT:
                tWindow.tLocation.iX = -this.Switch.Slide.iOffset;
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Slide.tSceneWindow, 
                                            false);
                break;
            case ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT:
                tWindow.tLocation.iX = this.Switch.Slide.iOffset;
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Slide.tSceneWindow, 
                                            false);
                break;
            case ARM_2D_SCENE_SWITCH_MODE_SLIDE_UP:
                tWindow.tLocation.iY = -this.Switch.Slide.iOffset;
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Slide.tSceneWindow, 
                                            false);
                break;

            case ARM_2D_SCENE_SWITCH_MODE_SLIDE_DOWN:
                tWindow.tLocation.iY = this.Switch.Slide.iOffset;
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Slide.tSceneWindow, 
                                            false);
                break;

            default:
                assert(false);      /* this should not happen */
        }

        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead;

        __draw_erase_scene( ptThis, 
                            ptScene, 
                            &this.Switch.Slide.tSceneWindow, 
                            bIsNewFrame,
                            this.Switch.tConfig.Feature.bIgnoreOldSceneBG,
                            this.Switch.tConfig.Feature.bIgnoreOldScene);
        


        /* generate new tile for new scene */
        if (NULL == ptScene->ptNext) {
            break;
        }
        
        tWindow = (arm_2d_region_t){ .tSize = ptTile->tRegion.tSize };
        
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT:
                tWindow.tLocation.iX = iTargetDistance - this.Switch.Slide.iOffset;
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Slide.tSceneWindow, 
                                            false);
                break;

            case ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT:
                tWindow.tLocation.iX = -(iTargetDistance - this.Switch.Slide.iOffset);
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Slide.tSceneWindow, 
                                            false);
                break;

            case ARM_2D_SCENE_SWITCH_MODE_SLIDE_UP:
                tWindow.tLocation.iY = iTargetDistance - this.Switch.Slide.iOffset;
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Slide.tSceneWindow, 
                                            false);
                break;

            case ARM_2D_SCENE_SWITCH_MODE_SLIDE_DOWN:
                tWindow.tLocation.iY = -(iTargetDistance - this.Switch.Slide.iOffset);
                arm_2d_tile_generate_child( ptTile, 
                                            &tWindow, 
                                            &this.Switch.Slide.tSceneWindow, 
                                            false);
                break;
            default:
                assert(false);      /* this should not happen */
        }

        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead->ptNext;
        
        
        __draw_erase_scene( ptThis, 
                            ptScene, 
                            &this.Switch.Slide.tSceneWindow, 
                            bIsNewFrame,
                            this.Switch.tConfig.Feature.bIgnoreNewSceneBG,
                            this.Switch.tConfig.Feature.bIgnoreNewScene);
        
    } while(0);

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
                /* if the dirty region list is available, draw fnScene directly 
                 * as background 
                 */
                if (NULL != ptScene->ptDirtyRegion && NULL != ptScene->fnScene) {
                    ARM_2D_INVOKE(ptScene->fnOnBGStart, ptScene);
            
                    ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(   
                        &this.use_as__arm_2d_helper_pfb_t,
                        ptScene->fnScene,
                        ptScene);
                    this.Runtime.chState = DRAW_BACKGROUND;
                } else {
                    this.Runtime.chState = DRAW_SCENE_PREPARE;
                }
                break;
            } else {
                ARM_2D_INVOKE(ptScene->fnOnBGStart, ptScene);
            
                ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(   
                    &this.use_as__arm_2d_helper_pfb_t,
                    ptScene->fnBackground,
                    ptScene);
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
                ptScene->fnScene,
                ptScene);
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
                this.Switch.hwPeriod 
                    = MAX(  __ARM_2D_CFG_HELPER_SWITCH_MIN_PERIOD__, 
                            this.Switch.hwPeriod);
                
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
