cmake_minimum_required(VERSION 3.20)
project(VehicleMultiBindingWorkspace CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Discover D-Bus libraries through package config paths
find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src-gen
    /usr/local/include
    /usr/local/include/CommonAPI-3.2
    ${DBUS_INCLUDE_DIRS}
)

link_directories(/usr/local/lib ${DBUS_LIBRARY_DIRS})

# ------------------------------------------------------------------------------
# Target Component 1: D-Bus Bound Executable
# ------------------------------------------------------------------------------
add_executable(DbusServerApp
    dbus-service/DbusServer.cpp
    src-gen/v1/commonapi/dbus/DbusServiceDBusDeployment.cpp
    src-gen/v1/commonapi/dbus/DbusServiceDBusStubAdapter.cpp
)
target_link_libraries(DbusServerApp CommonAPI CommonAPI-DBus ${DBUS_LIBRARIES} pthread)
target_include_directories(DbusServerApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src-gen
    /usr/local/include
    /usr/local/include/CommonAPI-3.2
    ${DBUS_INCLUDE_DIRS}
)

# ------------------------------------------------------------------------------
# Target Component 2: SOME/IP Bound Executable
# ------------------------------------------------------------------------------
add_executable(SomeipServerApp
    someip-service/SomeipServer.cpp
    src-gen/v1/commonapi/someip/SomeipServiceSomeIPDeployment.cpp
    src-gen/v1/commonapi/someip/SomeipServiceSomeIPStubAdapter.cpp
)
target_link_libraries(SomeipServerApp CommonAPI CommonAPI-SomeIP vsomeip3 pthread)
target_include_directories(SomeipServerApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src-gen
    /usr/local/include
    /usr/local/include/CommonAPI-3.2
    ${DBUS_INCLUDE_DIRS}
)

# ------------------------------------------------------------------------------
# Target Component 3: Vehicle DBus server and client
# ------------------------------------------------------------------------------
file(GLOB VEHICLE_DBUS_GENERATED
    ${CMAKE_CURRENT_SOURCE_DIR}/src-gen/v1/com/example/vehicle/*Vehicle*cpp
)

add_executable(VehicleServerApp
    dbus-service/VehicleServer.cpp
    ${VEHICLE_DBUS_GENERATED}
)
target_link_libraries(VehicleServerApp CommonAPI CommonAPI-DBus ${DBUS_LIBRARIES} pthread)
target_include_directories(VehicleServerApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src-gen
    /usr/local/include
    /usr/local/include/CommonAPI-3.2
    ${DBUS_INCLUDE_DIRS}
)

add_executable(VehicleClientApp
    dbus-service/VehicleClient.cpp
    ${VEHICLE_DBUS_GENERATED}
)
target_link_libraries(VehicleClientApp CommonAPI CommonAPI-DBus ${DBUS_LIBRARIES} pthread)
target_include_directories(VehicleClientApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src-gen
    /usr/local/include
    /usr/local/include/CommonAPI-3.2
    ${DBUS_INCLUDE_DIRS}
)

# ------------------------------------------------------------------------------
# Target Component 4: Media player DBus server and client
# ------------------------------------------------------------------------------
file(GLOB MEDIA_DBUS_GENERATED
    ${CMAKE_CURRENT_SOURCE_DIR}/src-gen/v1/org/example/*MediaPlayer*cpp
)

add_executable(MediaServerApp
    dbus-service/MediaServer.cpp
    ${MEDIA_DBUS_GENERATED}
)
target_link_libraries(MediaServerApp CommonAPI CommonAPI-DBus ${DBUS_LIBRARIES} ${GSTREAMER_LIBRARIES} pthread)
target_include_directories(MediaServerApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src-gen
    /usr/local/include
    /usr/local/include/CommonAPI-3.2
    ${DBUS_INCLUDE_DIRS}
    ${GSTREAMER_INCLUDE_DIRS}
)
target_compile_options(MediaServerApp PRIVATE ${GSTREAMER_CFLAGS_OTHER})
target_link_directories(MediaServerApp PRIVATE ${GSTREAMER_LIBRARY_DIRS})

add_executable(MediaClientApp
    dbus-service/MediaClient.cpp
    ${MEDIA_DBUS_GENERATED}
)
target_link_libraries(MediaClientApp CommonAPI CommonAPI-DBus ${DBUS_LIBRARIES} pthread)
target_include_directories(MediaClientApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src-gen
    /usr/local/include
    /usr/local/include/CommonAPI-3.2
    ${DBUS_INCLUDE_DIRS}
)
