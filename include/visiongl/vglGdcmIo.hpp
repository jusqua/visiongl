#ifndef VISIONGL_VGLGDCMIO_HPP
#define VISIONGL_VGLGDCMIO_HPP

#ifdef __GDCM__

#include <visiongl/vglImage.hpp>

VglImage* vglGdcmLoadDicom(char* inFilename);
VglImage* vglGdcmLoad4dDicom(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
int vglGdcmSaveDicom(char* outFilename, VglImage* imagevgl, int compress);
int vglGdcmSaveDicomUncompressed(char* outFilename, VglImage* imagevgl);
int vglGdcmSaveDicomCompressed(char* outFilename, VglImage* imagevgl);
int vglGdcmSave4dDicom(char* filename, VglImage* image, int lStart, int lEnd, int compress = 0);
int convertDepthGdcmToVgl(int dcmDepth);
int convertDepthVglToGdcm(int vglDepth);

#endif  // __GDCM__

#endif  // VISIONGL_VGLGDCMIO_HPP
