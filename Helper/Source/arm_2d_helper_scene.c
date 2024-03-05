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
 * $Date:        01. March 2024
 * $Revision:    V.1.4.7
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
#   pragma clang diagnostic ignored "-Wpedantic"
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
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
/*============================ LOCAL VARIABLES ===============================*/

extern
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_user);

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_fade);

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_slide);

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_erase);

/*============================ GLOBAL VARIABLES ==============================*/

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_NONE = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_NONE,
    .fnSwitchDrawer =           NULL,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_USER = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_USER,
    .fnSwitchDrawer =           &__pfb_draw_scene_mode_user,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_FADE_WHITE,
    .fnSwitchDrawer =           &__pfb_draw_scene_mode_fade,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_FADE_BLACK = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_FADE_BLACK,
    .fnSwitchDrawer =           &__pfb_draw_scene_mode_fade,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_SLIDE_LEFT,
    .fnSwitchDrawer =           __pfb_draw_scene_mode_slide,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_SLIDE_RIGHT,
    .fnSwitchDrawer =           __pfb_draw_scene_mode_slide,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_UP = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_SLIDE_UP,
    .fnSwitchDrawer =           __pfb_draw_scene_mode_slide,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_DOWN = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_SLIDE_DOWN,
    .fnSwitchDrawer =           __pfb_draw_scene_mode_slide,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_LEFT = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_ERASE_LEFT,
    .fnSwitchDrawer =           __pfb_draw_scene_mode_erase,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_RIGHT = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_ERASE_RIGHT,
    .fnSwitchDrawer =           __pfb_draw_scene_mode_erase,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_UP = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_ERASE_UP,
    .fnSwitchDrawer =           __pfb_draw_scene_mode_erase,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_DOWN = {
    .chEffects =                ARM_2D_SCENE_SWITCH_CFG_ERASE_DOWN,
    .fnSwitchDrawer =           __pfb_draw_scene_mode_erase,
};


/*============================ PROTOTYPES ====================================*/


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


