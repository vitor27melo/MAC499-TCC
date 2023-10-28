#include "shader.h"
#include "drawFunctions.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_opengl.h>
#include "glm/glm/gtx/string_cast.hpp"
#include "Entidades.h"

#define PI 3.142857

#define SQUARE(x) ((x) * (x))
#define ROUND(x) ((x < 0) ? (int)(x - 0.5) : (int)(x + 0.5))
#define SIGN(x) ((x >= 0) ? 1 : -1)


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
};


unsigned int blockVAO;
unsigned int hairVAO;

void setupBlockVAO(float d)
{
  float vertices[] = {
      // positions         // colors          // texture coords  //normals
      -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
      -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,


      -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

      -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,

      0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

  unsigned int VBO, VAO;
  int i;

  for (i = 0; i < 36; i++)
  {
    vertices[i * 11 + 0] *= d;
    vertices[i * 11 + 1] *= d;
    vertices[i * 11 + 2] *= d;
  }
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  // normal attribute
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(8 * sizeof(float)));
  glEnableVertexAttribArray(3);

  blockVAO = VAO;
}

// void setupHairVAO(float R, float G, float B){
//   float th0, th1;
//   float hair_R = 8.0;
//   th0 = 90.0;
//   th1 = 180.0;
//   hairVAO = curvedXCylinderVAO(3.0,
// 				  0.0,
// 				  hair_R,
// 				  th0, th1,
// 				  R, G, B,
// 				  20, 4);
// }

// unsigned int curvedXCylinderVAO(float radius0,
// 				      float radius1,
// 				      float bend_radius,
// 				      float bend_ang0,
// 				      float bend_ang1,
// 				      float R, float G, float B,
// 				      int slices,
// 				      int dtheta){
//   unsigned int VAO;
//   unsigned int VBO;
//   float *vertices;
//   float w0, w1, ang0, ang1, angle, x0, y0, x1, y1, xb, yb, zb;
//   float cos_angle, sin_angle, cos_ang0, sin_ang0, cos_ang1, sin_ang1;
//   float rad0, rad1, mag;
//   float C[3];
//   int t = 0;
//   int i, j, k, n;

//   n = slices*(360/dtheta+1)*2*11;
//   k = 0;
//   vertices = (float *)calloc(n, sizeof(float));

//   bend_ang0 *= (PI / 180.0f);
//   bend_ang1 *= (PI / 180.0f);
//   for(i = 0; i < slices; i++){
//     w0 = (float)i / (float)slices;
//     w1 = (float)(i+1) / (float)slices;
//     ang0 = bend_ang0*(1.0-w0) + bend_ang1*w0;
//     ang1 = bend_ang0*(1.0-w1) + bend_ang1*w1;

//     rad0 = radius0*(1.0-w0) + radius1*w0;
//     rad1 = radius0*(1.0-w1) + radius1*w1;    

//     sin_ang0 = sinf(ang0);
//     cos_ang0 = cosf(ang0);
//     sin_ang1 = sinf(ang1);
//     cos_ang1 = cosf(ang1);
    
//     //glBegin(GL_QUAD_STRIP);
//     for(j = 0; j <= 360; j+= dtheta){
//       angle = (float)j * (PI / 180.0f);
//       cos_angle = cosf(angle);
//       sin_angle = sinf(angle);
//       y0 = rad0 * cos_angle + bend_radius;
//       x0 = rad0 * sin_angle;
//       y1 = rad1 * cos_angle + bend_radius;
//       x1 = rad1 * sin_angle;
      
//       C[0] = x0;
//       C[1] = sin_ang0*rad0*cos_angle;
//       C[2] = cos_ang0*rad0*cos_angle;
//       mag = magnitude(C);
//       C[0] /= mag;
//       C[1] /= mag;
//       C[2] /= mag;
//       //glNormal3f(C[0], C[1], C[2]);
//       yb = sin_ang0 * y0;
//       xb = x0;
//       zb = cos_ang0 * y0;
//       //glVertex3f( xb, yb, zb );
//       vertices[k] = xb; k++;
//       vertices[k] = yb; k++;
//       vertices[k] = zb; k++;

//       //Colors:
//       vertices[k] = R; k++;
//       vertices[k] = G; k++;
//       vertices[k] = B; k++;

//       //Texture:
//       vertices[k] = t;   k++;
//       vertices[k] = 0.0; k++;

//       //Normals:
//       vertices[k] = C[0]; k++;
//       vertices[k] = C[1]; k++;
//       vertices[k] = C[2]; k++;
      
//       C[0] = x1;
//       C[1] = sin_ang1*rad1*cos_angle;
//       C[2] = cos_ang1*rad1*cos_angle;
//       mag = magnitude(C);
//       C[0] /= mag;
//       C[1] /= mag;
//       C[2] /= mag;
//       //glNormal3f(C[0], C[1], C[2]);
//       yb = sin_ang1 * y1;
//       xb = x1;
//       zb = cos_ang1 * y1;
//       //glVertex3f( xb, yb, zb );
//       vertices[k] = xb; k++;
//       vertices[k] = yb; k++;
//       vertices[k] = zb; k++;

//       //Colors:
//       vertices[k] = R; k++;
//       vertices[k] = G; k++;
//       vertices[k] = B; k++;

//       //Texture:
//       vertices[k] = t;   k++;
//       vertices[k] = 1.0; k++;

//       //Normals:
//       vertices[k] = C[0]; k++;
//       vertices[k] = C[1]; k++;
//       vertices[k] = C[2]; k++;

//       t++;
//       t = t%2;
//     }
//     //glEnd();
//   }

//   glGenVertexArrays(1, &VAO);
//   glGenBuffers(1, &VBO);
  
//   glBindVertexArray(VAO);
  
//   glBindBuffer(GL_ARRAY_BUFFER, VBO);
//   glBufferData(GL_ARRAY_BUFFER, n*sizeof(float), vertices, GL_DYNAMIC_DRAW);

//   // position attribute
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
// 			(void*)0);
//   glEnableVertexAttribArray(0);
//   // color attribute
//   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
// 			(void*)(3* sizeof(float)));
//   glEnableVertexAttribArray(1);
//   // texture attribute
//   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
// 			(void*)(6 * sizeof(float)));
//   glEnableVertexAttribArray(2);
//   // normal attribute
//   glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
// 			(void*)(8 * sizeof(float)));
//   glEnableVertexAttribArray(3);

//   free(vertices);
//   return VAO;
// }


// FIM SETUP VAOS


float magnitude(float A[3])
{
  return sqrtf(A[0] * A[0] + A[1] * A[1] + A[2] * A[2]);
}


