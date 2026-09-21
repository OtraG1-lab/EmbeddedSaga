cmake_minimum_required(VERSION 3.16)

project(VehicleCommonAPI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------
# Paths
# --------------------------------------------------

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(GENERATED_DIR
    ${PROJECT_ROOT}/src-gen
)

# CommonAPI ARM64 runtime inside Docker image
set(COMMONAPI_ROOT
    /opt/commonapi-arm64
)

set(COMMONAPI_INCLUDE_DIR
    ${COMMONAPI_ROOT}/include/CommonAPI-3.2
)

set(COMMONAPI_LIB_DIR
    ${COMMONAPI_ROOT}/lib
)

# D-Bus headers inside the ARM64 sysroot
set(DBUS_INCLUDE_DIR
    /build/sysroot/usr/include/dbus-1.0
)

set(DBUS_ARCH_INCLUDE_DIR
    /build/sysroot/usr/lib/aarch64-linux-gnu/dbus-1.0/include
)

# --------------------------------------------------
# Include directories
# --------------------------------------------------

include_directories(
    ${GENERATED_DIR}
    ${COMMONAPI_INCLUDE_DIR}
    ${PROJECT_ROOT}/server

    ${DBUS_INCLUDE_DIR}
    ${DBUS_ARCH_INCLUDE_DIR}
)

# --------------------------------------------------
# Vehicle Server
# --------------------------------------------------

add_executable(vehicle-server

    server/main.cpp
    server/VehicleService.cpp

    src-gen/v1/com/example/vehicle/VehicleDBusDeployment.cpp
    src-gen/v1/com/example/vehicle/VehicleDBusStubAdapter.cpp
)

target_link_libraries(vehicle-server

    ${COMMONAPI_LIB_DIR}/libCommonAPI.so
    ${COMMONAPI_LIB_DIR}/libCommonAPI-DBus.so

    pthread
)

# --------------------------------------------------
# Vehicle Client
# --------------------------------------------------

add_executable(vehicle-client

    client/main.cpp

    src-gen/v1/com/example/vehicle/VehicleDBusDeployment.cpp
    src-gen/v1/com/example/vehicle/VehicleDBusProxy.cpp
)

target_link_libraries(vehicle-client

    ${COMMONAPI_LIB_DIR}/libCommonAPI.so
    ${COMMONAPI_LIB_DIR}/libCommonAPI-DBus.so

    pthread
)