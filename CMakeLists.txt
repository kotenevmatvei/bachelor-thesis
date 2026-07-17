cmake_minimum_required(VERSION 3.20)
project(bachelor_thesis C)

# index the project
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force C17 (GNU17) standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS ON) # Allows gnu17 extensions

# Add compiler flags (-Wall -Werror -Wextra -O3)
add_compile_options(-Wall -Werror -Wextra -O3 -g)

# Find math library (libm)
find_library(MATH_LIB m)

find_package(OpenMP REQUIRED)

# Link directories for GSL (if not in standard paths)
# link_directories(/path/to/gsl/lib)

# Build the uop
add_executable(uop uop.c io.c simulation.c)
target_link_libraries(uop ${MATH_LIB} gsl gslcblas)

# build single diffusion
add_executable(single_diffusion single_diffusion.c io.c simulation.c utils.c)
target_link_libraries(single_diffusion ${MATH_LIB} gsl gslcblas)

# build double diffusion
add_executable(double_diffusion double_dependent_diffusion.c io.c simulation.c utils.c)
target_link_libraries(double_diffusion PRIVATE OpenMP::OpenMP_C ${MATH_LIB} gsl gslcblas)

# build double diffusion
add_executable(tripple_diffusion tripple_diffusion.c io.c simulation.c utils.c)
target_link_libraries(tripple_diffusion PRIVATE OpenMP::OpenMP_C ${MATH_LIB} gsl gslcblas)

# Build the tests
add_executable(run_tests testing.c io.c tests.c simulation.c)
target_link_libraries(run_tests ${MATH_LIB} gsl gslcblas)
