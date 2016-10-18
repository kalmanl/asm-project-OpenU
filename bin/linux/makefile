all: assembler

assembler : main.o first.o checkLine.o checkValid.o checkCmd.o data.o output.o second.o 
	gcc -g -ansi -Wall -pedantic main.o first.o checkLine.o checkValid.o checkCmd.o data.o output.o second.o -o assembler -lm
main.o: main.c first.h output.h second.h
	gcc -c -ansi -Wall -pedantic main.c -o main.o
first.o: first.c first.h parse.h data.h checkLine.h
	gcc -c -ansi -Wall -pedantic first.c -o first.o
checkLine.o: checkLine.c checkLine.h data.h checkValid.h parse.h
	gcc -c -ansi -Wall -pedantic checkLine.c -o checkLine.o
checkValid.o: checkValid.c checkValid.h data.h parse.h checkCmd.h
	gcc -c -ansi -Wall -pedantic checkValid.c -o checkValid.o
checkCmd.o: checkCmd.c checkCmd.h data.h parse.h
	gcc -c -ansi -Wall -pedantic checkCmd.c -o checkCmd.o
data.o: data.c data.h parse.h
	gcc -c -ansi -Wall -pedantic data.c -o data.o
output.o: output.c output.h data.h parse.h
	gcc -c -ansi -Wall -pedantic output.c -o output.o
second.o: second.c second.h data.h parse.h checkCmd.h data.h
	gcc -c -ansi -Wall -pedantic second.c -o second.o
	
tests: test1
	
test1: all asm.as
	./assembler asm
	echo testing...
	cat asm.op
	cat asm.ext
	cat asm.ent
	
#cleanup
clean:
	rm *.o

