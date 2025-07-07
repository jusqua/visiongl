/** Invert N-dimensional image.

  */

__kernel void vglClNdNot(__global char* img_input, __global char* img_output)
{
  int coord = get_global_linear_id();
  img_output[coord] = 255 - img_input[coord];
}
