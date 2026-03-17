#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "vgl_context.h"

/** Initialize GLUT and create output window with default size (1280, 960).
  */
int vglInit()
{
  return vglInit(1280, 960);
}

/** Initialize GLUT and create output window with size (w, h).
  */
int vglInit(int w, int h)
{
    char* argv = new char[255];
    int argc = 0;
    int window_id = 0;

    int hasDisplay = vglHasDisplay();
    if (!hasDisplay)
    {
      fprintf(stdout, "%s: %s: Warning: No display available. You should not call vglInit from a machine without X or with $DISPLAY variable not set. You can try to set DISPLAY environment variable to a suitable value. You can also call vglClInit and use OpenCL functions.\n", __FILE__, __FUNCTION__);
    }

    int glut_time = glutGet(GLUT_ELAPSED_TIME);
    printf("Glut elapsed time = %dms\n", glut_time);
    static int started = 0;
    if(!started)
    {
      glutInit(&argc, &argv);
      glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
      glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
      glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
      glutInitWindowSize(w, h);
      if (w < 10 && h < 10)
      {
        glutInitWindowPosition(-50, -50);
      }
      else
      {
        glutInitWindowPosition(-50, -50);
      }
      window_id = glutCreateWindow("Main window");

      glEnable(GL_TEXTURE_2D); //deve ficar depois de glutCreateWindow

      GLenum err = glewInit();
      if (GLEW_VERSION_1_3)
      {
        if(!glewIsSupported("GL_EXT_framebuffer_object"))
          fprintf(stderr, "%s: %s: glGenFramebuffersEXT not supported. The program may not work.\n", __FILE__, __FUNCTION__);
      }
      else
      {
        fprintf(stderr, "%s: %s: OpenGL 1.3 not supported. The program may not work.\n", __FILE__, __FUNCTION__);
      }

      started = 1;

      //glutDisplayFunc(display);
      //glutIdleFunc(display);
      //glutReshapeFunc(reshape);
      //glutKeyboardFunc(keyboard);
    }
    return window_id;
}

/** Test if there is display available.

    Test if there is display available by checking existence of
environment variable DISPLAY.
 */
int vglHasDisplay()
{
#ifdef __linux__
  char* pDisplay;
  pDisplay = getenv ("DISPLAY");
  if (pDisplay == NULL)
  {
    printf ("No display available.\n");
    return 0;
  }
  printf ("The current display is: %s\n", pDisplay);
  return 1;
#elif defined WIN32
  printf ("Check not implemented for Windows. Assuming that display is available.\n");
  return 1;
#endif
}

/** Send image data from RAM to GPU. Add GPU as valid context.

    If swapRGB is true, channels R and B are swapped.
  */
