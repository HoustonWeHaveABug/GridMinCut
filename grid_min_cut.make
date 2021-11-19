GRID_MIN_CUT_C_FLAGS=-c -O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

grid_min_cut: grid_min_cut.o
	gcc -o grid_min_cut grid_min_cut.o

grid_min_cut.o: grid_min_cut.c grid_min_cut.make
	gcc ${GRID_MIN_CUT_C_FLAGS} -o grid_min_cut.o grid_min_cut.c

clean:
	rm -f grid_min_cut grid_min_cut.o
