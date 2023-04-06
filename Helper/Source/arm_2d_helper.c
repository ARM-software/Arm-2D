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
 * $Date:        06. April 2023
 * $Revision:    V.1.5.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
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
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wunused-value"
#endif

/*============================ MACROS ========================================*/
#undef this
#define this        (*ptThis)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

static struct {
    bool bInitialized;
    uint32_t wMSUnit;

#if __ARM_2D_HAS_ASYNC__
    struct {
        arm_2d_task_t tTaskCB;
        uintptr_t semResourceAvailable;
        uintptr_t semTaskAvailable;
    }Async;
#endif
} s_tHelper = {
    .wMSUnit = 1,
};

/*============================ PROTOTYPES ====================================*/

extern
uintptr_t arm_2d_port_new_semaphore(void);

/*============================ IMPLEMENTATION ================================*/

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
#endif


}


__WEAK int64_t arm_2d_helper_get_system_timestamp(void)
{
#if defined(__PERF_COUNTER__)
    return get_system_ticks();
#else
    return (int64_t)clock();
#endif
}

__WEAK 
uint32_t arm_2d_helper_get_reference_clock_frequency(void)
{
    extern uint32_t SystemCoreClock;
    return SystemCoreClock;
}


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
                *plTimestamp = lTimestamp;
                
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
                iDelta = (int32_t)((1.0f-arm_cos_f32(fDegree + fPhase)) * (float)iDelta);
                iDelta >>= 1;
                
                //if (NULL != pnStroke) {
                    (*pnStroke) = nFrom + iDelta;
                //}
            } else {
                /* timeout */
                iDelta = nTo - nFrom;
                
                float fDegree = 6.2831852f;
                iDelta = (int32_t)((1.0f-arm_cos_f32(fDegree + fPhase)) * (float)iDelta);
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
                *plTimestamp = lTimestamp;
                
                return true;
            }
        }
    }
    
    return false;
}


/*----------------------------------------------------------------------------*
 * RTOS Port                                                                  *
 *----------------------------------------------------------------------------*/

#if __ARM_2D_HAS_ASYNC__
__WEAK
uintptr_t arm_2d_port_new_semaphore(void)
{
    return (uintptr_t)NULL;
}


__WEAK
bool arm_2d_port_wait_for_semaphore(uintptr_t pSemaphore)
{
    ARM_2D_UNUSED(pSemaphore);
    return true;
}

__WEAK
void arm_2d_port_set_semaphoret(uintptr_t pSemaphore)
{
    ARM_2D_UNUSED(pSemaphore);
}

__OVERRIDE_WEAK
void arm_2d_notif_aync_op_cpl(uintptr_t pUserParam, uintptr_t pSemaphore)
{
    ARM_2D_UNUSED(pUserParam);
    arm_2d_port_set_semaphoret(pSemaphore);
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
    arm_2d_port_set_semaphoret(s_tHelper.Async.semTaskAvailable);
}


__OVERRIDE_WEAK 
void arm_2d_notif_aync_sub_task_cpl(uintptr_t pUserParam)
{
    ARM_2D_UNUSED(pUserParam);
    assert (NULL != s_tHelper.Async.semResourceAvailable) ;
    arm_2d_port_set_semaphoret(s_tHelper.Async.semResourceAvailable);
}

void arm_2d_helper_backend_task(void)
{
    arm_fsm_rt_t tTaskResult;

    do {
        tTaskResult = arm_2d_task(&s_tHelper.Async.tTaskCB);

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

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif
