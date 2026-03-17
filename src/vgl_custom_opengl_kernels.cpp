#include "vgl_custom_opengl_kernels.h"
#include "vgl_opengl_kernels.g.h"
#include "vgl_opengl_context.h"
#include "vgl_context_utils.h"

/** Copy data from src texture to dst texture
 */
void vglCopyImageTex(VglImage* src, VglImage* dst)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	vglCheckContext(src, VGL_GL_CONTEXT);

	glBindTexture(GL_TEXTURE_2D, src->tex);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
	ERRCHECK()

	glViewport(0, 0, 2*dst->getWidth(), 2*dst->getHeight());

	glBegin(GL_QUADS);
		glTexCoord2f( 0.0,  0.0);
		glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

		glTexCoord2f( 1.0,  0.0);
		glVertex3f (  0.0, -1.0, 0.0); //Right Up

		glTexCoord2f( 1.0,  1.0);
		glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

		glTexCoord2f( 0.0,  1.0);
		glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
	glEnd();

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Copy data from src texture to dst texture using a fragment shader
 */
void vglCopyImageTexFS(VglImage* src, VglImage* dst)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src, VGL_GL_CONTEXT);

      glBindTexture(GL_TEXTURE_2D, src->tex);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
      ERRCHECK()

  static GLuint f = 0;
  if (f == 0){
    fprintf(stdout, "FRAGMENT SHADER\n====================\n");
    f = vglShaderLoad(GL_FRAGMENT_SHADER, (char*)"kernel/opengl/vglCopy.frag");
    if (!f){
      fprintf(stderr, "%s: %s: Error loading fragment shader.\n", __FILE__, __FUNCTION__);
      exit(1);
   }
  }
  ERRCHECK()

  glUseProgram(f);

  glUniform2f(glGetUniformLocation(f, (char*)"tex_size"), src->ipl->width, src->ipl->height);

  glUniform1f(glGetUniformLocation(f, (char*)"width"), src->ipl->width);
  glUniform1f(glGetUniformLocation(f, (char*)"height"), src->ipl->height);

  ERRCHECK()

    //printf("Using fragment shader = %d.\n", f);

	glViewport(0, 0, 2*dst->getWidth(), 2*dst->getHeight());

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();
  glUseProgram(0);

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Copy data from src texture to dst texture using a
    fragment shader and a vertex shader
 */
void vglCopyImageTexVFS(VglImage* src, VglImage* dst)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src, VGL_GL_CONTEXT);

      glBindTexture(GL_TEXTURE_2D, src->tex);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
      ERRCHECK()


  glUseProgram(0);

  static GLuint p = 0;
  if (p == 0){
    fprintf(stdout, "VERTEX SHADER\n====================\n");
    p = vglShaderLoad(GL_VERTEX_SHADER, (char*)"kernel/util/vglPassThrough.vert");
    if (!p){
      fprintf(stderr, "%s: %s: Error loading vertex shader.\n", __FILE__, __FUNCTION__);
      exit(1);
    }
    ERRCHECK()

    fprintf(stdout, "FRAGMENT SHADER\n====================\n");
    p = vglShaderLoad(GL_FRAGMENT_SHADER, (char*)"kernel/opengl/vglNot.frag");
    if (!p){
      fprintf(stderr, "%s: %s: Error loading fragment shader.\n", __FILE__, __FUNCTION__);
      exit(1);
    }
    ERRCHECK()
  }

  glUseProgram(p);

  /*
  int x = glGetUniformLocation(p, "tex_size");
  printf("uniform location of tex_size = %d\n", x);
  ERRCHECK()

  glUniform2f(glGetUniformLocation(p, "tex_size"), src->ipl->width, src->ipl->height);
  ERRCHECK()

  glUniform1f(glGetUniformLocation(p, "width"), src->ipl->width);
  ERRCHECK()

  glUniform1f(glGetUniformLocation(p, "height"), src->ipl->height);
  ERRCHECK()
  */
      int retval;
      glGetIntegerv(GL_CURRENT_PROGRAM, &retval);
      //printf("current program = %d\n", retval);

  glViewport(0, 0, 2*dst->getWidth(), 2*dst->getHeight());

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();
  glUseProgram(0);

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  vglSetContext(dst, VGL_GL_CONTEXT);
}

