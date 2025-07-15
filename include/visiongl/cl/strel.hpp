#ifndef VGL_CL_STREL_HPP
#define VGL_CL_STREL_HPP

#include <visiongl/constants.hpp>

// OpenCL
#ifdef __OPENCL_VERSION__
typedef struct VglClStrEl {
  float data[VGL_ARR_CLSTREL_SIZE];
  int ndim;
  int shape[VGL_ARR_SHAPE_SIZE];
  int offset[VGL_ARR_SHAPE_SIZE];
  int size;
} VglClStrEl;
// API
#else
#include <CL/cl.h>
typedef struct VglClStrEl {
  cl_float data[VGL_ARR_CLSTREL_SIZE];
  cl_int ndim;
  cl_int shape[VGL_ARR_SHAPE_SIZE];
  cl_int offset[VGL_ARR_SHAPE_SIZE];
  cl_int size;
} VglClStrEl;
#endif

#endif // VGL_CL_STREL_HPP
