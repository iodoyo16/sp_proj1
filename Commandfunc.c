#include"20161581.h"
unsigned char memory_arr[65536][16] = { 0, };

void Help() { 
	for (int i = 0; arrCmdPrint[i][0] != 0; i++)	//	print help string list
		printf("%s\n", arrCmdPrint[i]);
}

void Dir(){
	int cnt=0;
	DIR* dirptr=NULL;
	struct dirent* tmpent;
	struct stat buf;	
	dirptr=opendir("./");						//	open "./" and return directory pointer
	if(dirptr==NULL){
		printf("Directory Open Error!\n");		//	handling directory open error
		exit(1);
	}
	while((tmpent=readdir(dirptr))!=NULL){		//	read directory ptr and return directory entry
		if(!strncmp(tmpent->d_name,".",1))		//	skip entry start with "."
			continue;
		if(lstat(tmpent->d_name,&buf)!=0){		//	read tmpent->d_name 's status and handling error
			printf("Stat Error\n");
			exit(1);
		}
		if(S_ISDIR(buf.st_mode)){												//	DIR: print	"name/"
			printf("%*.*s/", DIRNAME_LEN+5, DIRNAME_LEN ,tmpent->d_name);
		}
		else{
			if(S_ISREG(buf.st_mode)){											//	regular file: print "name"
				printf("%*.*s", DIRNAME_LEN+5, DIRNAME_LEN ,tmpent->d_name);
				if(S_IEXEC&buf.st_mode)											//	if execuatble print "*" also
					printf("*");
			}
		}
		if(strlen(tmpent->d_name)>DIRNAME_LEN){		//	if directory name is too long
			printf("...");
		}
		cnt++;
		if(cnt%3==0)			//	new line per 3 file
			printf("\n");
	}
	printf("\n");
	closedir(dirptr);
}
void Quit() {
	exit(0);
}

void PrintHistory(CmdHistoryNode* curnode) {
	if (curnode == NULL)return;
	PrintHistory(curnode->next);
	printf("%-9d %s\n", curnode->num, curnode->str);
}


void Dump(int argc, char argv[][ARGV_MAX_LEN]) {
	int memory_index[2];
	for(int i=1;i<argc;i++)
		memory_index[i-1] = strtol(argv[i], NULL, 16);		//	set s, e by input arg
	if (argc == 1) {
		if (last_idx == MAX_MEMORY_INDEX)			//	set s by last_idx
			last_idx = -1;
		memory_index[0] = last_idx + 1;
	}
	if (argc != 3)									//	set e by default dump line
		memory_index[1] = memory_index[0] + 159 < MAX_MEMORY_INDEX? memory_index[0] + 159: MAX_MEMORY_INDEX;
	PrintMemory(memory_index[0], memory_index[1]);
}

void PrintMemory(int s, int e) {						//	s,e is address of memory
	int start_line = s / 16;							//	row of s
	int end_line = e / 16;								//	row of e
	int start_pos = s % 16;								//	column of s
	int end_pos = BYTE_PER_LINE-1;						//	max column
	for (int i = start_line; i <= end_line; i++) {
		if (i > start_line)start_pos = 0;				//	not start line -> start_pos=0
		if (i == end_line)end_pos = e % 16;				//	end line -> end_pos = column of e
		PrintLnMemory(i, start_pos, end_pos);
	}
	last_idx = e;		//	update last index
}

void PrintLnMemory(int line_num,int s,int e) {
	int line_index = line_num * BYTE_PER_LINE;			//	index of first column of this line
	printf("%05X ", line_index);
	for (int i = 0; i < s; i++)							//	before s or after e , print space
		printf("   ");
	for (int i = s; i <= e; i++)
		printf("%02X ", memory_arr[line_num][i]);		//	print memory value
	for (int i = e + 1; i < BYTE_PER_LINE; i++)
		printf("   ");
	printf("; ");

	for (int i = 0; i < s; i++)							//	ASCII character of memory value part
		printf(".");
	for (int i = s; i <= e; i++) {
		if (MIN_VALUE <= memory_arr[line_num][i] && memory_arr[line_num][i] <= MAX_VALUE)
			printf("%c", memory_arr[line_num][i]);
		else
			printf(".");
	}	
	for (int i = e + 1; i < BYTE_PER_LINE; i++)
		printf(".");
	printf("\n");
}

void Edit(char argv[][ARGV_MAX_LEN]) {
	int address = strtol(argv[1], NULL, 16);
	int value = strtol(argv[2], NULL, 16);
	memory_arr[address / 16][address % 16] = value;
}


void Fill(char argv[][ARGV_MAX_LEN]) {
	int start_idx = strtol(argv[1],NULL,16);
	int end_idx = strtol(argv[2], NULL, 16);
	int value = strtol(argv[3], NULL, 16);
	for(int i=start_idx;i<=end_idx;i++)
		memory_arr[i / 16][i % 16] = value;
}


void Reset() {
	for (int i = 0; i <= MAX_MEMORY_INDEX; i++)
		memory_arr[i / 16][i % 16] = 0;
}

void TypeFile( char argv[][ARGV_MAX_LEN]){
	int ch;
	FILE* fp=fopen(argv[1],"r");
	if(fp==NULL){
		printf("fileopen error\n");
		return;
	}
	while(1){
		ch = fgetc(fp);
		if(feof(fp)!=0)
			break;
		putchar(ch);
	}
	fclose(fp);
	return;
}
