pybind11_add_module(speedquant_core core_bindings.cpp)

target_link_libraries(speedquant_core PRIVATE
    speedquant_core
    ${Boost_LIBRARIES}
    ${ZeroMQ_LIBRARIES}
)

install(TARGETS speedquant_core
    LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}
)
