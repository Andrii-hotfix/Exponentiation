cmake_minimum_required(VERSION 3.5)

project(CrpytoStudy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PROJECT_DESCRIPTION "Crpytography study")

find_package(GTest)
find_package(fmt REQUIRED)
find_package(Boost COMPONENTS program_options)
find_package(OpenSSL)

add_subdirectory(bigint)
add_subdirectory(random)
