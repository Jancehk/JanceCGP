#####################################################################
#						CGP 平台Makefile							#
#						  Shen Huashan								#
#						Shenghs2004@163.com							#
#						2011-06-04 in My home						#
#####################################################################
########################可以修改部分开始#############################
#  程序编译后房的目录
BIN_DIR = ../bin/linux
#	当前目录
CURRENT_DIR = 
#	代码文件目录
SOURCE_DIR = ../source
#	头文件目录
INCLUDE_DIR = ../include
#	静态库目录
LIB_DIR = ../lib
#	程序名称
APP_NAME = test
LOG = make.log
#######################可以修改部分结束###############################
#########################以下不用修改#################################
SOURCE_FILE = $(foreach dir,$(SOURCE_DIR),$(wildcard $(dir)/*.c))
OBJS_FILE   = $(notdir $(SOURCE_FILE:.c=.o))
TAGET_OBJS 		= $(addprefix $(BIN_DIR)/,$(OBJS_FILE))

BUILD_INCLUDE = $(addprefix -I,$(INCLUDE_DIR))
#LIB_PATH = $(addprefix -L,$(LIB_DIR))
CC = gcc
AR = ar
CFLAG = -g -Wall -Wextra
DEFLAG =# DSWP_OS_TYPE = "swp_os_linux" -DCPSS_VOS_LINUX -DUM_OS_LINUX
DEFAULT_CFLAG = -lpthread#-lrt -lm -lreadine -lcares
LINK_CFLAG = $(DEFAULT_CFLAG) #-Xlinker -Map -Xlinker linux_app_sgmbol.map
CFLAGS = $(CFLAG) $(DEFLAG)$(LINK_CFLAG)# -DLIBXML_DEBUG_ENABLE
override CFLAGS += $(BUILD_INCLUDE)
.PHONY: all clean

all: $(LOG) $(APP_NAME)
$(LOG):
	@rm -f $(BIN_DIR)/$(LOG)
	@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	@touch $(BIN_DIR)/$(LOG)
$(APP_NAME): $(TAGET_OBJS)
	@echo Linking ...
	@[ -d $(@D) ] || mkdir -D $(@D)
#	@$(AR) -r $(BIN_DIR)/$(APP_NAME) $(TAGET_OBJS)
	@$(CC) $(TAGET_OBJS) $(CFLAGS) -o $(BIN_DIR)/$(APP_NAME) $(LIB_DIR)/cpssglb.a $(LIB_WS)
	@echo Create $(APP_NAME) was Made.
$(TAGET_OBJS):$(SOURCE_FILE)
	@echo Compling $(notdir $<) ...
	@rm -f $(BIN_DIR)/$(LOG)
	@[ -d $(@D) ] || mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@ 2>>$(BIN_DIR)/$(LOG) || cat $(BIN_DIR)/$(LOG)
clean:
	-rm -f $(BIN_DIR)/*.o
	-rm -f $(BIN_DIR)/$(LOG)
lib_link:
	-rm -f ../include/cpss_common.h ../lib/cpssglb.a
	ln -s ../../Framwork/cpss/public/include/cpss_common.h ../include/cpss_common.h
	ln -s ../../Framwork/lib/cpssglb.a ../lib/cpssglb.a
############################Makefile结束##############################