// INICIO DESENHOS
unsigned int create_curved_cylinder_y(float radius0,
				      float radius1,
				      float bend_radius,
				      float bend_ang0,
				      float bend_ang1,
				      float R, float G, float B,
				      int slices,
				      int dtheta){
  unsigned int VAO;
  unsigned int VBO;
  float *vertices;
  float w0, w1, ang0, ang1, angle, x0, y0, x1, y1, xb, yb, zb;
  float cos_angle, sin_angle, cos_ang0, sin_ang0, cos_ang1, sin_ang1;
  float rad0, rad1, mag;
  float C[3];
  int t = 0;
  int i, j, k, n;

  n = slices*(360/dtheta+1)*2*11;
  k = 0;
  vertices = (float *)calloc(n, sizeof(float));

  bend_ang0 *= (PI / 180.0f);
  bend_ang1 *= (PI / 180.0f);
  for(i = 0; i < slices; i++){
    w0 = (float)i / (float)slices;
    w1 = (float)(i+1) / (float)slices;
    ang0 = bend_ang0*(1.0-w0) + bend_ang1*w0;
    ang1 = bend_ang0*(1.0-w1) + bend_ang1*w1;

    rad0 = radius0*(1.0-w0) + radius1*w0;
    rad1 = radius0*(1.0-w1) + radius1*w1;    

    sin_ang0 = sinf(ang0);
    cos_ang0 = cosf(ang0);
    sin_ang1 = sinf(ang1);
    cos_ang1 = cosf(ang1);
    
    //glBegin(GL_QUAD_STRIP);
    for(j = 0; j <= 360; j+= dtheta){
      angle = (float)j * (PI / 180.0f);
      cos_angle = cosf(angle);
      sin_angle = sinf(angle);
      x0 = rad0 * cos_angle + bend_radius;
      y0 = rad0 * sin_angle;
      x1 = rad1 * cos_angle + bend_radius;
      y1 = rad1 * sin_angle;
      
      C[0] = sin_ang0*rad0*cos_angle;
      C[1] = y0;
      C[2] = cos_ang0*rad0*cos_angle;
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      //glNormal3f(C[0], C[1], C[2]);
      xb = sin_ang0 * x0;
      yb = y0;
      zb = cos_ang0 * x0;
      //glVertex3f( xb, yb, zb );
      vertices[k] = xb; k++;
      vertices[k] = yb; k++;
      vertices[k] = zb; k++;

      //Colors:
      vertices[k] = R; k++;
      vertices[k] = G; k++;
      vertices[k] = B; k++;

      //Texture:
      vertices[k] = t;   k++;
      vertices[k] = 0.0; k++;

      //Normals:
      vertices[k] = C[0]; k++;
      vertices[k] = C[1]; k++;
      vertices[k] = C[2]; k++;
      
      C[0] = sin_ang1*rad1*cos_angle;
      C[1] = y1;
      C[2] = cos_ang1*rad1*cos_angle;
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      //glNormal3f(C[0], C[1], C[2]);
      xb = sin_ang1 * x1;
      yb = y1;
      zb = cos_ang1 * x1;
      //glVertex3f( xb, yb, zb );
      vertices[k] = xb; k++;
      vertices[k] = yb; k++;
      vertices[k] = zb; k++;

      //Colors:
      vertices[k] = R; k++;
      vertices[k] = G; k++;
      vertices[k] = B; k++;

      //Texture:
      vertices[k] = t;   k++;
      vertices[k] = 1.0; k++;

      //Normals:
      vertices[k] = C[0]; k++;
      vertices[k] = C[1]; k++;
      vertices[k] = C[2]; k++;

      t++;
      t = t%2;
    }
    //glEnd();
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, n*sizeof(float), vertices, GL_DYNAMIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(3* sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  // normal attribute
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(8 * sizeof(float)));
  glEnableVertexAttribArray(3);

  free(vertices);
  return VAO;
}

unsigned int create_cylinder_z(float radius0,
			       float radius1,
			       float z0,
			       float z1,
			       float R, float G, float B,
			       int dtheta){
  unsigned int VAO;
  unsigned int VBO;
  float *vertices;
  float angle, x0, y0, x1, y1, xb, yb, zb;
  float cos_angle, sin_angle;
  float mag;
  float C[3];
  int t = 0;
  int j,n,k;

  n = (360/dtheta+1)*2*11;
  k = 0;
  vertices = (float *)calloc(n, sizeof(float));

  //glBegin(GL_QUAD_STRIP);
  for(j = 0; j <= 360; j+= dtheta){
    angle = (float)j * (PI / 180.0f);
    cos_angle = cosf(angle);
    sin_angle = sinf(angle);
    y0 = radius0 * cos_angle;
    x0 = radius0 * sin_angle;
    y1 = radius1 * cos_angle;
    x1 = radius1 * sin_angle;
    
    C[0] = x0;
    C[1] = y0;
    C[2] = 0;
    mag = magnitude(C);
    C[0] /= mag;
    C[1] /= mag;
    C[2] /= mag;
    //glNormal3f(C[0], C[1], C[2]);
    yb = y0;
    xb = x0;
    zb = z0;
    //glVertex3f( xb, yb, zb );
    vertices[k] = xb; k++;
    vertices[k] = yb; k++;
    vertices[k] = zb; k++;
    
    //Colors:
    vertices[k] = R; k++;
    vertices[k] = G; k++;
    vertices[k] = B; k++;
    
    //Texture:
    vertices[k] = t;   k++;
    vertices[k] = 0.0; k++;
    
    //Normals:
    vertices[k] = C[0]; k++;
    vertices[k] = C[1]; k++;
    vertices[k] = C[2]; k++;
    
    C[0] = x1;
    C[1] = y1;
    C[2] = 0;
    mag = magnitude(C);
    C[0] /= mag;
    C[1] /= mag;
    C[2] /= mag;
    //glNormal3f(C[0], C[1], C[2]);
    yb = y1;
    xb = x1;
    zb = z1;
    //glVertex3f( xb, yb, zb );
    vertices[k] = xb; k++;
    vertices[k] = yb; k++;
    vertices[k] = zb; k++;
    
    //Colors:
    vertices[k] = R; k++;
    vertices[k] = G; k++;
    vertices[k] = B; k++;

    //Texture:
    vertices[k] = t;   k++;
    vertices[k] = 1.0; k++;

    //Normals:
    vertices[k] = C[0]; k++;
    vertices[k] = C[1]; k++;
    vertices[k] = C[2]; k++;    

    t++;
    t = t%2;
  }
  //glEnd();

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, n*sizeof(float), vertices, GL_DYNAMIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(3* sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  // normal attribute
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(8 * sizeof(float)));
  glEnableVertexAttribArray(3);

  free(vertices);
  return VAO;
}


unsigned int create_curved_cylinder_x(float radius0,
				      float radius1,
				      float bend_radius,
				      float bend_ang0,
				      float bend_ang1,
				      float R, float G, float B,
				      int slices,
				      int dtheta){
  unsigned int VAO;
  unsigned int VBO;
  float *vertices;
  float w0, w1, ang0, ang1, angle, x0, y0, x1, y1, xb, yb, zb;
  float cos_angle, sin_angle, cos_ang0, sin_ang0, cos_ang1, sin_ang1;
  float rad0, rad1, mag;
  float C[3];
  int t = 0;
  int i, j, k, n;

  n = slices*(360/dtheta+1)*2*11;
  k = 0;
  vertices = (float *)calloc(n, sizeof(float));

  bend_ang0 *= (PI / 180.0f);
  bend_ang1 *= (PI / 180.0f);
  for(i = 0; i < slices; i++){
    w0 = (float)i / (float)slices;
    w1 = (float)(i+1) / (float)slices;
    ang0 = bend_ang0*(1.0-w0) + bend_ang1*w0;
    ang1 = bend_ang0*(1.0-w1) + bend_ang1*w1;

    rad0 = radius0*(1.0-w0) + radius1*w0;
    rad1 = radius0*(1.0-w1) + radius1*w1;    

    sin_ang0 = sinf(ang0);
    cos_ang0 = cosf(ang0);
    sin_ang1 = sinf(ang1);
    cos_ang1 = cosf(ang1);
    
    //glBegin(GL_QUAD_STRIP);
    for(j = 0; j <= 360; j+= dtheta){
      angle = (float)j * (PI / 180.0f);
      cos_angle = cosf(angle);
      sin_angle = sinf(angle);
      y0 = rad0 * cos_angle + bend_radius;
      x0 = rad0 * sin_angle;
      y1 = rad1 * cos_angle + bend_radius;
      x1 = rad1 * sin_angle;
      
      C[0] = x0;
      C[1] = sin_ang0*rad0*cos_angle;
      C[2] = cos_ang0*rad0*cos_angle;
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      //glNormal3f(C[0], C[1], C[2]);
      yb = sin_ang0 * y0;
      xb = x0;
      zb = cos_ang0 * y0;
      //glVertex3f( xb, yb, zb );
      vertices[k] = xb; k++;
      vertices[k] = yb; k++;
      vertices[k] = zb; k++;

      //Colors:
      vertices[k] = R; k++;
      vertices[k] = G; k++;
      vertices[k] = B; k++;

      //Texture:
      vertices[k] = t;   k++;
      vertices[k] = 0.0; k++;

      //Normals:
      vertices[k] = C[0]; k++;
      vertices[k] = C[1]; k++;
      vertices[k] = C[2]; k++;
      
      C[0] = x1;
      C[1] = sin_ang1*rad1*cos_angle;
      C[2] = cos_ang1*rad1*cos_angle;
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      //glNormal3f(C[0], C[1], C[2]);
      yb = sin_ang1 * y1;
      xb = x1;
      zb = cos_ang1 * y1;
      //glVertex3f( xb, yb, zb );
      vertices[k] = xb; k++;
      vertices[k] = yb; k++;
      vertices[k] = zb; k++;

      //Colors:
      vertices[k] = R; k++;
      vertices[k] = G; k++;
      vertices[k] = B; k++;

      //Texture:
      vertices[k] = t;   k++;
      vertices[k] = 1.0; k++;

      //Normals:
      vertices[k] = C[0]; k++;
      vertices[k] = C[1]; k++;
      vertices[k] = C[2]; k++;

      t++;
      t = t%2;
    }
    //glEnd();
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, n*sizeof(float), vertices, GL_DYNAMIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(3* sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  // normal attribute
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(8 * sizeof(float)));
  glEnableVertexAttribArray(3);

  free(vertices);
  return VAO;
}


unsigned int create_ellipsoid_lune(float a, float b, float c,
				   int angle1, int angle2,
				   float R, float G, float B,
				   int slices){
  unsigned int VAO;
  unsigned int VBO;
  float *vertices;
  float w0, w1, angle, x0, y0, z0, x1, y1, z1, xb, yb, zb;
  float cos_angle, sin_angle;
  float mag, a0, a1, b0, b1;
  float C[3];
  int t = 0;
  int i, j, k, n;

  n = slices*(angle2-angle1+1)*2*11;
  k = 0;
  vertices = (float *)calloc(n, sizeof(float));
  for(i = 0; i < slices; i++){
    w0 = (float)i / (float)slices;
    w1 = (float)(i+1) / (float)slices;

    z0 = (-c)*(1.0-w0) + c*w0;
    z1 = (-c)*(1.0-w1) + c*w1;    

    b0 = sqrtf(b*b*(1.0 - (z0*z0)/(c*c)));
    b1 = sqrtf(b*b*(1.0 - (z1*z1)/(c*c)));

    a0 = sqrtf(a*a*(1.0 - (z0*z0)/(c*c)));
    a1 = sqrtf(a*a*(1.0 - (z1*z1)/(c*c)));
    
    //glBegin(GL_QUAD_STRIP);
    for(j = angle1; j <= angle2; j++){
      angle = (float)j * (PI / 180.0f);
      cos_angle = cosf(angle);
      sin_angle = sinf(angle);
      x0 = a0 * cos_angle;
      y0 = b0 * sin_angle;
      x1 = a1 * cos_angle;
      y1 = b1 * sin_angle;
      
      C[0] = (2.0*x0)/(a*a); //b0*cos_angle;
      C[1] = (2.0*y0)/(b*b); //y0;
      C[2] = (2.0*z0)/(c*c); //b0*cos_angle;
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      //glNormal3f(C[0], C[1], C[2]);
      
      //glVertex3f( x0, y0, z0 );
      vertices[k] = x0; k++;
      vertices[k] = y0; k++;
      vertices[k] = z0; k++;

      //Colors:
      vertices[k] = R; k++;
      vertices[k] = G; k++;
      vertices[k] = B; k++;

      //Texture:
      vertices[k] = t;   k++;
      vertices[k] = 0.0; k++;

      //Normals:
      vertices[k] = C[0]; k++;
      vertices[k] = C[1]; k++;
      vertices[k] = C[2]; k++;
     
      C[0] = (2.0*x1)/(a*a); //b1*cos_angle;
      C[1] = (2.0*y1)/(b*b); //y1;
      C[2] = (2.0*z1)/(c*c); //b1*cos_angle;
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      //glNormal3f(C[0], C[1], C[2]);
      
      //glVertex3f( x1, y1, z1 );
      vertices[k] = x1; k++;
      vertices[k] = y1; k++;
      vertices[k] = z1; k++;

      //Colors:
      vertices[k] = R; k++;
      vertices[k] = G; k++;
      vertices[k] = B; k++;

      //Texture:
      vertices[k] = t;   k++;
      vertices[k] = 1.0; k++;

      //Normals:
      vertices[k] = C[0]; k++;
      vertices[k] = C[1]; k++;
      vertices[k] = C[2]; k++;
      
      t++;
      t = t%2;
    }
    //glEnd();
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, n*sizeof(float), vertices, GL_DYNAMIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(3* sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  // normal attribute
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(8 * sizeof(float)));
  glEnableVertexAttribArray(3);
  
  free(vertices);
  return VAO;
}

void drawCube(GLuint tex[], struct GameData *GD, int id1, int id2)
{
  glBindVertexArray(blockVAO);
  glBindTexture(GL_TEXTURE_2D, tex[id1]);
  glDrawArrays(GL_TRIANGLES, 0, 24);
  glBindTexture(GL_TEXTURE_2D, tex[id2]);
  glDrawArrays(GL_TRIANGLES, 24, 12);
  glBindTexture(GL_TEXTURE_2D, GD->null_tex);
}


void draw_curved_cylinder_x(unsigned int VAO,
			    int slices,
			    int dtheta){
  int i,k;
  glBindVertexArray(VAO);
  k = 0;
  for(i = 0; i < slices; i++){
    glDrawArrays(GL_TRIANGLE_STRIP, k, (360/dtheta+1)*2);
    k += (360/dtheta+1)*2;
  }
}

void draw_curved_cylinder_y(unsigned int VAO,
			    int slices,
			    int dtheta){
  int i,k;
  glBindVertexArray(VAO);
  k = 0;
  for(i = 0; i < slices; i++){
    glDrawArrays(GL_TRIANGLE_STRIP, k, (360/dtheta+1)*2);
    k += (360/dtheta+1)*2;
  }
}



void draw_ellipsoid_lune(unsigned int VAO,
			 int angle1, int angle2,
			 int slices){
  int i,k;

  glBindVertexArray(VAO);
  k = 0;
  for(i = 0; i < slices; i++){
    glDrawArrays(GL_TRIANGLE_STRIP, k, (angle2-angle1+1)*2);
    k += (angle2-angle1+1)*2;
  }
}


unsigned int create_hairy_ellipsoid(float R, float G, float B){
  float th0, th1;
  float hair_R = 8.0;
  th0 = 90.0;
  th1 = 180.0;
  return create_curved_cylinder_x(3.0,
				  0.0,
				  hair_R,
				  th0, th1,
				  R, G, B,
				  20, 4);
}

unsigned int create_ellipsoid(float a, float b, float c,
			      float R, float G, float B,
			      int slices){
  unsigned int VAO;
  unsigned int VBO;
  float *vertices;
  float w0, w1, angle, x0, y0, z0, x1, y1, z1, xb, yb, zb;
  float cos_angle, sin_angle;
  float mag, a0, a1, b0, b1;
  float C[3];
  int t = 0;
  int i, j, k, n, dangle = 1; //2;
  
  n = slices*(360/dangle+1)*2*11;
  k = 0;
  vertices = (float *)calloc(n, sizeof(float));
  for(i = 0; i < slices; i++){
    w0 = (float)i / (float)slices;
    w1 = (float)(i+1) / (float)slices;

    z0 = (-c)*(1.0-w0) + c*w0;
    z1 = (-c)*(1.0-w1) + c*w1;    

    b0 = sqrtf(b*b*(1.0 - (z0*z0)/(c*c)));
    b1 = sqrtf(b*b*(1.0 - (z1*z1)/(c*c)));

    a0 = sqrtf(a*a*(1.0 - (z0*z0)/(c*c)));
    a1 = sqrtf(a*a*(1.0 - (z1*z1)/(c*c)));
    
    //glBegin(GL_TRIANGLE_STRIP);
    for(j = 0; j <= 360; j+=dangle){
      angle = (float)j * (PI / 180.0f);
      cos_angle = cosf(angle);
      sin_angle = sinf(angle);
      x0 = a0 * cos_angle;
      y0 = b0 * sin_angle;
      x1 = a1 * cos_angle;
      y1 = b1 * sin_angle;
      
      C[0] = (2.0*x0)/(a*a); //b0*cos_angle;
      C[1] = (2.0*y0)/(b*b); //y0;
      C[2] = (2.0*z0)/(c*c); //b0*cos_angle;
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      //glNormal3f(C[0], C[1], C[2]);
      
      //glVertex3f( x0, y0, z0 );
      vertices[k] = x0; k++;
      vertices[k] = y0; k++;
      vertices[k] = z0; k++;

      //Colors:
      vertices[k] = R; k++;
      vertices[k] = G; k++;
      vertices[k] = B; k++;

      //Texture:
      vertices[k] = t;   k++;
      vertices[k] = 0.0; k++;

      //Normals:
      vertices[k] = C[0]; k++;
      vertices[k] = C[1]; k++;
      vertices[k] = C[2]; k++;
      
      C[0] = (2.0*x1)/(a*a); //b1*cos_angle;
      C[1] = (2.0*y1)/(b*b); //y1;
      C[2] = (2.0*z1)/(c*c); //b1*cos_angle;
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      //glNormal3f(C[0], C[1], C[2]);
      
      //glVertex3f( x1, y1, z1 );
      vertices[k] = x1; k++;
      vertices[k] = y1; k++;
      vertices[k] = z1; k++;

      //Colors:
      vertices[k] = R; k++;
      vertices[k] = G; k++;
      vertices[k] = B; k++;

      //Texture:
      vertices[k] = t;   k++;
      vertices[k] = 1.0; k++;

      //Normals:
      vertices[k] = C[0]; k++;
      vertices[k] = C[1]; k++;
      vertices[k] = C[2]; k++;
      
      t++;
      t = t%2;
    }
    //glEnd();
  }
  
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, n*sizeof(float), vertices, GL_DYNAMIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(3* sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  // normal attribute
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
			(void*)(8 * sizeof(float)));
  glEnableVertexAttribArray(3);

  free(vertices);
  return VAO;
}

void draw_ellipsoid(unsigned int VAO,
		    int slices){
  int i,k,dangle = 1; //2;

  glBindVertexArray(VAO);
  k = 0;
  for(i = 0; i < slices; i++){
    glDrawArrays(GL_TRIANGLE_STRIP, k, (360/dangle+1)*2);
    k += (360/dangle+1)*2;
  }
}

void draw_cylinder_z(unsigned int VAO,
		     int dtheta){
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, (360/dtheta+1)*2);
}


void draw_ellipsoid(float a,
                    float b,
                    float c,
                    int slices)
{
  GLfloat w0, w1, angle, x0, y0, z0, x1, y1, z1, xb, yb, zb;
  float cos_angle, sin_angle;
  float mag, a0, a1, b0, b1;
  float C[3];
  int i, j;
  for (i = 0; i < slices; i++)
  {
    w0 = (float)i / (float)slices;
    w1 = (float)(i + 1) / (float)slices;

    z0 = (-c) * (1.0 - w0) + c * w0;
    z1 = (-c) * (1.0 - w1) + c * w1;

    b0 = sqrtf(b * b * (1.0 - (z0 * z0) / (c * c)));
    b1 = sqrtf(b * b * (1.0 - (z1 * z1) / (c * c)));

    a0 = sqrtf(a * a * (1.0 - (z0 * z0) / (c * c)));
    a1 = sqrtf(a * a * (1.0 - (z1 * z1) / (c * c)));

    glBegin(GL_QUAD_STRIP);
    for (j = 0; j <= 360; j++)
    {
      angle = (float)j * (PI / 180.0f);
      cos_angle = cosf(angle);
      sin_angle = sinf(angle);
      x0 = a0 * cos_angle;
      y0 = b0 * sin_angle;
      x1 = a1 * cos_angle;
      y1 = b1 * sin_angle;

      C[0] = (2.0 * x0) / (a * a);
      C[1] = (2.0 * y0) / (b * b);
      C[2] = (2.0 * z0) / (c * c);
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      glNormal3f(C[0], C[1], C[2]);

      glVertex3f(x0, y0, z0);

      C[0] = (2.0 * x1) / (a * a);
      C[1] = (2.0 * y1) / (b * b);
      C[2] = (2.0 * z1) / (c * c);
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      glNormal3f(C[0], C[1], C[2]);

      glVertex3f(x1, y1, z1);
    }
    glEnd();
  }
}

void draw_ellipsoid_lune(float a,
                         float b,
                         float c,
                         int angle1,
                         int angle2,
                         int slices)
{
  GLfloat w0, w1, angle, x0, y0, z0, x1, y1, z1, xb, yb, zb;
  float cos_angle, sin_angle;
  float mag, a0, a1, b0, b1;
  float C[3];
  int i, j;

  float vertices[10];
  int cont = 0;
  for (i = 0; i < slices; i++)
  {
    cont = 0;
    w0 = (float)i / (float)slices;
    w1 = (float)(i + 1) / (float)slices;

    z0 = (-c) * (1.0 - w0) + c * w0;
    z1 = (-c) * (1.0 - w1) + c * w1;

    b0 = sqrtf(b * b * (1.0 - (z0 * z0) / (c * c)));
    b1 = sqrtf(b * b * (1.0 - (z1 * z1) / (c * c)));

    a0 = sqrtf(a * a * (1.0 - (z0 * z0) / (c * c)));
    a1 = sqrtf(a * a * (1.0 - (z1 * z1) / (c * c)));

    glBegin(GL_QUAD_STRIP);
    for (j = angle1; j <= angle2; j++)
    {
      angle = (float)j * (PI / 180.0f);
      cos_angle = cosf(angle);
      sin_angle = sinf(angle);
      x0 = a0 * cos_angle;
      y0 = b0 * sin_angle;
      x1 = a1 * cos_angle;
      y1 = b1 * sin_angle;

      C[0] = (2.0 * x0) / (a * a);
      C[1] = (2.0 * y0) / (b * b);
      C[2] = (2.0 * z0) / (c * c);
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      glNormal3f(C[0], C[1], C[2]);

      glVertex3f(x0, y0, z0);

      C[0] = (2.0 * x1) / (a * a);
      C[1] = (2.0 * y1) / (b * b);
      C[2] = (2.0 * z1) / (c * c);
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      glNormal3f(C[0], C[1], C[2]);

      glVertex3f(x1, y1, z1);
    }
    glEnd();
  }
}

void draw_hairy_ellipsoid(unsigned int VAO,
			  glm::mat4 std_model, glm::mat4 std_normal,
			  unsigned int modelLoc, unsigned int normalLoc,
			  float a, float b, float c,
			  int slices){
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 normal = glm::mat4(1.0f);

  // int k;
  // glBindVertexArray(hairVAO);
  // k = 0;
  // for(int i = 0; i < 20; i++){
  //   glDrawArrays(GL_TRIANGLE_STRIP, k, (360/4+1)*2);
  //   k += (360/4+1)*2;
  // }
  // glBindVertexArray(0);
  // return;
  float w0, w1, angle, x0, y0, z0, x1, y1, z1, xb, yb, zb;
  float cos_angle, sin_angle, hair_theta, hair_R;
  float mag, a0, a1, b0, b1, th0, th1, c_th0, s_th0;
  float C[3];
  int i, j, dj = 0, di = 3;
  hair_R = 8.0;
  th0 = 90.0;
  th1 = 180.0;
  c_th0 = cosf(th0*(PI / 180.0f));
  s_th0 = sinf(th0*(PI / 180.0f));

  for(i = 1; i < (int)(0.8*slices+0.5); i++){
    if(i%di != 0) continue;
    w0 = (float)i / (float)slices;
    w1 = (float)(i+1) / (float)slices;

    z0 = (-c)*(1.0-w0) + c*w0;
    z1 = (-c)*(1.0-w1) + c*w1;    

    b0 = sqrtf(b*b*(1.0 - (z0*z0)/(c*c)));
    b1 = sqrtf(b*b*(1.0 - (z1*z1)/(c*c)));

    a0 = sqrtf(a*a*(1.0 - (z0*z0)/(c*c)));
    a1 = sqrtf(a*a*(1.0 - (z1*z1)/(c*c)));

    if(dj == 0)
      dj = 10;
    else
      dj = 0;
    
    for(j = dj; j <= 360; j+=20){ 
      angle = (float)j * (PI / 180.0f);
      cos_angle = cosf(angle);
      sin_angle = sinf(angle);
      x0 = a0 * cos_angle;
      y0 = b0 * sin_angle;
      x1 = a1 * cos_angle;
      y1 = b1 * sin_angle;
      
      C[0] = (2.0*x0)/(a*a); //b0*cos_angle;
      C[1] = (2.0*y0)/(b*b); //y0;
      C[2] = (2.0*z0)/(c*c); //b0*cos_angle;
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      //glNormal3f(C[0], C[1], C[2]);
      //glVertex3f( x0, y0, z0 );

      hair_theta = -(atan2f(x0, -z0)*(180.0f/PI))/2.0;
      //hair_theta = -60;
      //hair_theta = 0;      

      model = std_model;
      normal = std_normal;
      model = glm::translate(model,
			     glm::vec3(x0 - C[0]*2,
				       y0 - C[1]*2 - s_th0*hair_R,
				       z0 - C[2]*2 - c_th0*hair_R));
      model = glm::rotate(model, glm::radians(hair_theta),
			  glm::vec3(0.0, 1.0, 0.0));
      normal = glm::rotate(normal, glm::radians(hair_theta),
			   glm::vec3(0.0, 1.0, 0.0));
      glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
      glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
      // std::cout << glm::to_string(model) << "\n" << std::endl;
      // std::cout << "chamo draw_curved";

      // float aaa[16] = { 0.500364, 0.0, 0.865815, 0.0, 
      // 0.0, 1.0, 0.0, 0.0,
      // -0.86581, 0.0, 0.500364, 0.0,
      // 79.052498, 52.062199, 251.005051, 1.0
      // };
      // glm::mat4 bbb;
      // memcpy( glm::value_ptr( bbb ), aaa, sizeof( aaa ) );
      // glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(bbb));
      draw_curved_cylinder_x(VAO, 20, 4);
      
      C[0] = (2.0*x1)/(a*a); //b1*cos_angle;
      C[1] = (2.0*y1)/(b*b); //y1;
      C[2] = (2.0*z1)/(c*c); //b1*cos_angle;
      mag = magnitude(C);
      C[0] /= mag;
      C[1] /= mag;
      C[2] /= mag;
      //glNormal3f(C[0], C[1], C[2]);
      //glVertex3f( x1, y1, z1 );
    }
  }
}




void DrawFluffyBody(
		    float a, float b, float c,
		    unsigned int modelLoc,
		    unsigned int normalLoc,
		    glm::mat4 std_model,
		    glm::mat4 std_normal,
		    float deltaTime){
  static bool firsttime = true;
  glm::mat4 model;
  glm::mat4 normal;
  static float eye_angle = -10.0; //Intervalo: [-10.0, -80.0]
  static float eye_angle_step = 150.0*deltaTime;
  eye_angle_step = SIGN(eye_angle_step)*150.0*deltaTime;
  //------------Body hair:--------------------------
  static unsigned int hair_VAO;
  if(firsttime)
    hair_VAO = create_hairy_ellipsoid(1.0, 1.0, 1.0);

  // glBindTexture(GL_TEXTURE_2D, GD->null_tex);

  // if(GD->skin == SKIN_Professor)
  //   DrawFluffyProfessorSkin(GD, a, b, c, modelLoc, normalLoc,
	// 		    std_model, std_normal, deltaTime);


  draw_hairy_ellipsoid(hair_VAO,
		       std_model, std_normal,
		       modelLoc, normalLoc,
		       a, b, c,
		       40);
  //------------Body and mouth:---------------------------
  static unsigned int body_VAO[4];
  if(firsttime){
    body_VAO[0] = create_ellipsoid_lune(a, b, c,
					-5, 340,
					1.0, 1.0, 1.0,
					40);
    body_VAO[1] = create_ellipsoid_lune(a*0.95, b*0.95, c*0.95,
					-16,   0,
					1.0, 1.0, 1.0,
					40);
    body_VAO[2] = create_ellipsoid_lune(a*0.95, b*0.95, c*0.95,
					-40, -18,
					1.0, 1.0, 1.0,
					40);
    body_VAO[3] = create_ellipsoid_lune(a*0.94, b*0.94, c*0.94,
					-40, 0,
					0.0, 0.0, 0.0,
					40);
  }
  normal = std_normal;
  normal = glm::rotate(normal, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  model = std_model;
  model  = glm::rotate(model,  glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(body_VAO[0], -5, 340, 40);
  draw_ellipsoid_lune(body_VAO[1], -16, 0, 40);
  draw_ellipsoid_lune(body_VAO[2], -40, -18, 40);
  draw_ellipsoid_lune(body_VAO[3], -40, 0, 40);
  // //------------Fix cheek:-------------------------------
  static unsigned int cheek_VAO[4];
  if(firsttime){
    cheek_VAO[0] = create_ellipsoid_lune(a, c, b,
					 0,  60,
					 1.0, 1.0, 1.0,
					 40);
    cheek_VAO[1] = create_ellipsoid_lune(a, c, b,
					 120,  180,
					 1.0, 1.0, 1.0,
					 40);
    cheek_VAO[2] = create_ellipsoid_lune(a*0.97, c*0.97, b*0.97,
					 55,  65,
					 0.0, 0.0, 0.0,
					 40);
    cheek_VAO[3] = create_ellipsoid_lune(a*0.97, c*0.97, b*0.97,
					 115, 125,
					 0.0, 0.0, 0.0,
					 40);
  }
  model = std_model;
  model = glm::rotate(model,  glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = std_normal;
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(cheek_VAO[0], 0, 60, 40);
  draw_ellipsoid_lune(cheek_VAO[1], 120, 180, 40);
  draw_ellipsoid_lune(cheek_VAO[2], 55, 65, 40);
  draw_ellipsoid_lune(cheek_VAO[3], 115, 125, 40);
  // //------------Tuft of hair:------------------------
  static unsigned int tuft_VAO;
  if(firsttime){
    tuft_VAO = create_curved_cylinder_x(2.0, 0.0, 8.0,
					-20.0, 130.0,
					1.0, 1.0, 1.0,
					20, 4);
  }
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(0.0, b*0.9, c*0.15));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_x(tuft_VAO, 20, 4);

  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-5.0, b*0.9, c*0.15));
  model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_x(tuft_VAO, 20, 4);
  
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(5.0, b*0.9, c*0.15));
  model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_x(tuft_VAO, 20, 4);
  
  // //------------Tail:--------------------------------
  static unsigned int tail_VAO;
  if(firsttime)
    tail_VAO = create_ellipsoid(b*0.30, b*0.30, b*0.30,
				1.0, 1.0, 1.0,
				10);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3( 0.0, 0.0, -c*1.05));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(tail_VAO, 10);
  // //------------Eyebrow:-----------------------------
  static unsigned int eyebrow_VAO[2];
  if(firsttime)
    eyebrow_VAO[0] = create_ellipsoid(b*0.25, b*0.15, b*0.15,
				      1.0, 1.0, 1.0,
				      10);
  normal = std_normal;
  model = std_model;
  normal = glm::rotate(normal, glm::radians(-35.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3(-a*0.15, b*0.55, c*0.75));
  model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));  
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(eyebrow_VAO[0], 10);

  if(firsttime)
    eyebrow_VAO[1] = create_ellipsoid(b*0.25, b*0.15, b*0.15,
				      1.0, 1.0, 1.0,
				      10);
  normal = std_normal;
  model = std_model;
  normal = glm::rotate(normal, glm::radians(35.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3(a*0.15, b*0.55, c*0.75));
  model = glm::rotate(model, glm::radians(35.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));  
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(eyebrow_VAO[1], 10);
  // //------------Eyes:--------------------------------
  static unsigned int eyes_VAO[2];
  int eye_opening = 180;
  int angle_r1, angle_r2;
  int angle_l1, angle_l2;
  angle_r1 = 270-eye_opening/2;
  angle_r2 = 270+eye_opening/2;
  angle_l1 = 270+eye_opening/2-360;
  angle_l2 = 270-eye_opening/2;
  if(firsttime){
    eyes_VAO[0] = create_ellipsoid_lune(b*0.45, b*0.45, b*0.45,
					angle_r1, angle_r2,
					0.65, 0.65, 0.65,
					10);
    eyes_VAO[1] = create_ellipsoid_lune(b*0.45, b*0.45, b*0.45,
					angle_l1, angle_l2,
					1.0, 1.0, 1.0,
					10);
  }
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(0.0, b*0.25, c*0.5));
  model  = glm::rotate(model, glm::radians(eye_angle), glm::vec3(1.0, 0.0, 0.0));
  model  = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(eye_angle), glm::vec3(1.0, 0.0, 0.0));  
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));  
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(eyes_VAO[0], angle_r1, angle_r2, 10);
  draw_ellipsoid_lune(eyes_VAO[1], angle_l1, angle_l2, 10);

  //Intervalo: [-10.0, -80.0]
  eye_angle += eye_angle_step;
  if(eye_angle < -80.0){
    eye_angle = -80.0;
    eye_angle_step = -eye_angle_step;
  }
  else if(eye_angle > -10.0){
    eye_angle = -10.0;
    eye_angle_step = -eye_angle_step;
  }
  // //------------Nose:--------------------------------
  static unsigned int nose_VAO;
  if(firsttime)
    nose_VAO = create_ellipsoid(b*0.20, b*0.10, b*0.15,
				0.0, 0.0, 0.0,
				10);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3( 0.0, b*0.15, c*1.05));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(nose_VAO, 10);
  // //-----------------------------------------------
  firsttime = false;
}


