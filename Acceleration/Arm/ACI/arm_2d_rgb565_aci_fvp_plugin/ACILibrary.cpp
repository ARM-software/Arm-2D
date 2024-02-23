/*!
 *  \file   ACILibrary.cpp
 *  \brief  Provides an example implementation for ACILibraryAPI.h
 *  \date   Copyright ARM Limited 2013 All Rights Reserved.
 */

#include "arm_2d_rgb565_aci_fvp.h"
#include "ACILibraryAPI.h"
#include <string>
#include <cstdio>

extern "C"
{

struct ACIHandleInstance{
    arm_2d_rgb565_aci_fvp* lib;
};

uint8_t aci_copro_ena_mask = 0;

uint16_t aci_get_library_version(void)
{
    return ACI_API_VERSION;
}

const char* aci_get_library_name(void)
{
    return "ARM_2D_RGB565_ACI";
}



uint8_t aci_get_coprocessor_mask(void)
{
    //printf("aci_get_coprocessor_mask %x\n", aci_copro_ena_mask);
    return aci_copro_ena_mask;
}




void aci_set_param(const char * parameter)
{
     std::string s(parameter);

    if (s.substr(0, 6) == "COPRO=") {
        std::string xx = s.substr(6, 2);

        int value = stoi(xx, nullptr, 16);
        aci_copro_ena_mask = (uint8_t)value;
    }
}

ACIHandle aci_new(void)
{
    ACIHandle handle = new ACIHandleInstance;
    handle->lib = new arm_2d_rgb565_aci_fvp;

    return handle;
}

void aci_free(ACIHandle handle)
{
    delete handle->lib;
    delete handle;
}

ACI_Status aci_get_custom_mnemonics(ACIHandle handle, ACIMnemonics* mnemonics)
{
    *mnemonics = handle->lib->get_mnemonics();
    return ACI_STATUS_OK;
}

ACI_Status aci_exec_cx1(ACIHandle               handle,
                        const ACICX1DecodeInfo* decode_info,
                        uint32_t                rd_val,
                        uint32_t*               result)
{
    return handle->lib->exec_cx1(decode_info, rd_val, result);
}

ACI_Status aci_exec_cx1_d(ACIHandle               handle,
                          const ACICX1DecodeInfo* decode_info,
                          uint64_t                rfd_val,
                          uint64_t*               result)
{
    return handle->lib->exec_cx1_d(decode_info, rfd_val, result);
}

ACI_Status aci_exec_cx2(ACIHandle                handle,
                        const ACICX2DecodeInfo*  decode_info,
                        uint32_t                 rd_val,
                        uint32_t                 rn_val,
                        uint32_t*                result)
{
    return handle->lib->exec_cx2(decode_info, rd_val, rn_val, result);
}

ACI_Status aci_exec_cx2_d(ACIHandle               handle,
                          const ACICX2DecodeInfo* decode_info,
                          uint64_t                rfd_val,
                          uint32_t                rn_val,
                          uint64_t*               result)
{
    return handle->lib->exec_cx2_d(decode_info, rfd_val, rn_val, result);
}

ACI_Status aci_exec_cx3(ACIHandle               handle,
                        const ACICX3DecodeInfo* decode_info,
                        uint32_t                rd_val,
                        uint32_t                rn_val,
                        uint32_t                rm_val,
                        uint32_t*               result)
{
    return handle->lib->exec_cx3(decode_info, rd_val, rn_val, rm_val, result);
}

ACI_Status aci_exec_cx3_d(ACIHandle              handle,
                          const ACICX3DecodeInfo* decode_info,
                          uint64_t                rfd_val,
                          uint32_t                rn_val,
                          uint32_t                rm_val,
                          uint64_t*               result)
{
    return handle->lib->exec_cx3_d(decode_info, rfd_val, rn_val, rm_val, result);
}


ACI_Status aci_exec_vcx1_s(ACIHandle                handle,
                           const ACIVCX1DecodeInfo* decode_info,
                           uint32_t                 sd_val,
                           uint32_t*                result)
{
    return handle->lib->exec_vcx1_s(decode_info, sd_val, result);
}

ACI_Status aci_exec_vcx1_d(ACIHandle                handle,
                           const ACIVCX1DecodeInfo* decode_info,
                           uint64_t                 dd_val,
                           uint64_t*                result)
{
    return handle->lib->exec_vcx1_d(decode_info, dd_val, result);
}

ACI_Status aci_exec_vcx1_beatwise(ACIHandle                handle,
                                  const ACIVCX1DecodeInfo* decode_info,
                                  uint32_t                 d_val,
                                  uint8_t                  beat,
                                  uint8_t                  elmt_mask,
                                  uint32_t*                result)
{
    return handle->lib->exec_vcx1_beatwise(decode_info, d_val, beat, elmt_mask, result);
}

ACI_Status aci_exec_vcx2_s(ACIHandle                handle,
                           const ACIVCX2DecodeInfo* decode_info,
                           uint32_t                 sd_val,
                           uint32_t                 sm_val,
                           uint32_t*                result)
{
    return handle->lib->exec_vcx2_s(decode_info, sd_val, sm_val, result);
}

ACI_Status aci_exec_vcx2_d(ACIHandle                handle,
                           const ACIVCX2DecodeInfo* decode_info,
                           uint64_t                 dd_val,
                           uint64_t                 dm_val,
                           uint64_t*                result)
{
    return handle->lib->exec_vcx2_d(decode_info, dd_val, dm_val, result);
}

ACI_Status aci_exec_vcx2_beatwise(ACIHandle                handle,
                                  const ACIVCX2DecodeInfo* decode_info,
                                  uint32_t                 d_val,
                                  uint32_t                 m_val,
                                  uint8_t                  beat,
                                  uint8_t                  elmt_mask,
                                  uint32_t*                result)
{
    return handle->lib->exec_vcx2_beatwise(decode_info, d_val, m_val, beat, elmt_mask, result);
}

ACI_Status aci_exec_vcx3_s(ACIHandle                handle,
                           const ACIVCX3DecodeInfo* decode_info,
                           uint32_t                 sd_val,
                           uint32_t                 sn_val,
                           uint32_t                 sm_val,
                           uint32_t*                result)
{
    return handle->lib->exec_vcx3_s(decode_info, sd_val, sn_val, sm_val, result);
}

ACI_Status aci_exec_vcx3_d(ACIHandle                handle,
                           const ACIVCX3DecodeInfo* decode_info,
                           uint64_t                 dd_val,
                           uint64_t                 dn_val,
                           uint64_t                 dm_val,
                           uint64_t*                result)
{
    return handle->lib->exec_vcx3_d(decode_info, dd_val, dn_val, dm_val, result);
}

ACI_Status aci_exec_vcx3_beatwise(ACIHandle                handle,
                                   const ACIVCX3DecodeInfo* decode_info,
                                   uint32_t                 d_val,
                                   uint32_t                 n_val,
                                   uint32_t                 m_val,
                                   uint8_t                  beat,
                                   uint8_t                  elmt_mask,
                                   uint32_t*                result)
{
    return handle->lib->exec_vcx3_beatwise(decode_info, d_val, n_val, m_val, beat, elmt_mask, result);
}

} // extern "C"
