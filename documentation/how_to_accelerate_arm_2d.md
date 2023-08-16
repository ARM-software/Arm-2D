# How To Accelerate Arm-2D



Arm-2D provides several standard ways to add support for the hardware acceleration of your target platform. Those methods address the typical 2D accelerations that are available in the ecosystem. Arm-2D categorizes them into several topics:

- Accelerations that are **Tightly Coupled with Software Algorithm**. This is also known as **Synchronouse** Acceleration.
- Accelerations **Asynchronousely** via dedicated Processing Element (PE).
  - 2D-Capable-DMA, for example: DMAC-350, DMA2D
  - 2D GPUs
  - Processors are dedicated for running 2D processing tasks
- Acceleration via **Arm Custom Instruction (ACI)**



Depending on the capability of the target platform, the acceleration methods might exist simultaneously, for example, a dual-core Cortex-M55 system with Helium and ACI extensions, using DMAC-350 to accelerate some 2D operations. In other words, the aforementioned acceleration methods are not mutually exclusive. You can apply and enable them to arm-2d if available.

>  **NOTE**: 
>
> 1. When Helium is available, and one enables the Helium support during the compilation, Arm-2D detects the Helium and turns on the Helium acceleration automatically. 
> 2. This document is for system/application engineers who design drivers to add various accelerations to Arm-2D for a target hardware platform. 



## 1 Software Tightly Coupled (Synchronouse) Acceleration 

### 1.1 Acceleration via Arm-2D Intrinsics

Arm-2D uses some Arm-2D specific Intrinsics in the default low-level C implementations. These intrinsics are defined as macros in  the private header file `__arm_2d_impl.h`:

