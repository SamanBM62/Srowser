CXX = g++
CXXFLAGS = -Wall -std=c++20
DEBUGFLAGS = -g -O0

# Automatically include all .cpp files in current directory
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = app
DEBUG_TARGET = app_debug
DEBUG_OBJ = $(SRC:.cpp=.dbg.o)

# Build executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Build debug executable
$(DEBUG_TARGET): $(DEBUG_OBJ)
	$(CXX) $(DEBUG_OBJ) -o $(DEBUG_TARGET)

# Generic rule to build .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generic rule to build debug .o files
%.dbg.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJ) $(TARGET) $(DEBUG_OBJ) $(DEBUG_TARGET)