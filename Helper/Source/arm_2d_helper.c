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
 * Title:        #include "arm_2d_helper.h"
 * Description:  The source code for arm-2d helper utilities
 *
 * $Date:        30. April 2025
 * $Revision:    V.2.3.2
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#define __ARM_2D_HELPER_COMMON_IMPLEMENT__
#define __ARM_2D_HELPER_IMPLEMENT__
#include "arm_2d_helper.h"

#if defined(__PERF_COUNTER__)
#   include "perf_counter.h"
#endif

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
#   pragma clang diagnostic ignored "-Wunreachable-code-break"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wunused-value"
#   pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif

/*============================ MACROS ========================================*/
#undef this
#define this        (*ptThis)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
static struct {
    bool bInitialized;
    uint32_t wMSUnit;

#if __ARM_2D_HAS_ASYNC__
    struct {
        uintptr_t semResourceAvailable;
        uintptr_t semTaskAvailable;
    }Async;
#endif
    int64_t lTimestampInit;
} s_tHelper = {
    .wMSUnit = 1,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
uintptr_t arm_2d_port_new_semaphore(void);

extern
void arm_2d_helper_rtos_init(void);

/*============================ IMPLEMENTATION ================================*/

__STATIC_INLINE
uint8_t __convert_2digits_bdc(const char *pchBCD)
{
    uint8_t chResult = 0;
    uint8_t chChar = pchBCD[0];
    if (chChar >= '0' && chChar <= '9') {
        chResult = (chChar - '0') * 10;
    }
    
    chChar = pchBCD[1];
    if (chChar >= '0' && chChar <= '9') {
        chResult += (chChar - '0');
    }
    
    return chResult;
}

void arm_2d_helper_init(void)
{

    bool bInitialized = true;
    arm_irq_safe {
        bInitialized = s_tHelper.bInitialized;
        s_tHelper.bInitialized = true;
    }
    
    if (bInitialized) {
        return ;
    }

    s_tHelper.wMSUnit = arm_2d_helper_get_reference_clock_frequency() / 1000ul;
    if (s_tHelper.wMSUnit == 0) {
        s_tHelper.wMSUnit = 1;
    }
#if __ARM_2D_HAS_ASYNC__
    /*! \note create a event flag and attach it to the default OP */
    arm_2d_op_attach_semaphore(NULL, arm_2d_port_new_semaphore());
    
    s_tHelper.Async.semResourceAvailable = arm_2d_port_new_semaphore();
    s_tHelper.Async.semTaskAvailable = arm_2d_port_new_semaphore();
    
    arm_2d_helper_rtos_init();
#endif

    arm_2d_helper_built_in_init();
    
    do {
        const char c_chStartTime[] = {__TIME__};
        uint8_t chHours = __convert_2digits_bdc(&c_chStartTime[0]);
        uint8_t chMinus = __convert_2digits_bdc(&c_chStartTime[3]);
        uint8_t chSeconds = __convert_2digits_bdc(&c_chStartTime[6]);

        int64_t lTimestampInit = chSeconds * 1000ul;
        lTimestampInit += chMinus * 60ul * 1000ul;
        lTimestampInit += chHours * 3600 * 1000ul;
    
        s_tHelper.lTimestampInit = arm_2d_helper_convert_ms_to_ticks(lTimestampInit);
    
    } while(0);

}

/* NOTE: for non-arm architecture, you have to implement those functions.
 */
#if __IS_SUPPORTED_ARM_ARCH__
__WEAK int64_t arm_2d_helper_get_system_timestamp(void)
{
    int64_t iOriginTimestamp = 
#if defined(__PERF_COUNTER__)
    get_system_ticks();
#else
    0;
#endif

    return iOriginTimestamp + s_tHelper.lTimestampInit;
}

__WEAK 
uint32_t arm_2d_helper_get_reference_clock_frequency(void)
{
#if defined(__PERF_COUNTER__) && __PER_COUNTER_VER__ >= 20300
    extern uint32_t perfc_port_get_system_timer_freq(void);
    return perfc_port_get_system_timer_freq();
#else
    extern uint32_t SystemCoreClock;
    return SystemCoreClock;
#endif
}
#endif

int64_t arm_2d_helper_convert_ticks_to_ms(int64_t lTick)
{
    return lTick / (int64_t)s_tHelper.wMSUnit;
}

int64_t arm_2d_helper_convert_ms_to_ticks(uint32_t wMS)
{
    int64_t lResult = (int64_t)s_tHelper.wMSUnit * (int64_t)wMS;
    return lResult ? lResult : 1;
}


ARM_NONNULL(2)
bool __arm_2d_helper_is_time_out(int64_t lPeriod, int64_t *plTimestamp)
{
    assert(NULL != plTimestamp);
    
    int64_t lTimestamp = arm_2d_helper_get_system_timestamp();


    if (0 == *plTimestamp) {
        *plTimestamp = lPeriod;
        *plTimestamp += lTimestamp;
        
        return false;
    }

    if (lTimestamp >= *plTimestamp) {
        *plTimestamp = lPeriod + lTimestamp;
        return true;
    }

    return false;
}

ARM_NONNULL(1)
int64_t __arm_2d_helper_time_elapsed(int64_t *plTimestamp)
{
    if (NULL == plTimestamp) {
        return 0;
    };
    return arm_2d_helper_get_system_timestamp() - *plTimestamp;
}

ARM_NONNULL(4,5)
bool __arm_2d_helper_time_liner_slider( int32_t nFrom, 
                                        int32_t nTo, 
                                        int64_t lPeriod,
                                        int32_t *pnStroke,
                                        int64_t *plTimestamp)
{
    assert(NULL != plTimestamp);
    assert(NULL != pnStroke);

    int64_t lTimestamp = arm_2d_helper_get_system_timestamp();

    if (nFrom == nTo) {
        *pnStroke = nTo;
        return true;
    } else {
        if (0 == *plTimestamp) {
            *plTimestamp = lTimestamp;
            (*pnStroke) = nFrom;
        } else {
            /* code for update this.Runtime.iOffset */
            int64_t lElapsed = (lTimestamp - *plTimestamp);
            
            int32_t iDelta = 0;
            if (lElapsed < lPeriod) {
                iDelta = nTo - nFrom;
                iDelta = (int32_t)(lElapsed * (int64_t)iDelta / lPeriod);

                //if (NULL != pnStroke) {
                    (*pnStroke) = nFrom + iDelta;
                //}
            } else {
                /* timeout */
                //if (NULL != pnStroke) {
                    (*pnStroke) = nTo;
                //}
                //*plTimestamp = lTimestamp;
                
                return true;
            }
        }
    }
    
    return false;
}


ARM_NONNULL(5,6)
bool __arm_2d_helper_time_cos_slider(   int32_t nFrom, 
                                        int32_t nTo, 
                                        int64_t lPeriod,
                                        float fPhase,
                                        int32_t *pnStroke,
                                        int64_t *plTimestamp)
{
    int64_t lTimestamp = arm_2d_helper_get_system_timestamp();
    assert(NULL != plTimestamp);
    assert(NULL != pnStroke);

    if (nFrom == nTo) {
        *pnStroke = nTo;
        return true;
    } else {
        if (0 == *plTimestamp) {
            *plTimestamp = lTimestamp;
        }  
        do {
            /* code for update this.Runtime.iOffset */
            int64_t lElapsed = (lTimestamp - *plTimestamp);
            
            int32_t iDelta = 0;
            if (lElapsed < lPeriod) {
                iDelta = nTo - nFrom;
                
                float fDegree = (float)(((float)lElapsed / (float)lPeriod) * 6.2831852f);
                iDelta = (int32_t)((1.0001f-arm_cos_f32(fDegree + fPhase)) * (float)iDelta);
                iDelta >>= 1;
                
                //if (NULL != pnStroke) {
                    (*pnStroke) = nFrom + iDelta;
                //}
            } else {
                /* timeout */
                iDelta = nTo - nFrom;
                
                float fDegree = 6.2831852f;
                iDelta = (int32_t)((1.0001f - arm_cos_f32(fDegree + fPhase)) * (float)iDelta);
                iDelta >>= 1;
                //if (NULL != pnStroke) {
                    (*pnStroke) = nFrom + iDelta;
                //}
                *plTimestamp = lTimestamp;
                
                return true;
            }
        } while(0);
    }
    
    return false;
}

uint32_t __arm_2d_helper_colour_slider( uint32_t wFrom, 
                                        uint32_t wTo,
                                        int32_t nDistance,
                                        int32_t nOffset)
{
    assert(nDistance > 0);

    if (nOffset >= nDistance) {
        return wTo;
    }
    if (nOffset <= 0) {
        return wFrom;
    }

    uint32_t wResult = 0;
    
    uint8_t *pchChannelFrom = (uint8_t *)&wFrom;
    uint8_t *pchChannelTo = (uint8_t *)&wTo;
    uint8_t *pchResult = (uint8_t *)&wResult;
    
    for (int n = 0; n < 4; n++ ) {
        int_fast16_t iFrom = pchChannelFrom[n];
        int_fast16_t iTo = pchChannelTo[n];
        
        int_fast16_t iDelta = iTo - iFrom;
        
        int32_t nResult = iDelta * nOffset / nDistance;
        
        pchResult[n] = (uint8_t)(nResult + iFrom);
    }
    
    return wResult;
}


/*!
 * \brief calculate the stroke of a cosine slider (0~pi) based on time
 *
 * \param[in] nFrom the start of the slider
 * \param[in] nTo the end of the slider
 * \param[in] lPeriod a given period in which the slider should finish the whole
 *            stroke
 * \param[out] pnStroke the address of an int32_t stroke variable
 * \param[in] plTimestamp the address of a timestamp variable, if you pass NULL
 *            the code that call this funtion will not be reentrant.
 * \retval true the slider has finished the whole stroke
 * \retval false the slider hasn't reach the target end
 */
ARM_NONNULL(4,5)
bool __arm_2d_helper_time_half_cos_slider(  int32_t nFrom, 
                                            int32_t nTo, 
                                            int64_t lPeriod,
                                            int32_t *pnStroke,
                                            int64_t *plTimestamp)
{
    
    int64_t lTimestamp = arm_2d_helper_get_system_timestamp();
    assert(NULL != plTimestamp);
    assert(NULL != pnStroke);

    if (nFrom == nTo) {
        *pnStroke = nTo;
        return true;
    } else {
        if (0 == *plTimestamp) {
            *plTimestamp = lTimestamp;
            (*pnStroke) = nFrom;
        } else {
            /* code for update this.Runtime.iOffset */
            int64_t lElapsed = (lTimestamp - *plTimestamp);
            
            int32_t iDelta = 0;
            if (lElapsed < lPeriod) {
                iDelta = nTo - nFrom;

                float fDegree = (float)(((float)lElapsed / (float)lPeriod) * 3.1415926f);
                iDelta = (int32_t)((float)(1.0f-arm_cos_f32(fDegree)) * (float)iDelta);
                iDelta >>= 1;
                
                //if (NULL != pnStroke) {
                    (*pnStroke) = nFrom + iDelta;
                //}
            } else {
                /* timeout */
                //if (NULL != pnStroke) {
                    (*pnStroke) = nTo;
                //}
                //*plTimestamp = lTimestamp;
                
                return true;
            }
        }
    }
    
    return false;
}


ARM_NONNULL(1,2)
arm_2d_helper_pi_slider_t *arm_2d_helper_pi_slider_init(  
                                    arm_2d_helper_pi_slider_t *ptThis, 
                                    arm_2d_helper_pi_slider_cfg_t *ptCFG, 
                                    int16_t iStartPosition)
{
    assert(NULL != ptThis);

    /* the default parameter for PI control*/
    arm_2d_helper_pi_slider_cfg_t tCFG = {
        .fProportion = 0.25f,
        .fIntegration = 0.10f,
        .nInterval = 20,
    };

    if (NULL == ptCFG) {
        ptCFG = &tCFG;
    }

    memset(ptThis, 0, sizeof(arm_2d_helper_pi_slider_t));

    this.nInterval = arm_2d_helper_convert_ms_to_ticks(ptCFG->nInterval);

    this.q16Current = reinterpret_q16_s16(iStartPosition);
    this.q16Proportion = reinterpret_q16_f32(ptCFG->fProportion);
    this.q16Integration = reinterpret_q16_f32(ptCFG->fIntegration);

    return ptThis;
}

ARM_NONNULL( 1, 3 )
bool arm_2d_helper_pi_slider(arm_2d_helper_pi_slider_t *ptThis,
                             int16_t iTargetPosition,
                             int16_t *piResult)
{
    int64_t lTimestamp = arm_2d_helper_get_system_timestamp();
    assert( NULL != ptThis );
    assert( NULL != piResult );
    bool bResult = false;

    if ( 0 == this.lTimestamp ) { //first entry init
        this.lTimestamp = lTimestamp;
        this.q16OP = 0;
    }

    do {
        int64_t lElapsed = ( lTimestamp - this.lTimestamp ) + this.nTimeResidual;
        this.lTimestamp = lTimestamp;

        /* perform iterations */
        while ( lElapsed >= this.nInterval ) {
            lElapsed -= this.nInterval;

            q16_t q16Error = reinterpret_q16_s16(iTargetPosition) - this.q16Current;
            q16_t q16Prop = mul_q16(q16Error, this.q16Proportion);
            this.q16OP += mul_q16(q16Error, this.q16Integration);
            this.q16Current += (q16Prop + this.q16OP);

            q16_t q16StableCheck = abs_q16(q16Prop + this.q16OP);

        #if 0
            printf( "Error : %f,  Delta: %f \r\n", 
                    reinterpret_f32_q16(q16Error),
                    reinterpret_f32_q16(q16StableCheck)
                    );
        #endif

            if (    (q16StableCheck <= reinterpret_q16_f32(0.050f)) 
              &&    (abs_q16(q16Error) < reinterpret_q16_f32(1.0f)) ) {
                
                /* has reached the final value */
                this.q16Current = reinterpret_q16_s16(iTargetPosition); /* correct the residual error */
                this.q16OP = 0;
                bResult = true;
            }
        }
        this.nTimeResidual = (int32_t)lElapsed;
        *piResult = reinterpret_s16_q16(this.q16Current);

    } while( 0 );


    return bResult;
}

ARM_NONNULL(1)
void arm_2d_helper_pi_slider_set_current_f32(arm_2d_helper_pi_slider_t *ptThis,
                                            float fCurrent)
{
    assert( NULL != ptThis );
    this.q16Current = reinterpret_q16_f32(fCurrent);

    this.lTimestamp = arm_2d_helper_get_system_timestamp();
    this.q16OP = 0;
}

ARM_NONNULL(1)
void arm_2d_helper_pi_slider_set_current(arm_2d_helper_pi_slider_t *ptThis,
                                         int16_t iCurrent)
{
    assert( NULL != ptThis );
    this.q16Current = reinterpret_q16_s16(iCurrent);

    this.lTimestamp = arm_2d_helper_get_system_timestamp();
    this.q16OP = 0;
}

ARM_NONNULL( 1, 3 )
bool arm_2d_helper_pi_slider_f32(arm_2d_helper_pi_slider_t *ptThis,
                                 float fTargetPosition,
                                 float *pfResult)
{
    int64_t lTimestamp = arm_2d_helper_get_system_timestamp();
    assert( NULL != ptThis );
    assert( NULL != pfResult );
    bool bResult = false;


    if ( 0 == this.lTimestamp ) { //first entry init
        this.lTimestamp = lTimestamp;
        this.q16OP = 0;
    }

    do {
        int64_t lElapsed = ( lTimestamp - this.lTimestamp ) + this.nTimeResidual;
        this.lTimestamp = lTimestamp;

        /* perform iterations */
        while ( lElapsed >= this.nInterval ) {
            lElapsed -= this.nInterval;

            q16_t q16Error = reinterpret_q16_f32(fTargetPosition) - this.q16Current;
            q16_t q16Prop = mul_q16(q16Error, this.q16Proportion);
            this.q16OP += mul_q16(q16Error, this.q16Integration);
            this.q16Current += (q16Prop + this.q16OP);
            q16_t q16StableCheck = abs_q16(q16Prop + this.q16OP);
        
            if (    (q16StableCheck <= reinterpret_q16_f32(0.050f)) 
              &&    (abs_q16(q16Error) < reinterpret_q16_f32(1.0f)) ) {
                /* has reached the final value */
                this.q16Current = reinterpret_q16_f32(fTargetPosition); /* correct the residual error */
                this.q16OP = 0;

                bResult = true;
            }
        }
        this.nTimeResidual = (int32_t)lElapsed;
        //*pfResult = this.fCurrent;
        *pfResult = reinterpret_f32_q16(this.q16Current);

    } while( 0 );

    return bResult;
}

void arm_2d_helper_draw_box( const arm_2d_tile_t *ptTarget,
                             const arm_2d_region_t *ptRegion,
                             int16_t iBorderWidth, 
                             COLOUR_INT tColour,
                             uint8_t chOpacity)
{
    assert( NULL != ptTarget );
    
    arm_2d_region_t tTargetRegion = {.tSize = ptTarget->tRegion.tSize};
    if (NULL == ptRegion) {
        ptRegion = &tTargetRegion;
    }

    arm_2d_region_t tDrawRegion = *ptRegion;
    
    tDrawRegion.tSize.iHeight = iBorderWidth;
    
    /* draw the top horizontal line */
    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    chOpacity);
    
    arm_2d_op_wait_async(NULL);
    
    tDrawRegion.tLocation.iY += ptRegion->tSize.iHeight - iBorderWidth;
    
    /* draw the bottom horizontal line */
    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    chOpacity);
    
    arm_2d_op_wait_async(NULL);
    
    tDrawRegion = *ptRegion;
    
    /* draw left vertical line */
    tDrawRegion.tSize.iWidth = iBorderWidth;
    tDrawRegion.tLocation.iY += iBorderWidth;
    tDrawRegion.tSize.iHeight -= iBorderWidth * 2;

    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    chOpacity);
    
    arm_2d_op_wait_async(NULL);
    
    /* draw right vertical line */
    tDrawRegion.tLocation.iX += ptRegion->tSize.iWidth - iBorderWidth;
    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    chOpacity);
    
    arm_2d_op_wait_async(NULL);
}

