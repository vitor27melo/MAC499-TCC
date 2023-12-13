#include "shader.h"
#include "camera.h"

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

#include "drawFunctions.h"
#include "Entidades.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

// file
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define numTex 26
#define passoCam 2
#define numFases 4
#define numTelaInstrucoes 5
#define MAX_MINI_FLUFFYS 15
#define MINI_FLUFFY_LIMITER 2

int startMap = 1;
int firstRun = 1;

// "Dimension": Vector size multiplier
float d = 60;

// Number of seconds between render loops
float deltaTime = 0.001;
// How many seconds of total game time
int gameSeconds = 0;

// Maximum framerate (set 0 for unlimited)
#define MAX_FRAMERATE 0

// This valued multiplied by "deltaTime" to determine how fast things move in the game 
float speedMult = 7;


#define PI 3.142857
#define DEBUG 0
#define SQUARE(x) ((x) * (x))
#define ROUND(x) ((x < 0) ? (int)(x - 0.5) : (int)(x + 0.5))
#define SIGN(x) ((x >= 0) ? 1 : -1)

// globais
SDL_Window *Window;
int gameIsRunning = false;
int SCR_WIDTH = 1067;
int SCR_HEIGHT = 600;
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;
glm::mat4 normal;
glm::vec3 lightPos;


Camera camera;
float xpos = 0.0f;
float ypos = 0.0f;
float xoffset = 0.0f;
float yoffset = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;



// The music that will be played
Mix_Music *gMusic = NULL;

ofstream logfile;
int numTelaInstrucao = 1;
bool bloqueiaMov = false;

SDL_Rect arrowPosition;

GLuint null_tex;

enum estadosJogo
{
  Menu,
  Instrucoes,
  Pause,
  EmJogo,
  Vitoria,
  Derrota,
  TelaFinal
};
estadosJogo estadoJogo = Menu;

struct GameData
{
  int fase;
  GLuint null_tex;
  torre *Torre;
  player *Player;

  // camera
  float zoom;
  float theta_y;
  int cx, cz;

  LLBlocos *ListaUpdate;
  desfaz *Desfaz;
  int miniFluffyIndex = 0;
  int nMiniFluffysAlive = 0;       
  int nMiniFluffysRescued = 0;       
  int nMiniFluffysDead = 0;  
  miniFluffy *miniFluffys[MAX_MINI_FLUFFYS];
};

struct Tela
{
  estadosJogo nomeTela;
  SDL_Surface *background;
  int x1, x2;
  int y1, y2;
  bool selecionaOpcao1 = true;
  bool temOpcao2 = true;
  bool showArrow = true;
};


unsigned int curvedHairVAO;


SDL_Surface *loadImage(char *filename)
{
  // The image that's loaded
  SDL_Surface *loadedImage = NULL;
  // The optimized image that will be used
  // SDL_Surface* optimizedImage = NULL;Z
  // Load the image using SDL_image
  loadedImage = IMG_Load(filename);

  if (loadedImage == NULL)
  {
    printf("Error: Load Image %s\n", filename);
    exit(1);
  }
  // If the image loaded
  return loadedImage;
}


void flipSurfaceVertical(SDL_Surface *surface) 
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

void loadTexture(char* path, GLuint tex[], int index, bool alpha, bool flipVertical) {
  SDL_Surface *img = NULL;
  glBindTexture(GL_TEXTURE_2D, tex[index]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  img = loadImage(path);
  if (flipVertical) flipSurfaceVertical(img);
  if (alpha) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
  else       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);
  SDL_FreeSurface(img);

}

void setupTextures(struct GameData *GD, GLuint tex[]) {
    SDL_Surface *img = NULL;
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &GD->null_tex);
    glGenTextures(numTex, tex);

    glBindTexture(GL_TEXTURE_2D, GD->null_tex);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    img = loadImage((char *)"./textures/null.png");
    glTexImage2D(GL_TEXTURE_2D, 0,
                GL_RGB, 1, 1, 0,
                GL_RGB, GL_UNSIGNED_BYTE,
                img->pixels);
    SDL_FreeSurface(img);

    loadTexture((char*)"./textures/wall_texture2.png", tex, 0, 0, 0);
    loadTexture((char*)"./textures/wall_texture_sup2.png", tex, 1, 0, 0);
    loadTexture((char*)"./textures/block-fixo/1.png", tex, 2, 1, 0);
    loadTexture((char*)"./textures/block-fixo/5.png", tex, 3, 1, 0);
    loadTexture((char*)"./textures/block_final_fixo/1.png", tex, 4, 1, 0);
    loadTexture((char*)"./textures/block_final_fixo/2.png", tex, 5, 1, 0);
    loadTexture((char*)"./textures/block_final_fixo/3.png", tex, 6, 1, 0);
    loadTexture((char*)"./textures/block_final_fixo/4.png", tex, 7, 1, 0);
    loadTexture((char*)"./textures/block_final_movel/1.png", tex, 8, 1, 0);
    loadTexture((char*)"./textures/block_final_movel/2.png", tex, 9, 1, 0);
    loadTexture((char*)"./textures/block_final_movel/3.png", tex, 10, 1, 0);
    loadTexture((char*)"./textures/block_final_movel/4.png", tex, 11, 1, 0);
    loadTexture((char*)"./textures/wood_block/crate.png", tex, 12, 1, 0);
    loadTexture((char*)"./textures/wood_block/crate.png", tex, 13, 1, 0);
    loadTexture((char*)"./textures/wall/wall.png", tex, 14, 1, 0);
    loadTexture((char*)"./textures/wall/wall.png", tex, 15, 1, 0);
    loadTexture((char*)"./textures/final_block_transparent/1.png", tex, 16, 1, 0);
    loadTexture((char*)"./textures/final_block_transparent/1.png", tex, 17, 1, 0);
    loadTexture((char*)"./textures/block_final_movel/3.png", tex, 18, 1, 0);
    loadTexture((char*)"./textures/block_final_movel/4.png", tex, 19, 1, 0);
    loadTexture((char*)"./textures/direction_block/left.png", tex, 20, 1, 1);
    loadTexture((char*)"./textures/direction_block/right.png", tex, 21, 1, 1);
    loadTexture((char*)"./textures/direction_block/no_direction.png", tex, 22, 1, 0);
    loadTexture((char*)"./textures/fluffy_spawn/face.png", tex, 23, 1, 1);
    loadTexture((char*)"./textures/fluffy_spawn/roof.png", tex, 24, 1, 0);
    loadTexture((char*)"./textures/door/door.png", tex, 25, 1, 1);
    loadTexture((char*)"./textures/door/roof.png", tex, 26, 1, 0);
}



void spawnMiniFluffy(struct GameData *GD, block *b) {
  GD->miniFluffys[GD->miniFluffyIndex] = new miniFluffy;
  // GD->miniFluffys[GD->miniFluffyIndex]->SetMiniFluffy();
  // std::cout << "Bloco: " << b->pos->x << " " << b->pos->y << " " << b->pos->z << "\n";
  // std::cout << "minifluffy: " << pos.x << " " << pos.y << " " << pos.z << "\n";
  GD->miniFluffys[GD->miniFluffyIndex]->SetMiniFluffy(b->pos, b->tipo);
  // GD->miniFluffys[GD->miniFluffyIndex]->teste(5);
  GD->miniFluffyIndex++;
}



