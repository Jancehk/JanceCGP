#####################################################################
#						CGP 平台Makefile							#
#						  Shen Huashan								#
#						Shenghs2004@163.com							#
#						2011-06-04 in My home						#
#####################################################################
########################可以修改部分开始#############################
#  程序编译后房的目录
BIN_DIR = ../bin
#	当前目录
CURRENT_DIR = 
#	代码文件目录
SOURCE_DIR = ./com/source  ./public/source ./mm/source ./vos/source ./vk/source
#	静态库目录
LIB_DIR = 
#	程序名称
APP_NAME = cpssglb.a
LOG = make.log
#######################可以修改部分结束###############################
#########################以下不用修改#################################
SOURCE_FILE = $(foreach dir,$(SOURCE_DIR),$(wildcard $(dir)/*.c))
OBJS_FILE   = $(notdir $(SOURCE_FILE:.c=.o))
TAGET_OBJS 	= $(addprefix $(BIN_DIR)/,$(OBJS_FILE))
PROJECT		= $(addprefix $(SOURCE_DIR)/,$(OBJS_FILE))

INCLUDE_DIR = $(shell find $(CURRENT_DIR) -type d -name include)
BUILD_INCLUDE = $(addprefix -I , $(INCLUDE_DIR))
CC = gcc
AR = ar
DEFLAG = #DSWP_OS_TYPE = "swp_os_linux" -DCPSS_VOS_LINUX -DUM_OS_LINUX
CFLAG = -g -Wall -Wextra
DEFAULT_CFLAG = #-lpthread -lrt -lm -lreadine -lcares
LINK_CFLAG = $(DEFAULT_CFLAG) #-Xlinker -Map -Xlinker linux_app_sgmbol.map
CFLAGS = $(CFLAG) $(DEFLAG) $(LINK_CFLAG) -DLIBXML_DEBUG_ENABLE $(BUILD_INCLUDE)
.PHONY: all clean

all: $(LOG) $(APP_NAME)
$(LOG):
	@echo $(TAGET_OBJS)..........................
	@rm -f $(BIN_DIR)/$(LOG)
	@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	@touch $(BIN_DIR)/$(LOG)
$(APP_NAME): $(TAGET_OBJS)
	@echo Linking ...
	@[ -d $(@D) ] || mkdir -D $(@D)
	@$(AR) -r $(BIN_DIR)/$(APP_NAME) $(TAGET_OBJS)
#	$(CC) $(TAGET_OBJS) $(CFLAGS) -o $(BIN_DIR)/$(SUBSYSTEM_NAME) $(LIB_PATH) $(LIB_WS)
	@echo Create $(APP_NAME) was Made.
$(TAGET_OBJS):%.o:%.c
	@echo Compling $(notdir $<) ...
	@echo Compling $< ...
	@rm -f $(BIN_DIR)/$(LOG)
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@ 2>>$(BIN_DIR)/$(LOG) || cat $(BIN_DIR)/$(LOG)
clean:
	-rm -f $(BIN_DIR)/*.o
	-rm -f $(BIN_DIR)/$(LOG)


############################Makefile结束##############################

