#ifndef VISIONGL_VGLCONTEXT_HPP
#define VISIONGL_VGLCONTEXT_HPP

#include <visiongl/vglImage.hpp>

#define VGL_BLANK_CONTEXT 0
#define VGL_RAM_CONTEXT 1
#define VGL_GL_CONTEXT 2
#define VGL_CUDA_CONTEXT 4
#define VGL_CL_CONTEXT 8

#define vglIsContextValid(x) ((x >= 1) && (x <= 15))
#define vglIsContextUnique(x) ((x == 0) || (x == 1) || (x == 2) || (x == 4) || (x == 8))
#define vglIsInContext(img, x) ((img)->inContext & (x) || ((img)->inContext == 0 && x == 0))

int vglAddContext(VglImage* img, int context);
int vglSetContext(VglImage* img, int context);
int vglCheckContext(VglImage* img, int context);
int vglCheckContextForOutput(VglImage* img, int context);
void vglPrintContext(int context, char* msg = NULL);
void vglPrintContext(VglImage* img, char* msg = NULL);

#endif  // VISIONGL_VGLCONTEXT_HPP
