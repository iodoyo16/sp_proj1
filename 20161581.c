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
		fgets(input, INPUT_MAX_LEN,stdin);////���� ���� �÷ο� �����ϼ���
		input[strlen(input)-1]='\0';
		argc = CmdParser(input, argv, " \t");//////////////////// variable ���� ,�θ� �����ϴ��� ���ο� ���� �����ʿ�
		input_case = InputCategorize(argv, argc, &cmdcase);
		switch (input_case) {
		case INPUT_ERROR:
			printf("WRONG INPUT!!\n");
			break;
		case PROMPT://�ƹ��Է� x
			break;
		case VALID_CMD:
			FuncExecute(cmdcase, argv, argc);
			break;
		}
	}
}
