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
	@$($(PLAT)_CXX) $(OBJ) -o $($(PLAT)_OUT)

# COMPILE
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$($(PLAT)_MKDIR)
ifeq ($(OS),Windows_NT)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
else
	@mkdir -p $(dir $@)
endif
	@mod=$(call get_module,$<); \
	$($(PLAT)_CXX) $(CFLAGS) -I$(mod) $($(PLAT)_FLAGS) $($(mod)_FLAG) -c $< -o $@

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

# TESTS
# The suite links every module except main.c and needs a POSIX host (it
# redirects stdin/stdout to check what the units read and print).

TEST_DIR				:=	tests
TEST_OBJ_DIR		:=	$(OBJ_DIR)/tests
COV_DIR					:=	$(OBJ_DIR)/coverage

TEST_SRC				:=	$(wildcard $(TEST_DIR)/*.c)
TESTED_SRC			:=	$(filter-out $(MAIN_SRC),$(SRC))
TEST_INCLUDE		:=	-I$(INCLUDE_DIR) -I$(TEST_DIR) $(foreach m,$(MODULES),-I$(SRC_DIR)/$(m))
TEST_CFLAGS			:=	-std=c17 -Wall -g -O0

test: $(TEST_SRC) $(TESTED_SRC)
	@mkdir -p $(TEST_OBJ_DIR)
	@$(u_CXX) $(TEST_CFLAGS) $(TEST_INCLUDE) $^ -o $(TEST_OBJ_DIR)/run_tests
	@./$(TEST_OBJ_DIR)/run_tests

# COVERAGE
# Line coverage of the tested modules, reported by gcov.

coverage: $(TEST_SRC) $(TESTED_SRC)
	@mkdir -p $(COV_DIR)
	@for src in $^; do \
		$(u_CXX) $(TEST_CFLAGS) --coverage $(TEST_INCLUDE) -c $$src \
			-o $(COV_DIR)/`basename $$src .c`.o || exit 1; \
	done
	@$(u_CXX) --coverage $(COV_DIR)/*.o -o $(COV_DIR)/run_tests
	@./$(COV_DIR)/run_tests
	@echo
	@gcov -o $(COV_DIR) $(TESTED_SRC) | grep -A1 "^File '$(SRC_DIR)"
	@mv *.gcov $(COV_DIR)

clean-test:
	@rm -rf $(TEST_OBJ_DIR) $(COV_DIR)

.PHONY: all clean test coverage clean-test print-src-obj

