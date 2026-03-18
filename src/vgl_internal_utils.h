#ifndef INTERNAL_UTILS_H
#define INTERNAL_UTILS_H
#pragma once

typedef enum {
    VGL_LOG_LEVEL_NO_LOG,
    VGL_LOG_LEVEL_INFO,
    VGL_LOG_LEVEL_WARN,
    VGL_LOG_LEVEL_ERROR,
} vgl_log_level_t;

typedef enum {
    VGL_LOG_INFO = VGL_LOG_LEVEL_INFO,
    VGL_LOG_WARN = VGL_LOG_LEVEL_WARN,
    VGL_LOG_ERROR = VGL_LOG_LEVEL_ERROR,
} vgl_log_kind_t;

void vgl_panic(const char* format, ...);
void vgl_log(vgl_log_kind_t level, const char* message, ...);

#endif // INTERNAL_UTILS_H
