#ifndef VGL_IMAGE_IO_H
#define VGL_IMAGE_IO_H

#include "vgl_image.h"

#ifndef MAX_IO_BUFFER_SIZE
#   define MAX_IO_BUFFER_SIZE 256
#endif // MAX_IO_BUFFER_SIZE
_Static_assert(
    MAX_IO_BUFFER_SIZE >= 256,
    "MAX_IO_BUFFER_SIZE is too low"
);

#ifndef MAX_IO_CURSOR_ROLLBACK_SIZE
#   define MAX_IO_CURSOR_ROLLBACK_SIZE 32
#endif // MAX_IO_CURSOR_ROLLBACK_SIZE
_Static_assert(
    MAX_IO_CURSOR_ROLLBACK_SIZE >= 32 && MAX_IO_CURSOR_ROLLBACK_SIZE < MAX_IO_BUFFER_SIZE / 2,
    "MAX_IO_CURSOR_ROLLBACK_SIZE must be lower than MAX_IO_BUFFER_SIZE / 2"
);

/// IO module operations
typedef struct {
    const char* exts;                                ///< File valid extensions
    int (*verify)(void* fd);                         ///< Verify if file is valid
    int (*load)(void* fd, vgl_image_t* image);       ///< Load image data from file descriptor
    int (*save)(void* fd, const vgl_image_t* image); ///< Save image data to file descriptor
} io_module_t;

int register_io_module(io_module_t* module);

#endif // VGL_IMAGE_IO_H