void miniFluffyStageUpdate(struct GameData *GD) {
  if (GD->nMiniFluffysRescued > 0 && GD->nMiniFluffysAlive == GD->nMiniFluffysRescued) {
    // std::cout << "avançou stage\n";
    // std::cout << "n stages" << GD->Torre->nStages;
    std::cout << "AUMENTOU";
    GD->Player->levelStage++;
  }
  for (int j = 0; j < GD->nMiniFluffysAlive; j++) {
    if (GD->miniFluffys[j] == NULL) continue;
    posicao posDesejada = posicao(GD->miniFluffys[j]->pos) + velocidade(GD->miniFluffys[j]->rotacao);
    block *aux = GD->Torre->retornaBloco(posDesejada);
    
    if (aux != nullptr){
      if (aux->tipo == Fixo || aux->tipo == FluffySpawnRight || aux->tipo == FluffySpawnDown || aux->tipo == FluffySpawnLeft || aux->tipo == FluffySpawnUpwards) {
        GD->miniFluffys[j]->rotacao->vx *= -1;
        GD->miniFluffys[j]->rotacao->vz *= -1;
      } else if (aux->tipo == DirecEsquerda || aux->tipo == DirecDireita) {
        if (aux->tipo == DirecEsquerda) {
          if (GD->miniFluffys[j]->rotacao->vx == 1 &&  GD->miniFluffys[j]->rotacao->vz == 0) {
            GD->miniFluffys[j]->rotacao->vx = 0;
            GD->miniFluffys[j]->rotacao->vz = -1;
          }
          else if (GD->miniFluffys[j]->rotacao->vx == -1 &&  GD->miniFluffys[j]->rotacao->vz == 0) {
            GD->miniFluffys[j]->rotacao->vx = 0;
            GD->miniFluffys[j]->rotacao->vz = 1;
          }
          else if (GD->miniFluffys[j]->rotacao->vx == 0 &&  GD->miniFluffys[j]->rotacao->vz == 1) {
            GD->miniFluffys[j]->rotacao->vx = 1;
            GD->miniFluffys[j]->rotacao->vz = 0;
          }
          else if (GD->miniFluffys[j]->rotacao->vx == 0 &&  GD->miniFluffys[j]->rotacao->vz == -1) {
            GD->miniFluffys[j]->rotacao->vx = -1;
            GD->miniFluffys[j]->rotacao->vz = 0;
          }
          
        } else if (aux->tipo == DirecDireita) {
          if (GD->miniFluffys[j]->rotacao->vx == 1 &&  GD->miniFluffys[j]->rotacao->vz == 0) {
            GD->miniFluffys[j]->rotacao->vx = 0;
            GD->miniFluffys[j]->rotacao->vz = 1;
          }
          else if (GD->miniFluffys[j]->rotacao->vx == -1 &&  GD->miniFluffys[j]->rotacao->vz == 0) {
            GD->miniFluffys[j]->rotacao->vx = 0;
            GD->miniFluffys[j]->rotacao->vz = -1;
          }
          else if (GD->miniFluffys[j]->rotacao->vx == 0 &&  GD->miniFluffys[j]->rotacao->vz == 1) {
            GD->miniFluffys[j]->rotacao->vx = -1;
            GD->miniFluffys[j]->rotacao->vz = 0;
          }
          else if (GD->miniFluffys[j]->rotacao->vx == 0 &&  GD->miniFluffys[j]->rotacao->vz == -1) {
            GD->miniFluffys[j]->rotacao->vx = 1;
            GD->miniFluffys[j]->rotacao->vz = 0;
          }
        }
          // Bloco logo após virar
          posicao posDesejada2 = posicao(GD->miniFluffys[j]->pos) + velocidade(GD->miniFluffys[j]->rotacao);
          block *aux2 = GD->Torre->retornaBloco(posDesejada2);
          if (aux2 != nullptr) {
            GD->miniFluffys[j]->rotacao->vx *= -1;
            GD->miniFluffys[j]->rotacao->vz *= -1;
          }
      } else if (aux->tipo == FluffyDestination) {
        GD->nMiniFluffysRescued++;
        std::cout << "minifluffy rescued\n";
        GD->miniFluffys[j] = NULL;
        continue;
      }
    }
    GD->miniFluffys[j]->mexe(posDesejada, velocidade(GD->miniFluffys[j]->rotacao), deltaTime * speedMult);
    GD->miniFluffys[j]->atualizaPos();
  }
}

void blockStageUpdate(struct GameData *GD, block *b) {
  int stage = GD->Player->levelStage;
  static int seconds = 0;
  if (b->tipo == ParedeQuebravel && b->levelStage < stage) GD->Torre->EjetaBloco(b);
  if (b->tipo == FluffySpawnRight || b->tipo == FluffySpawnDown || b->tipo == FluffySpawnDown || b->tipo == FluffySpawnUpwards)  std::cout << "levelstage: " << b->levelStage << " player: " << GD->Player->levelStage << "\n";
  if (
    (b->tipo == FluffySpawnRight || b->tipo == FluffySpawnDown || b->tipo == FluffySpawnDown || b->tipo == FluffySpawnUpwards) &&
    b->levelStage == GD->Player->levelStage &&
    b->miniFluffysSpawned < MINI_FLUFFY_LIMITER &&
    seconds < gameSeconds
  ) {
      seconds = gameSeconds;
      b->miniFluffysSpawned++;
      GD->nMiniFluffysAlive++;
      spawnMiniFluffy(GD, b);
  }
}

bool blockStageCheck(struct GameData *GD, block *b) {

  int stage = GD->Player->levelStage;
  if (b->alphaValue >= 1.0f) return true;
  else if (b->levelStage <= stage && !b->dissapearing) {
    b->alphaValue += deltaTime;
    return true;
  }
  return false;
}