ARM_NONNULL(1)
void arm_2d_scene_player_update_scene_background(arm_2d_scene_player_t *ptThis)
{
    assert(NULL != ptThis);

    this.Runtime.bUpdateBG = true;
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
void __arm_2d_scene_player_set_switching_mode(  arm_2d_scene_player_t *ptThis,
                                                arm_2d_scene_switch_mode_t *ptMode,
                                                uint16_t hwSettings)
{
    assert(NULL != ptThis);
    
    this.Switch.ptMode = ptMode;
    if (NULL == ptMode) {
        return ;
    }

    /* valid input */
    assert(     (   (hwSettings & __ARM_2D_SCENE_SWTICH_CFG_DEFAULT_BG_msk) 
                >>  __ARM_2D_SCENE_SWTICH_CFG_DEFAULT_BG_pos) 
            <   3);

    this.Switch.tConfig.hwSetting = hwSettings;
    this.Switch.tConfig.Feature.chMode = ptMode->chEffects;
}

ARM_NONNULL(1)
uint16_t arm_2d_scene_player_get_switching_cfg(arm_2d_scene_player_t *ptThis)
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

ARM_NONNULL(1)
arm_2d_err_t __arm_2d_scene_player_register_on_draw_navigation_event_handler(
                                    arm_2d_scene_player_t *ptThis,
                                    arm_2d_helper_draw_handler_t *fnHandler,
                                    void *pTarget,
                                    arm_2d_region_list_item_t *ptDirtyRegions)
{
    assert(NULL != ptThis);

    arm_2d_helper_pfb_dependency_t tDependency = {
        .Navigation.evtOnDrawing.fnHandler = fnHandler,
        .Navigation.evtOnDrawing.pTarget = ((pTarget == NULL) ? ptThis : pTarget),
        .Navigation.ptDirtyRegion = (NULL == fnHandler) ? NULL : ptDirtyRegions,
    };

    return arm_2d_helper_pfb_update_dependency(
                                    &ptThis->use_as__arm_2d_helper_pfb_t, 
                                    ARM_2D_PFB_DEPEND_ON_NAVIGATION,
                                    &tDependency);
}


ARM_NONNULL(1)
void arm_2d_scene_player_hide_navigation_layer(arm_2d_scene_player_t *ptThis)
{
    arm_2d_helper_hide_navigation_layer(&this.use_as__arm_2d_helper_pfb_t);
}

ARM_NONNULL(1)
void arm_2d_scene_player_show_navigation_layer(arm_2d_scene_player_t *ptThis)
{
    arm_2d_helper_show_navigation_layer(&this.use_as__arm_2d_helper_pfb_t);
}



arm_2d_err_t __arm_2d_scene_player_register_before_switching_event_handler(
                    arm_2d_scene_player_t *ptThis,
                    arm_2d_scene_before_scene_switching_handler_t *fnHandler,
                    void *pTarget
                )
{
    assert(NULL != ptThis);
    assert(NULL != fnHandler);
    
    this.Events.evtBeforeSwitching.fnHandler = fnHandler;
    this.Events.evtBeforeSwitching.pTarget = pTarget;
    
    return ARM_2D_ERR_NONE;
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
    case 0: /* ARM_2D_SCENE_SWITCH_CFG_DEFAULT_BG_WHITE */
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
        break;
    case 1: /* ARM_2D_SCENE_SWITCH_CFG_DEFAULT_BG_BLACK */
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_BLACK);
        break;
    case 2: /* ARM_2D_SCENE_SWITCH_CFG_DEFAULT_BG_USER */
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
    
    __arm_2d_color_t tColour    
        =   (   ARM_2D_SCENE_SWITCH_CFG_FADE_BLACK
            ==  this.Switch.tConfig.Feature.chMode)
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
                ARM_2D_INVOKE_RT_VOID( ptScene->fnOnBGStart, ptScene);
                ARM_2D_INVOKE(ptScene->fnBackground, ptScene, ptTile, bIsNewFrame);
                ARM_2D_INVOKE_RT_VOID( ptScene->fnOnBGComplete, ptScene);
            }
            if (!bIgnoreScene) {
                ARM_2D_INVOKE_RT_VOID( ptScene->fnOnFrameStart, ptScene);
                ARM_2D_INVOKE( ptScene->fnScene, ptScene, ptTile, bIsNewFrame);
                ARM_2D_INVOKE_RT_VOID( ptScene->fnOnFrameCPL, ptScene);
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
        ARM_2D_INVOKE_RT_VOID( ptScene->fnOnBGStart, ptScene);
        ARM_2D_INVOKE( ptScene->fnBackground, ptScene, ptTile, bIsNewFrame);
        ARM_2D_INVOKE_RT_VOID( ptScene->fnOnBGComplete, ptScene);
    }
    if (!bIgnoreScene) {
        ARM_2D_INVOKE_RT_VOID( ptScene->fnOnFrameStart, ptScene);
        ARM_2D_INVOKE( ptScene->fnScene, ptScene, ptTile, bIsNewFrame);
        ARM_2D_INVOKE_RT_VOID( ptScene->fnOnFrameCPL, ptScene);
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
        case ARM_2D_SCENE_SWITCH_CFG_ERASE_LEFT:
        case ARM_2D_SCENE_SWITCH_CFG_ERASE_RIGHT:
            iTargetDistance = ptTile->tRegion.tSize.iWidth;
            break;
        case ARM_2D_SCENE_SWITCH_CFG_ERASE_UP:
        case ARM_2D_SCENE_SWITCH_CFG_ERASE_DOWN:
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
        arm_2d_tile_t *ptWindow = NULL;
        if (NULL == this.SceneFIFO.ptHead) {
            break;
        }
        /* generate new tile for old scene */

        arm_2d_region_t tWindow = {
            .tSize = ptTile->tRegion.tSize,
        };
        
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_CFG_ERASE_LEFT:
                tWindow.tSize.iWidth -= iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tTemp, 
                                                        false);
                if (NULL == ptWindow) {
                    break;
                }

                tWindow.tSize.iWidth = iTargetDistance;
                ptWindow = arm_2d_tile_generate_child(  &this.Switch.Erase.tTemp, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tSceneWindow, 
                                                        false);
                break;
            case ARM_2D_SCENE_SWITCH_CFG_ERASE_RIGHT:
                tWindow.tSize.iWidth -= iOffset;
                tWindow.tLocation.iX += iOffset;

                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tTemp, 
                                                        false);
                if (NULL == ptWindow) {
                    break;
                }
                tWindow.tSize.iWidth = iTargetDistance;
                tWindow.tLocation.iX = -iOffset;
                ptWindow = arm_2d_tile_generate_child(  &this.Switch.Erase.tTemp, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tSceneWindow, 
                                                        false);
                break;
            case ARM_2D_SCENE_SWITCH_CFG_ERASE_UP:
                tWindow.tSize.iHeight -= iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tTemp, 
                                                        false);
                if (NULL == ptWindow) {
                    break;
                }

                tWindow.tSize.iHeight = iTargetDistance;
                ptWindow = arm_2d_tile_generate_child(  &this.Switch.Erase.tTemp, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_ERASE_DOWN:
                tWindow.tSize.iHeight -= iOffset;
                tWindow.tLocation.iY += iOffset;

                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tTemp, 
                                                        false);

                if (NULL == ptWindow) {
                    break;
                }

                tWindow.tSize.iHeight = iTargetDistance;
                tWindow.tLocation.iY = -iOffset;
                ptWindow = arm_2d_tile_generate_child(  &this.Switch.Erase.tTemp, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tSceneWindow, 
                                                        false);
                break;

            default:
                assert(false);      /* this should not happen */
        }

        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead;

        if (NULL != ptWindow) {
            __draw_erase_scene( ptThis, 
                                ptScene, 
                                &this.Switch.Erase.tSceneWindow, 
                                bIsNewFrame,
                                this.Switch.tConfig.Feature.bIgnoreOldSceneBG,
                                this.Switch.tConfig.Feature.bIgnoreOldScene);
        }


        /* generate new tile for new scene */
        if (NULL == ptScene->ptNext) {
            break;
        }
        
        tWindow = (arm_2d_region_t){ .tSize = ptTile->tRegion.tSize };
        
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_CFG_ERASE_LEFT:
                tWindow.tSize.iWidth = iOffset;
                tWindow.tLocation.iX = iTargetDistance - iOffset;

                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tTemp, 
                                                        false);
                if (NULL == ptWindow) {
                    break;
                }
                                            
                tWindow.tSize.iWidth = iTargetDistance;
                tWindow.tLocation.iX = -(iTargetDistance - iOffset);
                ptWindow = arm_2d_tile_generate_child(  &this.Switch.Erase.tTemp, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_ERASE_RIGHT:
                tWindow.tSize.iWidth = iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tTemp, 
                                                        false);
                if (NULL == ptWindow) {
                    break;
                }

                tWindow.tSize.iWidth = iTargetDistance;
                ptWindow = arm_2d_tile_generate_child(  &this.Switch.Erase.tTemp, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_ERASE_UP:
                tWindow.tSize.iHeight = iOffset;
                tWindow.tLocation.iY = iTargetDistance - iOffset;

                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tTemp, 
                                                        false);
                if (NULL == ptWindow) {
                    break;
                }

                tWindow.tSize.iHeight = iTargetDistance;
                tWindow.tLocation.iY = -(iTargetDistance - iOffset);
                ptWindow = arm_2d_tile_generate_child(  &this.Switch.Erase.tTemp, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_ERASE_DOWN:
                tWindow.tSize.iHeight = iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tTemp, 
                                                        false);

                if (NULL == ptWindow) {
                    break;
                }

                tWindow.tSize.iHeight = iTargetDistance;
                ptWindow = arm_2d_tile_generate_child(  &this.Switch.Erase.tTemp, 
                                                        &tWindow, 
                                                        &this.Switch.Erase.tSceneWindow, 
                                                        false);
                break;
            default:
                assert(false);      /* this should not happen */
        }

        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead->ptNext;
        
        if (NULL != ptWindow) {
            __draw_erase_scene( ptThis, 
                                ptScene, 
                                &this.Switch.Erase.tSceneWindow, 
                                bIsNewFrame,
                                this.Switch.tConfig.Feature.bIgnoreNewSceneBG,
                                this.Switch.tConfig.Feature.bIgnoreNewScene);
        }

    } while(0);

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}