ARM_NONNULL(1)
bool arm_2d_helper_film_next_frame(arm_2d_helper_film_t *ptThis)
{
    bool bReachTheEnd = false;
    assert(NULL != ptThis);
    
    arm_2d_tile_t *ptFrame = &this.use_as__arm_2d_tile_t;
                
    ptFrame->tRegion.tLocation.iX += ptFrame->tRegion.tSize.iWidth;
    if (ptFrame->tRegion.tLocation.iX >= ptFrame->tRegion.tSize.iWidth * this.hwColumn) {
        ptFrame->tRegion.tLocation.iX = 0;
        ptFrame->tRegion.tLocation.iY += ptFrame->tRegion.tSize.iHeight;
    }
    this.hwFrameIndex++;
    if (this.hwFrameIndex >= this.hwFrameNum) {
        this.hwFrameIndex = 0;
        bReachTheEnd = true;
        ptFrame->tRegion.tLocation.iX = 0;
        ptFrame->tRegion.tLocation.iY = 0;
    }

    return bReachTheEnd;
}

ARM_NONNULL(1)
void arm_2d_helper_film_reset(arm_2d_helper_film_t *ptThis)
{
    assert(NULL != ptThis);
    arm_2d_tile_t *ptFrame = &this.use_as__arm_2d_tile_t;

    ptFrame->tRegion.tLocation.iX = 0;
    ptFrame->tRegion.tLocation.iY = 0;
    this.hwFrameIndex = 0;
}


