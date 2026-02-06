cmake_minimum_required(VERSION 4.0.0)
project(Geometrize)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/geometrize-lib/geometrize/geometrize/*.cpp)

add_library(geometrize STATIC ${SOURCES})
add_library(Geometrize::Geometrize ALIAS geometrize)
target_include_directories(geometrize
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/geometrize-lib/geometrize/>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(geometrize PROPERTIES
    EXPORT_NAME Geometrize
)
