/****************************************************************************
*  Copyright 2021 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/
/*============================ INCLUDES ======================================*/
#include "cmsis_compiler.h"


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wmacro-redefined"
#endif

#ifndef __ENV_WRAPPER_H__
#   define __ENV_WRAPPER_H__        1

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "cmsis_compiler.h"

//! \name The macros to identify the compiler
//! @{

//! \note for IAR
#ifdef __IS_COMPILER_IAR__
#   undef __IS_COMPILER_IAR__
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#   define __IS_COMPILER_IAR__                  1
#endif

//! \note for arm compiler 5
#ifdef __IS_COMPILER_ARM_COMPILER_5__
#   undef __IS_COMPILER_ARM_COMPILER_5__
#endif
#if ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
#   define __IS_COMPILER_ARM_COMPILER_5__       1
#endif
//! @}

//! \note for arm compiler 6
#ifdef __IS_COMPILER_ARM_COMPILER_6__
#   undef __IS_COMPILER_ARM_COMPILER_6__
#endif
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#   define __IS_COMPILER_ARM_COMPILER_6__       1
#endif

#ifdef __IS_COMPILER_LLVM__
#   undef  __IS_COMPILER_LLVM__
#endif
#if defined(__clang__) && !__IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_LLVM__                 1
#else
//! \note for gcc
#   ifdef __IS_COMPILER_GCC__
#       undef __IS_COMPILER_GCC__
#   endif
#   if defined(__GNUC__) && !(  defined(__IS_COMPILER_ARM_COMPILER_5__)         \
                            ||  defined(__IS_COMPILER_ARM_COMPILER_6__)         \
                            ||  defined(__IS_COMPILER_LLVM__))
#       define __IS_COMPILER_GCC__              1
#   endif
//! @}
#endif
//! @}


#if defined(__IS_COMPILER_ARM_COMPILER_6__)
#   pragma clang diagnostic ignored "-Wformat-extra-args"
#endif

#ifndef   __ASM
  #define __ASM                                  __asm
#endif


#if defined(__clang__) || defined(__IS_COMPILER_ARM_COMPILER_5__)
#   undef __force_inline
#   define __force_inline       __attribute__((always_inline))

#   undef __always_inline
#   define __always_inline      __attribute__((always_inline))   

#   undef __unused
#   define __unused             __attribute__((unused))

#   undef __used
#   define __used               __USED

#   undef __packed_aligned
#   define __packed_aligned     __attribute__((packed, aligned(4)))

#   undef __printflike
#   define __printflike(...)    __attribute__((format(printf, __VA_ARGS__)))

#   undef __noinline
#   define __noinline           __attribute__((noinline))

#endif

#undef __STRING
#   define __STRING(__S)        #__S


#undef offsetof
#if defined(__clang__) || defined(__IS_COMPILER_GCC__)
#   define offsetof(st, m)      __builtin_offsetof(st, m)
#else
#   undef static_assert     
#   define static_assert(...)
#   define offsetof(st, m)      ((size_t)((char *)&((st *)0)->m - (char *)0))
#endif

typedef unsigned int        uint;

#undef UNUSED_PARAM
#define UNUSED_PARAM(__VAR)      __VAR = __VAR

#ifndef __CONCAT
#   define ____CONCAT(a,b)      a##b
#   define __CONCAT(a,b)        ____CONCAT(a,b)
#endif

///**
//  \brief   Get Control Register
//  \details Returns the content of the Control Register.
//  \return               Control Register value
// */
//static __force_inline uint32_t __get_current_exception(void)
//{
//  uint32_t result;

//  __ASM volatile ("MRS %0, ipsr" : "=r" (result) );
//  return(result);
//}   

#define __unhandled_user_irq    Default_Handler

#define __StackOneBottom        Image$$ARM_LIB_STACK_ONE$$ZI$$Base

#define __mutex_array_start     Image$$ER_MUTEX_ARRAY$$ZI$$Base
#define __mutex_array_end       Image$$ER_MUTEX_ARRAY$$ZI$$Limit

#define PROGRAM_STATUS_REG
#define PICO_ON_DEVICE                  1


#undef __sev
#undef __wfi
#undef __wfe
#undef __dmb
//#undef __dsb
//#undef __isb

#define __sev       __builtin_arm_sev
#define __wfi       __builtin_arm_wfi
#define __wfe       __builtin_arm_wfe

#ifndef __PLOOC_VA_NUM_ARGS_IMPL
#   define __PLOOC_VA_NUM_ARGS_IMPL( _0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,     \
                                    _12,_13,_14,_15,_16,__N,...)      __N
#endif

#ifndef __PLOOC_VA_NUM_ARGS
#define __PLOOC_VA_NUM_ARGS(...)                                                \
            __PLOOC_VA_NUM_ARGS_IMPL( 0,##__VA_ARGS__,16,15,14,13,12,11,10,9,   \
                                      8,7,6,5,4,3,2,1,0)
#endif

#define __dmb0()    __builtin_arm_dmb(0xF)
#define __dmb1(__N) __builtin_arm_dmb(__N)

#define __dmb(...)  __CONCAT(__dmb, __PLOOC_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

//#define __dsb       __DSB
//#define __isb       __ISB

typedef uint16_t __uint16_t;

#include "pico/platform.h"

#if 0 /* a configuration for over-clocking to 250MHz */
#define PLL_SYS_POSTDIV2                    1
#define PLL_SYS_POSTDIV1                    6
#define PLL_SYS_VCO_FREQ_KHZ                (1500 * KHZ)
#define SYS_CLK_KHZ                         (250 * KHZ)
#endif

#endif
