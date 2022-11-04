
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
 * Title:        __arm_2d_meta_copy_with_src_mask_and_mirror.inc
 * Description:  c code template for tile copy with mirroring and the source mask
 *
 * $Date:        04. Nov 2022
 * $Revision:    V.1.0.1
 *
 * -------------------------------------------------------------------- */


#ifndef __API_MCSMM_COLOUR
#   error You have to define __API_MCSMM_COLOUR before using this c template
#endif
#ifndef __API_MCSMM_COLOUR_UPPERCASE
#   error You have to define __API_MCSMM_COLOUR_UPPERCASE before using this c template
#endif
#ifndef __API_MCSMM_COLOUR_SZ
#   error You have to define __API_MCSMM_COLOUR_SZ before using this c template
#endif
#ifndef __API_MCSMM_INT_TYPE
#   error You have to define the __API_MCSMM_INT_TYPE before using this c template
#endif
#ifndef __API_MCSMM_OP
#   error You have to define __API_MCSMM_OP before using this c template
#endif
#ifndef __API_MCSMM_OP_IDX
#   error You have to define __API_MCSMM_OP_IDX before using this c template
#endif
#ifndef __API_MCSMM_IO
#   error You have to define __API_MCSMM_IO before using this c template
#endif

#ifndef __API_MCSMM_MIRROR_TYPE
#   error You have to define __API_MCSMM_MIRROR_TYPE before using this c template
#endif

#ifndef __API_MCSMM_MIRROR_TYPE_UPPERCASE
#   error You have to define __API_MCSMM_MIRROR_TYPE_UPPERCASE before using this c template
#endif

#ifndef __API_MCSMM_OP_TYPE
#   error You have to define __API_MCSMM_OP_TYPE before using this c template
#endif

#undef ____MCSMM_OPCODE
#define ___MCSMM_OPCODE(__OP, __MIRROR, __COLOUR)    __OP##_##__MIRROR##_##__COLOUR

#undef __MCSMM_OPCODE
#define ____MCSMM_OPCODE(__OP, __MIRROR, __COLOUR)                               \
            ___MCSMM_OPCODE(__OP, __MIRROR, __COLOUR)
#define __MCSMM_OPCODE                                                           \
            ____MCSMM_OPCODE(  __API_MCSMM_OP,                                    \
                            __API_MCSMM_MIRROR_TYPE_UPPERCASE,                   \
                            __API_MCSMM_COLOUR_UPPERCASE)

#undef ____MCSMM_IMPL_FUNC
#undef ___MCSMM_IMPL_FUNC
#undef __MCSMM_IMPL_FUNC
#define ____MCSMM_IMPL_FUNC(__NAME, __COLOUR, __MIRROR)                          \
            __arm_2d_impl_##__COLOUR##_##__NAME##_##__MIRROR
#define ___MCSMM_IMPL_FUNC(__NAME, __COLOUR, __MIRROR)                           \
            ____MCSMM_IMPL_FUNC(__NAME, __COLOUR, __MIRROR) 
#define __MCSMM_IMPL_FUNC(__NAME)                                                \
            ___MCSMM_IMPL_FUNC(__NAME, __API_MCSMM_COLOUR, __API_MCSMM_MIRROR_TYPE)


#undef ____MCSMM_SW_FUNC
#undef ___MCSMM_SW_FUNC
#undef __MCSMM_SW_FUNC
#define ____MCSMM_SW_FUNC(__NAME, __COLOUR, __MIRROR)                            \
            __arm_2d_##__COLOUR##_sw_##__NAME##_and_##__MIRROR
#define ___MCSMM_SW_FUNC(__NAME, __COLOUR, __MIRROR)                             \
            ____MCSMM_SW_FUNC(__NAME, __COLOUR, __MIRROR) 
