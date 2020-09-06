cmake_minimum_required(VERSION 3.17)
project(hdr_imaging_cplusplus)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(src)

add_executable(hdr_imaging_cplusplus
        include/hdr_imaging_interface.hpp
        src/hdr_recover_muti_exposure.cpp
        src/hdr_recover_muti_exposure.h
        src/main.cpp
        src/response_solver.cpp
        src/response_solver.h
        src/Source.cpp)