// NOTE: Seems to be a personal test function, probably unutil
/*
void vglAxis(void){

  glLineWidth(3.0);

  glColor4f(0.1, 0.1, 1.0, 0.1);
  glBegin(GL_LINES);
    glVertex3f( 0.0,  0.0,  0.0);
    glVertex3f( 0.0,  0.0, 99.0);
  glEnd();

  glColor4f(1.0, 0.1, 0.1, 0.1);
  glBegin(GL_LINES);
    glVertex3f( 0.0,  0.0,  0.0);
    glVertex3f(99.0,  0.0,  0.0);
  glEnd();

  glColor4f(0.1, 1.0, 0.1, 0.1);
  glBegin(GL_LINES);
    glVertex3f( 0.0,  0.0,  0.0);
    glVertex3f( 0.0, 99.0,  0.0);
  glEnd();

  glColor4f(1.0, 1.0, 1.0, 0.0);
}
*/

// NOTE: Seems to be a personal test function, probably unutil
/*
void vglGreen(VglImage* src, VglImage* dst){

  vglCheckContext(src, VGL_GL_CONTEXT);

      glBindTexture(GL_TEXTURE_2D, src->tex);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
      ERRCHECK()

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();

  vglSetContext(dst, VGL_GL_CONTEXT);
}
*/

/** Flip image vertically, that is, top becomes bottom.

    Image flip done by texture mapping, that is, by the fixed pipeline.
 */
void vglVerticalFlip2(VglImage* src, VglImage* dst){
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src, VGL_GL_CONTEXT);

      glBindTexture(GL_TEXTURE_2D, src->tex);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
      ERRCHECK()

  glViewport(0, 0, 2*dst->getWidth(), 2*dst->getHeight());

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Flip image horizontally, that is, left becomes right.

    Image flip done by texture mapping, that is, by the fixed pipeline.
 */
void vglHorizontalFlip2(VglImage* src, VglImage* dst){
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src, VGL_GL_CONTEXT);

      glBindTexture(GL_TEXTURE_2D, src->tex);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
      ERRCHECK()

  glViewport(0, 0, 2*dst->getWidth(), 2*dst->getHeight());

      glBegin(GL_QUADS);
          glTexCoord2f( 1.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 0.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 0.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 1.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  vglSetContext(dst, VGL_GL_CONTEXT);
}

void vglClear(VglImage* image, float r, float g, float b, float a){
  vglCheckContext(image, VGL_GL_CONTEXT);

  vglPrintImageInfo(image);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, image->fbo);
  ERRCHECK()

  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);

  vglSetContext(image, VGL_GL_CONTEXT);
}


