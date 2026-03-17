#include <stdint.h>
#include <string.h>

#include "vgl_image.h"
#include "legacy_opencv.h"

/** Create image with same format and data as img_in.

    TODO: fix. Not working as expected.
 */
// VglImage* vglCopyCreateImage(VglImage* img_in)
// {
//   VglImage* retval = vglCreateImage(cvSize(img_in->getWidthIn(), img_in->getHeightIn()), img_in->depth, img_in->nChannels, img_in->ndim, img_in->has_mipmap);
//   #ifdef __OPENCL__
//   if (img_in->clForceAsBuf)
//   {
//     vglClForceAsBuf(retval);
//   }
//   #endif

//   if (vglIsInContext(img_in, VGL_GL_CONTEXT))
//   {
//     vglCopy(img_in, retval);
//   }

//TODO: Finish this function
/*
  if !(vglIsInContext(img_in, VGL_BLANK_CONTEXT))
  {
    if (vglIsInContext(img_in, VGL_RAM_CONTEXT))
    {
      //TODO: Function to copy image from RAM to RAM
    }
    if (vglIsInContext(img_in, VGL_GL_CONTEXT))
    {
      vglCopy(img_in, retval);
    }
#ifdef __CUDA__
    if (vglIsInContext(img_in, VGL_CUDA_CONTEXT))
    {
      vglCudaCopy(img_in, retval);
    }
#endif
#ifdef __OPENCL__
    if (vglIsInContext(img_in, VGL_CL_CONTEXT))
    {
      vglClCopy(img_in, retval);
    }
#endif
  }
*/
//   return retval;
// }

/** Create image with same format and data as img_in
 */
VglImage* vglCopyCreateImage(IplImage* img_in, int ndim /*=2*/, int has_mipmap /*=0*/)
{
  VglImage* retval = vglCreateImage(cvSize(img_in->width, img_in->height), img_in->depth, img_in->nChannels, ndim, has_mipmap);
  cvCopy(img_in, retval->ipl);
  vglSetContext(retval, VGL_RAM_CONTEXT);
  //vglUpload(retval);
  return retval;
}

/** Create image with same format as img_in
 */
VglImage* vglCreateImage(VglImage* img_in)
{
  VglImage* retval = vglCreateImage(img_in->vglShape->shape, img_in->depth, img_in->ndim, img_in->has_mipmap);
  #ifdef __OPENCL__
  if (img_in->clForceAsBuf)
  {
    vglClForceAsBuf(retval);
  }
  #endif
  return retval;
}


/** Create image with same format as img_in
 */
VglImage* vglCreateImage(IplImage* img_in, int ndim /*=2*/, int has_mipmap /*=0*/)
{
  VglImage* retval = vglCreateImage(cvGetSize(img_in), img_in->depth, img_in->nChannels, ndim, has_mipmap);
  return retval;
}

/** Create image as described by the parameters
 */
VglImage* vglCreateImage(int* shape, int depth, int ndim /*=2*/, int has_mipmap /*=0*/)
{
  VglImage* vglImage = new VglImage;

  if (ndim > VGL_MAX_DIM)
  {
    fprintf(stderr, "%s:%s: Error: image should have at most %d dimensions but has %d dimensions\n", __FILE__, __FUNCTION__, VGL_MAX_DIM, ndim);
    return NULL;
  }

  vglImage->ipl = NULL;
  vglImage->ndarray = NULL;
  for (int i = 0; i <= VGL_MAX_DIM; i++)
  {
    if (i <= ndim)
    {
      vglImage->shape[i] = shape[i];
    }
    else
    {
      vglImage->shape[i] = 1;
    }
  }
  if ( (ndim == 1) && (shape[VGL_SHAPE_HEIGHT] > 1) )
  {
    vglImage->shape[VGL_SHAPE_HEIGHT] = shape[VGL_SHAPE_HEIGHT];
  }
  int bps = VglShape::findBitsPerSample(depth);
  vglImage->vglShape = new VglShape(vglImage->shape, ndim, bps);

  vglImage->ndim      = ndim;
  vglImage->depth     = depth;
  vglImage->nChannels = shape[0];
  vglImage->has_mipmap = has_mipmap;
  vglImage->fbo = -1;
  vglImage->tex = -1;
#ifdef __CUDA__
  vglImage->cudaPtr = NULL;
  vglImage->cudaPbo = -1;
#endif
#ifdef __OPENCL__
  vglImage->oclPtr = NULL;
  vglImage->clForceAsBuf = 0;
#endif
  vglImage->filename = NULL;


  if (ndim <= 2)
  {
    vglImage->ipl = cvCreateImage(cvSize(shape[1], shape[2]), depth, shape[0]);
  }
  else
  {
    vglImage->ndarray = malloc(vglImage->getTotalSizeInBytes());
  }

  vglSetContext(vglImage, VGL_BLANK_CONTEXT);

  return vglImage;
}

