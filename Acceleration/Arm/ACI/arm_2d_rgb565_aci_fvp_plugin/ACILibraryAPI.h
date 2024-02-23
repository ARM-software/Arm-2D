/*!
 *  \file   CTModel/Source/CDE/ACILibraryAPI.h
 *  \brief  C-API header for ACI Libraries
 *  \date   Copyright ARM Limited 2013 All Rights Reserved.
 */
#ifndef ACI_API_H
#define ACI_API_H
#include <stdint.h>
#include <stdbool.h>
#define ACI_API_VERSION 1
#ifndef _WIN32
    #define ACI_EXPORT __attribute__((visibility ("default")))
#else
    #define ACI_EXPORT __declspec(dllexport)
#endif
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------------------------------------------
/**
 * ACIHandle
 * @brief Opaque type specifying a handle for the ACI library.
 * This handle will be passed to all the aci_exec_* functions.
 **/
typedef struct ACIHandleInstance* ACIHandle;
/*!
 * @brief Returns the library version.
 * @return library version (ACI_API_VERSION).
 */
ACI_EXPORT uint16_t aci_get_library_version(void);
/*!
 * @brief Returns the library name.
 * The library name will be used to select this library with the model parameter "cpu.cde_impl_name".
 * This can be used to quickly change which library is used when providing multiple libraries.
 * @return library name.
 */
ACI_EXPORT const char* aci_get_library_name(void);
/*!
 * @brief Returns the coprocessor mask.
 * @return coprocessor mask.
 */
ACI_EXPORT uint8_t aci_get_coprocessor_mask(void);
/*!
 * @brief Sets the paramater provided to the model for the ACI library.
 * @param [in] parameter a generic string that can processed in any way that fits the library's implementation.
 * This can be set using the model's parameter "CDELoader.aci_parameters".
 */
ACI_EXPORT void aci_set_param(const char* parameter);
/*!
 * @brief Constructor of the ACI library.
 * @return handle object, can be nullptr if there is no handle needed.
 */
ACI_EXPORT ACIHandle aci_new(void);
/*!
 * @brief Destructor of the ACI library.
 * @param [in] handle object to free, can be ignored if there was no handle allocated.
 */
ACI_EXPORT void aci_free(ACIHandle handle);
//----------------------------------------------------------------------------------------
/*!
* @brief Status type alias for aci_exec_* functions
*/
typedef int ACI_Status;
/*!
* @brief Success.
* 0 is an arbitrary code number for the status.
*/
#define ACI_STATUS_OK (0)
/*!
* @brief Instruction not implemented.
* 100 is an arbitrary code number for the status.
*/
#define ACI_STATUS_NOT_IMPLEMENTED (100)
/*! @struct ACICX1DecodeInfo
 *  @brief Decode information of CX1 instruction
 *
 *  @var accumulate Accumulate with existing register contents
 *  @var coproc Number of coproc
 *  @var imm Immediate value
 *  @var rd_num General-purpose destination register
 */
typedef struct
{
    bool     accumulate;
    uint32_t coproc;
    uint32_t imm;
    uint32_t rd_num;
} ACICX1DecodeInfo;
/*!
 * @brief Instruction CX1
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the CX1 instruction.
 * @param [in] rd_val value of the destination register.
 * @param [out] result pointer to the value returned by the CX1 instruction.
 */
ACI_EXPORT ACI_Status aci_exec_cx1(ACIHandle               handle,
                                   const ACICX1DecodeInfo* decode_info,
                                   uint32_t                rd_val,
                                   uint32_t*               result);
/*!
 * @brief Instruction CX1D
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the CX1D instruction.
 * @param [in] rfd_val value of the first of destination register pair.
 * @param [out] result pointer to the value returned by the CX1D instruction.
 */
ACI_EXPORT ACI_Status aci_exec_cx1_d(ACIHandle               handle,
                                     const ACICX1DecodeInfo* decode_info,
                                     uint64_t                rfd_val,
                                     uint64_t*               result);
/*! @struct ACICX2DecodeInfo
 *  @brief Decode information of CX2 instruction
 *
 *  @var accumulate Accumulate with existing register contents
 *  @var coproc Number of coproc
 *  @var imm Immediate value
 *  @var rd_num General-purpose destination register
 *  @var rn_num General-purpose source register
 */
typedef struct
{
    bool     accumulate;
    uint32_t coproc;
    uint32_t imm;
    uint32_t rd_num;
    uint32_t rn_num;
} ACICX2DecodeInfo;
/*!
 * @brief Instruction CX2
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the CX2 instruction.
 * @param [in] rd_val value of the destination register.
 * @param [in] rn_val value of the source register.
 * @param [out] result pointer to the value returned by the CX2 instruction.
 */
