CC = gcc
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall -Wextra

SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

FILES := $(shell find $(SRC_DIR) -type f -name '*.cpp' | sort)
OBJS := $(FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TARGET = $(BIN_DIR)/Srowser
DEBUG = $(BIN_DIR)/Srowser_debug

.PHONY: all debug release clean

# ==================== Default ====================
all: release

# ==================== Release Build ====================
release: CXXFLAGS = -std=c++20 -O2 -Wall -Wextra
release: $(TARGET)

# ==================== Debug Build ====================
debug: CXXFLAGS = -std=c++20 -g -O0 -Wall -Wextra
debug: $(DEBUG)

# Create directories
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Linking
$(TARGET) $(DEBUG): $(OBJS) | $(BIN_DIR)
	$(CXX) $^ -o $@

# Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)