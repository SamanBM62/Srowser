CC = gcc
CXX = g++

# ==================== Compiler Settings ====================
CPPFLAGS = $(shell pkg-config --cflags freetype2 glfw3)
CPPFLAGS += -Iexternal/glad/include

CXXFLAGS = -std=c++20 -Wall -Wextra

LDLIBS = -lX11 -lGL
LDLIBS += $(shell pkg-config --libs freetype2 glfw3)

# ==================== Directories ====================

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

DEBUG_OBJ_DIR = $(BUILD_DIR)/debug/obj
RELEASE_OBJ_DIR = $(BUILD_DIR)/release/obj

# ==================== GLAD ====================

GLAD_SRC = external/glad/src/gl.c

DEBUG_GLAD_OBJ = $(DEBUG_OBJ_DIR)/glad.o
RELEASE_GLAD_OBJ = $(RELEASE_OBJ_DIR)/glad.o

# ==================== Source Files ====================

FILES := $(shell find $(SRC_DIR) -type f -name '*.cpp' | sort)

DEBUG_OBJS := $(FILES:$(SRC_DIR)/%.cpp=$(DEBUG_OBJ_DIR)/%.o)
RELEASE_OBJS := $(FILES:$(SRC_DIR)/%.cpp=$(RELEASE_OBJ_DIR)/%.o)

# ==================== Targets ====================

TARGET = $(BIN_DIR)/Srowser
DEBUG = $(BIN_DIR)/Srowser_debug

.PHONY: all debug release clean

# Default target
all: release

# ==================== Release Build ====================

release: $(TARGET)

$(TARGET): $(RELEASE_OBJS) $(RELEASE_GLAD_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(LDLIBS)

$(RELEASE_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O2 -MMD -MP -c $< -o $@

$(RELEASE_GLAD_OBJ): $(GLAD_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -O2 -MMD -MP -c $< -o $@

# ==================== Debug Build ====================

debug: $(DEBUG)

$(DEBUG): $(DEBUG_OBJS) $(DEBUG_GLAD_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(LDLIBS)

$(DEBUG_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -g -O0 -MMD -MP -c $< -o $@

$(DEBUG_GLAD_OBJ): $(GLAD_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -g -O0 -MMD -MP -c $< -o $@

# ==================== Dependencies ====================

-include $(RELEASE_OBJS:.o=.d)
-include $(DEBUG_OBJS:.o=.d)
-include $(RELEASE_GLAD_OBJ:.o=.d)
-include $(DEBUG_GLAD_OBJ:.o=.d)

# ==================== Clean ====================

clean:
	rm -rf $(BUILD_DIR)