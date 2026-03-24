#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "vgl_internal_utils.h"

#ifndef VGL_LOG_LEVEL
#   ifdef NDEBUG
#       define VGL_LOG_LEVEL VGL_LOG_LEVEL_NO_LOG
#   else
#       define VGL_LOG_LEVEL VGL_LOG_LEVEL_ERROR
#   endif // NDEBUG
#endif // VGL_LOG_LEVEL

_Static_assert(VGL_LOG_LEVEL >= VGL_LOG_LEVEL_NO_LOG && VGL_LOG_LEVEL < __VGL_LOG_LEVEL_COUNT, "Invalid log level");

void vgl_panic(const char* format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "[PANIC] %s:%d: ", __FILE__, __LINE__);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
    abort();
}

void vgl_log(vgl_log_kind_e level, const char* format, ...) {
    switch (level) {
    case VGL_LOG_INFO: {
        if (VGL_LOG_LEVEL >= VGL_LOG_LEVEL_INFO) return;
        fprintf(stderr, "[INFO]  ");
    } break;
    case VGL_LOG_WARN: {
        if (VGL_LOG_LEVEL >= VGL_LOG_LEVEL_WARN) return;
        fprintf(stderr, "[WARN]  ");
    } break;
    case VGL_LOG_ERROR: {
        if (VGL_LOG_LEVEL >= VGL_LOG_LEVEL_ERROR) return;
        fprintf(stderr, "[ERROR] ");
    } break;
    default:
        vgl_panic("Invalid log kind");
        return;
    break;
    }

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}
