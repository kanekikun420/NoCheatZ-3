BASE = ..
PROJECT = server-plugin

ifeq "$(GAME)" "CSS"
	SDK_VERSION = source-sdk-2013/mp/src
	SDK_DEF = -DCSTRIKE_DLL -DNO_MALLOC_OVERRIDE -DGNUC -DPOSIX -DCOMPILER_GCC
endif
ifeq "$(GAME)" "CSP"
	SDK_VERSION = ep1-sdk
	SDK_DEF = -DPOSIX -DCOMPILER_GCC -DGNUC 
endif
ifeq "$(GAME)" "CSGO"
	SDK_VERSION = csgo-sdk
	SDK_DEF = -DPOSIX -DCOMPILER_GCC -DGNUC 
endif
ifeq "$(GAME)" "BMS"
	SDK_VERSION = bms-sdk
	SDK_DEF = -DPOSIX -DCOMPILER_GCC -DGNUC 
endif

SDK_DIR = $(BASE)/$(SDK_VERSION)
PROJECT_DIR = $(BASE)/$(PROJECT)
SRC_DIR = $(PROJECT_DIR)/Code

OTHER_PREPROCESSORS = -DNCZ_$(GAME) $(SDK_DEF)

# Compilateur
CXX = g++

# Nom du fichier binaire de sortie
BINARY_NAME = nocheatzX.so

# Dossier de sortie du fichier binaire
BUILD_DIR = $(BASE)/Builds/$(GAME)
BINARY_DIR = $(BUILD_DIR)/addons/NoCheatZ/

# Dossiers de sortie
RELEASE_DIR = Release/linux
DEBUG_DIR = Debug/linux

# Dossier contenant les librairies dynamiques
SRCDS_BIN_DIR = ./bin

# Dossier contenant les librairies statiques
SRCDS_A_DIR = $(SDK_DIR)/lib/public/linux32
ifeq "$(GAME)" "CSGO"
	SRCDS_A_DIR = $(SDK_DIR)/lib/linux
endif

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

OPT_FLAGS = -O3 -funroll-loops -s -pipe $(OTHER_PREPROCESSORS)
DEBUG_FLAGS = -g -ggdb3 -O0 -D_DEBUG $(OTHER_PREPROCESSORS)               
# Fichiers à compiler
SRC= $(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */*/*.cpp) $(wildcard */*/*/*.cpp)                      

# Fichiers à lier
ifeq "$(GAME)" "CSS"
	LINK_SO =       $(SRCDS_BIN_DIR)/libtier0_srv.so \
			$(SRCDS_BIN_DIR)/libvstdlib_srv.so

	LINK_A =        $(SRCDS_A_DIR)/tier1.a \
			$(SRCDS_A_DIR)/mathlib.a
endif

ifeq "$(GAME)" "CSGO"
	LINK_SO =       $(SRCDS_BIN_DIR)/libtier0.so \
			$(SRCDS_BIN_DIR)/libvstdlib.so

	LINK_A =        $(SRCDS_A_DIR)/tier1_i486.a \
			$(SRCDS_A_DIR)/mathlib_i486.a \
			$(SRCDS_A_DIR)/interfaces_i486.a \
			$(SDK_DIR)/lib/linux32/release/libprotobuf.a
endif

ifeq "$(GAME)" "CSP"
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
		INCLUDE += -I$(SDK_DIR)/interfaces \
			-I$(SDK_DIR)/common/protobuf-2.5.0/src \
			-I$(SDK_DIR)/public/game/shared/csgo/protobuf \
			-I$(SDK_DIR)/public/engine/protobuf
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
        
release: prepare-build
	@$(MAKE) all DEBUG=false
	@rm -rf $(SRC_DIR)/temp_extern/
	
debug: prepare-build
	@$(MAKE) all DEBUG=true
	@rm -rf $(SRC_DIR)/temp_extern/

clean:
	@rm -rf $(RELEASE_DIR)
	@rm -rf $(DEBUG_DIR)
	@rm -rf $(BINARY_DIR)/$(BINARY_NAME)

prepare-build:
	@mkdir -p $(BUILD_DIR)/addons/NoCheatZ/
	@mkdir -p $(BUILD_DIR)/cfg/
	@mkdir -p $(BUILD_DIR)/logs/NoCheatZ_4_Logs/
	@cp ./Res/nocheatz.vdf $(BUILD_DIR)/addons/
	@cp ./Res/nocheatz-$(GAME).cfg $(BUILD_DIR)/cfg/nocheatz.cfg ;true
	@mkdir -p $(SRC_DIR)/temp_extern/
	@cp -R $(SDK_DIR)/public/engine/protobuf/ $(SRC_DIR)/temp_extern/ ;true
	@cp -R $(SDK_DIR)/public/game/shared/csgo/protobuf/ $(SRC_DIR)/temp_extern/ ;true
	@rename 's/.cc$$/.cpp/g' $(SRC_DIR)/temp_extern/**/*.cc
        
.PHONY: clean