/** Create image as described by the parameters
 */
VglImage* vglCreateImage(VglShape* vglShape, int depth, int has_mipmap /*=0*/)
{
  VglImage* vglImage = vglCreateImage(vglShape->shape, depth, vglShape->ndim, has_mipmap);
  return vglImage;
}

/** Create image as described by the parameters
 */
VglImage* vglCreateImage(CvSize size, int depth, int nChannels, int ndim, int has_mipmap)
{
  int shape[VGL_MAX_DIM];
  shape[0] = nChannels;
  shape[1] = size.width;
  shape[2] = size.height;

  VglImage* vglImage = vglCreateImage(shape, depth, ndim, has_mipmap);

  return vglImage;
}

/** Create image as described by the parameters
 */
VglImage* vglCreate3dImage(CvSize size, int depth, int nChannels, int layers, int has_mipmap /*=0*/)
{
  VglImage* vglImage;
  IplImage* ipl = NULL;

  int shape[VGL_MAX_DIM];
  for (int i = 0; i < VGL_MAX_DIM; i++)
  {
    shape[i] = 0;
  }
  shape[0] = nChannels;
  shape[1] = size.width;
  shape[2] = size.height;
  shape[3] = layers;

  vglImage = vglCreateImage(shape, depth, 3, has_mipmap);

  return vglImage;
}


/** Create image as described by the parameters
 */
VglImage* vglCreateNdImage(int ndim, int* shape, int depth, int has_mipmap /*=0*/)
{
  VglImage* vglImage = vglCreateImage(shape, depth, ndim, has_mipmap);

  return vglImage;
}

/** Converts ndarray from 3 channels to 4 channels
  */
void vglNdarray3To4Channels(VglImage* img)
{

    if (img->nChannels == 4)
    {
        fprintf(stdout, "%s:%s: Warning: image already has 4 channels\n", __FILE__, __FUNCTION__);
        return;
    }
    else if (img->nChannels != 3)
    {
        fprintf(stderr, "%s:%s: Error: image should have 3 channels but has %d channels\n", __FILE__, __FUNCTION__, img->nChannels);
        return;
    }

    int d = img->depth / 8;
    if (d < 1) d = 1; //d is the byte size of the depth color format

    int datasize = img->getHeight() * img->getWidth() * 4 * d * img->getLength();

    void* newndarray = (char*)malloc(img->getHeight() * img->getWidth() * 4 * d * img->getLength());

    int offset = 0;
    uint8_t temp_alpha = 0;
    for(int i = 0; i < (datasize/d); i++)//for(int i = (datasize/d)-1; i >= 0; i--)
    {
        if (((i+1) % 4) == 0)
        {
            switch(d)
            {
                case 1:
                    ((uint8_t*)newndarray)[i] = temp_alpha;
                    break;
                case 2:
                    ((uint16_t*)newndarray)[i] = temp_alpha;
                    break;
                case 4:
                    ((uint32_t*)newndarray)[i] = temp_alpha;
                    break;
                case 8:
                  ((uint64_t*)newndarray)[i] = temp_alpha;
                    break;
            }
        }
        else
        {
            switch(d)
            {
                case 1:
                    ((uint8_t*)newndarray)[i] = ((uint8_t*)img->ndarray)[offset];
                    break;
                case 2:
                    ((uint16_t*)newndarray)[i] = ((uint16_t*)img->ndarray)[offset];
                    break;
                case 4:
                    ((uint32_t*)newndarray)[i] = ((uint32_t*)img->ndarray)[offset];
                    break;
                case 8:
                    ((uint64_t*)newndarray)[i] = ((uint64_t*)img->ndarray)[offset];
                    break;
            }
            offset++;
        }
    }
    printf("freeing ndarray inside 3to4 channels\n");
    free(img->ndarray);
    printf("freeing ndarray inside 3to4 channels OK\n");

    img->vglShape->shape[0] = 4;
    VglShape* vglShape = new VglShape(img->vglShape->shape, img->vglShape->ndim);
    delete(img->vglShape);
    img->vglShape = vglShape;

    img->ndarray = newndarray;
    img->nChannels = 4;
}

/** Converts ndarray from 4 channels to 3 channels
  */
void vglNdarray4To3Channels(VglImage* img)
{
    if (img->nChannels == 3)
    {
        fprintf(stdout, "%s:%s: Warning: image already has 3 channels\n", __FILE__, __FUNCTION__);
        return;
    }
    else if (img->nChannels != 4)
    {
        fprintf(stderr, "%s:%s: Error: image should have 4 channels but has %d channels\n", __FILE__, __FUNCTION__, img->nChannels);
        return;
    }

    int d = img->depth / 8;
    if (d < 1) d = 1;

    int datasize = img->getHeight() * img->getWidth() * img->nChannels * d * img->getLength();

    void* newndarray = malloc(img->getHeight() * img->getWidth() * 3 * d * img->getLength());

    int offset = 0;
    for(int i = 0 ; i < (datasize/d); i++)
    {
        if (((i+1) % 4) != 0)
        {
            switch(d)
            {
               case 1:
                   ((uint8_t*)newndarray)[offset] = ((uint8_t*)img->ndarray)[i];
                   break;
               case 2:
                   ((uint16_t*)newndarray)[offset] = ((uint16_t*)img->ndarray)[i];
                   break;
               case 4:
                   ((uint32_t*)newndarray)[offset] = ((uint32_t*)img->ndarray)[i];
                   break;
               case 8:
                   ((uint64_t*)newndarray)[offset] = ((uint64_t*)img->ndarray)[i];
                   break;
            }
            offset++;
        }
    }

    free(img->ndarray);

    img->vglShape->shape[0] = 3;
    VglShape* vglShape = new VglShape(img->vglShape->shape, img->vglShape->ndim);
    delete(img->vglShape);
    img->vglShape = vglShape;

    img->ndarray = newndarray;
    img->nChannels = 3;
}

/** Convert ipl field of VglImage from 3 to 4 channels
 */
void vglIpl3To4Channels(VglImage* img)
{
    if (!img->ipl){
        return;
    }

    if (img->nChannels == 4)
    {
        fprintf(stdout, "%s:%s: Warning: image already has 4 channels\n", __FILE__, __FUNCTION__);
        return;
    }
    else if (img->nChannels != 3)
    {
        fprintf(stderr, "%s:%s: Error: image should have 3 channels but has %d channels\n", __FILE__, __FUNCTION__, img->nChannels);
        return;
    }

    IplImage* iplRGBA = cvCreateImage(cvGetSize(img->ipl), img->ipl->depth, 4);
    cvCvtColor(img->ipl, iplRGBA, CV_RGB2RGBA);
    cvReleaseImage(&(img->ipl));
    img->ipl = iplRGBA;
    img->nChannels = 4;

    img->vglShape->shape[0] = 4;
    VglShape* vglShape = new VglShape(img->vglShape->shape, img->vglShape->ndim);
    vglShape->print();
    delete(img->vglShape);
    img->vglShape = vglShape;
    img->vglShape->print();
}

/** Convert ipl field of VglImage from 4 to 3 channels
 */
void vglIpl4To3Channels(VglImage* img)
{
    if (!img->ipl){
        return;
    }
    if (img->nChannels == 3)
    {
        fprintf(stdout, "%s:%s: Warning: image already has 3 channels\n", __FILE__, __FUNCTION__);
        return;
    }
    else if (img->nChannels != 4)
    {
        fprintf(stderr, "%s:%s: Error: image should have 4 channels but has %d channels\n", __FILE__, __FUNCTION__, img->nChannels);
        return;
    }

    IplImage* iplRGB = cvCreateImage(cvGetSize(img->ipl), img->ipl->depth, 3);
    cvCvtColor(img->ipl, iplRGB, CV_RGBA2RGB);
    cvReleaseImage(&(img->ipl));
    img->ipl = iplRGB;
    img->nChannels = 3;

    img->vglShape->shape[0] = 3;
    VglShape* vglShape = new VglShape(img->vglShape->shape, img->vglShape->ndim);
    delete(img->vglShape);
    img->vglShape = vglShape;
}

/** Convert VglImage from 3 to 4 channels
 */
void vglImage3To4Channels(VglImage* img)
{
    if (img->nChannels != 3)
    {
        return;
    }
    if (img->ipl)
    {
        vglIpl3To4Channels(img);
    }
    if (img->ndarray)
    {
        vglNdarray3To4Channels(img);
    }
}

/** Convert VglImage from 4 to 3 channels
 */
void vglImage4To3Channels(VglImage* img)
{
    if (img->nChannels == 3)
    {
        fprintf(stdout, "%s:%s: Warning: image already has 3 channels\n", __FILE__, __FUNCTION__);
        return;
    }
    else if (img->nChannels != 4)
    {
        fprintf(stderr, "%s:%s: Error: image should have 4 channels but has %d channels\n", __FILE__, __FUNCTION__, img->nChannels);
        return;
    }

    if (img->ipl)
    {
        vglIpl4To3Channels(img);
    }
    if (img->ndarray)
    {
        vglNdarray4To3Channels(img);
    }
}

/** Release memory occupied by image in RAM and GPU
 */
void vglReleaseImage(VglImage** p_image)
{
  VglImage* image = *p_image;
  if (!image){
    fprintf(stdout, "%s:%s: Warning: image is null\n", __FILE__, __FUNCTION__);
    return;
  }
  if (image->ipl){
    cvReleaseImage(&(image->ipl));
  }
  if (image->ndarray){
    free(image->ndarray);
  }
  if (image->fbo != -1){
    glDeleteFramebuffersEXT(1, &(image->fbo));
  }
  if (image->tex != -1){
    glDeleteTextures(1, &(image->tex));
  }
#ifdef __OPENCL__
  if (image->oclPtr != NULL){
    clReleaseMemObject(image->oclPtr);
  }
#endif
  delete(*p_image);
  p_image = NULL;
}

/** Replace IplImage, stored inside a VglImage, with new IplImage.

    Both new and old images must have exactly the same properties,
    dimensions, depth, type etc. Is useful when grabbing frames from a camera.
 */
void vglReplaceIpl(VglImage* image, IplImage* new_ipl)
{
  IplImage* ipl;

  if (!image){
    fprintf(stdout, "vglReplaceIpl: Warning: VglImage is null\n");
    return;
  }
  if (!new_ipl){
    fprintf(stdout, "vglReplaceIpl: Warning: new IplImage is null\n");
    return;
  }
  ipl = image->ipl;
  if (ipl){
    cvReleaseImage(&ipl);
  }
  image->ipl = new_ipl;

  vglSetContext(image, VGL_RAM_CONTEXT);
  //vglUpload(image);
}

/** /brief Reshape image to given shape.

    Reshape image to given shape. Size of original and new shape must be the same.
*/
int vglReshape(VglImage* img, VglShape* newShape)
{
  VglShape* origShape = img->vglShape;
  int origSize = origShape->getSize();
  int newSize = newShape->getSize();
  if (origSize != newSize)
  {
    fprintf(stderr, "%s: %s: Error: original shape size = %d != %d = new shape size\n", __FILE__, __FUNCTION__, origSize, newSize);
    exit(1);
  }
  if ( (img->ipl != NULL) && (newShape->ndim > 2) )
  {
    int ws = newShape->findWidthStep(newShape->getBps(), newShape->getWidth(), newShape->getNChannels());
    if (img->ipl->widthStep != ws)
    {
      fprintf(stderr, "%s: %s: Error: ipl widthStep = %d != %d = new widthStep.\n", __FILE__, __FUNCTION__, ws, img->ipl->widthStep);
      //exit(1);
    }
    img->vglShape = new VglShape(newShape);
    int size = img->getTotalSizeInBytes();
    img->ndarray = malloc(size);
    memcpy(img->ndarray, (void*)img->ipl->imageData, size);
    cvReleaseImage(&img->ipl);
    /*
    fprintf(stderr, "%s: %s: Error: unable to reshape ipl image to shape with more than 2 dimensions\n", __FILE__, __FUNCTION__);
    exit(1);
    */
  }
  img->vglShape = new VglShape(newShape);
  delete(origShape);
  return 0;
}

/** Print information about image.

    Print width, height, depth and number of channels

 */
void iplPrintImageInfo(IplImage* ipl, char* msg){
        if (msg){
            printf("====== %s:\n", msg);
	}
	else
	{
            printf("====== iplPrintImageInfo:\n");
	}
        printf("Image @ %p: w x h = %d(%d) x %d\n",
                ipl, ipl->width, ipl->widthStep, ipl->height);
        printf("imageData @ %p\n", ipl->imageData);
        printf("nChannels = %d\n", ipl->nChannels);
        printf("depth = ");
        switch (ipl->depth){
          case IPL_DEPTH_1U:  printf("IPL_DEPTH_1U");  break;
          case IPL_DEPTH_8U:  printf("IPL_DEPTH_8U");  break;
          case IPL_DEPTH_16U: printf("IPL_DEPTH_16U"); break;
          case IPL_DEPTH_32F: printf("IPL_DEPTH_32F"); break;
          case IPL_DEPTH_8S:  printf("IPL_DEPTH_8S");  break;
          case IPL_DEPTH_16S: printf("IPL_DEPTH_16S"); break;
          case IPL_DEPTH_32S: printf("IPL_DEPTH_32S"); break;
          default: printf("unknown");
	}
        printf("\n");
}

/** Print information about image.

    Print width, height, depth, number of channels, OpenGL texture handler,
    OpenGL FBO handler, and current valid context (RAM, GPU or FBO).

 */
void vglPrintImageInfo(VglImage* image, char* msg){
    //IplImage* ipl = image->ipl;
    if (msg){
        printf("====== %s:\n", msg);
    }
    else
    {
        printf("====== vglPrintImageInfo:\n");
    }
    printf("Image @ %p: w x h x l = %d(%d) x %d x %d\n",
	    image, image->getWidth(), image->getWidthStep(), image->getHeight(), image->getLength());
    printf("ndim = %d\n", image->ndim);
    printf("size = %d\n", image->vglShape->getSize());
    printf("shape = {");
    for(int i = 0; i <= VGL_MAX_DIM; i++)
    {
      if (i > 0) printf(", ");
      printf("%d", image->vglShape->shape[i]);
    }
    printf("}\n");
    printf("Ipl @ %p\n", image->ipl);
    printf("ndarray @ %p\n", image->ndarray);
    printf("nChannels = %d\n", image->nChannels);
    printf("depth = ");
    switch (image->depth){
        case IPL_DEPTH_1U:  printf("IPL_DEPTH_1U");  break;
        case IPL_DEPTH_8U:  printf("IPL_DEPTH_8U");  break;
        case IPL_DEPTH_16U: printf("IPL_DEPTH_16U"); break;
        case IPL_DEPTH_32F: printf("IPL_DEPTH_32F"); break;
        case IPL_DEPTH_8S:  printf("IPL_DEPTH_8S");  break;
        case IPL_DEPTH_16S: printf("IPL_DEPTH_16S"); break;
        case IPL_DEPTH_32S: printf("IPL_DEPTH_32S"); break;
        default: printf("unknown");
    }
    printf("\n");
    printf("TEX = %d\n", image->tex);
    printf("FBO = %d\n", image->fbo);
#ifdef __CUDA__
    printf("CUDAPtr @ %p\n", image->cudaPtr);
    printf("CUDAPbo = %d\n", image->cudaPbo);
#endif
#ifdef __OPENCL__
    printf("OCL @ %p\n", image->oclPtr);
    printf("clForceAsBuf = %d\n", image->clForceAsBuf);
#endif
    printf("Context = %d\n", image->inContext);
}

/** Print image pixels in text format to stdout

 */
void vglPrintImageData(VglImage* image, char* msg /*= NULL*/, char* format /*= "%c"*/){
  if (msg){
    printf("====== %s:\n", msg);
  }
  else
  {
    printf("====== vglPrintImageData:\n");
  }
  int w = image->getWidthStep();
  int h = image->getHeight();
  int ndarraySize = image->getTotalSizeInBytes();
  char* ptr = image->getImageData();

  for(int i = 0; i < ndarraySize;)
  {
    if (i % w == 0)
    {
      printf("%d: ", i / w);
    }
    printf(format, ((unsigned char*)ptr)[i]);
    i++;
    if (i % w == 0)
    {
      printf("\n");
    }
    else if (i % 8 == 0)
    {
      printf(" ");
    }
  }
}

/** Print image pixels in text format to stdout

 */
void iplPrintImageData(IplImage* image, char* msg /*= NULL*/, char* format /*= "%c"*/){
  if (msg){
    printf("====== %s:\n", msg);
  }
  else
  {
    printf("====== iplPrintImageData:\n");
  }
  int w = image->widthStep;
  int h = image->height;
  int ndarraySize = w * h;
  char* ptr = image->imageData;

  for(int i = 0; i < ndarraySize;)
  {
    if (i % w == 0)
    {
      printf("%d: ", i / w);
    }
    printf(format, ((unsigned char*)ptr)[i]);
    i++;
    if (i % w == 0)
    {
      printf("\n");
    }
    else if (i % 8 == 0)
    {
      printf(" ");
    }
  }
}

// NOTE: Seems to be a personal test function, probably unutil
/** Save compressed YUV411 image data to file.

    Requires one half of the disk space required to save an uncompressed PPM.
*/
// int SaveYUV411(char* filename, int w, int h, void* savebuf){
//     w = (int) 1.5 * w;
//     FILE *fp = fopen(filename, "wb");
//     fprintf(fp, "P5\n%d %d\n255\n", w, h);
//     fwrite(savebuf, w * h, 1, fp);
//     fclose(fp);
//     return 0;
// }

/** Force image to be traated as buffer

    Data with 2 and 3 dimensions are, by default, treated as images, i.e. are created
    and transferred by the API functions
    clCreate2DImage, clCreate3DImage, clEnqueueWriteImage, clEnqueueReadImage.

    To treat data as images has as advantage the possibility of automatic clamping to edge
    in window operations.

    Data with 1, 4 or more dimensions are always treated as buffers, i.e. are created
    and transferred by the API functions
    clCreateBuffer, clEnqueueWriteBuffer, clEnqueueReadBuffer.

    Use this function to force 2 and 3 dimensions data to be treated as buffers.

*/
void vglClForceAsBuf(VglImage*  img)
{
#ifdef __OPENCL__
  img->clForceAsBuf = 1;
#else
  fprintf(stderr, "%s: %s: OpenCL not supported. Please recompile setting WITH_OPENCL to true in the Makefile.\n", __FILE__, __FUNCTION__);
#endif
}

/*  Call after copy. Context must be unique.

    Return 0 in case of error. Resulting context if successful.
 */
int vglAddContext(VglImage* img, int context){
  if (!vglIsContextUnique(context)){
    fprintf(stderr, "vglAddContext: Error: context = %d is not unique or invalid\n", context);
    return 0;
  }
  img->inContext = img->inContext | context;
  return img->inContext;
}

/*  Call after processing or creating image. Context must be unique.
    After creating, use VGL_BLANK_CONTEXT

    Return 0 in case of error. Resulting context if successful.
 */
int vglSetContext(VglImage* img, int context){
  if (!vglIsContextUnique(context) && context != 0){
    fprintf(stderr, "vglSetContext: Error: context = %d is not unique\n", context);
    return 0;
  }
  img->inContext = context;
  return img->inContext;
}

void vglPrintContext(int context, char* msg){
  if (msg) printf("%s", msg);
  printf("(");
  ( context & VGL_RAM_CONTEXT ? printf("RAM ") : printf("    ") );
  ( context & VGL_GL_CONTEXT ? printf("GL ") : printf("   ") );
  ( context & VGL_CUDA_CONTEXT ? printf("CUDA") : printf("    ") );
  printf(") (%d)\n", context);
}

void vglPrintContext(VglImage* img, char* msg){
  vglPrintContext(img->inContext, msg);
}