/** Morphological opening by square structuring element. Opening is an
    erosion followed by a dilation. A buffer is required. Source
    and destination may be the same.

    The structuring element is a 3x3 square. The parameter
    "times" indicates how many times the erosion will be applied.

*/
void vglOpenSq3(VglImage* src, VglImage* dst, VglImage* buf, int times){
  int i;
  int in_buf, in_dst;

  vglCheckContext(src, VGL_GL_CONTEXT);

  vglErodeSq3(src, buf);
  in_buf = 1;
  for (i = 1; i < times; i++){
    if (in_buf){
      vglErodeSq3(buf, dst);
      in_dst = 1; in_buf = 0;
    }
    else{
      vglErodeSq3(dst, buf);
      in_dst = 0; in_buf = 1;
    }
  }
  for (i = 0; i < times; i++){
    if (in_buf){
      vglDilateSq3(buf, dst);
      in_dst = 1; in_buf = 0;
    }
    else{
      vglDilateSq3(dst, buf);
      in_dst = 0; in_buf = 1;
    }
  }

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Morphological closing by square structuring element. A buffer
    is required. Source and destination may be the same.

    The structuring element is a 3x3 square. The parameter
    "times" indicates how many times the closing will be applied.

*/
void vglCloseSq3(VglImage* src, VglImage* dst, VglImage* buf, int times){
  int i;
  int in_buf, in_dst = 0;

  vglCheckContext(src, VGL_GL_CONTEXT);

  vglDilateSq3(src, buf);
  in_buf = 1;
  for (i = 1; i < times; i++){
    if (in_buf){
      vglDilateSq3(buf, dst);
      in_dst = 1; in_buf = 0;
    }
    else{
      vglDilateSq3(dst, buf);
      in_dst = 0; in_buf = 1;
    }
  }
  for (i = 0; i < times; i++){
    if (in_buf){
      vglErodeSq3(buf, dst);
      in_dst = 1; in_buf = 0;
    }
    else{
      vglErodeSq3(dst, buf);
      in_dst = 0; in_buf = 1;
    }
  }

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Morphological erosion by square structuring element 3x3. A buffer is
    required. Source and destination may be the same.

    The structuring element is a 3x3 square. The parameter
    "times" indicates how many times the erosion will be applied.

*/
void vglErodeSq3Sep(VglImage* src, VglImage* dst, VglImage* buf, int times){
  int i;
  int in_buf, in_dst;

  vglCheckContext(src, VGL_GL_CONTEXT);

  vglErodeHL3(src, buf);
  vglErodeVL3(buf, dst);
  for (i = 1; i < times; i++){
    vglErodeHL3(dst, buf);
    vglErodeVL3(buf, dst);
  }

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Morphological erosion by square structuring element 5x5. A buffer is
    required. Source and destination may be the same.

    The structuring element is a 5x5 square. The parameter
    "times" indicates how many times the erosion will be applied.

*/
void vglErodeSq5Sep(VglImage* src, VglImage* dst, VglImage* buf, int times){
  int i;
  int in_buf, in_dst;

  vglCheckContext(src, VGL_GL_CONTEXT);

  vglErodeHL5(src, buf);
  vglErodeVL5(buf, dst);
  for (i = 1; i < times; i++){
    vglErodeHL5(dst, buf);
    vglErodeVL5(buf, dst);
  }

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Morphological conditional erosion by cross structuring
    element 3x3. A buffer is required. Source and destination
    may be the same.

    The structuring element is a 3x3 cross. The parameter
    "times" indicates how many times the erosion will be applied.

*/
void vglCErodeCross3(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buf, int times){
  int i;

  vglCheckContext(src, VGL_GL_CONTEXT);
  vglCheckContext(mask, VGL_GL_CONTEXT);

  vglOr(src, mask, dst);
  for (i = 0; i < times; i++){
    vglErodeCross3(dst, buf);
    vglOr(buf, mask, dst);
  }

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Distance transform given by elementary cross.

    Perform successive erosions on input image thresholded to 1/256. The sum of
the erosions results is returned as the distance transform result.

  */
void vglDistTransformCross3(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times /*=1*/){
    vglThresh(src, buf, 0.0, 1.0/256.0);
    vglClear(dst, 0.0, 0.0, 0.0);
    for(int i = 0; i < times; i++){
      if (i % 2 == 0){
        vglErodeCross3(buf, buf2);
        vglSum(buf2, dst, dst);
      }
      else{
        vglErodeCross3(buf2, buf);
        vglSum(buf, dst, dst);
      }
    }
}

/** Distance transform given by square 3x3.

    Perform successive erorions on input image thresholded to 1/256. The sum of
the erosions results is returned as the distance transform result.

  */
void vglDistTransformSq3(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times /*=1*/){
    vglThresh(src, buf, 0.0, 1.0/256.0);
    vglClear(dst, 0.0, 0.0, 0.0);
    for(int i = 0; i < times; i++){
      if (i % 2 == 0){
        vglErodeSq3(buf, buf2);
        vglSum(buf2, dst, dst);
      }
      else{
        vglErodeSq3(buf2, buf);
        vglSum(buf, dst, dst);
      }
    }
}

/** Distance transform given by alternating an elementary cross and a square 3x3.

    Perform successive erorions on input image thresholded to 1/256. The sum of
the erosions results is returned as the distance transform result.

  */
void vglDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times /*=1*/){
    vglThresh(src, buf, 0.0, 1.0/256.0);
    vglClear(dst, 0.0, 0.0, 0.0);
    vglCopy(buf, dst);
    for(int i = 0; i < times; i++){
      if (i % 2 == 0){
        vglErodeSq3(buf, buf2);
        vglSum(buf2, dst, dst);
      }
      else{
        vglErodeCross3(buf2, buf);
        vglSum(buf, dst, dst);
      }
    }
}

/** Get level curve of distance transform5

    Perform successive erorions on input image thresholded to 1/256.
The returned image is the difference between the results obtained in
the iterations "times" and "times" - 1.

  */
void vglGetLevelDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times /*=1*/){
    //vglClear(dst, 0.0, 0.0, 0.0);
    vglCopy(src, buf);
    int i;
    for(i = 0; i <= times; i++){
      if (i % 2 == 0){
        vglErodeSq3(buf, buf2);
      }
      else{
        vglErodeCross3(buf2, buf);
      }
    }
    vglAbsDiff(buf, buf2, dst);
}

/** Structuring element thinning. Algorithm by Bernard and Manzanera 1999.

    Receive as input the image to be thinned. The second image is an auxiliary
image. The third image stores the result. Both the second and third images must
have the same size and type as the first input image.

    The fourth parameter is the number of iterations.

    Reference:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    T. M. Bernard and A. Manzanera, Improved low complexity fully parallel
        thinning algorithms, 1999

  */
void vglThinBernard(VglImage* src, VglImage* dst, VglImage* buf, int times /*=1*/){
    for(int i = 0; i < times; i++){
      vglErodeCross3(src, buf);
      vglThinBernardAux(src, buf, dst);
      vglCopy(dst, src);
    }
}

/** Structuring element thinning. Algorithm by Chin, Wan
Stover and Iverson, 1987.

    Receive as input the image to be thinned, buffer image and number
of times to iterate.

    Neighborhood pixels are indexed as follows:

\f$
\begin{array}{ccc}

    P3  &  P2  &  P1    \\
    P4  &  P8  &  P0    \\
    P5  &  P6  &  P7

\end{array}
\f$

    Reference:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    R. T. Chin et al., A one-pass thinning algorithm and its parallel
        implementation, 1987
  */
void vglThinChin(VglImage* src, VglImage* dst, VglImage* buf, int times /*=1*/){
    vglCopy(src, buf);
    for(int i = 0; i < times; i++){
      vglThinChinAux(buf, dst);
      vglCopy(dst, buf);
    }
}

/** Calculates baricenter of vga image.

    The resuld is stored in image RGB with one pixel.
    R = M(0, 0) = sum(     f(x, y) )
    G = M(1, 0) = sum( x * f(x, y) )
    B = M(0, 1) = sum( y * f(x, y) )

    Reference:

    William K. Pratt, Digital Image Processing, Second Edition

  */
void vglBaricenterVga(VglImage* src, double* x_avg /*= NULL*/, double* y_avg /*= NULL*/, double* pix_count /*= NULL*/){
  static VglImage* img[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  int width  = 640;
  int height = 480;

  if(src->getWidth() != 640 || src->getHeight() != 480){
      fprintf(stderr, "%s: %s: Error: image must be 640x480.\n", __FILE__, __FUNCTION__);
  }

  if(img[1] == NULL){
    img[1]  = vglCreateImage(cvSize(width      , height      ), IPL_DEPTH_32F);
    img[2]  = vglCreateImage(cvSize(width / 5  , height      ), IPL_DEPTH_32F);
    img[3]  = vglCreateImage(cvSize(width / 20 , height      ), IPL_DEPTH_32F);
    img[4]  = vglCreateImage(cvSize(width / 20 , height / 5  ), IPL_DEPTH_32F);
    img[5]  = vglCreateImage(cvSize(width / 20 , height / 15 ), IPL_DEPTH_32F);
    img[6]  = vglCreateImage(cvSize(width / 40 , height / 30 ), IPL_DEPTH_32F);
    img[7]  = vglCreateImage(cvSize(width / 80 , height / 60 ), IPL_DEPTH_32F);
    img[8]  = vglCreateImage(cvSize(width / 160, height / 120), IPL_DEPTH_32F);
    img[9]  = vglCreateImage(cvSize(width / 320, height / 240), IPL_DEPTH_32F);
    img[10] = vglCreateImage(cvSize(width / 640, height / 480), IPL_DEPTH_32F);
  }

  vglBaricenterInit(src, img[1]);
  vglSelfSum5h(img[1], img[2]);
  vglSelfSum4h(img[2], img[3]);
  vglSelfSum5v(img[3], img[4]);
  vglSelfSum3v(img[4], img[5]);
  vglSelfSum22(img[5], img[6]);
  vglSelfSum22(img[6], img[7]);
  vglSelfSum22(img[7], img[8]);
  vglSelfSum22(img[8], img[9]);
  vglSelfSum22(img[9], img[10]);
  vglDownload(img[10]);

  float* data = (float*) img[10]->ipl->imageData;
  printf("MOMENTS = (%f, %f, %f)\n", data[0], data[1], data[2]);
  printf("(x, y)  = (%f, %f)\n", data[1]/data[2], data[0]/data[2]);
  if (x_avg != NULL){
    *x_avg = data[1]/data[2];
  }
  if (y_avg != NULL){
    *y_avg = data[0]/data[2];
  }
  if (pix_count != NULL){
    *pix_count = data[2];
  }

}

/** Test and model for IN_OUT semantics.

    First parameter is input and output. Second parameter is output.

  */
void vglInOut_model(VglImage*  dst, VglImage*  dst1){

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(dst, VGL_GL_CONTEXT);

  static GLuint f = 0;
  if (f == 0){
    fprintf(stdout, "FRAGMENT SHADER\n====================\n");
    f = vglShaderLoad(GL_FRAGMENT_SHADER, (char*)"FS/vglInOut.frag");
    if (!f){
      fprintf(stderr, "%s: %s: Error loading fragment shader.\n", __FILE__, __FUNCTION__);
      exit(1);
   }
  }
  ERRCHECK()

  glUseProgram(f);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, dst->tex);
  glUniform1i(glGetUniformLocation(f, "sampler0"),  0);
  ERRCHECK()

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
  CHECK_FRAMEBUFFER_STATUS()
  ERRCHECK()

    //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, dst->tex, 0);
    //CHECK_FRAMEBUFFER_STATUS()
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, dst1->tex, 0);
  CHECK_FRAMEBUFFER_STATUS()

  glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);

  GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
  glDrawBuffers(2, buffers);
  ERRCHECK()

    //glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
    ERRCHECK()

  glViewport(0, 0, 2*dst->getWidth(), 2*dst->getHeight());

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();
  glUseProgram(0);

  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  glPopAttrib();

  glActiveTexture(GL_TEXTURE0);

  vglSetContext(dst, VGL_GL_CONTEXT);
  vglSetContext(dst1, VGL_GL_CONTEXT);
}

/** Test and model for functions with multiple output images.

    First parameter is input. Second and third parameters are output.

  */
void vglMultiOutput_model(VglImage*  src, VglImage*  dst, VglImage*  dst1){

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src, VGL_GL_CONTEXT);

  static GLuint f = 0;
  if (f == 0){
    fprintf(stdout, "FRAGMENT SHADER\n====================\n");
    f = vglShaderLoad(GL_FRAGMENT_SHADER, (char*)"FS/vglMultiOutput.frag");
    if (!f){
      fprintf(stderr, "%s: %s: Error loading fragment shader.\n", __FILE__, __FUNCTION__);
      exit(1);
   }
  }
  ERRCHECK()

  glUseProgram(f);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, src->tex);
  glUniform1i(glGetUniformLocation(f, "sampler0"),  0);
  ERRCHECK()

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
  CHECK_FRAMEBUFFER_STATUS()
  ERRCHECK()

    //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, dst->tex, 0);
    //CHECK_FRAMEBUFFER_STATUS()
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, dst1->tex, 0);
  CHECK_FRAMEBUFFER_STATUS()

  glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);

  GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
  glDrawBuffers(2, buffers);
  ERRCHECK()

    //glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
    ERRCHECK()

  glViewport(0, 0, 2*dst->getWidth(), 2*dst->getHeight());

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();
  glUseProgram(0);

  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  glPopAttrib();

  glActiveTexture(GL_TEXTURE0);

  vglSetContext(dst, VGL_GL_CONTEXT);
  vglSetContext(dst1, VGL_GL_CONTEXT);
}