float get_player_angle(velocidade *rotacao){
  if(rotacao->vx == 0 && rotacao->vz == 1)
    return 0.0;
  else if(rotacao->vx == 0 && rotacao->vz == -1)
    return 180.0;
  else if(rotacao->vx == -1 && rotacao->vz == 0)
    return -90.0;
  else if(rotacao->vx == 1 && rotacao->vz == 0)
    return 90.0;
  return 0.0;
}

void DrawFluffy(
		unsigned int shaderID,
    float deltaTime,
    bool restart,
    posicao *pos,
    velocidade *rotacao,
    float scale){
  float d = 60.0, a, b, c;
  static bool firsttime = true;
  glm::mat4 model = glm::mat4(1.0f);
  unsigned int modelLoc = glGetUniformLocation(shaderID,"model");
  glm::mat4 normal = glm::mat4(1.0f);
  unsigned int normalLoc = glGetUniformLocation(shaderID,"normal");
  float player_angle = get_player_angle(rotacao);
  
  glm::mat4 std_model = glm::mat4(1.0f);
  
  // std::cout << "X:" << GD->Player->pos->x << " Y: " << GD->Player->pos->y << " Z: "<< GD->Player->pos->z << "\n";
  std_model = glm::translate(std_model, glm::vec3(pos->x*d, pos->y*d, pos->z*d));
  std_model = glm::rotate(std_model, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));
  // std_model = glm::scale(std_model, glm::vec3(1.0f,1.0f,1.0f));

  glm::mat4 std_normal = glm::mat4(1.0f);
  std_normal = glm::rotate(std_normal, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));
  a = 0.8*d/2.0;
  b = d/2.0;
  c = 0.8*d/2.0;
  //------------Body:------------------------------
  DrawFluffyBody(a, b, c, modelLoc, normalLoc,
		 std_model, std_normal, deltaTime);
  //------------Feet:------------------------------
  static unsigned int feet_VAO[2];
  if(firsttime){
    feet_VAO[0] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
    feet_VAO[1] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
  }
  normal = std_normal;
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.35, -b,  c*0.2));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid_lune(feet_VAO[0], 0, 180, 40);
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.35, -b,  c*0.2));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid_lune(feet_VAO[1], 0, 180, 40);
  // //------------Hands:------------------------------
  static unsigned int hands_VAO[6];
  // //------------Left hand:--------------------------
  if(firsttime)
    hands_VAO[0] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.95, 0.0,  c*0.75));
  glUniformMatrix4fv(modelLoc, 1,  GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[0], 40);

  if(firsttime){
    
    hands_VAO[1] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  }
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[1], 40);  
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[2] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3(-a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[2], 40);
  // //------------Right hand:--------------------------
  if(firsttime)
    hands_VAO[3] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.95, 0.0,  c*0.75));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[3], 40);

  if(firsttime)
    hands_VAO[4] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[4], 40);  
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[5] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3( a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[5], 40);
  //-------------------------------------------------
  firsttime = false;
}

