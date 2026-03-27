CXX = g++
CXXFLAGS = -Wall -std=c++17

# Automatically include all .cpp files in current directory
# Pick up all .cpp files in current directory except DOM.cpp
SRC = $(filter-out DOM.cpp, $(wildcard *.cpp))
OBJ = $(SRC:.cpp=.o)
TARGET = app

# Build executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Generic rule to build .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Clean
clean:
	rm -f $(OBJ) $(TARGET)