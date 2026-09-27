# Step 1: Use an ARM64 Debian image matching Raspberry Pi OS (Bookworm)
FROM --platform=linux/arm64 debian:bookworm-slim

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Qt WebEngine runs as root in the container, so disable Chromium's sandbox.
ENV QTWEBENGINE_CHROMIUM_FLAGS="--no-sandbox --disable-gpu-sandbox --ignore-gpu-blocklist --use-gl=swiftshader"
ENV QTWEBENGINE_DISABLE_SANDBOX=1

# Step 2: Install build tools, Qt6, virtual display, and noVNC web tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-webengine-dev \
    libgl1-mesa-dri \
    xvfb \
    x11-utils \
    x11vnc \
    novnc \
    websockify \
    wpasupplicant \
    && rm -rf /var/lib/apt/lists/*

# Qt SVG and XKB development dependencies
RUN apt-get update && apt-get install -y \
    ninja-build \
    libqt6svg6-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    libgl1-mesa-dev \
    libfontconfig1-dev \
    libfreetype6-dev \
    libasound2-dev \
    libpulse-dev \
    && rm -rf /var/lib/apt/lists/*

# Qt Quick Controls runtime QML modules
RUN apt-get update && apt-get install -y \
    qml6-module-qtqml \
    qml6-module-qtquick-controls \
    qml6-module-qtquick-templates \
    qml6-module-qtquick-layouts \
    qml6-module-qtquick-window \
    qml6-module-qtqml-workerscript \
    qt6-qmltooling-plugins \
    && rm -rf /var/lib/apt/lists/*

# Qt WebEngine runtime QML module
RUN apt-get update && apt-get install -y \
    qml6-module-qtwebengine \
    && rm -rf /var/lib/apt/lists/*

# Set working directory inside the container
WORKDIR /app

# Copy your source code into the container
COPY IVI-main/ .

# Step 3: Configure the Qt application under ARM64 emulation
RUN cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Step 4: Compile the Qt application
RUN  cmake --build build --parallel 2

# Expose port 8080 for the web browser connection
EXPOSE 8080

# Step 5: Windows-safe execution block
# 1. Starts virtual display (:99)
# 2. Starts VNC server targeting display :99
# 3. Starts noVNC proxy to convert VNC traffic to web traffic on port 8080
# 4. Launches the compiled Qt Application
CMD ["sh", "-c", "mkdir -p /tmp/runtime-root; chmod 700 /tmp/runtime-root; export XDG_RUNTIME_DIR=/tmp/runtime-root; Xvfb :99 -screen 0 1024x600x24 & export DISPLAY=:99; until xdpyinfo -display \"$DISPLAY\" >/dev/null 2>&1; do sleep 0.1; done; xdpyinfo -display \"$DISPLAY\" | grep dimensions; x11vnc -display :99 -nopw -forever -bg; websockify --web /usr/share/novnc/ 8080 localhost:5900 & exec ./build/headunit_ivi"]