ARM_NONNULL(1)
void arm_2d_helper_film_set_frame(arm_2d_helper_film_t *ptThis, int32_t nIndex)
{
    assert(NULL != ptThis);
    arm_2d_tile_t *ptFrame = &this.use_as__arm_2d_tile_t;

    nIndex %= this.hwFrameNum;
    if (nIndex < 0) {
        nIndex += this.hwFrameNum;
    }

    this.hwFrameIndex = nIndex;
    ptFrame->tRegion.tLocation.iX 
        = (nIndex % this.hwColumn) * ptFrame->tRegion.tSize.iWidth;
    ptFrame->tRegion.tLocation.iY 
        = (nIndex / this.hwColumn) * ptFrame->tRegion.tSize.iHeight;
}

#if __ARM_2D_HELPER_CFG_LAYOUT_DEBUG_MODE__
ARM_NONNULL(1)
void __arm_2d_helper_layout_debug_print_label(const arm_2d_tile_t *ptTile, 
                                              arm_2d_region_t *ptRegion,
                                              const char *pchString)
{
    arm_lcd_text_set_font(
        (const arm_2d_font_t *)&ARM_2D_FONT_6x8); 

    arm_lcd_text_set_display_mode(
        ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR);

    arm_lcd_text_set_target_framebuffer(ptTile);

    if (NULL == ptRegion) {
        arm_2d_region_t tRegion = {
            .tSize = ptTile->tRegion.tSize,
        };
        arm_lcd_text_set_draw_region(&tRegion);
    } else {
        arm_lcd_text_set_draw_region(ptRegion);
    }

    arm_lcd_printf_label(ARM_2D_ALIGN_TOP_RIGHT, "CANVAS: %s", pchString);
    arm_lcd_text_set_display_mode(ARM_2D_DRW_PATN_MODE_COPY);
}
#endif