void DrawFluffyPush(struct GameData *GD,
		     unsigned int shaderID,
		     float deltaTime,
		     bool restart){
  float d = 60.0, a, b, c;
  static bool firsttime = true;
  glm::mat4 model = glm::mat4(1.0f);
  unsigned int modelLoc = glGetUniformLocation(shaderID,"model");
  glm::mat4 normal = glm::mat4(1.0f);
  unsigned int normalLoc = glGetUniformLocation(shaderID,"normal");
  float player_angle = get_player_angle(GD->Player->rotacao);
  static float length_forearm = 0.0;
  static float step_forearm = 5.0*deltaTime;
  step_forearm = SIGN(step_forearm)*5.0*deltaTime;

  if(restart){
    length_forearm = 0.0;
  }
  
  glm::mat4 std_model = glm::mat4(1.0f);
  std_model = glm::translate(std_model, glm::vec3(GD->Player->pos->x*d, GD->Player->pos->y*d, GD->Player->pos->z*d));
  std_model = glm::rotate(std_model, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));

  glm::mat4 std_normal = glm::mat4(1.0f);
  std_normal = glm::rotate(std_normal, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));

  a = 0.8*d/2.0;
  b = d/2.0;
  c = 0.8*d/2.0;
  //------------Body:------------------------------
  DrawFluffyBody(a, b, c, modelLoc, normalLoc,
		 std_model, std_normal, deltaTime);
  //------------Feet:------------------------------
  static unsigned int feet_VAO[2];
  if(firsttime){
    feet_VAO[0] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
    feet_VAO[1] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
  }
  normal = std_normal;
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.35, -b,  c*0.2));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid_lune(feet_VAO[0], 0, 180, 40);
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.35, -b,  c*0.2));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid_lune(feet_VAO[1], 0, 180, 40);
  //------------Hands:------------------------------
  static unsigned int hands_VAO[6];
  //------------Left arm:---------------------------
  static unsigned int larm_VAO;
  if(firsttime)
    larm_VAO = create_curved_cylinder_y(3.0, 3.0,
					0.27*d/2.0, 90.0, 180.0,
					1.0, 1.0, 1.0,
					20, 4);
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.95 - 0.27*d/2.0, -c*0.25,  c*0.75 - a*0.10));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  draw_curved_cylinder_y(larm_VAO, 20, 4);
  //------------Left forearm:-----------------------
  static unsigned int lforearm_VAO;
  if(firsttime)
    lforearm_VAO = create_cylinder_z(3.0, 3.0,
				     0, d/2.0,
				     1.0, 1.0, 1.0,
				     5);
  if(length_forearm > 0.0){
    normal = std_normal;
    model = std_model;
    model = glm::translate(model, glm::vec3(a*0.95, -c*0.25,  c*0.75 - a*0.10));
    model = glm::scale(model, glm::vec3(1.0, 1.0, length_forearm));
    glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
    draw_cylinder_z(lforearm_VAO, 5);
  }
  //------------Left hand:--------------------------
  if(firsttime)
    hands_VAO[0] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.95, 0.0,  c*0.75 + (d/2.0)*length_forearm));
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1,  GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[0], 40);

  if(firsttime)
    hands_VAO[1] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[1], 40);  
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[2] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3(-a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[2], 40);
  //------------Right arm:---------------------------
  static unsigned int rarm_VAO;
  if(firsttime)
    rarm_VAO = create_curved_cylinder_y(3.0, 3.0,
					0.27*d/2.0, 180.0, 270.0,
					1.0, 1.0, 1.0,
					20, 4);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.95 + 0.27*d/2.0, -c*0.25,  c*0.75 - a*0.10));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_y(rarm_VAO, 20, 4);
  //------------Right forearm:-----------------------
  static unsigned int rforearm_VAO;
  if(firsttime)
    rforearm_VAO = create_cylinder_z(3.0, 3.0,
				     0, d/2.0,
				     1.0, 1.0, 1.0,
				     5);
  if(length_forearm > 0.0){
    normal = std_normal;
    model = std_model;
    model = glm::translate(model, glm::vec3(-a*0.95, -c*0.25,  c*0.75 - a*0.10));
    model = glm::scale(model, glm::vec3(1.0, 1.0, length_forearm));
    glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
    draw_cylinder_z(rforearm_VAO, 5);
  }
  //------------Right hand:--------------------------
  if(firsttime)
    hands_VAO[3] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.95, 0.0,  c*0.75 + (d/2.0)*length_forearm));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[3], 40);

  if(firsttime)
    hands_VAO[4] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[4], 40);  
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[5] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3( a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[5], 40);

  length_forearm += step_forearm;
  if(length_forearm > 1.0){
    step_forearm = -step_forearm;
    length_forearm = 1.0;
  }
  else if(length_forearm < 0.0){
    step_forearm = -step_forearm;
    length_forearm = 0.0;
  }
  //-------------------------------------------------
  firsttime = false;
}

