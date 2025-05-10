#ifndef VISIONGL_VGLCLIMAGE_HPP
#define VISIONGL_VGLCLIMAGE_HPP

#ifdef __OPENCL__

#include <CL/cl.h>

#include <visiongl/vglImage.hpp>

struct VglClContext {
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

#endif  // __OPENCL__

#endif  // VISIONGL_VGLCLIMAGE_HPP