/*----------------------------------------------------------------------------*
 * RTOS Port                                                                  *
 *----------------------------------------------------------------------------*/

__WEAK
void arm_2d_helper_rtos_init(void)
{

}


__WEAK
uintptr_t arm_2d_port_new_semaphore(void)
{
    return (uintptr_t)NULL;
}

__WEAK
void arm_2d_port_free_semaphore(uintptr_t pSemaphore)
{
    ARM_2D_UNUSED(pSemaphore);
}


__WEAK
bool arm_2d_port_wait_for_semaphore(uintptr_t pSemaphore)
{
    ARM_2D_UNUSED(pSemaphore);
    return true;
}

__WEAK
void arm_2d_port_set_semaphore(uintptr_t pSemaphore)
{
    ARM_2D_UNUSED(pSemaphore);
}

#if __ARM_2D_HAS_ASYNC__
__OVERRIDE_WEAK
void arm_2d_notif_aync_op_cpl(uintptr_t pUserParam, uintptr_t pSemaphore)
{
    ARM_2D_UNUSED(pUserParam);
    arm_2d_port_set_semaphore(pSemaphore);
}

__OVERRIDE_WEAK
bool arm_2d_port_wait_for_async(uintptr_t pUserParam, uintptr_t pSemaphore)
{
    ARM_2D_UNUSED(pUserParam);
    return arm_2d_port_wait_for_semaphore(pSemaphore);
}


