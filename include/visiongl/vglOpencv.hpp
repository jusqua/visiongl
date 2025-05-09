#ifndef VISIONGL_VGLOPENCV_HPP
#define VISIONGL_VGLOPENCV_HPP

#ifndef __OPENCV__

#include <visiongl/vglCommonTypes.hpp>

/*********************************************************************
***  Function prototypes for OpenCV compatibility layer            ***
*********************************************************************/

void cvReleaseImage(IplImage** p_ipl);
IplImage* cvCreateImage(CvSize size, int depth, int channels);
IplImage* cvCopy(IplImage* src, IplImage* dst);
void cvCvtColor(IplImage* src, IplImage* dst, int code);
IplImage* cvLoadImage(char* filename, int iscolor = CV_LOAD_IMAGE_UNCHANGED);
int cvSaveImage(char* filename, IplImage* image, int* params = 0);

#endif  // __OPENCV__

#endif  // VISIONGL_VGLOPENCV_HPP