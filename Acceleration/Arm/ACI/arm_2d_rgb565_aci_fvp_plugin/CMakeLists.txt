#
# Copyright 2023 Arm Limited. All rights reserved
#

add_plugin(NAME arm_2d_rgb565_aci_fvp
    SOURCES
        arm_2d_rgb565_aci_fvp.cpp
        ACILibrary.cpp
    INCLUDE_DIRECTORIES
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    LINK_LIBRARIES
        PRIVATE public_headers
    )