// TODO: everytime fluffys y changes by more than 60, allow updateCamera to be called FPS times 
void updateCamera(struct GameData *GD) {
  switch(GD->Torre->stages[GD->Player->levelStage - 1]) {
    case Rapunzel:
      if (camera.Pitch > -35.0f && camera.Pitch <= 0.0f) {
        camera.ChangePitch(-1*(deltaTime * 40));
      } 
      if ((camera.Position[1] - (GD->Player->pos->y * d)) < 250.0f) {
        camera.Position[1] += deltaTime * speedMult * d;
      }
      if ((camera.Position[1] - (GD->Player->pos->y * d)) > 250.0f) {
        camera.Position[1] -= deltaTime * speedMult * d;
      }
      if ((camera.Position[0] - (GD->Player->pos->x * d)) < 250.0f) {
        camera.Position[0] += deltaTime * speedMult * d;
      }
      if ((camera.Position[0] - (GD->Player->pos->x * d)) > 250.0f) {
        camera.Position[0] -= deltaTime * speedMult * d;
      }
      if ((camera.Position[2] - (GD->Player->pos->z * d)) < -150.0f) {
        camera.Position[2] += deltaTime * speedMult * d;
      }
      if ((camera.Position[2] - (GD->Player->pos->z * d)) > -150.0f) {
        camera.Position[2] -= deltaTime * speedMult * d;
      }
      break;
    case Sokkoban:
      if (camera.Pitch > -89.0f && camera.Pitch <= 0.0f) {
        camera.ChangePitch(-1*(deltaTime * 25));
      }
      if (camera.Position[0] > (GD->Player->pos->x * d)) {
        camera.Position[0] -= deltaTime * speedMult * d;
      }
      if (camera.Position[0] < (GD->Player->pos->x * d)) {
        camera.Position[0] += deltaTime * speedMult * d;
      }
      if (camera.Position[2] > (GD->Player->pos->z * d)) {
        camera.Position[2] -= deltaTime * speedMult * d;
      }
      if (camera.Position[2] < (GD->Player->pos->z * d)) {
        camera.Position[2] += deltaTime * speedMult * d;
      }
      if (camera.Position[1] < ((GD->Player->pos->y * d) + 10*d)) {
        camera.Position[1] += deltaTime * speedMult * d;
      }
      if (camera.Position[1] > ((GD->Player->pos->y * d) + 12*d)) {
        camera.Position[1] -= deltaTime * speedMult * d;
      }
    break;
    case MiniFluffys:
      if (camera.Pitch < -50.0f) {
        camera.ChangePitch(1*(deltaTime * 25));
      }
      if ((camera.Position[1] - (GD->Player->pos->y * d)) < 400.0f) {
        camera.Position[1] += deltaTime * speedMult * d;
      }
      if ((camera.Position[1] - (GD->Player->pos->y * d)) > 400.0f) {
        camera.Position[1] -= deltaTime * speedMult * d;
      }
      if ((camera.Position[0] - (GD->Player->pos->x * d)) < 200.0f) {
        camera.Position[0] += deltaTime * speedMult * d;
      }
      if ((camera.Position[0] - (GD->Player->pos->x * d)) > 200.0f) {
        camera.Position[0] -= deltaTime * speedMult * d;
      }
      if ((camera.Position[2] - (GD->Player->pos->z * d)) < -200.0f) {
        camera.Position[2] += deltaTime * speedMult * d;
      }
      if ((camera.Position[2] - (GD->Player->pos->z * d)) > -200.0f) {
        camera.Position[2] -= deltaTime * speedMult * d;
      }
      
    break;
  }
}

int cont = 0;

void drawScreen(SDL_Window *Window,
                 struct GameData *GD,
                 GLuint tex[],
                 Shader simpleShader)
{
  int count = 0;
  cont++;
  for (andar *a = GD->Torre->primeiroAndar; a != nullptr; a = a->prox)
  {
    for (block *b = a->Lista->lista; b != nullptr; b = b->prox)
    {
      count++;
    }
  }
  if (firstRun) {
    firstRun = 0;
  }
  simpleShader.use();
  updateCamera(GD);
  static enum animacaoPlayer last_anim = AnimNormal;

  float di, dj, dk;

  // Clear the color and depth buffers.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);

  // Configura a matrix de projeção
  projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1024.0f);
  simpleShader.setMat4("projection", projection);

  // Configura a matriz view
  view = camera.GetViewMatrix();

  simpleShader.setMat4("view", view);

  // Configura a matriz modelo
  model = glm::mat4(1.0f);
  simpleShader.setMat4("model", model);

  // Configura a matriz normal
  normal = glm::mat4(1.0f);
  simpleShader.setMat4("normal", normal);

  lightPos = camera.Front;
  simpleShader.setVec3("lightPos", camera.Position);

  simpleShader.setVec3("viewPos", lightPos);

  simpleShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

  int i = 0;

  // desenha Torre
  for (andar *a = GD->Torre->primeiroAndar; a != nullptr; a = a->prox)
  {
    for (block *b = a->Lista->lista; b != nullptr; b = b->prox)
    {
      blockStageUpdate(GD, b);
      if (!blockStageCheck(GD, b)) continue;
      // count++;
      dj = b->pos->x;
      di = b->pos->y;
      dk = b->pos->z;
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(dj * d, di * d, dk * d));

      simpleShader.setMat4("model", model);
      simpleShader.setFloat("alphaVal", b->alphaValue);

      if (b->tipo == Fixo)
        drawCube(tex, GD, 2, 3);
      else if (b->tipo == FinalFixo)
        drawCube(tex, GD, 4, 5);
      else if (b->tipo == FinalFixoCompleto)
        drawCube(tex, GD, 6, 7);
      else if (b->tipo == FinalMovel)
        drawCube(tex, GD, 8, 9);
      else if (b->tipo == FinalMovelCompleto)
        drawCube(tex, GD, 10, 11);
      else if (b->tipo == ApenasEmpurra)
        drawCube(tex, GD, 12, 13);
      else if (b->tipo == Parede || b->tipo == ParedeQuebravel)
        drawCube(tex, GD, 14, 15);
      else if (b->tipo == FinalTargetIncompleto)
        drawCube(tex, GD, 16, 17);
      else if (b->tipo == FinalTargetCompleto)
        drawCube(tex, GD, 18, 19);
      else if (b->tipo == DirecEsquerda)
        drawCube(tex, GD, 20, 22);
      else if (b->tipo == DirecDireita)
        drawCube(tex, GD, 21, 22);
      else if (b->tipo == FluffySpawnRight || b->tipo == FluffySpawnDown || b->tipo == FluffySpawnLeft || b->tipo == FluffySpawnUpwards)
        drawCube(tex, GD, 23, 24);
      else if (b->tipo == FluffyDestination)
        drawCube(tex, GD, 25, 26);
      else
        drawCube(tex, GD, 0, 1);
    }
  }

  // // desenha lista updates
  for (block * b = GD->ListaUpdate->lista; b != nullptr; b = b->prox){
      if (!blockStageCheck(GD, b)) continue;
      // count ++;
      dj = b->pos->x;
      di = b->pos->y;
      dk = b->pos->z;

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(dj * d, di * d, dk * d));
      simpleShader.setMat4("model", model);
      if (b->tipo == Fixo) drawCube(tex, GD, 2, 3);
      else if (b->tipo == FinalFixo) drawCube(tex, GD, 4, 5);
      else if (b->tipo == FinalFixoCompleto) drawCube(tex, GD, 6, 7);
      else if (b->tipo == FinalMovel) drawCube(tex, GD, 8, 9);
      else if (b->tipo == FinalMovelCompleto) drawCube(tex, GD, 10, 11);
      else if (b->tipo == ApenasEmpurra) drawCube(tex, GD, 12, 13);
      else if (b->tipo == Parede || b->tipo == ParedeQuebravel) drawCube(tex, GD, 14, 15);
      else if (b->tipo == FinalTargetIncompleto) drawCube(tex, GD, 16, 17);
      else if (b->tipo == FinalTargetCompleto) drawCube(tex, GD, 18, 19);
      else if (b->tipo == DirecEsquerda) drawCube(tex, GD, 20, 22);
      else if (b->tipo == DirecDireita) drawCube(tex, GD, 21, 22);
      else drawCube(tex, GD, 0, 1);
  }

  // desenha player
  bool restart = last_anim != GD->Player->animacao;
  last_anim = GD->Player->animacao;

  // TESTE MINI FLUFFY
  for (int j = 0; j < GD->miniFluffyIndex; j++) {
    if (GD->miniFluffys[j] != NULL) {
      DrawFluffyWalk(simpleShader.ID, deltaTime, restart, GD->miniFluffys[j]->pos, GD->miniFluffys[j]->rotacao, 0.5f);
    }
  }

  // std::cout << "nblock " << count << "\n";
  simpleShader.setFloat("alphaVal", 1.0f);
  switch (GD->Player->animacao)
  {
  case AnimNormal:
    DrawFluffy(simpleShader.ID, deltaTime, restart, GD->Player->pos, GD->Player->rotacao, 1.0f);
  break;

  case AnimAnda:
    DrawFluffyWalk(simpleShader.ID, deltaTime, restart, GD->Player->pos, GD->Player->rotacao, 1.0f);
    break;

  case AnimPuxa:
    DrawFluffyPull(GD, simpleShader.ID, deltaTime, restart);
    break;

  case AnimEmpurra:
      DrawFluffyPush(GD, simpleShader.ID, deltaTime, restart);
    break;

  case AnimPendurado:
    DrawFluffyHang(GD, simpleShader.ID, deltaTime, restart);
    break;

  case AnimPenduradoDir:
    DrawFluffyHangRight(GD, simpleShader.ID, deltaTime, restart);
    break;

  case AnimPenduradoEsq:
    DrawFluffyHangLeft(GD, simpleShader.ID, deltaTime, restart);
    break;

  default:
    break;
  }
  
  SDL_GL_SwapWindow(Window);
}

