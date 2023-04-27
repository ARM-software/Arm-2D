# Getting Started as an Professional User









### "I am interested in the implementation"

- We apologise that at the current stage (it's the early stage, as you can see), there is no sufficient guidance or documents about:

  - How the library is implemented
  - How to contribute
  - How to add new features
  - What's the design principles behind the code
  - What's the structure of the design in details

- Some design considerations:

  - The library supports **Arm Compiler 5/6**, **GCC**, **LLVM** and **IAR**.
  - The library supports **ALL** Cortex-M processors. There should be no problem for working with existing Cortex-M processors, i.e. **Cortex-M0/M0+/M1/M3/M4/M7/M23/M33/Star-MC1/M35P/M55/M85**. If you find any issue, please feel free to let us know. 
  - The library is designed with some **OOPC** (Object-Oriented Programming with ANSI-C) methodologies. And the bottom line is that any methods and tricks adopted in this library should come with no or very little cost. 

- This library is compliant with **C11** standard and uses some **widely accepted GCC extensions**:

  - [Macros with a Variable Number of Arguments](https://gcc.gnu.org/onlinedocs/gcc/Variadic-Macros.html#Variadic-Macros) 
  - [ ***\_\_alignof\_\_()*** ](https://gcc.gnu.org/onlinedocs/gcc/Alignment.html#Alignment) 
  - [Unnamed Structure and Union Fields](https://gcc.gnu.org/onlinedocs/gcc/Unnamed-Fields.html)
  - [Statements and Declarations in Expressions](https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html#Statement-Exprs)

- Some of the definitions are written with the support of the **Microsoft Extensions** in mind \( `-fms-extensions` \), but **the library never depends on it**. This means that if programmers enable the support of the Microsoft Extensions in their project, they can benefit from it. 

- This library follows ***"Using Extensions to replace Modifications"*** principle

  - Keywords `__WEAK` and `__OVERRIDE_WEAK` are introduced for default functions and extensions; it is similar to the concept of "virtual functions" and "override functions" in C#. 

    - `arm_2d_async.c` is used to override some infrastructure functions in `arm_2d.c` to support asynchronous mode in the programmers' mode.  
    - ***arm_2d_helium.c*** is used to override some default software algorithm implementations across the library. 

  - Supports for hardware accelerators (both from Arm and 3rd-parties) should be added in the same manner in the future. 

    - Override the target low level IO defined with `def_low_lv_io()` macro that originally defined in `arm_2d_op_table.c` to add your own version of algorithms and hardware accelerations. For example, if you want to add alpha-blending support for RGB565 using your 2D hardware accelerator, you should do the following steps:

      1. In one of your own C source code, override the definition of `__ARM_2D_IO_COPY_WITH_OPACITY_RGB565`

         ```c
         //! PLEASE add following three lines in your hardware adapter source code
         #define __ARM_2D_IMPL__
         #include "arm_2d.h"
         #include "__arm_2d_impl.h"
         
         ...
         
         __OVERRIDE_WEAK
         def_low_lv_io(__ARM_2D_IO_COPY_WITH_OPACITY_RGB565, 
                         __arm_2d_rgb565_sw_tile_copy_with_opacity,
                         __arm_2d_rgb565_my_hw_tile_copy_with_opacity);
         ```

      2. Copy the function body of `__arm_2d_rgb565_sw_tile_copy_with_opacity()` to your source code as a template of the ***hardware adaptor*** and rename it as `__arm_2d_rgb565_my_hw_tile_copy_with_opacity()`

      3. Modify ***\_\_arm_2d_rgb565_my_hw_tile_copy_with_opacity*** to use your own hardware accelerator. 

      4. Based on the arguments passed to the function and the capability of your 2D accelerator, you can:

         - return `ARM_2D_ERR_NOT_SUPPORT` if the hardware isn't capable to do what is requested.
         - return `arm_fsm_rt_cpl` if the task is done immediately and no need to wait.
         - return `arm_fsm_rt_async` if the task is done asynchronously and later report to arm-2d by calling function `__arm_2d_notify_sub_task_cpl()`. 

      ***NOTE***: The Arm-2D pipeline will keep issuing tasks to your ***hardware adaptor***, please quickly check whether the hardware is capable of doing the work or not, and then add the task (an `__arm_2d_sub_task_t` object) to a list in ***First-In-First-Out*** manner if your hardware adaptor decides to keep it. After that, your hardware accelerator can fetch tasks one by one.  

``````c
typedef struct __arm_2d_sub_task_t __arm_2d_sub_task_t;


typedef arm_fsm_rt_t __arm_2d_io_func_t(__arm_2d_sub_task_t *ptTask);

typedef struct __arm_2d_low_level_io_t {
    __arm_2d_io_func_t *SW;
    __arm_2d_io_func_t *HW;
} __arm_2d_low_level_io_t;

...

/*----------------------------------------------------------------------------*
 * Low Level IO Interfaces                                                    *
 *----------------------------------------------------------------------------*/
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_C8BIT, __arm_2d_c8bit_sw_tile_copy);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_RGB16, __arm_2d_rgb16_sw_tile_copy);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_RGB32, __arm_2d_rgb32_sw_tile_copy);

...

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_ONLY_C8BIT, __arm_2d_c8bit_sw_tile_fill_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_ONLY_RGB16, __arm_2d_rgb16_sw_tile_fill_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_ONLY_RGB32, __arm_2d_rgb32_sw_tile_fill_only);

...
``````