__OVERRIDE_WEAK
void arm_2d_notif_new_op_arrive(uintptr_t pUserParam)
{
    ARM_2D_UNUSED(pUserParam);
    arm_2d_port_set_semaphore(s_tHelper.Async.semTaskAvailable);
}


__OVERRIDE_WEAK 
void arm_2d_notif_aync_sub_task_cpl(uintptr_t pUserParam)
{
    ARM_2D_UNUSED(pUserParam);
    arm_2d_port_set_semaphore(s_tHelper.Async.semResourceAvailable);
}

__OVERRIDE_WEAK
arm_2d_op_core_t *arm_2d_op_init(arm_2d_op_core_t *ptOP, size_t tSize)
{
    do {
        if (NULL == ptOP) {
            break;
        }
        
        memset(ptOP, 0, MAX(tSize, sizeof(arm_2d_op_core_t)));
        arm_2d_op_attach_semaphore(ptOP, arm_2d_port_new_semaphore());
    
    } while(0);

    return ptOP;
}

__OVERRIDE_WEAK
arm_2d_op_core_t *arm_2d_op_depose(arm_2d_op_core_t *ptOP, size_t tSize)
{
    do {
        if (NULL == ptOP) {
            break;
        }
        
        memset(ptOP, 0, MAX(tSize, sizeof(arm_2d_op_core_t)));
        
        arm_2d_port_free_semaphore(arm_2d_op_get_semaphore(ptOP));
        
        arm_2d_op_attach_semaphore(ptOP, NULL);
    
    } while(0);

    return ptOP;
}

