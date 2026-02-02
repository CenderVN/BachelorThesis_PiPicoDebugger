# Use Ubuntu 22.04 as the base
FROM ubuntu:22.04

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# 1. Install Host System components & Build Tools
# Includes: Git, build-essential, Python 3, LibUSB, LibHIDAPI, CMake, Ninja, GDB Multiarch
RUN apt-get update && apt-get install -y \
    git \
    build-essential \
    python3 \
    python3-pip \
    libusb-1.0-0-dev \
    libhidapi-dev \
    cmake \
    ninja-build \
    gdb-multiarch \
    wget \
    curl \
    pkg-config \
    autoconf \
    automake \
    libtool \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# 2. Setup Working Directory
WORKDIR /home/dev
ENV PICO_SDK_PATH=/home/dev/pico-sdk

# 3. Install Pico SDK (Target Framework)
# Matches the requirement for the core library and USB stacks
RUN git clone --recurse-submodules https://github.com/raspberrypi/pico-sdk.git ${PICO_SDK_PATH} \
    && cd ${PICO_SDK_PATH} \
    && git checkout 2.2.0 || echo "Version 2.2.0 not found, using latest stable"

# 4. Install OpenOCD (Raspberry Pi Fork)
# Standard OpenOCD often lacks RP2040 support; we build the Pi-specific version
RUN git clone https://github.com/raspberrypi/openocd.git --depth=1 --recursive \
    && cd openocd \
    && ./bootstrap \
    && ./configure --enable-ftdi --enable-sysfsgpio --enable-bcm2835gcc \
    && make -j$(nproc) \
    && make install \
    && cd .. && rm -rf openocd

# 5. Setup udev Rules (Host Config)
# Note: These are placed in the image, but for hardware access, 
# you must run the container with --privileged or --device flags.
RUN curl -o /etc/udev/rules.d/60-openocd.rules https://raw.githubusercontent.com/raspberrypi/pico-setup/master/60-openocd.rules

# 6. Set environment variables for the Toolchain
ENV ARM_TOOLCHAIN_PATH=/usr/bin
ENV PICO_SDK_FETCH_FROM_GIT=1

# Default command
CMD ["/bin/bash"]
