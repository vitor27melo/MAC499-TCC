all: game
	@echo "DONE"

#Compiladores
CXX=g++
CC=gcc 


game: Entidades.o drawFunctions.o game.cpp 
	$(CXX) Entidades.o drawFunctions.o -O3 game.cpp glad/src/gl.c -I./glad/include/ -I./glm -lm -lSDL2main -lSDL2 -lSDL2_image -lSDL2_gfx -lSDL2_mixer -lGL -lGLU -o game

lib: drawFunctions.cpp
	$(CXX) drawFunctions.cpp -lm -lSDL2 -lSDL2_image -lSDL2_gfx -lSDL2_mixer -lGL -lGLU -c

clean:
	$(RM) *~ *.o game -f

entidades: Entidades.cpp
	$(CXX) Entidades.cpp -c