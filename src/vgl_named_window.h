#ifndef VGL_NAMED_WINDOW_H
#define VGL_NAMED_WINDOW_H
#pragma once

#include "vgl_image.h"

class VglNamedWindow  {
public:
  VglImage* image;
  char* name;
};

class VglNamedWindowList  {

public:
  VglNamedWindowList(void);
  ~VglNamedWindowList(void);
  //void Init();
  int MaxWindows();
  int CreateNamedSubwindow(char* winname);
  int CreateSubwindow(void);
  int ShowImage(char* name, VglImage* image);
  void Reset(void);
  int Cycle(void);
  void Refresh(int win_index, int split = VGL_DEFAULT_WINDOW_SPLIT);
  void RefreshAll(int split = VGL_DEFAULT_WINDOW_SPLIT);
  int main_window_id;
  int NamedWindow(char* winname);

private:
  VglNamedWindow WindowList[VGL_MAX_WINDOWS];
  int FreePosition[VGL_MAX_WINDOWS];
  int WindowIdByName(char* name);
  int current_window;
};

#endif // VGL_NAMED_WINDOW_H
