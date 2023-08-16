all: game
	@echo "DONE"

CXX=g++
CC=gcc 

game: game.cpp 
	$(CXX) game.cpp glad/src/gl.c -I./glad/include/ -I./glm -lSDL2main -lSDL2 -lSDL2_image -lSDL2_gfx -lSDL2_mixer -lm -o game
