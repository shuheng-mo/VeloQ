# IPC Bridge module - Inter-process communication with Python

# Source files
file(GLOB_RECURSE IPC_BRIDGE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Create IPC bridge library
add_library(veloq_ipc_bridge STATIC ${IPC_BRIDGE_SOURCES})

target_include_directories(veloq_ipc_bridge
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(veloq_ipc_bridge
    PUBLIC
        veloq_common
    PRIVATE
        Boost::system
)

# Tests
if(BUILD_TESTS)
    file(GLOB_RECURSE IPC_BRIDGE_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    if(IPC_BRIDGE_TEST_SOURCES)
        add_executable(veloq_ipc_bridge_tests ${IPC_BRIDGE_TEST_SOURCES})
        target_link_libraries(veloq_ipc_bridge_tests
            PRIVATE
                veloq_ipc_bridge
        )
        add_test(NAME ipc_bridge_tests COMMAND veloq_ipc_bridge_tests)
    endif()
endif()
