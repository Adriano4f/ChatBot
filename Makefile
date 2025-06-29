include Flags.mk

# DIRECTORIES
SRC_DIR 		:= source
OBJ_DIR 		:= source/obj
INCLUDE_DIR		:= include
OBJ     		:= $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# UNIX
u_CXX   		:= gcc
u_OUT   		:= Dana
u_MKDIR 		:= mkdir -p $(OBJ_DIR)

# WINDOWS
w_CXX   		:= x86_64-w64-mingw32-gcc
w_OUT   		:= Dana.exe
w_MKDIR 		:= @if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

# NAME PLATFORM
ifeq ($(OS),Windows_NT)
    PLAT    	:= w
else
    UNAME_S 	:= $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLAT 	:= u
    else ifeq ($(UNAME_S),Darwin)
        PLAT 	:= u
    else
        PLAT 	:= Unknown
    endif
endif

# LINK
all: $(OBJ)
	$($(PLAT)_CXX) $(OBJ) -o $($(PLAT)_OUT)

# COMPILE
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$($(PLAT)_MKDIR)
	$($(PLAT)_CXX) $(FLAGS) $($(PLAT)_FLAGS) $(${$(basename $(notdir $<))}_FLAG) -c $< -o $@


# CLEAN
u_CLEAN     	:= rm -f
w_CLEAN    		:= del /F /Q

clean:
	$($(PLAT)_CLEAN) *.o

print:
	@echo SRC = $(SRC)
	@echo OBJ = $(OBJ)

