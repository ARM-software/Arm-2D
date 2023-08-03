cmake_minimum_required(VERSION 3.14)
project(ARM2D)

option(ARM2D_HOST "Build for host" OFF)
option(ARM2D_HELIUM "Build with Helium support" OFF)
option(ARM2D_ALPHA_BLENDING "Build with alpha blending support" OFF)
option(ARM2D_TRANSFORM "Build with transform support" OFF)
option(ARM2D_HELPER "Build with helper support" OFF)
option(ARM2D_CMSIS_RTOS2 "Build with CMSIS-RTOS2 helper support" OFF)
option(ARM2D_RT_THREAD "Build with RT-Thread helper support" OFF)
option(ARM2D_CUSTOM_RTOS "Build with custom RTOS helper support" OFF)
option(ARM2D_LCD_PRINTF "Build with LCD printf support" OFF)
option(ARM2D_CONTROLS "Build with controls" OFF)
option(ARM2D_BENCHMARK_GENERIC "Build with generic benchmark support" OFF)
option(ARM2D_BENCHMARK_WATCHPANEL "Build with watchpanel benchmark support" OFF)
set(CMSISCORE "" CACHE STRING "Path to CMSIS Core")

add_library(ARM2D STATIC)

if (ARM2D_HOST)
    target_compile_definitions(ARM2D PUBLIC HOST)

    target_compile_options(ARM2D PUBLIC 
    -DARM_SECTION\(x\)= 
    -D__va_list=va_list)
else() 
    target_include_directories(ARM2D PUBLIC ${CMSISCORE}/Include)
endif()

target_sources(ARM2D PRIVATE Library/Source/arm_2d.c
	Library/Source/arm_2d_async.c
	Library/Source/arm_2d_draw.c
	Library/Source/arm_2d_conversion.c)

if (ARM2D_HELIUM)
	target_sources(ARM2D PRIVATE Library/Source/arm_2d_helium.c)
endif()

if (ARM2D_ALPHA_BLENDING)
	target_sources(ARM2D PRIVATE Library/Source/arm_2d_alpha_blending.c)
endif()

if (ARM2D_TRANSFORM)
	# Require CMSIS-DSP
	target_sources(ARM2D PRIVATE Library/Source/arm_2d_transform.c)
endif()

if (ARM2D_HELPER)
	target_include_directories(ARM2D PUBLIC Helper/Include/)

	target_sources(ARM2D PRIVATE 
		    Helper/Source/arm_2d_helper.c
            Helper/Source/arm_2d_helper_pfb.c
            Helper/Source/arm_2d_helper_scene.c
            Helper/Source/arm_2d_helper_list.c
            )
endif()

if (ARM2D_CMSIS_RTOS2)
	target_sources(ARM2D PRIVATE Helper/Source/template/arm_2d_helper_rtos_cmsis_rtos2.c)
endif()

if (ARM2D_RT_THREAD)
	target_sources(ARM2D PRIVATE Helper/Source/template/arm_2d_helper_rtos_rt_thread.c)
endif()

if (ARM2D_CUSTOM_RTOS)
	target_sources(ARM2D PRIVATE Helper/Source/template/arm_2d_helper_rtos_user.c)
endif()

if (ARM2D_LCD_PRINTF)
	target_include_directories(ARM2D PUBLIC examples/common/controls/)

	target_sources(ARM2D PRIVATE examples/common/controls/lcd_printf.c
            examples/common/controls/GLCD_Fonts.c)
endif()

if (ARM2D_CONTROLS)
	target_include_directories(ARM2D PUBLIC examples/common/controls/)
	target_sources(ARM2D PRIVATE 
		    examples/common/controls/controls.c
            examples/common/controls/busy_wheel.c
            examples/common/controls/shape_round_corner_box.c
            examples/common/controls/spinning_wheel.c
            examples/common/controls/progress_bar_drill.c
            examples/common/controls/progress_bar_flowing.c
            examples/common/controls/progress_bar_simple.c
            examples/common/controls/number_list.c
            examples/common/controls/progress_wheel.c
            examples/common/controls/list_view.c
            examples/common/controls/battery_gasgauge.c
            examples/common/controls/dynamic_nebula.c
            examples/common/asset/white_dot.c
            examples/common/asset/SpinWheel.c
            examples/common/asset/blue_slashes.c
            examples/common/asset/wave.c
            examples/common/asset/cmsis_logo.c
            examples/common/asset/GreenCircle.c
            examples/common/asset/QuaterArc.c
            examples/common/asset/ListCover.c
            examples/common/asset/DigitsFont.c
            examples/common/asset/battery_boarder_1.c
            examples/common/asset/battery_gasgauge_block.c
            examples/common/asset/battery_gasgauge_grade_boarder.c
            examples/common/asset/SinWave.c
            examples/common/asset/glass_reflection_wide.c
            examples/common/asset/glass_reflection_narrow.c
            examples/common/asset/Lighting.c
            examples/common/asset/Pointer.c
            examples/common/asset/watch_panel.c
            examples/common/asset/pointer_sec.c
            examples/common/asset/gear_01.c
            examples/common/asset/gear_02.c
            examples/common/asset/star.c
            examples/common/asset/small_icon_sun.c
            examples/common/asset/software.c
            examples/common/asset/circle_mask.c
            examples/common/asset/circle_small.c
            examples/common/asset/wifi_signal.c
            examples/common/asset/helium.c
            examples/common/asset/background.c
          )
endif()

if (ARM2D_BENCHMARK_GENERIC)
	target_include_directories(ARM2D PUBLIC examples/common/benchmark/)
	target_sources(ARM2D PRIVATE examples/common/benchmark/benchmark_generic.c
            examples/common/benchmark/arm_2d_scene_benchmark_generic.c
			examples/common/benchmark/arm_2d_scene_benchmark_generic_cover.c
          )
endif()

if (ARM2D_BENCHMARK_WATCHPANEL)
	target_include_directories(ARM2D PUBLIC examples/common/benchmark/)
	target_sources(ARM2D PRIVATE examples/common/benchmark/benchmark_watch_panel.c
            examples/common/benchmark/arm_2d_scene_benchmark_watch_panel.c
			examples/common/benchmark/arm_2d_scene_benchmark_watch_panel_cover.c
          )
endif()



target_include_directories(ARM2D PUBLIC Library/Include/)
