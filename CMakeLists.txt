cmake_minimum_required(VERSION 3.20)
project(bachelor_thesis C)

# Force C17 (GNU17) standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS ON) # Allows gnu17 extensions

# Add compiler flags (-Wall -Werror -Wextra -O3)
add_compile_options(-Wall -Werror -Wextra -g)

# Find math library (libm)
find_library(MATH_LIB m)

# Link directories for GSL (if not in standard paths)
# link_directories(/path/to/gsl/lib)

# 1. Build the main app
add_executable(main main.c io.c simulation.c)
target_link_libraries(main ${MATH_LIB} gsl gslcblas)

# 2. Build the tests
add_executable(run_tests testing.c io.c tests.c simulation.c)
target_link_libraries(run_tests ${MATH_LIB} gsl gslcblas)