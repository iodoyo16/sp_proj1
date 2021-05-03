#pragma once
#include<stdio.h>
#include<sys/stat.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<unistd.h>
//////////////////////////////////
//		MACRO
////////////////////////////////
//GENERAL USE //
#define TRUE 1
#define FALSE 0
// input, argv, argc, instruction, cmd MAX len OR num
#define INPUT_MAX_LEN 100		// Max length of standard input
#define ARGV_MAX_LEN 100		// Max length of argument
#define ARGC_MAX 100			// Max number of Argument
#define CMD_LEN 100				// Max length of CMD
#define CMD_NUM 100				// MAX number of CMD
#define DIRNAME_LEN 20			// Directory print format size
#define BREAK_POINT_MAX 20
#define MAX_LINK_FILE 10
#define MAX_EXTERN_SYM 7
#define OBJ_MAX_LEN 100
// INPUT CASE num
#define VALID_CMD 0				// Valid input case for CMD
#define DONE 1					// Process Done
#define INPUT_ERROR 2			// WRONG INPUT
#define PROMPT 3				// Return to Prompt
// CMD
#define HELP_NUM 1				// CMD matching case
#define DIR_NUM 2
#define QUIT_NUM 3
#define HISTORY_NUM 4
#define DUMP_NUM 5
#define EDIT_NUM 6
#define FILL_NUM 7
#define RESET_NUM 8
#define OPCODE_NUM 9
#define OPCODELIST_NUM 10
#define ASSEMBLE_NUM 11
#define TYPE_NUM 12
#define SYMBOL_NUM 13
#define PROGADDR_NUM 14
#define LOADER_NUM 15
#define BP_NUM 16
#define RUN_NUM 17
//Memory
#define MAX_MEMORY_INDEX 1048575	// 65536*16-1
#define MIN_VALUE 32				// 0x20
#define MAX_VALUE 126				// 0x7e
#define DEFAULT_DUMP_LINE 10		// Default number of line to print for dump CMD
#define BYTE_PER_LINE 16			// Number of index per Line
//OPCODE
#define TABLE_SIZE 20				// OPCODE HASH TABLE SIZE
#define OPCODE_FILE_INPUT_MAX 100	// Max length of line, opcode.txt file read
#define MNEMONIC_LEN 100			// MAX length of MNEMONIC string
//error
#define WRONG_MNEMONIC -5
#define WRONG_OPERAND -6
#define DUPLICATE_SYMBOL_ERROR -7
#define MEMORY_ADDRESS_ERROR -8
#define NO_BASE_ERROR -9
#define WRONG_FORMAT_ERROR -10

#define ERROR -20

#define PC_MODE 2			//pc relative 000'01'0 = 2
#define BASE_MODE 4			//base relative 000'10'0 = 4
#define DIRECT_MODE 0		//direct relative 000'00'0 = 0

#define INDEX_MODE 8		//index mode 00'1'000 =8

#define IMMEDIATE_MODE 16	//immediate '01'0000=16
#define INDIRECT_MODE 32	//indirect '10'0000=32
#define SIMPLE_MODE 48		//simple '11'0000=48
#define STD_MODE 0			//standard sic '00'0000 *bpe is address field

#define FORMAT_3 0			// format 3 00000'0'
#define FORMAT_4 1			// format 4 00000'1'



extern char arrCmd[CMD_NUM][CMD_LEN];			// SHELL cmd
extern char arrCmdPrint[CMD_NUM][CMD_LEN];		// HELP print string
extern int arrCmdNum[CMD_NUM];					//  CMD NUM matches with CMD string
extern unsigned char memory_arr[65536][16];
//extern char assemblerDirective[CMD_NUM][CMD_LEN];
////////////////////////////////////////////////////
//		LINKED LIST
///////////////////////////////////////////////////

typedef struct _OpcodeNode {		//	Opcode Hash Table  linked list node
	int opcode;						//	opcode (hexademical)
	char mnemonic[MNEMONIC_LEN];	//	mnemonic string
	int format[2];					//	format
	struct _OpcodeNode* next;
}OpcodeNode;

typedef struct _CmdNode {			//	CMD List
	int cmd_num;					//  cmd_num match with CMD string
	char str[CMD_LEN];				//	CMD string
	struct _CmdNode* next;
}CmdNode;

typedef struct _CmdHistoryNode{		// CMD History List
	int num;						// nth history
	char str[CMD_LEN];				// CMD string
	struct _CmdHistoryNode* next;
}CmdHistoryNode;
typedef struct _LstNode{
	int haslabel;
	int iscomment;
	int operand_num;
	int locctr;
	int format;
	char label[ARGV_MAX_LEN];
	char mnemonic[ARGV_MAX_LEN];
	char operand[2][ARGV_MAX_LEN];
	long long object_code;
	char str[INPUT_MAX_LEN];
	struct _LstNode* next;
	struct _LstNode* prev;
}LstNode;
typedef struct _SymbolNode{
	int locctr;
	char str[MNEMONIC_LEN];
	struct _SymbolNode* next;
}SymbolNode;

typedef struct _EstabNode{
	int address;
	char symbol_name[7];
	struct _EstabNode* next;
}EstabNode;

CmdNode* CmdList;					//	CMD List headptr
CmdHistoryNode* CmdHistoryList;		//	CMD History List headptr
OpcodeNode* HashTable[TABLE_SIZE];	//	Opcode Hash Table
LstNode* LstList;
SymbolNode* SymbolList;

