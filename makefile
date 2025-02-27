# VC6 makefile

BIN_DIR = bin
INCLUDE_DIR = include
OBJ_DIR = obj
RC_DIR = rc
SRC_DIR = src
DEF_DIR = def

RC_FILE = $(RC_DIR)\main.rc
COMMON_DEF = $(DEF_DIR)\Common.def
DALIB_DEF = $(DEF_DIR)\DALib.def
DEPS_FILE = makefile.deps

RES_FILE = $(OBJ_DIR)\main.RES
OBJ_FILES = $(OBJ_DIR)\fl_math.obj \
$(OBJ_DIR)\main.obj \
$(OBJ_DIR)\feature_config.obj \
$(OBJ_DIR)\config_reader.obj \
$(OBJ_DIR)\Freelancer.obj \
$(OBJ_DIR)\update.obj \
$(OBJ_DIR)\utils.obj \
$(OBJ_DIR)\waypoint.obj \
$(OBJ_DIR)\projectiles.obj \
$(OBJ_DIR)\resolutions.obj \
$(OBJ_DIR)\resolutions_asm.obj \
$(OBJ_DIR)\test_sounds.obj \
$(OBJ_DIR)\trade_lane_lights.obj \
$(OBJ_DIR)\copy_paste.obj \
$(OBJ_DIR)\ui_anim.obj \
$(OBJ_DIR)\weapon_anim.obj \
$(OBJ_DIR)\flash_particles.obj \
$(OBJ_DIR)\rep_requirements.obj \
$(OBJ_DIR)\cgun_wrapper.obj \
$(OBJ_DIR)\temp_fixes.obj

COMMON_LIB = $(OBJ_DIR)\Common.lib
DALIB_LIB = $(OBJ_DIR)\DALib.lib
EXTERNAL_LIBS = User32.lib Gdi32.lib

OUTPUT_FILE = $(BIN_DIR)\FLSharp.dll

CPP_FLAGS = /c /O2 /nologo /W3 /WX /LD /MT /EHsc /D_CRT_SECURE_NO_WARNINGS /D_CRT_NON_CONFORMING_SWPRINTFS /D_CRT_NONSTDC_NO_DEPRECATE /DUSE_ST6
LD_FLAGS = /DLL /NOLOGO /RELEASE /DEBUG
LIB_FLAGS = /NOLOGO /MACHINE:IX86

$(OUTPUT_FILE): $(OBJ_FILES) $(RES_FILE) $(COMMON_LIB) $(DALIB_LIB) $(BIN_DIR)
    link $(OBJ_FILES) $(EXTERNAL_LIBS) $(COMMON_LIB) $(DALIB_LIB) $(RES_FILE) $(LD_FLAGS) /OUT:$(OUTPUT_FILE)

{$(SRC_DIR)}.cpp{$(OBJ_DIR)}.obj::
    $(CPP) $(CPP_FLAGS) $< -I$(INCLUDE_DIR) /Fo./$(OBJ_DIR)/

$(RES_FILE): $(RC_FILE) $(OBJ_DIR) makefile
    rc /fo $(RES_FILE) $(RC_FILE)

$(COMMON_LIB): $(COMMON_DEF) makefile
    lib $(LIB_FLAGS) /def:$(COMMON_DEF) /name:COMMON /out:$(COMMON_LIB)

$(DALIB_LIB): $(DALIB_DEF) makefile
    lib $(LIB_FLAGS) /def:$(DALIB_DEF) /name:DALIB /out:$(DALIB_LIB)

$(OBJ_DIR):
    if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(BIN_DIR):
    if not exist $(BIN_DIR) mkdir $(BIN_DIR)

clean:
    del $(BIN_DIR)\*.dll $(OBJ_DIR)\*.obj $(OBJ_DIR)\*.RES $(OBJ_DIR)\*.lib

!INCLUDE $(DEPS_FILE)
