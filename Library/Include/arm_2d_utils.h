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
 * Title:        arm_2d_utils.h
 * Description:  Public header file for Arm-2D Library
 *
 * $Date:        27. Dec 2023
 * $Revision:    V.1.4.1
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_UTILS_H__
#define __ARM_2D_UTILS_H__

/*============================ INCLUDES ======================================*/

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#   pragma clang diagnostic ignored "-Wundef"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif

#ifdef  __ARM_2D_HAS_USER_HEADER__
#   include __ARM_2D_HAS_USER_HEADER__
#endif



#undef __IS_SUPPORTED_ARM_ARCH__
#if (   (defined(__ARM_ARCH) && __ARM_ARCH)                                     \
    ||  defined(__TARGET_ARCH_ARM)) && !defined(__APPLE__)
#   define __IS_SUPPORTED_ARM_ARCH__        1
#else
#   define __IS_SUPPORTED_ARM_ARCH__        0
#endif

/*! \note arm-2d relies on CMSIS 5.8.0 and above.
 */
#if __IS_SUPPORTED_ARM_ARCH__

#ifdef   __cplusplus
extern "C" {
#endif

#   include "cmsis_compiler.h"

#ifdef   __cplusplus
}
#endif

#else
#   include "arm_2d_user_arch_port.h"
#endif


#ifdef   __cplusplus
extern "C" {
#endif

/*!
 * \addtogroup gKernel 1 Kernel
 * @{
 */

/*============================ MACROS ========================================*/


/*----------------------------------------------------------------------------*
 * Environment Detection                                                      *
 *----------------------------------------------------------------------------*/


/* The macros to identify compilers */

/* detect IAR */
#undef __IS_COMPILER_IAR__
#if defined(__IAR_SYSTEMS_ICC__)
#   define __IS_COMPILER_IAR__                  1
#endif

/* detect arm compiler 5 */
#undef __IS_COMPILER_ARM_COMPILER_5__
#if ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
#   define __IS_COMPILER_ARM_COMPILER_5__       1
#endif


/* detect arm compiler 6 */
#undef __IS_COMPILER_ARM_COMPILER_6__
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#   define __IS_COMPILER_ARM_COMPILER_6__       1
#endif

/* detect arm compilers */
#undef __IS_COMPILER_ARM_COMPILER__
#if defined(__IS_COMPILER_ARM_COMPILER_5__) && __IS_COMPILER_ARM_COMPILER_5__   \
||  defined(__IS_COMPILER_ARM_COMPILER_6__) && __IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_ARM_COMPILER__         1
#endif

/* detect clang (llvm) */
#undef  __IS_COMPILER_LLVM__
#if defined(__clang__) && !__IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_LLVM__                 1
#else

/* detect gcc */
#   undef __IS_COMPILER_GCC__
#   if defined(__GNUC__) && !(  defined(__IS_COMPILER_ARM_COMPILER__)           \
                            ||  defined(__IS_COMPILER_LLVM__)                   \
                            ||  defined(__IS_COMPILER_IAR__))
#       define __IS_COMPILER_GCC__              1
#   endif

#endif

/*----------------------------------------------------------------------------*
 * OOC and Private Protection                                                 *
 *----------------------------------------------------------------------------*/
/*  minimal support for OOPC */
#undef __implement_ex
#undef __implement
#undef implement
#undef implement_ex
#undef inherit
#undef inherit_ex

/*!
 * \note do NOT use this macro directly
 */
#ifdef   __cplusplus
#   define __implement_ex(__type, __name)   __type  __name
#else
#   define __implement_ex(__type, __name)                                       \
            union {                                                             \
                __type  __name;                                                 \
                __type;                                                         \
            }
#endif
/*!
 * \note do NOT use this macro
 */
#define __inherit_ex(__type, __name)   __type  __name


/*!
 * \note do NOT use this macro directly
 */
#define __implement(__type)             __implement_ex( __type,                 \
                                                        use_as__##__type)


/*!
 * \note do NOT use this macro directly
 */
#define __inherit(__type)               __inherit_ex(__type,use_as__##__type)


/*!
 * \brief inherit a given class
 * \param __type the base class, you can use .use_as_xxxxx for referencing 
 *               the base.
 * \note this macro supports microsoft extensions (-fms-extensions)
 */
#define implement(__type)               __implement(__type)

/*!
 * \brief inherit a given class and give it an alias name
 * \param __type the base class
 * \param __name an alias name for referencing the base class
 * \note this macro supports microsoft extensions (-fms-extensions)
 */
#define implement_ex(__type, __name)    __implement_ex(__type, __name)


/*!
 * \brief inherit a given class
 * \param __type the base class, you can use .use_as_xxxxx for referencing 
 *               the base.
 * \note this macro does NOT support microsoft extensions (-fms-extensions)
 */
#define inherit(__type)                 __inherit(__type)

/*!
 * \brief inherit a given class and give it an alias name
 * \param __type the base class
 * \param __name an alias name for referencing the base class
 * \note this macro does NOT support microsoft extensions (-fms-extensions)
 */
#define inherit_ex(__type, __name)      __inherit_ex(__type, __name)


/*----------------------------------------------------------------------------*
 * Misc                                                                       *
 *----------------------------------------------------------------------------*/

/*! 
 * \brief a macro to mark unused variables and let the compiler happy
 */
#ifndef ARM_2D_UNUSED
#   define ARM_2D_UNUSED(__VAR)     (void)(__VAR)
#endif

