cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == SDK & TOOLCHAIN CONFIGURATION ==
if(DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()

# Fallback for the VS Code extension helper script
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ======================================

set(PICO_BOARD pico2 CACHE STRING "Board type")

# 1. USE THE GLOBAL ENV PATH FOR FREERTOS
# We check if the environment variable is set; if not, we fallback to USERHOME
if(DEFINED ENV{FREERTOS_KERNEL_PATH})
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH} CACHE PATH "Path to FreeRTOS" FORCE)
else()
    set(FREERTOS_KERNEL_PATH ${USERHOME}/FreeRTOS-Kernel CACHE PATH "Path to FreeRTOS" FORCE)
endif()

# 2. IMPORT THE SDK
include(pico_sdk_import.cmake)
project(blink C CXX ASM)
pico_sdk_init()

# 3. ADD THE KERNEL SUBDIRECTORY
# Note: Using the variable we just set above
add_subdirectory(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040 FREERTOS_KERNEL)

add_executable(blink blink.c)

# 4. DIRECTORIES & LIBRARIES
target_include_directories(blink PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(blink 
    pico_stdlib
    pico_multicore
    FreeRTOS-Kernel-Heap4
    pico_async_context_freertos
)

# 5. STDIO CONFIGURATION
pico_enable_stdio_uart(blink 1)
pico_enable_stdio_usb(blink 0)

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(blink pico_cyw43_arch_none)
endif()

pico_add_extra_outputs(blink)