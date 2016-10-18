#ifndef DATAFILE
#define DATAFILE

#define NUMCMD 16
#define NUMREG 8
#define STARTADDRS 100
#define MAXROW 1000

extern int decimalAdrs;
extern int signCount;
extern char *labels[];

/*for checking labels	*/
enum labelType { EXTERN = 0, ENTRY, DATA, STRING, INTRN, REFRNC};

/*in use for commands array	*/
enum cmdValue
{
	MOV = 0, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP
};

struct cmd
{
	int id;
	const char * name;
	int numOfOp;
	char opcode[5];
};
struct Signstable
{
	char name[30];
	int DC;
	char DCBase8[4];
	int type;
};
struct Convert
{
	char bin[4];
	char base8;
};
struct Instructions
{
	char name[80];
	int counter;
	char binValue[16];
	char base8Value[6];
	char base8counter[6];
};


extern struct Instructions instructions[];
extern struct  Signstable signtable[];
extern struct cmd commands[];

int clearTables();
int addToSignTable(char * , int );
void PrintInstructionsTable();
int addToInstructionsTable(char * );

#endif


