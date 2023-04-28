# Getting Started as an Application Designer

Arm-2D is not a GUI. It lacks many key elements required to be called a GUI, such as the element tree, the message processing, a complete set of controls, the support for interactive input devices, or a GUI designer.

If possible, I hope that no one would not be forced to use Arm-2D to develop GUI applications directly. However, under the pressure of cost, there are always some embedded products that need to develop GUI applications in a resource-constrained environment. Meanwhile, although some of the devices have relatively richer resources, the application software is often too big to leave sufficient memory for using a decent GUI stack.

If your graphics application is relatively simple, consists of panels (for displaying parameters and user settings), and the human-machine interactions are relatively simple (through the keyboard, or without complicated touch gestures), then using Arm-2D APIs directly to design GUI applications might be a simple and effective option. 

GUI applications designed with Arm-2D is not only looking good but also have an impressively low memory footprint. This is an application subdivision field that cannot be covered by the current mainstream embedded GUI.

If you happen to meet the conditions aforementioned, **don't panic**, we will try our best to provide you with a simple and efficient user experience. 

Focusing on application development, following the steps introduced in this article, I believe you can quickly master the way of using Arm-2D for application development.


## Tips For Exploring the Library

### "I am a library user, I only care about how to use the library"

- For library users, **ALL** useful information, i.e. type definitions, macros, prototypes of functions etc., are stored in header files which have **NO** double under-scope as their prefixes. We call those header files the **PUBLIC HEADER FILES**. 
- Please **ONLY** use APIs, macros and types that are defined in the public header files. 

- Any symbol, e.g. file name, function name, macro name, type name etc., having a double under-scope as the prefix is considered as **PRIVATE** to the library. You should save your time from touching them. 

- The library is designed with the philosophy that Users are free to use anything in public header files and should not touch anything marked implicitly or explicitly as private. 

- Despite which processor you use, during the compilation, all C source files are safe to be added to the compilation (and we highly recommend you to do this for simplicity reason). For example, when you use Cortex-M4, which doesn't support Helium extension (introduced by Armv8.1-M architecture and first implemented by the Cortex-M55 processor), it is OK to include `arm_2d_helium.c` in the compilation process, as the C source files are constructed with environment detection in pre-processing phase. 

- In your application, including `arm_2d.h` is sufficient to get all the services and APIs ready for you. 

- Make sure that the library is initialised by calling `arm_2d_init()` before using any of the services. 

  **NOTE**: 

  1. Feature configuration macros are checked by `arm_2d_feature.h`. For the current stage of the library, please **DO NOT** override those feature configuration macros.
