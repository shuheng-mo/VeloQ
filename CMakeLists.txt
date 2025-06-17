cmake_minimum_required(VERSION 3.14)
project(SpeedQuant VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_GUI "Build GUI components" ON)

# Find dependencies
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(ZeroMQ REQUIRED)
find_package(pybind11 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
    ${Boost_INCLUDE_DIRS}
    ${ZeroMQ_INCLUDE_DIRS}
    ${PYBIND11_INCLUDE_DIR}
)

# Core library
add_subdirectory(core)

# GUI (if enabled)
if(BUILD_GUI)
    find_package(Qt6 COMPONENTS Core Widgets Charts REQUIRED)
    add_subdirectory(gui)
endif()

# Tests (if enabled)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(core/tests)
endif()

# Python bindings
add_subdirectory(speedquant_lib/bridge)