void DrawFluffyPull(struct GameData *GD,
		     unsigned int shaderID,
		     float deltaTime,
		     bool restart){
  float d = 60.0, a, b, c;
  static bool firsttime = true;
  glm::mat4 model = glm::mat4(1.0f);
  unsigned int modelLoc = glGetUniformLocation(shaderID,"model");
  glm::mat4 normal = glm::mat4(1.0f);
  unsigned int normalLoc = glGetUniformLocation(shaderID,"normal");
  float player_angle = get_player_angle(GD->Player->rotacao);
  static float length_forearm = 0.3;
  static float step_forearm = 0.0;
  static float foot_angle = 0.0;
  static float foot_angle_step = 300.0*deltaTime;
  foot_angle_step = SIGN(foot_angle_step)*300.0*deltaTime;

  if(restart){
    foot_angle = 0.0;
  }
  
  glm::mat4 std_model = glm::mat4(1.0f);
  std_model = glm::translate(std_model, glm::vec3(GD->Player->pos->x*d, GD->Player->pos->y*d, GD->Player->pos->z*d));
  std_model = glm::rotate(std_model, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));

  glm::mat4 std_normal = glm::mat4(1.0f);
  std_normal = glm::rotate(std_normal, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));

  a = 0.8*d/2.0;
  b = d/2.0;
  c = 0.8*d/2.0;
  //------------Body:------------------------------
  DrawFluffyBody(a, b, c, modelLoc, normalLoc,
		 std_model, std_normal, deltaTime);
  //------------Feet:------------------------------
  static unsigned int feet_VAO[2];
  if(firsttime){
    feet_VAO[0] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
    feet_VAO[1] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
  }
  normal = std_normal;
  model = std_model;
  normal = glm::rotate(normal, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  model = glm::translate(model, glm::vec3(a*0.35, -b,  c*0.2));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(feet_VAO[0], 0, 180, 40);

  normal = std_normal;
  model = std_model;
  normal = glm::rotate(normal, glm::radians(-foot_angle), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(-foot_angle), glm::vec3(1.0, 0.0, 0.0));
  model = glm::translate(model, glm::vec3(-a*0.35, -b,  c*0.2));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(feet_VAO[1], 0, 180, 40);

  foot_angle += foot_angle_step;
  if(foot_angle > 45.0){
    foot_angle = 45.0;
    foot_angle_step = -foot_angle_step;
  }
  else if(foot_angle < -45.0){
    foot_angle = -45.0;
    foot_angle_step = -foot_angle_step;
  }

  //------------Hands:------------------------------
  static unsigned int hands_VAO[6];
  //------------Left arm:---------------------------
  static unsigned int larm_VAO;
  if(firsttime)
    larm_VAO = create_curved_cylinder_y(3.0, 3.0,
					0.27*d/2.0, 90.0, 180.0,
					1.0, 1.0, 1.0,
					20, 4);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*1.0 - 0.27*d/2.0, -c*0.25,  c*0.75 - a*0.10));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_y(larm_VAO, 20, 4);
  //------------Left forearm:-----------------------
  static unsigned int lforearm_VAO;
  if(firsttime)
    lforearm_VAO = create_cylinder_z(3.0, 3.0,
				     0, d/2.0,
				     1.0, 1.0, 1.0,
				     5);
  if(length_forearm > 0.0){
    normal = std_normal;
    model = std_model;
    model = glm::translate(model, glm::vec3(a*1.0, -c*0.25,  c*0.75 - a*0.10));
    model = glm::scale(model, glm::vec3(1.0, 1.0, length_forearm));
    glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
    draw_cylinder_z(lforearm_VAO, 5);
  }
  //------------Left hand:--------------------------
  if(firsttime)
    hands_VAO[0] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*1.15, -b*0.05,  c*0.75 + (d/2.0)*length_forearm));
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1,  GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[0], 40);

  if(firsttime)
    hands_VAO[1] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[1], 40);  
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[2] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  //normal = std_normal;
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3(-a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[2], 40);
  //------------Right arm:---------------------------
  static unsigned int rarm_VAO;
  if(firsttime)
    rarm_VAO = create_curved_cylinder_y(3.0, 3.0,
					0.27*d/2.0, 180.0, 270.0,
					1.0, 1.0, 1.0,
					20, 4);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*1.0 + 0.27*d/2.0, -c*0.25,  c*0.75 - a*0.10));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_y(rarm_VAO, 20, 4);
  //------------Right forearm:-----------------------
  static unsigned int rforearm_VAO;
  if(firsttime)
    rforearm_VAO = create_cylinder_z(3.0, 3.0,
				     0, d/2.0,
				     1.0, 1.0, 1.0,
				     5);
  if(length_forearm > 0.0){
    normal = std_normal;
    model = std_model;
    model = glm::translate(model, glm::vec3(-a*1.0, -c*0.25,  c*0.75 - a*0.10));
    model = glm::scale(model, glm::vec3(1.0, 1.0, length_forearm));
    glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
    draw_cylinder_z(rforearm_VAO, 5);
  }
  //------------Right hand:--------------------------
  if(firsttime)
    hands_VAO[3] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*1.15, -b*0.05,  c*0.75 + (d/2.0)*length_forearm));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::rotate(model, glm::radians( 90.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians( 90.0f), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[3], 40);

  if(firsttime)
    hands_VAO[4] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[4], 40);  
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[5] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  //normal = std_normal;
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3( a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[5], 40);
  //-------------------------------------------------
  firsttime = false;
}


