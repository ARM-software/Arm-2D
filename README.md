# README {#mainpage} <!-- omit from toc -->

![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/ARM-software/Arm-2D?include_prereleases) ![GitHub](https://img.shields.io/github/license/ARM-software/Arm-2D)

## Overview

**Arm-2D is an open-source project for 2.5D image processing on Cortex processors.**

- Initial target: IoT endpoint devices, white goods, hand-held devices and wearables, especially for devices with **resource-constrained** and **low power** requirements.
- Initial focus: Graphical User Interface

**Arm-2D consists of:**

- Generic software **pixel pipeline** infrastructure for graphic processing
  - A set of rules for adding new algorithms and colour formats
  - A default software implementation and **SIMD acceleration** when [Helium technology](https://developer.arm.com/architectures/instruction-sets/simd-isas/helium) is available.
  - Supports adding **3rd party hardware accelerators** at low-level in a "feature-agnostic" manner.
- A **CMSIS-Pack** for ease of use
- Examples and Documents

<img src="./documentation/pictures/ReadmeOverview.png" alt="PositionInEcosystem" style="zoom:67%;" /> 

**Position in Ecosystem:**

- **Focus on low-level** and works as **a hardware abstraction layer** for 2.5D image processing.
- **Arm-2D is NOT a GUI.**
  - No content creation, complex shape drawing or Scalable Vector Graphics (SVG) support
- **Provide conveniences for Arm eco-partners to create value through differentiation.**
  - Provide the default implementation for commonly used 2.5D operations and **enable 3rd-parties to accelerate application-specific algorithms.**

**Current Focus and Objectives:**

- **Help industry partners with technology adoption**
  - Guidance for integration of Arm-2D into graphic stacks
- **Optimisation for ultra-small memory footprint**
  - Enable existing Cortex-M processors to use modernised GUI with no or few cost increases.
  - Helping customers to cost-down

## Key Messages for You

- **Arm-2D always keeps light-weight**
  - Arm-2D keeps an ultra-low memory footprint for both ROM and RAM

  - Arm-2D uses the commonly available linker features to remove unused functions and data whenever possible.
    - Only functions that are actually used will take up memory

    - Please use feature-specific APIs whenever possible. 
      - For example, if you want to copy an rgb565 picture to a target tile with a source mask without any mirroring, please call `arm_2dp_rgb565_copy_with_src_mask_only()` rather than the generic one, i.e. `arm_2dp_rgb565_copy_with_src_mask()` which takes a mirroring mode as a parameter and keeps all low-level implementations of all possible mirroring modes.
- **Arm-2D always keeps a flat and straightforward framework, i.e. API Wrappers, Frontend and Backend.**

  - API wrappers take user input and generate task descriptors for the following stages.
  - Frontend is responsible for the commonly used and necessary services,
    - for example, region calculation, region clipping, pre-mirroring etc. So the Backend will receive simple, validated and detailed tasks which are friendly for hardware accelerators.
    - **The Frontend is small.**
  - The Backend is the place of Low-level implementations for specific 2D processing algorithms.
    - **Those algorithms are usually unusable if you take them out of arm-2d** because the data are validated in the Frontend, and tasks are simplified in the Frontend also.
    - **The linker will remove unused low-level implementations.**

<img src="./documentation/pictures/PixelPipeline.png" alt="PositionInEcosystem" style="zoom:67%;" />

- **Arm-2D always keeps backward compatibility**
  - Older APIs will still be usable in newer versions (even if they are marked as deprecated)
  - Arm-2D available for ALL Cortex-M processors

## How to Get Started

Potential users of Arm-2D may have different backgrounds and purposes. Whoever you are,  e.g. an application engineer from a silicon vendor who develops drivers, a GUI stack system engineer, or an application developer, based on the purposes , there are two ways to get started with Arm-2D:

- [Getting Started as a GUI Application Designer](./documentation/getting_started_as_an_application_designer.md)
  - In some resource-constrained systems, the Flash (ROM) and SRAM are limited (e.g. 64K Flash, 12K SRAM), or the memory space left for GUI applications is limited, such systems cannot afford the cost of a decent GUI stack.
    Meanwhile, these kinds of low-cost systems usually don't have a complex HMI, it is possible to use the arm-2d APIs to design applications directly .
  - Engineers focused on embedded video processing can preprocess the frame buffer just captured from cameras with Arm-2D. 
- [Getting Started as a GUI System Developer](./documentation/getting_started_as_a_gui_stack_developer.md)
    - Silicon vendors can provide drivers to [further accelerate Arm-2D](./documentation/how_to_accelerate_arm_2d.md) in order to take advantage of the proprietary hardware accelerators for their customers. 
    - System engineers from GUI stack providers can use the Arm-2D APIs to accelerate GUI's low-level operations. 
    - Homebrew GUI designers can create entirely-new GUI stacks faster and easier with Arm-2D.
    - ...

In general, anyone from opensource community can find something interesting from Arm-2D.



## Table of Content

- [Overview](#overview)
- [Key Messages for You](#key-messages-for-you)
- [How to Get Started](#how-to-get-started)
- [Table of Content](#table-of-content)
- [Features](#features)
  - [In this version (v1.2.2-rc8)](#in-this-version-v122-rc8)
  - [New Features Planned in the Future](#new-features-planned-in-the-future)
- [1 Introduction](#1-introduction)
  - [1.1 The Background](#11-the-background)
  - [1.2 The Problems in Current Solutions](#12-the-problems-in-current-solutions)
- [2 The Platform and Dependency](#2-the-platform-and-dependency)
  - [2.1 Processors and Hardware](#21-processors-and-hardware)
  - [2.2 Dependency](#22-dependency)
- [3 Examples, Benchmark and Templates](#3-examples-benchmark-and-templates)
  - [3.1 Summary](#31-summary)
  - [3.2 Benchmark](#32-benchmark)
- [4 Limitations](#4-limitations)
  - [4.1 The Generic Limitations](#41-the-generic-limitations)
  - [4.2 The Temporary Limitations](#42-the-temporary-limitations)
- [5 Folder Hierarchy](#5-folder-hierarchy)
- [6 Documentation](#6-documentation)
- [7 Feedback](#7-feedback)

## Features

### In this version (v1.2.2-rc8)

Since Arm-2D provides **Low-Level 2D Image Processing Services** that are deeply optimized in both performance and memory footprint, it has been quickly adopted in many **Deep Embedded Display systems** soon after it debuted in 2020.  Now you can find its [precence](https://www.linkedin.com/search/results/all/?keywords=%23amazingarm2d&origin=HASH_TAG_FROM_FEED) from the [tiniest Cortex-M0](https://www.linkedin.com/posts/gabriel-wang-554523167_amazingarm2d-activity-7239115506102726658-CKhE?utm_source=share&utm_medium=member_desktop&rcm=ACoAACfEJVABu43psSmn3fAYP2dptCi3QU4o0Yg) to the [Raspberry Pi 5 powerhouse](https://www.linkedin.com/posts/gabriel-wang-554523167_embeddedworld-activity-7305214952544391170-WkLP?utm_source=share&utm_medium=member_desktop&rcm=ACoAACfEJVABu43psSmn3fAYP2dptCi3QU4o0Yg).  The supported features include but not limited to:

- **Alpha-Blending** / **Masks**

  - With or without Colour-Keying
  - Filling-Colour with a mask and an optional opacity
    - For drawing anti-alias icons/texts in a specified colour
    - Supports A1, A2, A4 and A8 masks
    - Supports horizontal and vertical line masks
    - Supports Colour-Filling with Alpha-Gradient defined in 4-points, 3-points, horizontal and vertical mode.
      - Supports Mask
      - Supports an optional Opacity

- **Image Copy (Blit)**

  - With or without Colour-Keying
  - Supports four mirroring modes: No-mirroring, X-mirroring, Y-mirroring and XY-mirroring
  - Supports **masks** on the source side and/or the target side
  - Provides API variants that accept **Opacity** as arguments.
  -  Implicit colour conversion for **ARGB8888** (**ccca8888**)

- **Supported Colour formats**

  - **GRAY8 (8-bit Grayscale)**, **RGB565** and **RGB888**
  - Generic **C8BIT (8bit colour)**, **RGB16** and **RGB32**
  - Converting colour formats among **GRAY8**, **RGB565** and **RGB888**
    - Implicit colour conversion for **ARGB8888** (**ccca8888**)
  - Ready for **monochrome LCD** (the 1bit colour) and **E-ink displays** (the 2bits and 4bits colour formats)
    - Using **Gray8** as the internal processing format and converting to target colour format inside the Display Adapter, e.g. `Disp0_DrawBitmap`
    - Provides a dedicated colour solution in the display adapter service for monochrome display devices.

- **Display Adapter Service for connecting LCD**

  - Generic Partial Frame-buffer (PFB)
    - Easy to Use: transparent for applications
    - No limitation on screen resolution **(See note 1)**
    - Flexible in PFB size and shape (it could be a line or a rectangular with any size)  **(See note 2)**
    - Supports PFB alignment for both width and height.

    > [!NOTE]
    >
    > 1. As long as the size is smaller than 32767 * 32767. 
    >
    > 2. As long as the total pixel numebr is smaller than 16M pixels.
    
  - Supports **Dirty Regions** for refreshing specified areas only
    - Provides a debug mode for dirty regions
    - Provides optimization for dirty regions to reduce the chances of refreshing overlapped areas.
    - Provides dirty region helper services for ease of use.
    
  - Supports swapping high and low bytes for RGB16 pixels

  - Provides a **Scene Player** with various scene switching modes (Fade-In-Fade-Out, Slide, Erase etc.)

  - Supports various buffering Modes with a built-in frame buffer pool.
    - Single Buffer/Double-Buffers
    - Provides a helper service for Direct Mode (i.e. 3FB Mode)
    - Provides a helper service for Asynchronous Flushing (i.e. DMA + ISR)

- **Transform** (i.e. rotation and scaling)
  - With/Without Colour-keying
  - Supports an optional **Opacity** ratio
  - Supports Anti-Alias.
    - You can enable it by defining the macro  `__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__` to `1` at **compile-time**.
  - Supports source masks
  - **[new]** Supports specifying different scaling ratios for X and Y axes, respectively
  - **[new]** Supports using floating point numbers (i.e. `float`) to specify the pivot coordinate.

- **Filters**

  - Generic Anti-alias and Fast IIR-Blur
  - Colour Inversion

- **An Unified and User-Friendly Programmers' Model**
  - APIs can be used in **Synchronous** manner (  **Classic Blocking-code** ) and/or **Asynchronous** manner ( **Event-Driven** )
  - Supports both **bare-metal** and **RTOS** environments
  - Ultra-small memory footprint

- **Helper Services, Tools and Others**

  - **Timer Services**

    - Timeout: `arm_2d_helper_is_time_out`
    - Time-based functions:
      - Liner: `arm_2d_helper_time_liner_slider`
      - Cosine: `arm_2d_helper_time_cos_slider`
      - S-curve: `arm_2d_helper_time_half_cos_slider`

  - **Virtual Resource**

    - Supports loading resources on-demand from external memories which are not mapped into the Cortex-M memory space.
    - Supports background-image-loading mode that requests **NO** extra PFB block. 
    - **[new]** A JPEG loader derived from TJpgDec
      - Supports and Optimized for PFB and Dirty Regions

  - **Layout Assistant**
    - Alignments, e.g. `arm_2d_align_centre`, `arm_2d_align_bottom_right` etc.
    
    - Docking, e.g. `arm_2d_dock_top`,`arm_2d_dock_right`, `arm_2d_dock_vertical`, `__item_line_dock_horizontal` etc.
    
    - Line Stream Layout, e.g. `__item_line_horizontal` and `__item_line_vertical`
    
    - Stream Layout (with wrapping), e.g. `__item_horizontal` and `__item_vertical`
    
    - A dedicated layout debug mode
    
    - Users can specify alignment in `arm_2d_layout()` macro: TOP-DOWN, BOTTOM-UP, RIGHT-TO-LEFT etc.
    
  - RTOS Helper Services for CMSIS-RTOS2 and RT-Thread

  - **Templates**
    - Provides templates for user controls and scenes.
    - Scene templates for Meter, Watch and Fitness trackers
    - Provides a template for adding new RTOS support.
    
  - **Tools**
    - A `img2c.py` for generating arm-2d resources from user-specified images
    - A `ttf2c.py` for generating user-customized A1, A2, A4 and A8 fonts from an user-specified TrueType Font
    - A `jinja2c.py` for code generation.

  - **Demos**
    - Demos for various scenarios
    - A dedicated demo for Helium-ACI acceleration. 
    - A set of demos for monochrome devices

  - **Project Templates**
    - A project template for Windows, Linux and MacOS platform
    - A project template for Raspberry Pi Pico

- **CMSIS-Pack is available.**

- **Ready and Welcome 3rd party adoption**

  - Accelerating [LVGL](https://lvgl.io/) as [soft-GPU](https://docs.lvgl.io/master/details/integration/renderers/arm2d.html) when Helium is ready
  - Accelerating [CrankSoftware](https://www.cranksoftware.com/) as [soft-GPU](https://info.cranksoftware.com/platforms/demo-images/renesas/ra8d1) when Helium is ready

### New Features Planned in the Future

- Perspective Transform
- Supports DMAC-350
- SVE2 Acceleration

![](./documentation/pictures/FuturePlans.png)

## 1 Introduction

### 1.1 The Background

With the increasing introduction of smart IoT edge devices into our daily lives, users accustomed to smartphone-like graphical user interfaces (GUIs) now expect a similar modernized experience, even when using microcontroller-based products. This trend has been long recognized and understood by Arm's ecosystem partners.

Consequently, many silicon vendors have integrated dedicated hardware accelerators into their microcontroller products to facilitate 2D image processing. At the same time, GUI service providers have updated their product lines to cater to microcontroller-based GUI applications. Many open-source embedded GUI stacks, such as [LVGL](https://lvgl.io/), are gaining popularity on GitHub.

In reality, using GUIs with microcontrollers is not a new concept. However, with the recent rise of IoT and AI, the limitations of simple and monotonous GUIs have become more apparent and less tolerable. Pursuing a user experience comparable to a smartphone has become a fundamental requirement for many products.

Interestingly, seemingly complex GUIs are often created using simple techniques like 2D image copying and tiling. Even translucent effects are achievable on microcontrollers running at tens or hundreds of MHz.

Technologies for 2D graphics have been mature since the era of 8-bit gaming consoles. If an 8-bit 6502 processor could produce impressive graphics effects, there is no reason why the most advanced Cortex-M processors today cannot achieve even more.

### 1.2 The Problems in Current Solutions

As shown in **Figure 1-2**, Linux based systems have a complete ecosystem that provides GPU drivers and GUI software services. In contrast, resource-constrained embedded systems do not have such luxury. To enhance the MCU ecosystem, Arm-2D is introduced.

**Figure 1-2 Ecosystem Comparison between Rich Embedded and Constraint Embedded System in GUI**

![ Ecosystem Comparison](./documentation/pictures/TopReadme_1_2a.png)

When we look at the traditionally embedded  GUI architecture(as shown in **Figure 1-3** ), there are four layers: 1) the application and designer layer, 2) the GUI software service layer, 3) the rendering layer, and 3) the hardware driver layer.

**Arm-2D focuses on accelerating the low-level 2D image processing**, and will not touch upper layers of the GUI software stacks, hence avoiding competing with GUI service providers in the same ecosystem. In fact, because Arm has proposed an abstraction layer for the low-level 2D image processing, **a whole ecological level of cooperation can be quickly established** between chip manufacturers that provide hardware accelerators and software providers that provide GUI services. **Everyone can concentrate on enlarging their own values**. For example, by adding arm-2d adaptor drivers for their proprietary 2D accelerators, silicon vendors can achieve a wide range of support from GUI providers who build/port their GUI stack upon Arm-2D APIs.

**Figure 1-3 The Hierarchy of a Typical Embedded GUI System.**

![The Hierarchy of a Typical Embedded GUI System](./documentation/pictures/TopReadme_1_2b.png) 

## 2 The Platform and Dependency

### 2.1 Processors and Hardware

**The library is targeting ALL Cortex-M processors with/without various hardware 2D image accelerators:**

- Armv6-M processors: Cortex-M0/M0+/M1/SC000
- Armv7-M processors: Cortex-M3/M4/M7/SC300
- Armv8-M processors: Cortex-M23/M33/Star-MC1/M35P
- Armv8.1-M processors: Cortex-M55/M85/M52

**The library is designed with ACI (Arm Custom Instructions) in mind.** Accelerations implemented with user-defined instructions can be integrated into the library easily as software extensions and it is transparent for upper-layer software.

**The library is designed with 2D image accelerators in mind and follows the feature-agnostic principle.**  The support for 2DGPU (or other Accelerators) can be easily added as software extensions and it is transparent for upper-layer software.

**The library is designed with resource constraints in mind.** **For Cortex-M processors with 8K~32K SRAM that cannot afford a complete framebuffer**, Arm-2D introduces a feature called **Generic Partial Framebuffer**, enabling those existing MCUs to run GUI applications in a decent frame rate.

### 2.2 Dependency

- The library depends on **CMSIS 5.7.0 and above** (If you want to use Arm-2D with Cortex-M55, CMSIS 5.8.0 is required).
- The library is developed with the **C11** standard and depends on some **widely adopted GCC extensions**.
  - See **section 3.2** for details.
- The library supports **Arm Compiler 6**, **GCC**, **LLVM** and **IAR**
  - See **section 5** for details.

## 3 Examples, Benchmark and Templates

### 3.1 Summary

| Projects                     | Description                                                  | Folder                                | Note    |
| ---------------------------- | ------------------------------------------------------------ | ------------------------------------- | ------- |
| \[template\]\[bare-metal\]   | It is a project template for the bare-metal environment.     | examples/\[template\]\[bare-metal\]   |         |
| \[template\]\[cmsis-rtos2\]  | It is a project template for the RTOS environment, which uses CMSIS-RTOS2 as an example to show how Arm-2D can work with an RTOS. | examples/\[template\]\[cmsis-rtos2\]  |         |
| \[template\]\[pc\]\[vscode\] | It is a project template for PC (i.e. **MacOS**, **Windows** and **Linux**) using **VS Code + SDL2** | examples/\[template\]\[pc\]\[vscode\] |         |
| \[template\]\[csolution\]    | It is a csolution project template.                          | examples/\[template\]\[csolution\]    |         |
| \[template\]\[pico\]\[oled]  | An MDK project template for Raspberry Pi Pico to demonstrate OLED (monochrome) display. | examples/\[template\]\[pico\]\[oled]  | **New** |

### 3.2 Benchmark

There is no public 2D image processing benchmark available for microcontrollers. To facilitate the library development and optimization,  we overcome this problem with the following methods:

- **Choose the widely used algorithms in embedded GUI as the body of the benchmark**
  - Alpha-blending
  - Colour-Keying
  - Blit
  - Tiling
  - Transform
  - Mirroring
  - Masking
  - Colour-Filling (with alpha gradient)
- **Simulate a typical application scenario with sufficient complexity**
  - Background with Texture Tiling (switching mirroring modes every 4 seconds)
  - Foreground picture
  - Two layers for alpha-blending and tiling with colour keying
  - Moving icons
  - Spinning wheels
- **Choose a typical low-cost LCD resolution 320*240 in RGB565**
- **Let those layers float with different speeds and directions to cover corner cases.**
- **Accumulate the cycle count used for 1000 frames (iterations)**

**Figure 1-4 A snapshot of the generic benchmark running in Simulation**

<img src="./documentation/pictures/benchmark.png" alt="Benchmark" style="zoom:55%;" /> 

- **Calculate the average cycle count of 1000 frames, and use as the benchmark score (FPS30Freq)**
- For typical embedded applications, we derive a more meaningful metric called the **Minimal Frequency Required for 30 FPS (MHz), a.k.a FPS30Freq** as shown in **Figure 1-5**.

**Figure 1-5 Performance Comparison among some Cortex-M processors**

![Performance Comparison among some Cortex-M processors](./documentation/pictures/PerformanceDiagram.png) 



## 4 Limitations

### 4.1 The Generic Limitations

- The library focuses on Cortex-M processors and is also available for other platforms, e.g. Cortex-A processors and other 32/64 bit architectures.
- The library supports the following compilers:
  - Arm Compiler 5 (**Deprecated**)
  - Arm Compiler 6
  - GCC
  - LLVM
  - IAR
- The library focus on **Accelerating Low Level Pixel Processing**
  - In principle, the library will **NOT** provide APIs for content creation, such as drawing shapes, text display, etc., but simple point drawing and colour-filling APIs.
    - We provides demos to illustrate how to draw basic shapes using User-Defined-OPCODE.
  - In principle, the library will **NOT** provide a ready-to-use element tree, or GUI message handling, but some meta data structrues that homebrew GUI developers can use directly(or take as references). 

### 4.2 The Temporary Limitations

- The library currently only provides default software algorithms in C and a **[Helium](https://developer.arm.com/architectures/instruction-sets/simd-isas/helium) based acceleration library**.
- Most of the example projects are created in MDK.

## 5 Folder Hierarchy

| Folder and File | Type    | Description                                                  |
| :-------------- | ------- | ------------------------------------------------------------ |
| **Library**     | Folder  | This folder contains the library's source files and header files. |
| **Helper**      | Folder  | This folder contains the source files and header files of helper functions/services. |
| Acceleration    | Folder  | This folder contains hardware specific accelerations, e.g. Helium-ACI example in FVP |
| documentation   | Folder  | This folder contains all the documents.                      |
| examples        | Folder  | This folder contains all the examples, controls and templates, etc. |
| README          | .md     | The README.md you are currently reading.                     |
| LICENSE         | License | The Apache 2.0 License                                       |
| tools           | Folder  | This folder contains useful utilities for using the library. For example, img2c.py is a Python script that converts specified pictures into the arm2-d tile data structures. |

## 6 Documentation

| Name                                                         | Description                                                  | Location                                                     |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| API Manual                                                   | An API manual generated by Doxygen                           | * ![GitHub release](https://img.shields.io/github/v/release/ARM-software/Arm-2D) : [Documentation for latest official release](https://arm-software.github.io/Arm-2D/latest) <br/>* [Documentation for latest development release](https://arm-software.github.io/Arm-2D/developing) |
| **README.md**                                                | It is the document that you are reading. It provides basic information and guidance for the arm-2d library. | (root)                                                       |
| **[getting_started_as_an_application_designer.md](./documentation/getting_started_as_an_application_designer.md)** | A guidance for GUI application designers.                    | documentation                                                |
| **[getting_started_as_a_gui_stack_developer](./documentation/getting_started_as_a_gui_stack_developer.md)** | A guidance for GUI stack developers.                         | documentation                                                |
| **[how_to_deploy_the_arm_2d_library.md](./documentation/how_to_deploy_the_arm_2d_library.md)** | A step by step guide that helps you to deploy the library to your existing or new projects. | documentation                                                |
| **[introduction.md](./documentation/introduction.md)**       | A relatively detailed introduction for the library, including basic concepts, programmers' mode etc. | documentation                                                |
| **[how_to_use_layout_assistant.md](/documentation/how_to_use_layout_assistant.md)** | A detailed document introduce the layout assistant helper service. | documentation                                                |
| **[how_to_use_tile_operations.md](./documentation/how_to_use_tile_operations.md)** | A detailed document elaborates the APIs dedicated to basic tile operations in the arm-2d library. | documentation                                                |
| **[how_to_accelerate_arm_2d.md](./documentation/how_to_accelerate_arm_2d.md)** | A detailed document elaborates the methods available for accelerating Arm-2D library. | documentation                                                |
| **how_to_use_alpha_blending_operations.md**                  | A detailed document elaborates the APIs dedicated to alpha-blending services provided by the arm-2d library. | documentation                                                |
| **how_to_use_conversion_operations.md**                      | A detailed document elaborates the APIs dedicated to colour space conversion services provided by the arm-2d library. | documentation                                                |
| **how_to_use_drawing_operations.md**                         | A detailed document elaborates the APIs that provide basic point-drawing and colour-filling services in the arm-2d library. | documentation                                                |
| **[how_to_contribute](./documentation/contribution.md)**     | A guidance for contribution.                                 | documentation                                                |

## 7 Feedback

Arm-2D aims to bridge the gap between **low-cost, resource-constrained devices** and **high-performance, feature-rich embedded IoT edge devices** by providing **a unified and user-friendly programming model**. Our goal is to facilitate the seamless and easy migration of GUI applications across Cortex-M, Cortex-R, and Cortex-A platforms. We hope that Arm-2D will inspire more initiatives and engineering practices in this field.

Your thoughts and feedback are invaluable to us. If you can spend some time trying out the library, please feel free to share your insights. It would be helpful to cover the following perspectives:

- The industrial segments where you plan to introduce a GUI using Cortex processors.
- The resources of your platform, such as the size of the RAM, ROM, system frequency, and average power consumption.
- The LCD resolution and target frame rate (FPS).
- Algorithms that you find most useful but are currently missing.
- Details about the device:
  - Does it contain hardware accelerators for 2D image processing?
  - What is the feature set of this 2D accelerator?
  - Are there features in the 2D accelerator currently missing in this library?
- Any other thoughts or suggestions?

Thank you for your time.

***Arm-2D Development Team.***

29 April 2025
