CC = gcc
TARGET = 20161581.out
OBJS = 20161581.o CommandParse.o Commandfunc.o OperationCodeTable.o
CFLAGS = -Wall
## CFLAG -WALL 사용해서 WARNING 조심
all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC)  -o $(TARGET) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
clean :
	rm -f $(OBJS) $(TARGET)

