#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_opengl.h>

#include "shader_s.h"

#include <iostream>

static SDL_Window *Window;
static SDL_GLContext Context;

void panic_exit(int code) {
    SDL_Quit();
    exit(code);
}

void setup_libraries() {
    // O SDL será a biblioteca responsável por gerenciar user input e o contexto do OpenGL
    // Inicializa a biblioteca
    if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) {
        fprintf( stderr, "Inicialização do SDL falhou: %s\n", SDL_GetError());
        panic_exit(-1);
    }

    SDL_GL_LoadLibrary(NULL);

    // Configuramos alguns atributos antes da criação da janela
    // Aumentamos a quantidade de bits disponíveis p/ as variáveis de cor de 3 p/ 5
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    // Cria uma janela SDL centrada na tela
    Window =  SDL_CreateWindow("O Pesadelo de Fluffy",
		// SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED,
        0,0,
		1067, 600,
		SDL_WINDOW_OPENGL);
    
    if (!Window) {
        fprintf(stderr, "SDL_CreateWindow falhou para a versão atual do OpenGL: %s\n", SDL_GetError());
        panic_exit(-1);
    }

    // Cria um contexto OpenGL e o associa à janela recem criada
    Context = SDL_GL_CreateContext(Window);

    if (!Context) {
        fprintf(stderr, "SDL_GL_CreateContext falhou: %s\n", SDL_GetError());
        panic_exit(-1);
    }

    // Inicializa o SDL_mixer, responsável pelo áudio do jogo
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        fprintf(stderr, "Não foi possível inicializar a biblioteca SDL_mixer: %s\n", Mix_GetError() );
    }

    // O GLAD será responsável por linkar as funções do OpenGL da GPU ao SO usuário
    // // Carrega os links das funções do OpenGL da versão instalada
    int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);

    if (version == 0) {
        printf("GLAD não conseguiu recuperar funções OpenGL\n");
        panic_exit(-1);
    }
    // printf("Utilizando OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
}



void test2() {
    Shader ourShader("./shaders/shader.vs", "./shaders/shader.fs");

    float vertices[] = {
        // positions         // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };  

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 


    glBindVertexArray(0); 



    GLenum  erro;
    erro = glGetError();  
    fprintf(stderr, "%d", erro);
    SDL_Event event;
    // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    // float greenValue;
    while (1)
    {
        while( SDL_PollEvent( &event ) ) {
            switch( event.type ) {
                case SDL_KEYDOWN:
                    fprintf(stderr, "keydown");
                break;
                case SDL_QUIT:
                    panic_exit( 0 );
                break;
            }
        }
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // // draw our first triangle

        ourShader.use();
        // glUniform4f(vertexColorLocation, 0.0f, (sin(SDL_GetTicks()*1000) / 2.0f) + 0.5f, 0.0f, 1.0f);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0); // no need to unbind it every time 
        SDL_GL_SwapWindow(Window);
    }
}

int main() {
    // Cria e configura o contexto OpenGL
    setup_libraries();

    // LEVAR p/ uma função/arquivo loadMenu
    Mix_Music *gMusic = NULL;
    gMusic = Mix_LoadMUS( "assets/sounds/looperman-l-5041336-0314853-subspace-club-type-sample.wav" );
    // Mix_PlayMusic( gMusic, -1 );

    // test();
    test2();

    std::cin.ignore();
    return 0;
}