#undef ARM_TYPE_CONVERT
/*!
 * \brief convert a given variable to a specified type, the converted result 
 *        can be used as lvalue. 
 * \param __VAR the target variable
 * \param __TYPE the target type
 */
#define ARM_TYPE_CONVERT(__VAR, __TYPE)          (*((__TYPE *)&(__VAR)))

/*! 
 * \brief a macro to test the boolean result for a given value using a given 
 *         bitmask
 * \param[in] __VALUE the target value
 * \param[in] __BITS a bitmask
 * \retval true all bits in the bitmask is 1
 * \retval false not all bits in the bitmask is 1 
 */
#ifndef ARM_TEST_BITS
#   define ARM_TEST_BITS(__VALUE, __BITS)   ((__BITS) == ((__VALUE) & (__BITS)))
#endif

/*! 
 * \brief get the number of items in an given array
 */
#ifndef dimof
#   define dimof(__array)          (sizeof(__array)/sizeof(__array[0]))
#endif

/*! 
 * \brief get the offset of a given member in a specified structure/union
 * \param __type the host type
 * \param __member the name of the target member
 * \return size_t the offset (in bytes)
 */
#ifndef offsetof
#   define offsetof(__type, __member)                                           \
            ((uintptr_t)&(((__type *)NULL)->__member))
#endif

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_TO_STRING(__STR)          #__STR

/*!
 * \brief convert a string to C string
 */
#define ARM_TO_STRING(__STR)            __ARM_TO_STRING(__STR)

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_VA_NUM_ARGS_IMPL( _0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,      \
                                _13,_14,_15,_16,__N,...)      __N

/*!
 * \brief A macro to count the number of parameters
 * 
 * \note if GNU extension is not supported or enabled, the following express will
 *       be false:  (__ARM_VA_NUM_ARGS() != 0)
 *       This might cause problems when in this library.
 */
#define __ARM_VA_NUM_ARGS(...)                                                  \
            __ARM_VA_NUM_ARGS_IMPL( 0,##__VA_ARGS__,16,15,14,13,12,11,10,9,     \
                                      8,7,6,5,4,3,2,1,0)

/*! 
 * \brief detect whether GNU extension is enabled in compilation or not
 */
#if __ARM_VA_NUM_ARGS() != 0
#   warning Please enable GNU extensions, it is required by the Arm-2D.
#endif


#undef ARM_2D_PARAM

/*!
 * \brief a macro helper to be used with ARM_2D_INVODE to improve the 
 *        readability of the code
 */
#define ARM_2D_PARAM(...)           __VA_ARGS__

#ifndef ARM_2D_INVOKE
/*!
 * \brief A macro to safely invode a function pointer
 * 
 * \param[in] __FUNC_PTR the target function pointer
 * \param[in] ... an optional parameter list
 */
#   define ARM_2D_INVOKE(__FUNC_PTR, ...)                                       \
            ((NULL == (__FUNC_PTR)) ? 0 : ((*(__FUNC_PTR))(__VA_ARGS__)))

#endif

#ifndef ARM_2D_INVOKE_RT_VOID
/*!
 * \brief A macro to safely call a function pointer that has no return value
 * 
 * \param[in] __FUNC_PTR the target function pointer
 * \param[in] ... an optional parameter list
 */
#   define ARM_2D_INVOKE_RT_VOID(__FUNC_PTR, ...)                               \
            if (NULL != (__FUNC_PTR)) (*(__FUNC_PTR))(__VA_ARGS__)

#endif

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT2(__A, __B)                        __A##__B

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT2_ALT(__A, __B)                    __A##__B

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT3(__A, __B, __C)                   __A##__B##__C

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT4(__A, __B, __C, __D)              __A##__B##__C##__D

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT5(__A, __B, __C, __D, __E)         __A##__B##__C##__D##__E

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT6(__A, __B, __C, __D, __E, __F)                            \
                                    __A##__B##__C##__D##__E##__F

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT7(__A, __B, __C, __D, __E, __F, __G)                       \
                                    __A##__B##__C##__D##__E##__F##__G

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H)                  \
                                    __A##__B##__C##__D##__E##__F##__G##__H

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)             \
                                    __A##__B##__C##__D##__E##__F##__G##__H##__I

/*! 
 * \brief connect two symbol names as one
 */
#define ARM_CONNECT2(__A, __B)                  __ARM_CONNECT2(__A, __B)

/*! 
 * \brief connect two symbol names as one
 */
#define ARM_CONNECT2_ALT(__A, __B)              __ARM_CONNECT2_ALT(__A, __B)

/*! 
 * \brief connect three symbol names as one
 */
#define ARM_CONNECT3(__A, __B, __C)             __ARM_CONNECT3(__A, __B, __C)

/*! 
 * \brief connect four symbol names as one
 */
#define ARM_CONNECT4(__A, __B, __C, __D)        __ARM_CONNECT4(__A, __B, __C, __D)

/*! 
 * \brief connect five symbol names as one
 */
#define ARM_CONNECT5(__A, __B, __C, __D, __E)                                   \
                __ARM_CONNECT5(__A, __B, __C, __D, __E)

/*! 
 * \brief connect six symbol names as one
 */
#define ARM_CONNECT6(__A, __B, __C, __D, __E, __F)                              \
                __ARM_CONNECT6(__A, __B, __C, __D, __E, __F)

/*! 
 * \brief connect seven symbol names as one
 */
