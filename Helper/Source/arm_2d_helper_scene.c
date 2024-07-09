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
 * $Date:        9. July 2024
 * $Revision:    V.1.8.0
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
#   pragma clang diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#   pragma clang diagnostic ignored "-Wswitch-default"
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

#define __slide_on_change_switch_status     __default_on_change_switch_status
#define __erase_on_change_switch_status     __default_on_change_switch_status
#define __fly_in_on_change_switch_status    __default_on_change_switch_status

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

extern
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_user);

static
ARM_NONNULL(1)
void __fade_on_change_switch_status(arm_2d_scene_player_t *ptThis);

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_fade);

static
ARM_NONNULL(1)
void __default_on_change_switch_status(arm_2d_scene_player_t *ptThis);

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_slide);

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_erase);

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_fly_in);

/*============================ GLOBAL VARIABLES ==============================*/

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_NONE = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_NONE,
    .fnSwitchDrawer =                   NULL,
    .fnOnRequestChangeSwitchingStatus = NULL,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_USER = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_USER,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_user,
    .fnOnRequestChangeSwitchingStatus = NULL,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_FADE_WHITE,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_fade,
    .fnOnRequestChangeSwitchingStatus = &__fade_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_FADE_BLACK = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_FADE_BLACK,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_fade,
    .fnOnRequestChangeSwitchingStatus = &__fade_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_SLIDE_LEFT,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_slide,
    .fnOnRequestChangeSwitchingStatus = &__slide_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_RIGHT = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_SLIDE_RIGHT,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_slide,
    .fnOnRequestChangeSwitchingStatus = &__slide_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_UP = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_SLIDE_UP,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_slide,
    .fnOnRequestChangeSwitchingStatus = &__slide_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_SLIDE_DOWN = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_SLIDE_DOWN,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_slide,
    .fnOnRequestChangeSwitchingStatus = &__slide_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_LEFT = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_ERASE_LEFT,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_erase,
    .fnOnRequestChangeSwitchingStatus = &__erase_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_RIGHT = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_ERASE_RIGHT,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_erase,
    .fnOnRequestChangeSwitchingStatus = &__erase_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_UP = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_ERASE_UP,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_erase,
    .fnOnRequestChangeSwitchingStatus = &__erase_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_ERASE_DOWN = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_ERASE_DOWN,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_erase,
    .fnOnRequestChangeSwitchingStatus = &__erase_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_FLY_IN_FROM_LEFT = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_LEFT,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_fly_in,
    .fnOnRequestChangeSwitchingStatus = &__fly_in_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_FLY_IN_FROM_RIGHT = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_RIGHT,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_fly_in,
    .fnOnRequestChangeSwitchingStatus = &__fly_in_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_FLY_IN_FROM_TOP = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_TOP,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_fly_in,
    .fnOnRequestChangeSwitchingStatus = &__fly_in_on_change_switch_status,
};

arm_2d_scene_switch_mode_t ARM_2D_SCENE_SWITCH_MODE_FLY_IN_FROM_BOTTOM = {
    .chEffects =                        ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_BOTTOM,
    .fnSwitchDrawer =                   &__pfb_draw_scene_mode_fly_in,
    .fnOnRequestChangeSwitchingStatus = &__fly_in_on_change_switch_status,
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

        if (ptScene->bUseDirtyRegionHelper) {
            arm_2d_helper_dirty_region_init(&ptScene->tDirtyRegionHelper,
                                            &ptScene->ptDirtyRegion);
        }

    } while(--hwCount);
}


ARM_NONNULL(1)
void arm_2d_scene_player_update_scene_background(arm_2d_scene_player_t *ptThis)
{
    assert(NULL != ptThis);

    this.Runtime.bUpdateBG = true;
}

ARM_NONNULL(1)
arm_2d_size_t arm_2d_scene_player_get_screen_size(arm_2d_scene_player_t *ptThis)
{
    assert (NULL != ptThis);

    return arm_2d_helper_pfb_get_display_area(
                &this.use_as__arm_2d_helper_pfb_t).tSize;
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

        arm_2d_helper_dirty_region_depose(&ptScene->tDirtyRegionHelper);
        if (NULL != ptScene->fnDepose) {
            ptScene->fnDepose(ptScene);
        }
    } while(false);
}

