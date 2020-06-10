project(Random LANGUAGES CXX)
enable_testing()

add_library(${PROJECT_NAME}
            bbs.cpp
            gost.cpp
            )

target_link_libraries(${PROJECT_NAME}
                      Exponentiation
                      )

add_executable(random-main
               main.cpp
               )

target_link_libraries(random-main
                      Random
                      )