static void setupOpenGL(int width, int height)
{
  float ratio = (float)width / (float)height;
  static GLfloat light_pos[] = {0.0f, 0.0f, 0.0f, 1.0f};
  static GLfloat light_amb[] = {0.7f, 0.7f, 0.7f, 1.0f};
  static GLfloat light_dif[] = {0.8f, 0.8f, 0.8f, 1.0f};
  static GLfloat light_spe[] = {1.0f, 1.0f, 1.0f, 1.0f};
  static GLfloat light_att[] = {0.00002f};
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  /* Our shading model--Gouraud (smooth). */
  glShadeModel(GL_SMOOTH);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_spe);
  glLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, light_att);

  /* Set the clear color. */
  glClearColor(0, 0, 0, 0);

  /* Setup our viewport. */
  glViewport(0, 0, width, height);

  /*
   * Change to the projection matrix and set
   * our viewing volume.
   */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  projection = glm::perspective(
      // FOV & aspect
      57.6f, float(ratio),
      // Near and far planes
      1.0f, 1024.0f);
  glLoadMatrixf(glm::value_ptr(projection));
}

// funcoes auxiliares ao desfaz
void criaInstanciaDesfaz(struct GameData *GD)
{
  logfile << "\n criaInstanciaDesfaz >> entra\n";
  GD->Desfaz->CriaInstancia(GD->Torre, GD->Player);
}

void aplicaDesfaz(struct GameData *GD)
{
  logfile << "\n aplicaDesfaz >> entra\n";
  if (GD->Desfaz->DesfazAcao(GD->Torre, GD->Player))
    logfile << "aplicaDesfaz >> sucesso\n";
  else
    logfile << "aplicaDesfaz >> nao pode desfazer\n";
}

// camera
void setCamera(struct GameData *GD, string s)
{
  string delim = ",";

  auto start = 0U;
  auto end = s.find(delim);
  GD->cx = stoi(s.substr(start, end - start)) * d;
  start = end + delim.length();
  end = s.find(delim, start);

  GD->cz = stoi(s.substr(start, end - start)) * d;
  start = end + delim.length();
  end = s.find(delim, start);

  GD->theta_y = stoi(s.substr(start, end - start)) * d;
  start = end + delim.length();
  end = s.find(delim, start);

  GD->zoom = stoi(s.substr(start, end));
  start = end + delim.length();
  end = s.find(delim, start);

  camera = Camera(glm::vec3(GD->cx, GD->theta_y, GD->cz));
}
string cameraToString(struct GameData *GD)
{
  string s = "";
  s += "Camera (";
  s += "cx = " + to_string(GD->cx);
  s += ", cz = " + to_string(GD->cz);
  s += ", theta = " + to_string(GD->theta_y);
  s += ", zoom = " + to_string(GD->zoom);
  s += ")";
  return s;
}

// setups dos mapas
void loadMap(struct GameData *GD, string filename)
{
  ifstream arquivo;
  string line;
  GD->Desfaz->ClearDesfaz();

  arquivo.open(filename);
  if (arquivo.is_open())
  {
    getline(arquivo, line);
    GD->Player->SetPlayer(line);

    getline(arquivo, line);
    setCamera(GD, line);

    arquivo.close();

    GD->Torre->SetTorre(filename);
  }

  else
    logfile << "Erro em abrir arquivo\n";
}

static void quitGame(int code)
{
  /*
   * Quit SDL so we can release the fullscreen
   * mode and restore the previous video settings,
   * etc.
   */
  // free music
  // Mix_FreeMusic(gMusic);
  // gMusic = NULL;

  // Mix_Quit();
  gameIsRunning = false;
  glUseProgram(0);
  SDL_DestroyWindow(Window);
  SDL_Quit();

  /* Exit program. */
  exit(code);
}

void UpdateAndar(LLBlocos *ListaUpdate, torre *Torre, int n)
{
  logfile << "\nEntrou Update do andar_id=" << n << "\n";
  bool houveMudanca = false;
  block *b;
  block *aux;
  andar *base = Torre->retornaAndarN(n - 1);
  andar *atual = Torre->retornaAndarN(n);
  if (base != nullptr && atual != nullptr)
  {
    logfile << "\n Dando Update em andar\n ";
    logfile << "\n Andar Atual antes: " << atual->AndarToString() << "\n";
    b = atual->Lista->lista;
    while (b != nullptr)
    {
      aux = b;
      b = b->prox;
      if ((aux->tipo == Movel || aux->tipo == FinalMovel || aux->tipo == FinalMovelCompleto) && !base->temSuporte(aux))
      {
        logfile << "\n cai bloco \n ";
        houveMudanca = true;
        aux->setPosLim(posicao(aux->pos));
        aux->setVel(velocidade(0, 0, 0));
        // std::cout << "tipo bloco ejetado " << aux->tipo << "\n";

        atual->EjetaBloco(aux);
        ListaUpdate->AdicionaBloco(aux);
      }
    }
    if (houveMudanca)
    {
      UpdateAndar(ListaUpdate, Torre, n + 1);
    }
  }
}