static void __arm_2d_scene_player_delete_all_next_scene(arm_2d_scene_player_t *ptThis) 
{
    arm_2d_scene_t *ptScene = NULL;
    this.Runtime.bNextSceneReq = false;

    do {
        arm_irq_safe {
            do {
                ptScene = NULL;
                arm_2d_scene_t *ptHead = this.SceneFIFO.ptHead;
                if (NULL == ptHead) {
                    break;
                }
                ptScene = ptHead->ptNext;
                if (NULL == ptScene) {
                    /* no next item */
                    break;
                }

                /* remove all next item */
                ptHead->ptNext = NULL;
                this.SceneFIFO.ptTail = ptHead;
            } while(0);
        }
        if (NULL == ptScene) {
            break;
        }
        arm_2d_helper_dirty_region_depose(&ptScene->tDirtyRegionHelper);

        /* depose all following scenes */
        while (NULL != ptScene) {
            arm_2d_scene_t *ptItem = ptScene;
            ptScene = ptScene->ptNext;

            if (NULL != ptItem->fnDepose) {
                ptItem->fnDepose(ptItem);
            }
            ptItem->ptNext = NULL;
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

    this.Switch.tConfig.hwSetting = hwSettings ;
    this.Switch.tConfig.Feature.chMode = ptMode->chEffects;
}

ARM_NONNULL(1)
uint16_t arm_2d_scene_player_get_switching_cfg(arm_2d_scene_player_t *ptThis)
{
    assert(NULL != ptThis);
    return this.Switch.tConfig.hwSetting;
}

ARM_NONNULL(1)
void arm_2d_scene_player_set_auto_switching_period( arm_2d_scene_player_t *ptThis,
                                                    int_fast16_t iMS)
{
    assert(NULL != ptThis);

    if (iMS < 0) {
        /* use the manual switching mode */
        this.Runtime.bManualSwitchReq = true;
        this.Switch.tTouchPointer.iX = -1;
        this.Switch.tTouchPointer.iY = -1;
    } else {
        this.Runtime.bManualSwitchReq = false;
        this.Switch.hwPeriod = MIN(iMS, UINT16_MAX);
        this.Switch.hwPeriod = MAX(iMS, __ARM_2D_CFG_HELPER_SWITCH_MIN_PERIOD__);
    }

    do {
        if (NULL == this.Switch.ptMode) {
            break;
        }
        
        /* call on request chaging switching status event handler */
        ARM_2D_INVOKE_RT_VOID(
            this.Switch.ptMode->fnOnRequestChangeSwitchingStatus,
            ARM_2D_PARAM(
                ptThis
            ));
    } while(0);
}

ARM_NONNULL(1)
void arm_2d_scene_player_set_manual_switching_offset(   arm_2d_scene_player_t *ptThis,
                                                        arm_2d_location_t tPointer)
{
    assert(NULL != ptThis);

    if (tPointer.iX < 0 || tPointer.iY < 0) {
        return ;
    } else {
        this.Runtime.bManualSwitchReq = true;
        this.Switch.tTouchPointer = tPointer;
    }

    do {
        if (NULL == this.Switch.ptMode) {
            break;
        }
        
        /* call on request chaging switching status event handler */
        ARM_2D_INVOKE_RT_VOID(this.Switch.ptMode->fnOnRequestChangeSwitchingStatus,
                            ARM_2D_PARAM(
                                ptThis
                            ));
    } while(0);
}

ARM_NONNULL(1)
arm_2d_scene_player_switch_status_t
arm_2d_scene_player_get_switching_status(arm_2d_scene_player_t *ptThis)
{
    assert(NULL != ptThis);
    if (this.Runtime.bManualSwitch) {

        if (this.Runtime.bFinishManualSwitch) {
            if (this.Runtime.bCancelSwitch) {
                return ARM_2D_SCENE_SWITCH_STATUS_MANUAL_CANCEL;
            }

            return ARM_2D_SCENE_SWITCH_STATUS_MANUAL_AUTO_CPL;
        }

        return ARM_2D_SCENE_SWITCH_STATUS_MANUAL;
    }

    return ARM_2D_SCENE_SWITCH_STATUS_AUTO;
}

ARM_NONNULL(1)
arm_2d_err_t arm_2d_scene_player_finish_manual_switching(   arm_2d_scene_player_t *ptThis, 
                                                            bool bMoveToPreviousScene,
                                                            int_fast16_t iInMS)
{
    assert(NULL != ptThis);

    arm_2d_scene_player_switch_status_t tStatus 
        = arm_2d_scene_player_get_switching_status(ptThis);

    if (ARM_2D_SCENE_SWITCH_STATUS_AUTO == tStatus) {
        /* you cannot call this API for auto-switching mode */
        return ARM_2D_ERR_INVALID_STATUS;
    } else if (tStatus !=  ARM_2D_SCENE_SWITCH_STATUS_MANUAL) {
        return ARM_2D_ERR_NONE;
    }

    this.Switch.lTimeStamp = arm_2d_helper_get_system_timestamp();
    this.Runtime.bFinishManualSwitchReq = true;
    this.Runtime.bCancelSwitchReq = bMoveToPreviousScene;

    this.Switch.hwPeriod = MIN(iInMS, UINT16_MAX);
    this.Switch.hwPeriod = MAX(iInMS, __ARM_2D_CFG_HELPER_SWITCH_MIN_PERIOD__);

    do {
        if (NULL == this.Switch.ptMode) {
            break;
        }
        
        /* call on request changing switching status event handler */
        ARM_2D_INVOKE_RT_VOID(this.Switch.ptMode->fnOnRequestChangeSwitchingStatus,
                            ARM_2D_PARAM(
                                ptThis
                            ));
    } while(0);

    return ARM_2D_ERR_NONE;
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

/*----------------------------------------------------------------------------*
 * Switch Mode: Fade-In-Fade-Out                                              *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1)
static void __fade_on_change_switch_status(arm_2d_scene_player_t *ptThis)
{
    enum {
        START = 0,
        LEFT_PAD,
        FADE_IN,
        KEEP,
        FADE_OUT,
        RIGHT_PAD,
    };

    assert(NULL != ptThis);

    if (this.Runtime.bManualSwitchReq == this.Runtime.bManualSwitch) {
        if (!this.Runtime.bManualSwitch) {
            /* auto switch, no change */
            return ;
        }

        if (this.Runtime.bFinishManualSwitch == this.Runtime.bFinishManualSwitchReq) {
            if (this.Runtime.bCancelSwitch == this.Runtime.bCancelSwitchReq) {
                /* no switch mode changed */
                return ;
            }
        }
    }

    if (this.Runtime.bManualSwitch && this.Runtime.bFinishManualSwitchReq) {

        int16_t iFullLength = this.Switch.iFullLength;

        if (this.Switch.tTouchPointer.iX < 0) {
            this.Switch.tTouchPointer.iX = 0;
        }

        int16_t iTouchOffset = iFullLength - MIN(this.Switch.tTouchPointer.iX, iFullLength);
        int16_t iZoneWidth = (iFullLength >> 3);
        int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();
        uint16_t hwKeepPeriod = MIN(this.Switch.hwPeriod / 3, 500);
        int32_t nElapsedTime = 0;


        if (!this.Runtime.bCancelSwitchReq) {

            switch (this.Switch.chState) {
                case START:
                case LEFT_PAD:
                    this.Switch.Fade.chOpacity = 0;
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.chState = FADE_IN;
                    break;

                case FADE_IN:
                    iTouchOffset -= iZoneWidth * 1;
                    nElapsedTime = iTouchOffset * ((this.Switch.hwPeriod - hwKeepPeriod)>> 1) / (iZoneWidth * 2);
                    this.Switch.lTimeStamp 
                        = lTimeStamp 
                        - arm_2d_helper_convert_ms_to_ticks(nElapsedTime);
                    break;

                case KEEP:
                    iTouchOffset -= iZoneWidth * 3;
                    nElapsedTime = iTouchOffset * (hwKeepPeriod>> 1) / (iZoneWidth * 2);
                    this.Switch.lTimeStamp 
                        = lTimeStamp 
                        - arm_2d_helper_convert_ms_to_ticks(nElapsedTime);
                    break;

                case FADE_OUT: 
                    iTouchOffset -= iZoneWidth * 5;
                    nElapsedTime = iTouchOffset * ((this.Switch.hwPeriod - hwKeepPeriod)>> 1) / (iZoneWidth * 2);
                    this.Switch.lTimeStamp 
                        = lTimeStamp 
                        - arm_2d_helper_convert_ms_to_ticks(nElapsedTime);
                    break;

                case RIGHT_PAD:
                    this.Switch.chState = FADE_OUT;
                    this.Switch.lTimeStamp 
                        = lTimeStamp 
                        - arm_2d_helper_convert_ms_to_ticks(
                            (this.Switch.hwPeriod - hwKeepPeriod)>> 1);
                    break;
                default:
                    break;
            }
        } else {
            /* cancel the swiching, move back to the old scene */
            int16_t iBarLength =  (iZoneWidth * 2);

            switch (this.Switch.chState) {
                case START:
                case LEFT_PAD:
                    this.Switch.Fade.chOpacity = 0;
                    this.Switch.lTimeStamp 
                        = lTimeStamp 
                        - arm_2d_helper_convert_ms_to_ticks(
                            (this.Switch.hwPeriod - hwKeepPeriod)>> 1);
                    this.Switch.chState = FADE_IN;
                    break;

                case FADE_IN:
                    iTouchOffset -= iZoneWidth * 1;

                    nElapsedTime = (iBarLength - iTouchOffset) * ((this.Switch.hwPeriod - hwKeepPeriod)>> 1) / iBarLength;
                    this.Switch.lTimeStamp 
                        = lTimeStamp 
                        - arm_2d_helper_convert_ms_to_ticks(nElapsedTime);
                    break;

                case KEEP:
                    iTouchOffset -= iZoneWidth * 3;
                    nElapsedTime = (iBarLength - iTouchOffset) * (hwKeepPeriod>> 1) / iBarLength;
                    this.Switch.lTimeStamp 
                        = lTimeStamp 
                        - arm_2d_helper_convert_ms_to_ticks(nElapsedTime);
                    break;

                case FADE_OUT: 
                    iTouchOffset -= iZoneWidth * 5;
                    nElapsedTime = (iBarLength - iTouchOffset) * ((this.Switch.hwPeriod - hwKeepPeriod)>> 1) / iBarLength;
                    this.Switch.lTimeStamp 
                        = lTimeStamp 
                        - arm_2d_helper_convert_ms_to_ticks(nElapsedTime);
                    break;

                case RIGHT_PAD:
                    this.Switch.chState = FADE_OUT;
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.Fade.chOpacity = 0;
                    break;
                default:
                    break;
            }
        
        }
    }
}

