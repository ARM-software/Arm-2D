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
 * Title:        arm_2d_rgb565_aci_fvp.c
 * Description:  Arm-2D RGB565 ACI FVP emulation Library
 *
 * $Date:        23. Jan 2024
 * $Revision:    V.0.0.1
 *
 * -------------------------------------------------------------------- */


#include "arm_2d_rgb565_aci_fvp.h"
#include <valarray>
#include <cstdio>


std::valarray < uint8_t > uint32x1_to_uint8x4(uint32_t val)
{
    std::valarray < uint8_t > v(4);
    for (int i = 0; i < 4; i++)
        v[i] = (val >> (i * 8)) & 0xff;
    return v;
}

uint32_t uint32x4_to_uint32x1(std::valarray < uint8_t > v)
{
    uint32_t        val;

    val = 0;
    for (int i = 0; i < 8; i++)
        val |= ((uint32_t) v[i] & 0xff) << (i * 8);
    return val;
}


ACI_Status arm_2d_rgb565_aci_fvp::exec_cx1(const ACICX1DecodeInfo * decode_info,
                                 uint32_t rd_val, uint32_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}

ACI_Status arm_2d_rgb565_aci_fvp::exec_cx1_d(const ACICX1DecodeInfo * decode_info,
                                   uint64_t rfd_val, uint64_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}

ACI_Status arm_2d_rgb565_aci_fvp::exec_cx2(const ACICX2DecodeInfo * decode_info,
                                 uint32_t rd_val, uint32_t rn_val, uint32_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}

ACI_Status arm_2d_rgb565_aci_fvp::exec_cx2_d(const ACICX2DecodeInfo * decode_info,
                                   uint64_t rfd_val, uint64_t rn_val, uint64_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}

ACI_Status arm_2d_rgb565_aci_fvp::exec_cx3(const ACICX3DecodeInfo * decode_info,
                                 uint32_t rd_val,
                                 uint32_t rn_val, uint32_t rm_val, uint32_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}

ACI_Status arm_2d_rgb565_aci_fvp::exec_cx3_d(const ACICX3DecodeInfo * decode_info,
                                   uint64_t rfd_val,
                                   uint64_t rn_val, uint64_t rm_val, uint64_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}

ACI_Status arm_2d_rgb565_aci_fvp::exec_vcx1_s(const ACIVCX1DecodeInfo * decode_info,
                                    uint32_t sd_val, uint32_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}

ACI_Status arm_2d_rgb565_aci_fvp::exec_vcx1_d(const ACIVCX1DecodeInfo * decode_info,
                                    uint64_t dd_val, uint64_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}


ACI_Status arm_2d_rgb565_aci_fvp::exec_vcx1_beatwise(const ACIVCX1DecodeInfo * decode_info,
                                           uint32_t d_val,
                                           uint8_t beat, uint8_t elmt_mask, uint32_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}

ACI_Status arm_2d_rgb565_aci_fvp::exec_vcx2_s(const ACIVCX2DecodeInfo * decode_info,
                                    uint32_t sd_val, uint32_t sm_val, uint32_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}

ACI_Status arm_2d_rgb565_aci_fvp::exec_vcx2_d(const ACIVCX2DecodeInfo * decode_info,
                                    uint64_t dd_val, uint64_t dm_val, uint64_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}




ACI_Status arm_2d_rgb565_aci_fvp::exec_vcx2_beatwise(const ACIVCX2DecodeInfo * decode_info,
                                           uint32_t d_val,
                                           uint32_t m_val,
                                           uint8_t beat, uint8_t elmt_mask, uint32_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}


ACI_Status arm_2d_rgb565_aci_fvp::exec_vcx3_s(const ACIVCX3DecodeInfo * decode_info,
                                    uint32_t sd_val,
                                    uint32_t sn_val, uint32_t sm_val, uint32_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}

ACI_Status arm_2d_rgb565_aci_fvp::exec_vcx3_d(const ACIVCX3DecodeInfo * decode_info,
                                    uint64_t dd_val,
                                    uint64_t dn_val, uint64_t dm_val, uint64_t * result)
{
    return ACI_STATUS_NOT_IMPLEMENTED;
}



void arm_mix_rgb16_uint16x2(uint16_t * out, const uint16_t * in1, const uint16_t * in2, uint16_t * ratio)
{

    int32_t         blkCnt;
    uint16_t        maskRunpk = 0x001f;
    uint16_t        maskGunpk = 0x003f;
    uint16_t        maskRpk = 0x00f8;
    uint16_t        maskGpk = 0x00fc;
    uint8_t         r1, g1, b1;
    uint8_t         r2, g2, b2;

    blkCnt = 2;
    do {
        uint16_t        packedIn;
        uint8_t         R, B, G;
        int32_t         tmp;

        /* unpack 1st stream */
        packedIn = *in1++;
        R = packedIn & maskRunpk;
        r1 = R << 3;

        B = packedIn >> 11;
        b1 = B << 3;

        G = ((packedIn >> 5) & maskGunpk);
        g1 = G << 2;


        /* unpack 2nd stream */
        packedIn = *in2++;
        R = packedIn & maskRunpk;
        r2 = R << 3;

        B = packedIn >> 11;
        b2 = B << 3;

        G = ((packedIn >> 5) & maskGunpk);
        g2 = G << 2;


        /* merge */
        tmp = (r1 * *ratio) + (r2 * (256 - *ratio));
        r1 = (uint8_t) (tmp >> 8);

        tmp = (g1 * *ratio) + (g2 * (256 - *ratio));
        g1 = (uint8_t) (tmp >> 8);

        tmp = (b1 * *ratio) + (b2 * (256 - *ratio));
        b1 = (uint8_t) (tmp >> 8);

        /* pack merged stream */
        *out = (r1 >> 3) | ((g1 & maskGpk) << 3) | ((b1 & maskRpk) << 8);
        out++;
        ratio ++;

        blkCnt -= 1;
    }
    while (blkCnt > 0);
}



ACI_Status arm_2d_rgb565_aci_fvp::exec_vcx3_beatwise(const ACIVCX3DecodeInfo * decode_info,
                                           uint32_t d_val,
                                           uint32_t n_val,
                                           uint32_t m_val,
                                           uint8_t beat, uint8_t elmt_mask, uint32_t * result)
{

    if (!isCDECoprocessorValid(decode_info->coproc))
        return ACI_STATUS_NOT_IMPLEMENTED;


    /* unpack current 32-bit word into byte array */
    std::valarray < uint8_t > d(4), n(4), m(4);
    m = uint32x1_to_uint8x4(m_val);
    n = uint32x1_to_uint8x4(n_val);

    if (decode_info->accumulate) {
        arm_mix_rgb16_uint16x2((uint16_t*)&d_val, (uint16_t*)&d_val, (uint16_t*)&n_val, (uint16_t*)&m_val);
        *result = *(uint32_t *)&d_val;
        return ACI_STATUS_OK;
    } else
        return ACI_STATUS_NOT_IMPLEMENTED;

}