```c

#ifndef __ARM_2D_PIXEL_BLENDING_INIT
#   define __ARM_2D_PIXEL_BLENDING_INIT
#endif

#ifndef __ARM_2D_PIXEL_BLENDING_GRAY8
#   define __ARM_2D_PIXEL_BLENDING_GRAY8(__SRC_ADDR, __DES_ADDR, __TRANS)       \
            do {                                                                \
                uint16_t hwOPA = 256 - (__TRANS);                               \
                const uint8_t *pchSrc = (uint8_t *)(__SRC_ADDR);                \
                uint8_t *pchDes = (uint8_t *)(__DES_ADDR);                      \
                                                                                \
                *pchDes = ((uint16_t)( ((uint16_t)(*pchSrc++) * hwOPA)          \
                                     + ((uint16_t)(*pchDes) * (__TRANS))        \
                                     ) >> 8);                                   \
            } while(0)
#endif

#ifndef __ARM_2D_PIXEL_BLENDING_RGB565
#   define __ARM_2D_PIXEL_BLENDING_RGB565(__SRC_ADDR, __DES_ADDR, __TRANS)      \
            do {                                                                \
                uint16_t hwOPA = 256 - (__TRANS);                               \
                __arm_2d_color_fast_rgb_t tSrcPix, tTargetPix;                  \
                uint16_t *phwTargetPixel = (__DES_ADDR);                        \
                __arm_2d_rgb565_unpack(*(__SRC_ADDR), &tSrcPix);                \
                __arm_2d_rgb565_unpack(*phwTargetPixel, &tTargetPix);           \
                                                                                \
                for (int i = 0; i < 3; i++) {                                   \
                    uint16_t        hwTemp =                                    \
                        (uint16_t) (tSrcPix.BGRA[i] * hwOPA) +                  \
                        (tTargetPix.BGRA[i] * (__TRANS));                       \
                    tTargetPix.BGRA[i] = (uint16_t) (hwTemp >> 8);              \
                }                                                               \
                                                                                \
                /* pack merged stream */                                        \
                *phwTargetPixel = __arm_2d_rgb565_pack(&tTargetPix);            \
            } while(0)
#endif

#ifndef __ARM_2D_PIXEL_BLENDING_CCCN888
#   define __ARM_2D_PIXEL_BLENDING_CCCN888(__SRC_ADDR, __DES_ADDR, __TRANS)     \
            do {                                                                \
                uint16_t hwOPA = 256 - (__TRANS);                               \
                /* do not change alpha */                                       \
                uint_fast8_t ARM_2D_SAFE_NAME(n) = sizeof(uint32_t) - 1;        \
                const uint8_t *pchSrc = (uint8_t *)(__SRC_ADDR);                \
                uint8_t *pchDes = (uint8_t *)(__DES_ADDR);                      \
                                                                                \
                do {                                                            \
                    *pchDes = ( ((uint_fast16_t)(*pchSrc++) * hwOPA)            \
                              + ((uint_fast16_t)(*pchDes) * (__TRANS))          \
                              ) >> 8;                                           \
                     pchDes++;                                                  \
                } while(--ARM_2D_SAFE_NAME(n));                                 \
            } while(0)
#endif
            
#ifndef __ARM_2D_PIXEL_BLENDING_OPA_GRAY8
#   define __ARM_2D_PIXEL_BLENDING_OPA_GRAY8(__SRC_ADDR, __DES_ADDR, __OPA)     \
            do {                                                                \
                uint16_t hwTrans = 256 - (__OPA);                               \
                const uint8_t *pchSrc = (uint8_t *)(__SRC_ADDR);                \
                uint8_t *pchDes = (uint8_t *)(__DES_ADDR);                      \
                                                                                \
                *pchDes = ((uint16_t)( ((uint16_t)(*pchSrc++) * (__OPA))        \
                                     + ((uint16_t)(*pchDes) * hwTrans)          \
                                     ) >> 8);                                   \
            } while(0)
#endif

#ifndef __ARM_2D_PIXEL_BLENDING_OPA_RGB565
#   define __ARM_2D_PIXEL_BLENDING_OPA_RGB565(__SRC_ADDR, __DES_ADDR, __OPA)    \
            do {                                                                \
                uint16_t hwTrans = 256 - (__OPA);                               \
                __arm_2d_color_fast_rgb_t tSrcPix, tTargetPix;                  \
                uint16_t *phwTargetPixel = (__DES_ADDR);                        \
                __arm_2d_rgb565_unpack(*(__SRC_ADDR), &tSrcPix);                \
                __arm_2d_rgb565_unpack(*phwTargetPixel, &tTargetPix);           \
                                                                                \
                for (int i = 0; i < 3; i++) {                                   \
                    uint16_t        hwTemp =                                    \
                        (uint16_t) (tSrcPix.BGRA[i] * (__OPA)) +                \
                        (tTargetPix.BGRA[i] * hwTrans);                         \
                    tTargetPix.BGRA[i] = (uint16_t) (hwTemp >> 8);              \
                }                                                               \
                                                                                \
                /* pack merged stream */                                        \
                *phwTargetPixel = __arm_2d_rgb565_pack(&tTargetPix);            \
            } while(0)
#endif

#ifndef __ARM_2D_PIXEL_BLENDING_OPA_CCCN888
#   define __ARM_2D_PIXEL_BLENDING_OPA_CCCN888(__SRC_ADDR, __DES_ADDR, __OPA)   \
            do {                                                                \
                uint16_t hwTrans = 256 - (__OPA);                               \
                /* do not change alpha */                                       \
                uint_fast8_t ARM_2D_SAFE_NAME(n) = sizeof(uint32_t) - 1;        \
                const uint8_t *pchSrc = (uint8_t *)(__SRC_ADDR);                \
                uint8_t *pchDes = (uint8_t *)(__DES_ADDR);                      \
                                                                                \
                do {                                                            \
                    *pchDes = ( ((uint_fast16_t)(*pchSrc++) * (__OPA))          \
                              + ((uint_fast16_t)(*pchDes) * hwTrans)            \
                              ) >> 8;                                           \
                     pchDes++;                                                  \
                } while(--ARM_2D_SAFE_NAME(n));                                 \
            } while(0)
#endif

#ifndef __ARM_2D_PIXEL_AVERAGE_RGB565
#   define __ARM_2D_PIXEL_AVERAGE_RGB565(__PIXEL_IN, __ALPHA)                   \
    do {                                                                        \
        __arm_2d_color_fast_rgb_t tTempColour;                                  \
        __arm_2d_rgb565_unpack((__PIXEL_IN), &tTempColour);                     \
        tPixel.R += tTempColour.R * (__ALPHA);                                  \
        tPixel.G += tTempColour.G * (__ALPHA);                                  \
        tPixel.B += tTempColour.B * (__ALPHA);                                  \
    } while(0)
#endif

#ifndef __ARM_2D_PIXEL_AVERAGE_CCCN888
#   define __ARM_2D_PIXEL_AVERAGE_CCCN888(__PIXEL_IN, __ALPHA)                  \
    do {                                                                        \
        arm_2d_color_rgb888_t tTempColour = {.tValue = (__PIXEL_IN)};           \
        tPixel.R += tTempColour.u8R * (__ALPHA);                                \
        tPixel.G += tTempColour.u8G * (__ALPHA);                                \
        tPixel.B += tTempColour.u8B * (__ALPHA);                                \
    } while(0)
#endif

#ifndef __ARM_2D_PIXEL_AVERAGE_GRAY8
#   define __ARM_2D_PIXEL_AVERAGE_GRAY8(__PIXEL_IN, __ALPHA)                    \
    do {                                                                        \
        tPixel += (uint16_t)(__PIXEL_IN) * (uint16_t)(__ALPHA);                 \
    } while(0)
#endif
```