ACI_EXPORT ACI_Status aci_exec_cx2(ACIHandle                handle,
                                   const ACICX2DecodeInfo*  decode_info,
                                   uint32_t                 rd_val,
                                   uint32_t                 rn_val,
                                   uint32_t*                result);
/*!
 * @brief Instruction CX2D
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the CX2D instruction.
 * @param [in] rfd_val value of the first of destination register pair.
 * @param [in] rn_val value of the source register.
 * @param [out] result pointer to the value returned by the CX2D instruction.
*/
ACI_EXPORT ACI_Status aci_exec_cx2_d(ACIHandle               handle,
                                     const ACICX2DecodeInfo* decode_info,
                                     uint64_t                rfd_val,
                                     uint32_t                rn_val,
                                     uint64_t*               result);
/*! @struct ACICX3DecodeInfo
 *  @brief Decode information of CX3 instruction
 *
 *  @var accumulate Accumulate with existing register contents
 *  @var coproc Number of coproc
 *  @var imm Immediate value
 *  @var rd_num General-purpose destination register
 *  @var rn_num General-purpose source register
 *  @var rm_num General-purpose source register
 */
typedef struct
{
    bool     accumulate;
    uint32_t coproc;
    uint32_t imm;
    uint32_t rd_num;
    uint32_t rn_num;
    uint32_t rm_num;
} ACICX3DecodeInfo;
/*!
 * @brief Instruction CX3
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the CX3 instruction.
 * @param [in] rd_val value of the destination register.
 * @param [in] rn_val value of the source register.
 * @param [in] rm_val value of the source register.
 * @param [out] result pointer to the value returned by the CX3 instruction.
 */
ACI_EXPORT ACI_Status aci_exec_cx3(ACIHandle               handle,
                                   const ACICX3DecodeInfo* decode_info,
                                   uint32_t                rd_val,
                                   uint32_t                rn_val,
                                   uint32_t                rm_val,
                                   uint32_t*               result);
/*!
 * @brief Instruction CX3D
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the CX3D instruction.
 * @param [in] rfd_val value of the first of destination register pair.
 * @param [in] rn_val value of the source register.
 * @param [in] rm_val value of the source register.
 * @param [out] result pointer to the value returned by the CX3D instruction.
 */
ACI_EXPORT ACI_Status aci_exec_cx3_d(ACIHandle              handle,
                                    const ACICX3DecodeInfo* decode_info,
                                    uint64_t                rfd_val,
                                    uint32_t                rn_val,
                                    uint32_t                rm_val,
                                    uint64_t*               result);
// FP/SIMD Register CDE accessing instruction variants
/*! @struct ACIVCX1DecodeInfo
 *  @brief Decode information of VCX1 instruction
 *
 *  @var accumulate Accumulate with existing register contents
 *  @var coproc Number of coproc
 *  @var imm Immediate value
 *  @var vd_num Source and destination vector register
 */
typedef struct
{
    bool     accumulate;
    uint32_t coproc;
    uint32_t imm;
    uint32_t vd_num;
} ACIVCX1DecodeInfo;
/*!
 * @brief Instruction VCX1 with S register
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the VCX1 instruction.
 * @param [in] sd_val 32-bit value of the floating-point source and destination register
 * @param [out] result pointer to the value returned by the VCX1 instruction.
 */
ACI_EXPORT ACI_Status aci_exec_vcx1_s(ACIHandle                handle,
                                      const ACIVCX1DecodeInfo* decode_info,
                                      uint32_t                 sd_val,
                                      uint32_t*                result);
/*!
 * @brief Instruction VCX1 with D register
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the VCX1 instruction.
 * @param [in] dd_val 64-bit value of the floating-point source and destination register
 * @param [out] result pointer to the value returned by the VCX1 instruction.
 */
ACI_EXPORT ACI_Status aci_exec_vcx1_d(ACIHandle                handle,
                                      const ACIVCX1DecodeInfo* decode_info,
                                      uint64_t                 dd_val,
                                      uint64_t*                result);
/*!
 * @brief Instruction VCX1 for one beat. Caller handles predicated writeback.
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the VCX1 instruction.
 * @param [in] d_val 32-bit value of the floating-point source and destination register
 * @param [in] beat beat-number to index into the Q regs
 * @param [in] elmt_mask
 * @param [out] result pointer to the value returned by the VCX1 instruction.
 */