void DrawFluffyWalk(  
        unsigned int shaderID,
        float deltaTime,
        bool restart,
        posicao *pos,
        velocidade *rotacao,
        float scale){
  float d = 60.0, a, b, c;
  static bool firsttime = true;
  glm::mat4 model = glm::mat4(1.0f);
  unsigned int modelLoc = glGetUniformLocation(shaderID,"model");
  glm::mat4 normal = glm::mat4(1.0f);
  unsigned int normalLoc = glGetUniformLocation(shaderID,"normal");
  float player_angle = get_player_angle(rotacao);
  static float foot_angle = 0.0;
  static float foot_angle_step = 300.0*deltaTime;
  foot_angle_step = SIGN(foot_angle_step)*300.0*deltaTime;
  

  if(restart){
    foot_angle = 0.0;
  }
  
  glm::mat4 std_model = glm::mat4(1.0f);
  std_model = glm::translate(std_model, glm::vec3(pos->x*d, pos->y*d, pos->z*d));
  std_model = glm::rotate(std_model, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));
  std_model = glm::scale(std_model, glm::vec3(scale, scale, scale));

  glm::mat4 std_normal = glm::mat4(1.0f);
  std_normal = glm::rotate(std_normal, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));

  a = 0.8*d/2.0;
  b = d/2.0;
  c = 0.8*d/2.0;
  //------------Body:------------------------------
  DrawFluffyBody(a, b, c, modelLoc, normalLoc,
		 std_model, std_normal, deltaTime);
  //------------Feet:------------------------------
  static unsigned int feet_VAO[2];
  if(firsttime){
    feet_VAO[0] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
    feet_VAO[1] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
  }
  normal = std_normal;
  model = std_model;
  normal = glm::rotate(normal, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  model = glm::translate(model, glm::vec3(a*0.35, -b,  c*0.2));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(feet_VAO[0], 0, 180, 40);

  normal = std_normal;
  model = std_model;
  normal = glm::rotate(normal, glm::radians(-foot_angle), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(-foot_angle), glm::vec3(1.0, 0.0, 0.0));
  model = glm::translate(model, glm::vec3(-a*0.35, -b,  c*0.2));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(feet_VAO[1], 0, 180, 40);

  foot_angle += foot_angle_step;
  if(foot_angle > 45.0){
    foot_angle = 45.0;
    foot_angle_step = -foot_angle_step;
  }
  else if(foot_angle < -45.0){
    foot_angle = -45.0;
    foot_angle_step = -foot_angle_step;
  }

  //------------Hands:------------------------------
  static unsigned int hands_VAO[6];
  //------------Left hand:--------------------------
  if(firsttime)
    hands_VAO[0] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.95, 0.0,  c*0.75));
  glUniformMatrix4fv(modelLoc, 1,  GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[0], 40);

  if(firsttime)
    hands_VAO[1] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[1], 40);  
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[2] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3(-a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[2], 40);
  //------------Right hand:--------------------------
  if(firsttime)
    hands_VAO[3] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.95, 0.0,  c*0.75));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[3], 40);

  if(firsttime)
    hands_VAO[4] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[4], 40);  
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[5] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3( a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[5], 40);
  //-------------------------------------------------
  firsttime = false;
}

