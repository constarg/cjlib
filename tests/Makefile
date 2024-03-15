
librareis_producation = ../lib/libcjlib.a
librareis_debug = ../lib/libcjlib_debug.a

header_loc = -I ../include/ -I ../src/include/
GCC = gcc
c_production_flags = -O3 -Wall -Werror -Wpedantic -Wnull-dereference -Wextra -Wunreachable-code -Wpointer-arith -Wmissing-include-dirs -Wstrict-prototypes -Wunused-result -Waggregate-return -Wredundant-decls
c_debug_flags = -g -Wall -Wpedantic -Wnull-dereference -Wextra -Wunreachable-code -Wpointer-arith -Wmissing-include-dirs -Wstrict-prototypes -Wunused-result -Waggregate-return -Wredundant-decls

all: dir_make ${librareis_producation}
	${GCC} ${c_production_flags} ${header_loc} -c ./src/main.c -o ./build/main.o
	${GCC} ./build/main.o -L. ${librareis_producation} -o ./bin/main.out

debug: dir_make ${librareis_debug}
	${GCC} ${c_debug_flags} ${header_loc} -c ./src/main.c -o ./build/main_debug.o
	${GCC} ./build/main_debug.o -L. ${librareis_debug} -o ./bin/main_debug.out

dir_make:
	mkdir -p ./bin/
	mkdir -p ./build/

clean:
	rm -rf ./bin/