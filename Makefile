# # --- VARIABLES ---
# CC = gcc
# CFLAGS = -Wall -Werror -Wextra -O3 -std=gnu17
# LDLIBS = -lm -lgsl -lgslcblas
# 
# # --- RULES ---
# 
# # 1. The Default Rule: Build BOTH executables
# all: run_simulation run_tests
# 
# # 2. Linker Rule for the App
# run_simulation: io.o main.o simulation.o
# 	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)
# 
# # 3. Linker Rule for the Tests
# run_tests: io.o tests.o testing.o simulation.o
# 	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)
# 
# # 4. The Universal Compiler Rule (Builds ALL .o files)
# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@
# 
# # 5. Clean up
# clean:
# 	rm -f *.o run_simulation run_tests

all:
	cmake --build build
