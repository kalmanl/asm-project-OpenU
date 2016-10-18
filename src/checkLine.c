 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "parse.h"
#include "data.h"
#include "checkLine.h"
#include "checkValid.h"

/*check if input line from asm is label*/
/*gets line returns true or false or error*/
int checkIfLabel(char * pline)
{
	char label[30];
	char * lableend;
	char * prv;
	char * labletype;
	int methodOp = -1;

	lableend = strchr(pline, LEBLEEND);
	if (lableend)
	{
		prv = pline;
		pline = strchr(pline, SPACE);
		*lableend = '\0';
		strcpy(label, prv);

		if (!addToSignTable(label, INTRN))
			return ERROR;
		SKIPSPACE(pline)

		if (!addToInstructionsTable(pline))
			return ERROR;
		labletype = strstr(pline, labels[DATA]);
		if (labletype)
			methodOp = numOfData(pline);
		else
		{
			labletype = strstr(pline, labels[STRING]);
			if (labletype)
				methodOp = numOfString(pline);
			else if (isalpha(*label))
				methodOp = FindAddressingModes(pline);
		}
		if (methodOp == ERROR)
			return ERROR;
		decimalAdrs += methodOp;
		decimalAdrs++;
		return 1;
	}
	return 0;
}

/*check if input line from asm is a command*/
/*gets line returns true or false*/
int checkCmd(char * pline)
{
	int CountCmdLines;
	addToInstructionsTable(pline);
	CountCmdLines = FindAddressingModes(pline);
	if (CountCmdLines != -1)
		decimalAdrs += CountCmdLines;
	else
	{
		fprintf(stderr, "%s", "error with command");
		return ERROR;
	}
	decimalAdrs++;
	return 1;
}

/*check if input line from asm is label of ent or exst*/
/*gets line returns true or false*/
int checkIfEnteryExstOrEnt(char * pline)
{
	char label[30];
	char * lableend;
	int type;

	if (*pline == DOT)
	{
		if (strncmp(pline, labels[EXTERN], strlen(labels[EXTERN])) == 0)
		{
			type = EXTERN;
			pline += strlen(labels[EXTERN]);
		}
		else if (strncmp(pline, labels[ENTRY], strlen(labels[ENTRY])) == 0)
		{
			type = ENTRY;
			pline += strlen(labels[ENTRY]);
		}
		else
		{
			printf("error no such type of label");
			return -1;
		}

		SKIPSPACE(pline)
		lableend = pline;
		SKIPNONSPACE(lableend)
		lableend = '\0';
		strcpy(label, pline);

		addToSignTable(label, type);

		return 1;
	}
	return 0;
}
