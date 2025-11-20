BIN_DIR = bin
INCLUDE_DIR = include
OBJ_DIR = obj
RC_DIR = rc
SRC_DIR = src
DEF_DIR = def
TEST_DIR = test

RC_FILE = $(RC_DIR)\main.rc
COMMON_DEF = $(DEF_DIR)\Common.def
DALIB_DEF = $(DEF_DIR)\DALib.def
DACOM_DEF = $(DEF_DIR)\Dacom.def
DEPS_FILE = makefile.deps

RES_FILE = $(OBJ_DIR)\main.RES
OBJ_FILES = $(OBJ_DIR)\fl_math.obj \
$(OBJ_DIR)\main.obj \
$(OBJ_DIR)\feature_config.obj \
$(OBJ_DIR)\config_reader.obj \
$(OBJ_DIR)\Freelancer.obj \
$(OBJ_DIR)\update.obj \
$(OBJ_DIR)\utils.obj \
$(OBJ_DIR)\logger.obj \
$(OBJ_DIR)\version_check.obj \
$(OBJ_DIR)\waypoint.obj \
$(OBJ_DIR)\waypoint_names.obj \
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
$(OBJ_DIR)\infocards.obj \
$(OBJ_DIR)\temp_fixes.obj \
$(OBJ_DIR)\save_crash.obj \
$(OBJ_DIR)\alchemy_crash.obj \
$(OBJ_DIR)\blank_faction.obj \
$(OBJ_DIR)\server_filter.obj \
$(OBJ_DIR)\dll_crash.obj \
$(OBJ_DIR)\shield_capacity.obj \
$(OBJ_DIR)\dealer_fixes.obj \
$(OBJ_DIR)\cheat_detection.obj \
$(OBJ_DIR)\group_members.obj

COMMON_LIB = $(OBJ_DIR)\Common.lib
DALIB_LIB = $(OBJ_DIR)\DALib.lib
DACOM_LIB = $(OBJ_DIR)\DACOM.lib
EXTERNAL_LIBS = User32.lib Gdi32.lib

OUTPUT_FILE = $(BIN_DIR)\FLSharp.dll

CPP_FLAGS = /c /O2 /Oi /nologo /W3 /WX /GL /LD /MT /EHsc /Zc:threadSafeInit- /DNDEBUG /DUSE_ST6
LD_FLAGS = /LTCG:incremental /DLL /NOLOGO /RELEASE /DEBUG
LIB_FLAGS = /NOLOGO /MACHINE:X86
RC_FLAGS = /nologo

$(OUTPUT_FILE): $(OBJ_FILES) $(RES_FILE) $(COMMON_LIB) $(DALIB_LIB) $(DACOM_LIB) $(BIN_DIR)
    link $(OBJ_FILES) $(EXTERNAL_LIBS) $(COMMON_LIB) $(DALIB_LIB) $(DACOM_LIB) $(RES_FILE) $(LD_FLAGS) /OUT:$(OUTPUT_FILE)

{$(SRC_DIR)}.cpp{$(OBJ_DIR)}.obj::
    $(CPP) $(CPP_FLAGS) $< -I$(INCLUDE_DIR) /Fo./$(OBJ_DIR)/

$(RES_FILE): $(RC_FILE) $(OBJ_DIR) makefile
    rc $(RC_FLAGS) /fo $(RES_FILE) $(RC_FILE)

$(COMMON_LIB): $(COMMON_DEF) $(INCLUDE_DIR)/Common.h makefile
    lib $(LIB_FLAGS) /def:$(COMMON_DEF) /name:COMMON /out:$(COMMON_LIB)

$(DALIB_LIB): $(DALIB_DEF) $(INCLUDE_DIR)/DALib.h makefile
    lib $(LIB_FLAGS) /def:$(DALIB_DEF) /name:DALIB /out:$(DALIB_LIB)

$(DACOM_LIB): $(DACOM_DEF) $(INCLUDE_DIR)/Dacom.h makefile
    lib $(LIB_FLAGS) /def:$(DACOM_DEF) /name:DACOM /out:$(DACOM_LIB)

$(OBJ_DIR):
    if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(BIN_DIR):
    if not exist $(BIN_DIR) mkdir $(BIN_DIR)

CATCH_SRC_FILE = $(TEST_DIR)\catch_amalgamated.cpp
CATCH_H_FILE = $(TEST_DIR)\catch_amalgamated.hpp
TEST_SRC_FILE = $(TEST_DIR)\test.cpp
TEST_H_FILE = $(TEST_DIR)\test.h

CATCH_OBJ_FILE = $(OBJ_DIR)\catch_amalgamated.obj
TEST_OBJ_FILE = $(OBJ_DIR)\test.obj
TEST_OUTPUT_FILE = $(BIN_DIR)\test.exe

TEST_CPP_FLAGS = /c /nologo /W3 /WX /EHsc /DNDEBUG /DUSE_ST6
TEST_LD_FLAGS = /NOLOGO /RELEASE

test: $(TEST_OUTPUT_FILE)
    $(TEST_OUTPUT_FILE)

$(TEST_OUTPUT_FILE): $(TEST_OBJ_FILE) $(CATCH_OBJ_FILE)
    link $(TEST_OBJ_FILE) $(CATCH_OBJ_FILE) $(TEST_LD_FLAGS) /OUT:$(TEST_OUTPUT_FILE)

{$(TEST_DIR)}.cpp{$(OBJ_DIR)}.obj::
    $(CPP) $(TEST_CPP_FLAGS) $< -I$(INCLUDE_DIR) /Fo./$(OBJ_DIR)/

clean:
    del $(BIN_DIR)\*.dll $(BIN_DIR)\*.exe $(BIN_DIR)\*.pdb $(BIN_DIR)\*.ipdb $(BIN_DIR)\*.iobj $(BIN_DIR)\*.ilk $(OBJ_DIR)\*.obj $(OBJ_DIR)\*.RES $(OBJ_DIR)\*.lib $(OBJ_DIR)\*.pdb $(OBJ_DIR)\*.exp

$(TEST_SRC_FILE): $(TEST_H_FILE) $(CATCH_H_FILE) makefile

!INCLUDE $(DEPS_FILE)
