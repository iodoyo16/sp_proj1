#include "20161581.h"

int HashFunction(char mnemonic[]) {				// Hash OpcodeNode
	int hash=201;
	for (int i = 0; mnemonic[i] != '\0'; i++)
		hash = (hash<<3)^ mnemonic[i];
	return hash % TABLE_SIZE;
}

void ConstructOpcodeTable() {									//	Read opcode from opcode.txt and Make Hash Table
	FILE* fp = fopen("opcode.txt", "r");
	char input[OPCODE_FILE_INPUT_MAX];							//	readBuffer
	char opargv[ARGC_MAX][MNEMONIC_LEN];						//	opargv[0] opcode / [1] mnemonic / [2] argc1 / [ [3] argc2 ]
	int argc,idx;
	if (fp == NULL) {											//	handling file open error
		printf("File does not exist\n");
		return;
	}
	while (fgets(input, OPCODE_FILE_INPUT_MAX, fp) != NULL) {			//	read file
		argc=Parser(input, opargv, " /\t\n");							//	argc = number of argument
		OpcodeNode* newnode = (OpcodeNode*)malloc(sizeof(OpcodeNode));
		if (newnode == NULL) {											//	handle memory allocation error
			printf("Memory allocation error!\n");
			exit(1);
		}
		newnode->opcode = strtol(opargv[0],NULL,16);			//	Change opcode string to hexa value and save
		strcpy(newnode->mnemonic, opargv[1]);					//	Save mnemonic and argument number
		newnode->format[0] = atoi(opargv[2]);					
		if (argc == 4)newnode->format[1] = atoi(opargv[3]);	//	if there is another possible argc save it also
		else newnode->format[1] = -1;
		idx=HashFunction(newnode->mnemonic);					//	Get hash value
		newnode->next = HashTable[idx];							//	linke to hash table
		HashTable[idx] = newnode;
	}
	fclose(fp);
}
OpcodeNode* GetOpcodeNodeByMnemonic(char mnemonic[]) {
	OpcodeNode* curnode = HashTable[HashFunction(mnemonic)];	//	get idx of given mnemonic
	while (curnode != NULL) {									//	tour the list and return the opcode value
		if (strcmp(curnode->mnemonic, mnemonic) == 0)
			return curnode;
		curnode = curnode->next;
	}
	return NULL;													// there is no such mnemonic
}
void OpcodeList() {
	for (int i = 0; i < TABLE_SIZE; i++) {								// tour all idx of hash table
		printf("%d : ",i);
		OpcodeNode* curnode = HashTable[i];
		while (curnode != NULL) {										
			printf("[%s,%X]", curnode->mnemonic, curnode->opcode);		//	print each list
			if (curnode->next != NULL)
				printf(" -> ");
			curnode = curnode->next;
		}
		printf("\n");
	}
}
int Parser(char input[], char argv[][MNEMONIC_LEN], char* delimit) {
	char* token;
	int argc = 0;
	token = strtok(input, delimit);										//	split the input by delimit(' \t/')
	while (token != NULL) {
		strcpy(argv[argc], token);										//	save the arg and count how many
		argc++;
		token = strtok(NULL, delimit);
	}
	return argc;
}
