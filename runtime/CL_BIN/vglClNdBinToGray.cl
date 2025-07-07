/** Convert binary image to grayscale.

    Convert binary image to grayscale.
  */

// SHAPE in_shape  (img_input->vglShape->asVglClShape())
// SHAPE out_shape (img_output->vglShape->asVglClShape())

#include <visiongl/cl/shape.hpp>

__kernel void vglClNdBinToGray(__global VGL_PACK_CL_SHADER_TYPE* img_input,
                               __global char* img_output,
                               __constant VglClShape* in_shape,
                               __constant VglClShape* out_shape)
{
  int coord = get_global_linear_id();

  VGL_PACK_OUTPUT_SWAP_MASK

  unsigned char result = 0;
  VGL_PACK_CL_SHADER_TYPE p = img_input[coord];
  for (int bit = 0; bit < VGL_PACK_SIZE_BITS; bit++)
  {
    VGL_PACK_CL_SHADER_TYPE result_bit = p & ( (VGL_PACK_CL_SHADER_TYPE) 1 << (VGL_PACK_CL_SHADER_TYPE) bit ); //outputSwapMask[bit];
    if (result_bit)
      img_output[coord * VGL_PACK_SIZE_BITS + bit] = 255;
    else
      img_output[coord * VGL_PACK_SIZE_BITS + bit] = 0;
  }
}
