#ifndef PARSE   
#define PARSE     

/* 0 - false, 1 true continue, -1 error, stop	*/

#define BOTHREG -3
#define MAXLINE 80
#define LEBLEEND ':'
#define STRQUTS '"'
#define SPACE ' '
#define COMMA ','
#define DOT '.'
#define COMMENT ';'
#define ERROR -1
#define OPENBRACE '['
#define CLOSEBRACE ']'
#define DASH '-'

#define SKIPSPACE(ptr)			\
{								\
		while (isspace(*ptr))	\
		ptr++;					\
}

#define SKIPNONSPACE(ptr)						\
{												\
				while (*ptr&&!isspace(*ptr))	\
		ptr++;									\
}

#endif
