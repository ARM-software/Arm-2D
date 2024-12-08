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
 * $Date:        22. May 2024
 * $Revision:    V.0.0.2
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

void arm_mix_rgb16_uint16x2(uint16_t * out, const uint16_t * in1, const uint16_t * in2,
                            const uint16_t * ratio)
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
        /* ratio encoded with 7-bit*/
        uint16_t        ratioScal = (*ratio++ & 0x7f);
    
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
        tmp = (r1 * ratioScal) + (r2 * (127 - ratioScal));
        r1 = (uint8_t) (tmp >> 7);

        tmp = (g1 * ratioScal) + (g2 * (127 - ratioScal));
        g1 = (uint8_t) (tmp >> 7);

        tmp = (b1 * ratioScal) + (b2 * (127 - ratioScal));
        b1 = (uint8_t) (tmp >> 7);

        /* pack merged stream */
        *out = (r1 >> 3) | ((g1 & maskGpk) << 3) | ((b1 & maskRpk) << 8);
        out++;

        blkCnt -= 1;
    }
    while (blkCnt > 0);

}


#ifdef R_B_CHAN_REVERSED
void arm_pack_rgb16_uint16x2(uint16_t * out, uint16_t * in_b, const uint16_t * in_g,
                             const uint16_t * in_r, bool is_bottom)
#else
void arm_pack_rgb16_uint16x2(uint16_t * out, uint16_t * in_r, const uint16_t * in_g,
                             const uint16_t * in_b, bool is_bottom)
#endif
{

    int32_t         blkCnt;
    uint16_t        maskRpk = 0x00f8;
    uint16_t        maskGpk = 0x00fc;
    blkCnt = 2;

    do {
        uint16_t         R, B, G;

        R = *in_r;
        G = *in_g;
        B = *in_b;

        if (!is_bottom) {
            R >>= 8;
            G >>= 8;
            B >>= 8;
        }


        *out = (B >> 3) | ((G & maskGpk) << 3) | ((R & maskRpk) << 8);

        out++;
        in_r++;
        in_g++;
        in_b++;
        blkCnt -= 1;
    }
    while (blkCnt > 0);
}

void arm_mix_unpack_blue_uint16x2(uint16_t * out, const uint16_t * in1, bool is_bottom)
{

    int32_t         blkCnt;
    uint16_t        maskRunpk = 0x001f;
    blkCnt = 2;
    do {
        uint8_t         R;

        R = *in1++ & maskRunpk;
        R = R << 3;

        *out++ = is_bottom ? R : R << 8;

        blkCnt -= 1;
    }
    while (blkCnt > 0);
}

void arm_mix_unpack_green_uint16x2(uint16_t * out, const uint16_t * in1, bool is_bottom)
{

    int32_t         blkCnt;
    uint16_t        maskGunpk = 0x003f;
    blkCnt = 2;
    do {
        uint8_t         G;

        G = ((*in1++ >> 5) & maskGunpk);
        G = G << 2;

        *out++ = is_bottom ? G : G << 8;

        blkCnt -= 1;
    }
    while (blkCnt > 0);
}

void arm_mix_unpack_red_uint16x2(uint16_t * out, const uint16_t * in1, bool is_bottom)
{

    int32_t         blkCnt;
    blkCnt = 2;
    do {
        uint8_t         B;


        B = *in1++ >> 11;
        B = B << 3;

        *out++ = is_bottom ? B : B << 8;

        blkCnt -= 1;
    }
    while (blkCnt > 0);
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
                                                     uint8_t beat, uint8_t elmt_mask,
                                                     uint32_t * result)
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
                                                     uint8_t beat, uint8_t elmt_mask,
                                                     uint32_t * result)
{

    if (!isCDECoprocessorValid(decode_info->coproc))
        return ACI_STATUS_NOT_IMPLEMENTED;

    //printf("elmt_mask %x imm %x beat %d\n", elmt_mask, decode_info->imm, beat);

    bool is_bottom = (decode_info->imm & 0b1000000) > 0 ? false : true;

    switch (decode_info->imm & 0xf) {
#ifdef R_B_CHAN_REVERSED
      case 0b0000100:
#else
      case 0b0000001:
#endif
          arm_mix_unpack_red_uint16x2((uint16_t *) & d_val, (uint16_t *) & m_val, is_bottom);
          *result = *(uint32_t *) & d_val;
          return ACI_STATUS_OK;
          break;
      case 0b0000010:
          arm_mix_unpack_green_uint16x2((uint16_t *) & d_val, (uint16_t *) & m_val, is_bottom);
          *result = *(uint32_t *) & d_val;
          return ACI_STATUS_OK;
          break;
#ifdef R_B_CHAN_REVERSED
      case 0b0000001:
#else
      case 0b0000100:
#endif
          arm_mix_unpack_blue_uint16x2((uint16_t *) & d_val, (uint16_t *) & m_val, is_bottom);
          *result = *(uint32_t *) & d_val;
          return ACI_STATUS_OK;
          break;
      default:
          return ACI_STATUS_NOT_IMPLEMENTED;

    }

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






ACI_Status arm_2d_rgb565_aci_fvp::exec_vcx3_beatwise(const ACIVCX3DecodeInfo * decode_info,
                                                     uint32_t d_val,
                                                     uint32_t n_val,
                                                     uint32_t m_val,
                                                     uint8_t beat, uint8_t elmt_mask,
                                                     uint32_t * result)
{

    if (!isCDECoprocessorValid(decode_info->coproc))
        return ACI_STATUS_NOT_IMPLEMENTED;




    if (decode_info->accumulate) {
        if ((decode_info->imm & 0xf) == 1)
            arm_mix_rgb16_uint16x2((uint16_t *) & d_val, (uint16_t *) & d_val, (uint16_t *) & n_val,
                                   (uint16_t *) & m_val);
        else {
            bool is_bottom = (decode_info->imm & 0b1000) > 0 ? false : true;
            arm_pack_rgb16_uint16x2((uint16_t *) & d_val, (uint16_t *) & d_val,
                                    (uint16_t *) & n_val, (uint16_t *) & m_val, is_bottom);
        }

        *result = *(uint32_t *) & d_val;
        return ACI_STATUS_OK;
    } else
        return ACI_STATUS_NOT_IMPLEMENTED;

}
