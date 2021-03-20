#include "20161581.h"
char arrCmd[CMD_NUM][CMD_LEN] = {
	{"h"}, {"help"},
	{"d"}, {"dir"},
	{"q"}, {"quit"},
	{"hi"}, {"history"},
	{"du"}, {"dump"},
	{"e"}, {"edit"},
	{"f"}, {"fill"},
	{"reset"},
	{"opcode"},
	{"opcodelist"},
	0,
};
char arrCmdPrint[CMD_NUM][CMD_LEN] = {
	{"h[elp]"},
	{"d[ir]"},
	{"q[uit]"},
	{"hi[story]"},
	{"du[mp] [start, end]"},
	{"e[dit] address, value"},
	{"f[ill] start, end, value"},
	{"reset"},
	{"opcode mnemonic"},
	{"opcodelist"},
	0
};
int arrCmdNum[CMD_NUM] = {
	HELP_NUM , HELP_NUM ,
	DIR_NUM , DIR_NUM ,
	QUIT_NUM ,QUIT_NUM ,
	HISTORY_NUM ,HISTORY_NUM ,
	DUMP_NUM ,DUMP_NUM ,
	EDIT_NUM ,EDIT_NUM ,
	FILL_NUM ,FILL_NUM ,
	RESET_NUM ,
	OPCODE_NUM ,
	OPCODELIST_NUM ,
	0
};
void InitCmdList() {// cmd를 linked list로 초기화
	for (int i = 0; arrCmd[i][0] != 0; i++) {////빈문자열체크 반복....
		CmdNode* newnode = (CmdNode*)malloc(sizeof(CmdNode));
		if (newnode == NULL) {
			printf("Memory allocation error!\n");
			exit(1);
		}
		strcpy(newnode->str, arrCmd[i]);
		newnode->cmd_num = arrCmdNum[i];
		newnode->next = CmdList;
		CmdList = newnode;
	}
}
int StrParser(char input[], char argv[][ARGV_MAX_LEN], char* delimit) {
	char* token;
	int argc = 0;
	token = strtok(input, delimit);
	while (token != NULL) {
		strcpy(argv[argc], token);
		argc++;
		token = strtok(NULL, delimit);
	}
	return argc;
}
int InputCategorize(char argv[][ARGV_MAX_LEN], int argc, int* cmdcase) {
	char str[INPUT_MAX_LEN];
	if (argc == 0)return PROMPT;// 아무런 입력 x
	for (CmdNode* curnode = CmdList; curnode != NULL; curnode = curnode->next) {
		if (strcmp(curnode->str, argv[0]) == 0) {
			*cmdcase = curnode->cmd_num;
			if (InputArgsCheck(argv, argc, *cmdcase)==DONE) {
				CmdArgsCat(argc, argv, str);
				WriteHistory(str);
				return VALID_CMD;
			}
			else
				return INPUT_ERROR;
		}
	}
	return INPUT_ERROR;
}
void CmdArgsCat(int argc,char argv[][ARGV_MAX_LEN],char str[]) {
	strcpy(str, argv[0]);
	if (argc > 1) {
		strcat(str, " ");
		for (int i = 1; i < argc; i++) {
			strcat(str, argv[i]);
			if (i == argc - 1)continue;
			strcat(str, ", ");
		}
	}
}
void WriteHistory(char str[]) {
	CmdHistoryNode* newnode = (CmdHistoryNode*)malloc(sizeof(CmdHistoryNode));
	if (newnode == NULL) {
		printf("Memory allocation error!\n");
		exit(1);
	}
	strcpy(newnode->str, str);
	if (CmdHistoryList == NULL)
		newnode->num = 1;
	else
		newnode->num = CmdHistoryList->num + 1;
	newnode->next = CmdHistoryList;
	CmdHistoryList = newnode;
}
int isHex(char str[]) {
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (('a' <= str[i] && str[i] <= 'f')|| ('0' <= str[i] && str[i] <= '9')|| ('A' <= str[i] && str[i] <= 'F'))
			continue;
		return FALSE;
	}
	return TRUE;
}
int MemoryAddressCheck(char str[]) {
	if (!isHex(str))return FALSE;
	if (strtol(str, NULL, 16) > MAX_MEMORY_INDEX)return FALSE;
	return TRUE;
}
int MemoryAddressRangeCheck(char argv[][ARGV_MAX_LEN],int argc) {
	for (int i = 1; i < argc; i++) {
		if (MemoryAddressCheck(argv[i]))
			continue;
		return INPUT_ERROR;
	}
	if (argc == 3)
		if(strtol(argv[1], NULL, 16)>strtol(argv[2],NULL,16))
			return INPUT_ERROR;
	return DONE;
}
int InputArgsCheck(char argv[][ARGV_MAX_LEN], int argc, int cmdcase) {
	int status = DONE;
	int opcode;
	switch (cmdcase) {
	case HELP_NUM:
	case DIR_NUM:
	case QUIT_NUM:
	case HISTORY_NUM:
	case RESET_NUM:
	case OPCODELIST_NUM:
		if (argc == 1)status = DONE;
		else status = INPUT_ERROR;
		break;
	case DUMP_NUM:
		if (0 < argc && argc <= 3) {
			status = MemoryAddressRangeCheck(argv, argc);
		}
		else status = INPUT_ERROR;
		break;
	case EDIT_NUM:
		if (argc != 3)status = INPUT_ERROR;
		else {
			if (!MemoryAddressCheck(argv[1]))
				status = INPUT_ERROR;
			else if (!isHex(argv[2]) || strtol(argv[2], NULL, 16) > 255)
				status = INPUT_ERROR;
			else
				status = DONE;
		}
		break;
	case FILL_NUM:
		if (argc != 4)status = INPUT_ERROR;
		else {
			status = MemoryAddressRangeCheck(argv, argc - 1);
			if (!isHex(argv[3]) || strtol(argv[3], NULL, 16) > 255)
				status = INPUT_ERROR;
		}
		break;
	case OPCODE_NUM:
		if (argc != 2)return INPUT_ERROR;
		else {
			opcode=GetOpcodeByMnemonic(argv[1]);
			if (opcode == -1)status=INPUT_ERROR;
			else {
				printf("opcode is %X\n", opcode);
				status = DONE;
			}
		}
	}
	return status;
}
void FuncExecute(int cmdcase, char argv[][ARGV_MAX_LEN],int argc) {
	switch (cmdcase) {
	case HELP_NUM:
		Help();
		break;
	//case DIR_NUM:
		//PrintDir();
		//break;
	case QUIT_NUM:
		Quit();
		break;
	case HISTORY_NUM:
		PrintHistory(CmdHistoryList);
		break;
	case DUMP_NUM:
		Dump(argc, argv);
		break;
	case EDIT_NUM:
		Edit(argv);
		break;
	case FILL_NUM:
		Fill(argv);
		break;
	case RESET_NUM:
		Reset();
		break;
	case OPCODE_NUM:
//		OpcodeMnemonic();
		break;
	case OPCODELIST_NUM:
		OpcodeList();
		break;
	}
}