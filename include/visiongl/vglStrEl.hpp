#ifndef VISIONGL_VGLSTREL_HPP
#define VISIONGL_VGLSTREL_HPP

#include <visiongl/vglImage.hpp>
#include <visiongl/vglShape.hpp>
#include <visiongl/vglClStrEl.hpp>

#ifdef __OPENCL__
#include <CL/cl.h>
#endif

#define VGL_STREL_CUBE 1
#define VGL_STREL_CROSS 2
#define VGL_STREL_GAUSS 3
#define VGL_STREL_MEAN 4

class VglStrEl {
public:
    float* data;
    VglShape* vglShape;

    VglStrEl(float* data, VglShape* shape);
    VglStrEl(int type, int ndim);
    ~VglStrEl();
    void VglCreateStrEl(float* data, VglShape* shape);
    void print(char* msg = NULL);

    float* getData();

    int getSize();
    int getNpixels();
    int getNdim();
    int* getShape();
    int* getOffset();

#ifdef __OPENCL__
    VglClStrEl* asVglClStrEl();
#endif
};

#endif  // VISIONGL_VGLSTREL_HPP
