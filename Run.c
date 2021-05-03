#include "20161581.h"
int reg[15];
int cur_inst;
int loaded_program_len=0;
int program_start_address=0;
enum regnum{A=0,X=1,L=2,B=3,S=4,T=5,F=6,PC=8,SW=9};
enum opnum{
    TIXR=0xB8,COMPR=0xA0,CLEAR=0xB4,
    STL=0x14,LDB=0x68,LDA=0x00,COMP=0x28,JEQ=0x30,J=0x3C,STA=0x0C,LDT=0x74,
    TD=0xE0,RD=0xD8,STCH=0x54,JLT=0x38,STX=0x10,RSUB=0x4C,LDCH=0x50,WD=0xDC,
    JSUB=0x48};/*
void GetMnemonicByOpcode(int opcode) {
	for(int i=0;i<TABLE_SIZE;i++){
        OpcodeNode* curnode=HashTable[i];
        while (curnode != NULL) {									//	tour the list and return the opcode value
		if (curnode->opcode==opcode)
			printf("%s", curnode->mnemonic);
		curnode = curnode->next;
	    }
    }
}*/
void Run(){
    int err_flag;
    if(program_loaded==TRUE){
        if(newload==TRUE){
            loaded_program_len=reg[PC];
            program_start_address=reg[L];
            newload=FALSE;
        }
        while(reg[PC]<loaded_program_len+program_start_address){
            err_flag=RunInstruction(reg[PC]);
            if(err_flag<0){
                printf("Instruction execution error!\n");
                return ;
            }
            for(int i=0;i<break_point_num;i++)
                if(reg[PC]==breakpoint[i]){
                    printReg();
                    printf("    Stop at checkpoint[%X]\n",breakpoint[i]);
                    return;
                }
        }
        printReg();
        printf("    End program\n");
        Init_run(loaded_program_len,program_start_address);
    }
    else{
        printf("Program is not loaded\n");
        return;
    }
}
void printReg(){
    printf("A : %06X    X : %06X\n",reg[A], reg[X]);
    printf("L : %06X   PC : %06X\n",reg[L],reg[PC]);
    printf("B : %06X    S : %06X\n",reg[B],reg[S]);
    printf("T : %06X\n",reg[T]);
}
void Init_run(int program_start_address,int loaded_program_len){
    newload=TRUE;
    for(int i=0;i<14;i++)
        reg[i]=0;
    reg[PC]=program_start_address;
    reg[L]=loaded_program_len;
}
int RunInstruction(){
    int format;
    unsigned int val=0;
    unsigned int address=0;
    unsigned int opcode_ni=memory_arr[reg[PC]/16][reg[PC]%16];
    unsigned int ni=opcode_ni%4;
    unsigned int opcode=opcode_ni-ni;
    unsigned int second_byte=memory_arr[(reg[PC]+1)/16][(reg[PC]+1)%16];
    unsigned int xbpe=second_byte/16;
    switch(opcode){
        case TIXR:
        case COMPR:
        case CLEAR:
            format=2;
            break;
        default:
            if(xbpe&1)
                format=4;
            else 
                format=3;
            break;
    }
    cur_inst=reg[PC];
    //for(int i=0;i<format;i++)
        //printf("%02X ",memory_arr[(cur_inst+i)/16][(cur_inst+i)%16]);
    //printf("\n");
    if(opcode_ni==0){
        reg[PC]++;
        return 1;
    }
    reg[PC]+=format;
    if(format==2){
        unsigned int reg1=xbpe;
        unsigned int reg2=second_byte%16;
        switch(opcode){
            case TIXR:
            reg[X]++;
            SetSW(reg[X],reg[reg1]);
            break;
            case COMPR:
            SetSW(reg[reg1],reg[reg2]);
            break;
            case CLEAR:
            reg[reg1]=0;
            break;
        }
        //printReg();
        //GetMnemonicByOpcode(opcode);
        //printf(" reg1: %02X reg2 %02X format: %d\n\n",reg1, reg2,format);
        return 1;
    }
    else if(format==3){
        int disp=0;
        disp=((second_byte%16)<<8)|memory_arr[(cur_inst+2)/16][(cur_inst+2)%16];
        //printf("\nsecondbyte val: %X\n",second_byte);
        //printf("disp val: %06X\n",disp);
        address=0;
        switch(xbpe&14){
            case 0: address=(unsigned int)disp;
            break;

            case 0xA:
                address+=reg[X];
            case 2:
                if(disp&(1<<11))
                    disp-=(1<<12);
                address+=(reg[PC]+disp);
            break;

            case 0xC:
                address+=reg[X];
            case 4:
                address+=reg[B]+disp;
            break;
        }
        
        switch(ni){
            case 0:                 //std
                break;
            case 1:                 //immediate #
                val=address;
                break;
            case 2:                 //indirect @
                address=ReferencingMemory(address);
                val=ReferencingMemory(address);
                break;
            case 3:                 //simple
                val=ReferencingMemory(address);
                break;
        }
   
    }
    else{   //format 4
        address=second_byte%16;
        for(int i=2;i<4;i++){
            address=address<<8;
            address=address|memory_arr[(cur_inst+i)/16][(cur_inst+i)%16];
        }
        switch(ni){
            case 0:                 //std
                break;
            case 1:                 //immediate #
                val=address;
                break;
            case 2:                 //indirect @
                val=ReferencingMemory(address);
                val=ReferencingMemory(val);
                break;
            case 3:                 //simple
            if(opcode!=LDCH)
                val=ReferencingMemory(address);//////////////format 4
            break;
        }
    }
    switch(opcode){
        case STL:
        StoretoMemory(address,reg[L],3);
        break;
        case STA:
        StoretoMemory(address,reg[A],3);
        break;
        case STCH:
        StoretoMemory(address,reg[A]%256,1);
        break;
        case STX:
        StoretoMemory(address,reg[X],3);
        break;

        case LDB:
        reg[B]=val;
        break;

        case LDA:
        reg[A]=val;
        break;

        case COMP:
        SetSW(reg[A],val);
        break;

        case JEQ:
        if(reg[SW]=='=')
            reg[PC]=address;
        break;

        case J:
        reg[PC]=address;
        break;

        
        case LDT:
        reg[T]=val;
        break;

        case TD:
        reg[SW]='<';
        break;

        case RD:
        reg[A]=0;
        break;

        
        case JLT:
        if(reg[SW]=='<')
            reg[PC]=address;
        break;
        
        
        case RSUB:
        reg[PC]=reg[L];
        break;

        case LDCH:
        reg[A]=(reg[A]&0xffffff00)|memory_arr[address/16][address%16];
        break;

        case WD:
        break;

        case JSUB:
        reg[L]=reg[PC];
        reg[PC]=address;
        break;
        }
    //printReg();
    //GetMnemonicByOpcode(opcode);
    //printf(" val: %X address %X format: %d xbpe: %X\n\n",val, address,format,xbpe);
    return 1;
}
void SetSW(unsigned int reg1, unsigned int reg2){
    if(reg1<reg2)
        reg[SW]='<';
    else if(reg1==reg2)
        reg[SW]='=';
    else
        reg[SW]='>';
}
unsigned int ReferencingMemory(int address){
    unsigned int ret=0;
    for(int i=0;i<3;i++){
        ret=ret<<8;
        ret+=memory_arr[(address+i)/16][(address+i)%16];
    }
    return ret;
}
void StoretoMemory(int address, int value, int size){///
    for(int i=0;i<size;i++){
        memory_arr[(address+i)/16][(address+i)%16]=value/(1<<(8*(size-i-1)));
        value%=(1<<(8*(size-i-1)));
    }
}

