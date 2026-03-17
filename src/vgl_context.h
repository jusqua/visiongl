#ifndef VGL_CONTEXT_H
#define VGL_CONTEXT_H
#pragma once

#include <assert.h>
#include <GL/glew.h>
#include <CL/cl.h>

#include "vgl_image.h"

#define VGL_MAX_INFO_LOG_SIZE 1000
#define GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT 0x8CD8
#define GL_FRAMEBUFFER_STATUS_ERROR_EXT 0x8CDE

#define CHECK_FRAMEBUFFER_STATUS() \
{ \
 /*printf("CHECK_FRAMENBUFFER_STATUS\n");*/ \
 GLenum status; \
 status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
 switch(status) { \
 case GL_FRAMEBUFFER_COMPLETE_EXT: \
   break; \
 case GL_FRAMEBUFFER_UNSUPPORTED_EXT: \
   printf("framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n"); \
    /* you gotta choose different formats */ \
   assert(0); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: \
   printf("framebuffer INCOMPLETE_ATTACHMENT\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: \
   printf("framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: \
   printf("framebuffer FRAMEBUFFER_DIMENSIONS\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT: \
   printf("framebuffer INCOMPLETE_DUPLICATE_ATTACHMENT\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: \
   printf("framebuffer INCOMPLETE_FORMATS\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: \
   printf("framebuffer INCOMPLETE_DRAW_BUFFER\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: \
   printf("framebuffer INCOMPLETE_READ_BUFFER\n"); \
   break; \
 case GL_FRAMEBUFFER_BINDING_EXT: \
   printf("framebuffer BINDING_EXT\n"); \
   break; \
 case GL_FRAMEBUFFER_STATUS_ERROR_EXT: \
   printf("framebuffer STATUS_ERROR\n"); \
   break; \
 default: \
   printf("programming error; will fail on all hardware\n"); \
   assert(0); \
 } \
} \

#define ERRCHECK() \
{ \
  static GLenum errCode; \
  const GLubyte *errString; \
  if ((errCode = glGetError()) != GL_NO_ERROR) { \
    /*errString = gluErrorString(errCode);*/     \
    errString = glGetString(errCode); \
    printf ("OpenGL Error %x: %s at %s:%d\n", errCode, errString,  __FILE__,__LINE__); \
	exit(1); \
  } \
} \

GLuint vglShaderTypeIsOk(GLuint type);
GLuint vglShaderCreateIsOk(GLuint shader);
GLuint vglShaderCompileIsOk(GLuint shader);
GLuint vglShaderLinkIsOk(GLuint program);
GLuint vglShaderValidateIsOk(GLuint program);
void vglShaderFileReadStatus(const GLchar* text, char* filename);
GLchar *vglTextFileRead(char *fn) ;
GLuint vglSingleShaderLoad(GLuint ShaderType, char* filename);
GLuint vglShaderLoad(GLuint ShaderType, char* filename);
GLuint vglProgramLoad(char* v_filename, char* f_filename);

int vglInit();
int vglInit(int w, int h);
int vglHasDisplay();
void vglUpload(VglImage* image, int swapRGB = 0);
void vglDownload(VglImage* image);
void vglDownloadPPM(VglImage* image);
void vglDownloadPGM(VglImage* image);
void vglDownloadFBO(VglImage* image);
void vglDownloadFaster(VglImage* image/*, VglImage* buf*/);

int vglCudaToGl(VglImage* img);
int vglGlToCuda(VglImage* img);
int vglCudaAlloc(VglImage* img);
int vglCudaAllocPbo(VglImage* img);
int vglCudaFree(VglImage* img);
int vglCudaFreePbo(VglImage* img);
int vglCudaMapPbo(VglImage* img);
int vglCudaUnmapPbo(VglImage* img);

struct VglClContext
{
	cl_platform_id* platformId;
	cl_device_id* deviceId;
	cl_context context;
	cl_command_queue commandQueue;
};

void vglClInteropSetTrue(void);
void vglClInteropSetFalse(void);
void vglClPrintContext(void);
void vglClCheckError(cl_int error, char* name);
void vglClInit();
void vglClFlush();
void vglClBuildDebug(cl_int err, cl_program program);
void vglClUploadForce(VglImage* img);
void vglClDownloadForce(VglImage* img);
void vglClUpload(VglImage* img);
void vglClDownload(VglImage* img);
void vglClUploadInterop(VglImage* img);
void vglClDownloadInterop(VglImage* img);
void vglClToGl(VglImage* img);
void vglGlToCl(VglImage* img);
void vglClAlloc(VglImage* img);

// TODO: Bad function definition, find another way to make easier context switching
int vglCheckContext(VglImage* img, int context);
// int vglCheckContextForOutput(VglImage* img, int context); // NOTE: Seems unused

#endif // VGL_CONTEXT_H