static
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
        case ARM_2D_SCENE_SWITCH_CFG_SLIDE_LEFT:
        case ARM_2D_SCENE_SWITCH_CFG_SLIDE_RIGHT:
            iTargetDistance = ptTile->tRegion.tSize.iWidth;
            break;
        case ARM_2D_SCENE_SWITCH_CFG_SLIDE_UP:
        case ARM_2D_SCENE_SWITCH_CFG_SLIDE_DOWN:
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
            
            if (NULL != ptScene) {
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
        } else {
            bDrawDefaultBG = true;
        }

        if (bDrawDefaultBG) {
            __pfb_draw_scene_mode_default_background(pTarget, ptTile, bIsNewFrame);
        }
    } while(0);

    do {
        arm_2d_tile_t *ptWindow = NULL;
        if (NULL == this.SceneFIFO.ptHead) {
            break;
        }
        /* generate new tile for old scene */

        arm_2d_region_t tWindow = {
            .tSize = ptTile->tRegion.tSize,
        };
        
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_LEFT:
                tWindow.tLocation.iX = -this.Switch.Slide.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;
            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_RIGHT:
                tWindow.tLocation.iX = this.Switch.Slide.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;
            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_UP:
                tWindow.tLocation.iY = -this.Switch.Slide.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_DOWN:
                tWindow.tLocation.iY = this.Switch.Slide.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;

            default:
                assert(false);      /* this should not happen */
        }

        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead;

        if (NULL != ptWindow) {
            __draw_erase_scene( ptThis, 
                                ptScene, 
                                &this.Switch.Slide.tSceneWindow, 
                                bIsNewFrame,
                                this.Switch.tConfig.Feature.bIgnoreOldSceneBG,
                                this.Switch.tConfig.Feature.bIgnoreOldScene);
        }

        /* generate new tile for new scene */
        if (NULL == ptScene->ptNext) {
            break;
        }
        
        tWindow = (arm_2d_region_t){ .tSize = ptTile->tRegion.tSize };
        
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_LEFT:
                tWindow.tLocation.iX = iTargetDistance - this.Switch.Slide.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_RIGHT:
                tWindow.tLocation.iX = -(iTargetDistance - this.Switch.Slide.iOffset);
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_UP:
                tWindow.tLocation.iY = iTargetDistance - this.Switch.Slide.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_DOWN:
                tWindow.tLocation.iY = -(iTargetDistance - this.Switch.Slide.iOffset);
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;
            default:
                assert(false);      /* this should not happen */
        }

        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead->ptNext;
        
        if (ptWindow) {
            __draw_erase_scene( ptThis, 
                                ptScene, 
                                &this.Switch.Slide.tSceneWindow, 
                                bIsNewFrame,
                                this.Switch.tConfig.Feature.bIgnoreNewSceneBG,
                                this.Switch.tConfig.Feature.bIgnoreNewScene);
        }
        
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
        DRAW_FRAME_START,
        DRAW_BACKGROUND_PREPARE,
        DRAW_BACKGROUND,
        DRAW_SCENE_PREPARE,
        DRAW_SCENE,
        POST_SCENE_CHECK,
        
        SWITCH_SCENE_PREPARE,
        SWITCH_SCENE,
        SWITCH_SCENE_POST,
    };
    
    switch (this.Runtime.chState) {
        case START:

            if (NULL == ptScene) {
                bool bNextSceneReq = false;
                
                arm_irq_safe {
                    bNextSceneReq = this.Runtime.bNextSceneReq;
                    this.Runtime.bNextSceneReq = false;
                }

                if (bNextSceneReq) {
                    /* invoke evtBeforeSwitching */
                    ARM_2D_INVOKE_RT_VOID(
                        this.Events.evtBeforeSwitching.fnHandler, 
                        ARM_2D_PARAM(
                            this.Events.evtBeforeSwitching.pTarget,
                            ptThis, 
                            ptScene
                        ));
                    /* try again */
                    break;
                }

                //! no scene available
                return arm_fsm_rt_cpl;
            }
            this.Runtime.bUpdateBG = true;
            this.Runtime.chState++;
            // fall-through

        case DRAW_FRAME_START:
            ARM_2D_INVOKE_RT_VOID(ptScene->fnOnFrameStart, ptScene);
            if (!this.Runtime.bUpdateBG) {
                this.Runtime.chState = DRAW_SCENE_PREPARE;
                break;
            } else {
                this.Runtime.bUpdateBG = false;
                this.Runtime.chState = DRAW_BACKGROUND_PREPARE;
            }
            // fall-through
        
        case DRAW_BACKGROUND_PREPARE:
            if (NULL == ptScene->fnBackground) {
                /* if the dirty region list is available, draw fnScene directly 
                 * as background 
                 */
                if (NULL != ptScene->ptDirtyRegion && NULL != ptScene->fnScene) {
                    ARM_2D_INVOKE_RT_VOID(ptScene->fnOnBGStart, ptScene);
            
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
                ARM_2D_INVOKE_RT_VOID(ptScene->fnOnBGStart, ptScene);
            
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
            
            ARM_2D_INVOKE_RT_VOID(ptScene->fnOnBGComplete, ptScene);
            
            if (NULL == ptScene->fnBackground && NULL != ptScene->fnScene) {
                /* avoid call fnScene twice */
                this.Runtime.chState = POST_SCENE_CHECK;
                break;
            }

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
            /* call the on-frame-complete event handler if present */
            ARM_2D_INVOKE_RT_VOID(ptScene->fnOnFrameCPL, ptScene);

            if (this.Runtime.bNextSceneReq) {
                /* call the before-scene-switch-out event handler if present */
                ARM_2D_INVOKE_RT_VOID(ptScene->fnBeforeSwitchOut, ptScene);
                
                /* invoke evtBeforeSwitching */
                ARM_2D_INVOKE_RT_VOID(  this.Events.evtBeforeSwitching.fnHandler, 
                    ARM_2D_PARAM(
                        this.Events.evtBeforeSwitching.pTarget,
                        ptThis, 
                        ptScene
                    ));
                
                if (NULL == this.Switch.ptMode) {
                    this.Switch.tConfig.Feature.chMode = ARM_2D_SCENE_SWITCH_CFG_NONE;
                } else {
                    /* update switching mode */
                    this.Switch.tConfig.Feature.chMode = this.Switch.ptMode->chEffects;
                }
                
                if (    (    ARM_2D_SCENE_SWITCH_CFG_NONE 
                        ==   this.Switch.tConfig.Feature.chMode)
                   ||   (   this.Switch.tConfig.Feature.chMode
                        >=  __ARM_2D_SCENE_SWITCH_CFG_VALID)
                   ||   (NULL == this.Switch.ptMode)) {
                   /* no or unsupported visual effect for switching*/
                    this.Runtime.chState = SWITCH_SCENE_POST;
                } else {
                    this.Runtime.chState = SWITCH_SCENE_PREPARE;
                    /* also return arm_fsm_rt_cpl to indicate the completion of a frame */
                }
            } else {
                this.Runtime.chState = DRAW_FRAME_START;
            }

            /* return arm_fsm_rt_cpl to indicate a end of a frame */
            return arm_fsm_rt_cpl;

        default:
            ARM_2D_USER_SCENE_PLAYER_TASK_RESET();
            break;
        
        case SWITCH_SCENE_PREPARE: {

                assert(     this.Switch.ptMode->chEffects 
                        !=  ARM_2D_SCENE_SWITCH_CFG_NONE);

                /* update drawer */
                ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(
                    &this.use_as__arm_2d_helper_pfb_t,
                    this.Switch.ptMode->fnSwitchDrawer,
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

ARM_NONNULL(1,2)
bool arm_2d_scene_player_append_dirty_regions(arm_2d_scene_t *ptScene, 
                                              arm_2d_region_list_item_t *ptItems,
                                              size_t tCount)
{

    do {
        if (NULL == ptItems || 0 == tCount || NULL == ptScene) {
            break;
        }

        arm_2d_region_list_item_t **ppDirtyRegionList = &ptScene->ptDirtyRegion;
        if (NULL != ppDirtyRegionList) {
                while(NULL != (*ppDirtyRegionList)) {
                    ppDirtyRegionList = &((*ppDirtyRegionList)->ptNext);
                }

                /* add dirty region items to the list */
                (*ppDirtyRegionList) = ptItems;
                while(--tCount) {
                    ptItems->ptNext = ptItems + 1;
                    ptItems++;
                }
                ptItems->ptNext = NULL;
        }

        return true;
    } while(0);

    return false;
}

ARM_NONNULL(1,2)
bool arm_2d_scene_player_remove_dirty_regions(arm_2d_scene_t *ptScene, 
                                              arm_2d_region_list_item_t *ptItems,
                                              size_t tCount)
{

    do {
        if (NULL == ptItems || 0 == tCount || NULL == ptScene) {
            break;
        }

        do {
            /* search and remove item */
            arm_2d_region_list_item_t **ppDirtyRegionList = &ptScene->ptDirtyRegion;

            while(NULL != (*ppDirtyRegionList)) {

                /* remove the dirty region from the user dirty region list */
                if ((*ppDirtyRegionList) == ptItems) {
                    (*ppDirtyRegionList) = ptItems->ptNext;
                    ptItems->ptNext = NULL;
                    break;
                }

                ppDirtyRegionList = &((*ppDirtyRegionList)->ptNext);
            }

            /* next item */
            ptItems++;
        } while(--tCount);

        return true;
    } while(0);

    return false;
}

ARM_NONNULL(1)
arm_2d_scene_t * arm_2d_scene_player_get_the_current_scene(arm_2d_scene_player_t *ptThis)
{
    assert(NULL != ptThis);
    if (NULL == ptThis) {
        return NULL;
    }

    return this.SceneFIFO.ptHead;;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif
