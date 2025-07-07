/** Negation of binary image img_input. Result is stored in img_output.

  */

#include <visiongl/constants.hpp>

__kernel void vglClNdBinNot(__global VGL_PACK_CL_SHADER_TYPE* img_input,
                            __global VGL_PACK_CL_SHADER_TYPE* img_output)
{
  int coord = get_global_linear_id();

  VGL_PACK_CL_SHADER_TYPE p = img_input[coord];
  img_output[coord] = VGL_PACK_MAX_UINT & ~p;
}
