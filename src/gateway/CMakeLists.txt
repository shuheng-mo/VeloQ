# Gateway module - CTP Market Data Gateway

# Source files
file(GLOB_RECURSE GATEWAY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Create gateway library
add_library(veloq_gateway STATIC ${GATEWAY_SOURCES})

target_include_directories(veloq_gateway
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${THIRD_PARTY_DIR}/ctp/include
)

target_link_libraries(veloq_gateway
    PUBLIC
        veloq_common
    PRIVATE
        # CTP library will be linked here
        # ${THIRD_PARTY_DIR}/ctp/lib/thostmduserapi_se.so
)

# Tests
if(BUILD_TESTS)
    file(GLOB_RECURSE GATEWAY_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    if(GATEWAY_TEST_SOURCES)
        add_executable(veloq_gateway_tests ${GATEWAY_TEST_SOURCES})
        target_link_libraries(veloq_gateway_tests
            PRIVATE
                veloq_gateway
        )
        add_test(NAME gateway_tests COMMAND veloq_gateway_tests)
    endif()
endif()
