#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "first.h"
#include "output.h"
#include "second.h"
#include "data.h"

/*opens asm files for reading by argv input*/
int readAsmFile(int argc, char *argv[])
{
	FILE *fp;
	char extname[] = ".as";
	char * dotPlace = NULL;
	char tempname[FILENAME_MAX];

	if (argc == 1)
		printf("no input file names args\n");
	else
		while (--argc > 0)
		{
			/*look if file has a extension name*/
			strcpy(tempname, *++argv);
			dotPlace = strrchr(tempname, '.');
			if (!dotPlace)
				strcat(tempname, extname);
			if ((fp = fopen(tempname, "r")) == NULL)
			{
				fprintf(stderr, "file %s not found\n", tempname);
				fprintf(stderr, "error opening file\n");
				continue;
			}
			if (readfile(fp) && secondPass())
				printTofile(tempname);
						
			clearTables();
		}
	return 1;
}

int main(int argc, char *argv[])
{
	readAsmFile(argc, argv);
	return 0;
}
