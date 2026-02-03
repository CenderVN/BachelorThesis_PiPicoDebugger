# Use Ubuntu 24.04
FROM ubuntu:24.04

# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# 1. Install System Tools and Dependencies
RUN apt-get update && apt-get install -y \
    git build-essential python3 python3-pip python3-venv python3-serial python-is-python3 \
    usbutils clangd cmake ninja-build gdb-multiarch wget curl pkg-config \
    autoconf automake libtool texinfo gawk sudo libusb-1.0-0-dev libhidapi-dev \
    libftdi1-dev libjim-dev libudev-dev gcc-arm-none-eabi libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# 2. Setup User and Permissions (Fixed for USB access)
RUN useradd -m -s /bin/bash coder && \
    usermod -aG sudo,dialout,plugdev coder && \
    echo "coder ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# 3. Install code-server
RUN curl -fsSL https://code-server.dev/install.sh | sh

USER coder
WORKDIR /home/coder/project

# 4. Install Pico SDK (Extension-friendly path)
ENV PICO_SDK_PATH=/home/coder/.pico-sdk/sdk/2.2.0
RUN mkdir -p /home/coder/.pico-sdk/sdk && \
    git clone --recurse-submodules https://github.com/raspberrypi/pico-sdk.git ${PICO_SDK_PATH} \
    && cd ${PICO_SDK_PATH} && git checkout 2.2.0

# 5. Build OpenOCD (Raspberry Pi version)
USER root
RUN git clone https://github.com/raspberrypi/openocd.git --depth=1 /tmp/openocd \
    && cd /tmp/openocd && git submodule update --init --recursive \
    && ./bootstrap && ./configure --enable-ftdi --enable-cmsis-dap --enable-hidapi \
    && make -j$(nproc) && make install && rm -rf /tmp/openocd

# 6. Build Picotool (Stops GitHub 403 errors)
RUN git clone https://github.com/raspberrypi/picotool.git --depth=1 /tmp/picotool \
    && cd /tmp/picotool && mkdir build && cd build \
    && cmake .. && make -j$(nproc) && make install && rm -rf /tmp/picotool

# 7. Apply udev rules for the Pico Probe
USER root
# Create the directory first, then download the file
RUN mkdir -p /etc/udev/rules.d && \
    curl -o /etc/udev/rules.d/60-openocd.rules https://raw.githubusercontent.com/raspberrypi/pico-setup/master/60-openocd.rules

# Switch back to the non-root user for the rest of the build
USER coder

# 8. Install Extensions
RUN code-server --install-extension ms-python.python \
    && code-server --install-extension llvm-vs-code-extensions.vscode-clangd \
    && code-server --install-extension eclipse-cdt.serial-monitor \
    && code-server --install-extension marus25.cortex-debug \
    && code-server --install-extension raspberry-pi.raspberry-pi-pico

# 9. Pre-Configure VS Code Settings (User Settings)
RUN mkdir -p /home/coder/.local/share/code-server/User/ && \
    echo '{ \
    "raspberry-pi-pico.pythonExecutable": "/usr/bin/python3", \
    "raspberry-pi-pico.picoSdkPath": "/home/coder/.pico-sdk/sdk/2.2.0", \
    "raspberry-pi-pico.picotoolPath": "/usr/local/bin/picotool", \
    "raspberry-pi-pico.openocdPath": "/usr/local/bin/openocd", \
    "raspberry-pi-pico.useBuildinOpenocd": false, \
    "raspberry-pi-pico.serverType": "openocd", \
    "cmake.cmakePath": "/usr/bin/cmake", \
    "cmake.generator": "Ninja", \
    "python.defaultInterpreterPath": "/usr/bin/python3", \
    "cmake.configureArgs": ["-DCMAKE_EXPORT_COMPILE_COMMANDS=1"] \
    }' > /home/coder/.local/share/code-server/User/settings.json

# 10. Create a Template launch.json for the project
# This ensures every time they open the container, they can just copy this file
RUN mkdir -p /home/coder/templates && \
    echo '{ \
    "version": "0.2.0", \
    "configurations": [ \
        { \
            "name": "Pico Debug (External)", \
            "type": "cortex-debug", \
            "cwd": "${workspaceRoot}", \
            "executable": "${command:raspberry-pi-pico.launchTargetPath}", \
            "request": "launch", \
            "servertype": "external", \
            "gdbTarget": "localhost:3333", \
            "gdbPath": "gdb-multiarch", \
            "device": "RP2040", \
            "svdFile": "/home/coder/.pico-sdk/sdk/2.2.0/src/rp2040/hardware_regs/RP2040.svd", \
            "runToEntryPoint": "main", \
            "overrideLaunchCommands": [ \
                "set mem inaccessible-by-default off", \
                "monitor reset init", \
                "load" \
            ] \
        } \
    ] \
}' > /home/coder/templates/launch.json

# 11. Add helper script to start OpenOCD easily
RUN echo 'sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 1000"' > /home/coder/start_debug.sh && \
    chmod +x /home/coder/start_debug.sh

EXPOSE 8080
RUN echo "export PICO_SDK_PATH=/home/coder/.pico-sdk/sdk/2.2.0" >> /home/coder/.bashrc
CMD ["code-server", "--bind-addr", "0.0.0.0:8080", "--auth", "none", "."]