cmake_minimum_required(VERSION 3.16)
project(opencsg VERSION 1.6.0 LANGUAGES CXX)

option(BUILD_EXAMPLE "Build example program" ON)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

include(GNUInstallDirs)

add_subdirectory(src)
if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
