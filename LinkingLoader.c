#include "20161581.h"

extern unsigned char memory_arr[65536][16];

void Loader(int argc, char argv[][ARGV_MAX_LEN]){
    int err_flag;
    int loaded_program_len;
    FILE* fp_arr[10];
    for(int i=1;i<argc;i++){                    // store filepointer of given .obj file
        fp_arr[i-1]=fopen(argv[i],"r");
        if(fp_arr[i-1]==NULL){
            printf("File open error\n");
            historypop();
            return ;
        }
    }
    err_flag=LoadPassOne(fp_arr,argc-1);        // Passone
    if(err_flag<0){
        EraseEstab(argc-1);                     // fail to passone, then erase estab and pop history
        historypop();
        return;
    }
    
    for(int i=1;i<argc;i++){                    // reopen given .obj file
        fclose(fp_arr[i-1]);
        fp_arr[i-1]=fopen(argv[i],"r");
        if(fp_arr[i-1]==NULL){
            printf("File open error\n");
            historypop();
            return ;
        }
    }
    err_flag=LoadPassTwo(fp_arr,argc-1);        //Passtwo
    if(err_flag<0){
        EraseEstab(argc-1);
        historypop();
        return;
    }
    printEstab(argc-1);
    EraseEstab(argc-1);
    program_loaded=TRUE;
    loaded_program_len=0;
    for(int i=0;i<argc-1;i++)
        loaded_program_len+=control_section_len[i];
    Init_run(progaddr,loaded_program_len);
}
int LoadPassOne(FILE* fp_arr[],int filenum){
    int csaddr=progaddr;
    char symbol_name[7];
    char object_file_input[OBJ_MAX_LEN];
    for(int file_i=0;file_i<filenum;file_i++){

        while(fgets(object_file_input,OBJ_MAX_LEN,fp_arr[file_i])!=NULL){
            char tmp[7];
            if(object_file_input[0]=='H'){          // store cs name, program len and csaddr
                ExtractSymbolName(symbol_name,object_file_input+1,6);
                control_section_len[file_i]=strtol(object_file_input+13,NULL,16);        
                AddExternalSymbol(file_i,symbol_name,csaddr);
                continue;
            }
            else if(object_file_input[0]!='D')
                continue;
            int input_len=strlen(object_file_input);    // external define part
            if(input_len!=0){
                object_file_input[input_len-1]='\0';
                input_len--;
            }
            for(int i=1;i<input_len;i+=12){             // add external define to estab
                ExtractSymbolName(symbol_name,object_file_input+i,6);
                strncpy(tmp,object_file_input+i+6,6);
                int locctr=strtol(tmp,NULL,16);
                AddExternalSymbol(file_i,symbol_name,csaddr+locctr);
            }
        }
        csaddr+=control_section_len[file_i];
    }
    return 1;
}
int LoadPassTwo(FILE* fp_arr[], int filenum){
    int csaddr=progaddr;
    int err_flag=0;
    char object_file_input[OBJ_MAX_LEN];
    for(int file_i=0;file_i<filenum;file_i++){
        int ref_num_arr[MAX_EXTERN_SYM];
        ref_num_arr[1]=Estab[file_i]->address;              // ref num of csec name == 01
        //printf("\n%dth file csaddr: %X\n\n",file_i,csaddr);s
        while(fgets(object_file_input,OBJ_MAX_LEN,fp_arr[file_i])!=NULL){
            int input_len=strlen(object_file_input);
            if(input_len!=0)
                object_file_input[input_len-1]='\0';
            input_len--;
            char record_letter=object_file_input[0];
            if(record_letter=='R'){             // external reference part
                char symbol_name[7];
                char tmp[3];
                for(int i=1;i<input_len;i+=8){
                    strncpy(tmp,object_file_input+i,2);
                    tmp[2]='\0';
                    int ref_num=strtol(tmp,NULL,16);
                    ExtractSymbolName(symbol_name,object_file_input+i+2,6);
                    EstabNode* node=getEstabNodebySymbol(symbol_name,filenum);
                    if(node==NULL){
                        printf("no such external symbol\n");
                        return -1;
                    }
                    ref_num_arr[ref_num]=node->address;     // store address of external reference
                    //printf("\nR : %d %s, %X\n",ref_num,symbol_name,ref_num_arr[ref_num]);
                }
            }
            else if(record_letter=='T'){
                char start_add_str[7];
                char text_content[OBJ_MAX_LEN];
                int start_address;
                int content_len;
                strncpy(start_add_str,object_file_input+1, 6);
                strcpy(text_content,object_file_input+9);
                content_len=strlen(text_content);
                start_add_str[6]='\0';
                start_address=strtol(start_add_str,NULL,16)+csaddr;
                //printf("location: %X\n",start_address);
                for(int i=0;i<content_len;i+=2){            // load textrecord content to memory
                    int byte_value;
                    char byte_str[3];
                    strncpy(byte_str,text_content+i,2);
                    byte_str[2]='\0';
                    byte_value=strtol(byte_str,NULL,16);
                    if((start_address+i/2)<0||MAX_MEMORY_INDEX <(start_address+i/2)){
                        printf("Memory out of bounds\n");
                        return -1;
                    }
                    memory_arr[(start_address+i/2)/16][(start_address+i/2)%16]=byte_value;
                    //PrintMemory(start_address,start_address+i/2);
                }
                
            }
            else if(record_letter=='M'){    // modification part
                char addr_str[7];
                char ref_str[4];
                int modified_address;
                int ref_address;
                strncpy(addr_str,object_file_input+1, 6);
                strcpy(ref_str,object_file_input+9);
                addr_str[6]='\0';
                modified_address=strtol(addr_str,NULL,16)+csaddr;
                int idx=strtol(ref_str+1,NULL,16);
                ref_address=ref_num_arr[idx];   // indexing external reference
                err_flag=ModifyAddress(modified_address,ref_address,ref_str[0]);
                if(err_flag<0)
                    return err_flag;
            }
            else if(record_letter=='E'){
                char exeaddr_str[7];
                if(input_len>1){        // return executable instruction address to execaddr
                    strncpy(exeaddr_str,object_file_input+1,6);
                    exeaddr_str[6]='\0';
                    execaddr=strtol(exeaddr_str,NULL,16)+csaddr;
                }
            }
            else 
                continue;
        }
        csaddr+=control_section_len[file_i];
    }
    return 1;
}
void AddExternalSymbol(int file_idx,char symbol_name[],int address){
    EstabNode* newnode=(EstabNode*)malloc(sizeof(EstabNode));
    if(newnode==NULL){
        printf("Memory allocation error\n");
        return ;
    }
    strcpy(newnode->symbol_name,symbol_name);
    newnode->address=address;
    newnode->next=NULL;
    if(Estab[file_idx]==NULL)
        Estab[file_idx]=newnode;
    else{
        for(EstabNode* tmp=Estab[file_idx];tmp!=NULL;tmp=tmp->next){
            if(tmp->next==NULL){
                tmp->next=newnode;
                return;
            }
        }
    }
}
void printEstab(int file_num){
    int program_len=0;
    printf("control symbol address length\n");
    printf("section name\n");
    printf("--------------------------------\n");
    for(int i=0;i<file_num;i++){
        program_len+=control_section_len[i];
        for(EstabNode* tmp=Estab[i];tmp!=NULL;tmp=tmp->next){
            if(tmp==Estab[i])
                printf("%-7s%14.04X%7.04X\n",tmp->symbol_name,tmp->address,control_section_len[i]);
            else
                printf("%14s%7.04X\n",tmp->symbol_name,tmp->address);
        }
    }
    printf("--------------------------------\n");
    printf("           total length %.04X\n",program_len);
}
void ExtractSymbolName(char symbol_name[],char input[],int maxlen){
    strncpy(symbol_name,input,maxlen);
    int symbol_len=strlen(symbol_name);
     for(int i=0;i<symbol_len;i++){             // fill space with \0
        if(symbol_name[i]==' '||symbol_name[i]=='\n')
            symbol_name[i]='\0';
    }
    symbol_name[maxlen]='\0';
}
void EraseEstab(int file_num){
    for(int file_idx=0;file_idx<file_num;file_idx++){
        EstabNode* next=NULL;
        EstabNode* tmp=Estab[file_idx];
        Estab[file_idx]=NULL;
        while(tmp!=NULL){
            next=tmp->next;
            tmp->next=NULL;
            free(tmp);
            tmp=next;
        }
    }
}
int ModifyAddress(int address,int ref_address,char operation){
    unsigned int temp=0;
    if(address>MAX_MEMORY_INDEX||address<0){
        printf("Out of Memory Boundary\n");
        return -1;
    }
    for(int i=0;i<3;i++){                       // read memory
        temp=temp<<8;
        temp+=memory_arr[(address+i)/16][(address+i)%16];
    }
    //printf("loca: %X\n",address);
    //printf("befor modified: %X\n",temp);
    if(operation=='+')                          // modify
        temp+=(unsigned int)ref_address;
    else if(operation=='-')
        temp-=(unsigned int)ref_address;
    unsigned int div=1<<24;
    //printf("modified addr : %X\n\n",temp);
    for(int i=0;i<3;i++){                       // store
        div=div>>8;
        memory_arr[(address+i)/16][(address+i)%16]=temp/div;
        temp=temp%div;
    }
    return 1;
    //PrintMemory(address-address%16,address-(address%16)+31);
}
EstabNode* getEstabNodebySymbol(char symbol_name[],int filenum){
    for(int file_i=0;file_i<filenum;file_i++){
        EstabNode* tmp=Estab[file_i];
        while(tmp!=NULL){
            if(strcmp(tmp->symbol_name,symbol_name)==0)
                return tmp;
            tmp=tmp->next;
        }
    }
    return NULL;
}