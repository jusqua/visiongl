#ifndef VGL_GDCM_IO_HPP
#define VGL_GDCM_IO_HPP

#ifdef VGL_USE_GDCM

#include <vglImage.h>

VglImage* vglGdcmLoadDicom(char* inFilename);
VglImage*  vglGdcmLoad4dDicom(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
int vglGdcmSaveDicom(char* outFilename, VglImage* imagevgl, int compress);
int vglGdcmSaveDicomUncompressed(char* outFilename, VglImage* imagevgl);
int vglGdcmSaveDicomCompressed(char* outFilename, VglImage* imagevgl);
int vglGdcmSave4dDicom(char* filename, VglImage* image, int lStart, int lEnd, int compress = 0);
int convertDepthGdcmToVgl(int dcmDepth);
int convertDepthVglToGdcm(int vglDepth);

#endif // VGL_USE_GDCM

#endif // VGL_GDCM_IO_HPP


