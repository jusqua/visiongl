#ifndef VISIONGL_VGLDECONV_HPP
#define VISIONGL_VGLDECONV_HPP

#include <visiongl/vglImage.hpp>

void vglSaveColorDeconv(char *outFilename, VglImage *imagevgl, double *mInitial, int find3rdColor = 0);
VglImage *vglColorDeconv(VglImage *imagevgl, double *mInitial, int find3rdColor = 0);

#endif  // VISIONGL_VGLDECONV_HPP
