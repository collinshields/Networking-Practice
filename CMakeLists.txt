cmake_minimum_required(VERSION 3.15)
project(Networking_Practice LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Automatically add any top-level subdirectory that contains a CMakeLists.txt
file(GLOB CHILD_DIRS RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/*)
foreach(dir ${CHILD_DIRS})
    if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${dir}" AND EXISTS "${CMAKE_SOURCE_DIR}/${dir}/CMakeLists.txt")
        add_subdirectory("${dir}")
    endif()
endforeach()