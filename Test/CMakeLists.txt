cmake_minimum_required(VERSION 3.5)

project(test-exponentiation LANGUAGES CXX)

add_executable(test-exponentiation
            main.cpp
            )

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -lgmp -lgmpxx")

target_link_libraries(test-exponentiation
                      Exponentiation
                      GTest::GTest GTest::Main)
