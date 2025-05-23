obj_files = ./build/cjlib.o ./build/cjlib_queue.o ./build/cjlib_dictionary.o ./build/cjlib_stack.o ./build/cjlib_error.o ./build/cjlib_list.o
obj_files_debug = ./build/cjlib_debug.o ./build/cjlib_dictionary_debug.o ./build/cjlib_queue_debug.o ./build/cjlib_stack_debug.o ./build/cjlib_error_debug.o ./build/cjlib_list_debug.o

test_file_dir = ./tests/bin/

test_file = main.out
test_file_debug = main_debug.out

GCC = gcc
header_loc = -I ./include/ -I ./src/include/

c_production_flags = -Wall -Werror -Wpedantic -Wnull-dereference -Wextra -Wunreachable-code -Wpointer-arith -Wmissing-include-dirs -Wstrict-prototypes -Wunused-result -Waggregate-return -Wredundant-decls
c_debug_flags = -g -Wall -Wpedantic -Wnull-dereference -Wextra -Wunreachable-code -Wpointer-arith -Wmissing-include-dirs -Wstrict-prototypes -Wunused-result -Waggregate-return -Wredundant-decls

all: dir_make ${obj_files}
	ar rcs ./lib/libcjlib.a ${obj_files}

debug: dir_make ${obj_files_debug}
	ar rcs ./lib/libcjlib_debug.a ${obj_files_debug}

build_test_debug:
	cd tests/ && make debug

build_test:
	cd tests/ && make 

run_test_debug: build_test_debug ${test_file_dir}${test_file_debug}
	cd ${test_file_dir} && ./${test_file_debug}

run_test_debug_check: build_test_debug ${test_file_dir}${test_file_debug}
	cd ${test_file_dir} && valgrind --leak-check=full ./${test_file_debug}

run_test: build_test ${test_file_dir}${test_file}
	cd ${test_file_dir} && ./${test_file}

run_test_check: build_test_debug ${test_file_dir}${test_file_debug}
	cd ${test_file_dir} && valgrind --leak-check=full ./${test_file}

./build/cjlib.o: ./src/cjlib.c
	${GCC} ${c_production_flags} ${header_loc} -c ./src/cjlib.c -o ./build/cjlib.o

./build/cjlib_queue.o: ./src/cjlib_queue.c
	${GCC} ${c_production_flags} ${header_loc} -c ./src/cjlib_queue.c -o ./build/cjlib_queue.o

./build/cjlib_dictionary.o: ./src/cjlib_dictionary.c
	${GCC} ${c_production_flags} ${header_loc} -c ./src/cjlib_dictionary.c -o ./build/cjlib_dictionary.o

./build/cjlib_stack.o: ./src/cjlib_stack.c
	${GCC} ${c_production_flags} ${header_loc} -c ./src/cjlib_stack.c -o ./build/cjlib_stack.o

./build/cjlib_error.o: ./src/cjlib_error.c
	${GCC} ${c_production_flags} ${header_loc} -c ./src/cjlib_error.c -o ./build/cjlib_error.o

./build/cjlib_list.o: ./src/cjlib_list.c
	${GCC} ${c_production_flags} ${header_loc} -c ./src/cjlib_list.c -o ./build/cjlib_list.o

./build/cjlib_debug.o: ./src/cjlib.c
	${GCC} ${c_debug_flags} ${header_loc} -c ./src/cjlib.c -o ./build/cjlib_debug.o

./build/cjlib_queue_debug.o: ./src/cjlib_queue.c
	${GCC} ${c_debug_flags} ${header_loc} -c ./src/cjlib_queue.c -o ./build/cjlib_queue_debug.o

./build/cjlib_dictionary_debug.o: ./src/cjlib_dictionary.c
	${GCC} ${c_debug_flags} ${header_loc} -c ./src/cjlib_dictionary.c -o ./build/cjlib_dictionary_debug.o

./build/cjlib_stack_debug.o: ./src/cjlib_stack.c
	${GCC} ${c_debug_flags} ${header_loc} -c ./src/cjlib_stack.c -o ./build/cjlib_stack_debug.o

./build/cjlib_error_debug.o: ./src/cjlib_error.c
	${GCC} ${c_debug_flags} ${header_loc} -c ./src/cjlib_error.c -o ./build/cjlib_error_debug.o

./build/cjlib_list_debug.o: ./src/cjlib_list.c
	${GCC} ${c_debug_flags} ${header_loc} -c ./src/cjlib_list.c -o ./build/cjlib_list_debug.o

dir_make:
	mkdir -p ./build/
	mkdir -p ./lib/

clean:
	rm -rf ./build/

cleanall:
	rm -rf ./build/
	rm -rf ./bin/
