cmake_minimum_required(VERSION 3.16)
project(dlt_raspberry_pi_demo LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DLT REQUIRED automotive-dlt)

add_executable(dlt_sample_app src/dlt_sample_app.c)
target_include_directories(dlt_sample_app PRIVATE ${DLT_INCLUDE_DIRS})
target_link_directories(dlt_sample_app PRIVATE ${DLT_LIBRARY_DIRS})
target_link_libraries(dlt_sample_app PRIVATE ${DLT_LIBRARIES})
target_compile_options(dlt_sample_app PRIVATE ${DLT_CFLAGS_OTHER})