int progaddr;
int execaddr;
int break_point_num;
int breakpoint[BREAK_POINT_MAX];
EstabNode* Estab[TABLE_SIZE];
int control_section_len[TABLE_SIZE];
//int loaded_program_len;
int newload;

int program_loaded;
/////////////////////////////////////////
//	CommandParse.c function
////////////////////////////////////////
int CmdParser(char input[], char argv[][ARGV_MAX_LEN], char* delimit);		//	Split INPUT to Cmd and Arguments
void StrTrim(char* str);													//	Delete Space front and back of string
void trim(char* s,char* e, char* str);										//	Delete Space front and back of a character
int TrimAroundComma(char parsed_str[], char* start_ptr);					//	Trim Around Comma in string
int SplitArgs(char parsed_str[],int parsed_len,char argv[][ARGV_MAX_LEN]);	//	Split Parsed string to Cmd and Arguments
void InitCmdList();															//	Init Shell command Linked List
int InputArgsCheck(char argv[][ARGV_MAX_LEN],int argc,int cmdcase);			//	Check Argument is appropriate to CMD
void CmdArgsCat(int argc, char argv[][ARGV_MAX_LEN], char str[]);			//	Make appropriate format for History
int InputCategorize(char argv[][ARGV_MAX_LEN], int argc, int* cmdcase);		//	Classify the Input case and Save history when it is Available
void FuncExecute(int cmdcase, char argv[][ARGV_MAX_LEN],int argc);			//	Execute function for matching CMD
int isHex(char str[]);														//	Check it is hexademical numb

//MEMORY
int last_idx;														//	last dump ends index
int MemoryAddressCheck(char str[]);									//	Check it is right memory addres
int MemoryAddressRangeCheck(char argv[][ARGV_MAX_LEN], int argc);	//	Check the range of memory address
void PrintMemory(int s, int e);										//	Print Memory value in range
void PrintLnMemory(int line_num, int start_pos, int end_pos);		//	Print Memory value in line

/////////////////////////////////////////
//	Commandfunc.c function
/////////////////////////////////////////
void Help();										//	help CMD function
void Dir();											//	dir CMD functioin
void Quit();										//	Exit program
void WriteHistory(char str[]);						//	Save History to Linked List
void PrintHistory(CmdHistoryNode*);					//	history CMD function
void Dump(int argc, char argv[][ARGV_MAX_LEN]);		//	dump CMD function
void Edit(char argv[][ARGV_MAX_LEN]);				//	edit CMD function
void Fill(char argv[][ARGV_MAX_LEN]);				//	fill CMD function
void Reset();										//	reset CMD function
void BP(int argc, char argv[][ARGV_MAX_LEN]);		//  set or clear or print break points

//////////////////////////
//	OperationCodeTable.c
/////////////////////////
int Parser(char input[], char argv[][MNEMONIC_LEN], char* delimit);		//	Read opcode,mnemonic, argument_num from opcode.txt and parse them
void ConstructOpcodeTable();											//	Make Opcode Hash Table 
int HashFunction(char mnemonic[]);										//	Hashing fuction
OpcodeNode* GetOpcodeNodeByMnemonic(char mnemonic[]);								//	Find opcode that matches mnemonic
void OpcodeList();														//	Pring Opcode Hash Table Linked List


void TypeFile(char argv[][ARGV_MAX_LEN]);				// type file
void AssembleFile(char argv[][ARGV_MAX_LEN]);			// pass1 pass2 writelst writeobj
int PassOne(FILE* fp,int* program_len, char program_name[],char base_name[],int* start_address); // set locctr and construct symtab
int PassTwo(char base_name[]);																	// set object code
void AddLstNode(int haslabel, int iscomment,int locctr,int argc,char str[],char asm_argv[][ARGV_MAX_LEN],int inst_size);//inser node to lstlist
int InstructionMemorySize(char mnemonic[],char operand[]);		// caculate instruction size
int InsertSymbol(char label[],int locctr);
void InitSymTab();							// insert register name in symtab
void PrintError(int flag);					// print error line depends on err_flag value
void EraseSymTab(SymbolNode* thislist);		// delete symbol table and free memory
void EraseLstList(LstNode* thislist);		// delete LstList and free memory
SymbolNode* FindSymbol(char label[]);		// get matched symbol 
int SetAddressingMode(LstNode* lst_reader);		// set n,i,x,e of nixbpe 
void WriteLstfile(FILE* fp,LstNode* thislist);
void WriteObjectfile(FILE* fp,LstNode* thislist,char program_name[],int start_address,int program_len);
int CountTextRecordSize(LstNode* head); // count size of objectcode of this text records line
int isDecimal(char str[]);
void PrintSymbol();
int isReg(char str[]);			// check this string is matched with register name
void historypop();				// pop recent history



void Loader(int argc, char argv[][ARGV_MAX_LEN]);
int LoadPassOne(FILE* fp_arr[],int filenum);
int LoadPassTwo(FILE* fp_arr[],int filenum);
void ExtractSymbolName(char symbol_name[], char input[],int maxlen);
void AddExternalSymbol(int file_i, char symbol_name[], int address);
void printEstab(int file_num);
void EraseEstab(int file_num);
int ModifyAddress(int address,int ref_address,char operation);
EstabNode* getEstabNodebySymbol(char symbol_name[],int filenum);

void Init_run(int program_start_address, int loaded_program_len);
void printReg();
int RunInstruction();
void Run();
void SetSW(unsigned int reg1, unsigned int reg2);
unsigned int ReferencingMemory(int address);
void StoretoMemory(int address, int value, int size);