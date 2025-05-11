#ifndef VGL_DECONV_HPP
#define VGL_DECONV_HPP

#include <vglImage.h>

void vglSaveColorDeconv(char *outFilename, VglImage *imagevgl, double *mInitial, int find3rdColor = 0);
VglImage* vglColorDeconv(VglImage *imagevgl, double *mInitial, int find3rdColor = 0);

#endif // VGL_DECONV_HPP
