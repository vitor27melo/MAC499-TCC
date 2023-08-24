#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL_image.h>
#include <glad/gl.h>

class Texture
{
public:
    SDL_Surface *surface;
    unsigned int ID;

    Texture(char *path)
    {
        surface = NULL; 
        surface = IMG_Load( path ); 

        if( surface == NULL ) {
            fprintf(stderr, "ERRO:SDL_IMAGE:Não foi possível carregar o arquivo de imagem: %s\n", path );
        }

        // Arquivos de imagem geralmente consideram a margem superioresquerdo como à que possui coordenadas
        // (0,0), enquanto que o padrão para texturas OpenGL é que a margem inferior esquerda possua estas coordenadas.
        flipSurfaceVertical();

        glGenTextures(1, &ID);
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

    void use()
    {
        glBindTexture(GL_TEXTURE_2D, ID);  

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
        glGenerateMipmap(GL_TEXTURE_2D); 
    }

private:
    bool freeImage() {
        free(surface);
        return true;
    }

};
#endif