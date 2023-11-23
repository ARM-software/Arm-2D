/*
 * Copyright (C) 2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        #include "arm_2d_helper_pfb.h"
 * Description:  Public header file for the PFB helper service 
 *
 * $Date:        23. Nov 2023
 * $Revision:    V.1.7.5
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_PFB_H__
#define __ARM_2D_HELPER_PFB_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"

#include "./__arm_2d_helper_common.h"

#ifdef   __cplusplus
extern "C" {
#endif


#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/*!
 * \addtogroup gHelper 7 Helper Services
 * @{
 */

/*============================ MACROS ========================================*/

#define ARM_2D_FPS_MODE_RENDER_ONLY     0
#define ARM_2D_FPS_MODE_REAL            1

/*============================ MACROFIED FUNCTIONS ===========================*/

/*!
 * \brief a macro wrapper in uppercase to help initialising PFB service
 * \param[in] __CB_ADDR the address of the arm_2d_helper_pfb_t object
 * \param[in] __SCREEN_WIDTH the width of the screen
 * \param[in] __SCREEN_HEIGHT the hight of the screen
 * \param[in] __PIXEL_TYPE the integer type of the pixel, i.e. uint8_t, uint16_t,
 *                         uint32_t
 * \param[in] __WIDTH the width of the PFB block
 * \param[in] __HEIGHT the height of the PFB block
 * \note For the same number of pixels in a PFB block, please priority the width
 *       over height, for example, 240 * 1 is better than 30 * 8 
 * \param[in] __PFB_NUM the number of PFB blocks in the built-in PFB pool.
 * \param[in] ... a code block to add additional initializer, see example below:
 * \return arm_2d_err_t the result of the initialisation process
 * 
 * \code {.c}
    
    static ARM_NOINIT arm_2d_helper_pfb_t s_tExamplePFB;
    ...
    // initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT(
        &s_tExamplePFB,                 // FPB Helper object
        __GLCD_CFG_SCEEN_WIDTH__,       // screen width
        __GLCD_CFG_SCEEN_HEIGHT__,      // screen height
        uint16_t,                       // colour date type
        240,                            // PFB block width
        1,                              // PFB block height
        1,                              // number of PFB in the PFB pool
        {
            .evtOnLowLevelRendering = {
                // callback for low level rendering
                .fnHandler = &__pfb_render_handler,
            },
            .evtOnDrawing = {
                // callback for drawing GUI
                .fnHandler = &__pfb_draw_background_handler,
            },
        },
        //.FrameBuffer.bSwapRGB16 = true,
    ) < 0) {
        //! error detected
        assert(false);
    }
 * \endcode
 * 
 */
#define ARM_2D_HELPER_PFB_INIT( __CB_ADDR,      /* PFB Helper object address */ \
                                __SCREEN_WIDTH, /* Screen width */              \
                                __SCREEN_HEIGHT,/* Screen height */             \
                                __PIXEL_TYPE,   /* The type of the pixels */    \
                                __PFB_WIDTH,    /* The width of the PFB block */\
                                __PFB_HEIGHT,   /* The height of the PFB block*/\
                                __PFB_NUM,      /* Block count in the PFB pool*/\
                                ...             /* Event Handler */             \
                                )                                               \
    ({                                                                          \
        ARM_SECTION(".bss.noinit.arm_2d_pfb_pool")                              \
         static struct {                                                        \
            arm_2d_pfb_t tFPB;                                                  \
            __ALIGNED(4)                                                        \
            __PIXEL_TYPE tBuffer[(__PFB_WIDTH) * (__PFB_HEIGHT)];               \
        } s_tPFBs[__PFB_NUM];                                                   \
                                                                                \
        arm_2d_helper_pfb_cfg_t tCFG = {                                        \
            .tDisplayArea.tSize = {                                             \
                .iWidth = (__SCREEN_WIDTH),                                     \
                .iHeight = (__SCREEN_HEIGHT),                                   \
            },                                                                  \
                                                                                \
            .FrameBuffer.ptPFBs = (arm_2d_pfb_t *)s_tPFBs,                      \
            .FrameBuffer.tFrameSize = {                                         \
                .iWidth = (__PFB_WIDTH),                                        \
                .iHeight = (__PFB_HEIGHT),                                      \
            },                                                                  \
            .FrameBuffer.wBufferSize = sizeof(s_tPFBs[0].tBuffer),              \
            .FrameBuffer.hwPFBNum = dimof(s_tPFBs),                             \
            .Dependency =                                                       \
            __VA_ARGS__                                                         \
        };                                                                      \
                                                                                \
        arm_2d_helper_pfb_init((__CB_ADDR), &tCFG);                             \
    })