ACI_EXPORT ACI_Status aci_exec_vcx1_beatwise(ACIHandle                handle,
                                             const ACIVCX1DecodeInfo* decode_info,
                                             uint32_t                 d_val,
                                             uint8_t                  beat,
                                             uint8_t                  elmt_mask,
                                             uint32_t*                result);
/*! @struct ACIVCX2DecodeInfo
 *  @brief Decode information of VCX2 instruction
 *
 *  @var accumulate Accumulate with existing register contents
 *  @var coproc Number of coproc
 *  @var imm Immediate value
 *  @var vd_num Source and destination vector register
 *  @var vm_num Source vector register
 */
typedef struct
{
    bool     accumulate;
    uint32_t coproc;
    uint32_t imm;
    uint32_t vd_num;
    uint32_t vm_num;
} ACIVCX2DecodeInfo;
/*!
 * @brief Instruction VCX2 with S register
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the VCX2 instruction.
 * @param [in] sd_val 32-bit value of the floating-point source and destination register
 * @param [in] sm_val 32-bit value of the floating-point source register.
 * @param [out] result pointer to the value returned by the VCX2 instruction.
 * @return ACI_STATUS_OK on success, ACI_STATUS_NOT_IMPLEMENTED if instruction not implemented.
 */
ACI_EXPORT ACI_Status aci_exec_vcx2_s(ACIHandle                handle,
                                      const ACIVCX2DecodeInfo* decode_info,
                                      uint32_t                 sd_val,
                                      uint32_t                 sm_val,
                                      uint32_t*                result);
/*!
 * @brief Instruction VCX2 with D register
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the VCX2 instruction.
 * @param [in] dd_val 64-bit value of the source and destination vector register.
 * @param [in] dm_val 64-bit value of the source vector register.
 * @param [out] result pointer to the value returned by the VCX2 instruction.
 * @return ACI_STATUS_OK on success, ACI_STATUS_NOT_IMPLEMENTED if instruction not implemented.
 */
ACI_EXPORT ACI_Status aci_exec_vcx2_d(ACIHandle                handle,
                                      const ACIVCX2DecodeInfo* decode_info,
                                      uint64_t                 dd_val,
                                      uint64_t                 dm_val,
                                      uint64_t*                result);
/*!
 * @brief Instruction VCX2 for one beat. Caller handles predicated writeback.
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the VCX2 instruction.
 * @param [in] d_val 32-bit value of the source and destination vector register
 * @param [in] m_val 32-bit value of the source vector register
 * @param [in] beat beat-number to index into the Q regs
 * @param [in] elmt_mask
 * @param [out] result pointer to the value returned by the VCX2 instruction.
 */
ACI_EXPORT ACI_Status aci_exec_vcx2_beatwise(ACIHandle                handle,
                                             const ACIVCX2DecodeInfo* decode_info,
                                             uint32_t                 d_val,
                                             uint32_t                 m_val,
                                             uint8_t                  beat,
                                             uint8_t                  elmt_mask,
                                             uint32_t*                result);
/*! @struct ACIVCX3DecodeInfo
 *  @brief Decode information of VCX3 instruction
 *
 *  @var accumulate Accumulate with existing register contents
 *  @var coproc Number of coproc
 *  @var imm Immediate value
 *  @var vd_num Source and destination vector register
 *  @var vn_num Source vector register
 *  @var vm_num Source vector register
 */
typedef struct
{
    bool     accumulate;
    uint32_t coproc;
    uint32_t imm;
    uint32_t vd_num;
    uint32_t vn_num;
    uint32_t vm_num;
} ACIVCX3DecodeInfo;
/*!
 * @brief Instruction VCX3 with S register
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the VCX3 instruction.
 * @param [in] sd_val 32-bit value of the floating-point source and destination register
 * @param [in] sn_val 32-bit value of the floating-point source register.
 * @param [in] sm_val 32-bit value of the floating-point source register.
 * @param [out] result pointer to the value returned by the VCX3 instruction.
 * @return ACI_STATUS_OK on success, ACI_STATUS_NOT_IMPLEMENTED if instruction not implemented.
 */
ACI_EXPORT ACI_Status aci_exec_vcx3_s(ACIHandle                handle,
                                      const ACIVCX3DecodeInfo* decode_info,
                                      uint32_t                 sd_val,
                                      uint32_t                 sn_val,
                                      uint32_t                 sm_val,
                                      uint32_t*                result);
/*!
 * @brief Instruction VCX3 with D register
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the VCX3 instruction.
 * @param [in] dd_val 64-bit value of the floating-point source and destination register
 * @param [in] dn_val 64-bit value of the floating-point source register.
 * @param [in] dm_val 64-bit value of the floating-point source register.
 * @param [out] result pointer to the value returned by the VCX3 instruction.
 * @return ACI_STATUS_OK on success, ACI_STATUS_NOT_IMPLEMENTED if instruction not implemented.
 */