#define SCENE_SWITCH_RESET_FSM()    do {this.Switch.chState = START;} while(0)

IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_fade)
{
    enum {
        START = 0,
        LEFT_PAD,
        FADE_IN,
        KEEP,
        FADE_OUT,
        RIGHT_PAD,
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

    /* internal state-machine */
    if (bIsNewFrame) {

        //! update the switching status
        this.Runtime.bManualSwitch = this.Runtime.bManualSwitchReq;
        this.Runtime.bFinishManualSwitch = this.Runtime.bFinishManualSwitchReq;
        this.Runtime.bCancelSwitch = this.Runtime.bCancelSwitchReq;

        this.Switch.iFullLength = ptTile->tRegion.tSize.iWidth;

        arm_2d_scene_player_switch_status_t tStatus = arm_2d_scene_player_get_switching_status(ptThis);

        if (    (ARM_2D_SCENE_SWITCH_STATUS_AUTO == tStatus)
           ||   (ARM_2D_SCENE_SWITCH_STATUS_MANUAL_AUTO_CPL == tStatus)) {
            uint16_t hwKeepPeriod = MIN(this.Switch.hwPeriod / 3, 500);
        
            int32_t nElapsed;
            int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();
            uint_fast16_t hwOpacity;
            switch (this.Switch.chState) {
                case START:
                    this.Switch.Fade.chOpacity = 0;
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.chState = FADE_IN;
                    //break;
                case FADE_IN:
                    this.Runtime.bCallOldSceneFrameCPL = true;

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
                    this.Runtime.bCallOldSceneFrameCPL = false;

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
                        //SCENE_SWITCH_RESET_FSM();
                    }
                    break;
            }

        } else if (ARM_2D_SCENE_SWITCH_STATUS_MANUAL == tStatus) {
            uint_fast16_t hwOpacity = 0;
            
            int16_t iFullLength = this.Switch.iFullLength;

            if (this.Switch.tTouchPointer.iX < 0) {
                this.Switch.tTouchPointer.iX = 0;
            }

            int16_t iTouchOffset = iFullLength - MIN(this.Switch.tTouchPointer.iX, iFullLength);
            int16_t iZoneWidth = (iFullLength >> 3);
            int_fast8_t chStage = iTouchOffset / iZoneWidth;
            switch (chStage) {
                case 0:
                    this.Switch.chState = LEFT_PAD;
                    hwOpacity = 0;
                    this.Runtime.bCallOldSceneFrameCPL = true;
                    break;
                case 1:
                case 2:
                    this.Switch.chState = FADE_IN;
                    iTouchOffset -= iZoneWidth;
                    hwOpacity = iTouchOffset * 255 / (iZoneWidth * 2);
                    this.Runtime.bCallOldSceneFrameCPL = true;
                    break;
                case 3:
                case 4:
                    this.Switch.chState = KEEP;
                    this.Runtime.bCallOldSceneFrameCPL = false;
                    break;
                case 5:
                case 6:
                    this.Switch.chState = FADE_OUT;
                    iTouchOffset -= iZoneWidth * 5;
                    hwOpacity = 255 - (iTouchOffset * 255 / (iZoneWidth * 2));
                    this.Runtime.bCallOldSceneFrameCPL = false;
                    break;
                case 7:
                    this.Switch.chState = RIGHT_PAD;
                    hwOpacity = 0;
                    this.Runtime.bCallOldSceneFrameCPL = false;

                    //this.Runtime.bSwitchCPL = true;
                    break;
            }

            this.Switch.Fade.chOpacity = MIN(255, hwOpacity);

        } else if (ARM_2D_SCENE_SWITCH_STATUS_MANUAL_CANCEL == tStatus) {
            uint16_t hwKeepPeriod = MIN(this.Switch.hwPeriod / 3, 500);
        
            int32_t nElapsed;
            int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();
            uint_fast16_t hwOpacity;
            switch (this.Switch.chState) {
                case START:
                    this.Switch.Fade.chOpacity = 0;
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.chState = FADE_OUT;
                    //break;

                case FADE_OUT:
                    this.Runtime.bCallOldSceneFrameCPL = false;

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
                    this.Runtime.bCallOldSceneFrameCPL = false;

                    nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                    if (nElapsed >= arm_2d_helper_convert_ms_to_ticks(hwKeepPeriod)) {
                        this.Switch.lTimeStamp = lTimeStamp;
                        this.Switch.chState = FADE_IN;
                    }
                    break;
                case FADE_IN:
                    this.Runtime.bCallOldSceneFrameCPL = true;

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
                        //SCENE_SWITCH_RESET_FSM();
                    }
                    break;
            }

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

        switch (this.Switch.chState) {
            case LEFT_PAD:
            case FADE_IN:
                /* draw the old scene background */
                ptScene = this.SceneFIFO.ptHead;

                bIgnoreBG = this.Switch.tConfig.Feature.bIgnoreOldSceneBG;
                bIgnoreScene = this.Switch.tConfig.Feature.bIgnoreOldScene;
                break;
            
            case FADE_OUT:
            case RIGHT_PAD:
                if (NULL != this.SceneFIFO.ptHead) {
                    /* draw the new scene background */
                    ptScene = this.SceneFIFO.ptHead->ptNext;

                    bIgnoreBG = this.Switch.tConfig.Feature.bIgnoreNewSceneBG;
                    bIgnoreScene = this.Switch.tConfig.Feature.bIgnoreNewScene;
                }
                break;
            default:
                break;
        }

        if (NULL != ptScene) {
            bIgnoreBG = ptScene->bOnSwitchingIgnoreBG || bIgnoreBG;
            bIgnoreScene = ptScene->bOnSwitchingIgnoreScene || bIgnoreScene;
        }

        if (bIsNewFrame) {
            if (FADE_IN == this.Switch.chState) {
                this.Runtime.bCallOldSceneBGCPL = !bIgnoreBG;
            } else if (FADE_OUT == this.Switch.chState) {
                this.Runtime.bCallNewSceneFrameCPL = (NULL != ptScene);
                this.Runtime.bCallNewSceneBGCPL = !bIgnoreBG && (NULL != ptScene);
            }
        }

        if (NULL == ptScene || (bIgnoreBG && bIgnoreScene)) {
            __pfb_draw_scene_mode_default_background(pTarget, ptTile, bIsNewFrame);
        } else {
            bool bCanvasFilled = false;

            if (!bIgnoreBG) {
                if (bIsNewFrame) {
                    ARM_2D_INVOKE_RT_VOID( ptScene->fnOnBGStart, ptScene);
                }

                /* fill canvas */
                arm_2d_helper_fill_tile_colour( ptTile, 
                                                ptTile->tInfo.tColourInfo,
                                                ptScene->tCanvas);
                bCanvasFilled = true;
                ARM_2D_INVOKE(ptScene->fnBackground, ptScene, ptTile, bIsNewFrame);
            }

            if (!bIgnoreScene) {
                if (bIsNewFrame) {
                    ARM_2D_INVOKE_RT_VOID( ptScene->fnOnFrameStart, ptScene);
                }

                if (!bCanvasFilled) {
                    /* fill canvas */
                    arm_2d_helper_fill_tile_colour( ptTile, 
                                                    ptTile->tInfo.tColourInfo,
                                                    ptScene->tCanvas);
                }

                ARM_2D_INVOKE( ptScene->fnScene, ptScene, ptTile, bIsNewFrame);
            }
        }

        arm_2d_fill_colour_with_opacity(ptTile,
                                        NULL,
                                        tColour,
                                        this.Switch.Fade.chOpacity);
    } while(0);

    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}



