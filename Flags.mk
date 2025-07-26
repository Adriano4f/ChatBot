#SRC & INC

SRC_DIR			:=	source
OBJ_DIR			:=	build
INCLUDE_DIR	:=	include

MODULES	:=	$(subst /,,$(subst source/,,$(filter %/,$(wildcard source/*/))))

MAIN_SRC := $(SRC_DIR)/main.c
MODULE_SRCS	:=	$(foreach m,$(MODULES),$(wildcard $(SRC_DIR)/$(m)/*.c))

SRC	:=	$(MAIN_SRC) $(MODULE_SRCS)

#FLAGS

#COMMON
CFLAGS	:=	-O2 -std=c17 -Wall -I$(INCLUDE_DIR)
u_FLAGS	:=	-m64 -unicode
w_FLAGS	:=	-D_WIN32

# FLAGS ARE NAMED LIKE modulename_FLAG, THEY ARE APPLIED PER MODULE

General_Utility_FLAG	:=
CPU_FLAG							:=
Hash_FLAG							:=
