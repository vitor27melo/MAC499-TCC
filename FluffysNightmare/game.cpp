#include "image_s.h"
#include "shader_s.h"
#include "camera_s.h"

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>

static SDL_Window *Window;
static SDL_GLContext Context;

unsigned int SCR_WIDTH = 1067;
unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float xpos = 0.0f;
float ypos = 0.0f;
float xoffset = 0.0f;
float yoffset = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.1f;	// time between current frame and last frame
float lastFrame = 0.0f;

void panicExit(int code) {
    fprintf(stderr, "FATAL:Foi preciso fnalizar oprocesso\n");
    SDL_Quit();
    exit(code);
}

//TODO
void windowResize(int width, int heigth) {
    
}

void processEvents(SDL_Event event) {
    while( SDL_PollEvent( &event ) ) {
        switch( event.type ) {
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        windowResize(event.window.data1, event.window.data2);
                    break;
                }
            break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        camera.ProcessKeyboard(FORWARD, deltaTime);
                    break;
                    case SDLK_s:
                        camera.ProcessKeyboard(BACKWARD, deltaTime);
                    break;
                    case SDLK_a:
                        camera.ProcessKeyboard(LEFT, deltaTime);
                    break;
                    case SDLK_d:
                        camera.ProcessKeyboard(RIGHT, deltaTime);
                    break;
                }
                
            break;
            case SDL_MOUSEMOTION:
                if (event.motion.state & SDL_BUTTON_LMASK) {
                    
                    xpos = static_cast<float>(event.motion.x);
                    ypos = static_cast<float>(event.motion.y);

                    if (firstMouse)
                    {
                        lastX = xpos;
                        lastY = ypos;
                        firstMouse = false;
                    }

                    xoffset = xpos - lastX;
                    yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

                    lastX = xpos;
                    lastY = ypos;

                    camera.ProcessMouseMovement(xoffset, yoffset);
                }
            break;
            case SDL_QUIT:
                panicExit( 0 );
            break;
        }
    }
}

void setupLibraries() {
    // O SDL será a biblioteca responsável por gerenciar user input e o contexto do OpenGL
    // Inicializa a biblioteca
    if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) {
        fprintf( stderr, "ERRO:SDL:Inicialização do SDL falhou: %s\n", SDL_GetError());
        panicExit(-1);
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
		SCR_WIDTH, SCR_HEIGHT,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    
    if (!Window) {
        fprintf(stderr, "ERRO:SDL:SDL_CreateWindow falhou para a versão atual do OpenGL: %s\n", SDL_GetError());
        panicExit(-1);
    }

    // Cria um contexto OpenGL e o associa à janela recem criada
    Context = SDL_GL_CreateContext(Window);

    if (!Context) {
        fprintf(stderr, "ERRO:SDL:SDL_GL_CreateContext falhou: %s\n", SDL_GetError());
        panicExit(-1);
    }

    // Inicializa o SDL_mixer, responsável pelo áudio do jogo
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        fprintf(stderr, "ERRO:SDL_MIXER:Não foi possível inicializar a biblioteca SDL_mixer: %s\n", Mix_GetError() );
    }

    // O GLAD será responsável por linkar as funções do OpenGL da GPU ao SO usuário
    // // Carrega os links das funções do OpenGL da versão instalada
    int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);

    if (version == 0) {
        printf("ERRO:GLAD:Não foi possível recuperar funções OpenGL\n");
        panicExit(-1);
    }
    // printf("Utilizando OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    // Configuração de constantes

    // Diz para o OpenGL calcular e comparar a profundeza dos vértices
    glEnable(GL_DEPTH_TEST); 
}

void test() {
    Shader ourShader("./shaders/shader.vs", "./shaders/shader.fs");

    float vertices[] = {
        // positions          // texture coords
        1.0f,  1.0f, 0.0f,   1.0f, 1.0f,   // top right
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3
    };


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
 

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0); 


    Image wall((char*)"./assets/textures/telas_menu/1.png");
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wall.surface->w, wall.surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, wall.surface->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);
    

    GLenum  erro;
    erro = glGetError();  
    fprintf(stderr, "%d", erro);
    SDL_Event event;

    glm::mat4 trans = glm::mat4(1.0f);

    unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
    

    while (1)
    {
        while( SDL_PollEvent( &event ) ) {
            switch( event.type ) {
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            windowResize(event.window.data1, event.window.data2);
                        break;
                    }
                break;
                case SDL_KEYDOWN:
                    fprintf(stderr, "keydown");
                break;
                case SDL_QUIT:
                    panicExit( 0 );
                break;
            }
        }
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // // draw our first triangle

        trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, (float)(SDL_GetTicks()/100), glm::vec3(0.0f, 0.0f, 1.0f));

        ourShader.use();
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        // glUniform4f(vertexColorLocation, 0.0f, (sin(SDL_GetTicks()*1000) / 2.0f) + 0.5f, 0.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // no need to unbind it every time 
        SDL_GL_SwapWindow(Window);
    }
}

void test2() {
    Shader ourShader("./shaders/shader.vs", "./shaders/shader.fs");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3
    };


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
 

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0); 


    Image wall((char*)"./assets/textures/telas_menu/1.png");
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wall.surface->w, wall.surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, wall.surface->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);
    

    GLenum  erro;
    erro = glGetError();  
    fprintf(stderr, "%d", erro);
    

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 

    glm::mat4 view = camera.GetViewMatrix();
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(camera.Zoom), 1067.0f / 600.0f, 0.1f, 100.0f);

    int modelLoc = glGetUniformLocation(ourShader.ID, "model");
    int viewLoc = glGetUniformLocation(ourShader.ID, "view");
    int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");


    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    const float cameraSpeed = 0.05f;

    

    SDL_Event event;
    while (1)
    {
        // Processa entradas de mouse e de teclado
        processEvents(event);
        // Limpamos, a cada frame, o buffer de profundidade e o buffer de cor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Cor de fundo do mundo
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // Pede p/ o OpenGL usar nosso programa de shaders
        ourShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        // glUniform4f(vertexColorLocation, 0.0f, (sin(SDL_GetTicks()*1000) / 2.0f) + 0.5f, 0.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0); // no need to unbind it every time 
        SDL_GL_SwapWindow(Window);
        
    }
}

int main() {
    // Cria e configura o contexto OpenGL
    setupLibraries();

    // LEVAR p/ uma função/arquivo loadMenu
    Mix_Music *gMusic = NULL;
    gMusic = Mix_LoadMUS( "assets/sounds/looperman-l-5041336-0314853-subspace-club-type-sample.wav" );
    // Mix_PlayMusic( gMusic, -1 );

    test2();

    std::cin.ignore();
    return 0;
}