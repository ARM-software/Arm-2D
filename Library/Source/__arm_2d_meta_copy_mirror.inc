
/*
 * Copyright (C) 2010-2021 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        __arm_2d_meta_copy_mirror.inc
 * Description:  c code template for tile copying
 *
 * $Date:        13. Sept 2022
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */


#ifndef __API_MCM_COLOUR
#   error You have to define __API_MCM_COLOUR before using this c template
#endif
#ifndef __API_MCM_COLOUR_UPPERCASE
#   error You have to define __API_MCM_COLOUR_UPPERCASE before using this c template
#endif
#ifndef __API_MCM_COLOUR_SZ
#   error You have to define __API_MCM_COLOUR_SZ before using this c template
#endif
#ifndef __API_MCM_INT_TYPE
#   error You have to define the __API_MCM_INT_TYPE before using this c template
#endif
#ifndef __API_MCM_OP
#   error You have to define __API_MCM_OP before using this c template
#endif
#ifndef __API_MCM_OP_IDX
#   error You have to define __API_MCM_OP_IDX before using this c template
#endif
#ifndef __API_MCM_IO
#   error You have to define __API_MCM_IO before using this c template
#endif

#ifndef __API_MCM_MIRROR_TYPE
#   error You have to define __API_MCM_MIRROR_TYPE before using this c template
#endif

#ifndef __API_MCM_MIRROR_TYPE_UPPERCASE
#   error You have to define __API_MCM_MIRROR_TYPE_UPPERCASE before using this c template
#endif

#ifndef __API_MCM_OP_TYPE
#   error You have to define __API_MCM_OP_TYPE before using this c template
#endif

#undef ____MCM_OPCODE
#define ___MCM_OPCODE(__OP, __MIRROR, __COLOUR)    __OP##_##__MIRROR##_##__COLOUR

#undef __MCM_OPCODE
#define ____MCM_OPCODE(__OP, __MIRROR, __COLOUR)                                \
            ___MCM_OPCODE(__OP, __MIRROR, __COLOUR)
#define __MCM_OPCODE                                                            \
            ____MCM_OPCODE(  __API_MCM_OP,                                      \
                            __API_MCM_MIRROR_TYPE_UPPERCASE,                    \
                            __API_MCM_COLOUR_UPPERCASE)

#undef ____MCM_IMPL_FUNC
#undef ___MCM_IMPL_FUNC
#undef __MCM_IMPL_FUNC
#define ____MCM_IMPL_FUNC(__NAME, __COLOUR, __MIRROR)                           \
            __arm_2d_impl_##__COLOUR##_##__NAME##_##__MIRROR
#define ___MCM_IMPL_FUNC(__NAME, __COLOUR, __MIRROR)                            \
            ____MCM_IMPL_FUNC(__NAME, __COLOUR, __MIRROR) 
#define __MCM_IMPL_FUNC(__NAME)                                                 \
            ___MCM_IMPL_FUNC(__NAME, __API_MCM_COLOUR, __API_MCM_MIRROR_TYPE)


#undef ____MCM_SW_FUNC
#undef ___MCM_SW_FUNC
#undef __MCM_SW_FUNC
#define ____MCM_SW_FUNC(__NAME, __COLOUR, __MIRROR)                             \
            __arm_2d_##__COLOUR##_sw_##__NAME##_##__MIRROR
#define ___MCM_SW_FUNC(__NAME, __COLOUR, __MIRROR)                              \
            ____MCM_SW_FUNC(__NAME, __COLOUR, __MIRROR) 
#define __MCM_SW_FUNC(__NAME)                                                   \
            ___MCM_SW_FUNC(__NAME, __API_MCM_COLOUR, __API_MCM_MIRROR_TYPE)

#undef ____MCM_API_FUNC
#undef ___MCM_API_FUNC
#undef __MCM_API_FUNC
#define ____MCM_API_FUNC(__NAME, __COLOUR, __MIRROR)                            \
            arm_2dp_##__COLOUR##_##__NAME##_with_##__MIRROR
#define ___MCM_API_FUNC(__NAME, __COLOUR, __MIRROR)                             \
            ____MCM_API_FUNC(__NAME, __COLOUR, __MIRROR) 
#define __MCM_API_FUNC(__NAME)                                                  \
            ___MCM_API_FUNC(__NAME, __API_MCM_COLOUR, __API_MCM_MIRROR_TYPE)

#if 0
__WEAK
def_low_lv_io(  ARM_CONNECT(__API_MCM_IO,
                            _,
                            __API_MCM_MIRROR_TYPE_UPPERCASE,
                            _,
                            __API_MCM_COLOUR_UPPERCASE), 
                __MCM_SW_FUNC(tile_copy));



const __arm_2d_op_info_t __MCM_OPCODE = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_CONNECT(ARM_2D_COLOUR_, __API_MCM_COLOUR_UPPERCASE),
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = ARM_CONNECT(  __API_MCM_OP_IDX, 
                                        _,
                                        __API_MCM_MIRROR_TYPE_UPPERCASE),
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(
                            ARM_CONNECT(__API_MCM_IO,
                                    _,
                                    __API_MCM_MIRROR_TYPE_UPPERCASE,
                                    _,
                                    __API_MCM_COLOUR_UPPERCASE)
                        ),
            .ptFillLike = NULL,
        },
    },
};
#endif

ARM_NONNULL(2,3)
arm_fsm_rt_t __MCM_API_FUNC(tile_copy)( __API_MCM_OP_TYPE *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion)
{

    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(__API_MCM_OP_TYPE, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));
    
    //OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_ONLY_RGB32;
    OP_CORE.ptOp = &__MCM_OPCODE;
    
    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = ARM_CONNECT(ARM_2D_CP_MODE_, __API_MCM_MIRROR_TYPE_UPPERCASE);

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


arm_fsm_rt_t __MCM_SW_FUNC(tile_copy)( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(__API_MCM_OP_TYPE, ptTask->ptOP);
    assert(__API_MCM_COLOUR_SZ == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    __MCM_IMPL_FUNC(copy)    (   ptTask->Param.tCopy.tSource.pBuffer,
                                ptTask->Param.tCopy.tSource.iStride,
                                ptTask->Param.tCopy.tTarget.pBuffer,
                                ptTask->Param.tCopy.tTarget.iStride,
                                &ptTask->Param.tCopy.tCopySize);

    return arm_fsm_rt_cpl;
}


#undef ____MCM_API_FUNC
#undef ___MCM_API_FUNC
#undef __MCM_API_FUNC

#undef ____MCM_SW_FUNC
#undef ___MCM_SW_FUNC
#undef __MCM_SW_FUNC


#undef ____MCM_IMPL_FUNC
#undef ___MCM_IMPL_FUNC
#undef __MCM_IMPL_FUNC

#undef __API_MCM_IO
#undef __API_MCM_MIRROR_TYPE
#undef __API_MCM_MIRROR_TYPE_UPPERCASE
#undef __API_MCM_OP_IDX
#undef __API_MCM_OP_TYPE
#undef __API_MCM_OP
#undef __API_MCM_COLOUR_SZ
#undef __API_MCM_COLOUR_UPPERCASE
#undef __API_MCM_COLOUR
#undef __API_MCM_INT_TYPE
