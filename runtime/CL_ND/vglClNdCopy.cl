/** Copy N-dimensional image.

  */

__kernel void vglClNdCopy(__global char* img_input, __global char* img_output)
{
  int coord = get_global_linear_id();
  img_output[coord] = img_input[coord];
}
