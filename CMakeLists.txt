cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14) #set C++ standard
set(CMAKE_C_STANDARD 11) #set C standard
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

project(single_shot)

add_subdirectory(bp_osd)

set(PROJECT single_shot_hgp3d)
project(${PROJECT})
add_executable(${PROJECT} sim_scripts/single_shot_hgp3d.cpp)
target_link_libraries(${PROJECT} bp_osd radford_neal mtwister_rng json_library)