void DrawFluffyHang(struct GameData *GD,
		     unsigned int shaderProgram,
		     float deltaTime,
		     bool restart){
  float d = 60.0, a, b, c;
  static bool firsttime = true;
  glm::mat4 model = glm::mat4(1.0f);
  unsigned int modelLoc = glGetUniformLocation(shaderProgram,"model");
  glm::mat4 normal = glm::mat4(1.0f);
  unsigned int normalLoc = glGetUniformLocation(shaderProgram,"normal");
  float player_angle = get_player_angle(GD->Player->rotacao);
  static float length_forearm = 0.75;
  static float foot_angle = 25.0;
  
  glm::mat4 std_model = glm::mat4(1.0f);
  std_model = glm::translate(std_model, glm::vec3(GD->Player->pos->x*d, GD->Player->pos->y*d, GD->Player->pos->z*d));
  std_model = glm::translate(std_model, glm::vec3(0.0, (d/2.0)*0.20, 0.0));
  std_model = glm::rotate(std_model, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));

  glm::mat4 std_normal = glm::mat4(1.0f);
  std_normal = glm::rotate(std_normal, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));

  a = 0.8*d/2.0;
  b = d/2.0;
  c = 0.8*d/2.0;
  //------------Body:------------------------------
  DrawFluffyBody(a, b, c, modelLoc, normalLoc,
		 std_model, std_normal, deltaTime);
  //------------Feet:------------------------------
  static unsigned int feet_VAO[2];
  if(firsttime){
    feet_VAO[0] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
    feet_VAO[1] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
  }
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.35, -b,  c*0.2));
  model = glm::rotate(model, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(feet_VAO[0], 0, 180, 40);

  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.35, -b,  c*0.2));
  model = glm::rotate(model, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(feet_VAO[1], 0, 180, 40);
  //------------Hands:------------------------------
  static unsigned int hands_VAO[6];
  //------------Left arm:---------------------------
  static unsigned int larm_VAO;
  if(firsttime)
    larm_VAO = create_curved_cylinder_y(3.0, 3.0,
					0.27*d/2.0, 90.0, 180.0,
					1.0, 1.0, 1.0,
					20, 5);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.95 - 0.27*d/2.0,
					  -c*0.10,
					  c*0.75 - a*0.25));
  model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-50.0f), glm::vec3(1.0, 0.0, 0.0));  
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_y(larm_VAO, 20, 5);
  //------------Left forearm:-----------------------  
  static unsigned int lforearm_VAO;
  if(firsttime)
    lforearm_VAO = create_cylinder_z(3.0, 3.0,
				     0, d/2.0,
				     1.0, 1.0, 1.0,
				     5);
  if(length_forearm > 0.0){
    normal = std_normal;
    model = std_model;
    model = glm::translate(model, glm::vec3(a*0.95,
					    -c*0.10,
					    c*0.75 - a*0.25));
    model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0, 0.0, 0.0));
    normal = glm::rotate(normal, glm::radians(-50.0f), glm::vec3(1.0, 0.0, 0.0));
    model = glm::scale(model, glm::vec3(1.0, 1.0, length_forearm));
    glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
    draw_cylinder_z(lforearm_VAO, 5);
  }
  //------------Left hand:--------------------------
  if(firsttime)
    hands_VAO[0] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.95, b*0.7,  c*0.75 + (d/2.0)*0.3));
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1,  GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[0], 40);

  if(firsttime)
    hands_VAO[1] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[1], 40);  
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[2] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3(-a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[2], 40);
  //------------Right arm:---------------------------
  static unsigned int rarm_VAO;
  if(firsttime)
    rarm_VAO = create_curved_cylinder_y(3.0, 3.0,
					0.27*d/2.0, 180.0, 270.0,
					1.0, 1.0, 1.0,
					20, 5);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.95 + 0.27*d/2.0,
					  -c*0.10,
					  c*0.75 - a*0.25));
  model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-50.0f), glm::vec3(1.0, 0.0, 0.0));  
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_y(rarm_VAO, 20, 5);
  //------------Right forearm:-----------------------
  static unsigned int rforearm_VAO;
  if(firsttime)
    rforearm_VAO = create_cylinder_z(3.0, 3.0,
				     0, d/2.0,
				     1.0, 1.0, 1.0,
				     5);
  if(length_forearm > 0.0){
    normal = std_normal;
    model = std_model;
    model = glm::translate(model, glm::vec3(-a*0.95,
					    -c*0.10,
					    c*0.75 - a*0.25));
    model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0, 0.0, 0.0));
    normal = glm::rotate(normal, glm::radians(-50.0f), glm::vec3(1.0, 0.0, 0.0));
    model = glm::scale(model, glm::vec3(1.0, 1.0, length_forearm));
    glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
    draw_cylinder_z(rforearm_VAO, 5);
  }
  //------------Right hand:--------------------------
  if(firsttime)
    hands_VAO[3] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.95, b*0.7,  c*0.75 + (d/2.0)*0.3));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[3], 40);

  if(firsttime)
    hands_VAO[4] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[4], 40);  
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[5] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3( a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[5], 40);
  //-------------------------------------------------
  firsttime = false;
}

