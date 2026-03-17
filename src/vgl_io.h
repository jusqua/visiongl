#ifndef VGL_IO_H
#define VGL_IO_H
#pragma once

#include "vgl_image.h"

void vglSaveImage(char* filename, VglImage* image);
void vglSaveIplImage(char* filename, IplImage* image, int* params = 0);
void vglSave3dImage(char* filename, VglImage* image, int lStart, int lEnd = -1);
void vglSaveNdImage(char* filename, VglImage* image, int lStart, int lEnd = -1);
VglImage* vglLoadImage(char* filename, int iscolor = -1, int has_mipmap = 0); // -1 = CV_LOAD_IMAGE_UNCHANGED
VglImage* vglLoad3dImage(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
VglImage* vglLoadNdImage(char* filename, int lStart, int lEnd, int* shape, int ndim, bool has_mipmap = 0);
// int SavePPM(char* filename, int w, int h, void* savebuf); // NOTE: Seems unused
// int vglSavePPM(char* filename, VglImage* img); // NOTE: Seems unused
int vglSavePgm(char* filename, VglImage* img);
VglImage* vglLoadPgm(char* filename);

VglImage* vglDcmtkLoadDicom(char* inFilename);
VglImage*  vglDcmtkLoad4dDicom(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
int vglDcmtkSaveDicom(char* outFilename, VglImage* imagevgl, int compress);
int vglDcmtkSaveDicomUncompressed(char* outFilename, VglImage* imagevgl);
int vglDcmtkSaveDicomCompressed(char* outFilename, VglImage* imagevgl);
int vglDcmtkSave4dDicom(char* filename, VglImage* image, int lStart, int lEnd, int compress = 0);
int convertDepthDcmtkToVgl(int dcmDepth);
int convertDepthVglToDcmtk(int vglDepth);

VglImage* vglGdcmLoadDicom(char* inFilename);
VglImage*  vglGdcmLoad4dDicom(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
int vglGdcmSaveDicom(char* outFilename, VglImage* imagevgl, int compress);
int vglGdcmSaveDicomUncompressed(char* outFilename, VglImage* imagevgl);
int vglGdcmSaveDicomCompressed(char* outFilename, VglImage* imagevgl);
int vglGdcmSave4dDicom(char* filename, VglImage* image, int lStart, int lEnd, int compress = 0);
int convertDepthGdcmToVgl(int dcmDepth);
int convertDepthVglToGdcm(int vglDepth);

VglImage* vglLoadTiff(char* inFilename);
IplImage* iplLoadTiff(char* inFilename);
VglImage* vglLoadTiffAlt(char* inFilename);
VglImage* vglLoad4dTiff(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
int vglSaveTiff(char* outFilename, VglImage* image);
int iplSaveTiff(char* outFilename, IplImage* image);
int vglSave4dTiff(char* filename, VglImage* image, int lStart, int lEnd);
int vglPrintTiffInfo(char* inFilename, char* msg = NULL);

#endif // VGL_IO_H
