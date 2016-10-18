#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "data.h"
#include "parse.h"
#include "checkCmd.h"
#include "second.h"

/*conversion table to base 8 from binary*/
struct Convert bintoBase8[8] = {
	{ "000", '!' },
	{ "001", '@' },
	{ "010", '#' },
	{ "011", '$' },
	{ "100", '%' },
	{ "101", '^' },
	{ "110", '&' },
	{ "111", '*' },
};

/*reverses a string*/
void reverse(char s[])
{
	int c, i, j;
	for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

/*turns an integer into a 16 bit number and puts the result in dest*/
char * tobin(char *dest, int value)
{
	int i;
	for (i = 0; i<15; value >>= 1, i++)
	{
		dest[i] = value & 1 ? '1' : '0';
	}
	dest[i] = '\0';
	reverse(dest);
	return dest;
}

/* gets a binary number and converts it to special base 8 and puts it in dest */
void toBase8(char *bin, char *dest, int len)
{
	int i, j;
	for (i = 0; i < len; i += 3)
	{
		for (j = 0; j < 8; j++)
		{
			if (strncmp(bin, bintoBase8[j].bin, 3) == 0)
			{
				dest[i / 3] = bintoBase8[j].base8;
			}
		}
		bin += 3;
	}
}


/*gets a line which starts with .data and puts the integers in the instructions table*/
void insertInts(char *data, int startIndex)
{
	data += strlen(labels[DATA]);/*skip the ".data" part of the string*/
	if (strchr(data, COMMA) != NULL)
	{
		while (strchr(data, COMMA) != NULL)
		{
			SKIPSPACE(data)
				tobin(instructions[startIndex].binValue, atoi(data));
			toBase8(instructions[startIndex].binValue, instructions[startIndex].base8Value, 15);
			startIndex++;
			while (isdigit(*data) || *data == '-')
				data++;
			data++;
			SKIPSPACE(data)
		}
	}
	SKIPSPACE(data)
		tobin(instructions[startIndex].binValue, atoi(data));
	toBase8(instructions[startIndex].binValue, instructions[startIndex].base8Value, 15);
}

/*gets a line which starts with .string and inserts the chars in the instructions table*/
int insertChars(char *string, int startIndex)
{
	int checkStr;

	/*skip the ".string" part of the string*/
	string += strlen(labels[STRING]); 
	SKIPSPACE(string)
		string++;
	SKIPSPACE(string)
		checkStr = isalpha(*string);
	while (checkStr)
	{
		tobin(instructions[startIndex].binValue, *string);
		toBase8(instructions[startIndex].binValue, instructions[startIndex].base8Value, 15);

		startIndex++;
		string++;
		checkStr = isalpha(*string);
	}
	tobin(instructions[startIndex].binValue, 0);
	toBase8(instructions[startIndex].binValue, instructions[startIndex].base8Value, 15);
	return 1;
}

/* gets a command and returns its number */
int getCmdNum(char* cmd)
{
	int i;
	SKIPSPACE(cmd)
		for (i = 0; i < NUMCMD; i++)
		{
			if (strncmp(cmd, commands[i].name, 3) == 0)
				return commands[i].id;
		}
	return ERROR;
}

/* gets a cmd number and returns the binary equivalent */
char* group(int cmdNum)
{
	int group;
	group = commands[cmdNum].numOfOp;
	if (group == 0)
		return "00";
	if (group == 1)
		return "01";
	return "10";
}

/* gets an addressing scheme and returns the binary equivalent */
char* addrSchemeTobin(int addrScheme)
{
	if (addrScheme == 1)
		return "00";
	if (addrScheme == 2)
		return "01";
	if (addrScheme == 3)
		return "10";
	return "11";
}

/* lookup for immediate dynamic addressing (uses type flag) */
int dynamicSignTableLookup(char* label)
{
	int length, i;
	for (i = 0; i < MAXROW; i++)
	{
		length = (strlen(label)>strlen(signtable[i].name)) ? strlen(signtable[i].name) : strlen(label);
		if ((strncmp(signtable[i].name, label, length) == 0) && (signtable[i].type == INTRN))
		{
			return signtable[i].DC - STARTADDRS;
		}
	}
	return 0;
}
/* looks for label in the table and returns index in which it was found */
/* looks for label in the table and returns index in which it was found */
int signsTableLookup(char* label)
{
	int i;
	char* remNewline;
	remNewline = strchr(label, '\n');
	if (remNewline) 
		*remNewline = '\0';
	for (i = 0; i < signCount; i++)
	{
		if ((strncmp(signtable[i].name, label, strlen(label)) == 0))
		{
			return i;
		}
	}
	return 0;
}

/* adds all the references to labels to sign table */
int addLabelRefToSignTable(char * lable, int method, int index)
{
	char * findEnd = lable;
	SKIPNONSPACE(findEnd)
		*findEnd = '\0';

	signtable[signCount].DC = index + STARTADDRS;
	strcpy(signtable[signCount].name, lable);
	signtable[signCount].type = method;

	signCount++;
	return 1;
}

/* gets binary and returns its 2's complement value as a decimal */
int twosToDec(char* bin, int binLength)
{
	int i, dec = 0, neg = 0;
	/* if it's negative exchange all the 0's for 1's and vice versa */
	if (bin[0] == '1')
	{
		neg = 1;
		for (i = 0; i <binLength - 1; i++)
		{
			if (bin[i] == '0')bin[i] = '1';
			if (bin[i] == '1')bin[i] = '0';
		}
	}
	/* sum all the places where there's a one */
	for (i = 0; i < binLength; i++)
	{
		if (bin[i] == '1')
		{
			dec = dec + (int)pow(2, (binLength - 1 - i));
		}
	}
	if (neg == 1)
		dec = dec*(-1);
	return dec;
}

/* handles immediate dynamic addressing scheme */
void dynamicImmediateAddr(char* operand, int index)
{
	char* label = operand;
	int labelIndex, start, end, i;
	char temp[14];

	/* separate label */
	while (*operand != OPENBRACE)operand++;
	*operand = '\0'; operand++;

	labelIndex = dynamicSignTableLookup(label);
	/* select bytes from appropriate location and copy them to temp */
	end = 14 - atoi(operand);
	while (*operand != '-')operand++; operand++;
	start = 14 - atoi(operand);
	for (i = 0, start; start <= end; i++, start++)
	{
		temp[i] = instructions[labelIndex].binValue[start];
	}
	temp[i] = '\0';

	/* turns temp into a 2's complement decimal number and inserts that after converting to binary*/
	tobin(instructions[index].binValue, twosToDec(temp, strlen(temp)) * 4);
}
/* gets 2 registers and converts them into one binary word */
void twoRegsCnvrt(char* regOne, char* regTwo, int index)
{
	tobin(instructions[index].binValue, atoi(++regOne) * 256 + atoi(++regTwo) * 4);
	toBase8(instructions[index].binValue, instructions[index].base8Value, 15);
}

/*converts the operand itself into binary and inserts it into the table returns 0 if a label isn't found*/
int operTobinCnvrt(char* operand, int index, int addrScheme, int srcDest)
{
	int signIndex;
	if (addrScheme == 1)
	{
		operand++;
		tobin(instructions[index].binValue, atoi(operand) * 4);
	}
	if (addrScheme == 2)
	{
		signIndex = signsTableLookup(operand);
		if (signIndex)
		{
			if (signtable[signIndex].type == EXTERN)
			{
				tobin(instructions[index].binValue, 1);
				addLabelRefToSignTable(operand, REFRNC, index);
			}
			else
			{
				tobin(instructions[index].binValue, signtable[signIndex].DC * 4 + 2);
			}
		}
		else
			return 0;
	}
	if (addrScheme == 3)
	{
		dynamicImmediateAddr(operand, index);
	}
	if (addrScheme == 4)
	{
		operand++;
		if (srcDest == SRC)
			tobin(instructions[index].binValue, atoi(operand) * 256);
		if (srcDest == DEST)
			tobin(instructions[index].binValue, atoi(operand) * 4);
	}
	toBase8(instructions[index].binValue, instructions[index].base8Value, 15);
	return 1;
}

/* handles operand converting for 1 operand */
int oneOpCnvrt(char* binCmd, char* cmd, int index)
{
	char* opOne;
	int opOneScheme;
	strcat(binCmd, "00");
	opOne = getSingalOp(cmd);
	opOneScheme = checkOper(opOne);
	strcat(binCmd, addrSchemeTobin(opOneScheme));
	if (!operTobinCnvrt(opOne, index + 1, opOneScheme, DEST))
		return 0;
	return 1;
}

/* handles operand converting for 2 operands */
int twoOpCnvrt(char* binCmd, char* cmd, int index)
{
	char *opOne, *opTwo, *temp;
	int opOneScheme, opTwoScheme;
	opTwo = getOpTwo(cmd);
	opTwoScheme = checkOper(opTwo);
	opOne = getOpOne(cmd);
	opOneScheme = checkOper(opOne);
	temp = addrSchemeTobin(opTwoScheme);
	strcat(binCmd, addrSchemeTobin(opOneScheme));
	strcat(binCmd, temp);

	/* if they're both registers */
	if (opOneScheme == 4 && opTwoScheme == 4)
	{
		twoRegsCnvrt(opOne, opTwo, index + 1);
	}
	else
	{
		if ((!operTobinCnvrt(opOne, index + 1, opOneScheme, SRC)) || (!operTobinCnvrt(opTwo, index + 2, opTwoScheme, DEST)))
			return 0;
	}
	return 1;
}

/* converts the cmd to binary returns 0 in case of error (label not found) */
int cmdCnvrt(char *cmd, int index)
{
	char binCmd[16];
	int cmdNum;
	SKIPSPACE(cmd)
	strcpy(binCmd, FIXEDBYTES);
	cmdNum = getCmdNum(cmd);
	strcat(binCmd, group(cmdNum)); /*group*/
	strcat(binCmd, commands[cmdNum].opcode); /*opcode*/

	/* skip cmd name  and any spaces after that */
	SKIPNONSPACE(cmd)
		SKIPSPACE(cmd)
		/* addressing schemes */
	if (commands[cmdNum].numOfOp == 0)
		strcat(binCmd, "0000");
	if (commands[cmdNum].numOfOp == 1)
	{
		if (!oneOpCnvrt(binCmd, cmd, index))
			return 0;
	}
	if (commands[cmdNum].numOfOp == 2)
	{
		if (!twoOpCnvrt(binCmd, cmd, index))
			return 0;
	}
	strcat(binCmd, TWOZEROS);
	strcpy(instructions[index].binValue, binCmd);
	toBase8(instructions[index].binValue, instructions[index].base8Value, 15);
	return 1;
}

/* convert row number to base 8 */
void rowCnvrt(int index)
{
	char temp[16], temp8b[6];
	char* ptemp8b;

	tobin(temp, index + STARTADDRS);
	toBase8(temp, temp8b, 15);
	temp8b[5] = '\0';
	ptemp8b = temp8b + 2;
	strcpy(instructions[index].base8counter, ptemp8b);
}


/* goes thru table and converts cmds .data .string and addresses */
int secondPass()
{
	char *temp;
	int i;

	/* converts the cmds/data/string and addresses */
	for (i = 0; i < decimalAdrs - STARTADDRS; i++)
	{
		rowCnvrt(i);
		temp = instructions[i].name;
		SKIPSPACE(temp)
		if (*temp != 0)
		{
			if (strncmp(temp, labels[STRING], strlen(labels[STRING])) == 0)
			{
				insertChars(temp, i);
				continue;
			}
			if (strncmp(temp, labels[DATA], strlen(labels[DATA])) == 0)
			{
				insertInts(temp, i);
				continue;
			}
			if (!cmdCnvrt(temp, i))
			{
				fprintf(stderr, "%s\n%s", "label not found in line ", temp);
				return 0;
			}
		}
	}
	return 1;
}
