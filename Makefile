
# Global Makefile for Arkanoid game using SFML

# variables and settings
CXX      = g++ # Compiler
CXXFLAGS = -Iinclude -I"C:/msys64/ucrt64/include" # Compiler flags
LDFLAGS  = -L"C:/msys64/ucrt64/lib" -mconsole # Linker flags
LIBS     = -lsfml-graphics -lsfml-window -lsfml-system # Libraries

# Directories and target (to work regularly on both Windows and Unix-like systems)
SRC_DIR = src
OBJ_DIR = obj
TARGET  = game.exe

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Rules
all: $(TARGET) 

$(TARGET): $(OBJS) # Link the object files to create the executable
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) # Compile source files to object files
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR): # Create the object directory if it doesn't exist
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR) 2>nul || mkdir -p $(OBJ_DIR)

run: all # Run the game after building
	./$(TARGET)

clean: # Clean up object files and the executable
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR) 2>nul || rm -rf $(OBJ_DIR)
	@if exist $(TARGET) del /q $(TARGET) 2>nul || rm -f $(TARGET)