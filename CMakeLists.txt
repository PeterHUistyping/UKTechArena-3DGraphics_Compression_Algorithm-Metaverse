# $COPYRIGHT

cmake_minimum_required(VERSION 3.0.0)
project(example-template VERSION 0.1.0)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
)

list(APPEND samples encoder)
list(APPEND samples decoder)
# list(APPEND samples serialize)


file(GLOB_RECURSE core_sources ${PROJECT_SOURCE_DIR}/src/core/*.cpp)
foreach(sample ${samples})
    set(exec ${sample})
    file(GLOB_RECURSE ${sample}_sources ${PROJECT_SOURCE_DIR}/src/${sample}/*.cpp)
    add_executable(${exec} 
        ${PROJECT_SOURCE_DIR}/src/${sample}_main.cpp
        ${core_sources}
        ${${sample}_sources})
    target_compile_options(${exec} PUBLIC -std=gnu++17 -Wall -O3)
    install(TARGETS ${exec} DESTINATION bin)
endforeach(sample)