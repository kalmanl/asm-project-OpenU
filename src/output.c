#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "data.h"
#include "parse.h"
#include "output.h"

#define ENTEXSTRN ".ent"
#define EXTEXSTRN ".ext"
#define EXTOB ".op"


/* prints label places to file	*/
int PrintLabelToFile(int type, char * outputFile)
{
	FILE *fp;
	int i, j;
	char * label;

	if ((fp = fopen(outputFile, "w+")) == NULL)
		{
			fprintf(stderr, "error opening file\n");
			return 0;
		}

	for (i = 0; i < signCount; i++)
		if (signtable[i].type == type)
		{
			label = signtable[i].name;
			for (j = i+1; j < signCount; j++)
				if (strncmp(label, signtable[j].name, strlen(label)) == 0)
					fprintf(fp, "%s\t%s\n", signtable[j].name, instructions[signtable[j].DC - STARTADDRS].base8counter);
		}
	fclose(fp);
	return 1;
}

/*check if label type exists. if yes it will print to external file	*/
int labelExist(int type)
{
	int i;
	for (i = 0; i < signCount; i++)
		if (signtable[i].type == type)
			return 1;
	return 0;
}


/* checks if we need to make out files for entry or extern labels	*/
int checkForOutputLabels(int type, char * name)
{
	char * dotPlace;
	char outputFile[FILENAME_MAX];
	strcpy(outputFile, name);

	dotPlace = strchr(outputFile, '.');

	if (labelExist(type))
	{
		if (type == ENTRY)
			strcpy(dotPlace, ENTEXSTRN);
		else if (type == EXTERN)
			strcpy(dotPlace, EXTEXSTRN);
		PrintLabelToFile(type, outputFile);
		return 1;
	}
	return 0;
}

/* to do function to check if extern files to make 	*/
int checkForOutput(char * name)
{
	checkForOutputLabels(ENTRY, name);
	checkForOutputLabels(EXTERN, name);
	return 1;
}

/*print the asm binary commands to file	*/
int PrintInstructionsTables(FILE *fp)
{
	int i;
	for (i = 0; i < decimalAdrs-STARTADDRS; i++)
	{
		fprintf(fp, "%s\t%s\n", instructions[i].base8counter, instructions[i].base8Value);
	}
	return 1;
}

/*asm process to output file	*/
/*open file, make extension		*/
int printTofile(char * tempname)
{
	FILE *fp;
	char outputFile[FILENAME_MAX];
	char *dotPlace;

	strcpy(outputFile, tempname);
	dotPlace = strchr(outputFile, DOT);
	strcpy(dotPlace, EXTOB);

	if ((fp = fopen(outputFile, "w+")) == NULL)
	{
		fprintf(stderr, "error writing to file\n");
		return 0;
	}

	PrintInstructionsTables(fp);
	fclose(fp);

	checkForOutput(tempname);

	return 1;
}



