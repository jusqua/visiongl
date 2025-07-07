#ifndef VGL_DCMTK_IO_HPP
#define VGL_DCMTK_IO_HPP

#include <visiongl/image.hpp>
#include "dcmtk/dcmdata/dctk.h"

VglImage* vglDcmtkLoadDicom(char* inFilename);
VglImage*  vglDcmtkLoad4dDicom(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
int vglDcmtkSaveDicom(char* outFilename, VglImage* imagevgl, int compress);
int vglDcmtkSaveDicomUncompressed(char* outFilename, VglImage* imagevgl);
int vglDcmtkSaveDicomCompressed(char* outFilename, VglImage* imagevgl);
int vglDcmtkSave4dDicom(char* filename, VglImage* image, int lStart, int lEnd, int compress = 0);
int convertDepthDcmtkToVgl(int dcmDepth);
int convertDepthVglToDcmtk(int vglDepth);

#endif // VGL_DCMTK_IO_HPP
