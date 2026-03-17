#ifndef VGL_IO_H
#define VGL_IO_H
#pragma once

#include "vgl_image.h"

void vglSaveImage(char* filename, VglImage* image);
void vglSaveIplImage(char* filename, IplImage* image, int* params = 0);
void vglSave3dImage(char* filename, VglImage* image, int lStart, int lEnd = -1);
void vglSaveNdImage(char* filename, VglImage* image, int lStart, int lEnd = -1);
VglImage* vglLoadImage(char* filename, int iscolor = -1, int has_mipmap = 0); // -1 = CV_LOAD_IMAGE_UNCHANGED
VglImage* vglLoad3dImage(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
VglImage* vglLoadNdImage(char* filename, int lStart, int lEnd, int* shape, int ndim, bool has_mipmap = 0);
// int SavePPM(char* filename, int w, int h, void* savebuf); // NOTE: Seems unused
// int vglSavePPM(char* filename, VglImage* img); // NOTE: Seems unused
int vglSavePgm(char* filename, VglImage* img);
VglImage* vglLoadPgm(char* filename);

#endif // VGL_IO_H
