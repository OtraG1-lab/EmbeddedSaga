# syntax=docker/dockerfile:1

FROM --platform=linux/amd64 commonapi-generator-base:latest AS source-generator
WORKDIR /workspace
COPY . .

RUN mkdir -p src-gen && \
    CORE_GENERATOR="$(find /opt/covesa/core-gen -type f -o -type l | grep -E '/commonapi-core-generator-linux-x86_64$' | head -n 1)" && \
    DBUS_GENERATOR="$(find /opt/covesa/dbus-gen -type f -o -type l | grep -E '/commonapi-dbus-generator-linux-x86_64$' | head -n 1)" && \
    SOMEIP_GENERATOR="$(find /opt/covesa/someip-gen -type f -o -type l | grep -E '/commonapi-someip-generator-linux-x86_64$' | head -n 1)" && \
    test -n "$CORE_GENERATOR" && test -n "$DBUS_GENERATOR" && test -n "$SOMEIP_GENERATOR" && \
    chmod +x "$CORE_GENERATOR" "$DBUS_GENERATOR" "$SOMEIP_GENERATOR" && \
    "$CORE_GENERATOR" -sk -d ./src-gen ./dbus-service/dbus_interface.fidl && \
    "$DBUS_GENERATOR" -d ./src-gen ./dbus-service/dbus_interface.fdepl && \
    "$CORE_GENERATOR" -sk -d ./src-gen ./dbus-service/vehiclefunctions.fidl && \
    "$DBUS_GENERATOR" -d ./src-gen ./dbus-service/vehiclefunctions.fdepl && \
    "$CORE_GENERATOR" -sk -d ./src-gen ./dbus-service/media.fidl && \
    "$DBUS_GENERATOR" -d ./src-gen ./dbus-service/media.fdepl && \
    "$CORE_GENERATOR" -sk -d ./src-gen ./someip-service/someip_interface.fidl && \
    "$SOMEIP_GENERATOR" -d ./src-gen ./someip-service/someip_interface.fdepl

FROM --platform=linux/arm64 commonapi-runtime-base:latest AS application-compiler
WORKDIR /workspace
ENV DEBIAN_FRONTEND=noninteractive
ARG DLT_DAEMON_REF=v2.18.10
ENV DLT_PREFIX=/opt/dlt
ENV PKG_CONFIG_PATH=/opt/dlt/lib/pkgconfig:/usr/local/lib/pkgconfig
ENV LD_LIBRARY_PATH=/opt/dlt/lib:/usr/local/lib:/usr/lib/aarch64-linux-gnu:/lib/aarch64-linux-gnu
ENV QTWEBENGINE_CHROMIUM_FLAGS="--no-sandbox --disable-gpu-sandbox --ignore-gpu-blocklist --use-gl=swiftshader"
ENV QTWEBENGINE_DISABLE_SANDBOX=1
COPY . .
COPY --from=source-generator /workspace/src-gen ./src-gen

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --parallel 2 && \
    cmake -S IVI-main -B ivi-build -G Ninja -DCMAKE_BUILD_TYPE=Release && \
    cmake --build ivi-build --parallel 2

FROM --platform=linux/arm64 commonapi-runtime-base:latest AS runtime-delivery
ENV DEBIAN_FRONTEND=noninteractive
ARG DLT_DAEMON_REF=v2.18.10
ENV DLT_PREFIX=/opt/dlt
ENV PKG_CONFIG_PATH=/opt/dlt/lib/pkgconfig:/usr/local/lib/pkgconfig
ENV LD_LIBRARY_PATH=/opt/dlt/lib:/usr/local/lib:/usr/lib/aarch64-linux-gnu:/lib/aarch64-linux-gnu
ENV QTWEBENGINE_CHROMIUM_FLAGS="--no-sandbox --disable-gpu-sandbox --ignore-gpu-blocklist --use-gl=swiftshader"
ENV QTWEBENGINE_DISABLE_SANDBOX=1

COPY --from=application-compiler /workspace/build/DbusServerApp /usr/local/bin/DbusServerApp
COPY --from=application-compiler /workspace/build/SomeipServerApp /usr/local/bin/SomeipServerApp
COPY --from=application-compiler /workspace/build/VehicleServerApp /usr/local/bin/VehicleServerApp
COPY --from=application-compiler /workspace/build/VehicleClientApp /usr/local/bin/VehicleClientApp.bin
COPY --from=application-compiler /workspace/build/MediaServerApp /usr/local/bin/MediaServerApp
COPY --from=application-compiler /workspace/build/MediaClientApp /usr/local/bin/MediaClientApp.bin
COPY --from=application-compiler /workspace/ivi-build/headunit_ivi /usr/local/bin/headunit_ivi.bin
COPY commonapi.ini /etc/commonapi.ini
COPY vsomeip.json /etc/vsomeip.json
COPY entrypoint.sh /usr/local/bin/entrypoint.sh
COPY headunit-entrypoint.sh /usr/local/bin/headunit_ivi
COPY vehicle-client-entrypoint.sh /usr/local/bin/VehicleClientApp
COPY media-client-entrypoint.sh /usr/local/bin/MediaClientApp

RUN chmod +x /usr/local/bin/entrypoint.sh /usr/local/bin/headunit_ivi \
    /usr/local/bin/VehicleClientApp /usr/local/bin/MediaClientApp

EXPOSE 8080 3490
VOLUME ["/media"]

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["sh", "-c", "set -eu; export DISPLAY=:99; export XDG_RUNTIME_DIR=/tmp/runtime-root; mkdir -p \"$XDG_RUNTIME_DIR\"; chmod 700 \"$XDG_RUNTIME_DIR\"; Xvfb \"$DISPLAY\" -screen 0 1024x600x24 -ac >/tmp/xvfb.log 2>&1 & until xdpyinfo -display \"$DISPLAY\" >/dev/null 2>&1; do sleep 0.1; done; x11vnc -display \"$DISPLAY\" -rfbport 5900 -nopw -shared -forever -bg -o /tmp/x11vnc.log; /usr/local/bin/DbusServerApp >/tmp/dbus-server.log 2>&1 & /usr/local/bin/SomeipServerApp >/tmp/someip-server.log 2>&1 & /usr/local/bin/VehicleServerApp >/tmp/vehicle-server.log 2>&1 & /usr/local/bin/MediaServerApp >/tmp/media-server.log 2>&1 & websockify --web /usr/share/novnc/ 8080 127.0.0.1:5900 >/tmp/websockify.log 2>&1 & exec /usr/local/bin/headunit_ivi"]

