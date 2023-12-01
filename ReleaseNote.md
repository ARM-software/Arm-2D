# Release Note



## Release v1.1.5 (01 Dec 2023)

### Core and Library

- Improves the support for masks in the pixel-pipeline.
- Updates transform operations
  - Always generating a child tile for the target tile in transform operations.
  - Adds optimization for hollow out masks in C implementations
- Adds the support for adding **Asynchronous acceleration**, **Synchronous acceleration** and **ACI acceleration**.
- Adds the support for trace log
- Adds a new API: `arm_2d_is_region_inside_target()`

### Helper Services

- Updates `ttf2c.py` for generating **A1**, **A2** and **A4** fonts.
- Updates the Display Adapter Service (the PFB Helper Service)
  - Introduces a **Dry-Run** mode in the PFB helper service.
  - Introduces the **Dynamic Dirty Region**, a new type of dirty region items that allows users to update its area during drawing and interact with the PFB. helper service. 
  - Introduces the concept of `virtual screen` which is used to anchor the virtual screen in the PFB helper service.
  - Introduces the Dirty Region Optimization in the PFB Helper service.
  - Updates the performance statistic service in the Display Adapter service.
    - Introduces the CPU Usage info calculation.
    - Places the performance statistic to the Display Adapter class.
  - Simplifies the way of navigation layer customization.
  - Improves the robustness of the Asynchronous Flushing helper.
  - Improves the dirty region debug mode.
  - Updates the `disp_adapter_task()` to support **Framerate-Locking** in the bare-metal environment.
- Introduces helper macros for docking.
- Adds the char scaling support to A8 fonts.
- Updates the transform helper service.
  - Fixes an issue in region calculation.
  - Improves the performance.

### Examples and Templates

- Adds a new example scene to demonstrate a curve-list-view.
- Adds a new example scene (arm_2d_scene_audiomark) to demonstrate the dirty region optimization
- Adds a new service, **3FB helper service** for supporting the so-called Direct Mode.
- Updates example controls and scene templates.
  - Adds dirty regions to the Progress Wheel.
  - Adds a new API for drawing round corner image.
  - Updates the APIs for drawing round corner boxes and allows specifying user defined resources.
  - Updates the progress bars.
  - Adds a new scene template for fitness trackers.
  - 

- Updates benchmarks
  - Reduces the SRAM footprint of the tiny mode for the generic benchmark.

- Improve PC template

### Documentation and Misc

- Adds an initial support for CMake.
- Introduces a header template `arm_2d_user_arch_port.h` and makes it easier to port arm-2d to a different architecture other than Cortex-M.
- Improves the support for C++ compilation.
- Fixes the support for the Arm Compiler 5.
- Improve the support for Arm Compiler 6.21.
- Adds a new document "**How To Accelerate Arm-2D**"
- Updates the document "How To Deploy Arm-2D" to introduce the supports for Asynchronous Flushing Mode and Direct Mode.
- Update benchmarks
- Update README.md
- Adds **ReleaseNote.md**

