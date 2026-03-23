#ifndef INTERNAL_UTILS_H
#define INTERNAL_UTILS_H
#pragma once

typedef enum {
    VGL_LOG_LEVEL_NO_LOG = -1,
    VGL_LOG_LEVEL_ERROR,
    VGL_LOG_LEVEL_WARN,
    VGL_LOG_LEVEL_INFO,
    __VGL_LOG_LEVEL_COUNT
} vgl_log_level_e;

typedef enum {
    VGL_LOG_ERROR,
    VGL_LOG_WARN,
    VGL_LOG_INFO,
    __VGL_LOG_KIND_COUNT
} vgl_log_kind_e;

void vgl_panic(const char* format, ...);
void vgl_log(vgl_log_kind_e level, const char* message, ...);

#endif // INTERNAL_UTILS_H
