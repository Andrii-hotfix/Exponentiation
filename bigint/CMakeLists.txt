project(Exponentiation LANGUAGES CXX)

enable_testing()
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake) # Include custom modules

add_library(Exponentiation
            bigint.cpp
            bigintfunct.cpp
            )

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           )

target_link_libraries(${PROJECT_NAME}
                      Boost::program_options
                      fmt::fmt
                      )

project(exponentiation-main)

add_executable(exponentiation-main
               main.cpp)

target_link_libraries(exponentiation-main
                      Exponentiation)

add_subdirectory(Test)
