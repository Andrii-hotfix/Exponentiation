cmake_minimum_required(VERSION 3.5)

project(Exponentiation LANGUAGES CXX)

find_package(GTest REQUIRED)
find_package(fmt REQUIRED)

enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake) # Include custom modules

add_library(Exponentiation
            bigint.cpp
            bigintfunct.cpp
            )

target_include_directories(${PROJECT_NAME} INTERFACE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           )

target_link_libraries(${PROJECT_NAME} fmt::fmt)

project(exponentiation-main)

add_executable(exponentiation-main
               main.cpp)

target_link_libraries(exponentiation-main
                      Exponentiation)

add_subdirectory(Test)