/** Test and model for functions with multiple input images.

    First and second parameters are input. Third parameter is output.

  */
void vglMultiInput_model(VglImage*  src0, VglImage*  src1, VglImage*  dst){

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src0, VGL_GL_CONTEXT);
  vglCheckContext(src1, VGL_GL_CONTEXT);

  static GLuint f = 0;
  if (f == 0){
    fprintf(stdout, "VERTEX SHADER\n====================\n");
    f = vglShaderLoad(GL_VERTEX_SHADER, (char*)"kernel/util/vglPassThrough.vert");
    if (!f){
      fprintf(stderr, "%s: %s: Error loading vertex shader.\n", __FILE__, __FUNCTION__);
      exit(1);
    }
    ERRCHECK()

    fprintf(stdout, "FRAGMENT SHADER\n====================\n");
    f = vglShaderLoad(GL_FRAGMENT_SHADER, (char*)"FS/vglMultiInput_model.frag");
    if (!f){
      fprintf(stderr, "%s: %s: Error loading fragment shader.\n", __FILE__, __FUNCTION__);
      exit(1);
   }
  }
  ERRCHECK()

    // funciona sem shader
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, src0->tex);

  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, src1->tex);
  // ate aqui

  glUseProgram(f);

  glActiveTexture(GL_TEXTURE1);
  //glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, src1->tex);

  glActiveTexture(GL_TEXTURE0);
  //glEnable(GL_TEXTURE_2D);
  //glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, src0->tex);

  glUniform1i(glGetUniformLocation(f, "sampler1"),  1);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
  ERRCHECK()


  ERRCHECK()

  glViewport(0, 0, 2*dst->getWidth(), 2*dst->getHeight());

      glBegin(GL_QUADS);
          glTexCoord2f( 0.3,  0.3);
          //glMultiTexCoord2f(GL_TEXTURE1, 0.0,  0.0);
          //glMultiTexCoord2f(GL_TEXTURE0, -0.2,  -0.2);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.3,  0.3);
          //glMultiTexCoord2f(GL_TEXTURE1, 1.0,  0.0);
          //glMultiTexCoord2f(GL_TEXTURE0, 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.3,  1.3);
          //glMultiTexCoord2f(GL_TEXTURE1, 1.0,  1.0);
          //glMultiTexCoord2f(GL_TEXTURE0, 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.3,  1.3);
          //glMultiTexCoord2f(GL_TEXTURE1, 0.0,  1.0);
          //glMultiTexCoord2f(GL_TEXTURE0, 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();
  glUseProgram(0);

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  glActiveTexture(GL_TEXTURE1);
  glDisable(GL_TEXTURE_2D);

  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);

  vglSetContext(dst, VGL_GL_CONTEXT);
}