void vglUpload(VglImage* image, int swapRGB){
  IplImage* ipl = image->ipl;
  const int LEVEL = 0;
  GLenum glFormat;
  GLenum glTarget;
  GLenum glType;
  GLenum internalFormat;
  int depth      = image->depth;
  int nChannels  = image->nChannels;
  int ndim       = image->ndim;
  int has_mipmap = image->has_mipmap;


  //printf("vglUpload: image context = %d\n", image->inContext);
  if (!vglIsInContext(image, VGL_RAM_CONTEXT)  &&
      !vglIsInContext(image, VGL_BLANK_CONTEXT)    ){
#ifdef __DEBUG__
    fprintf(stderr, "vglUpload: Error: image context = %d not in VGL_RAM_CONTEXT or VGL_BLANK_CONTEXT\n", image->inContext);
#endif
    return;
  }

  if (nChannels == 3){
        //swapRGB = (swapRGB + 1) % 2;
	if(ndim == 3)
	{
		vglNdarray3To4Channels(image);
	}
	else
	{
		/*IplImage* iplRGBA = cvCreateImage(cvGetSize(image->ipl), depth, 4);
		printf("\t\t\t    ipl->nChannels = %d\n", image->ipl->nChannels);
		printf("\t\t\tiplRGBA->nChannels = %d\n", iplRGBA->nChannels);
		cvCvtColor(image->ipl, iplRGBA, CV_BGR2RGBA);
		cvReleaseImage(&(image->ipl));
		free(image->ipl);
		image->ipl = iplRGBA;
		image->nChannels = 4;
		printf("\t\t\tipl->nChannels = %d\n", image->ipl->nChannels);
		nChannels = 4;*/
	}
  }

  if (nChannels == 3){
    if (swapRGB){
      glFormat = GL_BGR;
    }
    else{
      glFormat = GL_RGB;
    }
  }
  else if (nChannels == 4)
  {
    if (swapRGB){
      glFormat = GL_BGRA;
    }
    else{
      glFormat = GL_RGBA;
    }
  }
  else{
    glFormat = GL_LUMINANCE;
  }

  //vglInit(1, 1);
  if (image->tex == -1){
    glGenTextures(1, &image->tex);
  }

  if (depth != IPL_DEPTH_8U){
    fprintf(stderr, "%s: %s: Pixel type not supported. Must be IPL_DEPTH_8U\n", __FILE__, __FUNCTION__);
  }

  if (ndim <= 0){
    fprintf(stderr, "vglUpload: Image ndim (depth) must be greater than zero. Assuming ndim = 2\n");
    ndim =2;
  }

  if (ndim == 2){
    glTarget = GL_TEXTURE_2D;
  }
  else if (ndim == 3){
    glTarget = GL_TEXTURE_3D;
  }
  else {
    fprintf(stderr, "%s: %s: Error: images with more than 3 dimensions not supported\n", __FILE__, __FUNCTION__);
  }

  //printf("%s:%s: w x h x d = %d x %d x %d\n", __FILE__, __FUNCTION__, image->shape[0], image->shape[1], image->shape[2]);

  glBindTexture(glTarget, image->tex);
  glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);

  ERRCHECK()

  // MIPMAPPING!!!!
  if (has_mipmap){
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  }
  ERRCHECK()

  switch (depth){
          case IPL_DEPTH_8U:  glType = GL_UNSIGNED_BYTE;  break;
          case IPL_DEPTH_16U: glType = GL_UNSIGNED_SHORT; break;
          case IPL_DEPTH_32F: glType = GL_FLOAT; break;
          case IPL_DEPTH_8S:  glType = GL_BYTE; exit(1);  break;
          case IPL_DEPTH_16S: glType = GL_SHORT; exit(1); break;
          case IPL_DEPTH_32S: glType = GL_INT; break;
          case IPL_DEPTH_1U:
          default:
            fprintf(stderr, "vglUpload: Error: uploading unsupported image depth\n");
            vglPrintImageInfo(image);
            exit(1);
  }

  if (nChannels >= 3){
    if (glType == GL_FLOAT){
        internalFormat = GL_RGBA32F_ARB;
    }
    else{
      internalFormat = GL_RGBA8;
    }
  }
  else{
        internalFormat = GL_INTENSITY; //Must be fixed, but for now, it's the fix.
  }

  if (ndim == 3){
    glTexImage3D(glTarget, LEVEL, internalFormat,
                 image->getWidth(), image->getHeight(), image->getLength(), 0,
                 glFormat, glType, image->ndarray);
  }
  else{
    glTexImage2D(glTarget, LEVEL, internalFormat,
                 ipl->width, ipl->height, 0,
                 glFormat, glType, ipl->imageData);
  }

  //printf("%s:%s: checking framebuffer...\n", __FILE__, __FUNCTION__);

  CHECK_FRAMEBUFFER_STATUS()
  ERRCHECK()

  if (image->fbo == -1){
    glGenFramebuffersEXT(1, &image->fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, image->fbo);
    if (glTarget == GL_TEXTURE_3D){
      int layer = 0; // will render only to layer 0
      glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,
				glTarget, image->tex, 0, layer);
    }
    else{
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,
                              glTarget, image->tex, 0);
    }
    CHECK_FRAMEBUFFER_STATUS()
    ERRCHECK()
  }

  if (vglIsInContext(image, VGL_RAM_CONTEXT)){
    vglAddContext(image, VGL_GL_CONTEXT);
  }

}

