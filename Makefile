all: main.exe clean

main.exe: dynarray.o 
	gcc main.c dynarray.o -o main

dynarray.o: Libraries/dynarray.c Libraries/dynarray.h
	gcc -c Libraries/dynarray.c -o dynarray.o
	
clean:
	rm dynarray.o
	
.PHONY: all clean