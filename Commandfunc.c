#include"20161581.h"
unsigned char memory_arr[65536][16] = { 0, };
void Help() { // help 커맨드 수행
	for (int i = 0; arrCmdPrint[i][0] != 0; i++)////////////애매합니다잉............
		printf("%s\n", arrCmdPrint[i]);
}
void Quit() {
	exit(0);
}
void PrintHistory(CmdHistoryNode* curnode) {
	if (curnode == NULL)return;
	PrintHistory(curnode->next);
	printf("%-9d %s\n", curnode->num, curnode->str);
}
void Dump(int argc, char argv[][ARGV_MAX_LEN]) {///////////////문제는 없는데 불안정
	int memory_index[2];
	for(int i=1;i<argc;i++)
		memory_index[i-1] = strtol(argv[i], NULL, 16);
	if (argc == 1) {
		if (last_idx == MAX_MEMORY_INDEX)last_idx = -1;
		memory_index[0] = last_idx + 1;
	}
	if (argc != 3)// dump [s, e] e 정해주는 곳/////////////////수정할 수도 있음
		memory_index[1] = memory_index[0] + 159 < MAX_MEMORY_INDEX? memory_index[0] + 159: MAX_MEMORY_INDEX;
	PrintMemory(memory_index[0], memory_index[1]);
}
void PrintMemory(int s, int e) {
	int start_line = s / 16;
	int end_line = e / 16;
	int start_pos = s % 16;
	int end_pos = BYTE_PER_LINE-1;
	for (int i = start_line; i <= end_line; i++) {
		if (i > start_line)start_pos = 0;
		if (i == end_line)end_pos = e % 16;
		PrintLnMemory(i, start_pos, end_pos);
	}
	last_idx = e;
}
void PrintLnMemory(int line_num,int s,int e) {
	int line_index = line_num * BYTE_PER_LINE;//시작 주소 인덱스
	printf("%05X ", line_index);//
	for (int i = 0; i < s; i++)
		printf("   ");
	for (int i = s; i <= e; i++)
		printf("%02X ", memory_arr[line_num][i]);
	for (int i = e + 1; i < BYTE_PER_LINE; i++)
		printf("   ");
	printf("; ");
	for (int i = 0; i < s; i++)
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