# 库名称
LIB_NAME		?= mydef
# 库主版本号
LIB_VER_MAJOR	?= 1
# 库次版本号
LIB_VER_MINOR	?= 2
# 安装库的路径
LIB_INSTALL_PATH	?= /lib
# 库头文件路径
LIB_INC_PATH	?= /usr/include
# 库头文件
LIB_INC			?= myDef.h

# link name
LIB_LINKNAME	?= lib$(LIB_NAME).so
# soname
LIB_SONAME		?= $(LIB_LINKNAME).$(LIB_VER_MAJOR)

LIB				?= $(LIB_SONAME).$(LIB_VER_MINOR)

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

$(LIB) : $(OBJ)	
	$(CC)  $^ -g -shared -Wl,-soname,$(LIB_SONAME) -o $@

$(OBJ_C) : $(OBJPATH)/%.o : %.c | mkobjpath
	$(CC) -g -c -fPIC -Wall $(CFLAG) $< -o $@ 

PHONY:
	@echo Target	= $(Target)
	@echo LIBSONAME	= $(LIBSONAME)
	@echo LIB		= $(LIB)
	@echo COMPILE	= $(COMPILE)
	@echo CC 		= $(CC)
	@echo SRCPATH	= $(SRCPATH)
	@echo SRC_C		= $(SRC_C)
	@echo OBJ_C		= $(OBJ_C)
	@echo CFLAG		= $(CFLAG)

clean:
	rm -rf $(OBJPATH) $(LIB) 

mkobjpath:
	@mkdir -p $(OBJPATH)

install:
	@cp $(LIB) $(LIB_INSTALL_PATH)/
	@ldconfig 
	@rm $(LIB_INSTALL_PATH)/$(LIB_LINKNAME)
	@ln -s $(LIB_INSTALL_PATH)/$(LIB_SONAME) $(LIB_INSTALL_PATH)/$(LIB_LINKNAME)
	@rm $(LIB_INC_PATH)/$(LIB_INC)
	@cp $(LIB_INC) $(LIB_INC_PATH)/
