/** Erosion of src image by mask. Result is stored in dst image.

  */

// ARRAY convolution_window [window_size_x*window_size_y*window_size_z]
// SCALAR window_size_x
// SCALAR window_size_y
// SCALAR window_size_z

__kernel void vglCl3dFuzzyDrasticErode(__read_only image3d_t img_input,
                                       __write_only image3d_t img_output,
                                       __constant float* convolution_window,
                                       int window_size_x,
                                       int window_size_y,
                                       int window_size_z) {
    int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |  // Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |    // Clamp to next edge
                          CLK_FILTER_NEAREST;            // Don't interpolate

    int factorx = floor((float)window_size_x / 2.0f);
    int factory = floor((float)window_size_y / 2.0f);
    int factorz = floor((float)window_size_z / 2.0f);
    int conv_controller = 0;
    float4 pmin = (1.0, 1.0, 1.0, 1.0);
    for (int w = -factorz; w <= factorz; w++) {
        for (int j = -factory; j <= factory; j++) {
            for (int i = -factorx; i <= factorx; i++) {
                float4 a = read_imagef(img_input, smp, (int4)(coords.x + i, coords.y + j, coords.z + w, 0));
                float b = 1 - convolution_window[conv_controller];  // complement of mask
                float4 S;
                if (b == 0)
                    S = a;
                else {
                    if (a.x == 0)
                        S.x = b;
                    else
                        S.x = 1;

                    if (a.y == 0)
                        S.y = b;
                    else
                        S.y = 1;

                    if (a.z == 0)
                        S.z = b;
                    else
                        S.z = 1;

                    if (a.w == 0)
                        S.w = b;
                    else
                        S.w = 1;
                }
                pmin = min(pmin, S);
                conv_controller++;
            }
        }
    }
    write_imagef(img_output, coords, pmin);
}
