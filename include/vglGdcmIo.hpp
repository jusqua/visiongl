#ifndef VISIONGL_VGLGDCMIO_HPP
#define VISIONGL_VGLGDCMIO_HPP

#ifdef __GDCM__

#ifdef __cplusplus
#if __has_include(<gdcmImageReader.h>)
#define GDCM_HEADER(x) <x>
#else
#define GDCM_HEADER(x) <gdcm/x>
#endif
#endif  // __cplusplus

#include <visiongl/vglImage.hpp>
#include GDCM_HEADER(gdcmImageReader.h)
#include GDCM_HEADER(gdcmImage.h)
#include GDCM_HEADER(gdcmWriter.h)
#include GDCM_HEADER(gdcmAttribute.h)
#include GDCM_HEADER(gdcmImageWriter.h)
#include GDCM_HEADER(gdcmImageChangeTransferSyntax.h)
#include GDCM_HEADER(gdcmImageChangePhotometricInterpretation.h)
#include GDCM_HEADER(gdcmPhotometricInterpretation.h)

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