/** Force transfer of image from GPU to RAM. Add RAM as valid context.

    Transfer done by glGetTexImage. Color order is compatible with iplImage, that is, BGR.

    Time to transfer a VGA image = 2.5ms
 */
void vglDownload(VglImage* image){
  IplImage* ipl = image->ipl;
  GLenum glFormat;
  GLenum glType;
  int ndim       = image->ndim;
  int depth      = image->depth;
  int nChannels  = image->nChannels;

  //glPixelStorei(GL_PACK_ALIGNMENT, 4);


  switch (depth){
          case IPL_DEPTH_8U:  glType = GL_UNSIGNED_BYTE;  break;
          case IPL_DEPTH_16U: glType = GL_UNSIGNED_SHORT; break;
          case IPL_DEPTH_32F: glType = GL_FLOAT; break;
          case IPL_DEPTH_8S:  glType = GL_BYTE;  break;
          case IPL_DEPTH_16S: glType = GL_SHORT; break;
          case IPL_DEPTH_32S: glType = GL_INT; break;
          case IPL_DEPTH_1U:
          default:
            fprintf(stderr, "vglDownload: Error: downloading unsupported image depth\n");
            vglPrintImageInfo(image);
            exit(1);
  }

  ERRCHECK()

  if (nChannels == 3)
  {
    glFormat = GL_RGB;
  }
  else if (nChannels == 4)
  {
    glFormat = GL_RGBA;
  }
  else
  {
    glFormat = GL_LUMINANCE;
  }

  if (ndim == 2){
    glBindTexture(GL_TEXTURE_2D, image->tex);
    ERRCHECK()
    glGetTexImage(GL_TEXTURE_2D, 0, glFormat, glType, ipl->imageData);
    ERRCHECK()
  }
  else if (ndim == 3){
    glBindTexture(GL_TEXTURE_3D, image->tex);
    ERRCHECK()
    glGetTexImage(GL_TEXTURE_3D, 0, glFormat, glType, image->ndarray);
    ERRCHECK()
  }
  else {
    fprintf(stderr, "%s: %s: Error: images with more than 3 dimensions not supported\n", __FILE__, __FUNCTION__);
  }


  //glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, ipl->imageData);

  ERRCHECK()

  vglAddContext(image, VGL_RAM_CONTEXT);
}

/** Force transfer of image from FBO to RAM. Add RAM as valid context.

    Transfer done by glReadPixels. Color order is compatible with iplImage, that is, BGR.

 */
void vglDownloadFBO(VglImage* image){
  IplImage* ipl = image->ipl;
  GLenum glFormat;
  GLenum glType;
  int depth = image->depth;
  int nChannels = image->nChannels;

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, image->fbo);

  switch (depth){
          case IPL_DEPTH_8U:  glType = GL_UNSIGNED_BYTE;  break;
          case IPL_DEPTH_16U: glType = GL_UNSIGNED_SHORT; break;
          case IPL_DEPTH_32F: glType = GL_FLOAT; break;
          case IPL_DEPTH_8S:  glType = GL_BYTE;  break;
          case IPL_DEPTH_16S: glType = GL_SHORT; break;
          case IPL_DEPTH_32S: glType = GL_INT; break;
          case IPL_DEPTH_1U:
          default:
            fprintf(stderr, "vglDownload: Error: downloading unsupported image depth\n");
            vglPrintImageInfo(image);
            exit(1);
  }

  if (nChannels == 3)
  {
    glFormat = GL_BGR;
  }
  else if (nChannels == 4)
  {
    glFormat = GL_RGB;
  }
  else
  {
    glFormat = GL_LUMINANCE;
  }

  glReadPixels(0, 0, image->getWidth(), image->getHeight(), glFormat, glType, image->ipl->imageData);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  vglAddContext(image, VGL_RAM_CONTEXT);
}

/** Transfer image from GPU to RAM in format suitable for saving as PPM.

    Use it imediately before vglSavePPM. It is different from
    vglDownload in two points. The unpack alignment is 1 and
    color order is RGB

    Time to transfer a VGA image = 3ms
 */
