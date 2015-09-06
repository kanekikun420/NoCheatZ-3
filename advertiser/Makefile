BASE = ..
GAME = CSS
PROJECT = server-plugin

ifeq "$(GAME)" "CSS"
	SDK_VERSION = sdk-source-2013-valve
	SDK_DEF = -DCSTRIKE_DLL -DNO_MALLOC_OVERRIDE -DGNUC -DPOSIX -DCOMPILER_GCC
endif
ifeq "$(GAME)" "CSP"
	SDK_VERSION = sdk-ep1-alliedmods
	SDK_DEF = -DPOSIX -DCOMPILER_GCC
endif
ifeq "$(GAME)" "CSGO"
	SDK_VERSION = sdk-csgo-alliedmods
	SDK_DEF = -DPOSIX -DCOMPILER_GCC
endif

SDK_DIR = $(BASE)/$(SDK_VERSION)
PROJECT_DIR = $(BASE)/$(PROJECT)
SRC_DIR = $(PROJECT_DIR)/Code
BIN_DIR = $(SRC_DIR)

OTHER_PREPROCESSORS = -DNCZ_$(GAME) $(SDK_DEF)

# Compilateur
CXX = g++

# Nom du fichier binaire de sortie
BINARY_NAME = nocheatz-$(PROJECT)-$(GAME).so

# Dossier de sortie du fichier binaire
BINARY_DIR = $(BASE)/

# Dossiers de sortie
RELEASE_DIR = Release/linux
DEBUG_DIR = Debug/linux

# Dossier contenant les librairies dynamiques
SRCDS_BIN_DIR = ./bin

# Dossier contenant les librairies statiques
SRCDS_A_DIR = $(SDK_DIR)/lib/public/linux32

# Paramètres du compilateur
ARCH_CFLAGS =   -mtune=i486 -march=pentium -mmmx
USER_CFLAGS =   -DTIXML_USE_TICPP
BASE_CFLAGS =   -msse \
		-D_LINUX \
		-D_alloca=alloca \
		-fPIC \
		-Wno-deprecated \
		-msse \
		-fpermissive

#OPT_FLAGS = -O3 -funroll-loops -s -pipe $(OTHER_PREPROCESSORS)
DEBUG_FLAGS = -g -ggdb3 -O0 -D_DEBUG $(OTHER_PREPROCESSORS)               
#DEBUG_FLAGS = $(OPT_FLAGS)
OPT_FLAGS = $(DEBUG_FLAGS)
# Fichiers à compiler
SRC= $(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */*/*.cpp) $(wildcard */*/*/*.cpp)                      

# Fichiers à lier
ifneq "$(GAME)" "CSP"
	LINK_SO =       $(SRCDS_BIN_DIR)/libtier0_srv.so \
			$(SRCDS_BIN_DIR)/libvstdlib_srv.so

	LINK_A =        $(SRCDS_A_DIR)/tier1.a \
			$(SRCDS_A_DIR)/mathlib.a

	ifeq "$(GAME)" "CSGO"
		LINK_A += $(SRCDS_A_DIR)/interfaces.a
	endif
else
	LINK_SO =       $(SRCDS_BIN_DIR)/tier0_i486.so \
			$(SRCDS_BIN_DIR)/vstdlib_i486.so

	LINK_A =        $(SRCDS_A_DIR)/tier1_i486.a \
			$(SRCDS_A_DIR)/mathlib_i486.a
endif

LINK = -lm -ldl $(LINK_A) $(LINK_SO)

# Dossiers des fichiers inclus
INCLUDE =       -I$(SRC_DIR) \
		-I$(SDK_DIR)/public \
		-I$(SDK_DIR)/public/game/server \
		-I$(SDK_DIR)/public/engine \
		-I$(SDK_DIR)/public/tier0 \
		-I$(SDK_DIR)/public/tier1 \
		-I$(SDK_DIR)/common
ifeq "$(GAME)" "CSGO"
		INCLUDE += -I$(SDK_DIR)/interfaces
endif
                        
# Règles de compilation

ifeq "$(DEBUG)" "false"
	BIN_DIR = $(RELEASE_DIR)
	CFLAGS = $(OPT_FLAGS)
else
	BIN_DIR = $(DEBUG_DIR)
	CFLAGS = $(DEBUG_FLAGS)
endif
CFLAGS += $(USER_CFLAGS) $(BASE_CFLAGS) $(ARCH_CFLAGS)

OBJECTS := $(SRC:%.cpp=$(BIN_DIR)/%.o)

compile_object = \
	@mkdir -p $(2); \
	echo "$(1) => $(3)"; \
	$(CXX) $(INCLUDE) $(CFLAGS) -o $(3) -c $(1) 2> "error.txt";

$(BIN_DIR)/%.o: %.cpp %.h
	$(call compile_object, $<, $(@D), $@)

$(BIN_DIR)/%.o: %.cpp
	$(call compile_object, $<, $(@D), $@)

all: $(OBJECTS)
	@$(CXX) $(INCLUDE) $(CFLAGS) $(OBJECTS) $(LINK) -shared -o $(BINARY_DIR)/$(BINARY_NAME)
        
release:
	@$(MAKE) all DEBUG=false
	
remake:
	@$(MAKE) clean
	@$(MAKE)
	
debug:
	@$(MAKE) all DEBUG=true

clean:
	@rm -rf $(RELEASE_DIR)
	@rm -rf $(DEBUG_DIR)
	@rm -rf $(BINARY_DIR)/$(BINARY_NAME)
        
.PHONY: clean
