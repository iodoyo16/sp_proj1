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
		gets_s(input, sizeof(input));////���� ���� �÷ο� �����ϼ���
		argc = StrParser(input, argv, ", \t");//////////////////// variable ���� ,�θ� �����ϴ��� ���ο� ���� �����ʿ�
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