/** Copy N-dimensional image word by word.

  */

#include <visiongl/constants.hpp>

__kernel void vglClNdBinCopy(__global VGL_PACK_CL_SHADER_TYPE* img_input, __global VGL_PACK_CL_SHADER_TYPE* img_output)
{
  int coord = get_global_linear_id();

  img_output[coord] = img_input[coord];

}
