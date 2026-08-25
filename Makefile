include Flags.mk

OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

# UNIX
u_CXX					:= gcc
u_OUT					:= Dana
u_MKDIR				:= @mkdir -p $(OBJ_DIR)

# WINDOWS
w_CXX					:= @x86_64-w64-mingw32-gcc
w_OUT					:= Dana.exe
w_MKDIR				:= @if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

# NAME PLATFORM
ifeq ($(OS),Windows_NT)
	PLAT			:= w
else 
	UNAME_S		:= $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	PLAT			:= u
else ifeq ($(UNAME_S),Darwin)
	PLAT			:= u
else
	PLAT			:= Unknown
	@echo "Couldn't detect your OS"
endif
endif

# LINK
all: $(OBJ)
	@$($(PLAT)_CXX) $(OBJ) -o $($(PLAT)_OUT) $($(PLAT)_LDFLAGS)

# COMPILE
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$($(PLAT)_MKDIR)
ifeq ($(OS),Windows_NT)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
else
	@mkdir -p $(dir $@)
endif
	@$($(PLAT)_CXX) $(CFLAGS) -I$(dir $<) $($(PLAT)_FLAGS) -c $< -o $@

# CLEAN
u_CLEAN		:=	rm -r
w_CLEAN		:=	rmdir /S /Q

clean:
	@$($(PLAT)_CLEAN) $(OBJ_DIR)/*
	@$($(PLAT)_MKDIR)/000_dummy 
# This dummy file ^^^^^^^^^^ is just to avoid error when calling make clean again

print-src-obj:
	@echo SRC = $(SRC)
	@echo OBJ = $(OBJ)

