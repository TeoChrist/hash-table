# Makefile for compilation with MinGW on Windows

CC = gcc
CFLAGS = -Wall -c -O3
LIB = hash_table.lib
DLL = hash_table.dll

all: lib dll

dll: hash_table.o
	$(CC) -shared -o $(DLL) $< -Wl,-no-undefined,--enable-runtime-pseudo-reloc,--out-implib,hash_table_imp.lib

lib: hash_table.o
	ar rcs $(LIB) $^

hash_table.o: hash_table.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	del *.exe *.exp *.lib *.dll *.o
