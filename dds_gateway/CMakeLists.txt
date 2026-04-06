cmake_minimum_required(VERSION 3.0)
project(dds_gateway CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fastrtps REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(dds_gateway
    src/main.cpp
    src/manager.cpp
    src/config.cpp
    src/dds_input_adapter.cpp
    src/processor.cpp
    src/router.cpp
    src/tcp_output_adapter.cpp
    src/mqtt_output_adapter.cpp
)

target_link_libraries(dds_gateway fastrtps pthread)