block *b_aux;

// movimentacao player
void movePlayerFront(struct GameData *GD, bool push)
{
  static double moveTrack = 0.0f;
  logfile << "\nEntrou player front\n";

  player *Player = GD->Player;
  torre *Torre = GD->Torre;

  posicao posDesejada = posicao(Player->pos) + velocidade(Player->rotacao);
  
  block *b = Torre->retornaBloco(posDesejada);
  block *aux;

  // sem bloco na frente
  if (b == nullptr || b->tipo == FinalTargetIncompleto)
  {
    logfile << "\nSem bloco na frente do player\n";
    logfile << "player anda\n";
    Player->mexe(posDesejada, velocidade(Player->rotacao), deltaTime * speedMult);
    //  std::cout << "Velx: " << Player->rotacao->vx << "Vely: " << Player->rotacao->vy << "Velz: " << Player->rotacao->vz << " *X:" << GD->Player->pos->x << " Y: " << GD->Player->pos->y << " Z: "<< GD->Player->pos->z << "\n";
    Player->animacao = AnimAnda;
  }
  // com bloco na frente
  else
  {
    logfile << "ComBlocoNaFrente \n";
    // esta empurrando
    logfile << "bloco tipo:" << b->tipo << "\n";
    if (push && (b->tipo == Movel || b->tipo == FinalMovel || b->tipo == FinalMovelCompleto || b->tipo == ApenasEmpurra || b->tipo == DirecEsquerda || b->tipo == DirecDireita))
    {
      // checa se pode
      logfile << "ChecaEmpurrar \n";
      posicao posBlocoFrente = posDesejada + velocidade(Player->rotacao);
      aux = Torre->retornaBloco(posBlocoFrente);
      if (aux == nullptr || aux->tipo == FinalTargetIncompleto)
      {
        if (aux != nullptr && aux->tipo == FinalTargetIncompleto) {
          aux->tipo = FinalTargetCompleto;
          aux->alphaValue = 0.9f;
          b->dissapearing = true;
          b_aux = b;
        }
        logfile << "Empurra \n";
        criaInstanciaDesfaz(GD);
        b->mexe(posBlocoFrente, velocidade(Player->rotacao), deltaTime * speedMult);
        Player->animacao = AnimEmpurra;
        Torre->EjetaBloco(b);
        GD->ListaUpdate->AdicionaBloco(b);
      }
    }
    else if (b->tipo != ApenasEmpurra && b->tipo != Parede && b->tipo != ParedeQuebravel && b->tipo != FinalTargetCompleto)
    {
      // checa se pode subir
      logfile << "\n ChecaSePodeSubir \n";
      posicao posSubir = posDesejada + velocidade(0, 1, 0);
      posicao posAcima = posicao(Player->pos) + velocidade(0, 1, 0);
      if (Torre->retornaBloco(posSubir) == nullptr && Torre->retornaBloco(posAcima) == nullptr)
      {
        logfile << "\n Sobe \n";
        Player->estado2 = Normal;
        Player->mexe(posSubir, velocidade(Player->rotacao) + velocidade(0, 1, 0), deltaTime * speedMult);
        Player->animacao = AnimAnda; // seria anim pula
      }
    }
  }
}

void movePlayerBack(struct GameData *GD, bool pull)
{
  logfile << "\nmovePlayerBack >> Entrou player back\n";

  player *Player = GD->Player;
  torre *Torre = GD->Torre;

  posicao posDesejada = posicao(Player->pos) - velocidade(Player->rotacao);
  posicao posFrente = posicao(Player->pos) + velocidade(Player->rotacao);
  posicao posChaoTras = posicao(Player->pos) - velocidade(Player->rotacao) + velocidade(0, -1, 0);

  bool podeIrAtras = Torre->retornaBloco(posDesejada) == nullptr;
  block *blocoNaFrente = Torre->retornaBloco(posFrente);
  bool temChaoTras = Torre->retornaBloco(posChaoTras) != nullptr;
  // checa se pode ir para tras
  if (podeIrAtras)
  {
    logfile << "Sem bloco atras do player\n";
    // puxa se tiver bloco
    if (pull && blocoNaFrente != nullptr && (blocoNaFrente->tipo == Movel || blocoNaFrente->tipo == FinalMovel || blocoNaFrente->tipo == FinalMovelCompleto || blocoNaFrente->tipo == DirecEsquerda || blocoNaFrente->tipo == DirecDireita))
    {
      logfile << "movePlayerBack >> puxa\n";
      criaInstanciaDesfaz(GD);
      blocoNaFrente->mexe(posicao(Player->pos), velocidade(Player->rotacao) * (-1), deltaTime * speedMult);
      Torre->EjetaBloco(blocoNaFrente);
      GD->ListaUpdate->AdicionaBloco(blocoNaFrente);
      Player->animacao = AnimPuxa;
    }
    else
      Player->animacao = AnimAnda;

    // anda para tras
    Player->mexe(posDesejada, velocidade(Player->rotacao) * (-1), deltaTime * speedMult);

    if (!temChaoTras)
    {
      Player->estado2 = TentaPendurar;
      Player->setAgarrar(posChaoTras + velocidade(Player->rotacao));
    }
  }

  else
    logfile << "\nmovePlayerBack >> Bloco atras do player\n";
}

void movePlayerSideways(struct GameData *GD, bool right)
{
  logfile << "\nEntrou player sideways\n";

  player *Player = GD->Player;
  torre *Torre = GD->Torre;

  velocidade rotacaoLado;

  int dir = Player->iRotacao;
  if (right)
  {
    // move em direcao horaria
    dir = (dir + 1) % 4;
    Player->animacao = AnimPenduradoDir;
  }
  else
  {
    dir = (dir + 3) % 4;
    Player->animacao = AnimPenduradoEsq;
  }

  rotacaoLado = Player->rotacoes[dir];

  posicao posDesejada = posicao(Player->pos) + velocidade(rotacaoLado);

  posicao apoioDesejado = posicao(Player->pos) + velocidade(rotacaoLado) + velocidade(Player->rotacao);

  bool temApoio = Torre->retornaBloco(apoioDesejado) != nullptr;
  bool posLivre = Torre->retornaBloco(posDesejada) == nullptr;

  if (temApoio && posLivre)
  {
    Player->mexe(posDesejada, velocidade(rotacaoLado), deltaTime * speedMult);
  }
  // se for canto
  else
  {
    if (posLivre)
    {
      Player->mexe(posDesejada + velocidade(Player->rotacao), velocidade(rotacaoLado) + velocidade(Player->rotacao), deltaTime * speedMult);
      if (right)
        Player->Rotaciona(1);
      else
        Player->Rotaciona(0);
    }
    else
    {
      Player->mexe(posicao(Player->pos), velocidade(0, 0, 0), deltaTime * speedMult);
      if (right)
        Player->Rotaciona(0);
      else
        Player->Rotaciona(1);
    }
  }
}

