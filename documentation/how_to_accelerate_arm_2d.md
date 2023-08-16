# How To Accelerate Arm-2D



Arm-2D provides several standard ways to add support for the hardware acceleration of your target platform. Those methods address the typical 2D accelerations that are available in the ecosystem. Arm-2D categorizes them into several topics:

- Accelerations that are **Tightly Coupled with Software Algorithm**. This is also known as **Synchronouse** Acceleration.
- Accelerations **Asynchronousely** via dedicated Processing Element (PE).
  - 2D-Capable-DMA, for example: DMAC-350, DMA2D
  - 2D GPUs
  - Processors are dedicated for running 2D processing tasks
- Acceleration via **Arm Customized Instruction (ACI)**



Depending on the capability of the target platform, the acceleration methods might exist simultaneously, for example, a dual-core Cortex-M55 system with Helium and ACI extensions, using DMAC-350 to accelerate some 2D operations. In other words, the aforementioned acceleration methods are not mutually exclusive. You can apply and enable them to arm-2d if available.

>  **NOTE**: 
>
> 1. When Helium is available, and one enables the Helium support during the compilation, Arm-2D detects the Helium and turns on the Helium acceleration automatically. 
> 2. This document is for system/application engineers who design drivers to add various accelerations to Arm-2D for a target hardware platform. 



## 1 Software Tightly Coupled (Synchronouse) Acceleration 

### 1.1 Acceleration via Arm-2D Intrinsics



### 1.2 Overriding the Default Low-Level Implementations



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



### 2.4 Software Design Considerations for End Users

#### 2.4.1 When and How to Enable Asynchronouse Mode

#### 2.4.2 How to Manage Dependencies Among 2D Operations

#### 2.4.3 Acceleration Methods Preference







## 3 Acceleration via Arm Customized Instruction (ACI)



### 3.3 Insert An User Defined Header File

If you defined the macro `__ARM_2D_HAS_CDE__` to `1`, an user defined header file `arm_2d_user_sync_acc.h` will be included in compilation, as shown below:

```c
#if defined(__ARM_2D_HAS_CDE__) && __ARM_2D_HAS_CDE__
#   include "arm_2d_user_cde.h"
#endif
```

You can use this header file to 

- Override Arm-2D intrinsics and 
- Provide related information if required, such as function prototypes, variables definitions, user defined types, macros etc. 

> **NOTE**: 
>
> 1. The macro `__ARM_2D_HAS_CDE__` does NOT affect the overriding of Arm-2D intrinsics. 
> 2. It is NOT necessary but recommended to use `arm_2d_user_cde.h` to override the Arm-2D intrinsics as long as you have other viable solutions (for example, use `-D` command line option in GCC, LLVM and Arm Compiler 6 )
> 3. It is NOT necessary but recommanded to use macro `__ARM_2D_HAS_CDE__` to include the header file `arm_2d_user_cde.h` as long as you have other viable solutions (for example, use `-include` command line option in GCC, LLVM and Arm Compiler 6 ).