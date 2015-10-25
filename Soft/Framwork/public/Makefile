###############   makefile fromwork change #########################
SUBSYSTEM_NAME :=cpsslib.a
CURRENT_DIR	=$(shell pwd)
PROJECT_DIR = ../bin
APP_MODULE_INCLUDE_DIR = ../
SWP_OS_TYPE ?= SWP_OS_LINUX
APP_SRC_DIR :=../source
#include
APP_INCLUDE = $(shell find $(APP_MODULE_INCLUDE_DIR) -type d -name include)
LIB_PATH = -L ../lib
LOG = make.log
###############   makefile formwork  no change #####################
ADD_DEFINES ?= 
ADDED_CFLAGS := -g

SRCS = $(foreach dir ,$(APP_SRC_DIR), $(wildcard $(CURRENT_DIR)/*.c))
OBJS = $(notdir $(SRCS:.c=.o))
TAGET_OBJS = $(addprefix $(PROJECT_DIR)/, $(OBJS))
BUILD_INCLUDE = $(addprefix -I, $(APP_INCLUDE))
CFLAGS := $(CC_DEFINES) $(ADDED_CFLAGS) -DLIBXML -DEBUG_ENABLED
override CFLAGS += $(BUILD_INCLUDE)
override CFLAGS += $(WARN)

CC := gcc
AR := ar
WARN = -Wall -g
CC_DEFINES = # -DSWP_OS_TYPE = SWP_OS_LINUX -DCPSS_VOS_LINUX

.PHONT: all clean
all: $(LOG_PROCESS) $(SUBSYSTEM_NAME)
$(LOG_PROCESS):
	@rm -f $(PROJECT_DIR)/$(LOG)
	@[ -d $(PROJECT_DIR) ] || mkdir -p $(PROJECT_DIR)
	touch $(PROJECT_DIR)/$(LOG)
$(SUBSYSTEM_NAME): $(TAGET_OBJS)
	@echo Linking ...
	@[ -d $(@D) ] || mkdir -D $(@D)
	@$(CC) $(TAGET_OBJS) $(CFLAGS) -lpthread \
	-o $(PROJECT_DIR)/$(SUBSYSTEM_NAME) $(LIB_PATH) $(LIB_WS)
	@echo $(SUBSYSTEM_NAME) has been made.
$(TAGET_OBJS):$(PROJECT_DIR)/%.o:%.c
	@echo Compling $< ...
	@rm -f $(PROJECT_DIR)/$(LOG)
	@[ -d $(@D) ] || mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@ 2>>$(PROJECT_DIR)/$(LOG) || cat $(PROJECT_DIR)/$(LOG)
clean:
	-rm -f $(PROJECT_DIR)/*.o
	-rm -f $(PROJECT_DIR)/$(LOG)

