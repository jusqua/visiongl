/** Grayscale of src image by mask. Result is stored in dst image.

  */

__kernel void vglClGrayscale(__read_only image2d_t img_input, __write_only image2d_t img_output)
{
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	
	float4 p = read_imagef(img_input, smp, coords);
    float gray = dot(p.xyz, (float3)(0.299f, 0.587f, 0.114f));
	write_imagef(img_output, coords, (float4)(gray, gray, gray, p.w));
}
