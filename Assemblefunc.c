#include "20161581.h"
void AssembleFile(char argv[][ARGV_MAX_LEN]){
	int err_flag=0;
	int program_len;
	LstNode* prev_lst;
	SymbolNode* prev_symtab;
	char program_name[ARGV_MAX_LEN];
	FILE *fp = fopen(argv[1],"r");
	//printf("%s\n",argv[1]);
	if(fp==NULL){
		printf("fileopen error\n");
		return ;
	}

	prev_lst=LstList;
	prev_symtab=SymbolList;
	LstList=NULL;
	SymbolList=NULL;

	err_flag=PassOne(fp,&program_len,program_name);
	printf("flag: %d\n",err_flag);
	if(err_flag<0){
		EraseLstList(LstList);
		EraseSymTab(SymbolList);
		PrintError(err_flag);
		LstList=prev_lst;
		SymbolList=prev_symtab;
		return;
	}
	else{
		EraseLstList(prev_lst);
		EraseSymTab(prev_symtab);
	}
	//for(SymbolNode* temp=SymbolList;temp!=NULL;temp=temp->next)
		//printf("%04X %s\n",temp->locctr,temp->str);
	//for(LstNode* temp=LstList;temp!=NULL;temp=temp->next)
		//printf("%04X %10s %10s %10s\n",temp->locctr,temp->label,temp->mnemonic,temp->operand);
	err_flag=PassTwo();
//	WriteLstfile();
//	WriteObjectfile();
}
int PassOne(FILE* fp,int* program_len, char program_name[]){
	//char program_name[ARGV_MAX_LEN];
	char input[INPUT_MAX_LEN];
	char asm_argv[ARGC_MAX][ARGV_MAX_LEN];
	int locctr=0;
	int start_address=0;
	int haslabel=FALSE;
	int	iscomment=FALSE;
	int inst_size;
	int asm_argc=0;
	int label_idx, mnemonic_idx, operand_idx;//operand could be many?
	while(fgets(input,INPUT_MAX_LEN,fp)!=NULL){
		if(strlen(input)!=0)
			input[strlen(input)-1]='\0';

		if(strlen(input)==0)continue;//emptyline

		if(input[0]==' '||input[0]=='\t'){
			haslabel=FALSE;
			iscomment=FALSE;
			label_idx=-1;
			mnemonic_idx=0;
			operand_idx=1;
		}
		else if(input[0]=='.'){
			haslabel=FALSE;
			iscomment=TRUE;
			label_idx=mnemonic_idx=operand_idx=-1;
		}
		else{
			haslabel=TRUE;
			iscomment=FALSE;
			label_idx=0;
			mnemonic_idx=1;
			operand_idx=2;
		}
		char temp[INPUT_MAX_LEN];
		strcpy(temp,input);//strtok in Parser changes original string
		asm_argc=Parser(temp,asm_argv," \f\r\t\v\n");
///////parsing/////

		if(asm_argc==1)operand_idx=-1;
		if(asm_argc!=0){
			char mnemonic[ARGV_MAX_LEN]="";
			char label[ARGV_MAX_LEN]="";
			char operand[ARGV_MAX_LEN]="";
			if(haslabel)strcpy(label,asm_argv[label_idx]);
			if(mnemonic_idx!=-1)strcpy(mnemonic,asm_argv[mnemonic_idx]);
			if(operand_idx!=-1)strcpy(operand,asm_argv[operand_idx]);
		
			if(strcmp("START",mnemonic)==0){
				inst_size=0;
				if(!MemoryAddressCheck(operand))
					return MEMORY_ADDRESS_ERROR;///////////
				start_address=locctr=strtol(operand,NULL,16);
				AddLstNode(haslabel,iscomment,locctr,asm_argc,input,label,mnemonic,operand,inst_size);
				if(haslabel){
					int already_exist=InsertSymbol(label,locctr);//program name insert
					if(already_exist)
						return DUPLICATE_SYMBOL_ERROR;
					strcpy(program_name,label);
				}
			}
			else if(strcmp(mnemonic,"END")==0){
				inst_size=0;
				if(haslabel){
					//checkvalidlabel();
					InsertSymbol(label,locctr);
				}
				*program_len = locctr - start_address;
				AddLstNode(haslabel,iscomment,-1,asm_argc,input,label,mnemonic,operand,inst_size);
				break;
			}
			else if(strcmp(mnemonic,"BASE")==0){
				inst_size=0;
				if(haslabel){
					//checkvalidlabel();
					InsertSymbol(label,-1);
				}
				//do something base do
				AddLstNode(haslabel,iscomment,-1,asm_argc,input,label,mnemonic,operand,inst_size);
			}
			else{
				if(haslabel){
					//checkvalidlabel()
					int already_exist=InsertSymbol(label,locctr);
					if(already_exist)
						return DUPLICATE_SYMBOL_ERROR;
				}
				if(iscomment)
					inst_size=0;
				else
					inst_size=InstructionMemorySize(mnemonic,operand);
				if(inst_size==WRONG_OPERAND||inst_size==WRONG_MNEMONIC)
					return inst_size;
				AddLstNode(haslabel,iscomment,locctr,asm_argc,input,label,mnemonic,operand,inst_size);
			}
		}
		locctr+=inst_size;
	}
	return 0; //no error
}

