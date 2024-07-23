cmake_minimum_required(VERSION 3.10)
project(birdy)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

set(SOURCES
    src/main.cpp
)

add_executable(birdy ${SOURCES})

if(WIN32)
    target_link_libraries(birdy)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(birdy ${COCOA_LIBRARY})
elseif(UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(birdy ${X11_LIBRARIES})
    include_directories(${X11_INCLUDE_DIR})
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