ARM_NONNULL(1)
static void __default_on_change_switch_status(arm_2d_scene_player_t *ptThis)
{
    enum {
        START   = 0,
        MOVE, 
        ERASE   = MOVE,
        SLIDE   = MOVE,
        FLY_IN  = MOVE,
    };

    assert(NULL != ptThis);

    if (this.Runtime.bManualSwitchReq == this.Runtime.bManualSwitch) {
        if (!this.Runtime.bManualSwitch) {
            /* auto switch, no change */
            return ;
        }

        if (this.Runtime.bFinishManualSwitch == this.Runtime.bFinishManualSwitchReq) {
            if (this.Runtime.bCancelSwitch == this.Runtime.bCancelSwitchReq) {
                /* no switch mode changed */
                return ;
            }
        }
    }

    if (this.Runtime.bManualSwitch && this.Runtime.bFinishManualSwitchReq) {

        int16_t iFullLength = this.Switch.iFullLength;

        int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();
        int32_t nElapsedTime = 0;

        if (!this.Runtime.bCancelSwitchReq) {

            switch (this.Switch.chState) {
                case START:
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.iOffset = 0;
                    this.Switch.chState = MOVE;
                    break;
                case MOVE:
                    nElapsedTime = this.Switch.iOffset * this.Switch.hwPeriod / iFullLength;
                    this.Switch.lTimeStamp 
                        = lTimeStamp 
                        - arm_2d_helper_convert_ms_to_ticks(nElapsedTime);
                    break;

                default:
                    break;
            }
        } else {
            switch (this.Switch.chState) {
                case START:
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.iOffset = iFullLength;
                    this.Switch.chState = MOVE;
                    break;
                case MOVE:
                    nElapsedTime = (iFullLength - this.Switch.iOffset) * this.Switch.hwPeriod / iFullLength;
                    this.Switch.lTimeStamp 
                        = lTimeStamp 
                        - arm_2d_helper_convert_ms_to_ticks(nElapsedTime);
                    break;

                default:
                    break;
            }
        
        }
    }
}

/*----------------------------------------------------------------------------*
 * Switch Mode: Erase                                                         *
 *----------------------------------------------------------------------------*/