As shown above, you can override the default definition and implement with your own acceleration. 



### 1.2 Overriding the Default Low-Level Implementations

Depends on the toolchain and the way of compilation, different ways of overriding the Arm-2D intrinsics are available:

- * Creating a header file called `arm_2d_user_sync_acc.h`, adding your definition in the header file and defining the macro `__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__` to `1` (in `arm_2d_cfg.h` or `-D` option). (**This is the recommended method.**)
- Using `-D` in command line.
- Providing your own definition in a header file and pre-including it in the compilation (for example, via `-include` option in GCC, LLVM and Arm Compiler 6).



### 1.3 Insert An User Defined Header File

If you defined the macro `__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__` to `1`, an user defined header file `arm_2d_user_sync_acc.h` will be included in compilation, as shown below:

```c
#if defined(__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__) && __ARM_2D_HAS_TIGHTLY_COUPLED_ACC__
#   include "arm_2d_user_sync_acc.h"
#endif
```

You can use this header file to 

- Override Arm-2D intrinsics and 
- Provide related information if required, such as function prototypes, variables definitions, user defined types, macros etc. 

> **NOTE**: 
>
> 1. The macro `__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__` does NOT affect the overriding of Arm-2D intrinsics. 
> 2. It is NOT necessary but recommended to use `arm_2d_user_sync_acc.h` to override the Arm-2D intrinsics as long as you have other viable solutions (for example, use `-D` command line option in GCC, LLVM and Arm Compiler 6 )
> 3. It is NOT necessary but recommanded to use macro `__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__` to include the header file `arm_2d_user_sync_acc.h` as long as you have other viable solutions (for example, use `-include` command line option in GCC, LLVM and Arm Compiler 6 ).



After setting the macro `__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__` to `1`, `arm_2d.c` will call the `__arm_2d_sync_acc_init()` that you **MUST** implement in your own c source file. You can initialize the acceleration hardware logic here. 

```c
...
#if !defined(__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__) || !__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__
#   define __arm_2d_sync_acc_init()
#endif

...

#if defined(__ARM_2D_HAS_TIGHTLY_COUPLED_ACC__) && __ARM_2D_HAS_TIGHTLY_COUPLED_ACC__
/*! 
 * \brief initialise the tightly-coupled (sync) acceleration
 */
extern
void __arm_2d_sync_acc_init(void);
#endif

...

/*! 
 * \brief initialise arm-2d
 */
void arm_2d_init(void)
{
    __arm_2d_init();      
    
    ...
    __arm_2d_sync_acc_init();                                             
    ...                                              
}
```





## 2 Asynchronouse Acceleration

### 2.1 Pixel Pipeline Overview



### 2.2 OPCODE based Acceleration Entry



### 2.3 Insert An User Defined Header File

If you defined the macro `__ARM_2D_HAS_HW_ACC__` to `1`, an user defined header file `arm_2d_user_async_acc.h` will be included in compilation, as shown below:

