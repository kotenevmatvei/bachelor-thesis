cmake_minimum_required(VERSION 3.22.1)
project(bachelor_thesis C)

set(CMAKE_C_STANDARD 17)

add_executable(bachelor_thesis main.c)

target_link_libraries(bachelor_thesis
        PRIVATE
        gsl
        gslcblas
        m
)

