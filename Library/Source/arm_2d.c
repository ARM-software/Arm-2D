/*
 * Copyright (C) 2010-2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        arm-2d.c
 * Description:  Tables for pixel pipeline OPs
 *
 * $Date:        19. Nov 2023
 * $Revision:    V.1.3.1
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */


/*============================ INCLUDES ======================================*/
#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define __ARM_2D_COMPILATION_UNIT
#include "../Source/__arm_2d_core.c"

#define __ARM_2D_COMPILATION_UNIT
#include "../Source/__arm_2d_tile.c"

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif


#ifdef   __cplusplus
extern "C" {
#endif



/*============================ MACROS ========================================*/

#if !defined(__ARM_2D_HAS_ACI__) || !__ARM_2D_HAS_ACI__
#   define __arm_2d_aci_init()
#endif

#if !defined(__ARM_2D_HAS_HELIUM__) || !__ARM_2D_HAS_HELIUM__
#   define __arm_2d_helium_init()
#endif

#if !defined(__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__) || !__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__
#   define __arm_2d_sync_acc_init()
#endif

#if defined(__ARM_2D_HAS_HW_ACC__) && !__ARM_2D_HAS_HW_ACC__
#   define __arm_2d_async_acc_init()
#endif

#if defined(__ARM_2D_HAS_ASYNC__) && !__ARM_2D_HAS_ASYNC__
#   define __arm_2d_async_init(...)
#endif

#ifndef __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__
#   define __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__          4
#endif
#if __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__ < 4
#   warning The __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__ should be larger than or\
 equal to 4, set it to the default value 4.
#   undef __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__
#   define __ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__          4
#endif

#if defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__
#define __va_list    va_list
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if defined(__ARM_2D_HAS_ASYNC__) && __ARM_2D_HAS_ASYNC__
/*! 
 * \brief initialise the arm-2d pipeline
 * \param ptSubTasks an array of __arm_2d_sub_task_t objects
 * \param hwCount the number of items in the array
 * \return arm_2d_err_t error code
 */
extern
arm_2d_err_t __arm_2d_async_init(   __arm_2d_sub_task_t *ptSubTasks, 
                                    uint_fast16_t hwCount);
#endif

#if defined(__ARM_2D_HAS_HELIUM__) && __ARM_2D_HAS_HELIUM__
/*! 
 * \brief initialise the helium acceleration
 */
extern
void __arm_2d_helium_init(void);
#endif

#if defined(__ARM_2D_HAS_ACI__) && __ARM_2D_HAS_ACI__
/*! 
 * \brief initialise the ACI service
 */
extern
void __arm_2d_aci_init(void);
#endif

#if defined(__ARM_2D_HAS_HW_ACC__) && __ARM_2D_HAS_HW_ACC__
/*! 
 * \brief initialise the hardware (async) acceleration
 */
extern
void __arm_2d_async_acc_init(void);
#endif

#if defined(__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__) && __ARM_2D_HAS_TIGHTLY_COUPLED_ACC__
/*! 
 * \brief initialise the tightly-coupled (sync) acceleration
 */
extern
void __arm_2d_sync_acc_init(void);
#endif


/*! 
 *  \brief initialise the arm-2d core service
 */
extern
void __arm_2d_init(void);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/*! 
 * \brief initialise arm-2d
 */
void arm_2d_init(void)
{
    __arm_2d_init();      
    
    do {
        static __arm_2d_sub_task_t 
            s_tDefaultTaskPool[__ARM_2D_CFG_DEFAULT_SUB_TASK_POOL_SIZE__];

        ARM_2D_UNUSED(s_tDefaultTaskPool);

        __arm_2d_async_init(s_tDefaultTaskPool, dimof(s_tDefaultTaskPool));
    } while(0);

    __arm_2d_helium_init();                                             
    __arm_2d_aci_init();   
    __arm_2d_sync_acc_init();                                             
    __arm_2d_async_acc_init();                                                
}

__WEAK
void *__arm_2d_allocate_scratch_memory( uint32_t wSize, 
                                        uint_fast8_t nAlign,
                                        arm_2d_mem_type_t tType)
{
    ARM_2D_UNUSED(nAlign);
    ARM_2D_UNUSED(tType);

    /* ensure nAlign is 2^n */
    assert((((~nAlign) + 1) & nAlign) == nAlign);

    void *pBuff = malloc(wSize);
    assert(0 == ((uintptr_t)pBuff & (nAlign - 1)));
    
    return pBuff;
}

__WEAK
void __arm_2d_free_scratch_memory( arm_2d_mem_type_t tType,
                                   void *pBuff)
{
    ARM_2D_UNUSED(tType);

    free(pBuff);
}


typedef union arm_2d_log_chn_t {
    struct {
        uint32_t        u28ChannelMask  : 28;
        uint32_t        u4Type          : 4; 
    };
    uint32_t wValue;
} arm_2d_log_chn_t;


__WEAK
void __arm_2d_log_printf(int32_t nIndentLevel, 
                         uint32_t wChannelMask,
                         const char *pchPrefix,
                         const char *pchFormatString,
                         ...)
{
//    arm_2d_log_chn_t tChannelInfo = {
//        .wValue = wChannelMask,
//    };

    if ((__ARM_2D_LOG_CHANNEL_MASK_FILTER__ & wChannelMask) != wChannelMask) {
        return ;
    }

    uint32_t wMask = _BV(27);

    for (int32_t i = 0; i < 4; i++) {
        wMask <<= 1;
        if (!(wMask & wChannelMask)) {
            continue;
        }

        /* start a new line */
        __ARM_2D_PORT_PRINTF__("\r\n");

        for (int32_t n = 0; n < nIndentLevel; n++) {
            __ARM_2D_PORT_PRINTF__("\t");
        }

        if          (ARM_2D_LOG_CHN_TYPE_USER       == wMask) {
            __ARM_2D_PORT_PRINTF__("[USER]");
        } else if   (ARM_2D_LOG_CHN_TYPE_INFO       == wMask) {
        #if defined(__ARM_2D_CFG_LOG_OUTPUT_SUPPORT_COLOUR__) && __ARM_2D_CFG_LOG_OUTPUT_SUPPORT_COLOUR__
            __ARM_2D_PORT_PRINTF__(ARM_2D_TERMINAL_COLOUR_BRIGHT_BLACK "[INFO]" ARM_2D_TERMINAL_COLOUR_DEFAULT);
        #else
            __ARM_2D_PORT_PRINTF__("      ");
        #endif
        } else if   (ARM_2D_LOG_CHN_TYPE_WARNING    == wMask) {
            __ARM_2D_PORT_PRINTF__(ARM_2D_TERMINAL_COLOUR_YELLOW "[WARNING]" ARM_2D_TERMINAL_COLOUR_DEFAULT);
        } else if   (ARM_2D_LOG_CHN_TYPE_ERROR      == wMask) {
            __ARM_2D_PORT_PRINTF__(ARM_2D_TERMINAL_COLOUR_RED "[ERROR]" ARM_2D_TERMINAL_COLOUR_DEFAULT);
        }

        if (NULL != pchPrefix && strlen(pchPrefix) > 0) {
            __ARM_2D_PORT_PRINTF__("[%s]", pchPrefix);
        }

        int real_size;
        char *pchStringBuffer = __arm_2d_allocate_scratch_memory(__ARM_2D_LOG_MAX_STRING_LEN__, 1, ARM_2D_MEM_TYPE_FAST);
        
        if (NULL != pchStringBuffer) {
            __va_list ap;
            va_start(ap, pchFormatString);
                real_size = vsnprintf(pchStringBuffer, __ARM_2D_LOG_MAX_STRING_LEN__-1, pchFormatString, ap);
            va_end(ap);
            real_size = MIN(__ARM_2D_LOG_MAX_STRING_LEN__-1, real_size);
            pchStringBuffer[real_size] = '\0';

            __ARM_2D_PORT_PRINTF__("%s", pchStringBuffer);

            free(pchStringBuffer);
        } else {
            __ARM_2D_PORT_PRINTF__("[Insufficient memory for logging]%s", pchFormatString);
        }
    }

}


__WEAK
arm_2d_op_core_t *arm_2d_op_init(arm_2d_op_core_t *ptOP, size_t tSize)
{
    if (NULL != ptOP) {
        memset(ptOP, 0, MAX(tSize, sizeof(arm_2d_op_core_t)));
    }

    return ptOP;
}

__WEAK
arm_2d_op_core_t *arm_2d_op_depose(arm_2d_op_core_t *ptOP, size_t tSize)
{
    ARM_2D_UNUSED(tSize);

    return ptOP;
}

#ifdef   __cplusplus
}
#endif