static 
void __draw_scene_default(arm_2d_scene_player_t *ptThis,
                        arm_2d_scene_t *ptScene,
                        const arm_2d_tile_t *ptTile,
                        bool bIsNewFrame,
                        bool bIgnoreBG,
                        bool bIgnoreScene,
                        bool bIgnoreCavas)
{
    ARM_2D_UNUSED(ptThis);
    bool bCanvasFilled = false;

    if (NULL != ptScene) {
        bIgnoreBG = ptScene->bOnSwitchingIgnoreBG && bIgnoreBG;
        bIgnoreScene = ptScene->bOnSwitchingIgnoreScene && bIgnoreScene;
    }

    if (!bIgnoreBG) {
        if (bIsNewFrame) {
            ARM_2D_INVOKE_RT_VOID( ptScene->fnOnBGStart, ptScene);
        }

        if (!bIgnoreCavas) {
            /* fill canvas */
            arm_2d_helper_fill_tile_colour( ptTile, 
                                            ptTile->tInfo.tColourInfo,
                                            ptScene->tCanvas);
        }
        ARM_2D_INVOKE( ptScene->fnBackground, ptScene, ptTile, bIsNewFrame);
    }
    if (!bIgnoreScene) {
        if (bIsNewFrame) {
            ARM_2D_INVOKE_RT_VOID( ptScene->fnOnFrameStart, ptScene);
        }

        if (!bCanvasFilled && !bIgnoreCavas) {
            /* fill canvas */
            arm_2d_helper_fill_tile_colour( ptTile, 
                                            ptTile->tInfo.tColourInfo,
                                            ptScene->tCanvas);
        }

        ARM_2D_INVOKE( ptScene->fnScene, ptScene, ptTile, bIsNewFrame);
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

    this.Runtime.bCallOldSceneFrameCPL = false;
    this.Runtime.bCallNewSceneFrameCPL = false;

    this.Runtime.bCallOldSceneBGCPL = false;
    this.Runtime.bCallNewSceneBGCPL = false;

    /* internal statemachine */
    if (bIsNewFrame) {
        int32_t nElapsed;
        int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();

        int16_t iTouchOffset = 0;

        /* update full length */
        this.Switch.iFullLength = iTargetDistance;

        /* calculate iTouchOffset */
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_CFG_ERASE_LEFT:
                if (this.Switch.tTouchPointer.iX < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.iFullLength - this.Switch.tTouchPointer.iX;
                }
                break;
            case ARM_2D_SCENE_SWITCH_CFG_ERASE_RIGHT:
                if (this.Switch.tTouchPointer.iX < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.tTouchPointer.iX;
                }
                break;
            case ARM_2D_SCENE_SWITCH_CFG_ERASE_UP:
                if (this.Switch.tTouchPointer.iY < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.iFullLength - this.Switch.tTouchPointer.iY;
                }
                break;
            case ARM_2D_SCENE_SWITCH_CFG_ERASE_DOWN:
                if (this.Switch.tTouchPointer.iY < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.tTouchPointer.iY;
                }
                break;
            default:
                assert(false);      /* this should not happen */
        }

        iTouchOffset = MIN(this.Switch.iFullLength, iTouchOffset);
        iTouchOffset = MAX(0, iTouchOffset);

        this.Runtime.bManualSwitch = this.Runtime.bManualSwitchReq;
        this.Runtime.bFinishManualSwitch = this.Runtime.bFinishManualSwitchReq;
        this.Runtime.bCancelSwitch = this.Runtime.bCancelSwitchReq;

        arm_2d_scene_player_switch_status_t tStatus = arm_2d_scene_player_get_switching_status(ptThis);

        if (ARM_2D_SCENE_SWITCH_STATUS_MANUAL == tStatus) {

            switch (this.Switch.chState) {
                case START:
                    this.Switch.iOffset = 0;
                    this.Switch.chState++;
                    //break;
                case ERASE:
                    this.Switch.iOffset = iTouchOffset;
                    break;
            }

        } else if ( (ARM_2D_SCENE_SWITCH_STATUS_MANUAL_AUTO_CPL == tStatus)
                ||  (ARM_2D_SCENE_SWITCH_STATUS_AUTO == tStatus)) {

            switch (this.Switch.chState) {
                case START:
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.iOffset = 0;

                    this.Switch.chState++;
                    //break;
                case ERASE:
                    nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                    
                    this.Switch.iOffset = ((int64_t)iTargetDistance * (int64_t)nElapsed 
                            / arm_2d_helper_convert_ms_to_ticks(this.Switch.hwPeriod));
                    this.Switch.iOffset = MIN(iTargetDistance, this.Switch.iOffset);

                    if (this.Switch.iOffset >= iTargetDistance) {
                        this.Runtime.bSwitchCPL = true;
                        SCENE_SWITCH_RESET_FSM();
                    }
                    break;
            }
        } else if (ARM_2D_SCENE_SWITCH_STATUS_MANUAL_CANCEL == tStatus) {
            switch (this.Switch.chState) {
                case START:
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.iOffset = iTargetDistance;

                    this.Switch.chState++;
                    //break;
                case ERASE:
                    nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                    
                    this.Switch.iOffset = ((int64_t)iTargetDistance * (int64_t)nElapsed 
                            / arm_2d_helper_convert_ms_to_ticks(this.Switch.hwPeriod));
                    this.Switch.iOffset = iTargetDistance - MIN(iTargetDistance, this.Switch.iOffset);
                    this.Switch.iOffset = MAX(0, this.Switch.iOffset);

                    if (this.Switch.iOffset <= 0) {
                        this.Runtime.bSwitchCPL = true;
                        SCENE_SWITCH_RESET_FSM();
                    }
                    break;
            }
        }
    }

    iOffset = this.Switch.iOffset;
    
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

            this.Runtime.bCallOldSceneFrameCPL = true;
            this.Runtime.bCallOldSceneBGCPL = !this.Switch.tConfig.Feature.bIgnoreOldSceneBG;

            __draw_scene_default( ptThis, 
                                ptScene, 
                                &this.Switch.Erase.tSceneWindow, 
                                bIsNewFrame,
                                this.Switch.tConfig.Feature.bIgnoreOldSceneBG,
                                this.Switch.tConfig.Feature.bIgnoreOldScene,
                                false);
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

            this.Runtime.bCallNewSceneFrameCPL = true;
            this.Runtime.bCallNewSceneBGCPL = !this.Switch.tConfig.Feature.bIgnoreNewSceneBG;

            __draw_scene_default( ptThis, 
                                ptScene, 
                                &this.Switch.Erase.tSceneWindow, 
                                bIsNewFrame,
                                this.Switch.tConfig.Feature.bIgnoreNewSceneBG,
                                this.Switch.tConfig.Feature.bIgnoreNewScene,
                                false);
        }

    } while(0);

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Switch Mode: Slide                                                         *
 *----------------------------------------------------------------------------*/

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

    this.Runtime.bCallOldSceneFrameCPL = false;
    this.Runtime.bCallNewSceneFrameCPL = false;

    this.Runtime.bCallOldSceneBGCPL = false;
    this.Runtime.bCallNewSceneBGCPL = false;
    
    /* internal statemachine */
    if (bIsNewFrame) {
        int32_t nElapsed;
        int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();

        int16_t iTouchOffset = 0;

        /* update full length */
        this.Switch.iFullLength = iTargetDistance;

        /* calculate iTouchOffset */
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_LEFT:
                if (this.Switch.tTouchPointer.iX < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.iFullLength - this.Switch.tTouchPointer.iX;
                }
                break;
            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_RIGHT:
                if (this.Switch.tTouchPointer.iX < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.tTouchPointer.iX;
                }
                break;
            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_UP:
                if (this.Switch.tTouchPointer.iY < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.iFullLength - this.Switch.tTouchPointer.iY;
                }
                break;
            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_DOWN:
                if (this.Switch.tTouchPointer.iY < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.tTouchPointer.iY;
                }
                break;
            default:
                assert(false);      /* this should not happen */
        }

        iTouchOffset = MIN(this.Switch.iFullLength, iTouchOffset);
        iTouchOffset = MAX(0, iTouchOffset);

        this.Runtime.bManualSwitch = this.Runtime.bManualSwitchReq;
        this.Runtime.bFinishManualSwitch = this.Runtime.bFinishManualSwitchReq;
        this.Runtime.bCancelSwitch = this.Runtime.bCancelSwitchReq;

        arm_2d_scene_player_switch_status_t tStatus = arm_2d_scene_player_get_switching_status(ptThis);

        if (ARM_2D_SCENE_SWITCH_STATUS_MANUAL == tStatus) {

            switch (this.Switch.chState) {
                case START:
                    this.Switch.iOffset = 0;
                    this.Switch.chState++;
                    //break;
                case SLIDE:
                    this.Switch.iOffset = iTouchOffset;
                    break;
            }

        } else if ( (ARM_2D_SCENE_SWITCH_STATUS_MANUAL_AUTO_CPL == tStatus)
                ||  (ARM_2D_SCENE_SWITCH_STATUS_AUTO == tStatus)) {
            switch (this.Switch.chState) {
                case START:
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.iOffset = 0;

                    this.Switch.chState++;
                    //break;
                case SLIDE:
                    nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                    
                    this.Switch.iOffset = ((int64_t)iTargetDistance * (int64_t)nElapsed 
                            / arm_2d_helper_convert_ms_to_ticks(this.Switch.hwPeriod));
                    this.Switch.iOffset = MIN(iTargetDistance, this.Switch.iOffset);
                    this.Switch.iOffset = MAX(0, this.Switch.iOffset);

                    if (this.Switch.iOffset >= iTargetDistance) {
                        this.Runtime.bSwitchCPL = true;
                        SCENE_SWITCH_RESET_FSM();
                    }
                    break;
            }
        } else if (ARM_2D_SCENE_SWITCH_STATUS_MANUAL_CANCEL == tStatus) {
            switch (this.Switch.chState) {
                case START:
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.iOffset = iTargetDistance;

                    this.Switch.chState++;
                    //break;
                case SLIDE:
                    nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                    
                    this.Switch.iOffset = ((int64_t)iTargetDistance * (int64_t)nElapsed 
                            / arm_2d_helper_convert_ms_to_ticks( 
                                    this.Switch.hwPeriod));
                    this.Switch.iOffset = iTargetDistance - MIN(iTargetDistance, this.Switch.iOffset);
                    this.Switch.iOffset = MAX(0, this.Switch.iOffset);

                    if (this.Switch.iOffset <= 0) {
                        this.Runtime.bSwitchCPL = true;
                        SCENE_SWITCH_RESET_FSM();
                    }
                    break;
            }
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
                tWindow.tLocation.iX = -this.Switch.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;
            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_RIGHT:
                tWindow.tLocation.iX = this.Switch.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;
            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_UP:
                tWindow.tLocation.iY = -this.Switch.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_DOWN:
                tWindow.tLocation.iY = this.Switch.iOffset;
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
        
            this.Runtime.bCallOldSceneFrameCPL = true;
            this.Runtime.bCallOldSceneBGCPL = !this.Switch.tConfig.Feature.bIgnoreOldSceneBG;

            __draw_scene_default( ptThis, 
                                ptScene, 
                                &this.Switch.Slide.tSceneWindow, 
                                bIsNewFrame,
                                this.Switch.tConfig.Feature.bIgnoreOldSceneBG,
                                this.Switch.tConfig.Feature.bIgnoreOldScene,
                                false);
        }

        /* generate new tile for new scene */
        if (NULL == ptScene->ptNext) {
            break;
        }
        
        tWindow = (arm_2d_region_t){ .tSize = ptTile->tRegion.tSize };
        
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_LEFT:
                tWindow.tLocation.iX = iTargetDistance - this.Switch.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_RIGHT:
                tWindow.tLocation.iX = -(iTargetDistance - this.Switch.iOffset);
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_UP:
                tWindow.tLocation.iY = iTargetDistance - this.Switch.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.Slide.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_SLIDE_DOWN:
                tWindow.tLocation.iY = -(iTargetDistance - this.Switch.iOffset);
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

            this.Runtime.bCallNewSceneFrameCPL = true;
            this.Runtime.bCallNewSceneBGCPL = !this.Switch.tConfig.Feature.bIgnoreNewSceneBG;

            __draw_scene_default( ptThis, 
                                ptScene, 
                                &this.Switch.Slide.tSceneWindow, 
                                bIsNewFrame,
                                this.Switch.tConfig.Feature.bIgnoreNewSceneBG,
                                this.Switch.tConfig.Feature.bIgnoreNewScene,
                                false);
        }
        
    } while(0);

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Switch Mode: Fly In                                                         *
 *----------------------------------------------------------------------------*/

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mode_fly_in)
{
    enum {
        START = 0,
        SLIDE,
    };

    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    arm_2d_scene_t *ptScene = NULL;
    
    int16_t iTargetDistance = 0;
    
    switch(this.Switch.tConfig.Feature.chMode) {
        case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_LEFT:
        case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_RIGHT:
            iTargetDistance = ptTile->tRegion.tSize.iWidth;
            break;
        case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_TOP:
        case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_BOTTOM:
            iTargetDistance = ptTile->tRegion.tSize.iHeight;
            break;
        default:
            assert(false);      /* this should not happen */
    }

    this.Runtime.bCallOldSceneFrameCPL = false;
    this.Runtime.bCallNewSceneFrameCPL = false;

    this.Runtime.bCallOldSceneBGCPL = false;
    this.Runtime.bCallNewSceneBGCPL = false;
    
    /* internal statemachine */
    if (bIsNewFrame) {
        int32_t nElapsed;
        int64_t lTimeStamp = arm_2d_helper_get_system_timestamp();

        int16_t iTouchOffset = 0;

        /* update full length */
        this.Switch.iFullLength = iTargetDistance;

        /* calculate iTouchOffset */
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_LEFT:
                if (this.Switch.tTouchPointer.iX < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.iFullLength - this.Switch.tTouchPointer.iX;
                }
                break;
            case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_RIGHT:
                if (this.Switch.tTouchPointer.iX < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.tTouchPointer.iX;
                }
                break;
            case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_TOP:
                if (this.Switch.tTouchPointer.iY < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.iFullLength - this.Switch.tTouchPointer.iY;
                }
                break;
            case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_BOTTOM:
                if (this.Switch.tTouchPointer.iY < 0) {
                    iTouchOffset = 0;
                } else {
                    iTouchOffset = this.Switch.tTouchPointer.iY;
                }
                break;
            default:
                assert(false);      /* this should not happen */
        }

        iTouchOffset = MIN(this.Switch.iFullLength, iTouchOffset);
        iTouchOffset = MAX(0, iTouchOffset);

        this.Runtime.bManualSwitch = this.Runtime.bManualSwitchReq;
        this.Runtime.bFinishManualSwitch = this.Runtime.bFinishManualSwitchReq;
        this.Runtime.bCancelSwitch = this.Runtime.bCancelSwitchReq;

        arm_2d_scene_player_switch_status_t tStatus = arm_2d_scene_player_get_switching_status(ptThis);

        if (ARM_2D_SCENE_SWITCH_STATUS_MANUAL == tStatus) {

            switch (this.Switch.chState) {
                case START:
                    this.Switch.iOffset = 0;
                    this.Switch.chState++;
                    //break;
                case SLIDE:
                    this.Switch.iOffset = iTouchOffset;
                    break;
            }

        } else if ( (ARM_2D_SCENE_SWITCH_STATUS_MANUAL_AUTO_CPL == tStatus)
                ||  (ARM_2D_SCENE_SWITCH_STATUS_AUTO == tStatus)) {
            switch (this.Switch.chState) {
                case START:
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.iOffset = 0;

                    this.Switch.chState++;
                    //break;
                case SLIDE:
                    nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                    
                    this.Switch.iOffset = ((int64_t)iTargetDistance * (int64_t)nElapsed 
                            / arm_2d_helper_convert_ms_to_ticks(this.Switch.hwPeriod));
                    this.Switch.iOffset = MIN(iTargetDistance, this.Switch.iOffset);
                    this.Switch.iOffset = MAX(0, this.Switch.iOffset);

                    if (this.Switch.iOffset >= iTargetDistance) {
                        this.Runtime.bSwitchCPL = true;
                        SCENE_SWITCH_RESET_FSM();
                    }
                    break;
            }
        } else if (ARM_2D_SCENE_SWITCH_STATUS_MANUAL_CANCEL == tStatus) {
            switch (this.Switch.chState) {
                case START:
                    this.Switch.lTimeStamp = lTimeStamp;
                    this.Switch.iOffset = iTargetDistance;

                    this.Switch.chState++;
                    //break;
                case SLIDE:
                    nElapsed = (int32_t)( lTimeStamp - this.Switch.lTimeStamp);
                    
                    this.Switch.iOffset = ((int64_t)iTargetDistance * (int64_t)nElapsed 
                            / arm_2d_helper_convert_ms_to_ticks( 
                                    this.Switch.hwPeriod));
                    this.Switch.iOffset = iTargetDistance - MIN(iTargetDistance, this.Switch.iOffset);
                    this.Switch.iOffset = MAX(0, this.Switch.iOffset);

                    if (this.Switch.iOffset <= 0) {
                        this.Runtime.bSwitchCPL = true;
                        SCENE_SWITCH_RESET_FSM();
                    }
                    break;
            }
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

        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead;

        this.Runtime.bCallOldSceneFrameCPL = true;
        this.Runtime.bCallOldSceneBGCPL = !this.Switch.tConfig.Feature.bIgnoreOldSceneBG;

        __draw_scene_default( ptThis, 
                            ptScene, 
                            //&this.Switch.FlyIn.tSceneWindow, 
                            ptTile,
                            bIsNewFrame,
                            this.Switch.tConfig.Feature.bIgnoreOldSceneBG,
                            this.Switch.tConfig.Feature.bIgnoreOldScene,
                            false);

        /* generate new tile for new scene */
        if (NULL == ptScene->ptNext) {
            break;
        }
        
        arm_2d_region_t tWindow = {
            .tSize = ptTile->tRegion.tSize,
        };
        
        switch(this.Switch.tConfig.Feature.chMode) {
            case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_LEFT:
                tWindow.tLocation.iX = iTargetDistance - this.Switch.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.FlyIn.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_RIGHT:
                tWindow.tLocation.iX = -(iTargetDistance - this.Switch.iOffset);
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.FlyIn.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_TOP:
                tWindow.tLocation.iY = iTargetDistance - this.Switch.iOffset;
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.FlyIn.tSceneWindow, 
                                                        false);
                break;

            case ARM_2D_SCENE_SWITCH_CFG_FLY_IN_FROM_BOTTOM:
                tWindow.tLocation.iY = -(iTargetDistance - this.Switch.iOffset);
                ptWindow = arm_2d_tile_generate_child(  ptTile, 
                                                        &tWindow, 
                                                        &this.Switch.FlyIn.tSceneWindow, 
                                                        false);
                break;
            default:
                assert(false);      /* this should not happen */
        }

        /* draw the old scene background */
        ptScene = this.SceneFIFO.ptHead->ptNext;
        
        if (ptWindow) {

            this.Runtime.bCallNewSceneFrameCPL = true;
            this.Runtime.bCallNewSceneBGCPL = !this.Switch.tConfig.Feature.bIgnoreNewSceneBG;

            uint8_t chOpacity = this.Switch.iOffset * 255 / this.Switch.iFullLength;

            arm_2d_fill_colour_with_opacity(ptTile, 
                                            NULL, 
                                            (__arm_2d_color_t){ptScene->tCanvas.wColour},
                                            chOpacity);

            __draw_scene_default( ptThis, 
                                ptScene, 
                                &this.Switch.FlyIn.tSceneWindow, 
                                bIsNewFrame,
                                this.Switch.tConfig.Feature.bIgnoreNewSceneBG,
                                this.Switch.tConfig.Feature.bIgnoreNewScene,
                                true);
        }
        
    } while(0);

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_background_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;
    arm_2d_scene_t *ptScene = this.SceneFIFO.ptHead;

    return ptScene->fnBackground(ptScene, ptTile, bIsNewFrame);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    arm_2d_scene_player_t *ptThis = (arm_2d_scene_player_t *)pTarget;

    arm_2d_scene_t *ptScene = this.SceneFIFO.ptHead;

    arm_fsm_rt_t tResult = ptScene->fnScene(ptScene, ptTile, bIsNewFrame);

    arm_2d_helper_dirty_region_update_dirty_regions(&ptScene->tDirtyRegionHelper, 
                                                    ptTile);

    return tResult;
}

