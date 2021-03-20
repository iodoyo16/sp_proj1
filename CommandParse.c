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
	{0},
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
	{0},
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
void InitCmdList() {// cmd�� linked list�� �ʱ�ȭ
	for (int i = 0; arrCmd[i][0] != 0; i++) {////���ڿ�üũ �ݺ�....
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
char* Strtrim(char* str){
	char* s=str;
	char* e=str+strlen(str)-1;
	char* ret;
	for(int i=0;s+i<=e;i++){
		if(*(s+i)==' '||*(s+i)=='\t')
			*(s+i)='\0';
		else{
			ret=s+i;
			break;
		}
	}
	for(int i=0;s<=e-i;i++){
		if(*(e-i)==' '||*(e-i)=='\t')
			*(e-i)='\0';
		else
			break;
	}
	return ret;
}
void trim(char* start, char* end, char* ptr){
	for(int i=1;start<=ptr-i;i++){
		if(*(ptr-i)==' '||*(ptr-i)=='\t')
			*(ptr-i)='\0';
		else
			break;
	}
	for(int i=1;ptr+i<=end;i++){
		if(*(ptr+i)==' '||*(ptr+i)=='\t')
			*(ptr+i)='\0';
		else 
			break;
	}
}
int CmdParser(char input[], char argv[][ARGV_MAX_LEN], char* delimit) {
	char* token;
	char* start_ptr;
	char* end_ptr;
	char* ptr;
	int len;
	int argc = 0;
	char parsed_str[INPUT_MAX_LEN];
	char* commaptr[ARGC_MAX];
	int comma_num=0;
	start_ptr=Strtrim(input);/////////////////����
	len=strlen(start_ptr);
	end_ptr=start_ptr+len-1;
	ptr=strchr(start_ptr,',');
	while(ptr!=NULL){
		commaptr[comma_num++] = ptr;	
		ptr=strchr(ptr+1,',');
	}
	for(int i=0;i<comma_num;i++){
		trim(start_ptr,end_ptr,commaptr[i]);
	}
	int idx=0;
	for(int i=0;i<len;i++){
		if(start_ptr[i]=='\0')
			continue;
		parsed_str[idx++]=start_ptr[i];
		if(start_ptr[i]==' ')
			printf("*");
		else if(start_ptr[i]=='\t')
			printf("****");
		else
			printf("%c",start_ptr[i]);
	}
	parsed_str[idx]='\0';
	token = strtok(parsed_str,delimit);
	strcpy(argv[0],token);
	argc++;
	start_ptr=strtok(NULL,"\0");
	end_ptr=strchr(start_ptr,',');
	while(end_ptr!=NULL){
		*ptr='\0';
		if(start_ptr==end_ptr){
			argc=-1;
			return argc;
		}
		strcpy(argv[argc],start_ptr);
		argc++;
		start_ptr=end_ptr+1;
		end_ptr=strchr(start_ptr,',');
	}
	return argc;
}
int InputCategorize(char argv[][ARGV_MAX_LEN], int argc, int* cmdcase) {
	char str[INPUT_MAX_LEN];
	if(argc==-1)return INPUT_ERROR;
	if (argc == 0)return PROMPT;// �ƹ��� �Է� x
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
	case DIR_NUM:
		Dir();
		break;
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