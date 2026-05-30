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

.PHONY: all clean

all: $(TARGET)

# Create directories
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Linking
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $^ -o $@

# Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)