/*!
 * \brief a macro wrapper to update the evtOnDrawring event handler
 * \param[in] __CB_ADDR the address of the arm_2d_helper_pfb_t object
 * \param[in] __HANDLER the new handler
 * \param[in] ... [Optional] an address (of user defined structure) passed to the 
 *                      event handler.
 * \return arm_2d_err_t the process result
 */
#define ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(                               \
                                    __CB_ADDR, /* PFB Helper object address */  \
                                    __HANDLER, /* new on-draw-handler function*/\
                                    ...)       /* An optional target address */ \
    arm_2d_helper_pfb_update_dependency((__CB_ADDR),                            \
                                        ARM_2D_PFB_DEPEND_ON_DRAWING,           \
                                        (arm_2d_helper_pfb_dependency_t []) {{  \
                                            .evtOnDrawing = {                   \
                                                .fnHandler = (__HANDLER),       \
                                                .pTarget = (NULL,##__VA_ARGS__),\
                                            },                                  \
                                        }})


#define __IMPL_ARM_2D_REGION_LIST(__NAME, ...)                                  \
            enum {                                                              \
                __NAME##_offset = __COUNTER__,                                  \
            };                                                                  \
            __VA_ARGS__                                                         \
            arm_2d_region_list_item_t __NAME[] = {
            
            
#define IMPL_ARM_2D_REGION_LIST(__NAME, ...)                                    \
            __IMPL_ARM_2D_REGION_LIST(__NAME,##__VA_ARGS__)
            

#define END_IMPL_ARM_2D_REGION_LIST(...)                                        \
            };
            
#define __ADD_REGION_TO_LIST(__NAME, ...)                                       \
            {                                                                   \
                .ptNext = (arm_2d_region_list_item_t *)                         \
                            &(__NAME[__COUNTER__ - __NAME##_offset]),           \
                .tRegion = {                                                    \
                    __VA_ARGS__                                                 \
                },                                                              \
            }
            
#define ADD_REGION_TO_LIST(__NAME, ...)                                         \
            __ADD_REGION_TO_LIST(__NAME, ##__VA_ARGS__) 
            
           
#define __ADD_LAST_REGION_TO_LIST(__NAME, ...)                                  \
            {                                                                   \
                .ptNext = NULL,                                                 \
                .tRegion = {                                                    \
                    __VA_ARGS__                                                 \
                },                                                              \
            }
            
#define ADD_LAST_REGION_TO_LIST(__NAME, ...)                                    \
            __ADD_LAST_REGION_TO_LIST(__NAME, ##__VA_ARGS__) 


#define IMPL_PFB_ON_DRAW(__NAME)   IMPL_ON_DRAW_EVT(__NAME)


#define IMPL_PFB_ON_LOW_LV_RENDERING(__NAME)                                    \
            void __NAME(void *pTarget,                                          \
                        const arm_2d_pfb_t *ptPFB,                              \
                        bool bIsNewFrame)    


#define IMPL_PFB_ON_FRAME_SYNC_UP(__NAME)                                       \
            bool __NAME(void *pTarget)


/*!
 * \brief a macro wrapper in lowercase to help initialising PFB service
 * \param[in] __CB_ADDR the address of the arm_2d_helper_pfb_t object
 * \param[in] __SCREEN_WIDTH the width of the screen
 * \param[in] __SCREEN_HEIGHT the hight of the screen
 * \param[in] __PIXEL_TYPE the integer type of the pixel, i.e. uint8_t, uint16_t,
 *                         uint32_t
 * \param[in] __WIDTH the width of the PFB block
 * \param[in] __HEIGHT the height of the PFB block
 * \note For the same number of pixels in a PFB block, please priority the width
 *       over height, for example, 240 * 1 is better than 30 * 8 
 * \param[in] __PFB_NUM the number of PFB blocks in the built-in PFB pool.
 * \param[in] ... a code block to add additional initializer, see example below:
 * \return arm_2d_err_t the result of the initialisation process
 * 
 * \code {.c}
    
    static ARM_NOINIT arm_2d_helper_pfb_t s_tExamplePFB;
    ...
    // initialise FPB helper
    if (init_arm_2d_helper_pfb(
        &s_tExamplePFB,                 // FPB Helper object
        __GLCD_CFG_SCEEN_WIDTH__,       // screen width
        __GLCD_CFG_SCEEN_HEIGHT__,      // screen height
        uint16_t,                       // colour date type
        240,                            // PFB block width
        1,                              // PFB block height
        1,                              // number of PFB in the PFB pool
        {
            .evtOnLowLevelRendering = {
                // callback for low level rendering
                .fnHandler = &__pfb_render_handler,
            },
            .evtOnDrawing = {
                // callback for drawing GUI
                .fnHandler = &__pfb_draw_background_handler,
            },
        },
        //.FrameBuffer.bSwapRGB16 = true,
    ) < 0) {
        //! error detected
        assert(false);
    }
 * \endcode
 * 
 */
#define init_arm_2d_helper_pfb( __CB_ADDR,                                      \
                                __SCREEN_WIDTH,                                 \
                                __SCREEN_HEIGHT,                                \
                                __PIXEL_TYPE,                                   \
                                __WIDTH,                                        \
                                __HEIGHT,                                       \
                                __PFB_NUM,                                      \
                                ...                                             \
                                )                                               \
            ARM_2D_HELPER_PFB_INIT(                                             \
                                __CB_ADDR,                                      \
                                __SCREEN_WIDTH,                                 \
                                __SCREEN_HEIGHT,                                \
                                __PIXEL_TYPE,                                   \
                                __WIDTH,                                        \
                                __HEIGHT,                                       \
                                __PFB_NUM,                                      \
                                ##__VA_ARGS__                                   \
                                )

/*!
 * \brief a macro wrapper to update the evtOnDrawring event handler
 * \param[in] __CB_ADDR the address of the arm_2d_helper_pfb_t object
 * \param[in] __HANDLER the new handler
 * \param[in] ... [Optional] an address (of user defined structure) passed to the 
 *                      event handler.
 * \return arm_2d_err_t the process result
 */
#define update_arm_2d_helper_pfb_on_draw_handler(                               \
                                    __CB_ADDR, /* PFB Helper object address */  \
                                    __HANDLER, /* new on-draw-handler function*/\
                                    ...)       /* An optional target address */ \
            ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(                           \
                                    (__CB_ADDR),                                \
                                    (__HANDLER),##__VA_ARGRS__)

/*!
 *  \brief tell PFB helper that a low level LCD flushing work is complete
 *  \note This function is THREAD-SAFE, You can call this function asynchronously, 
 *        e.g.
 *        - A ISR to indicate DMA-transfer complete event or 
 *        - A different Thread
 *  \param[in] ptThis the PFB helper control block
 *  \param[in] ... the used PFB block.
 *  \note please do not use this parameter, it is only kept for backward
 *        compatability.
 */
#define arm_2d_helper_pfb_report_rendering_complete(__PFB_HELPER_PTR,...)       \
            __arm_2d_helper_pfb_report_rendering_complete((__PFB_HELPER_PTR),   \
                                                          (NULL,##__VA_ARGS__))


#define impl_arm_2d_region_list(__NAME, ...)                                    \
            IMPL_ARM_2D_REGION_LIST(__NAME,##__VA_ARGS__)
#define add_region_to_list(__NAME, ...)                                         \
            ADD_REGION_TO_LIST(__NAME, ##__VA_ARGS__) 
#define add_last_region_to_list(__NAME, ...)                                    \
            ADD_LAST_REGION_TO_LIST(__NAME, ##__VA_ARGS__)
#define end_impl_arm_2d_region_list(...)                                        \
            END_IMPL_ARM_2D_REGION_LIST(__VA_ARGS__)
            
#define impl_pfb_on_draw(__NAME)    IMPL_PFB_ON_DRAW(__NAME)
#define impl_pfb_on_low_lv_rendering(__NAME)                                    \
            IMPL_PFB_ON_LOW_LV_RENDERING(__NAME)

/*============================ TYPES =========================================*/

/*!
 * \brief direct mode helper service frame-buffer control block states
 * 
 * \note state transition diagram
 *                                    <<< service initialization >>>
 *                                              | 
 *                                    ARM_3FB_STATE_READY_FOR_FLUSH <------+
 *                                              |                          |
 *                                    ARM_3FB_STATE_FLUSHING               |
 *                                              |                          |
 *      ARM_3FB_STATUS_UNUSED --->    ARM_3FB_STATE_READY_TO_DRAW          |
 *                                              |                          |
 *                                    ARM_3FB_STATE_COPYING_AS_TARGET      |
 *                                              |
 *                                    ARM_3FB_STATE_DRAWING                |
 *                                              |                          |
 *                                    ARM_3FB_STATE_COPYING_AS_SOURCE -----+
 *                                              
 */
enum {
    ARM_3FB_STATE_UNUSED = 0,              //!< the FB hasn't been used
    ARM_3FB_STATE_COPYING_AS_TARGET,       //!< the FB is used as the target of frame copy, the previous state is ARM_3FB_STATE_FLUSHING (or ARM_3FB_STATE_UNUSED)
    ARM_3FB_STATE_READY_TO_DRAW,           //!< the FB is ready to draw, the previous state is ARM_3FB_STATE_COPYING_AS_TARGET
    ARM_3FB_STATE_DRAWING,                 //!< the FB is used for drawing, the previous state is ARM_3FB_STATE_READY_TO_DRAW
    ARM_3FB_STATE_COPYING_AS_SOURCE,       //!< the FB is used as the source of frame copy, the previous state is ARM_3FB_STATE_READY_FOR_FLUSH
    ARM_3FB_STATE_READY_TO_FLUSH,         //!< the FB is ready for flushing and waiting for a v-sync event, the previous state is ARM_3FB_STATE_COPYING_AS_SOURCE
    ARM_3FB_STATE_FLUSHING,                //!< the FB is used for flushing, the previous state is ARM_3FB_STATE_READY_FOR_FLUSH
};

typedef struct arm_2d_helper_3fb_t arm_2d_helper_3fb_t;

/*!
 * \brief An interface for 2D-Copy.
 * \param[in] pnSource the source image address
 * \param[in] wSourceStride the stride of the source image
 * \param[in] pnTarget the address in the target framebuffer
 * \param[in] wTargetStride the stride of the target framebuffer
 * \param[in] iWidth the safe width of the source image
 * \param[in] iHeight the safe height of the source image
 * \retval true the 2D copy is complete when leaving this function
 * \retval false An async 2D copy request is sent to the DMA
 */
typedef
bool arm_2d_helper_2d_copy_handler_t(   arm_2d_helper_3fb_t *ptThis,
                                        void *pObj,
                                        uintptr_t pnSource,
                                        uint32_t wSourceStride,
                                        uintptr_t pnTarget,
                                        uint32_t wTargetStride,
                                        int16_t iWidth,
                                        int16_t iHeight,
                                        uint_fast8_t chBytePerPixel );

typedef struct arm_2d_helper_2d_copy_evt_t {
    arm_2d_helper_2d_copy_handler_t *fnHandler;
    void *pObj;
} arm_2d_helper_2d_copy_evt_t;

/*!
 * \brief An interface for DMA memory-to-memory copy.
 *        If you have a DMA, you can implement this function by using
 *        __OVERRIDE_WEAK. 
 *        You should implement an ISR for copy-complete event and call
 *        arm_2d_helper_3fb_report_dma_copy_complete() to notify the 
 *        3FB (direct mode) helper service.
 * 
 * \param[in] ptThis the helper service control block
 * \param[in] pObj the address of the user object
 * \param[in] pnSource the source address of the memory block
 * \param[in] pnTarget the target address
 * \param[in] nDataItemCount the number of date items
 * \param[in] chDataItemSize the size of each data item 
 */
typedef
void arm_2d_helper_dma_copy_handler_t(  arm_2d_helper_3fb_t *ptThis,
                                        void *pObj,
                                        uintptr_t pnSource,
                                        uintptr_t pnTarget,
                                        uint32_t nDataItemCount,
                                        uint_fast8_t chDataItemSize);

typedef struct arm_2d_helper_dma_copy_evt_t {
    arm_2d_helper_dma_copy_handler_t *fnHandler;
    void *pObj;
} arm_2d_helper_dma_copy_evt_t;

/*!
 * \brief configuration structure for the 3fb (direct mode) helper service
 */
typedef struct arm_2d_helper_3fb_cfg_t {
    arm_2d_size_t tScreenSize;              //!< the screen size
    uint8_t       chPixelBits;              //!< the number of bits in one pixel
    uintptr_t     pnAddress[3];             //!< addresses of the 3 full-frame-buffer

    arm_2d_helper_2d_copy_evt_t     evtOn2DCopy;
    arm_2d_helper_dma_copy_evt_t    evtOnDMACopy;
} arm_2d_helper_3fb_cfg_t;

#define ARM_2D_3FB_INVALID_IDX      3

/*!
 * \brief the control block of the 3FB (direct mode) service
 */
typedef struct arm_2d_helper_3fb_t {
ARM_PRIVATE(
    arm_2d_helper_3fb_cfg_t tCFG;

    struct {
        uint8_t         u2Drawing       : 2;        //!< FB pointer for drawing
        uint8_t         u2Flushing      : 2;        //!< FB pointer for flushing
        uint8_t         u2ReadyToFlush  : 2;        //!< FB pointer of ready to flush
        uint8_t         u2ReadyToDraw   : 2;        //!< FB pointer of ready to draw
        uint8_t         tState[3];
        uintptr_t       tSemaphore;                 //!< semaphore for async access
        bool            bFBCopyComplete;            //!< a flag to indicate the completion of a DMA copy
    } Runtime;
)
} arm_2d_helper_3fb_t;

typedef struct arm_2d_helper_pfb_t arm_2d_helper_pfb_t;

/*!
 * \brief the header of a PFB block
 */
typedef struct arm_2d_pfb_t {
    struct arm_2d_pfb_t *ptNext;                                                //!< next pfb block
    arm_2d_helper_pfb_t *ptPFBHelper;                                           //!< the pfb helper service current PFB block comes from
    arm_2d_tile_t tTile;                                                        //!< descriptor
    uint32_t u24Size                : 24;
    uint32_t                        : 7;
    uint32_t bIsNewFrame            : 1;                                        //!< a flag to indicate the starting of a frame
}arm_2d_pfb_t;

/*!
 * \brief the node of a region list
 * 
 */
typedef struct arm_2d_region_list_item_t {
    struct arm_2d_region_list_item_t    *ptNext;                                //!< the next node
    arm_2d_region_t                     tRegion;                                //!< the region
    uint8_t                             bIgnore     : 1;                        //!< ignore this region
    uint8_t                             bUpdated    : 1;                        //!< this region item has been updated, PFB helper should refresh it again.
    uint8_t                                         : 6;

ARM_PRIVATE(
    struct arm_2d_region_list_item_t   *ptInternalNext;                         //!< the next node in the internal list
    uint8_t                             bFromInternalPool : 1;                  //!< a flag indicating whether this list item coming from the internal pool
    uint8_t                             bFromHeap         : 1;                  //!< whether this item comes from the HEAP
    uint8_t                                               : 6;
)
}arm_2d_region_list_item_t;

/*!
 * \brief the On Low Level Rendering event handler for the low level (LCD Driver)
 * 
 * \param[in] pTarget a user attached target address 
 * \param[in] ptPFB the PFB block
 * \param[in] bIsNewFrame a flag indicate the starting of a new frame
 */
typedef void arm_2d_helper_render_handler_t( 
                                          void *pTarget,
                                          const arm_2d_pfb_t *ptPFB,
                                          bool bIsNewFrame);

/*!
 * \brief on low level render event 
 */
typedef struct arm_2d_helper_render_evt_t {
    arm_2d_helper_render_handler_t *fnHandler;              //!< event handler function
    void *pTarget;                                          //!< user attached target
} arm_2d_helper_render_evt_t;



/*!
 * \brief the enumeration for events
 * 
 */
enum {
    ARM_2D_PFB_DEPEND_ON_LOW_LEVEL_RENDERING    = _BV(0),   //!< On Low Level Rendering Event
    ARM_2D_PFB_DEPEND_ON_DRAWING                = _BV(1),   //!< On Drawing Event
    ARM_2D_PFB_DEPEND_ON_LOW_LEVEL_SYNC_UP      = _BV(2),   //!< On Low Level Sync-up Event
    ARM_2D_PFB_DEPEND_ON_FRAME_SYNC_UP          = _BV(3),   //!< On Frame Sync-up Event
    ARM_2D_PFB_DEPEND_ON_EACH_FRAME_CPL         = _BV(4),   //!< On Each Frame Complete Event
    ARM_2D_PFB_DEPEND_ON_NAVIGATION             = _BV(5),   //!< On Drawing Navigation Event
};

/*!
 * \brief The PFB Helper Service Dependency 
 * 
 */
typedef struct arm_2d_helper_pfb_dependency_t {
    //! event handler for low level rendering
    arm_2d_helper_render_evt_t      evtOnLowLevelRendering;

    //! event handler for drawing GUI
    arm_2d_helper_draw_evt_t        evtOnDrawing;  

    //! low level rendering handler wants to sync-up (return arm_fsm_rt_wait_for_obj)
    arm_2d_evt_t                    evtOnLowLevelSyncUp;  

    //! event handler for each frame complete
    arm_2d_evt_t                    evtOnEachFrameCPL;  

    //! event handler for drawing GUI
    struct {
        arm_2d_helper_draw_evt_t    evtOnDrawing;
        arm_2d_region_list_item_t  *ptDirtyRegion;
    } Navigation;

} arm_2d_helper_pfb_dependency_t;

/*!
 * \brief PFB Helper configuration
 * 
 */
typedef struct arm_2d_helper_pfb_cfg_t {

    arm_2d_region_t tDisplayArea;                               //!< screen description
    
    struct {
        arm_2d_pfb_t  *ptPFBs;                                  //!< PFB blocks for the internal PFB pool
        arm_2d_size_t  tFrameSize;                              //!< the size of the frame
        uint32_t       wBufferSize;                             //!< the buffer size
        uint16_t       hwPFBNum;                                //!< the number of PFB
        uint16_t       bDoNOTUpdateDefaultFrameBuffer   : 1;    //!< A flag to disable automatically default-framebuffer-registration
        uint16_t       bDisableDynamicFPBSize           : 1;    //!< A flag to disable resize of the PFB block
        uint16_t       bSwapRGB16                       : 1;    //!< A flag to enable swapping high and low bytes of an RGB16 pixel
        uint16_t       bDebugDirtyRegions               : 1;    //!< A flag to show dirty regions on screen for debug
        uint16_t                                        : 2;
        uint16_t       u3PixelWidthAlign                : 3;    //!< Pixel alignment in Width for dirty region (2^n)
        uint16_t       u3PixelHeightAlign               : 3;    //!< Pixel alignment in Height for dirty region (2^n)
        uint16_t       u4PoolReserve                    : 4;    //!< reserve specific number of PFB for other helper services

    } FrameBuffer;                                              //!< frame buffer context

    struct {
        arm_2d_region_list_item_t *ptRegions;                   //!< dirty region list item for internal pool
        uint8_t                   chCount;                      //!< number of dirty region list items
    } DirtyRegion;
    
    arm_2d_helper_pfb_dependency_t Dependency;                  //!< user registered dependency

} arm_2d_helper_pfb_cfg_t;

/*!
 * \brief the type of perf counter
 * 
 */
typedef enum {
    ARM_2D_PERFC_RENDER = 0,
    ARM_2D_PERFC_DRIVER,
    
    __ARM_2D_PERFC_COUNT,
} arm_2d_perfc_type_t;

/*!
 * \brief the PFB helper control block
 * 
 */
struct arm_2d_helper_pfb_t{

ARM_PRIVATE(
    arm_2d_helper_pfb_cfg_t tCFG;                               //!< user configuration 
    
    struct {                                                    
        arm_2d_region_t             tDrawRegion;
        arm_2d_region_t             tTargetRegion;

        arm_2d_region_list_item_t  *ptDirtyRegion;

        struct {
            arm_2d_region_list_item_t  *ptWorkingList;
            arm_2d_region_list_item_t  *ptOriginalList;
            arm_2d_region_list_item_t  *ptCandidateList;
            arm_2d_region_list_item_t  *ptFreeList;
            arm_2d_region_list_item_t  tWorkingItem;
            int16_t                    iFreeCount;
        } OptimizedDirtyRegions;

        arm_2d_tile_t               tPFBTile;
        arm_2d_size_t               tFrameSize;
        uint32_t                    wPFBPixelCount;

        uint8_t                     chPT;
        uint8_t                     chFreePFBCount;
        struct {
            uint16_t                bIsDirtyRegionOptimizationEnabled       : 1;
            uint16_t                bEnableDirtyRegionOptimizationRequest   : 1;
            uint16_t                bDisableDirtyRegionOptimizationRequest  : 1;
            uint16_t                bEncounterDynamicDirtyRegion            : 1;
            uint16_t                bFailedToOptimizeDirtyRegion            : 1;
            uint16_t                bIsUsingOptimizedDirtyRegionList        : 1;
            uint16_t                                                        : 2;

            uint16_t                bIsNewFrame                             : 1;
            uint16_t                                                        : 1;
            uint16_t                bIgnoreLowLevelFlush                    : 1;
            uint16_t                bHideNavigationLayer                    : 1;
            uint16_t                bIsDryRun                               : 1;    //!< A flag to indicate whether the first iteration was a dry run
            uint16_t                bNoAdditionalDirtyRegionList            : 1;
            uint16_t                bFirstIteration                         : 1;
            uint16_t                bIsRegionChanged                        : 1;
        };
        

        uintptr_t                   pFPBPoolAvailable;
        arm_2d_pfb_t               *ptCurrent;
        arm_2d_pfb_t               *ptFreeList;
        arm_2d_pfb_t               *ptFlushing;
        struct {
            arm_2d_pfb_t           *ptHead;
            arm_2d_pfb_t           *ptTail;
        }FlushFIFO;
        arm_2d_tile_t              *ptFrameBuffer;
    } Adapter;
)

    struct {
        int64_t lTimestamp;                                     //!< PLEASE DO NOT USE
        int32_t nTotalCycle;                                    //!< cycles used by drawing 
        int32_t nRenderingCycle;                                //!< cycles used in LCD flushing
    } Statistics;                                               //!< performance statistics

};

/*!
 * \brief the Transform helper control block
 * 
 */
typedef struct {

    float fAngle;
    float fScale;

ARM_PRIVATE(
    arm_2d_region_list_item_t tDirtyRegions[2];
    arm_2d_op_t *ptTransformOP;
    arm_2d_region_list_item_t **ppDirtyRegionList;

    struct {
        float fValue;
        float fStep;
    } Angle;

    struct {
        float fValue;
        float fStep;
    } Scale;

    bool bNeedUpdate;
)

} arm_2d_helper_transform_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief initialize pfb helper service
 * \param[in] ptThis the pfb helper control block
 * \param[in] ptCFG the configuration 
 * \return arm_2d_err_t the process result
 */
extern
ARM_NONNULL(1,2)
arm_2d_err_t arm_2d_helper_pfb_init(arm_2d_helper_pfb_t *ptThis, 
                                    arm_2d_helper_pfb_cfg_t *ptCFG);
/*!
 * \brief uninitialize pfb helper service
 * \param[in] ptThis the pfb helper control block
 * \return none
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_pfb_deinit(arm_2d_helper_pfb_t *ptThis);

/*!
 * \brief get the display (screen) region
 * \param[in] ptThis the pfb helper control block
 * \return arm_2d_region_t the screen region
 */
extern
ARM_NONNULL(1)
arm_2d_region_t arm_2d_helper_pfb_get_display_area(arm_2d_helper_pfb_t *ptThis);

/*!
 * \brief get the absolute location for a given location on the target tile canvas
 * \param[in] ptTile the target tile
 * \param[in] tLocation the location on the target tile canvas
 * \return arm_2d_location_t the absolute location on a (virtual) screen or on 
 *         a root tile canvas
 */
extern
ARM_NONNULL(1)
arm_2d_location_t arm_2d_helper_pfb_get_absolute_location(  
                                                    arm_2d_tile_t *ptTile, 
                                                    arm_2d_location_t tLocation);

/*!
 * \brief get the inital PFB size
 * \param[in] ptThis the pfb helper control block
 * \return arm_2d_size_t the PFB size
 */
extern
ARM_NONNULL(1)
arm_2d_size_t arm_2d_helper_pfb_get_pfb_size(arm_2d_helper_pfb_t *ptThis);

/*!
 * \brief the task function for pfb helper
 * \param[in] ptThis an initialised PFB helper control block
 * \param[in] ptDirtyRegions a region list pending for refresh, NULL means 
 *                           refreshing the whole screen
 * \retval arm_fsm_rt_cpl complete refreshing one frame
 * \retval arm_fsm_rt_on_going the refreshing work is on-going
 * \retval arm_fsm_rt_wait_for_obj user's OnDrawing event handler wants to wait 
 *                                 for some objects, e.g. semaphore etc.
 * \retval <0 An error is detected
 */
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2d_helper_pfb_task(arm_2d_helper_pfb_t *ptThis, 
                                    arm_2d_region_list_item_t *ptDirtyRegions);

/*!
 * \brief flush the FPB FIFO 
 * \note This function is THREAD-SAFE
 * \note For normal usage, please DO NOT use this function unless you know what
 *       you are doing.
 * \param[in] ptThis an initialised PFB helper control block
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_pfb_flush(arm_2d_helper_pfb_t *ptThis);

/*!
 * \brief hide the navigation layer
 * \param[in] ptThis an initialised PFB helper control block
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_hide_navigation_layer(arm_2d_helper_pfb_t *ptThis);

/*!
 * \brief show the navigation layer if there is a valid one
 * \param[in] ptThis an initialised PFB helper control block
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_show_navigation_layer(arm_2d_helper_pfb_t *ptThis);


/*!
 * \brief ignore the low level PFB flushing only
 * \param[in] ptThis an initialised PFB helper control block
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_ignore_low_level_flush(arm_2d_helper_pfb_t *ptThis);

/*!
 * \brief resume the low level PFB flushing
 * \param[in] ptThis an initialised PFB helper control block
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_resume_low_level_flush(arm_2d_helper_pfb_t *ptThis);


/*!
 * \brief update PFB dependency (event handlers)
 * \param[in] ptThis the PFB helper control block
 * \param[in] chMask the bit mask for event handlers
 * \param[in] ptDependency the new dependency description
 * \return arm_2d_err_t the process result
 */
extern
ARM_NONNULL(1,3)
arm_2d_err_t arm_2d_helper_pfb_update_dependency(
                            arm_2d_helper_pfb_t *ptThis, 
                            uint_fast8_t chMask,
                            const arm_2d_helper_pfb_dependency_t *ptDependency);

/*!
 *  \brief tell PFB helper that a low level LCD flushing work is complete
 *  \note This function is THREAD-SAFE, You can call this function asynchronously, 
 *        e.g.
 *        - A ISR to indicate DMA-transfer complete event or 
 *        - A different Thread
 *  \param[in] ptThis the PFB helper control block
 *  \param[in] ptPFB the used PFB block
 */
extern
ARM_NONNULL(1)
void __arm_2d_helper_pfb_report_rendering_complete( arm_2d_helper_pfb_t *ptThis,
                                                    arm_2d_pfb_t *ptPFB);

/*!
 * \brief swap the high and low bytes for each rgb16 pixel
 *
 * \param[in] phwBuffer the pixel buffer
 * \note the phwBuffer MUST aligned to half-word addresses
 *
 * \param[in] wSize the number of pixels
 */
extern
void arm_2d_helper_swap_rgb16(uint16_t *phwBuffer, uint32_t wCount);

/*!
 * \brief try to get a PFB block from the pool
 * \param[in] ptThis the PFB helper control block
 * \retval NULL the pool is empty
 * \retval !NULL a valid pfb block 
 */
extern
ARM_NONNULL(1)
arm_2d_pfb_t *__arm_2d_helper_pfb_new(arm_2d_helper_pfb_t *ptThis);

/*!
 * \brief free a PFB block to the pool
 * \param[in] ptThis the PFB helper control block
 * \param[in] ptPFB the target PFB block
 */
extern
ARM_NONNULL(1)
void __arm_2d_helper_pfb_free(arm_2d_helper_pfb_t *ptThis, arm_2d_pfb_t *ptPFB);

/*!
 * \brief enable dirty region optimization service
 * \param[in] ptThis the PFB helper control block
 * \param[in] ptRegions an optional array of dirty region items, which will be
 *            added to the dirty region item pool. NULL is acceptable.
 * \param[in] chCount the number of items in the array.
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_pfb_enable_dirty_region_optimization(
                                            arm_2d_helper_pfb_t *ptThis,
                                            arm_2d_region_list_item_t *ptRegions,
                                            uint_fast8_t chCount);
/*!
 * \brief disable dirty region optimization service
 * \param[in] ptThis the PFB helper control block
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_pfb_disable_dirty_region_optimization(
                                                arm_2d_helper_pfb_t *ptThis);

/*!
 * \brief initialize a given transform helper
 * \param[in] ptThis the target helper
 * \param[in] ptTransformOP the target transform OP, NULL is not accepted.
 * \param[in] fAngleStep the minimal acceptable angle change. 
 * \param[in] fScaleStep the minimal acceptable scale ratio change.
 * \param[in] ppDirtyRegionList the address of the dirty region list
 */
extern
ARM_NONNULL(1,2,5)
void arm_2d_helper_transform_init(arm_2d_helper_transform_t *ptThis,
                                  arm_2d_op_t *ptTransformOP,
                                  float fAngleStep,
                                  float fScaleStep,
                                  arm_2d_region_list_item_t **ppDirtyRegionList);

/*!
 * \brief depose a given transform helper
 * \param[in] ptThis the target helper
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_transform_depose(arm_2d_helper_transform_t *ptThis);

/*!
 * \brief the on-frame-begin event handler for a given transform helper
 * \param[in] ptThis the target helper
 * \note Usually this event handler should be insert the frame start event 
 *       handler of a target scene.
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_transform_on_frame_begin(arm_2d_helper_transform_t *ptThis);

/*!
 * \brief force transform helper to update dirty region
 *
 * \note sometimes, we want to force transform helper to update dirty regions 
 *       even if both the angel and scale keep the same, for example, the pivots
 *       are updated.
 * \param[in] ptThis the target helper
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_transform_force_update(arm_2d_helper_transform_t *ptThis);

/*!
 * \brief update a given transform helper with new values
 * \param[in] ptThis the target helper
 * \param[in] fAngle the new angle value
 * \param[in] fScale the new scale ratio
 * \note The new value is only accepted when the change between the old value 
 *       and the new value is larger than the minimal acceptable mount.
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_transform_update_value(  arm_2d_helper_transform_t *ptThis,
                                            float fAngle,
                                            float fScale);

/*!
 * \brief update the dirty region after a transform operation
 * \param[in] ptThis the target helper
 * \param[in] ptCanvas the canvas
 * \param[in] bIsNewFrame whether this is a new frame
 */
extern
ARM_NONNULL(1,2)
void arm_2d_helper_transform_update_dirty_regions(
                                    arm_2d_helper_transform_t *ptThis,
                                    const arm_2d_region_t *ptCanvas,
                                    bool bIsNewFrame);


/*!
 * \brief initialize the 3FB (direct mode) service
 * \param[in] ptThis the helper service control block
 * \param[in] ptCFG the configuration structure
 */
extern
ARM_NONNULL(1,2)
void arm_2d_helper_3fb_init(arm_2d_helper_3fb_t *ptThis, 
                            const arm_2d_helper_3fb_cfg_t *ptCFG);


/*!
 * \brief report the copy-completion event to the 3FB (direct mode) service
 * \note see function __arm_2d_helper_3fb_dma_copy for details
 * \param[in] ptThis the helper service control block
 */
extern
ARM_NONNULL(1)
void arm_2d_helper_3fb_report_dma_copy_complete(arm_2d_helper_3fb_t *ptThis);

/*!
 * \brief get a pointer for flushing
 * \param[in] ptThis the helper service control block
 * \return void * the address of a framebuffer
 * 
 * \note please only call this function when on vsync event.
 */
extern
ARM_NONNULL(1)
void * arm_2d_helper_3fb_get_flush_pointer(arm_2d_helper_3fb_t *ptThis);

/*!
 * \brief please do NOT use this function. It is used by the display adapter.
 */
extern
ARM_NONNULL(1,2)
bool __arm_2d_helper_3fb_draw_bitmap(arm_2d_helper_3fb_t *ptThis, 
                                     const arm_2d_pfb_t *ptPFB);



/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif



#endif
