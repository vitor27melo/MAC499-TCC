#ifndef IMAGE_H
#define IMAGE_H

#include <SDL2/SDL_image.h>

class Image
{
public:
    SDL_Surface *surface;

    Image(char *path)
    {
        surface = NULL; 
        surface = IMG_Load( path ); 

        if( surface == NULL ) {
            fprintf(stderr, "Não foi carregar o arquivo de imagem: %s\n", path );
        }

        // Arquivos de imagem geralmente consideram a margem superioresquerdo como à que possui coordenadas
        // (0,0), enquanto que o padrão para texturas OpenGL é que a margem inferior esquerda possua estas coordenadas.
        flipSurfaceVertical();
    }

    void flipSurfaceVertical() 
    {
        // Permite acesso ao ponteiro pixels da SDL_Surface
        SDL_LockSurface(surface);
        
        int pitch = surface->pitch;
        char* temp = new char[pitch];
        char* pixels = (char*) surface->pixels;
        
        for(int i = 0; i < surface->h / 2; ++i) {
            char* row1 = pixels + i * pitch;
            char* row2 = pixels + (surface->h - i - 1) * pitch;
            
            memcpy(temp, row1, pitch);
            memcpy(row1, row2, pitch);
            memcpy(row2, temp, pitch);
        }
        
        delete[] temp;

        SDL_UnlockSurface(surface);
    }
};
#endif