# Inference module - AI real-time inference with ONNX Runtime

# Source files
file(GLOB_RECURSE INFERENCE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Create inference library
add_library(veloq_inference STATIC ${INFERENCE_SOURCES})

target_include_directories(veloq_inference
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${THIRD_PARTY_DIR}/onnxruntime/include
)

target_link_libraries(veloq_inference
    PUBLIC
        veloq_common
        veloq_feature_engine
    PRIVATE
        # ONNX Runtime library will be linked here
        # ${THIRD_PARTY_DIR}/onnxruntime/lib/libonnxruntime.so
)

# Tests
if(BUILD_TESTS)
    file(GLOB_RECURSE INFERENCE_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    if(INFERENCE_TEST_SOURCES)
        add_executable(veloq_inference_tests ${INFERENCE_TEST_SOURCES})
        target_link_libraries(veloq_inference_tests
            PRIVATE
                veloq_inference
        )
        add_test(NAME inference_tests COMMAND veloq_inference_tests)
    endif()
endif()
