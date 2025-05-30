cmake_minimum_required(VERSION 3.14)
project(ARM2D)

option(ARM2D_HOST "Build for host" OFF)
option(ARM2D_HELPER "Build with helper support" OFF)
option(ARM2D_CMSIS_RTOS2 "Build with CMSIS-RTOS2 helper support" OFF)
option(ARM2D_RT_THREAD "Build with RT-Thread helper support" OFF)
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
    -D__va_list=va_list
    -fms-extensions)

   target_compile_options(ARM2D PRIVATE -Wno-macro-redefined)
else() 
    target_include_directories(ARM2D PUBLIC ${CMSISCORE}/Include)
    target_compile_options(ARM2D PRIVATE -Wno-nonnull-compare -Wno-unused-value)
    target_compile_options(ARM2D PUBLIC -fms-extensions)

endif()


target_compile_options(ARM2D PUBLIC
                       -DRTE_Acceleration_Arm_2D
                       -DRTE_Acceleration_Arm_2D_Alpha_Blending
                       -DRTE_Acceleration_Arm_2D_Transform
                       -DRTE_Acceleration_Arm_2D_Filter
                       )
target_compile_options(ARM2D PRIVATE -Wno-format
        -Wno-unused-function)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(ARM2D PRIVATE -Wno-maybe-uninitialized)
endif()
        
target_sources(ARM2D PRIVATE 
    Library/Source/arm_2d.c
	Library/Source/arm_2d_async.c
	Library/Source/arm_2d_draw.c
	Library/Source/arm_2d_conversion.c
    Library/Source/arm_2d_helium.c
    Library/Source/arm_2d_alpha_blending.c
    Library/Source/arm_2d_transform.c
    Library/Source/arm_2d_filter.c
    )


if (ARM2D_HELPER)
	target_include_directories(ARM2D PUBLIC 
                                Helper/Include/
                                Helper/built_in/)

	target_sources(ARM2D PRIVATE 
		    Helper/Source/arm_2d_helper.c
            Helper/Source/arm_2d_helper_pfb.c
            Helper/Source/arm_2d_helper_scene.c
            Helper/Source/arm_2d_helper_list.c
            Helper/Source/arm_2d_helper_shape.c
            Helper/Source/arm_2d_helper_control.c

            Helper/Source/WhiteDotSmall.c
            Helper/Source/cmsis_logo.c
            Helper/Source/circle_small.c

            Helper/built_in/arm_2d_helper_built_in.c
            Helper/built_in/busy_wheel.c
            Helper/built_in/console_box.c
            
            )
    target_compile_options(ARM2D PUBLIC
            -DRTE_Acceleration_Arm_2D_Helper_PFB
            )
endif()

if (ARM2D_CMSIS_RTOS2)
	target_sources(ARM2D PRIVATE Helper/Source/template/arm_2d_helper_rtos_cmsis_rtos2.c)
    target_compile_options(ARM2D PUBLIC
        -DRTE_Acceleration_Arm_2D_Helper_RTOS_CMSIS_RTOS2
        )
endif()

if (ARM2D_RT_THREAD)
	target_sources(ARM2D PRIVATE Helper/Source/template/arm_2d_helper_rtos_rt_thread.c)
    target_compile_options(ARM2D PUBLIC
        -DRTE_Acceleration_Arm_2D_Helper_RTOS_RT_Thread
        )
endif()

if (ARM2D_LCD_PRINTF)
	target_include_directories(ARM2D PUBLIC examples/common/controls/)

	target_sources(ARM2D PRIVATE 
            Helper/Source/arm_2d_helper_font.c
            Helper/Source/GLCD_Fonts.c
            )
    target_compile_options(ARM2D PUBLIC
            -DRTE_Acceleration_Arm_2D_Extra_LCD_printf__
            )
endif()

if (ARM2D_TJPGD_LOADER)
	target_include_directories(ARM2D PUBLIC examples/common/loader/tjpgd_loader)

	target_sources(ARM2D PRIVATE 
            examples/common/opcodes/arm_2d_user_opcode_draw_circle.c
            examples/common/opcodes/arm_2d_user_opcode_draw_line.c
            examples/common/loader/tjpgd_loader/tjpgd_loader.c
            examples/common/loader/tjpgd_loader/tjpgd/tjpgd.c
            )
    target_compile_options(ARM2D PUBLIC
            -DRTE_Acceleration_Arm_2D_Extra_TJpgDec_Loader
            -DRTE_Acceleration_Arm_2D_Extra_Loader
            )
endif()

