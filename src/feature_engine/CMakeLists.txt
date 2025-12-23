# Feature Engine module - Real-time feature computation

# Source files
file(GLOB_RECURSE FEATURE_ENGINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Create feature engine library
add_library(veloq_feature_engine STATIC ${FEATURE_ENGINE_SOURCES})

target_include_directories(veloq_feature_engine
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(veloq_feature_engine
    PUBLIC
        veloq_common
)

# Enable SIMD optimizations for performance-critical code
if(NOT MSVC)
    target_compile_options(veloq_feature_engine PRIVATE -msse4.2 -mavx2)
endif()

# Tests
if(BUILD_TESTS)
    file(GLOB_RECURSE FEATURE_ENGINE_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    if(FEATURE_ENGINE_TEST_SOURCES)
        add_executable(veloq_feature_engine_tests ${FEATURE_ENGINE_TEST_SOURCES})
        target_link_libraries(veloq_feature_engine_tests
            PRIVATE
                veloq_feature_engine
        )
        add_test(NAME feature_engine_tests COMMAND veloq_feature_engine_tests)
    endif()
endif()
