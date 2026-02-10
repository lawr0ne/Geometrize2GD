cmake_minimum_required(VERSION 3.21)
project(stb)

add_library(stb INTERFACE)
add_library(Stb::Stb ALIAS stb)
target_include_directories(stb
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(stb PROPERTIES
    EXPORT_NAME stb
)
