/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
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
 * Title:        arm_2d_rgb565_aci_fvp.h
 * Description:  Arm-2D RGB565 ACI FVP emulation Library
 *
 * $Date:        23. Jan 2024
 * $Revision:    V.0.0.1
 *
 * -------------------------------------------------------------------- */



#ifndef ARM_2D_RGB565_ACI_LIB
#define ARM_2D_RGB565_ACI_LIB

#include "ACILibraryAPI.h"

#include <cstdio>
#define ARM_2D_RGB565_ACI_LIB_COPRO_ID  0


class arm_2d_rgb565_aci_fvp
{
private:
    ACIMnemonics mnemonics {
        /* invalid */
        "NOT_IMPL1",       // cx1
        "NOT_IMPL1A",      // cx1a
        "NOT_IMPL1D",      // cx1d
        "NOT_IMPL1DA",     // cx1da
        "NOT_IMPL2",       // cx2
        "NOT_IMPL2A",      // cx2a
        "NOT_IMPL2D",      // cx2d
        "NOT_IMPL2DA",     // cx2da
        "NOT_IMPL3",       // cx3
        "NOT_IMPL3A",      // cx3a
        "NOT_IMPL3D",      // cx3d
        "NOT_IMPL3DA",     // cx3da
        "VNOT_IMPL1",      // vcx1_s
        "VNOT_IMPL1A",     // vcx1a_s
        "VNOT_IMPL1",      // vcx1_d
        "VNOT_IMPL1A",     // vcx1a_d
        "VNOT_IMPL1",      // vcx1_v
        "VNOT_IMPL1A",     // vcx1a_v
        "VNOT_IMPL2",      // vcx2_s
        "VNOT_IMPL2A",     // vcx2a_s
        "VNOT_IMPL2",      // vcx2_d
        "VNOT_IMPL2A",     // vcx2a_d
        "VRGBPCKUNPK",     // vcx2_v
        "VRGBPCKUNPKACC",  // vcx2a_v
        "VNOT_IMPL3",      // vcx3_s
        "VNOT_IMPL3A",     // vcx3a_s
        "VNOT_IMPL3",      // vcx3_d
        "VNOT_IMPL3A",     // vcx3a_d
        "VNOT_IMPL3",      // vcx3_v

        /* valid */
        "VRGB16MIX"        // vcx3a_v
    };

public:
    ACIMnemonics get_mnemonics() { return mnemonics;};

    uint8_t getCDECoprocessorMask() {
        return 1 << ARM_2D_RGB565_ACI_LIB_COPRO_ID;};

    bool isCDECoprocessorValid(uint32_t coproc) {
        return (!((1 << coproc) & aci_get_coprocessor_mask()) || (coproc != ARM_2D_RGB565_ACI_LIB_COPRO_ID)) ? false : true; };

    ACI_Status exec_cx1(const ACICX1DecodeInfo* decode_info,
                        uint32_t                rd_val,
                        uint32_t*               result);

    ACI_Status exec_cx1_d(const ACICX1DecodeInfo* decode_info,
                        uint64_t                rfd_val,
                        uint64_t*               result);

    ACI_Status exec_cx2(const ACICX2DecodeInfo*  decode_info,
                        uint32_t                 rd_val,
                        uint32_t                 rn_val,
                        uint32_t*                result);

    ACI_Status exec_cx2_d(const ACICX2DecodeInfo* decode_info,
                          uint64_t                rfd_val,
                          uint64_t                rn_val,
                          uint64_t*               result);

    ACI_Status exec_cx3(const ACICX3DecodeInfo* decode_info,
                        uint32_t                rd_val,
                        uint32_t                rn_val,
                        uint32_t                rm_val,
                        uint32_t*               result);

    ACI_Status exec_cx3_d(const ACICX3DecodeInfo* decode_info,
                          uint64_t                rfd_val,
                          uint64_t                rn_val,
                          uint64_t                rm_val,
                          uint64_t*               result);

    ACI_Status exec_vcx1_s(const ACIVCX1DecodeInfo* decode_info,
                           uint32_t                 sd_val,
                           uint32_t*                result);

    ACI_Status exec_vcx1_d(const ACIVCX1DecodeInfo* decode_info,
                           uint64_t                 dd_val,
                           uint64_t*                result);

    ACI_Status exec_vcx1_beatwise(const ACIVCX1DecodeInfo* decode_info,
                                  uint32_t                 d_val,
                                  uint8_t                  beat,
                                  uint8_t                  elmt_mask,
                                  uint32_t*                result);

    ACI_Status exec_vcx2_s(const ACIVCX2DecodeInfo* decode_info,
                           uint32_t                 sd_val,
                           uint32_t                 sm_val,
                           uint32_t*                result);

    ACI_Status exec_vcx2_d(const ACIVCX2DecodeInfo* decode_info,
                           uint64_t                 dd_val,
                           uint64_t                 dm_val,
                           uint64_t*                result);

    ACI_Status exec_vcx2_beatwise(const ACIVCX2DecodeInfo* decode_info,
                                  uint32_t                 d_val,
                                  uint32_t                 m_val,
                                  uint8_t                  beat,
                                  uint8_t                  elmt_mask,
                                  uint32_t*                result);

    ACI_Status exec_vcx3_s(const ACIVCX3DecodeInfo* decode_info,
                           uint32_t                 sd_val,
                           uint32_t                 sn_val,
                           uint32_t                 sm_val,
                           uint32_t*                result);

    ACI_Status exec_vcx3_d(const ACIVCX3DecodeInfo* decode_info,
                           uint64_t                 dd_val,
                           uint64_t                 dn_val,
                           uint64_t                 dm_val,
                           uint64_t*                result);

    ACI_Status exec_vcx3_beatwise(const ACIVCX3DecodeInfo* decode_info,
                                  uint32_t                 d_val,
                                  uint32_t                 n_val,
                                  uint32_t                 m_val,
                                  uint8_t                  beat,
                                  uint8_t                  elmt_mask,
                                  uint32_t*                result);
};

#endif // ARM_2D_RGB565_ACI_LIB

