all :
	gcc -c -g ./src/main.c
	gcc -c -g ./src/assembler.c
	gcc -c -g ./src/instruction_handler.c
	gcc -o assembler main.o assembler.o instruction_handler.o