void arm_2d_helper_backend_task(void)
{
    arm_fsm_rt_t tTaskResult;
    arm_2d_task_t tTaskCB = {0};
    do {
        tTaskResult = arm_2d_task(&tTaskCB);

        if (tTaskResult < 0) {
            //! a serious error is detected
            assert(false);
            break;
        }/* else if (arm_fsm_rt_wait_for_obj == tTaskResult) {
            //! user low level drivers want to sync-up with this thread
        } */
        else if (arm_fsm_rt_wait_for_res == tTaskResult) {
            /* wait for on-going OP releasing resources */
            /* block current thread */
            while(!arm_2d_port_wait_for_semaphore(s_tHelper.Async.semResourceAvailable));

        } else if (arm_fsm_rt_cpl == tTaskResult) {
            /* block current thread */
            while(!arm_2d_port_wait_for_semaphore(s_tHelper.Async.semTaskAvailable));
        }

    } while(true);
}
#else
void arm_2d_helper_backend_task(void)
{
}
#endif


/*----------------------------------------------------------------------------*
 * FIFO Helper Service                                                        *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(1,2)
bool arm_2d_byte_fifo_init( arm_2d_byte_fifo_t *ptThis, 
                            void *pBuffer, 
                            uint16_t hwSize)
{
    bool bResult = false;

    do {
        if (NULL == ptThis) {
            break;
        }

        memset(ptThis, 0, sizeof(arm_2d_byte_fifo_t));

        if ((0 == hwSize) || (NULL == pBuffer)) {
            break;
        }

        this.pchBuffer = (uint8_t *)pBuffer;
        this.hwSize = hwSize;
        
        bResult = true;
    } while(0);

    return bResult;
}

ARM_NONNULL(1)
void arm_2d_byte_fifo_drop_all( arm_2d_byte_fifo_t *ptThis)
{
    assert(NULL != ptThis);

    arm_irq_safe {
        void *pBuffer = this.pchBuffer;
        size_t tSize = this.hwSize;

        memset(ptThis, 0, sizeof(arm_2d_byte_fifo_t));

        this.pchBuffer = (uint8_t *)pBuffer;
        this.hwSize = tSize;
    }
}


ARM_NONNULL(1)
bool arm_2d_byte_fifo_enqueue(arm_2d_byte_fifo_t *ptThis, uint8_t chChar)
{
    assert(NULL != ptThis);
    bool bResult = false;

    if (NULL == this.pchBuffer) {
        return false;
    }

    arm_irq_safe {
        do {
            if ((this.hwTail == this.tHead.hwPointer) 
            &&  (this.tHead.hwDataAvailable > 0)) {
                /* FIFO is FULL */
                break;
            }

            this.pchBuffer[this.hwTail++] = chChar;

            if (this.hwTail >= this.hwSize) {
                this.hwTail = 0;
            }
            this.tHead.hwDataAvailable++;
            this.tPeek.hwDataAvailable++;

            bResult = true;
        } while(0);
    }

    return bResult;
}

