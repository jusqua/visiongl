#ifndef VGL_TIFF_IO_HPP
#define VGL_TIFF_IO_HPP

#include <visiongl/image.hpp>
#include <visiongl/common.hpp>

VglImage* vglLoadTiff(char* inFilename);
IplImage* iplLoadTiff(char* inFilename);
VglImage* vglLoadTiffAlt(char* inFilename);
VglImage* vglLoad4dTiff(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
int vglSaveTiff(char* outFilename, VglImage* image);
int iplSaveTiff(char* outFilename, IplImage* image);
int vglSave4dTiff(char* filename, VglImage* image, int lStart, int lEnd);

int vglPrintTiffInfo(char* inFilename, char* msg = NULL);

#endif // VGL_TIFF_IO_HPP
