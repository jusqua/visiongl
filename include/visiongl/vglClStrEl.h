#ifndef __VGLCLSTREL_H__
#define __VGLCLSTREL_H__

#include <visiongl/vglConst.h>

// OpenCL compiler definition
#if defined(__OPENCL_VERSION__) || defined(__OPENCL_C_VERSION__) || defined(__OPENCL_CPP_VERSION__)
typedef struct VglClStrEl {
    float data[VGL_ARR_CLSTREL_SIZE];
    int ndim;
    int shape[VGL_ARR_SHAPE_SIZE];
    int offset[VGL_ARR_SHAPE_SIZE];
    int size;
} VglClStrEl;
// CXX compiler definition
#elif defined(__OPENCL__)
#include <CL/cl.h>
typedef struct VglClStrEl {
    cl_float data[VGL_ARR_CLSTREL_SIZE];
    cl_int ndim;
    cl_int shape[VGL_ARR_SHAPE_SIZE];
    cl_int offset[VGL_ARR_SHAPE_SIZE];
    cl_int size;
} VglClStrEl;
#endif

#endif
