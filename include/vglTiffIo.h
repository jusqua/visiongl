/*********************************************************************
***                                                                ***
***  Header file vglTiffIo.h                                       ***
***                                                                ***
*********************************************************************/

#ifndef __VGLTIFFIO_H__
#define __VGLTIFFIO_H__

#ifdef __TIFF__

#include <vglImage.h>

//IplImage
#ifdef __OPENCV__
  #include <opencv2/core/types_c.h>
#else
  #include <vglOpencv.h>
#endif

VglImage* vglLoadTiff(char* inFilename);
IplImage* iplLoadTiff(char* inFilename);
VglImage* vglLoadTiffAlt(char* inFilename);
VglImage* vglLoad4dTiff(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
int vglSaveTiff(char* outFilename, VglImage* image);
int iplSaveTiff(char* outFilename, IplImage* image);
int vglSave4dTiff(char* filename, VglImage* image, int lStart, int lEnd);

int vglPrintTiffInfo(char* inFilename, char* msg = NULL);

#endif

#endif