// update player
static void updatePlayer(player *Player, torre *Torre)
{

  if (Player->levelStage > Torre->nStages ) estadoJogo = Vitoria;
  tipoColisao colisaoTorre;
  colisaoTorre = Torre->ChecaColisaoPlayer(Player, Torre);
  if (Player->pos->y < -2)
  {
    logfile << "\n Player caiu do mapa \n";
    estadoJogo = Derrota;
  }

  if (colisaoTorre == ColisaoAgressiva)
  {
    logfile << "\n Player esmagado \n";
    estadoJogo = Derrota;
  }
  switch (Player->estado)
  {
  case Caindo:
    switch (colisaoTorre)
    {
    case ColisaoDeApoio:
      Player->para();
      Player->animacao = AnimNormal;
      break;

    case ColisaoLateral:
      Player->estado2 = Pendurado;
      Player->para();
      Player->animacao = AnimPendurado;
      break;

    default:
      Player->atualizaPos();
      break;
    }

    break;

  case Movimento:
    Player->atualizaPos();
    break;

  default:
    break;
  }
}

// update lista
static void updateLista(LLBlocos *ListaUpdate, torre *Torre, player *Player)
{
  bool emMovimento = false;
  bool mudaEstado = false;
  int n = 0;
  // update lista
  if (!ListaUpdate->estaVazia())
  {
    block *aux;
    block *b = ListaUpdate->lista;
    while (b != nullptr)
    {
      if (b->estado == Movimento)
        emMovimento = true;
      b->atualizaPos();
      if (b->estado == Caindo && emMovimento)
      {
        mudaEstado = true;
        n = (int)b->pos->y;
      }
      aux = b;
      b = b->prox;
      if (aux->estado != Movimento)
      {
        tipoColisao colisao = Torre->ChecaColisao(aux);
        logfile << "\nupdateLista >> tipo de colisao: " << colisao << " \n";
        if (colisao == ColisaoDeApoio)
        {
          aux->para();
          ListaUpdate->EjetaBloco(aux);
          Torre->adicionaBloco(aux);
          logfile << "\n ------------\n";

          if (Player->animacao == AnimEmpurra)
            Player->animacao = AnimNormal;
        }
        else if (colisao == ColisaoAgressiva)
        {
          logfile << "\n Deleta Bloco \n";
          std::cout << "tipo bloco ejetado " << aux->tipo << "\n";

          ListaUpdate->EjetaBloco(aux);
          delete aux;
        }
      }
      if (mudaEstado)
        UpdateAndar(ListaUpdate, Torre, n + 1);
    }
  }
}

// funcoes dos menus
string arrowToString()
{
  string s = "";
  s += "Arrow (";
  s += "x = " + to_string(arrowPosition.x);
  s += ", y = " + to_string(arrowPosition.y);
  s += ")";
  return s;
}

void proximaAcao(struct GameData *GD, struct Tela *tela)
{

  logfile << "\nproximaAca << entrou \n";
  switch (tela->nomeTela)
  {
  case Menu:
    // inicia fase 1
    if (tela->selecionaOpcao1)
    {
      GD->fase = startMap;
      loadMap(GD, "maps/fase0" + to_string(GD->fase) + ".txt");
      estadoJogo = EmJogo;
      firstRun = 1;
    }

    // instrucoes
    else
    {
      estadoJogo = Instrucoes;
    }
    break;

  case Instrucoes:
    if (numTelaInstrucao < numTelaInstrucoes)
    {
      numTelaInstrucao += 1;
    }
    else
    {
      numTelaInstrucao = 1;
      estadoJogo = Menu;
    }
    break;

  case Vitoria:

    // for(int i=0; i<MAX_MINI_FLUFFYS; i++) {
    //   if (GD->miniFluffys[i]) {
    //     GD->miniFluffys[i] = nullptr;
    //   }
    // }
    GD->fase += 1;
    if (GD->fase > numFases)
      estadoJogo = TelaFinal;
    else
    {
      GD->miniFluffyIndex = 0;
      GD->nMiniFluffysAlive = 0;       
      GD->nMiniFluffysRescued = 0;       
      GD->nMiniFluffysDead = 0; 
      GD->Player->levelStage = 1;
      std::cout << "VITORIA";
      loadMap(GD, "maps/fase0" + to_string(GD->fase) + ".txt");
      estadoJogo = EmJogo;
      firstRun = 1;
    }
    break;

  case Derrota:
    GD->miniFluffyIndex = 0;
    GD->nMiniFluffysAlive = 0;       
    GD->nMiniFluffysRescued = 0;       
    GD->nMiniFluffysDead = 0; 
    GD->Player->levelStage = 1;
    GD->fase = 1;
    logfile << "\nproximaAca << GameOver \n";
    estadoJogo = Menu;
    break;

  case Pause:
    if (tela->selecionaOpcao1)
    {
      estadoJogo = EmJogo;
    }
    // instrucoes
    else
    {
      estadoJogo = Instrucoes;
    }
    break;

  case TelaFinal:
    estadoJogo = Menu;
    break;

  default:
    break;
  }
}

void passaTelaInstrucao(struct Tela *tela)
{
  string dirArquivo = "images/instructions/" + to_string(numTelaInstrucao) + ".png";
  static int numTela_atual = -1;
  if (numTela_atual != numTelaInstrucao)
  {
    SDL_FreeSurface(tela->background);
    tela->background = loadImage((char *)dirArquivo.c_str());
    flipSurfaceVertical(tela->background);
    numTela_atual = numTelaInstrucao;
  }
}

void drawMenu(SDL_Window *Window,
               struct Tela *tela, SDL_Surface *arrow)
{
  glUseProgram(0);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  // Clear the color and depth buffers.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // We don't want to modify the projection matrix.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (tela->selecionaOpcao1)
  {
    arrowPosition.x = tela->x1;
    arrowPosition.y = tela->y1;
  }
  else
  {
    arrowPosition.x = tela->x2;
    arrowPosition.y = tela->y2;
  }


  float fovy = 60.0 * (3.14159 / 180.0);
  float height = SCR_HEIGHT;
  float focallength = (height / 2.0) / tan(fovy / 2.0);
  glRasterPos3f(-SCR_WIDTH / 2,
                -SCR_HEIGHT / 2,
                -(focallength + 0.1));
  glDrawPixels(SCR_WIDTH,
               SCR_HEIGHT,
               GL_RGBA, GL_UNSIGNED_BYTE,
               tela->background->pixels);

  if (tela->showArrow)
  {
    glRasterPos3f(-tela->background->w / 2 + arrowPosition.x,
                  tela->background->h / 2 - arrowPosition.y - arrow->h,
                  -(focallength + 0.001));
    glDrawPixels(arrow->w,
                 arrow->h,
                 GL_RGBA, GL_UNSIGNED_BYTE,
                 arrow->pixels);
  }
  // Apply the arrow
  SDL_GL_SwapWindow(Window);
}

// input
static void handleKey(SDL_KeyboardEvent *key, struct GameData *GD, bool down,
                       struct Tela *tela = NULL)
{
  static bool hold_ctrl = false;
  player *Player = GD->Player;
  bool podeMover = Player->estado == Parado && estadoJogo == EmJogo;
  bool emTela = estadoJogo != EmJogo && tela != NULL;