ACI_EXPORT ACI_Status aci_exec_vcx3_d(ACIHandle                handle,
                                      const ACIVCX3DecodeInfo* decode_info,
                                      uint64_t                 dd_val,
                                      uint64_t                 dn_val,
                                      uint64_t                 dm_val,
                                      uint64_t*                result);
/*!
 * @brief Instruction VCX3 for one beat. Caller handles predicated writeback.
 * @param [in] handle aci handle object created by aci_new().
 * @param [in] decode_info decoded fields for the VCX3 instruction.
 * @param [in] d_val 32-bit value of the source and destination vector register
 * @param [in] n_val 32-bit value of the source vector register
 * @param [in] m_val 32-bit value of the source vector register
 * @param [in] beat beat-number to index into the Q regs
 * @param [in] elmt_mask
 * @param [out] result pointer to the value returned by the VCX3 instruction.
 */
ACI_EXPORT ACI_Status aci_exec_vcx3_beatwise(ACIHandle                handle,
                                             const ACIVCX3DecodeInfo* decode_info,
                                             uint32_t                 d_val,
                                             uint32_t                 n_val,
                                             uint32_t                 m_val,
                                             uint8_t                  beat,
                                             uint8_t                  elmt_mask,
                                             uint32_t*                result);
//----------------------------------------------------------------------------------------
/*! @struct ACIMnemonics
 *  @brief Custom mnemonics for the custom instructions.
 */
typedef struct
{
    const char* cx1;        /**< Instruction CX1 mnemonic                   */
    const char* cx1a;       /**< Instruction CX1A mnemonic                  */
    const char* cx1d;       /**< Instruction CX1D mnemonic                  */
    const char* cx1da;      /**< Instruction CX1DA mnemonic                 */
    const char* cx2;        /**< Instruction CX2 mnemonic                   */
    const char* cx2a;       /**< Instruction CX2A mnemonic                  */
    const char* cx2d;       /**< Instruction CX2D mnemonic                  */
    const char* cx2da;      /**< Instruction CX2DA mnemonic                 */
    const char* cx3;        /**< Instruction CX3 mnemonic                   */
    const char* cx3a;       /**< Instruction CX3A mnemonic                  */
    const char* cx3d;       /**< Instruction CX3D mnemonic                  */
    const char* cx3da;      /**< Instruction CX3DA mnemonic                 */
    const char* vcx1_s;     /**< Instruction VCX1 with S register mnemonic  */
    const char* vcx1a_s;    /**< Instruction VCX1A with S register mnemonic */
    const char* vcx1_d;     /**< Instruction VCX1 with D register mnemonic  */
    const char* vcx1a_d;    /**< Instruction VCX1A with D register mnemonic */
    const char* vcx1_v;     /**< Instruction VCX1 vector mnemonic           */
    const char* vcx1a_v;    /**< Instruction VCX1A vector mnemonic          */
    const char* vcx2_s;     /**< Instruction VCX2 with S register mnemonic  */
    const char* vcx2a_s;    /**< Instruction VCX2A with S register mnemonic */
    const char* vcx2_d;     /**< Instruction VCX2 with D register mnemonic  */
    const char* vcx2a_d;    /**< Instruction VCX2A with D register mnemonic */
    const char* vcx2_v;     /**< Instruction VCX2 vector mnemonic           */
    const char* vcx2a_v;    /**< Instruction VCX2A vector mnemonic          */
    const char* vcx3_s;     /**< Instruction VCX3 with S register mnemonic  */
    const char* vcx3a_s;    /**< Instruction VCX3A with S register mnemonic */
    const char* vcx3_d;     /**< Instruction VCX3 with D register mnemonic  */
    const char* vcx3a_d;    /**< Instruction VCX3A with D register mnemonic */
    const char* vcx3_v;     /**< Instruction VCX3 vector mnemonic           */
    const char* vcx3a_v;    /**< Instruction VCX3A vector mnemonic          */
} ACIMnemonics;
/*!
 * @brief Returns a list of custom mnemonics for the custom instructions.
 * @param [out] mnemonics pointer to the location of the object that has the list of custom mnemonics.
 * @return ACI_STATUS_OK if a pointer is provided, ACI_STATUS_NOT_IMPLEMENTED for default mnemonics.
 */
ACI_EXPORT ACI_Status aci_get_custom_mnemonics(ACIHandle handle, ACIMnemonics* mnemonics);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* ACI_API_H */