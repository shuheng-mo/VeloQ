# Common module - Shared utilities and data structures

# Source files
file(GLOB_RECURSE COMMON_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Create common library
add_library(veloq_common STATIC ${COMMON_SOURCES})

target_include_directories(veloq_common
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(veloq_common
    PUBLIC
        Threads::Threads
        Boost::system
        Boost::thread
)

# Tests
if(BUILD_TESTS)
    file(GLOB_RECURSE COMMON_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    if(COMMON_TEST_SOURCES)
        add_executable(veloq_common_tests ${COMMON_TEST_SOURCES})
        target_link_libraries(veloq_common_tests
            PRIVATE
                veloq_common
        )
        add_test(NAME common_tests COMMAND veloq_common_tests)
    endif()
endif()
