#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "data.h"
#include "parse.h"
#include "checkValid.h"
#include "checkCmd.h"

/*gets asm command checks how many operands it needs to have*/
int FindAddressingModes(char * cmd)
{
	int i;
	char * end;
	char * operation;
	int numOfMod = -1;
	int checkScm = 0;
	while (isspace(*cmd) || *cmd == COMMA)
		cmd++;
	end = cmd;
	if (*cmd == DOT)
		return 0;
	while (isalpha(*end))
		end++;
	operation = end + 1;	/*expected \n before end therefore wont be a error	*/
	*end = '\0';

	for (i = 0; i < NUMCMD; i++)
		if (strcmp(cmd, commands[i].name) == 0 && checkNUmOfOperands(operation, commands[i].numOfOp))
		{
			numOfMod = commands[i].numOfOp;
			break;
		}

	if (numOfMod == ERROR)
	{
		fprintf(stderr, "%s invalid command format\n", cmd);
		return ERROR;
	}

	checkScm = checkAddrScm(cmd, numOfMod, operation);

	if (checkScm == BOTHREG)
		return 1;
	if (checkScm)
		return numOfMod;
	return ERROR;
}

/*checks if the command has the right format of operands as expected*/
int checkNUmOfOperands(char * operands, int num)
{
	char * start = operands;
	SKIPSPACE(operands)

	/*no operands check if no extra just	*/
	if (num == 0)
	{
		SKIPSPACE(operands)
		if (!*operands)
			return 1;
	}

	/*tow operands check format op1,op2	*/
	if (num == 2)
		if (isalpha(*operands))
		{
			operands++;
			while (!isspace(*operands) && *operands != COMMA)
				operands++;
			SKIPSPACE(operands)
			if (!*start != *operands)
				if (strchr(operands, COMMA))
				{
					operands++;
					SKIPSPACE(operands)
						while (!isspace(*operands))
							operands++;
					SKIPSPACE(operands)
						if (!*operands)
							return 1;
				}
		}

	/*one operand op1 check	*/
	if (num == 1)
	{
		SKIPSPACE(operands)
		SKIPNONSPACE(operands)
			if (!*operands || isspace(*operands))
				return 1;
	}

	fprintf(stderr, "error: mismatch with number of operands\n");
	return 0;
}

/*gets .string command and return num of strings in asm deceleration*/
int numOfData(char * pline)
{
	int sum = 0, dataValue = 0;
	char * delimiter;
	pline = strchr(pline, SPACE);
	delimiter = strchr(pline, COMMA);
	dataValue = atoi(pline);
	if (delimiter)
	{
		delimiter++;
		if (dataValue || (*pline == '0'))
			while (delimiter)
			{
				delimiter = strchr(delimiter, COMMA);
				sum++;
				if (delimiter)
					delimiter++;
			}
		return sum;
	}
	else if (dataValue || (*pline == '0'))
		return sum;
	return ERROR;
}

/*gets .string command and return num of strings in asm deceleration*/
int numOfString(char * pline)
{
	int i = 0;
	pline = strchr(pline, STRQUTS);
	if (pline)
	{
		pline++;
		while (isalpha(*pline))
		{
			pline++;
			i++;
		}
		if (*pline == STRQUTS)
			return i;
	}
	return ERROR;
}
