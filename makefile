assembler :  assembler.o data_ds.o parcer.o print_functions.o main.o file_management.o data.o symbol_table.o assembler_utilities.o
	gcc -g -ansi -lm -Wall assembler.o data.o data_ds.o parcer.o print_functions.o main.o file_management.o assembler_utilities.o symbol_table.o -o assembler

main.o : main.c file_management.h
	gcc -c -ansi main.c  -lm -std=c90 -Wall -pedantic -o main.o

assembler.o : assembler.c assembler.h symbol_table.h parcer.h print_functions.h data.h data_ds.h
	gcc -c -ansi assembler.c -lm -Wall -std=c90 -pedantic -o assembler.o

assembler_utilities.o: assembler_utilities.c assembler.h data.h assembler_utilities.h
	gcc -c -ansi assembler_utilities.c -lm -std=c90 -Wall -pedantic -o assembler_utilities.o

data_ds.o : data_ds.c assembler.h data.h
	gcc -c -ansi data_ds.c -lm -std=c90 -Wall -pedantic -o data_ds.o

parcer.o : parcer.c assembler.h data.h
	gcc -c -ansi parcer.c -lm -std=c90 -Wall -pedantic -o parcer.o 

print_functions.o : print_functions.c data.h
	gcc -c -ansi print_functions.c -lm -std=c90 -Wall -pedantic -o print_functions.o

file_management.o : file_management.c file_management.h assembler.h
	gcc -c -ansi file_management.c -lm -std=c90 -Wall -pedantic -o file_management.o

symbol_table.o : symbol_table.c assembler.h data.h
	gcc -c -ansi symbol_table.c -lm -std=c90 -Wall -pedantic -o symbol_table.o

data.o : data.c data.h
	gcc -c -ansi data.c -lm -std=c90 -Wall -pedantic -o data.o