#define ARM_CONNECT7(__A, __B, __C, __D, __E, __F, __G)                         \
                __ARM_CONNECT7(__A, __B, __C, __D, __E, __F, __G)

/*! 
 * \brief connect eight symbol names as one
 */
#define ARM_CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H)                    \
                __ARM_CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H)

/*! 
 * \brief connect nine symbol names as one
 */
#define ARM_CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)               \
                __ARM_CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)

/*! 
 * \brief connect up to 9 symbol names as one
 */
#define arm_connect(...)                                                        \
            ARM_CONNECT2_ALT(ARM_CONNECT, __ARM_VA_NUM_ARGS(__VA_ARGS__))       \
                (__VA_ARGS__)

/*! 
 * \brief connect up to 9 symbol names as one
 */
#define ARM_CONNECT(...)        arm_connect(__VA_ARGS__)

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_USING1(__declare)                                                 \
            for (__declare, *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;  \
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;           \
                )

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_USING2(__declare, __on_leave_expr)                                \
            for (__declare, *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;  \
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;           \
                 (__on_leave_expr)                                              \
                )

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_USING3(__declare, __on_enter_expr, __on_leave_expr)               \
            for (__declare, *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;  \
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL ?          \
                    ((__on_enter_expr),1) : 0;                                  \
                 (__on_leave_expr)                                              \
                )

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_USING4(__dcl1, __dcl2, __on_enter_expr, __on_leave_expr)          \
            for (__dcl1,__dcl2,*ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)= NULL;\
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL ?          \
                    ((__on_enter_expr),1) : 0;                                  \
                 (__on_leave_expr)                                              \
                )

/*!
 * \brief create a code segment with up to two local variables and 
 *        entering/leaving operations
 * \note prototype 1
 *       arm_using(local variable declaration) {
 *           code body 
 *       }
 * 
 * \note prototype 2
 *       arm_using(local variable declaration, {code segment before leaving the body}) {
 *           code body
 *       }
 *
 * \note prototype 3
 *       arm_using( local variable declaration, 
 *                 {code segment before entering the body},
 *                 {code segment before leaving the body}
 *                 ) {
 *           code body 
 *       }
 *
 * \note prototype 4
 *       arm_using( local variable1 declaration,
                    local variable2 with the same type as the local variable 1,
 *                 {code segment before entering the body},
 *                 {code segment before leaving the body}
 *                 ) {
 *           code body 
 *       }
 */
