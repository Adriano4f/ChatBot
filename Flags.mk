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
# Exploit mitigations: stack canaries, fortified libc calls, no implicit common symbols
HARDENING	:=	-fstack-protector-strong -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 \
						-Wformat -Wformat-security -Werror=format-security -fno-common

CFLAGS	:=	-O2 -std=c17 -Wall $(HARDENING) -I$(INCLUDE_DIR)
u_FLAGS	:=	-m64 -fPIE
w_FLAGS	:=	-D_WIN32

# LINKER
u_LDFLAGS	:=	-pie -Wl,-z,relro,-z,now -Wl,-z,noexecstack
w_LDFLAGS	:=	-Wl,--nxcompat,--dynamicbase,--high-entropy-va

# FLAGS ARE NAMED LIKE modulename_FLAG, THEY ARE APPLIED PER MODULE

General_Utility_FLAG	:=
CPU_FLAG							:=
Hash_FLAG							:=
