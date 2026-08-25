CXX = g++

INCLUDE_DIR = include
SRC_DIR = src
LIB_DIR = nokilibs

BASE_CXXFLAGS = -std=c++20 -Wall -Wextra \
                -I$(INCLUDE_DIR) \
                -I$(INCLUDE_DIR)/runtime \
                -I$(INCLUDE_DIR)/runtime/objects \
                -I$(INCLUDE_DIR)/env \
                -I$(INCLUDE_DIR)/lib \
                -I$(LIB_DIR)/libcpp/dummy

SRC = \
    $(SRC_DIR)/ast.cpp \
    $(SRC_DIR)/builtin.cpp \
    $(SRC_DIR)/interpreter.cpp \
    $(SRC_DIR)/lexer.cpp \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/parser.cpp \
    $(SRC_DIR)/runner.cpp \
    $(SRC_DIR)/runtime/runtime.cpp \
    $(SRC_DIR)/runtime/runtimeval.cpp \
    $(SRC_DIR)/runtime/objects/runtimeobject.cpp \
    $(SRC_DIR)/runtime/objects/runtimeobjectManager.cpp \
    $(SRC_DIR)/runtime/objects/runtimeobjectType.cpp \
    $(SRC_DIR)/runtime/objects/runtimeobjectTypeInitializer.cpp \
    $(SRC_DIR)/runtime/objects/runtimefunc.cpp \
    $(SRC_DIR)/env/environment.cpp \
    $(SRC_DIR)/lib/libmanager.cpp \
    $(LIB_DIR)/libcpp/dummy/dummy.cpp

DEBUG_DIR = bin/debug
NORMAL_DIR = bin/normal

DEBUG_OBJ = $(patsubst %.cpp,$(DEBUG_DIR)/%.o,$(SRC))
NORMAL_OBJ = $(patsubst %.cpp,$(NORMAL_DIR)/%.o,$(SRC))


debug: nokifer-debug

nokifer-debug: $(DEBUG_OBJ)
	$(CXX) $(DEBUG_OBJ) -o $@

$(DEBUG_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(BASE_CXXFLAGS) -g -O0 -DDEBUG -c $< -o $@


optimized: nokifer

nokifer: $(NORMAL_OBJ)
	$(CXX) $(NORMAL_OBJ) -o $@

$(NORMAL_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(BASE_CXXFLAGS) -O3 -c $< -o $@


clean:
	rm -rf bin nokifer nokifer-debug


.PHONY: debug optimized clean