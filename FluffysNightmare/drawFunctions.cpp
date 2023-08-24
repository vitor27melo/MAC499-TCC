#include <glad/gl.h>

void draw_menu(SDL_Window *Window,
		 struct Tela *tela, SDL_Surface *arrow){
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  // Clear the color and depth buffers.
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // We don't want to modify the projection matrix.
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  
  if(tela->selecionaOpcao1){
    arrowPosition.x=tela->x1;
    arrowPosition.y=tela->y1;
  }
  else {
    arrowPosition.x=tela->x2;
    arrowPosition.y=tela->y2;
  }

  float fovy = 60.0*(3.14159/180.0);
  float height = 600;
  float focallength = (height/2.0)/tan(fovy/2.0);
  glRasterPos3f(-tela->background->w/2,
		-tela->background->h/2,
		-(focallength+0.1));
  glDrawPixels(tela->background->w,
	       tela->background->h,
	       GL_RGBA, GL_UNSIGNED_BYTE,
	       tela->background->pixels);

  if (tela->showArrow){
    glRasterPos3f(-tela->background->w/2 + arrowPosition.x,
      tela->background->h/2 - arrowPosition.y - arrow->h,
      -(focallength+0.001));
    glDrawPixels(arrow->w,
          arrow->h,
          GL_RGBA, GL_UNSIGNED_BYTE,
          arrow->pixels);
  }
  //Apply the arrow
  
  SDL_GL_SwapWindow(Window);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
}