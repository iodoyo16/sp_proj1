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
	{"assemble"},
	{"type"},
	{"symbol"},
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
	{"assemble filename"},
	{"type filename"},
	{"symbol"},
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
	ASSEMBLE_NUM,
	TYPE_NUM,
	SYMBOL_NUM,
	0
};
void InitCmdList() {
	for (int i = 0; arrCmd[i][0] != 0; i++) {
		CmdNode* newnode = (CmdNode*)malloc(sizeof(CmdNode));	
		if (newnode == NULL) {									//	memory allocation error handle
			printf("Memory allocation error!\n");
			exit(1);
		}
		strcpy(newnode->str, arrCmd[i]);						//	Init shell cmd list (cmd string, cmd num)
		newnode->cmd_num = arrCmdNum[i];
		newnode->next = CmdList;
		CmdList = newnode;
	}
}
char* Strtrim(char* str){
	char* s=str;
	char* e=str+strlen(str)-1;				//	end of this string
	char* ret=s;
	for(int i=0;s+i<=e;i++){				//	left trim
		if(*(s+i)==' '||*(s+i)=='\t')
			*(s+i)='\0';
		else{
			ret=s+i;
			break;
		}
	}
	for(int i=0;s<=e-i;i++){				//	right trim
		if(*(e-i)==' '||*(e-i)=='\t')
			*(e-i)='\0';
		else
			break;
	}
	return ret;
}
void trim(char* start, char* end, char* ptr){
	for(int i=1;start<=ptr-i;i++){
		if(*(ptr-i)==' '||*(ptr-i)=='\t')			//	trim left side of ptr
			*(ptr-i)='\0';
		else
			break;
	}
	for(int i=1;ptr+i<=end;i++){
		if(*(ptr+i)==' '||*(ptr+i)=='\t')			//	trim right side of ptr
			*(ptr+i)='\0';
		else 
			break;
	}
}


int TrimAroundComma(char parsed_str[],char* start_ptr){
	char* ptr;
	char* end_ptr;
	char* commaptr[ARGC_MAX];
	int parsed_len,comma_num=0;
	int len=strlen(start_ptr);
	end_ptr=start_ptr+len-1;
	ptr=strchr(start_ptr,',');						//	find location of comma
	while(ptr!=NULL){
		commaptr[comma_num++]=ptr;					//	save
		ptr=strchr(ptr+1,',');
	}
	for(int i=0;i<comma_num;i++){					//	trim every comma
		trim(start_ptr,end_ptr,commaptr[i]);
	}
	int idx=0;
	for(int i=0;i<len;i++){							//	copy string except space around comma
		if(start_ptr[i]=='\0')
			continue;
		parsed_str[idx++]=start_ptr[i];
	}
	parsed_str[idx]='\0';
	parsed_len=strlen(parsed_str);
	return parsed_len;
}


int SplitArgs(char parsed_str[],int parsed_len,char argv[][ARGV_MAX_LEN]){
	char* token;
	char* start_ptr;
	char* end_ptr;
	int argc=0;
	token = strtok(parsed_str," \t");
	if(token==NULL)
		return argc;
	strcpy(argv[0],token);						//	save CMD part(cut by space and tab)
	argc++;
	start_ptr=strtok(NULL,"\0");				//	rest of parsed str
	if(start_ptr==NULL)
		return argc;
	end_ptr=strchr(start_ptr,',');				//	find comma and split
	while(end_ptr!=NULL){
		*end_ptr='\0';
		if(start_ptr==end_ptr){					//	comma is consecutive -> wrong input
			argc=-1;
			return argc;
		}
		strcpy(argv[argc++],start_ptr);			//	save argv
		start_ptr=end_ptr+1;
		if(start_ptr>parsed_str+parsed_len-1){	//	end of str is comma -> wrong input
			return -1;
		}
		end_ptr=strchr(start_ptr,',');
	}
	if(*start_ptr!='\0')
		strcpy(argv[argc++],start_ptr);			//	save last arg
	return argc;
}
int CmdParser(char input[], char argv[][ARGV_MAX_LEN], char* delimit) {
	char* ptr;
	int parsed_len,argc = 0;
	char parsed_str[INPUT_MAX_LEN];
	if(strlen(input)==0)return 0;
	ptr = Strtrim(input);							//	trim front and back of str
	if(strlen(ptr)==0)return 0;
	parsed_len = TrimAroundComma(parsed_str,ptr);	//	trim front and back of comma
	argc=SplitArgs(parsed_str,parsed_len,argv);		//	split and save arg
	return argc;
}
int InputCategorize(char argv[][ARGV_MAX_LEN], int argc, int* cmdcase) {
	char str[INPUT_MAX_LEN];
	if(argc==-1)return INPUT_ERROR;													//	wrong input
	if (argc == 0)return PROMPT;													//	no input
	for (CmdNode* curnode = CmdList; curnode != NULL; curnode = curnode->next) {	
		if (strcmp(curnode->str, argv[0]) == 0) {									//	find match cmd
			*cmdcase = curnode->cmd_num;
			if (InputArgsCheck(argv, argc, *cmdcase)==DONE) {						//	check argument is appropriate for cmd
				CmdArgsCat(argc, argv, str);										//	make appropriate format for history
				WriteHistory(str);
				return VALID_CMD;
			}
			else																	//	wrong arg
				return INPUT_ERROR;
		}
	}
	return INPUT_ERROR;																//	no match cmd
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
	if (newnode == NULL) {								//	history node allocation error handle
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
	if (!isHex(str))return FALSE;										//	hexadecimal check
	if (strtol(str, NULL, 16) > MAX_MEMORY_INDEX)return FALSE;			//	boundary check
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
	OpcodeNode* opnode;
	char filename[ARGV_MAX_LEN]="./";
	switch (cmdcase) {
	case HELP_NUM:											//	no need other arg
	case DIR_NUM:
	case QUIT_NUM:
	case HISTORY_NUM:
	case RESET_NUM:
	case OPCODELIST_NUM:
	case SYMBOL_NUM:
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
			else if (!isHex(argv[2]) || strtol(argv[2], NULL, 16) > 255)		//	memory value available check
				status = INPUT_ERROR;
			else
				status = DONE;
		}
		break;
	case FILL_NUM:
		if (argc != 4)status = INPUT_ERROR;
		else {
			status = MemoryAddressRangeCheck(argv, argc - 1);
			if (!isHex(argv[3]) || strtol(argv[3], NULL, 16) > 255)				//	memory value available check
				status = INPUT_ERROR;
		}
		break;
	case OPCODE_NUM:
		if (argc != 2)return INPUT_ERROR;
		else {
			opnode=GetOpcodeNodeByMnemonic(argv[1]);
			if (opnode == NULL)status=INPUT_ERROR;
			else {
				printf("opcode is %X\n", opnode->opcode);
				status = DONE;
			}
		}
		break;
	case ASSEMBLE_NUM:
	case TYPE_NUM:
		if(argc!=2)return INPUT_ERROR;
		else{
			strcat(filename, argv[1]);
			if(access(filename,F_OK | R_OK)!=0){
				//printf("%s\n",filename);
				status=INPUT_ERROR;
			}
		}
		break;
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
	case ASSEMBLE_NUM:
		AssembleFile(argv);
		break;
	case TYPE_NUM:
		TypeFile(argv);
		break;
	case SYMBOL_NUM:
		break;
	}
}
