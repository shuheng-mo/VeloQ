add_library(speedquant_core STATIC
    src/market_data/market_data_feed.cpp
    src/order/order_manager.cpp
    src/risk/risk_controller.cpp
    src/utils/logger.cpp
    src/utils/config_manager.cpp
)

target_include_directories(speedquant_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(speedquant_core
    ${Boost_LIBRARIES}
    ${ZeroMQ_LIBRARIES}
)

# Install
install(TARGETS speedquant_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include/speedquant
    FILES_MATCHING PATTERN "*.h"
)
