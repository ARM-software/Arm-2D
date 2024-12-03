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

  - For the way of adding supports for hardware accelerations. Please click to [here](./how_to_accelerate_arm_2d.md) for details. 

