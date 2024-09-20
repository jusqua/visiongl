#ifndef __VGLCLSHAPE_H__
#define __VGLCLSHAPE_H__

#include <visiongl/vglConst.h>

// OpenCL compiler definition
#if defined(__OPENCL_VERSION__) || defined(__OPENCL_C_VERSION__) || defined(__OPENCL_CPP_VERSION__)
typedef struct VglClShape {
    int ndim;
    int shape[VGL_ARR_SHAPE_SIZE];
    int offset[VGL_ARR_SHAPE_SIZE];
    int size;
} VglClShape;
// CXX compiler definition
#elif defined(__OPENCL__)
#include <CL/cl.h>
typedef struct VglClShape {
    cl_int ndim;
    cl_int shape[VGL_ARR_SHAPE_SIZE];
    cl_int offset[VGL_ARR_SHAPE_SIZE];
    cl_int size;
} VglClShape;
#endif

#endif
