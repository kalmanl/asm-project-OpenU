#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "data.h"
#include "parse.h"
#include "checkCmd.h"


/*gets command and returns the first operand	*/
/*needs to be called after getOpTwo	*/
/*instead return start with leading zero at end	*/
char * getOpOne(char* cmd)
{
	char * start = cmd;
	char * end;

	end = strchr(cmd, COMMA);
	*end = '\0';
	return start;
}

char * getSingalOp(char *cmd)
{
	char *end;

	SKIPSPACE(cmd)
	end = cmd;
	SKIPNONSPACE(end)
	*end = '\n';
	return cmd;
}

/*gets the second operand 	*/

/* instead return start of cmd 	*/
char * getOpTwo(char* cmd)
{
	char * start, *end;
	start = cmd;
	start = (strchr(cmd, COMMA));
	start++;
	SKIPSPACE(start)
	end = start;
	SKIPNONSPACE(end)
	*end = '\0';
	return start;
}

/*checks that the scheme two is valid	*/
int schemeTwoCheck(char *oper)
{
	char *cp;
	int checkIfnum = 0;

	cp = strchr(oper, OPENBRACE);
	if (cp)
	{
		cp++;
		checkIfnum = atoi(++cp);
		if (checkIfnum || *cp == '0')
		{
			while (isdigit(*cp))
			{
				cp++;
			}
			if (*(cp) == DASH)
			{
				cp++;
				if (atoi(++cp))
					while (isdigit(*cp))
					{
						cp++;
					}
				if (*cp == CLOSEBRACE)
					return 3;
			}
			return ERROR;
		}
	}
	return 0;
}

int isReg(char * oper)
{
	int registerNum;
	if (*oper == 'r')
	{
		if (*(++oper) == '0')
			return 1;
		registerNum = atoi(oper);
		if (registerNum > 0 && registerNum < 8)
			return 1;
	}
	return 0;
}

/*gets an operand and returns its addressing scheme 1-4 or 0 if there's an error	*/
int checkOper(char* oper)
{
	int value;

	SKIPSPACE(oper)

	/*addressing scheme number 0 (returns 1 zero is for errors)	*/
	if (*oper == '#')
	{
		oper++;
		if (atoi(oper)) /*number after #	*/
			return 1;
		else
		{
			fprintf(stderr, "%s", "addressing scheme 0 error\n");
			return 0;
		}
	}
	/*addressing scheme number 2(returns 3)	*/
	if (isalpha(*oper))
	{
		/*checks the scheme 2 syntax	*/
		value = schemeTwoCheck(oper);
		if (value)
			return value;
	}
	/*addressing scheme number 3 	*/
	if (isReg(oper))
		return 4;

	/*if none of the above then it's direct addressing=scheme number 1 label	*/
	if (isalpha(*oper))
		return 2;
	fprintf(stderr, "%s", "addressing scheme error\n");
	return ERROR;
}

/*checks that everything is ok with the operands	*/
int checkAddrScm(const char *cmdName, const int numOfOp, const char *oper)
{
	char tempOp[MAXLINE];
	int opTwoScheme;
	char *opOne, *opTwo;
	int opOneScheme;

	strcpy(tempOp, oper);

	if (numOfOp == 1)
	{
		opTwo = getSingalOp(tempOp);
		opTwoScheme = checkOper(opTwo);
	}
	else if (numOfOp == 2)
	{
		opTwo = getOpTwo(tempOp);
		opTwoScheme = checkOper(opTwo);

		opOne = getOpOne(tempOp);
		opOneScheme = checkOper(opOne);
	}
	/*need name of group. put in char array to check with for. nicer	*/
	/*op one is source op two is dest	*/
	/* op one 0,1,2,3 op two 1,3 	*/
	if (strcmp(cmdName, commands[MOV].name) == 0 || strcmp(cmdName, commands[ADD].name) == 0 || strcmp(cmdName, commands[SUB].name) == 0)
	{
		if (opOneScheme == 4 && opTwoScheme == 4)
			return BOTHREG;
		if ((opOneScheme) && (opTwoScheme == 2 || opTwoScheme == 4))
			return 1;
	}
	/* op one 0,1,2,3 op two 0,1,2,3	*/
	if (strcmp(cmdName, commands[CMP].name) == 0)
	{
		if (opOneScheme == 4 && opTwoScheme == 4)
			return BOTHREG;
		if (opOneScheme&&opTwoScheme)
			return 1;
	}

	/*op one is 1. op two is 1,3	*/
	if (strcmp(cmdName, commands[LEA].name) == 0)
		if (opOneScheme == 2 && (opTwoScheme == 2 || opTwoScheme == 4))
			return 1;

	/*no first operand ,op two is 1,3	*/
	if (strcmp(cmdName, commands[NOT].name) == 0 || strcmp(cmdName, commands[CLR].name) == 0 || strcmp(cmdName, commands[INC].name ) == 0
	 || strcmp(cmdName, commands[DEC].name) == 0 || strcmp(cmdName, commands[JMP].name) == 0 || strcmp(cmdName, commands[BNE].name ) == 0 
	 ||	strcmp(cmdName, commands[RED].name) == 0 || strcmp(cmdName, commands[JSR].name ) == 0)
		if (opTwoScheme == 2 || opTwoScheme == 4)
			return 1;

	/*no op one, op two 0,1,2,3	*/
	if (strcmp(cmdName, commands[PRN].name ) == 0)
		if (opTwoScheme)
			return 1;

	/*no op one, op two is 1	*/
	if (strcmp(cmdName, commands[JSR].name ) == 0)
		if (opTwoScheme == 2 || opTwoScheme == 4)
			return 1;

	/*no op at all	*/
	if ((strcmp(cmdName, commands[RTS].name) == 0) || (strcmp(cmdName, commands[STOP].name ) == 0))
		return 1;

	/*fixed extra return 0 for everyone	*/

	fprintf(stderr, "%s\n", "invalid addressing scheme\n");
	return 0;
}
