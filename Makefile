# Compiler settings - Can change based on your compiler setup
CXX = clang++
CPPFLAGS = -I/opt/homebrew/Cellar/sfml/2.6.1/include
LDFLAGS = -L/opt/homebrew/Cellar/sfml/2.6.1/lib
LDLIBS = -lsfml-graphics -lsfml-window -lsfml-system
# Source files
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Name of your executable
TARGET = ray_tracer

# Default build with debug information and no optimizations
CXXFLAGS = -std=c++20 -stdlib=libc++ -g -Wall

# Release build with optimizations
release: CXXFLAGS = -std=c++20 -stdlib=libc++ -O3 -Wall
release: clean $(TARGET)

# Default target
all: $(TARGET)

# Link the target with object files
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile each source file to an object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJECTS)

# Run the target for development
run-dev: $(TARGET)
	./$(TARGET)

# Run the target for release
run: release
	./$(TARGET)

.PHONY: all clean run-dev run release