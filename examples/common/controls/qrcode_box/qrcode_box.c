/*
 * Copyright (c) 2009-2025 Arm Limited. All rights reserved.
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
#define __QRCODE_BOX_IMPLEMENT__

#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "qrcode_box.h"
#include <assert.h>
#include <string.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t qrcode_box_init(   qrcode_box_t *ptThis,
                                qrcode_box_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    memset(ptThis, 0, sizeof(qrcode_box_t));
    this.tCFG = *ptCFG;
    this.tCFG.bIsValid = false;
    size_t tInputSize = this.tCFG.hwInputSize;

    if (NULL == this.tCFG.pchBinary || 0 == this.tCFG.hwInputSize) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (this.tCFG.u6Version > qrcodegen_VERSION_MAX) {
        this.tCFG.u6Version = 0;
    }

    if (this.tCFG.bIsString) {
        tInputSize = strnlen(this.tCFG.pchString, this.tCFG.hwInputSize);
        if (tInputSize == this.tCFG.hwInputSize) {
            return ARM_2D_ERR_INVALID_PARAM;
        }
    }

    /* validate scratch memory */
    do {
        uint_fast8_t chVersion = (this.tCFG.u6Version == 0) ? qrcodegen_VERSION_MAX : this.tCFG.u6Version;
        size_t tBufferSize = qrcodegen_BUFFER_LEN_FOR_VERSION(chVersion);
        this.tCFG.bScratchMemoryInHeap = false;

        if (tInputSize > tBufferSize) {
            return ARM_2D_ERR_INSUFFICIENT_RESOURCE;
        }

        if (NULL != this.tCFG.pchBuffer) {
            if ( this.tCFG.hwQRCodeBufferSize < (tBufferSize * 2)) {
                return ARM_2D_ERR_INSUFFICIENT_RESOURCE;
            }
        } else {
            this.tCFG.pchBuffer = __arm_2d_allocate_scratch_memory(
                                    tBufferSize * 2,
                                    1,
                                    ARM_2D_MEM_TYPE_UNSPECIFIED);
            
            if (NULL == this.tCFG.pchBuffer) {
                return ARM_2D_ERR_INSUFFICIENT_RESOURCE;
            }
            this.tCFG.hwQRCodeBufferSize = tBufferSize * 2;
            this.tCFG.bScratchMemoryInHeap = true;
        }
    } while(0);

    if (0 == this.tCFG.chSquarePixelSize) {
        this.tCFG.chSquarePixelSize = 2;
    }

    this.tCFG.bIsValid = true;
    return ARM_2D_ERR_NONE;

}

ARM_NONNULL(1)
void qrcode_box_depose( qrcode_box_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (this.tCFG.bScratchMemoryInHeap && (NULL != this.tCFG.pchBuffer)) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, 
                                     this.tCFG.pchBuffer);
    }
}

ARM_NONNULL(1)
void qrcode_box_on_load( qrcode_box_t *ptThis)
{
    assert(NULL != ptThis);
    if (!this.tCFG.bIsValid) {
        return ;
    }

    assert(NULL != this.tCFG.pchBinary);
    assert(this.tCFG.hwInputSize > 0);
    assert(NULL != this.tCFG.pchBuffer);

    uint_fast8_t chVersion = (this.tCFG.u6Version == 0) ? qrcodegen_VERSION_MAX : this.tCFG.u6Version;
    size_t tBufferSize = qrcodegen_BUFFER_LEN_FOR_VERSION(chVersion);

    assert(this.tCFG.hwQRCodeBufferSize >= tBufferSize * 2);

    do {
        bool bResult = false;
        if (this.tCFG.bIsString) {
            bResult = qrcodegen_encodeText( this.tCFG.pchString, 
                                            this.tCFG.pchBuffer + tBufferSize, 
                                            this.tCFG.pchBuffer, 
                                            this.tCFG.u2ECCLevel,
                                            qrcodegen_VERSION_MIN, 
                                            chVersion, 
                                            qrcodegen_Mask_AUTO, 
                                            true);
        } else {
            assert(this.tCFG.hwInputSize <= tBufferSize);

            memcpy(this.tCFG.pchBuffer + tBufferSize, this.tCFG.pchBinary, this.tCFG.hwInputSize);
            bResult = qrcodegen_encodeBinary(   this.tCFG.pchBuffer + tBufferSize,
                                                this.tCFG.hwInputSize,
                                                this.tCFG.pchBuffer, 
                                                this.tCFG.u2ECCLevel,
                                                qrcodegen_VERSION_MIN, 
                                                chVersion, 
                                                qrcodegen_Mask_AUTO, 
                                                true);
        }

        if (!bResult) {
            this.tCFG.bIsValid = false;
            break;
        }

        this.iQRCodePixelSize = qrcodegen_getSize(this.tCFG.pchBuffer) * this.tCFG.chSquarePixelSize;
    } while(0);
}

