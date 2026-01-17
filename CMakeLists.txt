cmake_minimum_required(VERSION 3.22.1)
project(bachelor_thesis C)

set(CMAKE_C_STANDARD 17)

add_executable(simulation simulation.c)

target_link_libraries(simulation
        PRIVATE
        gsl
        gslcblas
        m
)