int PassTwo(){
	LstNode* lst_reader=LstList;
	while(lst_reader!=NULL){
		if(lst_reader->next!=NULL)
			lst_reader=lst_reader->next;
		else
			break;
	}
	while(lst_reader!=NULL){
		int pc=lst_reader->locctr+lst_reader->format;
		int disp;
		if(strcmp("START",lst_reader->mnemonic)==0){
			//write headline
		}
		else if(strcmp("END",lst_reader->mnemonic)==0){
			//write endrecord
			break;
		}
		else{

		}
	}
	
}

void AddLstNode(int haslabel,int iscomment,int locctr,int argc,char str[],char label[],char mnemonic[],char operand[],int inst_size){
	int operandnum;/////////////////

	LstNode* newnode=(LstNode*)malloc(sizeof(LstNode));
	if(newnode==NULL){
		printf("memory allocation error");
		exit(0);
	}
	operandnum=argc-1;
	if(haslabel)operandnum--;
	if(iscomment)operandnum=0;

	strcpy(newnode->str,str);
	newnode->haslabel=haslabel;
	newnode->iscomment=iscomment;
	newnode->locctr=locctr;
	newnode->operand_num=operandnum;
	newnode->format=inst_size;
	strcpy(newnode->mnemonic,mnemonic);
	strcpy(newnode->label,label);
	strcpy(newnode->operand,operand);
	newnode->next=LstList;
	if(Lstlist!=NULL)
		Lstlist->prev=newnode;
	LstList=newnode;
	//printf("%X %10s %10s %10s\n",newnode->locctr,newnode->label,newnode->mnemonic,newnode->operand);
}
int InstructionMemorySize(char mnemonic[],char operand[]){
	int inst_size=0;
	OpcodeNode* opnode;
	int plusflag=0;			
	if(mnemonic[0]=='+'){
		plusflag=1;
		inst_size=4;
	}

	opnode=GetOpcodeNodeByMnemonic((mnemonic+plusflag));// 'jsub' '+jsub'
	if(opnode==NULL){
		if(strcmp(mnemonic,"WORD")==0)inst_size=3;
		else if(strcmp(mnemonic,"RESB")==0)inst_size=atoi(operand);
		else if(strcmp(mnemonic,"RESW")==0)inst_size=3*atoi(operand);
		else if(strcmp(mnemonic,"BYTE")==0){
			if(operand[0]=='C')
				inst_size=strlen(operand)-3;
			else if(operand[0]=='X')
				inst_size=1;
			else
				return WRONG_OPERAND;
		}
		else return WRONG_MNEMONIC;
	}
	else
		if(!plusflag)
			inst_size=opnode->format[0];

	//printf("inst_size : %d\n",inst_size);
	return inst_size;
}
int InsertSymbol(char label[],int locctr){
	for(SymbolNode* temp=SymbolList;temp!=NULL;temp=temp->next){
		if(strcmp(label,temp->str)==0)
			return 1;
	}
	SymbolNode* newnode=(SymbolNode*)malloc(sizeof(SymbolNode));
	if(newnode==NULL){
		printf("memory allocation error");
		exit(0);
	}
	newnode->locctr=locctr;
	strcpy(newnode->str,label);
	newnode->next=SymbolList;
	SymbolList=newnode;
	return 0;
}

void EraseLstList(LstNode* thislist){
	LstNode* tmp=thislist;
	while(tmp!=NULL){
		LstNode* nxttmp=tmp->next;
		free(tmp);
		tmp=nxttmp;
	}
}
void EraseSymTab(SymbolNode* thislist){
	SymbolNode* tmp=thisList;
	while(tmp!=NULL){
		SymbolNode* nxttmp=tmp->next;
		free(tmp);
		tmp=nxttmp;
	}
}
void PrintError(int flag){
	switch(flag){
		case WRONG_MNEMONIC:
		printf("wrong mnemonic error\n");
			break;
		case WRONG_OPERAND:
		printf("wrong operand error\n");
			break;
		case DUPLICATE_SYMBOL_ERROR:
		printf("duplicate symbol error\n");
			break;
		//case
		//case
	}
}
void InitSymTab(){
	InsertSymbol("A",0);
	InsertSymbol("X",1);
	InsertSymbol("L",2);
	InsertSymbol("PC",8);
	InsertSymbol("SW",9);

	InsertSymbol("B",3);
	InsertSymbol("S",4);
	InsertSymbol("T",5);
	InsertSymbol("F",6);
}