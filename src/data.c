 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "data.h"
#include "parse.h"

struct Signstable signtable[MAXROW];

/*count row of instructions*/
int decimalAdrs = STARTADDRS;

struct Instructions instructions[MAXROW];

/*count row of signs*/
int signCount = 0;

/*list of supported commands in asm */
struct  cmd commands[NUMCMD] = {
	{ MOV, "mov", 2, "0000" },
	{ CMP, "cmp", 2, "0001" },
	{ ADD, "add", 2, "0010" },
	{ SUB, "sub", 2, "0011" },
	{ NOT, "not", 1, "0100" },
	{ CLR, "clr", 1, "0101" },
	{ LEA, "lea", 2, "0110" },
	{ INC, "inc", 1, "0111" },
	{ DEC, "dec", 1, "1000" },
	{ JMP, "jmp", 1, "1001" },
	{ BNE, "bne", 1, "1010" },
	{ RED, "red", 1, "1011" },
	{ PRN, "prn", 1, "1100" },
	{ JSR, "jsr", 1, "1101" },
	{ RTS, "rts", 0, "1110" },
	{ STOP, "stop", 0, "1111" },
};

/*labels that exists in	*/
char *labels[] = {
	".extern",
	".entry",
	".data",
	".string",
};

/*clear sign and instruction tables*/
int clearTables()
{
	int i;
	/*clear sign table and counter*/
	for ( i = 0; i < signCount; i++)
	{
		signtable[i].DC = 0;
		*(signtable[i].DCBase8) = '\0';
		*(signtable[i].name) = '\0';
		signtable[i].type = -1;
	}
	signCount = 0;

	/*clear instruction table and counter*/
	for ( i = 0; i < decimalAdrs; i++)
	{
		*(instructions[i].base8counter) = '\0';
		*(instructions[i].base8Value) = '\0';
		*(instructions[i].binValue) = '\0';
		instructions[i].counter = 0;
		(*instructions[i].name) = '\0';
	}
	decimalAdrs = STARTADDRS;
	return 1;
}

/* add label to sign table 	*/
int addToSignTable(char * lable, int method)
{
	int i;
	char * findEnd = lable;

	SKIPNONSPACE(findEnd)
	*findEnd = '\0';

	for (i = 0; i < signCount; i++)
		if (strcmp(signtable[signCount].name, lable) == 0 && method == signtable[signCount].type)
		{
			fprintf(stderr, "%s", "error: label exists already\n");
			return 0;
		}

	if (method == INTRN)
		signtable[signCount].DC = decimalAdrs;  /*change 	*/
	strcpy(signtable[signCount].name, lable);
	signtable[signCount].type = method;

	signCount++;
	return 1;
}

/*add cmd to instruction table	*/
int addToInstructionsTable(char * cmd)
{
	int address;

	address = decimalAdrs - STARTADDRS;
	strcpy(instructions[address].name, cmd);
	instructions[address].counter = decimalAdrs;
	return 1;
}
