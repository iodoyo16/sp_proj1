#include "20161581.h"
int main() {
	char input[INPUT_MAX_LEN];
	char argv[ARGC_MAX][ARGV_MAX_LEN];
	int argc=0;
	int cmdcase;
	int input_case;
	
	InitCmdList();					// Init Shell Command
	ConstructOpcodeTable();			// Read opcode.txt and Construct Hash Table
	last_idx = -1;
	LstList=NULL;
	SymbolList=NULL;
	progaddr=0;
	execaddr=0;
	break_point_num=0;
	program_loaded=FALSE;

	while (1) {
		printf("sicsim> ");
		fgets(input, INPUT_MAX_LEN,stdin);		//	fgets get '\n'
		input[strlen(input)-1]='\0';			//	So switch it to '\0'
		fflush(stdin);
		
		argc = CmdParser(input, argv, " \t");	//	Parsing input
		if(strcmp(argv[0],"loader")==0)
			argc=Parser(input,argv," \t");
		input_case = InputCategorize(argv, argc, &cmdcase);		//	VerifyCmd and Arguments and Classify the case
		switch (input_case) {
		case INPUT_ERROR:						//	Wrong input
			printf("Input Error!!\n");
			break;
		case PROMPT:							//	Processing is done so Return to Prompt
			break;
		case VALID_CMD:							//	Valid CMD and Argument so call matching function
			FuncExecute(cmdcase, argv, argc);
			break;
		}
		
	}
}
