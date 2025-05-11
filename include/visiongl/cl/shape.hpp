#ifndef VGL_CL_SHAPE_HPP
#define VGL_CL_SHAPE_HPP

#include <visiongl/constants.hpp>

// OpenCL
#ifdef __OPENCL_VERSION__
typedef struct VglClShape{ 
  int ndim;
  int shape[VGL_ARR_SHAPE_SIZE];
  int offset[VGL_ARR_SHAPE_SIZE];
  int size;
} VglClShape;
#endif

// API
#ifdef VGL_USE_OPENCL
typedef struct VglClShape{ 
  cl_int ndim;
  cl_int shape[VGL_ARR_SHAPE_SIZE];
  cl_int offset[VGL_ARR_SHAPE_SIZE];
  cl_int size;
} VglClShape;
#endif

#endif // VGL_CL_SHAPE_HPP
