cmake_minimum_required(VERSION 3.0.0)
project(Raytracer VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# detect apple silicon
set(APPLE_SILICON false)
if(RAY_PLATFORM_MAC)
    if(${CMAKE_OSX_ARCHITECTURES} STREQUAL "arm64")
        set(APPLE_SILICON true)
    endif()
    
    if(APPLE_SILICON)
        message(STATUS "Building for Apple Silicon Mac.")
    else()
        message(STATUS "Building for Intel Mac.")
    endif(APPLE_SILICON)
endif(RAY_PLATFORM_MAC)

set( DEFUALT_BULID_TYPE "Release")

include_directories( "${Raytracer_SOURCE_DIR}/src" )

file(GLOB_RECURSE project_headers src/*.h src/*.hpp)
file(GLOB_RECURSE project_cpps src/*.cpp)
file(GLOB_RECURSE project_cs src/*.c)
file(GLOB_RECURSE project_ccs src/*.cc)

include_directories("${Raytracer_SOURCE_DIR}/src/3rd/include/eigen3")

set(all_files ${project_headers} ${project_cpps} ${project_cs} ${project_ccs})

add_executable(Raytracer ${all_files})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