/*-----------------------------------------------------------------------------*
 * Misc                                                                        *
 *-----------------------------------------------------------------------------*/
#define ARM_2D_USER_SCENE_PLAYER_TASK_RESET()                                   \
            do {this.Runtime.chState = START;} while(0)

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

ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_scene_player_task(arm_2d_scene_player_t *ptThis)
{
    assert(NULL != ptThis);

    arm_2d_scene_t *ptScene = this.SceneFIFO.ptHead;
    arm_fsm_rt_t tResult;
    
    
    
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

            if (!ptScene->bLoaded) {
                ptScene->bLoaded = true;
                ARM_2D_INVOKE_RT_VOID(ptScene->fnOnLoad, ptScene);
            }

            this.Runtime.bUpdateBG = true;
            this.Runtime.chState++;
            // fall-through

        case DRAW_FRAME_START:
            ARM_2D_INVOKE_RT_VOID(ptScene->fnOnFrameStart, ptScene);

            if (ptScene->bUseDirtyRegionHelper) {
                arm_2d_helper_dirty_region_on_frame_start(&ptScene->tDirtyRegionHelper);
            }

            if (!this.Runtime.bUpdateBG) {
                this.Runtime.chState = DRAW_SCENE_PREPARE;
                break;
            } else {
                this.Runtime.bUpdateBG = false;
                this.Runtime.chState = DRAW_BACKGROUND_PREPARE;
            }
            __arm_2d_helper_pfb_enable_drawing_canvas_colour(
                                            &this.use_as__arm_2d_helper_pfb_t,
                                            ptScene->tCanvas);

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
                        __pfb_draw_scene_handler,
                        ptThis);
                    this.Runtime.chState = DRAW_BACKGROUND;
                } else {
                    this.Runtime.chState = DRAW_SCENE_PREPARE;
                }
                break;
            } else {
                ARM_2D_INVOKE_RT_VOID(ptScene->fnOnBGStart, ptScene);
            
                ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(   
                    &this.use_as__arm_2d_helper_pfb_t,
                    __pfb_draw_scene_background_handler,
                    ptThis);
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
                __pfb_draw_scene_handler,
                ptThis);
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

                /* reset flags */
                this.Runtime.bFinishManualSwitchReq = false;
                this.Runtime.bFinishManualSwitch = false;
                this.Runtime.bCancelSwitchReq = false;
                this.Runtime.bCancelSwitch = false;
                
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

                __arm_2d_helper_pfb_disable_drawing_canvas_colour(
                                            &this.use_as__arm_2d_helper_pfb_t);

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
                this.Runtime.bCallOldSceneFrameCPL = false;
                this.Runtime.bCallNewSceneFrameCPL = false;
                this.Runtime.bCallOldSceneBGCPL = false;
                this.Runtime.bCallNewSceneBGCPL = false;
                this.Switch.tTouchPointer = (arm_2d_location_t){
                    -1, -1
                };

                this.Switch.chState = START;
                this.Runtime.chState = SWITCH_SCENE;

                if (NULL != this.SceneFIFO.ptHead->ptNext) {
                    if (!this.SceneFIFO.ptHead->ptNext->bLoaded) {
                        this.SceneFIFO.ptHead->ptNext->bLoaded = true;

                        ARM_2D_INVOKE_RT_VOID(this.SceneFIFO.ptHead->ptNext->fnOnLoad, this.SceneFIFO.ptHead->ptNext);
                    }
                }
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

            if (this.Runtime.bCallOldSceneBGCPL) {
                arm_2d_scene_t *ptTempScene = this.SceneFIFO.ptHead;
                assert(NULL != ptTempScene);
                ARM_2D_INVOKE_RT_VOID( ptTempScene->fnOnBGComplete, ptTempScene);
            }

            if (this.Runtime.bCallOldSceneFrameCPL) {
                arm_2d_scene_t *ptTempScene = this.SceneFIFO.ptHead;
                assert(NULL != ptTempScene);
                ARM_2D_INVOKE_RT_VOID( ptTempScene->fnOnFrameCPL, ptTempScene);
            }

            if (this.Runtime.bCallNewSceneBGCPL) {
                arm_2d_scene_t *ptTempScene = this.SceneFIFO.ptHead->ptNext;
                assert(NULL != ptTempScene);
                ARM_2D_INVOKE_RT_VOID( ptTempScene->fnOnBGComplete, ptTempScene);
            }

            if (this.Runtime.bCallNewSceneFrameCPL) {
                arm_2d_scene_t *ptTempScene = this.SceneFIFO.ptHead->ptNext;
                assert(NULL != ptTempScene);
                ARM_2D_INVOKE_RT_VOID( ptTempScene->fnOnFrameCPL, ptTempScene);
            }

            if (this.Runtime.bSwitchCPL) {
                this.Runtime.chState = SWITCH_SCENE_POST;
            }

            /* return arm_fsm_rt_cpl to indicate a end of a frame */
            return arm_fsm_rt_cpl;
        
        case SWITCH_SCENE_POST:
            if (this.Runtime.bFinishManualSwitch && this.Runtime.bCancelSwitch) {
                __arm_2d_scene_player_delete_all_next_scene(ptThis);
            } else {
                __arm_2d_scene_player_next_scene(ptThis);
            }

            /* reset flags */
            this.Runtime.bFinishManualSwitchReq = false;
            this.Runtime.bFinishManualSwitch = false;
            this.Runtime.bCancelSwitchReq = false;
            this.Runtime.bCancelSwitch = false;

            ARM_2D_USER_SCENE_PLAYER_TASK_RESET();
            break;
    }
    
    return arm_fsm_rt_on_going;
}

