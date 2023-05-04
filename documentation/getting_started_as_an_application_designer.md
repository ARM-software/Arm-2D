# Getting Started as an Application Designer <!-- omit from toc -->

## 1 Before We Start

**Arm-2D is not a GUI.** It lacks many key elements to be called GUI, such as the element tree, the message processing, a rich set of controls, the support for interactive input devices, and a GUI designer.

If possible, I hope that no one would have to use Arm-2D directly to create GUI applications. However, under the pressure of cost, there are always some embedded products developing GUI applications in resource-constrained environments (e.g. 64K Flash, 12K RAM etc.). Meanwhile, although some devices have relatively richer resources, the application software is often too big to reserve sufficient memory for a decent GUI stack. Supporting GUI application in resource constraint environments is a world "forgotten" by most of the mainstream embedded GUI stacks.

If the graphics in your application is simple, i.e. consists of panels (for displaying parameters and user settings), and the human-machine interactions are implemented based on a keyboard (or a touch panel without complex touch gestures), then using Arm-2D APIs directly to design GUI applications might be an option.  In fact, if you can figure out a way to fulfill the missing parts aforementioned, the GUI application designed with Arm-2D is not only looking good but also has an impressively low memory footprint. 

If you happen to meet the conditions above, **don't panic**, we will try our best to provide you with a simple and efficient user experience. **By focusing on the API usage and following the guidance** introduced in this article, **you can master the Arm-2D application design quickly**.


- [1 Before We Start](#1-before-we-start)
- [2 Step by Step Guidance](#2-step-by-step-guidance)
  - [2.1 Familiar with the basic concepts](#21-familiar-with-the-basic-concepts)
  - [2.2 Port Arm-2D to your local platform](#22-port-arm-2d-to-your-local-platform)
  - [2.3 Play with Display Adapter, Scene Player and Scenes](#23-play-with-display-adapter-scene-player-and-scenes)
  - [2.4 Try the basic Tile Operations and the Layout Assistant](#24-try-the-basic-tile-operations-and-the-layout-assistant)
  - [2.5 Try existing example controls and scene templates](#25-try-existing-example-controls-and-scene-templates)
  - [2.6 Write a simple GUI Application](#26-write-a-simple-gui-application)
  - [2.7 Learn the basic method to optimize the performance and memory footprint](#27-learn-the-basic-method-to-optimize-the-performance-and-memory-footprint)
  - [2.8 Tips for other advanced topics](#28-tips-for-other-advanced-topics)
- [3 Tips For Exploring the Library](#3-tips-for-exploring-the-library)
  - ["I am a library user, I only care about how to use the library"](#i-am-a-library-user-i-only-care-about-how-to-use-the-library)


## 2 Step by Step Guidance

### 2.1 Familiar with the basic concepts

### 2.2 Port Arm-2D to your local platform

### 2.3 Play with Display Adapter, Scene Player and Scenes

### 2.4 Try the basic Tile Operations and the Layout Assistant

### 2.5 Try existing example controls and scene templates

### 2.6 Write a simple GUI Application

### 2.7 Learn the basic method to optimize the performance and memory footprint

### 2.8 Tips for other advanced topics


## 3 Tips For Exploring the Library

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