```c
#if defined(__ARM_2D_HAS_HW_ACC__) && __ARM_2D_HAS_HW_ACC__
#   include "arm_2d_user_async_acc.h"
#endif
```

You can use this header file to 

- Provide related information if required, such as function prototypes, variables definitions, user defined types, macros etc. 

> **NOTE**: 
>
> 1. The macro `__ARM_2D_HAS_HW_ACC__` does NOT affect the overriding of Arm-2D intrinsics. 
> 2. It is NOT necessary but recommended to use `arm_2d_user_async_acc.h` to override the Arm-2D intrinsics as long as you have other viable solutions (for example, use `-D` command line option in GCC, LLVM and Arm Compiler 6 )
> 3. It is NOT necessary but recommanded to use macro `__ARM_2D_HAS_HW_ACC__` to include the header file `arm_2d_user_async_acc.h` as long as you have other viable solutions (for example, use `-include` command line option in GCC, LLVM and Arm Compiler 6 ).



After setting the macro `__ARM_2D_HAS_HW_ACC__` to `1`, `arm_2d.c` will call the `__arm_2d_async_acc_init()` that you **MUST** implement in your own c source file. You can initialize the acceleration hardware logic here. 

```c
...
#if defined(__ARM_2D_HAS_HW_ACC__) && !__ARM_2D_HAS_HW_ACC__
#   define __arm_2d_async_acc_init()
#endif

...

#if defined(__ARM_2D_HAS_HW_ACC__) && __ARM_2D_HAS_HW_ACC__
/*! 
 * \brief initialise the hardware (async) acceleration
 */
extern
void __arm_2d_async_acc_init(void);
#endif

...

/*! 
 * \brief initialise arm-2d
 */
void arm_2d_init(void)
{
    __arm_2d_init();      
    
    ...
    __arm_2d_async_acc_init();                                             
    ...                                              
}
```





### 2.4 Software Design Considerations for End Users

#### 2.4.1 When and How to Enable Asynchronouse Mode

#### 2.4.2 How to Manage Dependencies Among 2D Operations

#### 2.4.3 Acceleration Methods Preference







## 3 Acceleration via Arm Custom Instruction (ACI)



### 3.3 Insert An User Defined Header File

If you defined the macro `__ARM_2D_HAS_ACI__` to `1`, an user defined header file `arm_2d_user_cde.h` will be included in compilation, as shown below:

```c
#if defined(__ARM_2D_HAS_ACI__) && __ARM_2D_HAS_ACI__
#   include "arm_2d_user_aci.h"
#endif
```

You can use this header file to 

- Override Arm-2D intrinsics and 
- Provide related information if required, such as function prototypes, variables definitions, user defined types, macros etc. 

> **NOTE**: 
>
> 1. The macro `__ARM_2D_HAS_ACI__` does NOT affect the overriding of Arm-2D intrinsics. 
> 2. It is NOT necessary but recommended to use `arm_2d_user_aci.h` to override the Arm-2D intrinsics as long as you have other viable solutions (for example, use `-D` command line option in GCC, LLVM and Arm Compiler 6 )
> 3. It is NOT necessary but recommanded to use macro `__ARM_2D_HAS_ACI__` to include the header file `arm_2d_user_aci.h` as long as you have other viable solutions (for example, use `-include` command line option in GCC, LLVM and Arm Compiler 6 ).



After setting the macro `__ARM_2D_HAS_ACI__` to `1`, `arm_2d.c` will call the `__arm_2d_aci_init()` that you **MUST** implement in your own c source file. You can initialize the acceleration hardware logic here. 

```c
...
#if !defined(__ARM_2D_HAS_ACI__) || !__ARM_2D_HAS_ACI__
#   define __arm_2d_aci_init()
#endif

...

#if defined(__ARM_2D_HAS_ACI__) && __ARM_2D_HAS_ACI__
/*! 
 * \brief initialise the ACI service
 */
extern
void __arm_2d_aci_init(void);
#endif

...

/*! 
 * \brief initialise arm-2d
 */
void arm_2d_init(void)
{
    __arm_2d_init();      
    
    ...
    __arm_2d_aci_init();                                             
    ...                                              
}
```