ARM_NONNULL(1)
bool arm_2d_scene_player_is_switching(arm_2d_scene_player_t *ptThis)
{
    if (NULL == ptThis) {
        assert(false);
        return false;
    }

    return (this.Runtime.chState == SWITCH_SCENE);
}

ARM_NONNULL(1,2)
bool arm_2d_scene_player_append_dirty_regions(arm_2d_scene_t *ptScene, 
                                              arm_2d_region_list_item_t *ptItems,
                                              size_t tCount)
{
    if (NULL == ptItems || 0 == tCount || NULL == ptScene) {
        return false;
    }

    return arm_2d_helper_pfb_append_dirty_regions_to_list(
                &ptScene->ptDirtyRegion,
                ptItems,
                tCount);
}

ARM_NONNULL(1,2)
bool arm_2d_scene_player_remove_dirty_regions(arm_2d_scene_t *ptScene, 
                                              arm_2d_region_list_item_t *ptItems,
                                              size_t tCount)
{
    if (NULL == ptItems || 0 == tCount || NULL == ptScene) {
        return false;
    }
    return arm_2d_helper_pfb_remove_dirty_regions_from_list(
                &ptScene->ptDirtyRegion,
                ptItems,
                tCount);
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

/*-----------------------------------------------------------------------------*
 * Dynamic Dirty Region Helper Service                                         *
 *-----------------------------------------------------------------------------*/

arm_2d_region_list_item_t *arm_2d_scene_player_dynamic_dirty_region_init(
                                arm_2d_region_list_item_t *ptThis,
                                arm_2d_scene_t *ptScene)
{
    ptThis = arm_2d_dynamic_dirty_region_init(ptThis);

    if (NULL != ptThis && NULL != ptScene) {
        /* add the dirty region to the given scene */
        arm_2d_scene_player_append_dirty_regions(ptScene, ptThis, 1);
    }

    return ptThis;
}

ARM_NONNULL(1)
void arm_2d_scene_player_dynamic_dirty_region_depose(
                                arm_2d_region_list_item_t *ptThis,
                                arm_2d_scene_t *ptScene)
{
    assert(NULL != ptThis);

    if (NULL == ptThis) {
        return ;
    }

    if (NULL != ptScene) {
        /* unregister the dirty region from the target scene*/
        arm_2d_scene_player_remove_dirty_regions(ptScene, ptThis, 1);
    }

    arm_2d_dynamic_dirty_region_depose(ptThis);
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif
