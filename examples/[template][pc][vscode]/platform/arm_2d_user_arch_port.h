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
 * Title:        arm_2d_user_arch_port.h
 * Description:  Public header file for porting Arm-2D to an non-supported
 *               Architecture. 
 *
 * $Date:        20 Sept 2024
 * $Revision:    V.1.1.0
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_USER_ARCH_PORT_H__
#define __ARM_2D_USER_ARCH_PORT_H__

/*============================ INCLUDES ======================================*/
/*
 * NOTE: You can modify this header file for your own architecture.
 */

/*============================ MACROS ========================================*/

#if defined (_MSC_VER) 
#   include <stdint.h>
#   define __STATIC_FORCEINLINE static __forceinline
#   define __STATIC_INLINE static __inline
#   define __ALIGNED(x) __declspec(align(x))
#   define __WEAK __attribute__((weak))
#elif defined ( __APPLE_CC__ )
#   include <stdint.h>
#   define  __ALIGNED(x) __attribute__((aligned(x)))
#   define __STATIC_FORCEINLINE static inline __attribute__((always_inline)) 
#   define __STATIC_INLINE static inline
#   define __WEAK __attribute__((weak))
#else
#   include <stdint.h>
#   define  __ALIGNED(x) __attribute__((aligned(x)))
#   define __STATIC_FORCEINLINE static inline __attribute__((always_inline)) 
#   define __STATIC_INLINE static inline
#   define __WEAK __attribute__((weak))
#endif

#undef arm_irq_safe
#undef arm_exit_irq_safe
#define arm_irq_safe  arm_using( uint32_t ARM_2D_SAFE_NAME(temp) = 0,         \
                                 {  ARM_2D_UNUSED(ARM_2D_SAFE_NAME(temp));    \
                                    VT_enter_global_mutex();},                \
                                 {  VT_leave_global_mutex();} )
#define arm_exit_irq_safe    continue


/**
  \brief   Reverse byte order (16 bit)
  \details Reverses the byte order within each halfword of a word. For example, 0x12345678 becomes 0x34127856.
  \param [in]    wValue  the two half words to reverse
  \return               Reversed value
 */
__STATIC_FORCEINLINE uint32_t __REV16(uint32_t wValue)
{
    uint32_t wHigh = wValue & 0xFF00FF00;
    uint32_t wLow = wValue & 0x00FF00FF;

    return (wHigh >> 8) | (wLow << 8);
}

extern void VT_enter_global_mutex(void);
extern void VT_leave_global_mutex(void);


#endif  /* end of __ARM_2D_USER_ARCH_PORT_H__ */

