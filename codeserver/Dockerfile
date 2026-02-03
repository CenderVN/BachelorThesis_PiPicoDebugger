# Use Ubuntu 24.04 as the base
FROM ubuntu:24.04

# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# 1. Install ALL necessary dependencies
# Added libjim-dev as a system fallback
RUN apt-get update && apt-get install -y \
    git \
    build-essential \
    python3 \
    python3-pip \
    libusb-1.0-0-dev \
    libhidapi-dev \
    libftdi1-dev \
    libjim-dev \
    cmake \
    ninja-build \
    gdb-multiarch \
    wget \
    curl \
    pkg-config \
    autoconf \
    automake \
    libtool \
    texinfo \
    gawk \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    sudo \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# 2. Install code-server
RUN curl -fsSL https://code-server.dev/install.sh | sh

# 3. Setup Working Directory & User
RUN useradd -m -s /bin/bash coder && \
    echo "coder ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
USER coder
WORKDIR /home/coder/project

# 4. Install Pico SDK
ENV PICO_SDK_PATH=/home/coder/pico-sdk
RUN git clone --recurse-submodules https://github.com/raspberrypi/pico-sdk.git ${PICO_SDK_PATH} \
    && cd ${PICO_SDK_PATH} \
    && git checkout 2.2.0

# 5. Build OpenOCD (Raspberry Pi version)
USER root
# We clone, then manually init submodules to ensure jimtcl is present
RUN git clone https://github.com/raspberrypi/openocd.git --depth=1 /tmp/openocd \
    && cd /tmp/openocd \
    && git submodule update --init --recursive \
    && ./bootstrap \
    && ./configure --enable-ftdi --enable-cmsis-dap --enable-hidapi \
    && make -j$(nproc) \
    && make install \
    && rm -rf /tmp/openocd
USER coder

# 6. Pre-install the Raspberry Pi Pico VS Code Extension
RUN code-server --install-extension raspberry-pi.raspberry-pi-pico

# 7. Expose the port for code-server
EXPOSE 8080

# 8. Set up convenience environment variables
RUN echo "export PICO_SDK_PATH=/home/coder/pico-sdk" >> /home/coder/.bashrc

# 9. Start code-server
CMD ["code-server", "--bind-addr", "0.0.0.0:8080", "--auth", "none", "."]