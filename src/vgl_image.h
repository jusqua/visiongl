#ifndef VGL_IMAGE_H
#define VGL_IMAGE_H
#pragma once

#include <stdint.h>

/// Pixel format kind enumeration
typedef enum {
    VGL_FORMAT_UNKNOWN = -1,
    VGL_FORMAT_GRAY8, ///< Gray color space, byte data type
    VGL_FORMAT_GRAYF, ///< Gray color space, float data type
    VGL_FORMAT_RGB8,  ///< RGB color space, byte data type
    VGL_FORMAT_RGBF,  ///< RGB color space, float data type
    VGL_FORMAT_RGBA8, ///< RGBA color space, byte data type
    VGL_FORMAT_RGBAF, ///< RGBA color space, float data type
    __VGL_FORMAT_KIND_LENGTH
} vgl_format_kind_e;

/// Image context kind enumeration
typedef enum {
    VGL_CONTEXT_HOST,
} vgl_context_kind_e;

/// Image structure
typedef struct {
    vgl_context_kind_e context;  ///< Image context kind
    vgl_format_kind_e  format;   ///< Pixel format kind
    uint32_t           channels; ///< Pixel channel count
    uint32_t           bps;      ///< Bytes per pixel sample
    uint32_t           dims;     ///< Number of image dimensions
    uint64_t*          extent;   ///< Data extent of each dimension
    uint64_t*          offset;   ///< Data position offset between dimensions
    uint64_t           length;   ///< Data byte count
    uint64_t           count;    ///< Data element count
    void*              data;     ///< Pointer to the image data
} vgl_image_t;

/// Returns the number of channels for the given format, returns 0 on failure
uint32_t vgl_channels_from_format(vgl_format_kind_e format);
/// Returns the number of bytes per pixel sample for the given format, returns 0 on failure
uint32_t vgl_bps_from_format(vgl_format_kind_e format);

/// Initializes an image with the given shape, returns 0 on failure
int vgl_image_init(vgl_image_t* image, uint64_t* extent, uint32_t dims, vgl_format_kind_e format);
/// Moves the contents of the source image to the destination image, leaving the source image empty, returns 0 on failure
int vgl_image_move(vgl_image_t* dst, vgl_image_t* src);
/// Initializes a 2D image with the given width and height, returns 0 on failure
int vgl_image_init_2d(vgl_image_t* image, uint64_t width, uint64_t height, vgl_format_kind_e format);
/// Initializes a 3D image with the given width, height, and depth, returns 0 on failure
int vgl_image_init_3d(vgl_image_t* image, uint64_t width, uint64_t height, uint64_t depth, vgl_format_kind_e format);
/// Initializes an image similar to the given image, returns 0 on failure
int vgl_image_init_similar(vgl_image_t* image, const vgl_image_t* source);
/// Deinitializes an image, returns 0 on failure
int vgl_image_deinit(vgl_image_t* image);

/// Loads an image from the given file, returns 0 on failure
int vgl_image_load(vgl_image_t* image, const char* filename);
/// Saves an image to the given file, returns 0 on failure
int vgl_image_save(const vgl_image_t* image, const char* filename);

/// Converts an image to the given format, and returns 0 on failure
int vgl_image_convert(vgl_image_t* image, vgl_format_kind_e format);
/// Reshapes an image to the given extent if matches the image length, and returns 0 on failure
int vgl_image_reshape(vgl_image_t* image, const uint64_t* target, uint32_t dims);

#endif // VGL_IMAGE_H
