cmake_minimum_required(VERSION 3.13)

# 1. == VS Code Extension Integration (DO NOT EDIT) ==
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
# =====================================================

# 2. Capture Platform from Status Bar (Defaults to rp2040)
set(PICO_PLATFORM $ENV{PICO_PLATFORM} CACHE STRING "Platform type (rp2040 or rp2350)")
if (NOT PICO_PLATFORM)
    set(PICO_PLATFORM "rp2040")
endif()

# 3. Dynamic Board Selection
if (PICO_PLATFORM STREQUAL "rp2350")
    set(PICO_BOARD pico2 CACHE STRING "" FORCE)
else()
    set(PICO_BOARD pico_w CACHE STRING "" FORCE)
endif()

# 4. Debug Mode Toggle (From Status Bar Button)
# ON = Core 0 only (pico-debug) | OFF = Core 0 & 1 (SMP)
option(PICO_DEBUG_MODE "Enable pico-debug compatibility (Single Core 0)" ON)

# Standard C/C++ settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pull in Raspberry Pi Pico SDK
include(pico_sdk_import.cmake)

project(demo_project C CXX ASM)
pico_sdk_init()

# 5. FreeRTOS Path Setup
if(DEFINED ENV{FREERTOS_KERNEL_PATH})
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH} CACHE PATH "Path to FreeRTOS" FORCE)
else()
    set(FREERTOS_KERNEL_PATH ${USERHOME}/FreeRTOS-Kernel CACHE PATH "Path to FreeRTOS" FORCE)
endif()

# Add the FreeRTOS subdirectory
add_subdirectory(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040 FREERTOS_KERNEL)

# 6. Core Count Abstraction
# This "injects" the core count into FreeRTOS and your C code
if(PICO_DEBUG_MODE)
    add_compile_definitions(configNUMBER_OF_CORES=1)
    message(STATUS "-------------------------------------------------------")
    message(STATUS "MODE: pico-debug (Core 0 Only) - Target: ${PICO_PLATFORM}")
    message(STATUS "-------------------------------------------------------")
else()
    add_compile_definitions(configNUMBER_OF_CORES=2)
    message(STATUS "-------------------------------------------------------")
    message(STATUS "MODE: Multicore SMP (Dual Core) - Target: ${PICO_PLATFORM}")
    message(STATUS "-------------------------------------------------------")
endif()

# 7. Add Executable
add_executable(demo_project demo_project.c)

# Binary Metadata
pico_set_program_name(demo_project "demo_project")
pico_set_program_version(demo_project "0.1")

# STDIO Configuration (UART and USB enabled)
pico_enable_stdio_uart(demo_project 1)
pico_enable_stdio_usb(demo_project 1)

# Include directories
target_include_directories(demo_project PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# 8. Unified Library Linking
# We link pico_multicore always; it just won't be used if cores=1
target_link_libraries(demo_project 
    pico_stdlib
    pico_multicore
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    pico_async_context_freertos
)

pico_add_extra_outputs(demo_project)