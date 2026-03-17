#ifndef VGL_CUSTOM_OPENGL_KERNELS_H
#define VGL_CUSTOM_OPENGL_KERNELS_H
#pragma once

#include "vgl_image.h"

void vglCopyImageTex(VglImage* src, VglImage* dst);
void vglCopyImageTexVFS(VglImage* src, VglImage* dst);
void vglCopyImageTexFS(VglImage* src, VglImage* dst);
// void vglAxis(void); // NOTE: Seems unused
// void vglGreen(VglImage* src, VglImage* dst); // NOTE: Seems unused
void vglVerticalFlip2(VglImage* src, VglImage* dst);
void vglHorizontalFlip2(VglImage* src, VglImage* dst);
void vglClear(VglImage* image, float r, float g, float b, float a = 0.0);
void vglOpenSq3 (VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglCloseSq3(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglErodeSq3Sep(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglErodeSq5Sep(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglCErodeCross3(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buf, int times);
void vglDistTransformCross3(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglDistTransformSq3(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglGetLevelDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglThinBernard(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglThinChin(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglBaricenterVga(VglImage* src, double* x_avg = NULL, double* y_avg = NULL, double* pix_count = NULL);
// void vglGray2(VglImage*  src, VglImage*  dst, VglImage*  dst1); // NOTE: Does not exist
void vglInOut_model(VglImage* dst, VglImage* dst1);
void vglMultiOutput_model(VglImage* src, VglImage* dst, VglImage* dst1);
void vglMultiInput_model(VglImage* src0, VglImage* src1, VglImage* dst);

#endif // VGL_CUSTOM_OPENGL_KERNELS_H
