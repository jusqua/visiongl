#ifndef VGL_CUDA_IMAGE_HPP
#define VGL_CUDA_IMAGE_HPP

#include <visiongl/image.hpp>

int vglCudaToGl(VglImage* img);
int vglGlToCuda(VglImage* img);
int vglCudaAlloc(VglImage* img);
int vglCudaFree(VglImage* img);

void vglCudaInvertOnPlace(VglImage* img);
int vglCudaAllocPbo(VglImage* img);
int vglCudaFreePbo(VglImage* img);
int vglCudaMapPbo(VglImage* img);
int vglCudaUnmapPbo(VglImage* img);
void vglCudaCopy(VglImage* img, VglImage* dst);
void vglCudaCopy2(VglImage* img, VglImage* dst);
void vglCudaInvert(VglImage* img, VglImage* dst);

#endif // VGL_CUDAIMAGE_HPP