ARM_NONNULL(1)
bool arm_2d_byte_fifo_dequeue(arm_2d_byte_fifo_t *ptThis, uint8_t *pchChar)
{
    assert(NULL != ptThis);
    bool bResult = false;
    uint8_t chChar;

    if (NULL == this.pchBuffer) {
        return false;
    }

    arm_irq_safe {
        do {
            
            if ((this.tHead.hwPointer == this.hwTail) 
            &&  (this.tHead.hwDataAvailable == 0)) {
                /* FIFO is EMPTY */
                break;
            }

            chChar = this.pchBuffer[this.tHead.hwPointer++];

            if (this.tHead.hwPointer >= this.hwSize) {
                this.tHead.hwPointer = 0;
            }
            this.tHead.hwDataAvailable--;

            /* force to reset peek */
            this.tPeek = this.tHead;

            if (NULL != pchChar) {
                *pchChar = chChar;
            }

            bResult = true;
        } while(0);
    }

    return bResult;
}

ARM_NONNULL(1)
bool arm_2d_byte_fifo_peek( arm_2d_byte_fifo_t *ptThis, 
                            uint8_t *pchChar, 
                            bool bMovePointer)
{
    assert(NULL != ptThis);
    bool bResult = false;
    uint8_t chChar;

    if (NULL == this.pchBuffer) {
        return false;
    }

    arm_irq_safe {
        do {
            
            if ((this.tPeek.hwPointer == this.hwTail) 
            &&  (this.tPeek.hwDataAvailable == 0)) {
                /* Nothing left to peek */
                break;
            }

            chChar = this.pchBuffer[this.tPeek.hwPointer];

            if (bMovePointer) {
                if (++this.tPeek.hwPointer >= this.hwSize) {
                    this.tPeek.hwPointer = 0;
                }
                this.tPeek.hwDataAvailable--;
            }

            if (NULL != pchChar) {
                *pchChar = chChar;
            }

            bResult = true;
        } while(0);
    }

    return bResult;
}

ARM_NONNULL(1)
void arm_2d_byte_fifo_get_all_peeked(arm_2d_byte_fifo_t *ptThis)
{
    assert(NULL != ptThis);
    arm_irq_safe {
        this.tHead = this.tPeek;
    }
}

ARM_NONNULL(1)
void arm_2d_byte_fifo_reset_peeked(arm_2d_byte_fifo_t *ptThis)
{
    assert(NULL != ptThis);
    arm_irq_safe {
        this.tPeek = this.tHead;
    }
}

/*----------------------------------------------------------------------------*
 * Misc                                                                       *
 *----------------------------------------------------------------------------*/

