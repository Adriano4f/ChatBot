#SRC
SRC     := source/main.c \
			source/CPU.c \
			source/LP_Unit.c\
			source/LG_Unit.c\
			source/LI_Unit.c\
			source/General_Utility.c\
			source/IntentP.c\
			source/SemanticP.c\
			source/M_Unit.c\
			source/BA_Unit.c\
			source/MLearning.c

#FLAGS

#COMMON
FLAGS = -O2 -std=c17 -Wall -I$(INCLUDE_DIR)
u_FLAGS := -m64 -unicode
w_FLAGS := -D_WIN32

# CONVENTION: NAME YOUR FLAGS LIKE cfilename_FLAG, OTHERWISE IT WON'T COMPILE.

main_FLAG				:= 
General_Utility_FLAG			:=
CPU_FLAG				:=
BA_unit_FLAG				:=
LG_Unit_FLAG				:=
LI_Unit_FLAG				:=
LP_Unit_FLAG				:=
M_Unit_FLAG				:=
MLearning_FLAG				:=
SemanticP_FLAG				:=
IntentP_FLAG				:=
