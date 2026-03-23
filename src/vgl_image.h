#ifndef VGL_IMAGE_H
#define VGL_IMAGE_H
#pragma once

#include <stdint.h>

#include "vgl_shape.h"

/// Color space kind enumeration
typedef enum {
    VGL_COLOR_SPACE_INVALID = -1,    ///< Invalid color space kind
    VGL_COLOR_SPACE_BINARY,          ///< Binary color space (1 bit per sample)
    VGL_COLOR_SPACE_GRAYSCALE,       ///< Grayscale color space (8 bits per sample)
    VGL_COLOR_SPACE_GRAYSCALE_ALPHA, ///< Grayscale with alpha color space (16 bits per sample)
    VGL_COLOR_SPACE_RGB,             ///< RGB color space (24 bits per sample)
    VGL_COLOR_SPACE_RGB_ALPHA,       ///< RGB with alpha color space (32 bits per sample)
    __VGL_COLOR_SPACE_KIND_COUNT,
} vgl_color_space_kind_e;

/// Image host context structure
typedef struct {
    uint8_t* data;
} vgl_image_host_context_t;

/// Image context kind enumeration
typedef enum {
    VGL_IMAGE_CONTEXT_HOST,
    __VGL_IMAGE_CONTEXT_KIND_COUNT
} vgl_image_context_kind_e;

/// Image context union
typedef union {
    vgl_image_host_context_t   host;
} vgl_image_context_u;

/// Image structure
typedef struct {
    vgl_shape_t              shape;   ///< Shape of the image
    vgl_image_context_kind_e kind;    ///< Context kind of the image
    vgl_image_context_u      context; ///< Context data of the image
} vgl_image_t;

/// Returns the number of bits per sample for a given color space kind
uint8_t vgl_bps_from_color_space_kind(vgl_color_space_kind_e kind);
/// Returns the color space kind for a given number of bits per sample
vgl_color_space_kind_e vgl_color_space_kind_from_bps(uint8_t bps);

/// Initializes an image with the given shape
void vgl_image_init(vgl_image_t* image, const vgl_shape_t* shape);
/// Initializes a 2D image with the given width, height
void vgl_image_init_2d(vgl_image_t* image, uint64_t width, uint64_t height, vgl_color_space_kind_e color_space);
/// Initializes a 3D image with the given width, height, depth
void vgl_image_init_3d(vgl_image_t* image, uint64_t width, uint64_t height, uint64_t depth, vgl_color_space_kind_e color_space);
/// Initializes an image similar to the given source image
void vgl_image_init_similar(vgl_image_t* image, const vgl_image_t* source);
/// Deinitializes an image
void vgl_image_deinit(vgl_image_t* image);

/// Converts an image from its color space to RGB with alpha
void vgl_image_color_space_to_rgb_alpha(vgl_image_t* image);
/// Converts an image from its color space to RGB
void vgl_image_color_space_to_rgb(vgl_image_t* image);
/// Converts an image from its color space to grayscale with alpha
void vgl_image_color_space_to_grayscale_alpha(vgl_image_t* image);
/// Converts an image from its color space to grayscale
void vgl_image_color_space_to_grayscale(vgl_image_t* image);
/// Converts an image from its color space to binary
void vgl_image_color_space_to_binary(vgl_image_t* image);

/// Resamples an image to the given bits per sample by converting to a different color space
void vgl_image_resample(vgl_image_t* image, uint8_t bps);
/// Reshapes an image to the given shape
void vgl_image_reshape(vgl_image_t* image, const vgl_shape_t* source);

#endif // VGL_IMAGE_H
