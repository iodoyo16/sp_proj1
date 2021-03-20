#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<sys/stat.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
//GENERAL USE
#define TRUE 1
#define FALSE 0
// input, argv, argc, instruction, cmd MAX len OR num
#define INPUT_MAX_LEN 100
#define ARGV_MAX_LEN 100
#define ARGC_MAX 100
#define INSTRUCTION_LEN 100
#define CMD_LEN 100
#define CMD_NUM 100
#define MNEMONIC_LEN 100
#define DIRNAME_LEN 20
// INPUT CASE num
#define VALID_CMD 0
#define DONE 1
#define INPUT_ERROR 2
#define PROMPT 3
// CMD CASE num
#define HELP_NUM 1
#define DIR_NUM 2
#define QUIT_NUM 3
#define HISTORY_NUM 4
#define DUMP_NUM 5
#define EDIT_NUM 6
#define FILL_NUM 7
#define RESET_NUM 8
#define OPCODE_NUM 9
#define OPCODELIST_NUM 10
//Memory
#define MAX_MEMORY_INDEX 1048575 // 65536*16-1
#define MIN_VALUE 32 //0x20
#define MAX_VALUE 126 // 0x7e
#define DEFAULT_DUMP_LINE 10
#define BYTE_PER_LINE 16
//OPCODE
#define TABLE_SIZE 20
extern char arrCmd[CMD_NUM][CMD_LEN];
extern char arrCmdPrint[CMD_NUM][CMD_LEN];
extern int arrCmdNum[CMD_NUM];
typedef struct _OpcodeNode {
	int opcode;
	char mnemonic[100];//////////////////////
	int arg_num[2];
	struct _OpcodeNode* next;
}OpcodeNode;
typedef struct _CmdNode {
	int cmd_num;
	char str[CMD_LEN];
	struct _CmdNode* next;
}CmdNode;
typedef struct _CmdHistoryNode {
	int num;
	char str[CMD_LEN];
	struct _CmdHistoryNode* next;
}CmdHistoryNode;
CmdNode* CmdList;
CmdHistoryNode* CmdHistoryList;
OpcodeNode* HashTable[TABLE_SIZE];
int last_idx;
// function
int Parser(char input[], char argv[][ARGV_MAX_LEN], char* delimit);
int CmdParser(char input[], char argv[][ARGV_MAX_LEN], char* delimit);
void trim(char* s,char* e, char* str);
void InitCmdList();// cmd를 linked list로 초기화
int InputArgsCheck(char argv[][ARGV_MAX_LEN],int argc,int cmdcase);
void CmdArgsCat(int argc, char argv[][ARGV_MAX_LEN], char str[]);
int InputCategorize(char argv[][ARGV_MAX_LEN], int argc, int* cmdcase);
void FuncExecute(int cmdcase, char argv[][ARGV_MAX_LEN],int argc);


void Help();
void Dir();
void Quit();
void WriteHistory(char str[]);
void PrintHistory(CmdHistoryNode*);
void Dump(int argc, char argv[][ARGV_MAX_LEN]);
void Edit(char argv[][ARGV_MAX_LEN]);
void Fill(char argv[][ARGV_MAX_LEN]);
void Reset();

int MemoryAddressCheck(char str[]);
int MemoryAddressRangeCheck(char argv[][ARGV_MAX_LEN], int argc);
void PrintMemory(int s, int e);
void PrintLnMemory(int line_num, int start_pos, int end_pos);
int isHex(char str[]);

void ConstructOpcodeTable();
int HashFunction(char mnemonic[]);
int GetOpcodeByMnemonic(char mnemonic[]);
void OpcodeList();