if (ARM2D_CONTROLS)
	target_include_directories(ARM2D PUBLIC examples/common/controls/)
	target_sources(ARM2D PRIVATE 
            examples/common/controls/controls.c
            
            examples/common/controls/spinning_wheel.c
            examples/common/controls/progress_bar_drill.c
            examples/common/controls/progress_bar_flowing.c
            examples/common/controls/progress_bar_simple.c
            examples/common/controls/progress_bar_round.c
            examples/common/controls/__simple_list.c
            examples/common/controls/number_list.c
            examples/common/controls/text_list.c
            examples/common/controls/text_tracking_list.c
            examples/common/controls/progress_wheel.c
            examples/common/controls/list_view.c
            examples/common/controls/battery_gasgauge.c
            examples/common/controls/dynamic_nebula.c
            examples/common/controls/histogram.c
            examples/common/controls/spin_zoom_widget.c
            examples/common/controls/cloudy_glass.c
            examples/common/controls/meter_pointer.c
            examples/common/controls/text_box.c

            
            examples/common/asset/WhiteDotMiddle.c
            examples/common/asset/WhiteDotBig.c
            examples/common/asset/SpinWheel.c
            examples/common/asset/blue_slashes.c
            examples/common/asset/wave.c
            examples/common/asset/GreenCircle.c
            examples/common/asset/QuaterArcSmall.c
            examples/common/asset/QuaterArcMiddle.c
            examples/common/asset/QuaterArcBig.c
            examples/common/asset/ListCover.c
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
            
            examples/common/asset/wifi_signal.c
            examples/common/asset/helium.c
            examples/common/asset/background.c
            examples/common/asset/BackgroundSmall.c
            examples/common/asset/WatchCoverRound.c
            examples/common/asset/GlassHalo.c
            examples/common/asset/alarm_clock_font32.c
            examples/common/asset/alarm_clock_font64.c
            examples/common/asset/ECG.c
            examples/common/asset/ECGScan.c
            examples/common/asset/fan_blade.c
            examples/common/asset/RadialGradient.c
            examples/common/asset/GlassBall.c
            examples/common/asset/HallowOutCircleMask.c
            examples/common/asset/Fire8x4.c
            examples/common/asset/DigitsFont.c
            examples/common/asset/alarm_clock_font32.c
            examples/common/asset/alarm_clock_font64.c
            examples/common/asset/arial_ascii_font14.c
            examples/common/asset/ClockMarking.c
            examples/common/asset/compass.c
            examples/common/asset/Earth.c
            examples/common/asset/knob_circle_quater.c
            examples/common/asset/radial_line_cover.c
            examples/common/asset/radial_line_pattern.c
            examples/common/asset/wedge_pattern.c
            examples/common/asset/GlassBall40.c

            examples/common/asset/icon_automobile.c
            examples/common/asset/icon_compute.c 
            examples/common/asset/icon_Developer.c 
            examples/common/asset/icon_drone.c 
            examples/common/asset/icon_Durability.c 
            examples/common/asset/icon_enterprise.c 
            examples/common/asset/icon_healthcare.c 
            examples/common/asset/icon_mobile.c 
            examples/common/asset/icon_multicore.c 
            examples/common/asset/icon_smartmeter.c 
            examples/common/asset/icon_visual_sensor.c 
            examples/common/asset/icon_wearables.c
            

          )
    target_compile_options(ARM2D PUBLIC
          -DRTE_Acceleration_Arm_2D_Extra_Controls
          )
endif()

if (ARM2D_BENCHMARK_GENERIC)
	target_include_directories(ARM2D PUBLIC examples/common/benchmark/)
	target_sources(ARM2D PRIVATE examples/common/benchmark/benchmark_generic.c
            examples/common/benchmark/arm_2d_scene_benchmark_generic.c
			examples/common/benchmark/arm_2d_scene_benchmark_generic_cover.c
          )
    target_compile_options(ARM2D PUBLIC
          -DRTE_Acceleration_Arm_2D_Extra_Benchmark
          -DRTE_Acceleration_Arm_2D_Extra_Benchmark_Generic
          )
endif()

if (ARM2D_BENCHMARK_WATCHPANEL)
	target_include_directories(ARM2D PUBLIC examples/common/benchmark/)
	target_sources(ARM2D PRIVATE examples/common/benchmark/benchmark_watch_panel.c
            examples/common/benchmark/arm_2d_scene_benchmark_watch_panel.c
			examples/common/benchmark/arm_2d_scene_benchmark_watch_panel_cover.c
          )
    target_compile_options(ARM2D PUBLIC
          -DRTE_Acceleration_Arm_2D_Extra_Benchmark
          -DRTE_Acceleration_Arm_2D_Extra_Benchmark_Watch_Panel
          )
endif()



target_include_directories(ARM2D PUBLIC Library/Include/)