ARM_NONNULL(1)
int16_t qrcode_box_get_size(qrcode_box_t *ptThis)
{
    assert(NULL != ptThis);
    
    return this.iQRCodePixelSize;
}

ARM_NONNULL(1)
void qrcode_box_show(   qrcode_box_t *ptThis,
                        const arm_2d_tile_t *ptTile, 
                        const arm_2d_region_t *ptRegion, 
                        COLOUR_INT tColour,
                        uint8_t chOpacity)
{
    assert(NULL != ptThis);

    if (!this.tCFG.bIsValid || 0 == chOpacity) {
        return ;
    }

    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }

    bool bIsNewFrame = (ARM_2D_RT_FALSE != arm_2d_target_tile_is_new_frame(ptTile));

    assert(NULL!= ptThis);

    arm_2d_safe_canvas_open(ptTile, __qrcode_panel, ptRegion) {

        arm_2d_align_centre_open(__qrcode_panel, 
                                 this.iQRCodePixelSize, 
                                 this.iQRCodePixelSize) {
            int16_t iQRSize = qrcodegen_getSize(this.tCFG.pchBuffer);
            arm_2d_region_t tBox = {
                .tSize = {
                    .iHeight = this.tCFG.chSquarePixelSize,
                    .iWidth = this.tCFG.chSquarePixelSize,
                },
            };

            arm_2d_container(ptTile, __qrcode_canvas, &__centre_region) {
                /* example code: flash a 50x50 red box in the centre */

                arm_2d_region_t tPFBScanRegion;
                do {
                    arm_2d_region_t tValidRegion;
                    const arm_2d_tile_t *ptVirtualScreen = __arm_2d_tile_get_virtual_screen_or_root(  
                                                            &__qrcode_canvas,
                                                            &tValidRegion, 
                                                            &tPFBScanRegion.tLocation,
                                                            NULL,
                                                            false);
                    if (NULL == ptVirtualScreen && !bIsNewFrame) {
                        return ;
                    }

                    tPFBScanRegion.tSize = tValidRegion.tSize;
                } while(0);

                int16_t iY = 0, iX = 0;
                int16_t iPFBXLimit = tPFBScanRegion.tLocation.iX + tPFBScanRegion.tSize.iWidth;
                int16_t iPFBYLimit = tPFBScanRegion.tLocation.iY + tPFBScanRegion.tSize.iHeight;

                iY = tPFBScanRegion.tLocation.iY / tBox.tSize.iHeight;
                iY = MAX(0, iY);
                
                for (; iY < iQRSize; iY++ ) {
                    tBox.tLocation.iY = iY * tBox.tSize.iHeight;

                    if (tBox.tLocation.iY >= iPFBYLimit) {
                        /* we can end earlier */
                        return ;
                    }

                    iX = tPFBScanRegion.tLocation.iX / tBox.tSize.iWidth;
                    iX = MAX(0, iX);

                    for (; iX < iQRSize; iX++ ) {
                        tBox.tLocation.iX = iX * tBox.tSize.iWidth;

                        if (tBox.tLocation.iX >= iPFBXLimit) {
                            break;
                        }

                        if (qrcodegen_getModule(this.tCFG.pchBuffer, iX, iY)) {
                            arm_2d_fill_colour_with_opacity(&__qrcode_canvas, 
                                                            &tBox, 
                                                            (__arm_2d_color_t){tColour}, 
                                                            chOpacity);    

                        }
                    }

                }
            }
        }
        


            

    }

    ARM_2D_OP_WAIT_ASYNC();
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
