ifeq ($(OS),Windows_NT)
	$(error Built application in Windows using Solution VS-Project if there, do not use this make!)
endif

null=
space=$(null) $(null)
UNAME_S=$(shell uname -s)
UNAME_P=$(shell uname -p)

#MAIN INFO
EXEC=AutonomousDriving

ENTRY=entry
ENTRY_DEPS=Main.hpp


#DIRECTORIES
LIB_DIR=libs
SRC_DIR=src
INC_DIR=includes
RLS_DIR=bin
DEP_DIR=deps

#COMPILER SETTINGS
COMPILER=g++ -std=c++0x 
COMPILERFLAGS=-g -Wall -Wno-comment
EXT_LIBS=-lm

#DEPENDENCY SETTINGS
GLM=glm
KHRONOS=khronos_headers
HOG=hog
DLNN=dlnn
SIFT=sift
SVM=svm
PBM=pbm

DEPS=$(GLM) $(KHRONOS)
MAKELIBS=

#OS DEPENDED SETTINGS
ifeq ($(UNAME_S),Darwin)
	COMPILERFLAGS += -D OSX
	KHRONOS +=khronos_headers/CL1.0
    EXT_LIBS+=-framework OpenCL
endif
#else
ifeq ($(UNAME_S),Linux)
	COMPILERFLAGS += -D LINUX
	KHRONOS +=khronos_headers/CL1.2
    EXT_LIBS+=-lOpenCL
endif

#INTERNAL LIBRARIES
#INT_LIBS=-lhog -ldlnn -lsift -lsvm -lpbm

#PREPARE LINKERFLAGS
_LD_LIB_PATH=$(addprefix -I,$(subst :,$(space),$(LD_LIBRARY_PATH)))
_DEPS=$(addprefix -I$(DEP_DIR)/,$(DEPS))
_MAKELIBS=$(addprefix $(DEP_DIR)/,$(MAKELIBS))
LINKERFLAGS=-I$(INC_DIR) $(EXT_LIBS) $(INT_LIBS) -L$(LIB_DIR)

#ADD OBJECTS HERE WIHOUT .OBJ
ELEMENTS=Main MLogManager MOpenCLManager OpenCLProgram OpenCLArgument MComputationManager


#PREPARING
OBJECTS=$(addsuffix .o,$(ELEMENTS))

_OBJECTS=$(addprefix $(RLS_DIR)/objects/,$(OBJECTS))
_ENTRY_DEPS=$(addprefix $(SRC_DIR)/,$(ENTRY_DEPS))
_ENTRY_OBJ=$(RLS_DIR)/objects/$(ENTRY).o
_ENTRY_SRC=$(SRC_DIR)/$(ENTRY).cpp

#BUILDING AND COMPILING
$(RLS_DIR)/$(EXEC): $(_OBJECTS) $(_ENTRY_OBJ)
	$(COMPILER) $(COMPILERFLAGS) $(_OBJECTS) $(_ENTRY_OBJ) $(LINKERFLAGS) -o $@

$(_ENTRY_OBJ): $(_ENTRY_SRC)
	$(COMPILER) $(COMPILERFLAGS) $(_DEPS) -c -o $@ $<

$(RLS_DIR)/objects/%.o: $(SRC_DIR)/%.cpp
	$(COMPILER) $(COMPILERFLAGS) $(_DEPS) -c -o $@ $<

include make.dep


#EXTRA OPTIONS
makelibs:
	$(MAKE) -C $(_MAKELIBS)

cleanlibs:
	$(MAKE) clean -C $(_MAKELIBS)

cleanlogs:
	rm -f $(RLS_DIR)/*.log
	rm -f *.log

clean:
	rm -f $(_OBJECTS) $(_ENTRY_OBJ) $(RLS_DIR)/$(EXEC)

makedirs:
	mkdir -p $(LIB_DIR) 
	mkdir -p $(INC_DIR)
	mkdir -p $(RLS_DIR)
	mkdir -p $(RLS_DIR)/objects
	mkdir -p $(DEP_DIR)
	mkdir -p $(SRC_DIR)