void vglDownloadPPM(VglImage* image){
  IplImage* ipl = image->ipl;
  GLint pack;

  glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  ERRCHECK()

  glBindTexture(GL_TEXTURE_2D, image->tex);
  ERRCHECK()
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, ipl->imageData);
  ERRCHECK()

  glPixelStorei(GL_PACK_ALIGNMENT, pack);
  ERRCHECK()

  //This image can not be uploaded correctly and call no vglAddContext.
  //vglAddContext(image, VGL_RAM_CONTEXT);
}

/** Transfer image from GPU to RAM in format suitable for saving as PGM.

    Use it imediately before vglSavePGM. It is different from
    vglDownload in two points. The unpack alignment is 1 and
    color is grayscale

    Time to transfer a VGA image = 10ms
 */
void vglDownloadPGM(VglImage* image){
  IplImage* ipl = image->ipl;
  GLint pack;

  glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  ERRCHECK()

  glBindTexture(GL_TEXTURE_2D, image->tex);
  ERRCHECK()
  glGetTexImage(GL_TEXTURE_2D, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, ipl->imageData);
  ERRCHECK()

  glPixelStorei(GL_PACK_ALIGNMENT, pack);
  ERRCHECK()

  //This image can not be uploaded correctly and call no vglAddContext.
  //vglAddContext(image, VGL_RAM_CONTEXT);
}

/** Force transfer of image from GPU to RAM. Add RAM as valid context.

    Transfer done by glReadPixels. Color order is compatible with iplImage, that is, BGR.

    Time to transfer a VGA image = 1.0 to 1.5ms
 */
void vglDownloadFaster(VglImage* image/*, VglImage* aux*/){
  IplImage* ipl = image->ipl;
  GLenum glFormat;
  GLenum glType;
  int depth     = image->depth;
  int nChannels = image->nChannels;

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  if (!vglIsInContext(image, VGL_GL_CONTEXT)){
    fprintf(stderr, "vglUpload: Error: image context = %d is not in VGL_GL_CONTEXT\n", image->inContext);
    return;
  }

  glBindTexture(GL_TEXTURE_2D, image->tex);
  //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

  // New version ddantas 5/2/3009
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, image->fbo);
  glViewport(0, 0, 2*image->getWidth(), 2*image->getHeight());
  // Old version needs aux image
  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, aux->fbo);
  //glViewport(0, 0, 2*aux->width, 2*aux->height);

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

  switch (depth){
          case IPL_DEPTH_8U:  glType = GL_UNSIGNED_BYTE;  break;
          case IPL_DEPTH_16U: glType = GL_UNSIGNED_SHORT; break;
          case IPL_DEPTH_32F: glType = GL_FLOAT; break;
          case IPL_DEPTH_8S:  glType = GL_BYTE;  break;
          case IPL_DEPTH_16S: glType = GL_SHORT; break;
          case IPL_DEPTH_32S: glType = GL_INT; break;
          case IPL_DEPTH_1U:
          default:
            fprintf(stderr, "vglDownload: Error: downloading unsupported image depth\n");
            vglPrintImageInfo(image);
            exit(1);
  }

  ERRCHECK()

  if (nChannels == 3)
  {
    glFormat = GL_RGB;
  }
  else if (nChannels == 4)
  {
    glFormat = GL_RGBA;
  }
  else
  {
    glFormat = GL_LUMINANCE;
  }

  glReadPixels(0,0,ipl->width,ipl->height, glFormat, glType, ipl->imageData);

  ERRCHECK()

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  vglAddContext(image, VGL_RAM_CONTEXT);
}

GLuint vglShaderTypeIsOk(GLuint type){
  switch (type){
  case GL_VERTEX_SHADER:
  case GL_FRAGMENT_SHADER:
    fprintf(stderr, "Shader type OK.\n");
    return 1;
    break;
  }
  fprintf(stderr, "Error: invalid shader type.\n");
  return 0;
}

