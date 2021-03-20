#include "20161581.h"
int main() {
	char input[INPUT_MAX_LEN];
	char argv[ARGC_MAX][ARGV_MAX_LEN];
	int argc=0;
	int cmdcase;
	int input_case;
	InitCmdList();
	ConstructOpcodeTable();
	last_idx = -1;////////////////////////////////////////
	while (1) {
		printf("sicsim> ");
		fgets(input, INPUT_MAX_LEN,stdin);////버퍼 오버 플로우 조심하세요
		input[strlen(input)-1]='\0';
		argc = CmdParser(input, argv, " \t");//////////////////// variable 끼리 ,로만 구분하는지 여부에 따라 수정필요
		input_case = InputCategorize(argv, argc, &cmdcase);
		switch (input_case) {
		case INPUT_ERROR:
			printf("WRONG INPUT!!\n");
			break;
		case PROMPT://아무입력 x
			break;
		case VALID_CMD:
			FuncExecute(cmdcase, argv, argc);
			break;
		}
	}
}
