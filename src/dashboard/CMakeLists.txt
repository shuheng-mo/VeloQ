# Dashboard module - Real-time visualization with Dear ImGui

# Source files
file(GLOB_RECURSE DASHBOARD_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Dear ImGui sources (to be placed in third_party/imgui)
set(IMGUI_DIR ${THIRD_PARTY_DIR}/imgui)
# Uncomment when Dear ImGui is available:
# file(GLOB IMGUI_SOURCES
#     "${IMGUI_DIR}/*.cpp"
#     "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
#     "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
# )

# Create dashboard executable
add_executable(veloq_dashboard ${DASHBOARD_SOURCES})

target_include_directories(veloq_dashboard
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

target_link_libraries(veloq_dashboard
    PRIVATE
        veloq_common
        veloq_gateway
        veloq_feature_engine
        veloq_inference
        veloq_ipc_bridge
        # OpenGL, GLFW libraries will be linked here when available
)

# Install
install(TARGETS veloq_dashboard
    RUNTIME DESTINATION bin
)
