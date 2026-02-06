/*
 * Copyright (c) 2009-2026 Arm Limited. All rights reserved.
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

/*============================ INCLUDES ======================================*/


#include "lmsk_decoder.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#undef this
#define this (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


static inline 
bool arm_lmsk_decoder_seek(arm_lmsk_decoder_t *ptThis, int32_t nOffset)
{
    return this.tCFG.IO.fnSeek(this.tCFG.IO.pTarget, nOffset);
}

static inline 
size_t arm_lmsk_decoder_read(   arm_lmsk_decoder_t *ptThis,  
                                uint8_t *pchBuffer,
                                size_t tLength)
{
    return this.tCFG.IO.fnRead(this.tCFG.IO.pTarget, pchBuffer, tLength);
}

int arm_lmsk_decoder_init(  arm_lmsk_decoder_t *ptThis, 
                            arm_lmsk_decoder_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);

    memset(ptThis, 0, sizeof(arm_lmsk_decoder_t));

    this.tCFG = *ptCFG;

    if (NULL == this.tCFG.IO.fnSeek || NULL == this.tCFG.IO.fnRead) {
        return -1;
    }

    /* read header and palette */
    do {
        arm_lmsk_decoder_seek(ptThis, 0);

        arm_lmsk_header_t tHeader;

        if (sizeof(tHeader) 
        !=  arm_lmsk_decoder_read(ptThis, (uint8_t *)&tHeader, sizeof(tHeader))) {
            return -1;
        }

        /* check signature */
        if (0 != memcmp(tHeader.chName, "LMSK", 4)) {
            return -1;
        }

        /* check version */
        if (tHeader.Version.chValue > ARM_LMSK_VERSION) {
            return -1;
        }

        /* to do */
        if (tHeader.tSetting.bRaw) {
            return -1;
        }

        /* unsupported in this version */
        if (this.tSetting.u2TagSetBits > 0) {
            return -1;
        }

        this.tSetting = tHeader.tSetting;

        if (sizeof(this.chPalette) 
        !=  arm_lmsk_decoder_read(ptThis, this.chPalette, sizeof(this.chPalette))) {
            return -1;
        }

        this.bValid = true;
    } while(0);


    return 0;
}

#define FLOOR_TABLE_START       (   sizeof(arm_lmsk_header_t)   \
                                +   sizeof(this.chPalette)      )

static inline
int32_t __arm_lmsk_decoder_get_line_start_postion(  arm_lmsk_decoder_t *ptThis,
                                                    int16_t iY)
{
    
    uint_fast8_t chFloors = this.tSetting.chFloorCount;
    if (chFloors > 0) {
        do {
            if (iY >= this.hwCurrentFloor && iY < this.hwNextFloor) {
                /* we are still in the same floor */
                break;
            }

            uint8_t chStartFloorIndex = 0;
            uint32_t wFloorStart = 0;
            if (iY > this.hwCurrentFloor) {
                chStartFloorIndex = this.chLastFloorIndex + 1;
                wFloorStart = this.wFloorStart;
            }

            if (!arm_lmsk_decoder_seek(ptThis, FLOOR_TABLE_START + chStartFloorIndex * sizeof(uint16_t))) {
                return -1;
            }
            
            uint32_t wFloorSize = 1 << (16 - this.tSetting.u2TagSetBits);
            
            this.hwNextFloor = this.tSetting.iHeight;

            for (uint_fast8_t n = chStartFloorIndex; n < chFloors; n++) {
                uint16_t hwFloor;

                if (sizeof(uint16_t) != arm_lmsk_decoder_read(ptThis, (uint8_t *)&hwFloor, sizeof(uint16_t))) {
                    return -1;
                }

                if (hwFloor <= iY) {
                    wFloorStart += wFloorSize;
                    this.hwCurrentFloor = hwFloor;
                    this.chLastFloorIndex = n;
                }

                if (hwFloor > iY) {
                    this.hwNextFloor = hwFloor;
                    break;
                }
            }

            this.wFloorStart = wFloorStart;
        } while(0);
    }

    /* get to line index */
    if (!arm_lmsk_decoder_seek( ptThis, 
                                FLOOR_TABLE_START + iY * sizeof(uint16_t))) {
        return -1;
    }

    /* read line offset */
    uint32_t wLineStart = 0;
    if (sizeof(uint16_t) != arm_lmsk_decoder_read(ptThis, (uint8_t *)&wLineStart, sizeof(uint16_t))) {
        return -1;
    }

    return wLineStart + this.wFloorStart;
}

static
int __arm_lmsk_decode_line( arm_lmsk_decoder_t *ptThis, 
                            uint32_t wLineStartPosition,
                            int16_t iX,
                            int16_t iWidth,
                            uint8_t *pchBuffer)
{

    return 0;
}


int arm_lmsk_decode(arm_lmsk_decoder_t *ptThis,
                    int16_t iX,
                    int16_t iY,
                    int16_t iWidth,
                    int16_t iHeight,
                    uint8_t *pchBuffer,
                    uint32_t wTargetStride)
{
    if (!this.bValid) {
        return -1;
    }

    assert(iX < this.tSetting.iWidth);
    assert(iY < this.tSetting.iHeight);

    uint32_t wDataSectionStart 
        = sizeof(arm_lmsk_header_t)                     /* header */
        + sizeof(this.chPalette)                        /* palette */
        + this.tSetting.chFloorCount * sizeof(uint16_t) /* floor table */
        + this.tSetting.iHeight * sizeof(uint16_t);     /* line index table */

    int16_t iYLimit = iY + iHeight - 1;
    for (; iY < iYLimit; iY++) {
        int32_t iLineOffset = __arm_lmsk_decoder_get_line_start_postion(ptThis, iY);
        if (iLineOffset < 0) {
            return -1;
        }

        uint32_t wLinePostion = wDataSectionStart + iLineOffset;

        if (0 != __arm_lmsk_decode_line(ptThis, wLinePostion, iX, iWidth, pchBuffer)) {
            return -1;
        }

        pchBuffer += wTargetStride;
    }
    return 0;
}


#ifdef   __cplusplus
}
#endif



