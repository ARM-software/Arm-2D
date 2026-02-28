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
#if defined(__ENCODER_COMPILATION__)

#include "lmsk_encoder.h"

#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <inttypes.h>

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#undef this
#define this (*ptThis)

#define CRC32_POLYNOMIAL  0xEDB88320u  /* 反转多项式 */
#define CRC32_INITIAL     0xFFFFFFFFu
#define CRC32_XOROUT      0xFFFFFFFFu

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static uint32_t s_wCRC32Table[256];
static bool s_bCRCTableInitialized = 0;

/*============================ PROTOTYPES ====================================*/
static
void __arm_lmsk_free_output_lines(__arm_lmsk_output_t *ptThis);

static
uint32_t crc32_calculate(const void *data, size_t length);

extern
void __arm_lmsk_encode_line(arm_lmsk_encoder_t *ptThis,
                            __arm_lmsk_line_out_t *ptLine,
                            uint8_t chAlphaMSBBits);
extern
void __arm_lmsk_encoder_prepare(arm_lmsk_encoder_t *ptThis);

extern
void __arm_lmsk_encoder_report(arm_lmsk_encoder_t *ptThis);

/*============================ IMPLEMENTATION ================================*/

arm_lmsk_encoder_t * arm_lmsk_encoder_init( arm_lmsk_encoder_t *ptThis, 
                                            uint8_t *pchMaskBuffer, 
                                            int16_t iWidth, 
                                            int16_t iHeight)
{
    do {
        if (NULL == ptThis || NULL == pchMaskBuffer || iWidth <= 0 || iHeight <= 0) {
            break;
        }
        memset(ptThis, 0, sizeof(arm_lmsk_encoder_t));

        this.Mask.pchBuffer = pchMaskBuffer;
        this.Mask.iWidth = iWidth;
        this.Mask.iHeight = iHeight;

        memcpy(this.tOutput.tHeader.chName, "LMSK", 4);

        this.tOutput.tHeader.Version.chValue = ARM_LMSK_VERSION;
        this.tOutput.tHeader.tSetting.iWidth = iWidth;
        this.tOutput.tHeader.tSetting.iHeight = iHeight;
        //this.tOutput.tHeader.tSetting.u2TagSetBits = 3;

        /* allocate memory for the line index reference table */
        this.tOutput.tLineIndexTable.pwReferences = (uint32_t *)malloc(iHeight * sizeof(uint32_t));
        assert(NULL != this.tOutput.tLineIndexTable.pwReferences);
    } while(0);

    return ptThis;
}

static
__arm_lmsk_line_out_t *__arm_lmsk_find_existing_line(arm_lmsk_encoder_t *ptThis, uint8_t *pchSourceLine)
{
    assert(NULL != ptThis);
    assert(NULL != pchSourceLine);

    int16_t iWidth = this.Mask.iWidth;
    uint32_t wCRC32New = crc32_calculate(pchSourceLine, iWidth);

    /* search existing output */
    __arm_lmsk_line_out_t *ptLine = this.tOutput.Lines.ptTail;
    while(NULL != ptLine) {

        if (ptLine->wCRC == wCRC32New) {
            if (0 == memcmp(ptLine->pchSourceLine, pchSourceLine, iWidth)) {
                /* double confirm */
                return ptLine;
            }
        }

        ptLine = ptLine->ptPrev;
    }

    return NULL;
}

static inline 
void __arm_lmsk_encoder_append(arm_lmsk_encoder_t *ptThis, __arm_lmsk_line_out_t *ptLine)
{
    assert(NULL != ptThis);
    assert(NULL != ptLine);

    ptLine->ptNext = NULL;
    ptLine->ptPrev = NULL;

    if (NULL == this.tOutput.Lines.ptHead) {
        this.tOutput.Lines.ptHead = ptLine;
    }

    if (NULL == this.tOutput.Lines.ptTail) {
        this.tOutput.Lines.ptTail = ptLine;
    } else {
        this.tOutput.Lines.ptTail->ptNext = ptLine;
        ptLine->ptPrev = this.tOutput.Lines.ptTail;
        this.tOutput.Lines.ptTail = ptLine;
    }
}

