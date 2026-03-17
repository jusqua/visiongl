#include <stdio.h>
#include "vgl_context.h"
#include "vgl_image.h"

/*  Call before using image. Context must be unique.

    Return 0 in case of error. Resulting context if successful.

    Shaders (vglCopy, vglDilateSq3, vglCudaInvertOnPlace etc)
    CALL ->
    vglCheckContext
    CALL ->
    vglUpload, vglDownload, vglGlToCuda, vglCudaToGl
    CALL ->
    vglSetContext, vglAddContext

 */
int vglCheckContext(VglImage* img, int context){
  if (!vglIsContextUnique(context)){
    fprintf(stderr, "vglCheckContext: Error: context = %d is not unique or invalid\n", context);
    return 0;
  }
  if (vglIsInContext(img, context)){
    return context;
  }
  switch (context){
    case VGL_RAM_CONTEXT:
      if (vglIsInContext(img, VGL_BLANK_CONTEXT)){
        //printf("%s: case 1\n", __FUNCTION__);
        vglAddContext(img, VGL_RAM_CONTEXT);
      }
      else
      if (vglIsInContext(img, VGL_CL_CONTEXT)){
        printf("%s: case 2\n", __FUNCTION__);
	/*
        if (img->getBitsPerSample() == 1 && VGL_PACK_SIZE_BYTES > 1)
	{
          VglImage aux* = vglCreateImage(img);
          if ( !(img->clForceAsBuf) )
	  {
            if (img->ndim == 2)
            {
              vglClBinSwap(img, aux);
	    }
            else if (img->ndim == 3)
              vglCl3dBinSwap(img, aux);
	    }
            vglClDownload(aux);
            memcpy(img->getImageData(), aux->getImageData(), img->getImageSizeInBytes());
            vglReleaseImage(aux);
            vglAddContext(img, VGL_RAM_CONTEXT);
	  }
          else
	  {
            fprintf(stdout, "%s:%s: Error: clForceAsBuf case not implemented\n", __FILE__, __FUNCTION__);
            //vglNdClBinSwap(img, aux);
            //vglClDownload(aux);
            //memcpy(img->getImageData(), aux->getImageData(), img->getImageSizeInBytes());
            //vglReleaseImage(aux);
            //vglAddContext(img, VGL_RAM_CONTEXT);
	  }
	}
        else
	*/
	{
          vglClDownload(img);
	}
      }
      else
      if (!vglIsInContext(img, VGL_RAM_CONTEXT))
      {
        //printf("%s: case 3\n", __FUNCTION__);
        vglCheckContext(img, VGL_GL_CONTEXT);
        if (vglIsInContext(img, VGL_GL_CONTEXT)){
          vglDownload(img);
        }
      }
      else{
        fprintf(stderr, "vglCheckContext: Internal Error: unable to transfer to RAM from invalid context = %d\n", img->inContext);
      }
    break;
    case VGL_GL_CONTEXT:
      if (vglIsInContext(img, VGL_BLANK_CONTEXT)){
        //printf("%s: case 1\n", __FUNCTION__);
        vglAddContext(img, VGL_RAM_CONTEXT);
      }
      else
      if (vglIsInContext(img, VGL_CL_CONTEXT)){
        vglClDownload(img);
        //if (!ok){
        //  fprintf(stderr, "vglCheckContext: error transfering from cuda to gl\n");
	//}
      }
      if (vglIsInContext(img, VGL_RAM_CONTEXT)){
        vglUpload(img);
      }
      else if (vglIsInContext(img, VGL_CUDA_CONTEXT)){
        int ok = vglCudaToGl(img);
        if (!ok){
          fprintf(stderr, "vglCheckContext: error transfering from cuda to gl\n");
	}
      }
      else{
        fprintf(stderr, "vglCheckContext: Internal Error: unable to transfer to GL from invalid context = %d\n", img->inContext);
        vglPrintImageInfo(img);
      }
    break;
    case VGL_CUDA_CONTEXT:
      vglCheckContext(img, VGL_GL_CONTEXT);
      if (vglIsInContext(img, VGL_GL_CONTEXT)){
        int ok = vglGlToCuda(img);
        if (!ok){
          fprintf(stderr, "vglCheckContext: Error: unable to transfer from gl to cuda\n");
	}
      }
      else{
        fprintf(stderr, "vglCheckContext: Internal Error: unable to transfer to CUDA from invalid context = %d\n", img->inContext);
      }
    break;
    case VGL_CL_CONTEXT:
      if (vglIsInContext(img, VGL_BLANK_CONTEXT)){
        vglClUpload(img);
      }
      else{
        vglCheckContext(img, VGL_RAM_CONTEXT);
        if (vglIsInContext(img, VGL_RAM_CONTEXT) || vglIsInContext(img, VGL_BLANK_CONTEXT)){
          vglClUpload(img);
          //if (!ok){
          //  fprintf(stderr, "vglCheckContext: Error: unable to transfer from ram to cl\n");
         //}
        }
        else{
          fprintf(stderr, "vglCheckContext: Internal Error: unable to transfer to CL from invalid context = %d\n", img->inContext);
        }
      }
    break;
    default:
      fprintf(stderr, "vglCheckContext: Error: Trying to copy to invalid context = %d\n", context);
      return 0;
  }
  return img->inContext;
}

/*  Call before writing to image. Context must be unique.

    GL and RAM images are always allocated. CUDA images must be allocated
    if cudaPbo == -1.

 */
// int vglCheckContextForOutput(VglImage* img, int context){
//   if (img){
//     #ifdef __CUDA__
//     if (context == VGL_CUDA_CONTEXT){
//       if (img->cudaPbo == -1 || img->cudaPtr == 0){
//         return vglCudaAlloc(img);
//       }
//     }
//     #endif
//   }
//   return 0;
// }
