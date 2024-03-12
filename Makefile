obj_files: ./build/cjlib_queue.o ./build/cjlib_dictionary.o ./build/cjlib.o
obj_files_debug: ./build/cjlib_debug.o ./build/cjlib_queue_debug.o ./build/cjlib_dictionary_debug.o ./build/cjlib_debug.o

GCC = gcc
header_loc = -I ./include/ -I ./src/include/

c_production_flags = -O3 -Wall -Werror -Wpedantic -Wnull-dereference -Wextra -Wunreachable-code -Wpointer-arith -Wmissing-include-dirs -Wstrict-prototypes -Wunused-result -Waggregate-return -Wredundant-decls
c_debug_flags = -g -Wall -Wpedantic -Wnull-dereference -Wextra -Wunreachable-code -Wpointer-arith -Wmissing-include-dirs -Wstrict-prototypes -Wunused-result -Waggregate-return -Wredundant-decls

all: dir_make ${obj_files}
	${GCC} ${c_production_flags} ${header_loc} -c ./src/cjlib.c ${obj_files} -o ./bin/lib_cjlib.o
	ar rcs ./lib/lib_cjlib.a ./bin/lib_cjlib.o 

debug: dir_make ${obj_files_debug}
	${GCC} ${c_debug_flags} ${header_loc} -c ./src/cjlib.c ${obj_files_debug} -o ./bin/lib_cjlib_debug.o
	ar rcs ./lib/lib_cjlib_debug.a ./bin/lib_cjlib_debug.o 

./build/cjlib_queue.o: ./src/cjlib_queue.c
	${GCC} ${c_production_flags} ${header_loc} -c ./src/cjlib_queue.c -o ./build/cjlib_queue.o

./build/cjlib_dictionary.o: ./src/cjlib_dictionary.c
	${GCC} ${c_production_flags} ${header_loc} -c ./src/cjlib_dictionary.c -o ./build/cjlib_dictionary.o

./build/cjlib_queue_debug.o: ./src/cjlib_queue.c
	${GCC} ${c_debug_flags} ${header_loc} -c ./src/cjlib_queue.c -o ./build/cjlib_queue_debug.o

./build/cjlib_dictionary_debug.o: ./src/cjlib_dictionary.c
	${GCC} ${c_debug_flags} ${header_loc} -c ./src/cjlib_dictionary.c -o ./build/cjlib_dictionary_debug.o

dir_make:
	mkdir -p ./bin/
	mkdir -p ./build/
	mkdir -p ./lib/

clean:
	rm -rf ./build/

cleanall:
	rm -rf ./build/
	rm -rf ./bin/