  switch (key->keysym.sym)
  {

  case SDLK_ESCAPE:
    if (down)
      quitGame(0);
    break;

  // case SDLK_SPACE:
  //   if (down && podeMover)
  //     aplicaDesfaz(GD);
  //   break;

  case SDLK_LEFT:
    if (down)
    {
      if (podeMover)
      {
        if (Player->estado2 == Pendurado)
        {
          movePlayerSideways(GD, 0);
        }
        else
        {
          GD->Player->Rotaciona(1);
        }
      }
    }
    break;

  case SDLK_RIGHT:
    if (down)
    {
      if (podeMover)
      {
        if (Player->estado2 == Pendurado)
        {
          movePlayerSideways(GD, 1);
        }
        else
        {
          GD->Player->Rotaciona(0);
        }
      }
    }
    break;

  case SDLK_DOWN:
    if (down)
    {
      if (podeMover)
      {
        if (Player->estado2 == Pendurado)
        {
          Player->estado2 = Normal;
          Player->cai();
          logfile << "\n handleKey >> solta da beirada \n";
          logfile << Player->PlayerToString();
        }
        else
          movePlayerBack(GD, hold_ctrl);
      }
      if (emTela)
      {
        logfile << "\n handleKey >> entrou \n";
        if (tela->temOpcao2)
          tela->selecionaOpcao1 = !tela->selecionaOpcao1;
      }
    }
    break;

  case SDLK_UP:
    if (down)
    {
      if (podeMover)
        movePlayerFront(GD, hold_ctrl);
      if (emTela)
      {
        if (tela->temOpcao2)
          tela->selecionaOpcao1 = !tela->selecionaOpcao1;
      }
    }
    break;

  case SDLK_RETURN:
    if (down && emTela)
    {

      logfile << "\nHandle Key << proxima \n";
      proximaAcao(GD, tela);
    }
    break;

  case SDLK_LCTRL: // Left Ctrl
    if (down)
      hold_ctrl = true;
    else
      hold_ctrl = false;
    break;

  case SDLK_RALT: // Right Alt
    break;

  // camera
  case SDLK_z: // controle de rotacao
    if (down)
      GD->theta_y += 1.0;
    break;

  case SDLK_x:
    if (down)
      GD->theta_y -= 1.0;
    break;

    // case SDLK_a: // controle no plano
    //   if(down) {
    //     GD->zoom += passoCam;
    //   }
    //   break;
  // case SDLK_h:
  //   GD->Player->levelStage = 4;
    // camera.ChangePitch(-1*(deltaTime * 10));
    // std::cout << camera.Pitch << "\n";
    // std::cout << camera.Pitch << "\n";
    // camera.ProcessKeyboard(UP, speedMult);
    // std::cout << "UP: " << glm::to_string(camera.WorldUp) << "\n";
    // camera.Position += glm::vec3(0.0f, 1.0f, 0.0f);
    // std::cout << "UP: " << glm::to_string(camera.Position) << "\n\n\n";
    break;
  case SDLK_w:
    camera.ProcessKeyboard(FORWARD, speedMult);
    break;
  case SDLK_s:
    camera.ProcessKeyboard(BACKWARD, speedMult);
    break;
  case SDLK_a:
    camera.ProcessKeyboard(LEFT, speedMult);
    break;
  case SDLK_d:
    camera.ProcessKeyboard(RIGHT, speedMult);
    break;

    // case SDLK_s:
    //   if(down) {
    //     GD->zoom -= passoCam;
    //   }
    //   break;

  case SDLK_o: // pause/play music
    if (down)
    {
      logfile << "music"
              << "\n";
      // If there is no music playing
      if (Mix_PlayingMusic() == 0)
      {
        // Play the music
        Mix_PlayMusic(gMusic, -1);
      }
      // If music is being played
      else
      {
        // If the music is paused
        if (Mix_PausedMusic() == 1)
        {
          // Resume the music
          Mix_ResumeMusic();
        }
        // If the music is playing
        else
        {
          // Pause the music
          Mix_PauseMusic();
        }
      }
    }
    break;

  case SDLK_p:
    if (down)
    {
      logfile << GD->Torre->TorreToString() << "\n";
    }
    break;

  default:
    break;
  }
}

void ResizeWindow(int width, int height,
                  SDL_Window *Window,
                  struct GameData *GD)
{
  float Sratio, Wratio;
  int w, h, x, y;

  Sratio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
  Wratio = (float)width / (float)height;

  if (Sratio > Wratio)
  {
    w = width;
    h = ROUND(width / Sratio);
    x = 0;
    y = height / 2 - h / 2;
    glViewport(x, y, w, h);
  }
  else
  {
    w = ROUND(height * Sratio);
    h = height;
    x = width / 2 - w / 2;
    y = 0;
    glViewport(x, y, w, h);
  }
  // glViewport( 0, 0, width, height );
}

