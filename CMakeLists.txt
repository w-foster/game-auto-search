cmake_minimum_required(VERSION 3.10)

project(AutoSearch)

set(SOURCES
    src/testing.cpp
    src/screengrabber.cpp
    src/utils.cpp
)

add_executable(testing ${SOURCES})

target_include_directories(testing PRIVATE include)

# Link against the GDI32 library
target_link_libraries(testing PRIVATE gdi32)