__WEAK 
void arm_2d_helper_swap_rgb16(uint16_t *phwBuffer, uint32_t wCount)
{
    assert(NULL != phwBuffer);

    if (0 == wCount) {
        return ;
    }

    // aligned (2)
    assert((((uintptr_t) phwBuffer) & 0x01) == 0);

    // it is not aligned to 4
    if ((((uintptr_t) phwBuffer) & 0x03) == 0x02) {
        // handle the leading pixel
        uint32_t wTemp = *phwBuffer;
        *phwBuffer++ = (uint16_t)__REV16(wTemp);
        wCount--;
    }


    uint32_t wWords = wCount >> 1;
    uint32_t *pwBuffer = (uint32_t *)phwBuffer;
    wCount &= 0x01;

    if (wWords > 0) {
        do {
            uint32_t wTemp = *pwBuffer;
            *pwBuffer++ = __REV16(wTemp);
        } while(--wWords);
    }

    if (wCount) {
        uint32_t wTemp = *pwBuffer;
        (*(uint16_t *)pwBuffer) = (uint16_t)__REV16(wTemp);
    }
}


ARM_NONNULL(1)
void arm_2d_helper_fill_tile_colour(const arm_2d_tile_t *ptTile, 
                                    arm_2d_color_info_t tColourFormat,
                                    arm_2d_colour_t tColour)
{
    assert(NULL != ptTile);

    switch (tColourFormat.u3ColourSZ) {
        case ARM_2D_M_COLOUR_SZ_8BIT:
            arm_2d_c8bit_fill_colour(ptTile, NULL, tColour.chColour);
            break;
        case ARM_2D_M_COLOUR_SZ_16BIT:
            arm_2d_rgb16_fill_colour(ptTile, NULL, tColour.hwColour);
            break;
        case ARM_2D_M_COLOUR_SZ_32BIT:
            arm_2d_rgb32_fill_colour(ptTile, NULL, tColour.wColour);
            break;
        default:
            /* unsupported colour size */
            assert(false);
            break;
    }
    ARM_2D_OP_WAIT_ASYNC();
}

ARM_NONNULL(2)
void __arm_2d_layout_wrap_horizontal(   int16_t iTempX, 
                                        __arm_2d_layout_t *ptLayout)
{
    assert(NULL != ptLayout);
    
    if (ptLayout->tAlignTable.Horizontal.sAdvance > 0) {
        /* left to right */
        if (!(    iTempX                                              
            >=  (   ptLayout->tArea.tLocation.iX
                +   ptLayout->tArea.tSize.iWidth))) {
            return ;
        }
    } else {
        /* right to left */
        if (!(iTempX <= ptLayout->tArea.tLocation.iX)) {
            return ;
        }
    }

    /* move to the next line */
    ptLayout->tLayout.tLocation.iY 
        += ptLayout->tLayout.tSize.iHeight
         * ptLayout->tAlignTable.Vertical.sAdvance;

    /* reset the max line height */
    ptLayout->tLayout.tSize.iHeight = 0;

    ptLayout->tLayout.tLocation.iX = ptLayout->tArea.tLocation.iX 
                                   - (  ptLayout->tArea.tSize.iWidth 
                                     *  ptLayout->tAlignTable.Horizontal.sWidth);
}


ARM_NONNULL(2)
void __arm_2d_layout_wrap_vertical( int16_t iTempY, 
                                    __arm_2d_layout_t *ptLayout)
{
    assert(NULL != ptLayout);
    
    if (ptLayout->tAlignTable.Vertical.sAdvance > 0) {
        /* top down */
        if (!(    iTempY                                              
            >=  (   ptLayout->tArea.tLocation.iY
                +   ptLayout->tArea.tSize.iHeight))) {
            return ;
        }
    } else {
        /* bottom up */
        if (!(iTempY <= ptLayout->tArea.tLocation.iY)) {
            return ;
        }
    }

    /* move to the next line */
    ptLayout->tLayout.tLocation.iX 
        += ptLayout->tLayout.tSize.iWidth
         * ptLayout->tAlignTable.Horizontal.sAdvance;

    /* reset the max line width */
    ptLayout->tLayout.tSize.iWidth = 0;

    ptLayout->tLayout.tLocation.iY = ptLayout->tArea.tLocation.iY 
                                   - (  ptLayout->tArea.tSize.iHeight 
                                     *  ptLayout->tAlignTable.Vertical.sHeight);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif
