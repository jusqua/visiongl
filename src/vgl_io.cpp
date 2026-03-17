#include <string.h>

#include "vgl_io.h"
#include "vglContext.h"

/** Save images with any dimension to disk

    TODO: fix 2d.
*/
void vglSaveImage(char* filename, VglImage* image)
{
  vglCheckContext(image, VGL_RAM_CONTEXT);

  if (image->ndim <= 2 && image->ipl != NULL)
  {
    //vglSaveNdImage(filename, image, 0, 0);
  }
  else if (image->ndim == 3)
  {
    vglSave3dImage(filename, image, 0, image->getLength() - 1);
  }
  else
  {
    fprintf(stderr, "%s:%s: Error: unable to save image with more than 3 dimensions\n", __FILE__, __FUNCTION__);
  }
}

void vglSaveIplImage(char* filename, IplImage* ipl, int* params /*= 0*/)
{
  if (ipl->depth == IPL_DEPTH_1U)
  {
    iplSaveImage(filename, ipl);
  }
  else
  {
    iplSaveImage(filename, ipl);
  }
}

/** Save PGM 3d images on the disk
*/
void vglSave3dImage(char* filename, VglImage* image, int lStart, int lEnd /*= -1*/)
{
  vglSaveNdImage(filename, image, lStart, lEnd);
}

/*
void vglSave3dImage(char* filename, VglImage* image, int lStart, int lEnd)
{
  //vglDownload(image); //must be fixed before enabling
  char* temp_filename = (char*)malloc(strlen(filename)+256);
  sprintf(temp_filename, filename, lStart);
  int d = image->depth / 8;
  if (d < 1) d = 1; //d is the byte size of the depth color format

  char* temp_image = (char*)malloc(image->getHeight()*image->getWidth()*image->nChannels*d);
  memcpy(temp_image,image->ndarray,image->getHeight()*image->getWidth()*image->nChannels*d);

  IplImage* ipl = cvCreateImage(cvSize(image->getWidth(), image->getHeight()), image->depth, image->nChannels);
  ipl->imageData = temp_image;

  iplSaveImage(temp_filename, ipl);

  int c = image->getHeight()*image->getWidth()*d*image->nChannels;
  for(int i = lStart+1; i <= lEnd; i++)
  {
    memcpy(temp_image,((char*)image->ndarray)+c,image->getHeight()*image->getWidth()*image->nChannels*d);
    ipl->imageData = temp_image;
    sprintf(temp_filename, filename, i);
    iplSaveImage(temp_filename, ipl);
    c += image->getHeight()*image->getWidth()*image->nChannels*d;
  }
  cvReleaseImage(&ipl);
  free(temp_image);
}
*/

void vglSaveNdImage(char* filename, VglImage* image, int lStart, int lEndParam /*= -1*/)
{
  vglCheckContext(image, VGL_RAM_CONTEXT);

  int ndim = image->vglShape->getNdim();
  int shapeFrames = image->getNFrames();
  int lEnd = shapeFrames + lStart - 1;
  if ( (lEnd > lEndParam) && (lEndParam >= 0) )
  {
    lEnd = lEndParam;
  }

  char* temp_filename = (char*)malloc(strlen(filename)+256);
  sprintf(temp_filename, filename, lStart);
  int d = image->depth / 8;
  if (d < 1) d = 1; //d is the byte size of the depth color format

  char* ptr = image->getImageData();
  char* temp_image =   (char*)malloc(image->getHeight() * image->getRowSizeInBytes());
  memcpy(temp_image, ptr, image->getHeight() * image->getRowSizeInBytes());
  IplImage* ipl = cvCreateImage(cvSize(image->getWidthIn(), image->getHeightIn()), image->depth, image->nChannels);

  ipl->imageData = temp_image;

  vglSaveIplImage(temp_filename, ipl);
  int c = image->getHeight()*image->getRowSizeInBytes();
  for(int i = lStart+1; i <= lEnd; i++)
  {
    memcpy(temp_image,((char*)ptr)+c,image->getHeight()*image->getRowSizeInBytes());
    ipl->imageData = temp_image;
    sprintf(temp_filename, filename, i);
    vglSaveIplImage(temp_filename, ipl);
    c += image->getHeight()*image->getRowSizeInBytes();
  }
  cvReleaseImage(&ipl);
  free(temp_image);
}

/** Load image from file to new VglImage.

    This function uses cvLoadImage to read image file.
 */
VglImage* vglLoadImage(char* filename, int iscolor /*= -1*/, int has_mipmap /*= 0*/)
{
  IplImage* ipl = iplLoadImage(filename, iscolor);
  VglImage* img;

  if (!ipl){
    fprintf(stderr, "vglCreateImage: Error loading image from file %s\n", filename);
    free(img);
    return NULL;
  }

  int bpp = (ipl->depth & 255) / 8;
  if (bpp == 0)
    bpp = 1;

  int width  = ipl->width;
  int height = ipl->height;
  if (ipl->nChannels == 1 && ipl->widthStep != (ipl->width * bpp))
  {
    fprintf(stderr,"%s, %s, Warning: OpenCV added %d px of padding to the image\n", __FILE__, __FUNCTION__, ipl->widthStep - (ipl->width * bpp));
    width = ipl->widthStep / bpp;
  }

  img = vglCreateImage(cvSize(width, height), ipl->depth, ipl->nChannels);
  cvReleaseImage(&img->ipl);
  img->ipl = ipl;

  vglSetContext(img, VGL_RAM_CONTEXT);

  if (img->ipl){
    return img;
  }
  else{
    free(img);
    return 0;
  }

}

/** /brief Load sequence of images as 3d image.

    Filename must have a printf compatible integer format specifier, like %d or %03d.
*/
VglImage* vglLoad3dImage(char* filename, int lStart, int lEnd, bool has_mipmap /*=0*/)
{
  VglImage* img;
  char* tempFilename = (char*)malloc(strlen(filename) + 256);
  sprintf(tempFilename, filename, lStart);

  IplImage* ipl = iplLoadImage(tempFilename, CV_LOAD_IMAGE_UNCHANGED);
  if (!ipl){
    fprintf(stderr, "%s: %s: Error loading image %s\n", __FILE__, __FUNCTION__, tempFilename);
    return 0;
  }

  int bpp = (ipl->depth & 255) / 8;
  if (bpp == 0)
    bpp = 1;

  int bps = VglShape::findBitsPerSample(ipl->depth); //TODO: refactor replacing bpp with bps

  int width = ipl->width;
  int height = ipl->height;
  int n = lEnd - lStart + 1;
  if (ipl->nChannels == 1 && ipl->widthStep != (ipl->width * bpp))
  {
    fprintf(stderr,"%s, %s, Warning: OpenCV added %d px of padding to the image\n", __FILE__, __FUNCTION__, ipl->widthStep - (ipl->width * bpp));
    width = ipl->widthStep / bpp;
  }

  int shape[VGL_MAX_DIM+1];
  int ndim = 3;
  for (int i = 0; i <= VGL_MAX_DIM; i++)
  {
    shape[i] = 1;
  }
  shape[0] = ipl->nChannels;
  shape[1] = width;
  shape[2] = height;
  shape[3] = n;
  img = vglCreateImage((int*)shape, ipl->depth, ndim);
  free(img->ndarray);
  img->ndarray = (char*)malloc(img->getTotalSizeInBytes());

  int delta = ipl->height*ipl->width*ipl->nChannels*bpp;
  int offset = delta;
  memcpy(img->ndarray, (void*)ipl->imageData, delta);
	cvReleaseImage(&ipl);
  for(int i = lStart+1; i <= lEnd; i++)
  {
    sprintf(tempFilename,filename,i);

    ipl = iplLoadImage(tempFilename, CV_LOAD_IMAGE_UNCHANGED);
    if (!ipl){
      fprintf(stderr, "%s: %s: Error loading image %s\n", __FILE__, __FUNCTION__, tempFilename);
      vglReleaseImage(&img);
      return 0;
    }

    memcpy(((char*)img->ndarray) + offset, (void*) ipl->imageData, delta);
    offset += delta;
		cvReleaseImage(&ipl);
  }

  vglSetContext(img, VGL_RAM_CONTEXT);
  //vglUpload(img); //must be fixed before enabling

  return img;
}

/** /brief Load sequence of images as n-dimensional image.

    Filename must have a printf compatible integer format specifier, like %d or %03d.
*/
VglImage* vglLoadNdImage(char* filename, int lStart, int lEnd, int* shape, int ndim, bool has_mipmap /*=0*/)
{
  VglImage* img;
  char* tempFilename = (char*)malloc(strlen(filename) + 256);
  sprintf(tempFilename, filename, lStart);

  IplImage* ipl = iplLoadImage(tempFilename, CV_LOAD_IMAGE_UNCHANGED);
  if (!ipl){
    fprintf(stderr, "%s: %s: Error loading image %s\n", __FILE__, __FUNCTION__, tempFilename);
    return 0;
  }

  int bpp = (ipl->depth & 255) / 8;
  if (bpp == 0)
    bpp = 1;

  int width = ipl->width;
  int height = ipl->height;
  int n = lEnd - lStart + 1;
  if (ipl->nChannels == 1 && ipl->widthStep != (ipl->width * bpp))
  {
    fprintf(stderr,"%s: %s: Warning: OpenCV added %d px of padding to the image\n", __FILE__, __FUNCTION__, ipl->widthStep - (ipl->width * bpp));
    width = ipl->widthStep / bpp;
  }

  shape[VGL_SHAPE_NCHANNELS] = ipl->nChannels;
  if (shape[VGL_SHAPE_WIDTH] <= 0)
  {
    shape[VGL_SHAPE_WIDTH] = width;
  }
  if (shape[VGL_SHAPE_HEIGHT] <= 0)
  {
    shape[VGL_SHAPE_HEIGHT] = height;
  }
  VglShape* vglShape = new VglShape(shape, ndim);
  int shapeSize = vglShape->getSize();

  VglImage* tmp = vglCreateImage(ipl);
  int imgSize = tmp->vglShape->getSize() * n;
  vglReleaseImage(&tmp);

  if (imgSize != shapeSize)
  {
    fprintf(stderr, "%s: %s: Error: stack size = %d != %d = given shape size\n", __FILE__, __FUNCTION__, imgSize, shapeSize);
    return 0;
  }

  img = vglCreateImage(shape, ipl->depth, ndim);

  int delta = ipl->height*ipl->width*ipl->nChannels*bpp;
  int offset = delta;
  char* ptr = img->getImageData();
  memcpy(ptr, (void*)ipl->imageData, delta);
  cvReleaseImage(&ipl);

  for(int i = lStart+1; i <= lEnd; i++)
  {
    sprintf(tempFilename,filename,i);

    ipl = iplLoadImage(tempFilename, CV_LOAD_IMAGE_UNCHANGED);
    if (!ipl){
      fprintf(stderr, "%s: %s: Error loading image %s\n", __FILE__, __FUNCTION__, tempFilename);
      vglReleaseImage(&img);
      return 0;
    }

    memcpy(((char*)ptr) + offset, (void*) ipl->imageData, delta);
    offset += delta;
    cvReleaseImage(&ipl);
  }
  vglSetContext(img, VGL_RAM_CONTEXT);
  //vglUpload(img); //must be fixed before enabling

  return img;
}

/** Save image data to PPM file, 3 channels, unsigned byte

    Time to save a VGA image = 3.5ms
*/
// int SavePPM(char* filename, int w, int h, void* savebuf){
//     FILE *fp = fopen(filename, "wb");
//     fprintf(fp, "P6\n%d %d\n255\n", w, h);
//     fwrite(savebuf, w * h * 3, 1, fp);
//     fclose(fp);
//     return 0;
// }


/** Save image to PPM file, 3 channels, unsigned byte

*/
// int vglSavePPM(char* filename, VglImage* img){
//     vglCheckContext(img, VGL_GL_CONTEXT);
//     vglDownloadPPM(img);
//     return SavePPM(filename, img->getWidth(), img->getHeight(), img->ipl->imageData);
// }

/** Save image to PGM/PPM file, 1 or 3 channels, unsigned byte

*/
int vglSavePgm(char* filename, VglImage* img){
  vglCheckContext(img, VGL_RAM_CONTEXT);

  char* buf = img->getImageData();
  int w = img->getWidth();
  int h = img->getHeight() * img->getNFrames();
  int widthStep = img->getWidthStep();
  int c = img->getNChannels();
  int bps = iplFindBitsPerSample(img->depth);
  int result = iplGenericSavePgm(filename, buf, w, h, widthStep, c, bps);
  return result;
}


/** Load image data from PGM/PPM file.

    Load image data from PGM/PPM file, 1 or 3 channels, unsigned byte or short.
*/
VglImage* vglLoadPgm(char* filename){
  IplImage* ipl = iplLoadPgm(filename);
  VglImage* vgl = vglCreateImage(ipl);
  cvCopy(ipl, vgl->ipl);
  cvReleaseImage(&ipl);
  return vgl;
}
