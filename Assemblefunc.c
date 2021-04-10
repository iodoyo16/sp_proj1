#include "20161581.h"
void AssembleFile(char argv[][ARGV_MAX_LEN]){
	int err_flag=0;
	int program_len;
	int start_address=0;
	LstNode* prev_lst;
	SymbolNode* prev_symtab;
	char program_name[ARGV_MAX_LEN];
	char base_name[ARGV_MAX_LEN];
	char obj_file_name[30];
	char lst_file_name[30];
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
	err_flag=PassOne(fp,&program_len,program_name,base_name,&start_address);
	fclose(fp);
	//printf("flag: %d\n",err_flag);
	if(err_flag<0){
		EraseLstList(LstList);
		EraseSymTab(SymbolList);
		PrintError(err_flag);
		LstList=prev_lst;
		SymbolList=prev_symtab;
		historypop();
		return;
	}
	err_flag=PassTwo(base_name);
	//printf("flag: %d\n",err_flag);
	if(err_flag<0){
		EraseLstList(LstList);
		EraseSymTab(SymbolList);
		PrintError(err_flag);
		LstList=prev_lst;
		SymbolList=prev_symtab;
		historypop();
		return;
	}
	else{
		EraseLstList(prev_lst);
		EraseSymTab(prev_symtab);
	}
	for(int i=0;i<strlen(argv[1]);i++){
		if(argv[1][i]=='.')break;
		obj_file_name[i]=argv[1][i];
		lst_file_name[i]=argv[1][i];
	}
	strcat(obj_file_name,".obj");
	strcat(lst_file_name,".lst");
	fp=fopen(lst_file_name,"w");
	WriteLstfile(fp,LstList);
	fclose(fp);
	fp=fopen(obj_file_name,"w");
	WriteObjectfile(fp,LstList,program_name,start_address,program_len);
	fclose(fp);
	printf("[%s], [%s]\n",lst_file_name,obj_file_name);
}
int PassOne(FILE* fp,int* program_len, char program_name[], char base_name[], int* staddr){
	//char program_name[ARGV_MAX_LEN];
	char input[INPUT_MAX_LEN];
	char asm_argv[ARGC_MAX][ARGV_MAX_LEN];
	int locctr=0;
	int start_address=0;
	int haslabel=FALSE;
	int	iscomment=FALSE;
	int inst_size,cnt=0;
	int asm_argc=0;
	int label_idx, mnemonic_idx, operand_idx;//operand could be many?
	while(fgets(input,INPUT_MAX_LEN,fp)!=NULL){
		cnt++;
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
				if(!MemoryAddressCheck(operand)){
					printf("Line_num : %d .",cnt);
					return MEMORY_ADDRESS_ERROR;///////////
				}
				start_address=locctr=strtol(operand,NULL,16);
				*staddr=start_address;
				AddLstNode(haslabel,iscomment,locctr,asm_argc,input,asm_argv,inst_size);
				if(haslabel){
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
					if(already_exist){
						printf("Line_num : %d. ",cnt);
						return DUPLICATE_SYMBOL_ERROR;
					}
				}
				if(iscomment)
					inst_size=0;
				else
					inst_size=InstructionMemorySize(mnemonic,operand);
				if(inst_size==WRONG_OPERAND||inst_size==WRONG_MNEMONIC){
					printf("Line_num : %d. ",cnt);
					return inst_size;
				}
				AddLstNode(haslabel,iscomment,locctr,asm_argc,input,asm_argv,inst_size);
			}
		}
		locctr+=inst_size;
	}
	return 0; //no error
}

