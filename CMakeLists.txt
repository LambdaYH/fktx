cmake_minimum_required(VERSION 3.12)
project(fktx)
set(CMAKE_CXX_STANDARD 17)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
ELSE ()
message(FATAL_ERROR "unsupported platform")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb") #debug模式下开启-g选项
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
endif()

set(BUILD_SHARED_LIBS OFF)

# yaml-cpp
add_subdirectory(third-party/yaml-cpp)

# spdlog
add_subdirectory(third-party/spdlog)

add_executable(${PROJECT_NAME} 
                src/main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE
                            src
)    

target_link_libraries(${PROJECT_NAME} PRIVATE 
                        yaml-cpp 
                        spdlog
)

