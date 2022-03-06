Target	?= demo

ifeq ($(ARCH), arm)
COMPILE	?= arm-linux-gnueabihf-
else
COMPILE ?= 
endif

# 设置编译器
CC		?= $(COMPILE)gcc

# 设置源码路径
SRCPATH	?= 	. \

# 设置头文件路径
INCPATH	?=  . \

# 设置编译中间文件存放路径
OBJPATH	?= obj

# 源文件
SRC_C	?= $(foreach dir, $(SRCPATH), $(wildcard $(dir)/*.c))
SRC_S	?= $(foreach dir, $(SRCPATH), $(wildcard $(dir)/*.s))

# 目标文件
OBJ_C	?= $(patsubst %.c, $(OBJPATH)/%.o, $(notdir $(SRC_C)))
OBJ_S	?= $(patsubst %.s, $(OBJPATH)/%.o, $(notdir $(SRC_S)))
OBJ		?= $(OBJ_C) $(OBJ_S)

# 编译时去寻找的文件路径
VPATH	?= $(SRCPATH)

# 编译选项
CFLAG	?= $(patsubst %, -I%, $(INCPATH))

$(Target) : $(OBJ)	
	$(CC)  $^ -g -o $@ -lrt -pthread

$(OBJ_C) : $(OBJPATH)/%.o : %.c | mkobjpath
	$(CC) -c -g  -pthread $(CFLAG) $< -o $@ 

PHONY:
	@echo Target	= $(Target)
	@echo COMPILE	= $(COMPILE)
	@echo CC 		= $(CC)
	@echo SRCPATH	= $(SRCPATH)
	@echo SRC_C		= $(SRC_C)
	@echo OBJ_C		= $(OBJ_C)
	@echo CFLAG		= $(CFLAG)

clean:
	rm -rf $(OBJPATH) $(Target) server client aaa

mkobjpath:
	@mkdir -p $(OBJPATH)

special: server client

server:
	gcc file_server.c -o server -lrt

client:
	gcc file_client.c -o client -lrt