GLuint vglShaderCreateIsOk(GLuint shader){
  if (shader){
    fprintf(stderr, "Shader creation OK.\n");
  }
  else{
    GLchar infoLog[VGL_MAX_INFO_LOG_SIZE];
    fprintf(stderr, "Error in shader creation|\n");
  }
  return shader;
}

GLuint vglShaderCompileIsOk(GLuint shader){
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success){
    fprintf(stderr, "Shader compilation OK.\n");
  }
  else{
    GLchar infoLog[VGL_MAX_INFO_LOG_SIZE];
    glGetShaderInfoLog(shader, VGL_MAX_INFO_LOG_SIZE, NULL, infoLog);
    fprintf(stderr, "Error in shader compilation:\n");
    fprintf(stderr, "%s\n", infoLog);
  }
  return success;
}

GLuint vglShaderLinkIsOk(GLuint program){
  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success){
    fprintf(stderr, "Link status OK.\n");
  }
  else{
    GLchar infoLog[VGL_MAX_INFO_LOG_SIZE];
    glGetProgramInfoLog(program, VGL_MAX_INFO_LOG_SIZE, NULL, infoLog);
    fprintf(stderr, "Error in program linking:\n");
    fprintf(stderr, "%s\n", infoLog);
  }
  return success;
}

GLuint vglShaderValidateIsOk(GLuint program){
  GLint success;
  glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
  if (success){
    fprintf(stderr, "Validate status OK.\n");
  }
  else{
    GLchar infoLog[VGL_MAX_INFO_LOG_SIZE];
    glGetProgramInfoLog(program, VGL_MAX_INFO_LOG_SIZE, NULL, infoLog);
    fprintf(stderr, "Error in program validation:\n");
    fprintf(stderr, "%s\n", infoLog);
  }
  return success;
}

void vglShaderFileReadStatus(const GLchar* text, char* filename){
  if (text){
    fprintf(stderr, "Shader source file read OK.\n");
  }
  else{
    fprintf(stderr, "Error reading shader source file:");
    if (filename){
      fprintf(stderr, " %s", filename);
    }
    else{
      fprintf(stderr, " filename is null");
    }
    fprintf(stderr, "\n");
  }
}

GLchar *vglTextFileRead(char *fn) {
  FILE *fp;
  GLchar *content = NULL;
  int f, count;

  if (fn != NULL) {
    fp = fopen(fn,"rt");
    if (fp != NULL) {

      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      if (count > 0) {
        content = (GLchar *)malloc(sizeof(GLchar) * (count+1));
        count = fread(content,sizeof(GLchar),count,fp);
        content[count] = '\0';
      }
      fclose(fp);
    }
  }
  return content;
}

GLuint vglShaderLoad(GLuint ShaderType, char* filename){
  GLint retval = 0;

  GLuint s;
  GLuint success;
  GLint p;

  glGetIntegerv(GL_CURRENT_PROGRAM, &p);
  printf("current program = %d\n", p);

  if (vglShaderTypeIsOk(ShaderType)){
    s = glCreateShader(ShaderType);
  }

  if (vglShaderCreateIsOk (s)){
    const GLchar* src = vglTextFileRead(filename);

    vglShaderFileReadStatus(src, filename);

    printf("shader code = %s\n", src);
    if (src){
      const GLint len = strlen(src);
      printf("code length= %d\n", len);

      glShaderSource(s, 1, &src, &len);

      glCompileShader(s);
      success = vglShaderCompileIsOk(s);
      if (!success) return 0;

      if (p){
        printf("Will use existing program\n");
      }
      else{
        printf("Creating new program\n");
        p = glCreateProgram();
      }

      printf("Attaching shader\n");
      glAttachShader(p, s);

      glLinkProgram(p);
      success = vglShaderLinkIsOk(p);
      if (!success) return 0;

      glValidateProgram(p);
      success = vglShaderValidateIsOk(p);
      if (!success) return 0;

      glUseProgram(p);

      glGetIntegerv(GL_CURRENT_PROGRAM, &retval);
      printf("current program = %d\n", retval);

      glDeleteShader(s);
    }
    else{
      return 0;
    }
  }
  return p;
}