void DrawFluffyHangRight(struct GameData *GD,
			  unsigned int shaderProgram,
			  float deltaTime,
			  bool restart){
  float d = 60.0, a, b, c;
  static bool firsttime = true;
  glm::mat4 model = glm::mat4(1.0f);
  unsigned int modelLoc = glGetUniformLocation(shaderProgram,"model");
  glm::mat4 normal = glm::mat4(1.0f);
  unsigned int normalLoc = glGetUniformLocation(shaderProgram,"normal");
  float player_angle = get_player_angle(GD->Player->rotacao);
  static float length_forearm = 0.75;
  static float foot_angle = 25.0;
  static float arm_angle_left  = 0.0; //20.0;
  static float arm_angle_right = 0.0;
  static float step_arm_angle;
  static int   turn_arm = 0;
  float displacement, L_proj, L_right, gamma_right, L_left, gamma_left;
  step_arm_angle = 150.0*deltaTime;
  
  if(restart){
    arm_angle_left = 0.0;
    arm_angle_right = 0.0;
    turn_arm = 0;
  }

  glm::mat4 tmp_model = glm::mat4(1.0f);
  glm::mat4 std_model = glm::mat4(1.0f);
  std_model = glm::translate(std_model, glm::vec3(GD->Player->pos->x*d, GD->Player->pos->y*d, GD->Player->pos->z*d));
  std_model = glm::translate(std_model, glm::vec3(0.0, (d/2.0)*0.20, 0.0));
  std_model = glm::rotate(std_model, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));

  glm::mat4 std_normal = glm::mat4(1.0f);
  std_normal = glm::rotate(std_normal, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));

  a = 0.8*d/2.0;
  b = d/2.0;
  c = 0.8*d/2.0;
  //------------Body:------------------------------
  DrawFluffyBody(a, b, c, modelLoc, normalLoc,
		 std_model, std_normal, deltaTime);
  //------------Feet:------------------------------
  static unsigned int feet_VAO[2];
  if(firsttime){
    feet_VAO[0] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
    feet_VAO[1] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
  }
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.35, -b,  c*0.2));
  model = glm::rotate(model, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(feet_VAO[0], 0, 180, 40);

  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.35, -b,  c*0.2));
  model = glm::rotate(model, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(feet_VAO[1], 0, 180, 40);
  //------------Hands:------------------------------
  static unsigned int hands_VAO[6];

  if(turn_arm == 0){
    arm_angle_right += step_arm_angle;
    if(arm_angle_right > 20.0){
      arm_angle_right = 20.0;
      turn_arm = 1;
    }
  }
  else if(turn_arm == 1){
    arm_angle_right -= step_arm_angle;
    arm_angle_left  -= step_arm_angle;
    if(arm_angle_right < 0.0 || arm_angle_left < -20.0){
      arm_angle_right = 0.0;
      arm_angle_left  = -20.0;
      turn_arm = 2;
    }
  }
  else{
    arm_angle_left += step_arm_angle;
    if(arm_angle_left > 0.0){
      arm_angle_left = 0.0;
      turn_arm = 0;
    }
  }
  
  L_proj = length_forearm*sinf(PI*50.0/180.0);
  displacement = 4.0*L_proj*(1.0/cosf(PI*arm_angle_right/180.0) - 1.0);
  displacement += L_proj; //+ 3.0/d;
  L_right = sqrtf(SQUARE(length_forearm*cosf(PI*50.0/180.0)) + SQUARE(displacement));
  gamma_right = 180.0*asinf(displacement/L_right)/PI - 50;

  //printf("right = L: %f, gamma: %f, displacement: %f\n",L_right,gamma_right,displacement);

  L_proj = length_forearm*sinf(PI*50.0/180.0);
  displacement = 4.0*L_proj*(1.0/cosf(PI*arm_angle_left/180.0) - 1.0);
  displacement += L_proj; //+ 3.0/d;
  L_left = sqrtf(SQUARE(length_forearm*cosf(PI*50.0/180.0)) + SQUARE(displacement));
  gamma_left = 180.0*asinf(displacement/L_left)/PI - 50;

  //printf("left = L: %f, gamma: %f, displacement: %f\n",L_left,gamma_left,displacement);
  //------------Left arm:---------------------------
  static unsigned int larm_VAO;
  if(firsttime)
    larm_VAO = create_curved_cylinder_y(3.0, 3.0,
					0.27*d/2.0, 90.0, 180.0,
					1.0, 1.0, 1.0,
					20, 5);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.95 - 0.27*d/2.0,
					  -c*0.10,
					  c*0.75 - a*0.25));
  model = glm::rotate(model, glm::radians(arm_angle_left), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(arm_angle_left), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-(50.0f+gamma_left)), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-(50.0f+gamma_left)), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_y(larm_VAO, 20, 5);
  //------------Left forearm:-----------------------  
  static unsigned int lforearm_VAO;
  if(firsttime)
    lforearm_VAO = create_cylinder_z(3.0, 3.0,
				     0, d/2.0,
				     1.0, 1.0, 1.0,
				     5);
  if(length_forearm > 0.0){
    model = glm::rotate(model, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
    normal = glm::rotate(normal, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
    model = glm::translate(model, glm::vec3(0.27*d/2.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
    normal = glm::rotate(normal, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
    tmp_model = model;
    model = glm::scale(model, glm::vec3(1.0, 1.0, L_left));
    glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
    draw_cylinder_z(lforearm_VAO, 5);
    model = tmp_model;
  }
  //------------Left hand:--------------------------
  if(firsttime)
    hands_VAO[0] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  model = glm::translate(model, glm::vec3(0.0, 0.0, (d/2.0)*L_left));
  model = glm::rotate(model, glm::radians(50.0f+gamma_left), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(50.0f+gamma_left), glm::vec3(1.0, 0.0, 0.0));
  model = glm::translate(model, glm::vec3(0.0, c*0.26, a*0.02));

  model = glm::rotate(model, glm::radians(-arm_angle_left), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(-arm_angle_left), glm::vec3(0.0, 0.0, 1.0));  

  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1,  GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[0], 40);

  if(firsttime)
    hands_VAO[1] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[1], 40);  
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[2] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3(-a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[2], 40);
  //------------Right arm:---------------------------
  static unsigned int rarm_VAO;
  if(firsttime)
    rarm_VAO = create_curved_cylinder_y(3.0, 3.0,
					0.27*d/2.0, 180.0, 270.0,
					1.0, 1.0, 1.0,
					20, 5);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.95 + 0.27*d/2.0,
					  -c*0.10,
					  c*0.75 - a*0.25));
  model = glm::rotate(model, glm::radians(arm_angle_right), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(arm_angle_right), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_y(rarm_VAO, 20, 5);
  //------------Right forearm:-----------------------
  static unsigned int rforearm_VAO;
  if(firsttime)
    rforearm_VAO = create_cylinder_z(3.0, 3.0,
				     0, d/2.0,
				     1.0, 1.0, 1.0,
				     5);
  if(length_forearm > 0.0){
    model = glm::rotate(model, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
    normal = glm::rotate(normal, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
    model = glm::translate(model, glm::vec3(-0.27*d/2.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
    normal = glm::rotate(normal, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
    tmp_model = model;
    model = glm::scale(model, glm::vec3(1.0, 1.0, L_right));
    glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
    draw_cylinder_z(rforearm_VAO, 5);
    model = tmp_model;
  }
  //------------Right hand:--------------------------
  if(firsttime)
    hands_VAO[3] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  model = glm::translate(model, glm::vec3(0.0, 0.0, (d/2.0)*L_right));
  model = glm::rotate(model, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
  model = glm::translate(model, glm::vec3(0.0, c*0.26, a*0.02));
  model = glm::rotate(model, glm::radians(-arm_angle_right), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(-arm_angle_right), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[3], 40);

  if(firsttime)
    hands_VAO[4] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[4], 40);  
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[5] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3( a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[5], 40);
  //-------------------------------------------------
  firsttime = false;
}



void DrawFluffyHangLeft(struct GameData *GD,
			 unsigned int shaderProgram,
			 float deltaTime,
			 bool restart){
  float d = 60.0, a, b, c;
  static bool firsttime = true;
  glm::mat4 model = glm::mat4(1.0f);
  unsigned int modelLoc = glGetUniformLocation(shaderProgram,"model");
  glm::mat4 normal = glm::mat4(1.0f);
  unsigned int normalLoc = glGetUniformLocation(shaderProgram,"normal");
  float player_angle = get_player_angle(GD->Player->rotacao);
  static float length_forearm = 0.75;
  static float foot_angle = 25.0;
  static float arm_angle_left  = 0.0; //20.0;
  static float arm_angle_right = 0.0;
  static float step_arm_angle;
  static int   turn_arm = 0;
  float displacement, L_proj, L_right, gamma_right, L_left, gamma_left;
  step_arm_angle = 150.0*deltaTime;
  
  if(restart){
    arm_angle_left = 0.0;
    arm_angle_right = 0.0;
    turn_arm = 0;
  }

  glm::mat4 tmp_model = glm::mat4(1.0f);
  glm::mat4 std_model = glm::mat4(1.0f);
  std_model = glm::translate(std_model, glm::vec3(GD->Player->pos->x*d, GD->Player->pos->y*d, GD->Player->pos->z*d));
  std_model = glm::translate(std_model, glm::vec3(0.0, (d/2.0)*0.20, 0.0));
  std_model = glm::rotate(std_model, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));

  glm::mat4 std_normal = glm::mat4(1.0f);
  std_normal = glm::rotate(std_normal, glm::radians(player_angle), glm::vec3(0.0, 1.0, 0.0));

  a = 0.8*d/2.0;
  b = d/2.0;
  c = 0.8*d/2.0;
  //------------Body:------------------------------
  DrawFluffyBody(a, b, c, modelLoc, normalLoc,
		 std_model, std_normal, deltaTime);
  //------------Feet:------------------------------
  static unsigned int feet_VAO[2];
  if(firsttime){
    feet_VAO[0] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
    feet_VAO[1] = create_ellipsoid_lune(a*0.30, b*0.10, c*0.70,
					0,  180,
					1.0, 0.0, 0.0,
					40);
  }
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.35, -b,  c*0.2));
  model = glm::rotate(model, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(feet_VAO[0], 0, 180, 40);

  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.35, -b,  c*0.2));
  model = glm::rotate(model, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(foot_angle), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid_lune(feet_VAO[1], 0, 180, 40);
  //------------Hands:------------------------------
  static unsigned int hands_VAO[6];

  if(turn_arm == 0){
    arm_angle_left -= step_arm_angle;
    if(arm_angle_left < -20.0){
      arm_angle_left = -20.0;
      turn_arm = 1;
    }
  }
  else if(turn_arm == 1){
    arm_angle_right += step_arm_angle;
    arm_angle_left  += step_arm_angle;
    if(arm_angle_left > 0.0 || arm_angle_right > 20.0){
      arm_angle_left = 0.0;
      arm_angle_right  = 20.0;
      turn_arm = 2;
    }
  }
  else{
    arm_angle_right -= step_arm_angle;
    if(arm_angle_right < 0.0){
      arm_angle_right = 0.0;
      turn_arm = 0;
    }
  }
  
  L_proj = length_forearm*sinf(PI*50.0/180.0);
  displacement = 4.0*L_proj*(1.0/cosf(PI*arm_angle_right/180.0) - 1.0);
  displacement += L_proj; //+ 3.0/d;
  L_right = sqrtf(SQUARE(length_forearm*cosf(PI*50.0/180.0)) + SQUARE(displacement));
  gamma_right = 180.0*asinf(displacement/L_right)/PI - 50;

  //printf("right = L: %f, gamma: %f, displacement: %f\n",L_right,gamma_right,displacement);

  L_proj = length_forearm*sinf(PI*50.0/180.0);
  displacement = 4.0*L_proj*(1.0/cosf(PI*arm_angle_left/180.0) - 1.0);
  displacement += L_proj; //+ 3.0/d;
  L_left = sqrtf(SQUARE(length_forearm*cosf(PI*50.0/180.0)) + SQUARE(displacement));
  gamma_left = 180.0*asinf(displacement/L_left)/PI - 50;

  //printf("left = L: %f, gamma: %f, displacement: %f\n",L_left,gamma_left,displacement);
  //------------Left arm:---------------------------
  static unsigned int larm_VAO;
  if(firsttime)
    larm_VAO = create_curved_cylinder_y(3.0, 3.0,
					0.27*d/2.0, 90.0, 180.0,
					1.0, 1.0, 1.0,
					20, 5);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(a*0.95 - 0.27*d/2.0,
					  -c*0.10,
					  c*0.75 - a*0.25));
  model = glm::rotate(model, glm::radians(arm_angle_left), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(arm_angle_left), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-(50.0f+gamma_left)), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-(50.0f+gamma_left)), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_y(larm_VAO, 20, 5);
  //------------Left forearm:-----------------------  
  static unsigned int lforearm_VAO;
  if(firsttime)
    lforearm_VAO = create_cylinder_z(3.0, 3.0,
				     0, d/2.0,
				     1.0, 1.0, 1.0,
				     5);
  if(length_forearm > 0.0){
    model = glm::rotate(model, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
    normal = glm::rotate(normal, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
    model = glm::translate(model, glm::vec3(0.27*d/2.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
    normal = glm::rotate(normal, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
    tmp_model = model;
    model = glm::scale(model, glm::vec3(1.0, 1.0, L_left));
    glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
    draw_cylinder_z(lforearm_VAO, 5);
    model = tmp_model;
  }
  //------------Left hand:--------------------------
  if(firsttime)
    hands_VAO[0] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  model = glm::translate(model, glm::vec3(0.0, 0.0, (d/2.0)*L_left));
  model = glm::rotate(model, glm::radians(50.0f+gamma_left), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(50.0f+gamma_left), glm::vec3(1.0, 0.0, 0.0));
  model = glm::translate(model, glm::vec3(0.0, c*0.26, a*0.02));

  model = glm::rotate(model, glm::radians(-arm_angle_left), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(-arm_angle_left), glm::vec3(0.0, 0.0, 1.0));  

  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1,  GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[0], 40);

  if(firsttime)
    hands_VAO[1] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[1], 40);  
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[2] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3(-a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[2], 40);
  //------------Right arm:---------------------------
  static unsigned int rarm_VAO;
  if(firsttime)
    rarm_VAO = create_curved_cylinder_y(3.0, 3.0,
					0.27*d/2.0, 180.0, 270.0,
					1.0, 1.0, 1.0,
					20, 5);
  normal = std_normal;
  model = std_model;
  model = glm::translate(model, glm::vec3(-a*0.95 + 0.27*d/2.0,
					  -c*0.10,
					  c*0.75 - a*0.25));
  model = glm::rotate(model, glm::radians(arm_angle_right), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(arm_angle_right), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
  glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_curved_cylinder_y(rarm_VAO, 20, 5);
  //------------Right forearm:-----------------------
  static unsigned int rforearm_VAO;
  if(firsttime)
    rforearm_VAO = create_cylinder_z(3.0, 3.0,
				     0, d/2.0,
				     1.0, 1.0, 1.0,
				     5);
  if(length_forearm > 0.0){
    model = glm::rotate(model, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
    normal = glm::rotate(normal, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
    model = glm::translate(model, glm::vec3(-0.27*d/2.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
    normal = glm::rotate(normal, glm::radians(-(50.0f+gamma_right)), glm::vec3(1.0, 0.0, 0.0));
    tmp_model = model;
    model = glm::scale(model, glm::vec3(1.0, 1.0, L_right));
    glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
    draw_cylinder_z(rforearm_VAO, 5);
    model = tmp_model;
  }
  //------------Right hand:--------------------------
  if(firsttime)
    hands_VAO[3] = create_ellipsoid(a*0.15, b*0.30, c*0.40,
				    1.0, 0.0, 0.0,
				    40);
  model = glm::translate(model, glm::vec3(0.0, 0.0, (d/2.0)*L_right));
  model = glm::rotate(model, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(50.0f+gamma_right), glm::vec3(1.0, 0.0, 0.0));
  model = glm::translate(model, glm::vec3(0.0, c*0.26, a*0.02));
  model = glm::rotate(model, glm::radians(-arm_angle_right), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(-arm_angle_right), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  normal = glm::rotate(normal, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
  normal = glm::rotate(normal, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[3], 40);

  if(firsttime)
    hands_VAO[4] = create_ellipsoid(a*0.18, b*0.26, c*0.20,
				    1.0, 0.0, 0.0,
				    40);    
  model = glm::translate(model, glm::vec3(a*0.06, 0.0,  c*0.21));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  draw_ellipsoid(hands_VAO[4], 40);  
  model = glm::translate(model, glm::vec3(-a*0.06, 0.0,  -c*0.21));  

  if(firsttime)
    hands_VAO[5] = create_ellipsoid(a*0.10, b*0.10, c*0.30,
				    1.0, 0.0, 0.0,
				    40);
  normal = glm::rotate(normal, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  normal = glm::rotate(normal, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  model = glm::translate(model, glm::vec3( a*0.045, b*0.2, -c*0.1));
  model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
  model = glm::rotate(model, glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));
  draw_ellipsoid(hands_VAO[5], 40);
  //-------------------------------------------------
  firsttime = false;
}

void SetInitialView(float theta_y, float zoom, float rotacao)
{
  glLoadIdentity();
  glTranslatef(0.0, 0.0, zoom * 1.0);
  glRotatef(rotacao * 1.0, 1.0, 0.0, 0.0);
  glRotatef(theta_y, 0.0, 1.0, 0.0);
}