# Dossier de travail
BASE_DIR = ../

GAMEDIR = CSS
SDKREDIR = CSS
TIER0SO_CSP = tier0_i486.so
TIER0SO_CSS = libtier0_srv.so
TIER0 = $(TIER0SO_CSS)
OTHER_PREPROCESSORS = -DNCZ_CSS -DCOMPILER_GCC -DPOSIX

# Compilateur
#CXX = g++-3.4 # maximise la compatibilité
#CXX = g++-4.1 # disponible sur les distri récentes
CXX = g++

# Nom du fichier binaire de sortie
BINARY_NAME = nocheatz3_autoupdate.so

# Dossier de sortie du fichier binaire
BINARY_DIR = $(BASE_DIR)/Releases/nocheatz3-$(GAMEDIR)/addons/nocheatz/

# Code source du SDK de VALVE
SDK_SRC_DIR = $(BASE_DIR)/$(SDKREDIR)
SDK_PUBLIC_DIR = $(SDK_SRC_DIR)/public
SDK_TIER0_DIR = $(SDK_SRC_DIR)/public/tier0
SDK_TIER1_DIR = $(SDK_SRC_DIR)/tier1
SDK_GAME_DIR = $(SDK_SRC_DIR)/game
SDK_GAMESHARED_DIR = $(SDK_GAME_DIR)/shared
SDK_GAMESERVER_DIR = $(SDK_GAME_DIR)/server

# Dossiers de sortie
RELEASE_DIR = Release/linux
DEBUG_DIR = Debug/linux

# Dossier contenant les librairies dynamiques
#SRCDS_DIR = ./
SRCDS_BIN_DIR = bin

# Dossier contenant les librairies statiques
SRCDS_A_DIR = $(SDK_SRC_DIR)/lib/linux

# Paramètres du compilateur
ARCH_CFLAGS = -mtune=i486 -march=pentium  -mmmx
USER_CFLAGS = -DTIXML_USE_TICPP
BASE_CFLAGS =   -msse \
                                -fpermissive \
                                -D_LINUX \
                                -DNDEBUG \
                                -Dstricmp=strcasecmp \
                                -D_stricmp=strcasecmp \
                                -D_strnicmp=strncasecmp \
                                -Dstrnicmp=strncasecmp \
                                -D_snprintf=snprintf \
                                -D_vsnprintf=vsnprintf \
                                -D_alloca=alloca \
                                -Dstrcmpi=strcasecmp \
                                -fPIC \
                                -Wno-deprecated \
                                -msse 
OPT_FLAGS = -O3 -funroll-loops -s -pipe $(OTHER_PREPROCESSORS)
#DEBUG_FLAGS = -g -ggdb3 -O0 -D_DEBUG $(OTHER_PREPROCESSORS)               
DEBUG_FLAGS = $(OPT_FLAGS)
#OPT_FLAGS = $(DEBUG_FLAGS)
# Fichiers à compiler
SRC= $(wildcard *.cpp) 
# $(wildcard */*.cpp) $(wildcard */*/*.cpp)                        

# Fichiers à lier
LINK_SO =       $(SRCDS_BIN_DIR)/$(TIER0)                 
LINK_A =        $(SRCDS_A_DIR)/tier1_i486.a \
			#	$(SRCDS_A_DIR)/interfaces_i486.a \
				$(SRCDS_A_DIR)/mathlib_i486.a

LINK = -lm -ldl --export-dynamic $(LINK_A) $(LINK_SO)

# Dossiers des fichiers inclus
INCLUDE =       -I. \
                        -I$(SDK_PUBLIC_DIR) \
                        -I$(SDK_PUBLIC_DIR)/engine \
                        -I$(SDK_PUBLIC_DIR)/tier0 \
                        -I$(SDK_PUBLIC_DIR)/tier1 \
                        -I$(SDK_PUBLIC_DIR)/vstdlib \
						-I$(SDK_PUBLIC_DIR)/mathlib \
                        -I$(SDK_PUBLIC_DIR)/game/server \
                        -I$(SDK_SRC_DIR)/tier1 \
                        -I$(SDK_SRC_DIR)/game \
                        -I$(SDK_SRC_DIR)/game/server \
                        -I$(SDK_SRC_DIR)/game/shared \
						-I$(SDK_SRC_DIR)/public \
						-I$(SDK_SRC_DIR)/public/game/server \
						-I$(SDK_SRC_DIR)/public/engine \
						-I$(SDK_SRC_DIR)/public/tier0 \
						-I$(SDK_SRC_DIR)/public/tier1 \
						-I$(SDK_SRC_DIR)/common \
						-I$(SDK_SRC_DIR)/interfaces \
                        

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
	$(CXX) $(INCLUDE) $(CFLAGS) -o $(3) -c $(1) 2> "/sdk/error_triggerbot.txt";

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