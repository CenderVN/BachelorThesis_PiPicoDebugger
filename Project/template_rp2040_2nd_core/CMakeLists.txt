cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")
if(DEFINED ENV{FREERTOS_KERNEL_PATH})
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH} CACHE PATH "Path to FreeRTOS" FORCE)
else()
    set(FREERTOS_KERNEL_PATH ${USERHOME}/FreeRTOS-Kernel CACHE PATH "Path to FreeRTOS" FORCE)
endif()

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(demo_project C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()
add_subdirectory(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040 FREERTOS_KERNEL)
# Add executable. Default name is the project name, version 0.1
add_executable(demo_project demo_project.c )

pico_set_program_name(demo_project "demo_project")
pico_set_program_version(demo_project "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(demo_project 0)
pico_enable_stdio_usb(demo_project 0)

# Add the standard library to the build
target_link_libraries(demo_project
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(demo_project PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(demo_project 
        pico_stdlib
        pico_multicore
        FreeRTOS-Kernel-Heap4
        pico_async_context_freertos
        )

pico_add_extra_outputs(demo_project)

