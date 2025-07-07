/** Maximum or union between two images.

    Maximum or union between img_input1 and img_input2. Result saved in img_output.
  */

#include <visiongl/constants.hpp>

__kernel void vglClNdBinMax(__global VGL_PACK_CL_SHADER_TYPE* img_input1,
                            __global VGL_PACK_CL_SHADER_TYPE* img_input2,
                            __global VGL_PACK_CL_SHADER_TYPE* img_output)
{
  int coord = get_global_linear_id();

  VGL_PACK_CL_SHADER_TYPE p1 =  img_input1[coord];
  VGL_PACK_CL_SHADER_TYPE p2 =  img_input2[coord];
  VGL_PACK_CL_SHADER_TYPE result = p1 | p2;
  img_output[coord] = result;
}