int PassTwo(char base_name[]){
	SymbolNode* base_sym_node=FindSymbol(base_name);
	char mnemonic[ARGV_MAX_LEN]="";
	int base_locctr,cnt=0;

	if(base_sym_node!=NULL)
		base_locctr=base_sym_node->locctr;
	else {
		printf("Line_num : %d. ",cnt);
		return NO_BASE_ERROR;
	}

	LstNode* lst_reader=LstList;
	while(lst_reader!=NULL){
		if(lst_reader->next!=NULL)
			lst_reader=lst_reader->next;
		else
			break;
	}
	
	while(lst_reader!=NULL){
		int pc=lst_reader->locctr+lst_reader->format;
		long long object_code=0;
		cnt++;
		strcpy(mnemonic,lst_reader->mnemonic);
		OpcodeNode* opnode=GetOpcodeNodeByMnemonic(mnemonic);
		if(opnode==NULL&&strlen(mnemonic)!=0)
			opnode=GetOpcodeNodeByMnemonic(mnemonic+1);
		if(lst_reader->iscomment)
			lst_reader->object_code=-1;
		else if(strcmp("START",mnemonic)==0){
			lst_reader->object_code=-1;
		}
		else if(strcmp("END",mnemonic)==0){
			lst_reader->object_code=-1;
			break;
		}
		else if(opnode!=NULL){
			int nixbpe=0;
			int imm_val=-1;
			if(lst_reader->format==3||lst_reader->format==4)
				nixbpe=SetAddressingMode(lst_reader);
			int disp=0,pc_disp=0,base_disp=0;//disp=symbol-pc
			SymbolNode* symbol[10];
			for(int i=0;i<10;i++)
				symbol[i]=NULL;
			for(int i=0;i<lst_reader->operand_num;i++){
				symbol[i]=FindSymbol(lst_reader->operand[i]);
				if(symbol[i]==NULL){
					symbol[i]=FindSymbol((lst_reader->operand[i])+1);
					if(symbol[i]==NULL){
						//if(!decimal)return error;
						if(((nixbpe&SIMPLE_MODE)==IMMEDIATE_MODE)&&isDecimal((lst_reader->operand[i])+1))
							imm_val=atoi((lst_reader->operand[i])+1);
						else{
							printf("Line_num : %d. ",cnt);
							return WRONG_OPERAND;
						} 
					}
				}
			}
			if(lst_reader->format==3){
				object_code=opnode->opcode;				//
				object_code=object_code<<16;
				if(imm_val!=-1)
					disp=imm_val;
				else{
					if(symbol[0]!=NULL){
						pc_disp=(symbol[0]->locctr)-pc;
						base_disp=(symbol[0]->locctr)-base_locctr;
						if(-2048<=pc_disp&&pc_disp<=2047){				//pc relative
							if(pc_disp<0)
								pc_disp+=4096;
							disp=pc_disp;
							nixbpe=nixbpe|PC_MODE;
						}
						else if(0<=base_disp&&base_disp<=4095){
							disp=base_disp;
							nixbpe=nixbpe|BASE_MODE;
						}
						else if(0<=symbol[0]->locctr&&symbol[0]->locctr<=32767){
							nixbpe=nixbpe|STD_MODE;
							disp=symbol[0]->locctr;
						}			
						else{
							printf("Line_num : %d. ",cnt);
							return WRONG_FORMAT_ERROR;
						}
					}
					else disp=0;
				}
				object_code=object_code|(nixbpe<<12);
				lst_reader->object_code=object_code|disp;
			}
			else if(lst_reader->format==4){
				object_code=opnode->opcode;	//
				object_code=object_code<<24;
				if(imm_val!=-1)
					disp=imm_val;
				else{
					if(symbol[0]!=NULL)
						disp=symbol[0]->locctr;
					nixbpe=nixbpe|DIRECT_MODE;
				}
				object_code=object_code|(nixbpe<<20);
				lst_reader->object_code=object_code|disp;
			}
			else if(lst_reader->format==2){
				object_code=opnode->opcode;
				object_code=(object_code<<8);
				if(symbol[0]!=NULL)
					object_code=object_code|((symbol[0]->locctr)<<4);
				if(symbol[1]!=NULL)
					object_code=object_code|(symbol[1]->locctr);
				lst_reader->object_code=object_code;
			}
			else
				lst_reader->object_code=opnode->opcode;
		}
		else{
			//BYTE HAVE OBJECT_CODE
			if(strcmp(mnemonic,"BYTE")==0){
				if(lst_reader->operand[0]==NULL){
					printf("Line_num : %d. ",cnt);
					return WRONG_OPERAND;
				}
				if(lst_reader->operand[0][0]=='C'){
					int i=0;
					int len=strlen(lst_reader->operand[0]);
					while(i+2<len&&lst_reader->operand[0][2+i]!='\''){
						object_code=object_code<<8;// hexa 2 digit
						object_code=object_code|(lst_reader->operand)[0][2+i];
						//printf("byte const : %c\n",lst_reader->operand[0][2+i]);
						i++;
					}
					lst_reader->object_code=object_code;
				}
				else if(lst_reader->operand[0][0]=='X'){
					int i=0;
					int len=strlen(lst_reader->operand[0]);
					char str[ARGV_MAX_LEN]="";
					while(2+i<len&&lst_reader->operand[0][2+i]!='\''){
						str[i]=lst_reader->operand[0][2+i];
						i++;
					}
					lst_reader->object_code=strtol(str,NULL,16);
				}
				else{
					printf("Line_num : %d. ",cnt);
					return WRONG_OPERAND;
				}
			}
			else if(strcmp(mnemonic,"WORD")==0){
				if(lst_reader->operand[0]==NULL){
					printf("Line_num : %d. ",cnt);
					return WRONG_OPERAND;
				}
				lst_reader->object_code=atoi(lst_reader->operand[0]);
			}
			else
				lst_reader->object_code=-1;
		}
		lst_reader=lst_reader->prev;
	}
	return 0;
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
	else (newnode->label)[0]='\0';
	if(mnemonic_idx!=-1)strcpy(newnode->mnemonic,asm_argv[mnemonic_idx]);
	else (newnode->mnemonic)[0]='\0';
	for(int i=0;i<operandnum;i++)
		strcpy(newnode->operand[i],asm_argv[operand_idx+i]);
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
	SymbolNode* prev=NULL;
	SymbolNode* newnode=(SymbolNode*)malloc(sizeof(SymbolNode));
	if(newnode==NULL){
		printf("memory allocation error");
		exit(0);
	}
	newnode->locctr=locctr;
	strcpy(newnode->str,label);
	newnode->next=NULL;
	if(SymbolList==NULL){
		SymbolList=newnode;
		newnode->next=NULL;
	}
	else{
		for(SymbolNode* temp=SymbolList;temp!=NULL;temp=temp->next){
			if(strcmp(label,temp->str)==0){
				free(newnode);
				return 1;
			}
			else if(strcmp(label,temp->str)<0){
				if(prev!=NULL)
					prev->next=newnode;
				else SymbolList=newnode;
				newnode->next=temp;
				break;
			}
			prev=temp;
		}
		if(prev!=NULL)
			prev->next=newnode;
		else
			SymbolList=newnode;
	}
	return 0;
}
SymbolNode* FindSymbol(char label[]){
	for(SymbolNode* temp=SymbolList;temp!=NULL;temp=temp->next){
		if(strcmp(label,temp->str)==0)
			return temp;
	} 
	return NULL;
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
		printf("duplicate symbol or register name error\n");
			break;
		case MEMORY_ADDRESS_ERROR:
		printf("memory address error\n");
			break;
		case NO_BASE_ERROR:
		printf("no base error\n");
			break;
		case WRONG_FORMAT_ERROR:
		printf("wrong format error\n");
			break;
		case ERROR:
		printf("error\n");
			break;
		default:
		printf("error\n");
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
int SetAddressingMode(LstNode* lst_reader){
		int nixbpe=0;
		if(lst_reader->operand_num>1)
			if((lst_reader->operand)[1][0]=='X')
				nixbpe=(nixbpe|INDEX_MODE);			//index address x=1 else x=0

		if((lst_reader->operand)[0][0]=='#')
			nixbpe=(nixbpe|IMMEDIATE_MODE);			// immediate addressing
		else if((lst_reader->operand)[0][0]=='@')
			nixbpe=(nixbpe|INDIRECT_MODE);			// indirect addressing
		else nixbpe=(nixbpe|SIMPLE_MODE);			// simple addressing

		if(lst_reader->format==3)nixbpe=(nixbpe|FORMAT_3);
		else nixbpe=(nixbpe|FORMAT_4);
		//std sic(b,p,e is address field) n=0, i=0???????????????????
		return nixbpe;
}
void WriteLstfile(FILE* fp,LstNode* thislist){
	LstNode* lst_reader=thislist;
	while(lst_reader!=NULL){
		if(lst_reader->next!=NULL)
			lst_reader=lst_reader->next;
		else
			break;
	}
	int i=1;
	while(lst_reader!=NULL){
		fprintf(fp,"%-5d ",i*5);
		if(lst_reader->locctr<0||lst_reader->iscomment)
			fprintf(fp,"      ");
		else
			fprintf(fp,"%04X  ",lst_reader->locctr);
		fprintf(fp,"%s",lst_reader->str);
		if(lst_reader->object_code==-1)
			fprintf(fp,"\n");
		else{
			for(int j=strlen(lst_reader->str);j<30;j++)
				fprintf(fp," ");
			int size=(lst_reader->format)*2;
			if(size==2)fprintf(fp,"%02llX\n",lst_reader->object_code);
			else if(size==4)fprintf(fp,"%04llX\n",lst_reader->object_code);
			else if(size==6)fprintf(fp,"%06llX\n",lst_reader->object_code);
			else if(size==8)fprintf(fp,"%08llX\n",lst_reader->object_code);
			else fprintf(fp,"%llX\n",lst_reader->object_code);
		}
		lst_reader=lst_reader->prev;
		i++;
	}
}
void WriteObjectfile(FILE* fp,LstNode* thislist,char program_name[],int start_address,int program_len){
	LstNode* lst_reader=thislist;
	int start_inst=start_address+program_len;
	int modified[100];
	int modified_idx=0;
	while(lst_reader!=NULL){
		if(lst_reader->format==4){
			modified[modified_idx++]=(lst_reader->locctr)+1;
			if((lst_reader->operand)[0][0]=='#'&&isDecimal((lst_reader->operand)[0]+1))
				modified[--modified_idx]=0;
		}
		if(start_inst>(lst_reader->locctr)&&(lst_reader->object_code)!=-1)
			start_inst=lst_reader->locctr;
		if(lst_reader->next!=NULL)
			lst_reader=lst_reader->next;
		else
			break;
	}
	//printf("%s %X %X\n",program_name,start_address,program_len);
	
	fprintf(fp,"H%-6s%06X%06X\n",program_name,start_address,program_len);
	int size=0;
	while(lst_reader!=NULL){
		while(lst_reader!=NULL&&lst_reader->object_code==-1)
			lst_reader=lst_reader->prev;
		if(lst_reader==NULL)break;
		size=CountTextRecordSize(lst_reader);
		fprintf(fp,"T%06X%02X",lst_reader->locctr,size);
		while(lst_reader!=NULL){
			if(lst_reader->object_code!=-1){
				if(lst_reader->format==1)
					fprintf(fp,"%02llX",lst_reader->object_code);
				else if(lst_reader->format==2)
					fprintf(fp,"%04llX",lst_reader->object_code);
				else if(lst_reader->format==3)
					fprintf(fp,"%06llX",lst_reader->object_code);
				else if(lst_reader->format==4)
					fprintf(fp,"%08llX",lst_reader->object_code);
				size-=lst_reader->format;
			}
			lst_reader=lst_reader->prev;
			if(size==0)break;
		}
		fprintf(fp,"\n");
	}
	for(int i=1;i<=modified_idx;i++)
		fprintf(fp,"M%06X%02X\n",modified[modified_idx-i],5);
	fprintf(fp,"E%06X\n",start_inst);
}
int CountTextRecordSize(LstNode* head){
	LstNode* temp=head;
	int size=0;
	int varsize=0;
	while(temp!=NULL){
		if(size+temp->format<=30){
			size+=temp->format;
			if(temp->object_code==-1){
				varsize+=temp->format;
				if(temp->format!=0)
					return size-varsize;
			}
		}
		else return size-varsize;
		temp=temp->prev;
	}
	return size-varsize;
}
int isDecimal(char str[]){
	int len = strlen(str);
	if(len==0)return FALSE;
	for (int i = 0; i < len; i++) {
		if ('0' <= str[i] && str[i] <= '9')
			continue;
		return FALSE;
	}
	return TRUE;
}
void PrintSymbol(){
	for(SymbolNode* temp=SymbolList;temp!=NULL;temp=temp->next){
		if(isReg(temp->str))
			continue;
		printf("\t%s\t%04X\n",temp->str,temp->locctr);
	}
}
int isReg(char str[]){
	if(strcmp(str,"A")==0&&strlen(str)==1)return TRUE;
	if(strcmp(str,"X")==0&&strlen(str)==1)return TRUE;
	if(strcmp(str,"L")==0&&strlen(str)==1)return TRUE;
	if(strcmp(str,"PC")==0&&strlen(str)==2)return TRUE;
	if(strcmp(str,"SW")==0&&strlen(str)==2)return TRUE;
	if(strcmp(str,"B")==0&&strlen(str)==1)return TRUE;
	if(strcmp(str,"S")==0&&strlen(str)==1)return TRUE;
	if(strcmp(str,"T")==0&&strlen(str)==1)return TRUE;
	if(strcmp(str,"F")==0&&strlen(str)==1)return TRUE;
	return FALSE;
}