static void proccessEvents(struct GameData *GD, struct Tela *tela = NULL)
{
  /* Our SDL event placeholder. */

  SDL_Event event;

  if (estadoJogo == EmJogo)
  {
    updateLista(GD->ListaUpdate, GD->Torre, GD->Player);

    updatePlayer(GD->Player, GD->Torre);
  }

  /* Grab all the events off the queue. */
  while (SDL_PollEvent(&event))
  {

    switch (event.type)
    {
    case SDL_KEYDOWN:
      /* Handle key presses. */
      handleKey(&event.key, GD, true, tela);

      break;
    case SDL_KEYUP:
      handleKey(&event.key, GD, false);
      break;
    case SDL_QUIT:
      /* Handle quit requests (like Ctrl-c). */
      quitGame(0);
      break;
    case SDL_MOUSEMOTION:
      if (event.motion.state & SDL_BUTTON_LMASK)
      {

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
    case SDL_MOUSEBUTTONUP:
      firstMouse = true;
      break;
    case SDL_WINDOWEVENT:
      if (event.window.event == SDL_WINDOWEVENT_RESIZED)
      {
        // printf("MESSAGE:Resizing window...\n");
        ResizeWindow(event.window.data1, event.window.data2,
                     Window, GD);
      }
      break;
    }
  }
}


int main(int argc, char *argv[])
{
  if (argc > 1) {
    startMap = atoi(argv[1]);
    std::cout<<argv[1];
  }
  struct GameData GD;
  GD.Player = new player;
  GD.Torre = new torre;
  GD.ListaUpdate = new LLBlocos;
  GD.Desfaz = new desfaz;

  GD.zoom = -300;
  GD.theta_y = -30.0;
  GD.cx = 3;
  GD.cz = 3;

  struct Tela TelaMenu;
  struct Tela TelaPause;
  struct Tela TelaGameOver;
  struct Tela TelaNext;
  struct Tela TelaInstrucoes;
  struct Tela TelaTelaFinal;


  logfile.open("logfile.txt");
  logfile << "Log\n";

  /* Dimensions of our window. */
  int width = SCR_WIDTH;   // 1366; //512; //640;
  int height = SCR_HEIGHT; // 768; //288; //480;
  /* Flags we will pass into SDL_SetVideoMode. */
  int flags = 0;
  GLuint tex[numTex];
  

  SDL_Surface *imgArrow = loadImage((char *)"textures/arrow.png");

  if (DEBUG)
    fprintf(stderr, "\nPassed line 964\n");

  TelaMenu.background = loadImage((char *)"images/menu/main.png");
  flipSurfaceVertical(TelaMenu.background);
  TelaMenu.nomeTela = Menu;
  TelaMenu.x1 = 654;
  TelaMenu.y1 = 290;
  TelaMenu.x2 = 680;
  TelaMenu.y2 = 352;

  TelaPause.background = loadImage((char *)"images/menu/2.png");
  flipSurfaceVertical(TelaPause.background);
  TelaPause.nomeTela = Pause;
  TelaPause.x1 = 635;
  TelaPause.y1 = 288;
  TelaPause.x2 = 680;
  TelaPause.y2 = 349;

  TelaGameOver.background = loadImage((char *)"images/menu/3.png");
  flipSurfaceVertical(TelaGameOver.background);
  TelaGameOver.nomeTela = Derrota;
  TelaGameOver.x1 = 601;
  TelaGameOver.y1 = 288;
  TelaGameOver.temOpcao2 = false;

  TelaNext.background = loadImage((char *)"images/menu/4.png");
  flipSurfaceVertical(TelaNext.background);
  TelaNext.nomeTela = Vitoria;
  TelaNext.x1 = 702;
  TelaNext.y1 = 352;
  TelaNext.temOpcao2 = false;

  TelaInstrucoes.background = loadImage((char *)"images/instructions/1.png");
  flipSurfaceVertical(TelaInstrucoes.background);
  TelaInstrucoes.nomeTela = Instrucoes;
  TelaInstrucoes.temOpcao2 = false;
  TelaInstrucoes.showArrow = false;

  TelaTelaFinal.background = loadImage((char *)"images/menu/final.png");
  flipSurfaceVertical(TelaTelaFinal.background);
  TelaTelaFinal.nomeTela = Instrucoes;
  TelaTelaFinal.temOpcao2 = false;
  TelaTelaFinal.showArrow = false;

  /* First, initialize SDL's video subsystem. */

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
  {
    fprintf(stderr, "Video initialization failed: %s\n",
            SDL_GetError());
    quitGame(1);
  }
  // Initialize SDL_mixer
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
  {
    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
  }

  if (DEBUG)
    fprintf(stderr, "\nPassed line 1015\n");

  // Load music
  // https://www.looperman.com/loops/detail/314853/subspace-club-type-sample-free-115bpm-disco-pad-loop
  gMusic = Mix_LoadMUS("musics/looperman-l-5041336-0314853-subspace-club-type-sample.wav");

  Window = SDL_CreateWindow("O Pesadelo de Fluffy",
                            0, 0,
                            width, height,
                            SDL_WINDOW_OPENGL);
  assert(Window);
  SDL_GLContext Context = SDL_GL_CreateContext(Window);
  SDL_GL_SetSwapInterval(1);

  int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
  printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  /*
   * Now, we want to setup our requested
   * window attributes for our OpenGL window.
   * We want *at least* 5 bits of red, green
   * and blue. We also want at least a 16-bit
   * depth buffer.
   *
   * The last thing we do is request a double
   * buffered window. '1' turns on double
   * buffering, '0' turns it off.
   *
   * Note that we do not use SDL_DOUBLEBUF in
   * the flags to SDL_SetVideoMode. That does
   * not affect the GL attribute state, only
   * the standard 2D blitting setup.
   */
  // set texturas etc

  if (DEBUG)
    fprintf(stderr, "\nPassed line 1046\n");

  if (1)
  {
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    /*
     * At this point, we should have a properly setup
     * double-buffered window for use with OpenGL.
     */
    setupOpenGL(width, height);
    setupTextures(&GD, tex);

  }

  /*
   * Now we want to begin our normal app process--
   * an event loop with a lot of redrawing.
   */

  // Play the music
  Mix_PlayMusic( gMusic, -1 );

  Shader simpleShader("./shaders/shader.vs", "./shaders/shader.fs");
  setupBlockVAO(d);

  clock_t lastFrame, currentFrame;
  lastFrame = clock();
  gameIsRunning = 1;
  float secondCount = 0;
  while (gameIsRunning)
  {
    miniFluffyStageUpdate(&GD);
    currentFrame = clock();
    if (estadoJogo == Menu)
    {
      proccessEvents(&GD, &TelaMenu);
      drawMenu(Window, &TelaMenu, imgArrow);
      firstRun = 1;
    }

    if (estadoJogo == Instrucoes)
    {
      passaTelaInstrucao(&TelaInstrucoes);
      proccessEvents(&GD, &TelaInstrucoes);
      drawMenu(Window, &TelaInstrucoes, imgArrow);
    }

    if (estadoJogo == Pause)
    {
      proccessEvents(&GD, &TelaPause);
      drawMenu(Window, &TelaPause, imgArrow);
    }

    if (estadoJogo == EmJogo)
    {
      proccessEvents(&GD);
      drawScreen(Window, &GD, tex, simpleShader);
    }

    if (estadoJogo == Derrota)
    {
      proccessEvents(&GD, &TelaGameOver);
      drawMenu(Window, &TelaGameOver, imgArrow);
      GD.miniFluffyIndex = 0;
      GD.nMiniFluffysAlive = 0;       
      GD.nMiniFluffysRescued = 0;       
      GD.nMiniFluffysDead = 0; 
      GD.Player->levelStage = 1;
    }

    if (estadoJogo == Vitoria)
    {
      proccessEvents(&GD, &TelaNext);
      drawMenu(Window, &TelaNext, imgArrow);
      GD.miniFluffyIndex = 0;
      GD.nMiniFluffysAlive = 0;       
      GD.nMiniFluffysRescued = 0;       
      GD.nMiniFluffysDead = 0; 
      GD.Player->levelStage = 1;
    }

    if (estadoJogo == TelaFinal)
    {
      proccessEvents(&GD, &TelaTelaFinal);
      drawMenu(Window, &TelaTelaFinal, imgArrow);
    }

    // Framerate limiter
    while (MAX_FRAMERATE != 0 && ((float)(clock() - currentFrame )/ CLOCKS_PER_SEC < (float)(1.0f/MAX_FRAMERATE))) {
      continue;
    }


    deltaTime = ((float)(currentFrame - lastFrame)) / CLOCKS_PER_SEC;

    secondCount += deltaTime;
    if (secondCount >= 1.0f) {
      gameSeconds++;
      // std::cout << "Framerate: " << 1.0f/deltaTime << "\n";
      secondCount = 0;
    }
    // std::cout << deltaTime << "\n";
    lastFrame = currentFrame;

    // SDL_Delay(1);
  }

  logfile.close();

  return 0;
}