#define arm_using(...)                                                          \
            ARM_CONNECT2(__ARM_USING, __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \note do NOT use this macro directly
 */
#define __ARM_WITH2(__type, __addr)                                             \
            ARM_USING(__type *_p=(__addr))

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_WITH3(__type, __addr, __item)                                     \
            ARM_USING(__type *_p=(__addr), *__item = _p, (void)_p, (void)0)

/*!
 * \brief a with block to access members of a given object
 * 
 * \note prototype 1
 *       arm_with(object type, address of the object) {
 *           you can use _.xxxx to access the members of the object
 *       }
 *
 * \note prototype 2
 *       arm_with(object type, address of the object, name of the iterator) {
 *           you can use your own iterator to access the members of the object
 *       }
 */
#define arm_with(...)                                                           \
            ARM_CONNECT2(__ARM_WITH, __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief access each items in a given array
 * \param __type the type of the array
 * \param __array the target array
 * \note you can use "_" as the current object (iterator)
 */
#define ARM_FOREACH2(__type, __array)                                           \
            arm_using(__type *_ = __array)                                      \
            for (   uint_fast32_t ARM_CONNECT2(count,__LINE__) = dimof(__array);\
                    ARM_CONNECT2(count,__LINE__) > 0;                           \
                    _++, ARM_CONNECT2(count,__LINE__)--                         \
                )

/*!
 * \brief access each items in a given array
 * \param __type the type of the array
 * \param __array the target array
 * \param __item a name for the current item (iterator)
 */
#define ARM_FOREACH3(__type, __array, __item)                                   \
            arm_using(__type *_ = __array, *__item = _, (void)_, (void)0 )      \
            for (   uint_fast32_t ARM_CONNECT2(count,__LINE__) = dimof(__array);\
                    ARM_CONNECT2(count,__LINE__) > 0;                           \
                    _++, __item = _, ARM_CONNECT2(count,__LINE__)--             \
                )

/*!
 * \brief access each items in a given array
 * \param __type the type of the array
 * \param __array the target array or the pointer of an memory block
 * \param __count number of items in the array/memory block
 * \param __item a name for the current item (iterator)
 */
#define ARM_FOREACH4(__type, __array, __count, __item)                          \
            arm_using(__type *_ = __array, *__item = _, (void)_, (void)0)       \
            for (   uint_fast32_t ARM_CONNECT2(count,__LINE__) = (__count);     \
                    ARM_CONNECT2(count,__LINE__) > 0;                           \
                    _++, __item = _, ARM_CONNECT2(count,__LINE__)--             \
                )

/*!
 * \brief access each items in a given array
 * \note there are 3 prototypes, please refer to ARM_FOREACH1/2/3 for details
 */
#define arm_foreach(...)                                                        \
            ARM_CONNECT2(ARM_FOREACH, __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief a wrapper for __attribute__((nonnull))
 */
#ifndef ARM_NONNULL
#   if  defined(__IS_COMPILER_ARM_COMPILER_5__) ||\
        defined(__IS_COMPILER_ARM_COMPILER_6__) ||\
        defined(__IS_COMPILER_GCC__)            ||\
        defined(__IS_COMPILER_LLVM__)
#       define ARM_NONNULL(...)     __attribute__((nonnull(__VA_ARGS__)))
#   else
#       define ARM_NONNULL(...)
#   endif
#endif

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_SECTION(__X)          __attribute__((section(__X)))

/*!
 * \brief an attribute to specify the section
 * \note it works for both functions and static/global variables
 */
#ifndef ARM_SECTION
#   define ARM_SECTION(__X)         __ARM_SECTION(__X)
#endif

/*!
 * \brief an attribute for static variables that no initialisation is required 
 *        in the C startup process.
 */
#ifndef ARM_NOINIT
#   if     defined(__IS_COMPILER_ARM_COMPILER_5__)
#       define ARM_NOINIT   __attribute__(( section( ".bss.noinit"),zero_init))
#   elif   defined(__IS_COMPILER_ARM_COMPILER_6__)
#       define ARM_NOINIT   __attribute__(( section( ".bss.noinit")))
#   elif   defined(__IS_COMPILER_IAR__)
#       define ARM_NOINIT   __no_init
#   elif   (defined(__IS_COMPILER_GCC__) || defined(__IS_COMPILER_LLVM__)) && !defined(__APPLE__)
#       define ARM_NOINIT   __attribute__(( section( ".bss.noinit")))
#   else
#       define ARM_NOINIT
#   endif
#endif

#undef __ARM_ALIGN

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_ALIGN(__N)            __attribute__((aligned(__N)))

/*!
 * \brief an attribute to specify aligment requirement
 * \note it works for both functions and static/global variables
 */
#ifndef ARM_ALIGN
#   define ARM_ALIGN(__N)           __ARM_ALIGN(__N)
#endif



/*!
 * \brief local variable decoration for pointers: restrict
 */
#ifndef __RESTRICT
#   define __RESTRICT                __restrict
#endif

/*!
 * \brief an decoration for claiming that the immediate following symbol
 *        (variable / function) is not WEAK. If there is an __WEAK symbol, the
 *        __OVERRIDE_WEAK one can override it.
 *         
 */
#ifndef __OVERRIDE_WEAK
#   define __OVERRIDE_WEAK
#endif

/*!
 * \brief A macro to generate a safe name, usually used in macro template as the 
 *        name of local variables
 * 
 */
#define ARM_2D_SAFE_NAME(...)    ARM_CONNECT(__,__LINE__,##__VA_ARGS__)

/*!
 * \brief A macro to generate a safe name, usually used in macro template as the 
 *        name of local variables
 * 
 */
#define arm_2d_safe_name(...)    ARM_2D_SAFE_NAME(__VA_ARGS__)

/*!
 * \brief a decoration to make the immediate following code irq-safe
 * 
 * \code
     arm_irq_safe {
         // code inside the brackets are IRQ safe
         ...
     }

     // the printf is IRQ safe
     arm_irq_safe printf("IRQ safe printf\n");

   \endcode
 */
#if __IS_SUPPORTED_ARM_ARCH__
#   undef arm_irq_safe
#   undef arm_exit_irq_safe
#   define arm_irq_safe                                                         \
            arm_using(  uint32_t ARM_2D_SAFE_NAME(temp) =                       \
                        ({uint32_t temp=__get_PRIMASK();__disable_irq();temp;}),\
                        __set_PRIMASK(ARM_2D_SAFE_NAME(temp)))
#   define arm_exit_irq_safe    continue
#endif


#undef ARM_2D_WRAP_FUNC
#undef __ARM_2D_WRAP_FUNC
#undef ARM_2D_ORIG_FUNC
#undef __ARM_2D_ORIG_FUNC

#if defined(__IS_COMPILER_ARM_COMPILER_6__)

/*!
 * \note do NOT use this macro directly
 */
#   define __ARM_2D_WRAP_FUNC(__FUNC)           $Sub$$##__FUNC

/*!
 * \note do NOT use this macro directly
 */
#   define __ARM_2D_ORIG_FUNC(__FUNC)           $Super$$## __FUNC
#else

/*!
 * \note do NOT use this macro directly
 */
#   define __ARM_2D_WRAP_FUNC(x) __wrap_ ## x

/*!
 * \note do NOT use this macro directly
 */
#   define __ARM_2D_ORIG_FUNC(x) __real_ ## x
#endif

/*!
 * \brief a symbol wrapper to override a specified function
 */
#define ARM_2D_WRAP_FUNC(__FUNC)                __ARM_2D_WRAP_FUNC(__FUNC)

/*!
 * \brief a symbol wrapper to refer the original function with a given name
 */
#define ARM_2D_ORIG_FUNC(__FUNC)                __ARM_2D_ORIG_FUNC(__FUNC)

/*----------------------------------------------------------------------------*
 * List Operations                                                            *
 *----------------------------------------------------------------------------*/

/*  ALL the parameters passed to following macros must be pointer variables. */

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_LIST_STACK_PUSH(__P_TOP, __P_NODE)                                \
        do {                                                                    \
            ((__arm_slist_node_t *)(__P_NODE))->ptNext =                        \
                (__arm_slist_node_t *)(__P_TOP);                                \
            (*(__arm_slist_node_t **)&(__P_TOP)) =                              \
                (__arm_slist_node_t *)(__P_NODE);                               \
        } while(0)
        
/*! 
 * \brief push a item to a list
 * \param[in] __P_TOP a pointer points to the first item of a list
 * \param[in] __P_NODE a pointer points to the new item
 */
#define ARM_LIST_STACK_PUSH(__P_TOP, __P_NODE)                                  \
            __ARM_LIST_STACK_PUSH((__P_TOP), (__P_NODE))

/*! 
 * \brief insert a item after a specified node
 * \param[in] __P_TARGET a pointer points to the target node
 * \param[in] __P_NODE a pointer points to the new item
 */
#define ARM_LIST_INSERT_AFTER(__P_TARGET, __P_NODE)                             \
            __ARM_LIST_STACK_PUSH((__P_TARGET), (__P_NODE))

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_LIST_STACK_POP(__P_TOP, __P_NODE)                                 \
        do {                                                                    \
            (*(__arm_slist_node_t **)&(__P_NODE)) =                             \
                (__arm_slist_node_t *)(__P_TOP);                                \
            if (NULL != (__P_TOP)) {                                            \
                (*(__arm_slist_node_t **)&(__P_TOP)) =                          \
                    ((__arm_slist_node_t *)(__P_NODE))->ptNext;                 \
                ((__arm_slist_node_t *)(__P_NODE))->ptNext = NULL;              \
            }                                                                   \
        } while(0)

/*! 
 * \brief pop a item from a list
 * \param[in] __P_TOP a pointer points to the first item of a list
 * \param[out] __P_NODE a pointer variable for the node
 */
#define ARM_LIST_STACK_POP(__P_TOP, __P_NODE)                                   \
            __ARM_LIST_STACK_POP((__P_TOP), (__P_NODE))

/*! 
 * \brief remove a item after a specified node
 * \param[in] __P_TARGET a pointer points to the target node
 * \param[out] __P_NODE a pointer variable for the node
 */
#define ARM_LIST_REMOVE_AFTER(__P_TARGET, __P_NODE)                             \
            ARM_LIST_STACK_POP((__P_TARGET), (__P_NODE))

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_LIST_QUEUE_ENQUEUE(__HEAD, __TAIL, __ITEM)                        \
    do {                                                                        \
        if (NULL == (__TAIL)) {                                                 \
            (*((__arm_slist_node_t **)&(__TAIL))) =                             \
                (__arm_slist_node_t *)(__ITEM);                                 \
            (*((__arm_slist_node_t **)&(__HEAD))) =                             \
                (__arm_slist_node_t *)(__ITEM);                                 \
        } else {                                                                \
            ((__arm_slist_node_t *)(__TAIL))->ptNext =                          \
                (__arm_slist_node_t *)(__ITEM);                                 \
            (*(__arm_slist_node_t **)&(__TAIL)) =                               \
                (__arm_slist_node_t *)(__ITEM);                                 \
        }                                                                       \
        ((__arm_slist_node_t *)(__ITEM))->ptNext = NULL;                        \
    } while(0)

/*! 
 * \brief enter a node to a queue
 * \param[in] __HEAD a pointer points to the queue head
 * \param[in] __TAIL a pointer points to the queue tail
 * \param[in] __ITEM a pointer variable for the new node
 */
#define ARM_LIST_QUEUE_ENQUEUE(__HEAD, __TAIL, __ITEM)                          \
            __ARM_LIST_QUEUE_ENQUEUE((__HEAD), (__TAIL), (__ITEM))

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_LIST_QUEUE_DEQUEUE(__HEAD, __TAIL, __ITEM)                        \
    do {                                                                        \
        (*(__arm_slist_node_t **)&(__ITEM)) =  (__arm_slist_node_t *)(__HEAD);  \
        if (NULL != (__HEAD)) {                                                 \
            (*(__arm_slist_node_t **)&(__HEAD)) =                               \
                ((__arm_slist_node_t *)(__HEAD))->ptNext;                       \
            if (NULL == (__HEAD)) {                                             \
                (__TAIL) = NULL;                                                \
            }                                                                   \
        }                                                                       \
    } while(0)

/*! 
 * \brief fetch a node from a queue
 * \param[in] __HEAD a pointer points to the queue head
 * \param[in] __TAIL a pointer points to the queue tail
 * \param[out] __ITEM a pointer variable for the node
 */
#define ARM_LIST_QUEUE_DEQUEUE(__HEAD, __TAIL, __ITEM)                          \
            __ARM_LIST_QUEUE_DEQUEUE((__HEAD), (__TAIL), (__ITEM))

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_LIST_QUEUE_PEEK(__HEAD, __TAIL, __ITEM)                           \
    do {                                                                        \
        (*(__arm_slist_node_t **)&(__ITEM)) =  (__arm_slist_node_t *)(__HEAD);  \
    } while(0)

/*! 
 * \brief peek a node from a queue
 * \param[in] __HEAD a pointer points to the queue head
 * \param[in] __TAIL a pointer points to the queue tail
 * \param[out] __ITEM a pointer variable for the node
 */
#define ARM_LIST_QUEUE_PEEK(__HEAD, __TAIL, __ITEM)                             \
            __ARM_LIST_QUEUE_PEEK((__HEAD), (__TAIL), (__ITEM))                 \

/*----------------------------------------------------------------------------*
 * PT Operations                                                              *
 *----------------------------------------------------------------------------*/
/*
Protothreads open source BSD-style license
The protothreads library is released under an open source license that allows 
both commercial and non-commercial use without restrictions. The only 
requirement is that credits is given in the source code and in the documentation 
for your product.

The full license text follows.

Copyright (c) 2004-2005, Swedish Institute of Computer Science.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Neither the name of the Institute nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS `AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

Author: Adam Dunkels
*/

#define ARM_PT_BEGIN(__STATE)                                                   \
            enum {                                                              \
                count_offset = __COUNTER__ + 1,                                 \
            };                                                                  \
            uint8_t *ptPTState = &(__STATE);                                    \
            switch (__STATE) {                                                  \
                case __COUNTER__ - count_offset: 

#define ARM_PT_ENTRY(...)                                                       \
            (*ptPTState) = (__COUNTER__ - count_offset + 1) >> 1;               \
            __VA_ARGS__                                                         \
            case (__COUNTER__ - count_offset) >> 1: (void)(*ptPTState);
            
#define ARM_PT_YIELD(...)                                                       \
            ARM_PT_ENTRY(return __VA_ARGS__;)
            
#define ARM_PT_END()                                                            \
            (*ptPTState) = 0;                                                   \
            break;}

#define ARM_PT_GOTO_PREV_ENTRY(...)    return __VA_ARGS__;

            
#define ARM_PT_REPORT_STATUS(...)                                               \
            ARM_PT_ENTRY(                                                       \
                return __VA_ARGS__;                                             \
            )
            
#define ARM_PT_RETURN(...)                                                      \
            (*ptPTState) = 0;                                                   \
            return __VA_ARGS__;

/*----------------------------------------------------------------------------*
 * Definition Template                                                        *
 *----------------------------------------------------------------------------*/

/*!
 * \note do NOT use this macro directly
 */
#define __def_low_lv_io(__NAME, __SW, ...)                                      \
const __arm_2d_low_level_io_t LOW_LEVEL_IO##__NAME = {                          \
    .SW = (__arm_2d_io_func_t *)&(__SW),                                        \
    .HW = (NULL, ##__VA_ARGS__)                                                 \
}


/*!
 * \brief a template to implement a specified low-level io interface
 * \param __NAME the IO name
 * \param __SW the default c implementation
 * \param ... you can specify an optional implementation using some acceleration.
 */
#define def_low_lv_io(__NAME, __SW, ...)                                        \
            __def_low_lv_io(__NAME, __SW, ##__VA_ARGS__)

/*!
 * \note do NOT use this macro directly
 */
#define __ref_low_lv_io(__NAME)     &LOW_LEVEL_IO##__NAME

/*!
 * \brief a symbol wrapper for referencing a specified low-level IO
 */
#define ref_low_lv_io(__NAME)       __ref_low_lv_io(__NAME)

/*----------------------------------------------------------------------------*
 * LOG                                                                        *
 *----------------------------------------------------------------------------*/

/*!
 * \brief the bit-mask for log channels
 */
#define ARM_2D_LOG_CHN_TYPE_USER                            _BV(28)             //!< the channel for user messages
#define ARM_2D_LOG_CHN_TYPE_INFO                            _BV(29)             //!< the channel for generic information
#define ARM_2D_LOG_CHN_TYPE_WARNING                         _BV(30)             //!< the channel for warnings messages
#define ARM_2D_LOG_CHN_TYPE_ERROR                           _BV(31)             //!< the channel for error messages

#define ARM_2D_LOG_CHN_PIPELINE                             _BV(0)              //!< the channel dedicated to the pixel-pipeline
#define ARM_2D_LOG_CHN_OPCODE                               _BV(1)              //!< the channel dedicated to OPCODEs and related algorithms
#define ARM_2D_LOG_CHN_HELPER                               _BV(2)              //!< the channel dedicated to Helper services
#define ARM_2D_LOG_CHN_HELPER_PFB                           _BV(3)              //!< the channel dedicated to the PFB helper service
#define ARM_2D_LOG_CHN_SCENE_PLAYER                         _BV(4)              //!< the channel dedicated to the scene player service
#define ARM_2D_LOG_CHN_DIRTY_REGION_OPTIMISATION            _BV(5)              //!< the channel dedicated to the dirty region optimization services
#define ARM_2D_LOG_CHN_STATISTICS                           _BV(6)              //!< the channel dedicated to show statistics
#define ARM_2D_LOG_CHN_APP                                  _BV(24)             //!< the channel dedicated to applications and examples


#if defined(__ARM_2D_CFG_LOG_OUTPUT_SUPPORT_COLOUR__) && __ARM_2D_CFG_LOG_OUTPUT_SUPPORT_COLOUR__
#   define ARM_2D_TERMINAL_COLOUR_DEFAULT          "\x1b[0m"
#   define ARM_2D_TERMINAL_COLOUR_GREEN            "\x1b[32m"
#   define ARM_2D_TERMINAL_COLOUR_RED              "\x1b[31m"
#   define ARM_2D_TERMINAL_COLOUR_YELLOW           "\x1b[33m"
#   define ARM_2D_TERMINAL_COLOUR_WHITE            "\x1b[97m"
#   define ARM_2D_TERMINAL_COLOUR_BRIGHT_BLACK     "\x1b[90m"  
#else
#   define ARM_2D_TERMINAL_COLOUR_DEFAULT
#   define ARM_2D_TERMINAL_COLOUR_GREEN
#   define ARM_2D_TERMINAL_COLOUR_RED
#   define ARM_2D_TERMINAL_COLOUR_YELLOW
#   define ARM_2D_TERMINAL_COLOUR_WHITE
#   define ARM_2D_TERMINAL_COLOUR_BRIGHT_BLACK
#endif

/*!
 * \brief the filter to enable log channels
 */
#ifndef __ARM_2D_LOG_CHANNEL_MASK_FILTER__
#   define __ARM_2D_LOG_CHANNEL_MASK_FILTER__               0xFFFFFFFF
#endif

#ifndef __ARM_2D_LOG_PRINTF_PIPELINE

/*!
 * \brief the log printf entry dedicated to pixel pipeline
 */
#   define __ARM_2D_LOG_PRINTF_PIPELINE         __arm_2d_log_printf
#endif

#ifndef __ARM_2D_LOG_PRINTF_OPCODE
/*!
 * \brief the log printf entry dedicated to OPCODEs and related algorithms
 */
#   define __ARM_2D_LOG_PRINTF_OPCODE           __arm_2d_log_printf
#endif

#ifndef __ARM_2D_LOG_PRINTF_HELPER
/*!
 * \brief the log printf entry dedicated to Helper services
 */
#   define __ARM_2D_LOG_PRINTF_HELPER           __arm_2d_log_printf
#endif

#ifndef __ARM_2D_LOG_PRINTF_HELPER_PFB
/*!
 * \brief the log printf entry dedicated to the PFB helper service
 */
#   define __ARM_2D_LOG_PRINTF_HELPER_PFB       __arm_2d_log_printf
#endif

#ifndef __ARM_2D_LOG_PRINTF_SCENE_PLAYER
/*!
 * \brief the log printf entry dedicated to the scene player service
 */
#   define __ARM_2D_LOG_PRINTF_SCENE_PLAYER     __arm_2d_log_printf
#endif

#ifndef __ARM_2D_LOG_PRINTF_DIRTY_REGION_OPTIMISATION
/*!
 * \brief the log printf entry dedicated to the dirty region optimization service
 */
#   define __ARM_2D_LOG_PRINTF_DIRTY_REGION_OPTIMISATION                        \
                                                __arm_2d_log_printf
#endif

#ifndef __ARM_2D_LOG_PRINTF_STATISTICS
/*!
 * \brief the log printf entry dedicated to show statistics
 */
#   define __ARM_2D_LOG_PRINTF_STATISTICS     __arm_2d_log_printf
#endif

#ifndef __ARM_2D_LOG_PRINTF_APP
/*!
 * \brief the log printf entry dedicated to applications and examples
 */
#   define __ARM_2D_LOG_PRINTF_APP              __arm_2d_log_printf
#endif

#if defined(__ARM_2D_CFG_ENABLE_LOG__) && __ARM_2D_CFG_ENABLE_LOG__
/*!
 * \brief the log entry for generic information
 * \param[in] __CHN the channel name, e.g. PIPELINE, OPCODE, HELPER, HELPER_PFB etc.
 * \param[in] __INDENT number of indents before print actual content
 * \param[in] __PREFIX a string used as prefix, we can use it to print content
 *                      like function name, service name etc. 
 * \param[in] __FORMAT_STR the format string used in printf
 * \param[in] ... the optional parameters
 */
#   define ARM_2D_LOG_INFO(__CHN, __INDENT, __PREFIX, __FORMAT_STR, ...)        \
            __ARM_2D_LOG_PRINTF_##__CHN(                                        \
                (__INDENT),                                                     \
                (ARM_2D_LOG_CHN_TYPE_INFO | ARM_2D_LOG_CHN_##__CHN),            \
                (__PREFIX),                                                     \
                (__FORMAT_STR),##__VA_ARGS__)

/*!
 * \brief the log entry for warning messages
 * \param[in] __CHN the channel name, e.g. PIPELINE, OPCODE, HELPER, HELPER_PFB etc.
 * \param[in] __INDENT number of indents before print actual content
 * \param[in] __PREFIX a string used as prefix, we can use it to print content
 *                      like function name, service name etc. 
 * \param[in] __FORMAT_STR the format string used in printf
 * \param[in] ... the optional parameters
 */
#   define ARM_2D_LOG_WARNING(__CHN, __INDENT, __PREFIX, __FORMAT_STR, ...)     \
            __ARM_2D_LOG_PRINTF_##__CHN(                                        \
                (__INDENT),                                                     \
                (ARM_2D_LOG_CHN_TYPE_WARNING | ARM_2D_LOG_CHN_##__CHN),         \
                (__PREFIX),                                                     \
                (__FORMAT_STR),##__VA_ARGS__)

/*!
 * \brief the log entry for error messages
 * \param[in] __CHN the channel name, e.g. PIPELINE, OPCODE, HELPER, HELPER_PFB etc.
 * \param[in] __INDENT number of indents before print actual content
 * \param[in] __PREFIX a string used as prefix, we can use it to print content
 *                      like function name, service name etc. 
 * \param[in] __FORMAT_STR the format string used in printf
 * \param[in] ... the optional parameters
 */
#   define ARM_2D_LOG_ERROR(__CHN, __INDENT, __PREFIX, __FORMAT_STR, ...)       \
            __ARM_2D_LOG_PRINTF_##__CHN(                                        \
                (__INDENT),                                                     \
                (ARM_2D_LOG_CHN_TYPE_ERROR | ARM_2D_LOG_CHN_##__CHN),           \
                (__PREFIX),                                                     \
                (__FORMAT_STR),##__VA_ARGS__)

/*!
 * \brief the log entry for user messages
 * \param[in] __CHN the channel name, e.g. PIPELINE, OPCODE, HELPER, HELPER_PFB etc.
 * \param[in] __INDENT number of indents before print actual content
 * \param[in] __PREFIX a string used as prefix, we can use it to print content
 *                      like function name, service name etc. 
 * \param[in] __FORMAT_STR the format string used in printf
 * \param[in] ... the optional parameters
 */
#   define ARM_2D_LOG_USER(__CHN, __INDENT, __PREFIX, __FORMAT_STR, ...)        \
            __ARM_2D_LOG_PRINTF_##__CHN(                                        \
                (__INDENT),                                                     \
                (ARM_2D_LOG_CHN_TYPE_USER | ARM_2D_LOG_CHN_##__CHN),            \
                (__PREFIX),                                                     \
                (__FORMAT_STR),##__VA_ARGS__)

#else
#   define ARM_2D_LOG_INFO(__CHN, __INDENT, __PREFIX, __FORMAT_STR, ...)
#   define ARM_2D_LOG_WARNING(__CHN, __INDENT, __PREFIX, __FORMAT_STR, ...)
#   define ARM_2D_LOG_ERROR(__CHN, __INDENT, __PREFIX, __FORMAT_STR, ...)
#   define ARM_2D_LOG_USER(__CHN, __INDENT, __PREFIX, __FORMAT_STR, ...)
#endif


#ifndef __ARM_2D_PORT_PRINTF__
/*!
 * \brief the low level entry for printf
 * \note you can define this macro for retargeting
 */
#   define __ARM_2D_PORT_PRINTF__(__format, ...)   printf((__format), ##__VA_ARGS__)
#endif

#ifndef __ARM_2D_LOG_MAX_STRING_LEN__
/*!
 * \brief the maximum allowed string length. The log service requests the specified 
 *        number of bytes for storing string. default value is 256
 * \note please make sure there are sufficient memory in HEAP.
 */
#   define __ARM_2D_LOG_MAX_STRING_LEN__        256
#endif

/*============================ TYPES =========================================*/

/*!
 * \brief a type for generic list
 * 
 * \note to avoid using container_of() operand, please put __arm_slist_node_t
 *       at the beginning of a class/structure
 */
typedef struct __arm_slist_node_t __arm_slist_node_t;
struct __arm_slist_node_t {
    __arm_slist_node_t     *ptNext;             //!< the next node
};


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief an entry for log printf
 * \param[in] nIndentLevel number of indents before print actual content
 * \param[in] wChannelMask a bit-mask to indicate the log channel
 * \param[in] pchPrefix a string used as prefix, we can use it to print content
 *                      like function name, service name etc.
 * \param[in] pchFormatString the format string used in printf
 * \param[in] ... the optional parameters
 */
extern
void __arm_2d_log_printf(int32_t nIndentLevel, 
                         uint32_t wChannelMask,
                         const char *pchPrefix,
                         const char *pchFormatString,
                         ...);


#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif


#endif  /* end of __ARM_2D_UTILS_H__ */


/*! @} */

/*============================ MACROS ========================================*/
/*----------------------------------------------------------------------------*
 * Reentrant Macros                                                           *
 *----------------------------------------------------------------------------*/

/* un-define macros */
#undef ARM_PRIVATE
#undef ARM_PROTECTED

/* redefine macros */
#if defined(__cplusplus)
#   define ARM_PRIVATE(...)                                                     \
        struct  {                                                               \
            __VA_ARGS__                                                         \
        };

#   define ARM_PROTECTED(...)                                                   \
        struct  {                                                               \
            __VA_ARGS__                                                         \
        };

#elif defined(__ARM_2D_IMPL__) || defined(__IS_COMPILER_IAR__)

#   define ARM_PRIVATE(...)                                                     \
        struct  {                                                               \
            __VA_ARGS__                                                         \
        } __ALIGNED(__alignof__(struct {__VA_ARGS__}));

#   define ARM_PROTECTED(...)                                                   \
        struct  {                                                               \
            __VA_ARGS__                                                         \
        } __ALIGNED(__alignof__(struct {__VA_ARGS__}));

#elif defined(__ARM_2D_INHERIT__)

#   define ARM_PRIVATE(...)                                                     \
        uint8_t ARM_CONNECT3(chMask,__LINE__,__COUNTER__)                       \
                [sizeof(struct {__VA_ARGS__})]                                  \
                __ALIGNED(__alignof__(struct {__VA_ARGS__}));

#   define ARM_PROTECTED(...)                                                   \
        struct  {                                                               \
            __VA_ARGS__                                                         \
        } __ALIGNED(__alignof__(struct {__VA_ARGS__}));

#else
#   define ARM_PRIVATE(...)                                                     \
        uint8_t ARM_CONNECT3(chMask,__LINE__,__COUNTER__)                       \
                [sizeof(struct {__VA_ARGS__})]                                  \
                __ALIGNED(__alignof__(struct {__VA_ARGS__}));

#   define ARM_PROTECTED(...)                                                   \
        uint8_t ARM_CONNECT3(chMask,__LINE__,__COUNTER__)                       \
                [sizeof(struct {__VA_ARGS__})]                                  \
                __ALIGNED(__alignof__(struct {__VA_ARGS__}));
#endif




/* post un-define macros */

#undef __ARM_2D_IMPL__
#undef __ARM_2D_INHERIT__
