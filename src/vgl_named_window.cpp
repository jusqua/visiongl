#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "vglContext.h"
#include "vgl_named_window.h"

/** \brief Refresh all output images.
 */
void VglNamedWindowList::RefreshAll(int split){
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  Reset();
  int curr_id = Cycle();
  if (split < VGL_MIN_WINDOW_SPLIT || split > VGL_MAX_WINDOW_SPLIT){
    printf("VglNamedWindowList::RefreshAll(): Error: split = %d not between %d and %d. Using default, %d",
	     split, VGL_MIN_WINDOW_SPLIT, VGL_MAX_WINDOW_SPLIT, VGL_DEFAULT_WINDOW_SPLIT);
    split = VGL_DEFAULT_WINDOW_SPLIT;
  }
  while (curr_id >= 0){
    Refresh(curr_id, split);
    curr_id = Cycle();
  }
  glutSwapBuffers();
}

/** \brief Refresh single output image.
 */
void VglNamedWindowList::Refresh(int win_index, int split){
  float x0, x1, y0, y1;
  int win_x, win_y;
  //printf("VglNamedWindowList::Refresh(%d)\n", win_index);

  vglCheckContext(WindowList[win_index].image, VGL_GL_CONTEXT);

  if (split < VGL_MIN_WINDOW_SPLIT || split > VGL_MAX_WINDOW_SPLIT){
    printf("VglNamedWindowList::Refresh(): Error: split = %d not between %d and %d. Using default, %d",
	     split, VGL_MIN_WINDOW_SPLIT, VGL_MAX_WINDOW_SPLIT, VGL_DEFAULT_WINDOW_SPLIT);
    split = VGL_DEFAULT_WINDOW_SPLIT;
  }

  if (!FreePosition[win_index]){
    glutSetWindow(main_window_id);
    if (!WindowList[win_index].image){
      printf("VglNamedWindowList::Refresh(%d): Warning: image not set\n", win_index);
      return;
    }

    win_x = win_index % split;
    win_y = win_index / split;

    x0 = VGL_WIN_X0 + ((float)win_x      ) * VGL_WIN_DX / (float)split;
    x1 = VGL_WIN_X0 + ((float)win_x + 1.0) * VGL_WIN_DX / (float)split;
    y0 = VGL_WIN_Y1 - ((float)win_y      ) * VGL_WIN_DY / (float)split;
    y1 = VGL_WIN_Y1 - ((float)win_y + 1.0) * VGL_WIN_DY / (float)split;

    //glColor3f((float)win_index/2.0, 1.0 - (float)win_index/2.0, 0.0);
    glBindTexture(GL_TEXTURE_2D, WindowList[win_index].image->tex);
    //printf("VglNamedWindowList::Refresh(%d) Binding texture %d\n", win_index, WindowList[win_index].image->tex);

    glBegin(GL_QUADS);
        glTexCoord2f( 0.0,  0.0);
      //glVertex3f ( -1.0, -1.0, 0.0);
        glVertex3f (   x0,   y0, 0.0);

        glTexCoord2f( 1.0,  0.0);
      //glVertex3f (  1.0, -1.0, 0.0);
        glVertex3f (   x1,   y0, 0.0);

        glTexCoord2f( 1.0,  1.0);
      //glVertex3f (  1.0,  1.0, 0.0);
        glVertex3f (   x1,   y1, 0.0);

        glTexCoord2f( 0.0,  1.0);
      //glVertex3f ( -1.0,  1.0, 0.0);
        glVertex3f (   x0,   y1, 0.0);
    glEnd();
    //glutSwapBuffers();
  }
}

void VglNamedWindowList::Reset(void){
  for (int i = 0; i < VGL_MAX_WINDOWS; i++){
    if (!FreePosition[i]){
      current_window = -1;
    }
  }
  //printf("VglNamedWindowList::Reset: current_window = %d\n", current_window);
}

int VglNamedWindowList::Cycle(void){
  int i;
  for (i = current_window + 1; i < VGL_MAX_WINDOWS; i++){
    if (!FreePosition[i]){
      current_window = i;
      break;
    }
  }
  if (i == VGL_MAX_WINDOWS){
    current_window = -1;
  }
  //printf("VglNamedWindowList::Cycle: current_window = %d\n", current_window);
  return current_window;
}

VglNamedWindowList::VglNamedWindowList(void){
  for (int i = 0; i < VGL_MAX_WINDOWS; i++){
    FreePosition[i] = 1;
  }
}

VglNamedWindowList::~VglNamedWindowList(void){

}

int VglNamedWindowList::MaxWindows(void){
  return VGL_MAX_WINDOWS;
}

int VglNamedWindowList::CreateNamedSubwindow(char* name){
  for (int i = 0; i < VGL_MAX_WINDOWS; i++){
    if (FreePosition[i]){
      WindowList[i].name = name;
      FreePosition[i] = 0;
      return i;
    }
  }
  return -1;
}

int VglNamedWindowList::ShowImage(char* name, VglImage* image){
  int win_id = WindowIdByName(name);
  if (win_id >= 0){
    WindowList[win_id].image = image;
    printf("VglNamedWindowList::ShowImage: win_id = %d image = %p\n", win_id, image);
  }
  else{
    printf("VglNamedWindowList::ShowImage: window %s not found\n", name);
  }
  return win_id;
}

int VglNamedWindowList::WindowIdByName(char* name){
  for (int i = 0; i < VGL_MAX_WINDOWS; i++){
    if (!FreePosition[i]){
      int retval = strcmp(name, WindowList[i].name);
      if (retval == 0){
        return i;
	}
    }
  }
  return -1;
}

/*
void VglNamedWindowList::vglShowImage(char* winname, VglImage* image)
{
  window_list->ShowImage(winname, image);
}
*/

int VglNamedWindowList::NamedWindow(char* winname){
  return CreateNamedSubwindow(winname);
}
