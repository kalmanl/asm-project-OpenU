#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "checkLine.h"
#include "parse.h"
#include "data.h"
#include "first.h"

#define OPTNS 3

/*reads line from file asm input. max line 80*/
int getline(FILE *fp, char s[], int lim)
{
	int c, i;
	i = 0;
	while (--lim > 0 && (c = getc(fp)) != EOF && c != '\n')
		s[i++] = c;
	if (c == '\n')           
		s[i++] = c;
	s[i] = '\0';
	return i;
}

/*read asm file line by line and call functions to processes the lines*/
int readfile(FILE *fp)
{
	char line[MAXLINE], *pline;
	int result, i;
	int foundError = 0;
	int(*options[]) (char *) = { checkIfEnteryExstOrEnt, checkIfLabel, checkCmd };

	while (getline(fp,line,MAXLINE))
	{
		pline = line;
		SKIPSPACE(pline)

		/*comment in asm 	*/
		if (!*pline || *pline == '\n' || *pline == COMMENT)
			continue;
		
		for (i = 0; i < OPTNS; i++)
		{
			result = (*options[i])(pline);
			if (result) /*found the type of input*/
				break;
		}
		/*didn't find right type of line or error in line*/
		if (result == -1 || result == 0)  
		{
			foundError = 1;
			fprintf(stderr, "%s %s", "error: at line:", pline);
		}
	}
	if (foundError)
		return 0;
	return 1;
}