static 
size_t __arm_lmsk_encoder_line_process( arm_lmsk_encoder_t *ptThis, 
                                        int16_t iY, 
                                        uint8_t *pchSourceLine,
                                        uint8_t chAlphaMSBBits)
{
    assert(NULL != ptThis);
    assert(NULL != pchSourceLine);

    __arm_lmsk_line_out_t *ptOutputLine = __arm_lmsk_find_existing_line(ptThis, pchSourceLine);
    if (NULL != ptOutputLine) {
        /* find an existing one */
        uint32_t wFloorSize = 1 << (16 - this.tOutput.tHeader.tSetting.u2TagSetBits);

        uint32_t wCurrentFloor = this.wPosition >> (16 - this.tOutput.tHeader.tSetting.u2TagSetBits);
        uint32_t wTargetFloor = ptOutputLine->wPosition >> (16 - this.tOutput.tHeader.tSetting.u2TagSetBits);

        if (wCurrentFloor != wTargetFloor) {
            /* at different floor  */

            __arm_lmsk_line_out_t *ptNewLine = (__arm_lmsk_line_out_t *)malloc(sizeof(__arm_lmsk_line_out_t));
            assert(NULL != ptNewLine);

            *ptNewLine = *ptOutputLine;

            /* avoid double reference */
            ptNewLine->pchBuffer = (uint8_t *)malloc(ptNewLine->tSize);
            assert(NULL != ptNewLine->pchBuffer);
            memcpy(ptNewLine->pchBuffer, ptOutputLine->pchBuffer, ptNewLine->tSize);

            ptNewLine->wPosition = this.wPosition;
            
            /* append new line */
            __arm_lmsk_encoder_append(ptThis, ptNewLine);

            /* update line index */
            this.tOutput.tLineIndexTable.pwReferences[iY] = ptNewLine->wPosition;

            return ptNewLine->tSize;

        } 

        this.tOutput.tLineIndexTable.pwReferences[iY] = ptOutputLine->wPosition;
        return 0;   /* there is no new line added */
    }

    /* it is a new line */
    ptOutputLine = (__arm_lmsk_line_out_t *)malloc(sizeof(__arm_lmsk_line_out_t));
    assert(NULL != ptOutputLine);

    memset(ptOutputLine, 0, sizeof(__arm_lmsk_line_out_t));

    ptOutputLine->pchSourceLine = pchSourceLine;
    ptOutputLine->wPosition = this.wPosition;
    
    ptOutputLine->wCRC = crc32_calculate(pchSourceLine, this.Mask.iWidth);

    /*
     *
     *  typedef struct __arm_lmsk_line_out_t {
     *      struct __arm_lmsk_line_out_t *ptNext;
     *      struct __arm_lmsk_line_out_t *ptPrev;
     *      uint8_t *pchBuffer;
     *      uint8_t *pchSourceLine;
     *      size_t tSize;
     *      uint32_t wPosition;
     *      uint32_t wCRC;
     *  } __arm_lmsk_line_out_t;
     */

    __arm_lmsk_encode_line(ptThis, ptOutputLine, chAlphaMSBBits);

    /* append */
    __arm_lmsk_encoder_append(ptThis, ptOutputLine);

    this.tOutput.tLineIndexTable.pwReferences[iY] = ptOutputLine->wPosition;

    return ptOutputLine->tSize;
}

