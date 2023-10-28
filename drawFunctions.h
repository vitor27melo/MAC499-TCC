// drawFunctions.h

#ifndef DRAWFUNCTIONS_H
#define DRAWFUNCTIONS_H

#include "shader.h"
#include <GL/gl.h>
#include "Entidades.h"

float magnitude(float A[3]);

void setupBlockVAO(float d);

// void setupHairVAO(float R, float G, float B);

void drawCube(GLuint tex[], struct GameData *GD, int id1, int id2);



unsigned int curvedXCylinderVAO(float radius0,
			    float radius1,
			    float bend_radius,
			    float bend_ang0,
			    float bend_ang1,
			    float R, float G, float B,
				int slices,
				int dtheta);

void draw_curved_cylinder_x(float radius,
			    float bend_radius,
			    float bend_ang,
			    float length,
			    int slices,
			    bool cw);

void draw_curved_cylinder_y(unsigned int VAO,
			    int slices,
			    int dtheta);

unsigned int create_curved_cylinder_y(float radius0,
				      float radius1,
				      float bend_radius,
				      float bend_ang0,
				      float bend_ang1,
				      float R, float G, float B,
				      int slices,
				      int dtheta);

unsigned int create_cylinder_z(float radius0,
			       float radius1,
			       float z0,
			       float z1,
			       float R, float G, float B,
			       int dtheta);

void draw_cylinder_z(unsigned int VAO,
				int dtheta);

void draw_ellipsoid(float a,
		    float b,
		    float c,
		    int slices);



void draw_ellipsoid_lune(float a,
			 float b,
			 float c,
			 int angle1,
			 int angle2,
			 int slices);



void draw_hairy_ellipsoid(float a,
			  float b,
			  float c,
			  int slices,
			Shader simpleShader,
          unsigned int curvedHairVAO);


void draw_hairy_ellipsoid(float a,
			  float b,
			  float c,
			  int slices);



void DrawFluffy(unsigned int shaderID,
		     float deltaTime,
		     bool restart,
			 posicao *pos,
			 velocidade *rotacao,
			 float scale);

void DrawFluffyPush(struct GameData *GD,
		     unsigned int shaderID,
		     float deltaTime,
		     bool restart);

void DrawFluffyPull(struct GameData *GD,
		     unsigned int shaderID,
		     float deltaTime,
		     bool restart);

void DrawFluffyWalk(
		     unsigned int shaderID,
		     float deltaTime,
		     bool restart,
			 posicao *pos,
			 velocidade *rotacao,
			 float scale);

void DrawFluffyHang(struct GameData *GD,
		     unsigned int shaderID,
		     float deltaTime,
		     bool restart);

void DrawFluffyHangRight(struct GameData *GD,
			  unsigned int shaderProgram,
			  float deltaTime,
			  bool restart);

void DrawFluffyHangLeft(struct GameData *GD,
			 unsigned int shaderProgram,
			 float deltaTime,
			 bool restart);

#endif