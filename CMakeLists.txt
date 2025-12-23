cmake_minimum_required(VERSION 3.20)

# Project configuration
project(veloq
    VERSION 1.0.0
    DESCRIPTION "VeloQ - High-Performance Quantitative Trading Market Data Accelerator"
    LANGUAGES CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type default to Release with debug info
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build." FORCE)
endif()

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    # Performance optimization flags
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_compile_options(-O3 -march=native -DNDEBUG)
    endif()
endif()

# Global include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Options
option(BUILD_TESTS "Build test programs" ON)
option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_DASHBOARD "Build GUI dashboard (requires Dear ImGui)" ON)
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)

# Sanitizers (for development)
if(ENABLE_SANITIZERS AND NOT MSVC)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# Find dependencies
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)

# Third-party libraries directory
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third_party)

# Include third-party libraries
# Note: Users need to manually place CTP API, Dear ImGui, ONNX Runtime, spdlog in third_party/
# See docs/build/dependencies.md for details

# Add subdirectories
add_subdirectory(src/common)
add_subdirectory(src/gateway)
add_subdirectory(src/feature_engine)
add_subdirectory(src/inference)
add_subdirectory(src/ipc_bridge)

if(BUILD_DASHBOARD)
    add_subdirectory(src/dashboard)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Testing
if(BUILD_TESTS)
    enable_testing()
    # Tests are built within each module's CMakeLists.txt
endif()

# Installation
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/veloq
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Print configuration summary
message(STATUS "")
message(STATUS "VeloQ Configuration Summary:")
message(STATUS "  Version:           ${PROJECT_VERSION}")
message(STATUS "  Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard:      ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build tests:       ${BUILD_TESTS}")
message(STATUS "  Build examples:    ${BUILD_EXAMPLES}")
message(STATUS "  Build dashboard:   ${BUILD_DASHBOARD}")
message(STATUS "  Enable sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "")