__arm_lmsk_output_t *arm_lmsk_encode(arm_lmsk_encoder_t *ptThis, uint8_t chAlphaMSBBits)
{
    if (NULL == ptThis || chAlphaMSBBits == 0 || chAlphaMSBBits > 8) {
        return NULL;
    } else if (NULL == this.Mask.pchBuffer || 0 == this.Mask.iHeight || 0 == this.Mask.iWidth) {
        return NULL;
    }

    /* free previous output lines if any */
    __arm_lmsk_free_output_lines(&this.tOutput);

    __arm_lmsk_encoder_prepare(ptThis);

    printf("Valid Alpha MSB Count:%"PRId8"\r\n", chAlphaMSBBits);

    /* update the alpha valid MSB bit count */
    this.tOutput.tHeader.tSetting.u3AlphaMSBCount = chAlphaMSBBits - 1;

    uint8_t *pchMask = this.Mask.pchBuffer;
    int16_t iWidth = this.Mask.iWidth;
    int16_t iHeight = this.Mask.iHeight;


    this.tOutput.wDataSize = 0;
    this.wPosition = 0;

    int16_t iDuplicatedLineCount = 0;

    for (int_fast16_t iY = 0; iY < iHeight; iY++) {
        size_t tLineSize = __arm_lmsk_encoder_line_process(ptThis, iY, pchMask, chAlphaMSBBits);

        iDuplicatedLineCount += (tLineSize == 0);

        this.wPosition += tLineSize;
        this.tOutput.wDataSize += tLineSize;

        pchMask += iWidth;
    }

    __arm_lmsk_encoder_report(ptThis);

    printf("Merge %d duplicated lines\r\n", iDuplicatedLineCount);

    /* update floor table size */
    do {
        uint32_t wFloorSizeBit = 16 - this.tOutput.tHeader.tSetting.u2TagSetBits;

        uint32_t wFloorCount = this.tOutput.wDataSize >> wFloorSizeBit;
        if (wFloorCount > 255) {
            /* the output file is too big */

            printf( "ERROR: Output data size is too big! [%"PRIu32"]. It is bigger than the maximum allowed size %"PRIu32"\r\n", 
                    this.tOutput.wDataSize,
                    ((uint32_t)1 << 24) - 1);
            return NULL;
        }

        this.tOutput.tHeader.tSetting.chFloorCount = wFloorCount;

    } while(0);

    return &this.tOutput;
}

int arm_lmsk_write_to_file(__arm_lmsk_output_t *ptThis, FILE *ptOut)
{
    if (NULL == ptThis || NULL == ptOut) {
        return -1;
    }

    int32_t nTotalSize = 0;

    /* write floor table and index table  */
    do {
        uint16_t *phwFloorTable = NULL;
        if (this.tHeader.tSetting.chFloorCount) {
            phwFloorTable = (uint16_t *) malloc(this.tHeader.tSetting.chFloorCount * sizeof(uint16_t));
        }

        uint16_t *phwIndexTable = (uint16_t *)malloc(this.tHeader.tSetting.iHeight * sizeof(uint16_t));
        assert(NULL != phwIndexTable);

        uint32_t wFloorSize = 1 << (16 - this.tHeader.tSetting.u2TagSetBits);

        uint32_t wFloorLevel = 0;
        int_fast16_t iY = 0;
        uint_fast8_t chFoorCount = 0;
        for (; iY < this.tHeader.tSetting.iHeight; iY++) {

            if ((int32_t)this.tLineIndexTable.pwReferences[iY] - (int32_t)wFloorLevel < 0) {
                assert(false);
            }

            if (this.tLineIndexTable.pwReferences[iY] - wFloorLevel >= wFloorSize) {

                /* write a floor */
                wFloorLevel += wFloorSize;

                //printf( "  Floor %"PRId16 " Address 0x%08"PRIx32" 0x%08"PRIx32"\r\n", 
                //        iY, 
                //        this.tLineIndexTable.pwReferences[iY],
                //        wFloorLevel);
            

                phwFloorTable[chFoorCount++] = iY;

                nTotalSize += sizeof(uint16_t);
            }

            phwIndexTable[iY] = (uint16_t)( (uint32_t)this.tLineIndexTable.pwReferences[iY] 
                                          - (uint32_t)wFloorLevel);

            //if (iY < 10) {
            //    printf("\tLine %"PRIi16" Offset 0x%04"PRIx16"\r\n", iY, phwIndexTable[iY]);
            //}
        }
        //if (iY < 10) {
        //    printf("\r\n\r\n");
        //}

        /* update actual floor count */
        this.tHeader.tSetting.chFloorCount = chFoorCount;


        printf("[Floor Table: %"PRIu8"]\tFloor Size:%"PRIu32" 0x%08"PRIX32"\r\n", 
                this.tHeader.tSetting.chFloorCount,
                wFloorSize, wFloorSize);


        /* write header */
        do {
            if (sizeof(this.tHeader) != fwrite(&this.tHeader, 1, sizeof(this.tHeader), ptOut)) {
                if (NULL != phwFloorTable) {
                    free(phwFloorTable);
                }
                free(phwIndexTable);
                return -1;
            }
            nTotalSize += sizeof(this.tHeader);
        } while(0);

        /* write palette */
        do {
            if (sizeof(this.chPalette) != fwrite(&this.chPalette, 1, sizeof(this.chPalette), ptOut)) {
                if (NULL != phwFloorTable) {
                    free(phwFloorTable);
                }
                free(phwIndexTable);
                return -1;
            }
            nTotalSize += sizeof(this.chPalette);
        } while(0);

        /* write floor Table */
        if (NULL != phwFloorTable) {
            if (this.tHeader.tSetting.chFloorCount 
            !=  fwrite(phwFloorTable, sizeof(uint16_t), this.tHeader.tSetting.chFloorCount, ptOut)) {
                free(phwFloorTable);
                free(phwIndexTable);
                return -1;
            }
            nTotalSize += this.tHeader.tSetting.chFloorCount * sizeof(uint16_t);
        }


        /* write line index table */

        if (this.tHeader.tSetting.iHeight != fwrite(phwIndexTable, sizeof(uint16_t), this.tHeader.tSetting.iHeight, ptOut)) {
            if (NULL != phwFloorTable) {
                free(phwFloorTable);
            }
            free(phwIndexTable);
            return -1;
        }

        nTotalSize += this.tHeader.tSetting.iHeight * sizeof(uint16_t);

        if (NULL != phwFloorTable) {
            free(phwFloorTable);
        }
        free(phwIndexTable);
    } while(0);

    int16_t iEncodedLines = 0;

    printf("Data Section Start From 0x%"PRIX32"\r\n", nTotalSize);

    /* write data section */
    do {
        __arm_lmsk_line_out_t *ptLine = this.Lines.ptHead;
        while(NULL != ptLine) {

            if (ptLine->tSize != fwrite(ptLine->pchBuffer, 
                                        1, 
                                        ptLine->tSize, 
                                        ptOut)) {
                return -1;
            }

            nTotalSize += ptLine->tSize;
            iEncodedLines++;
            ptLine = ptLine->ptNext;
        }
    } while(0);

    uint32_t wEndMark = 0xDEADBEEF;

    /* write the 4 byte end mark */
    if (sizeof(wEndMark) != fwrite(&wEndMark, 1, sizeof(wEndMark), ptOut)) {
        return -1;
    }
    nTotalSize += sizeof(wEndMark);

    printf("Encoded %d line of pixels\r\n", iEncodedLines);

    return nTotalSize;
}


