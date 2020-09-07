cmake_minimum_required(VERSION 3.17)
project(hdr_imaging_cplusplus)

set(CMAKE_CXX_STANDARD 17)


include_directories(include)
include_directories(src)

find_package(opencv REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/usr/local/include/eigen3)

add_executable(hdr_imaging_cplusplus
        include/hdr_imaging_interface.hpp
        src/hdr_recover_muti_exposure.cpp
        src/hdr_recover_muti_exposure.h
        src/main.cpp
        src/response_solver.cpp
        src/response_solver.h
        )

target_link_libraries(hdr_imaging_cplusplus ${OpenCV_LIBS})
