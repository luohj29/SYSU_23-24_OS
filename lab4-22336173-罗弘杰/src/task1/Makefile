main.out: main.o c_func.o asm_func.o cpp_func.o
		g++ -o main.out  main.o c_func.o asm_func.o cpp_func.o -m32

main.o: main.cpp
		g++ -o main.o -m32 -c main.cpp

c_func.o: c_func.c
		gcc -c c_func.c

asm_func.o: asm_func.asm
		nasm -o asm_func.o -f elf32 asm_func.asm

cpp_func.o: cpp_func.cpp
		g++ -o cpp_func.o -m32 -c cpp_func.cpp

clean:
		rm -f *.o *.out
