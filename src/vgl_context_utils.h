#ifndef VGL_CONTEXT_UTILS_H
#define VGL_CONTEXT_UTILS_H
#pragma once

#include "vgl_image.h"

// TODO: Bad function definition, find another way to make easier context switching
int vglCheckContext(VglImage* img, int context);
// int vglCheckContextForOutput(VglImage* img, int context); // NOTE: Seems unused

#endif // VGL_CONTEXT_UTILS_H
