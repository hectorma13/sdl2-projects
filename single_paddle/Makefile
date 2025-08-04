# Universidad de La Laguna
# Escuela Superior de Ingeniería y Tecnología
# Grado en Ingeniería Informática
#
# Makefile for compiling the SDL2-based single paddle Pong game

# Compiler
CXX = g++

# Target binary
TARGET = single_paddle

# Source file
SRC = single_paddle.cc

# Compiler flags
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17

# SDL2 flags (uses pkg-config to get correct includes and libs)
SDLFLAGS = $(shell pkg-config --cflags --libs sdl2 SDL2_ttf)

# Default rule
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(SDLFLAGS)

# Clean rule
clean:
	rm -f $(TARGET)
