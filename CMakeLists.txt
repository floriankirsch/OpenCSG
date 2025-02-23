cmake_minimum_required(VERSION 3.11)
project(OpenCSG LANGUAGES CXX VERSION 1.8.0)

option(BUILD_EXAMPLE "Build example program" ON)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

include(GNUInstallDirs)

add_subdirectory(src)
if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
