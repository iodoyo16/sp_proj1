#include "20161581.h"
void AssembleFile(char argv[][ARGV_MAX_LEN]){
	int err_flag=0;
	int program_len;
	LstNode* prev_lst;
	SymbolNode* prev_symtab;
	char program_name[ARGV_MAX_LEN];
	char base_name[ARGV_MAX_LEN];
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
	InitSymTab();
	//printf("before\n");
	//for(SymbolNode* temp=SymbolList;temp!=NULL;temp=temp->next)
	//	printf("%04X %s\n",temp->locctr,temp->str);
	err_flag=PassOne(fp,&program_len,program_name,base_name);
	printf("flag: %d\n",err_flag);
	if(err_flag<0){
		EraseLstList(LstList);
		EraseSymTab(SymbolList);
		PrintError(err_flag);
		LstList=prev_lst;
		SymbolList=prev_symtab;
	}
	else{
		EraseLstList(prev_lst);
		EraseSymTab(prev_symtab);
	}
	//printf("after\n");
	//for(SymbolNode* temp=SymbolList;temp!=NULL;temp=temp->next)
	//	printf("%04X %s\n",temp->locctr,temp->str);
	//for(LstNode* temp=LstList;temp!=NULL;temp=temp->next)
	//	printf("%04X %10s %10s %10s\n",temp->locctr,temp->label,temp->mnemonic,temp->operand);
	//err_flag=PassTwo(base_name);
//	WriteLstfile();
//	WriteObjectfile();
}
int PassOne(FILE* fp,int* program_len, char program_name[], char base_name[]){
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
		inst_size=0;
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
		asm_argc=Parser(temp,asm_argv,", \f\r\t\v\n");
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
				AddLstNode(haslabel,iscomment,locctr,asm_argc,input,asm_argv,inst_size);
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
				AddLstNode(haslabel,iscomment,-1,asm_argc,input,asm_argv,inst_size);
				break;
			}
			else if(strcmp(mnemonic,"BASE")==0){
				inst_size=0;
				if(haslabel){
					//checkvalidlabel();
					InsertSymbol(label,-1);
				}
				//do something base do
				strcpy(base_name,operand);
				AddLstNode(haslabel,iscomment,-1,asm_argc,input,asm_argv,inst_size);
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
				AddLstNode(haslabel,iscomment,locctr,asm_argc,input,asm_argv,inst_size);
			}
		}
		locctr+=inst_size;
	}
	return 0; //no error
}

int PassTwo(char base_name[]){
	SymbolNode* base_sym_node=(FindSymbol(base_name));
	int base_locctr=base_sym_node->locctr;

	LstNode* lst_reader=LstList;
	while(lst_reader!=NULL){
		if(lst_reader->next!=NULL)
			lst_reader=lst_reader->next;
		else
			break;
	}
	while(lst_reader!=NULL){
		char mnemonic[ARGV_MAX_LEN];
		strcpy(mnemonic,lst_reader->mnemonic);
		int pc=lst_reader->locctr+lst_reader->format;
		if(strcmp("START",mnemonic)==0){
			//write headline
		}
		else if(strcmp("END",mnemonic)==0){
			//write endrecord
			break;
		}
	//	else if(assembler directives){
	//		BYTE HAVE OBJECT_CODE
	//		object_code=-1
	//	}
		else{
			if(lst_reader->format==3||lst_reader->format==4){
				int nixbpe=0;
			if(lst_reader->operand_num>1)
				if((lst_reader->operand)[1][0]=='X')
					nixbpe=nixbpe|INDEX_MODE;			//index address x=1 else x=0

			if((lst_reader->operand)[0][0]=='#')
				nixbpe=nixbpe|IMMEDIATE_MODE;			// immediate addressing
			else if((lst_reader->operand)[0][0]=='@')
				nixbpe=nixbpe|INDIRECT_MODE;			// indirect addressing
			else nixbpe=nixbpe|SIMPLE_MODE;				// simple addressing
			//pc relative disp(signed)// calculate memory range
			//base disp(unsigned)
			//direct disp(12bit)

			//std sic(b,p,e is address field) n=0, i=0
			}
			
			OpcodeNode* opnode=GetOpcodeNodeByMnemonic(mnemonic);

			int disp,pc_disp,base_disp;//disp=symbol-pc
			SymbolNode* symbol=FindSymbol(lst_reader->operand[0]);

			if(lst_reader->format==3){
				lst_reader->object_code=(opnode->opcode)<<18;				// nixbpe(6) + disp(12)
				pc_disp=(symbol->locctr)-pc;
				base_disp=(lst_symbol->locctr)-base_locctr;
				if(-2048=<pc_disp&&pc_disp<=2047){				//pc relative
					if(pc_disp<0)
						pc_disp+=4096;
					disp=pc_disp;
				}
				else if(0<=base_disp&&base_disp<=4095)
					disp=base_disp;
				else
					return ERROR;
				lst_reader->object_code=(lst_reader->object_code)|nixbpe<<12;
				lst_reader->object_code=(lst_reader->object_code)|disp;
			}
			else if(lst_reader->format==4){
				lst_reader->object_code=(opnode->opcode)<<26;	// address 20 + nixbpe(6)
				disp=symbol->locctr;
				lst_reader->object_code=(lst_reader->object_code)|nixbpe<<20;
				lst_reader->object_code=(lst_reader->object_code)|disp;
			}
			else if(lst_reader->format==2){

			}
			else{
				lst_reader->opcode=opnode->opcode;
			}
		}
	}
	
}

void AddLstNode(int haslabel,int iscomment,int locctr,int argc,char str[],char asm_argv[][ARGV_MAX_LEN],int inst_size){
	int operandnum;/////////////////
	int label_idx;
	int operand_idx;
	int mnemonic_idx;

	LstNode* newnode=(LstNode*)malloc(sizeof(LstNode));
	if(newnode==NULL){
		printf("memory allocation error");
		exit(0);
	}
	operandnum=argc-1;
	if(haslabel){
		operandnum--;
		label_idx=0;
		mnemonic_idx=1;
		operand_idx=2;
	}
	else if(iscomment){
		label_idx=operand_idx=mnemonic_idx=-1;
		operandnum=0;
	}
	else{
		label_idx=-1;
		mnemonic_idx=0;
		operand_idx=1;
	}
	strcpy(newnode->str,str);
	newnode->haslabel=haslabel;
	newnode->iscomment=iscomment;
	newnode->locctr=locctr;
	newnode->operand_num=operandnum;
	newnode->format=inst_size;
	if(label_idx!=-1)strcpy(newnode->label,asm_argv[label_idx]);
	if(mnemonic_idx!=-1)strcpy(newnode->mnemonic,asm_argv[mnemonic_idx]);
	for(int i=0;i<operandnum;i++)
		strcpy(newnode->operand,asm_argv[operand_idx+i]);
	newnode->next=LstList;
	if(LstList!=NULL)
		LstList->prev=newnode;
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
			return 1;//already exist
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
SymbolNode* FindSymbol(char label[]){
	for(SymbolNode* temp=SymbolList;temp!=NULL;temp=temp->next)
		if(strcmp(label,temp->str)==0)
			return temp;
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
	SymbolNode* tmp=thislist;
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