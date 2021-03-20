#include "20161581.h"
int HashFunction(char mnemonic[]) {
	int hash=201;
	for (int i = 0; i < mnemonic[i] != '\0'; i++)
		hash = (hash<<3)^ mnemonic[i];
	return hash % TABLE_SIZE;
}
void ConstructOpcodeTable() {
	FILE* fp = fopen("opcode.txt", "r");
	char input[100];
	char opargv[100][100];
	int argc,idx;
	if (fp == NULL) {
		printf("File does not exist\n");
		return;
	}
	while (fgets(input, MNEMONIC_LEN, fp) != NULL) {
		argc=Parser(input, opargv, " /\t\n");
		OpcodeNode* newnode = (OpcodeNode*)malloc(sizeof(OpcodeNode));
		if (newnode == NULL) {
			printf("Memory allocation error!\n");
			exit(1);
		}
		//opargv[0] opcode / [1] mnemonic / [2] argc1 / [ [3] argc2 ]
		newnode->opcode = strtol(opargv[0],NULL,16);
		strcpy(newnode->mnemonic, opargv[1]);
		newnode->arg_num[0] = atoi(opargv[2]);
		if (argc == 4)newnode->arg_num[1] = atoi(opargv[3]);
		else newnode->arg_num[1] = -1;
		idx=HashFunction(newnode->mnemonic);
		newnode->next = HashTable[idx];
		HashTable[idx] = newnode;
	}
	fclose(fp);
}
int GetOpcodeByMnemonic(char mnemonic[]) {
	OpcodeNode* curnode = HashTable[HashFunction(mnemonic)];
	while (curnode != NULL) {
		if (strcmp(curnode->mnemonic, mnemonic) == 0)
			return curnode->opcode;
		curnode = curnode->next;
	}
	return -1;
}
void OpcodeList() {
	for (int i = 0; i < TABLE_SIZE; i++) {
		printf("%d : ",i);
		OpcodeNode* curnode = HashTable[i];
		while (curnode != NULL) {
			printf("[%s,%X]", curnode->mnemonic, curnode->opcode);
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
	token = strtok(input, delimit);
	while (token != NULL) {
		strcpy(argv[argc], token);
		argc++;
		token = strtok(NULL, delimit);
	}
	return argc;
}