#define __MCSMM_SW_FUNC(__NAME)                                                  \
            ___MCSMM_SW_FUNC(__NAME, __API_MCSMM_COLOUR, __API_MCSMM_MIRROR_TYPE)

#undef ____MCSMM_API_FUNC
#undef ___MCSMM_API_FUNC
#undef __MCSMM_API_FUNC
#define ____MCSMM_API_FUNC(__NAME, __COLOUR, __MIRROR)                           \
            arm_2dp_##__COLOUR##_##__NAME##_and_##__MIRROR
#define ___MCSMM_API_FUNC(__NAME, __COLOUR, __MIRROR)                            \
            ____MCSMM_API_FUNC(__NAME, __COLOUR, __MIRROR) 
#define __MCSMM_API_FUNC(__NAME)                                                 \
            ___MCSMM_API_FUNC(__NAME, __API_MCSMM_COLOUR, __API_MCSMM_MIRROR_TYPE)


ARM_NONNULL(2,3)
arm_fsm_rt_t __MCSMM_API_FUNC(tile_copy_with_src_mask)(
                                        __API_MCSMM_OP_TYPE *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);
    
    ARM_2D_IMPL(__API_MCSMM_OP_TYPE, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(
                                ptSource, 
                                ptSrcMask,
                                ptTarget, 
                                NULL, 
                                ARM_CONNECT(ARM_2D_CP_MODE_, 
                                            __API_MCSMM_MIRROR_TYPE_UPPERCASE));
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &__MCSMM_OPCODE;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = ARM_CONNECT(ARM_2D_CP_MODE_, __API_MCSMM_MIRROR_TYPE_UPPERCASE);
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = NULL;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


arm_fsm_rt_t __MCSMM_SW_FUNC(tile_copy_with_src_mask)( __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(__API_MCSMM_OP_TYPE, ptTask->ptOP);
    assert(__API_MCSMM_COLOUR_SZ == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    //! this should not happen
    assert(NULL != this.Mask.ptSourceSide);

    if (ptTask->Param.tCopyMask.tSrcMask.bInvalid) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme) {
    
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__

            __MCSMM_IMPL_FUNC(src_chn_msk_copy)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,
                    
                &ptTask->Param.tCopy.tCopySize);
    #else
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #endif
    } else {
        assert(     ARM_2D_COLOUR_8BIT 
              ==    ptTask->Param.tCopyMask.tSrcMask.tColour.chScheme);

            __MCSMM_IMPL_FUNC(src_msk_copy)(
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tSource.iStride,

                ptTask->Param.tCopyMask.tSrcMask.pBuffer,
                ptTask->Param.tCopyMask.tSrcMask.iStride,
                &ptTask->Param.tCopyMask.tSrcMask.tValidRegion.tSize,

                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.pBuffer,
                ptTask->Param.tCopyMask
                    .use_as____arm_2d_param_copy_t.tTarget.iStride,
                    
                &ptTask->Param.tCopy.tCopySize);

    }

    return arm_fsm_rt_cpl;
}


#undef ____MCSMM_API_FUNC
#undef ___MCSMM_API_FUNC
#undef __MCSMM_API_FUNC

#undef ____MCSMM_SW_FUNC
#undef ___MCSMM_SW_FUNC
#undef __MCSMM_SW_FUNC


#undef ____MCSMM_IMPL_FUNC
#undef ___MCSMM_IMPL_FUNC
#undef __MCSMM_IMPL_FUNC

#undef __API_MCSMM_IO
#undef __API_MCSMM_MIRROR_TYPE
#undef __API_MCSMM_MIRROR_TYPE_UPPERCASE
#undef __API_MCSMM_OP_IDX
#undef __API_MCSMM_OP_TYPE
#undef __API_MCSMM_OP
#undef __API_MCSMM_COLOUR_SZ
#undef __API_MCSMM_COLOUR_UPPERCASE
#undef __API_MCSMM_COLOUR
#undef __API_MCSMM_INT_TYPE