arm_lmsk_encoder_t * arm_lmsk_encoder_depose(arm_lmsk_encoder_t *ptThis)
{
    if (NULL == ptThis) {
        return NULL;
    }


    /* free output lines */
    __arm_lmsk_free_output_lines(&this.tOutput);

    /* free the line index reference table */
    if (NULL != this.tOutput.tLineIndexTable.pwReferences) {
        free(this.tOutput.tLineIndexTable.pwReferences);
    }

    return ptThis;
}

static
void __arm_lmsk_free_output_lines(__arm_lmsk_output_t *ptThis)
{
    if (NULL == ptThis) {
        return ;
    }

    __arm_lmsk_line_out_t *ptLine = this.Lines.ptHead;
    while(NULL != ptLine) {

        __arm_lmsk_line_out_t *ptNext = ptLine->ptNext;

        if (NULL != ptLine->pchBuffer) {
            free(ptLine->pchBuffer);
        } else {
            assert(false);
        }
        
        free(ptLine);

        ptLine = ptNext;
    }

    this.Lines.ptHead = NULL;
    this.Lines.ptTail = NULL;
}

static
void crc32_init_table(void)
{
    uint32_t wCRC;
    
    for (int_fast16_t i = 0; i < 256; i++) {
        wCRC = i;
        for (int_fast8_t j = 0; j < 8; j++) {
            wCRC = (wCRC >> 1) ^ ((wCRC & 1u) ? CRC32_POLYNOMIAL : 0);
        }
        s_wCRC32Table[i] = wCRC;
    }
    s_bCRCTableInitialized = true;
}

static
uint32_t crc32_calculate(const void *pData, size_t nLength)
{
    const uint8_t *pchBytes = (const uint8_t *)pData;
    uint32_t wCRC = CRC32_INITIAL;
    size_t i;
    
    if (!s_bCRCTableInitialized) {
        crc32_init_table();
    }
    
    for (i = 0; i < nLength; i++) {
        wCRC = (wCRC >> 8) ^ s_wCRC32Table[(wCRC & 0xFF) ^ pchBytes[i]];
    }
    
    return wCRC ^ CRC32_XOROUT;
}


#ifdef   __cplusplus
}
#endif

#endif

