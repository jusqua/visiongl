#ifndef VGL_SHAPE_H
#define VGL_SHAPE_H
#pragma once

#include <stdint.h>

#include "vgl_constants.h"

//! Shape data length (including number of channels)
#define VGL_SHAPE_DATA_LENGTH (1 + VGL_MAX_DIM)

//! Shape of a data array, including number of channels and dimensions
typedef struct {
    uint64_t extent[VGL_SHAPE_DATA_LENGTH]; //!< Size of each dimension, including number of channels in position 0 and other dimensions in the following positions.
    uint64_t offset[VGL_SHAPE_DATA_LENGTH]; //!< Offset between array dimensions, position 0 is always 1.
    uint64_t size;                          //!< Data byte size
    uint8_t dims;                           //!< Shape dimensions
    uint8_t bps;                            //!< Bits per data sample, for custom image data types
} vgl_shape_t;

//! Find bits per sample from a depth
uint8_t vgl_find_bits_per_sample(uint8_t depth);
//! Find width step from bits per sample, width and number of channels
uint64_t vgl_find_width_step(uint8_t bits_per_sample, uint64_t width, uint64_t channels);

//! Create shape from shape array
vgl_shape_t* vgl_shape_create(uint64_t extent[VGL_SHAPE_DATA_LENGTH], uint8_t dims, uint8_t bps);
//! Create shape identical to another shape
vgl_shape_t* vgl_shape_clone(vgl_shape_t* shape);
//! Release shape from memory
void vgl_shape_release(vgl_shape_t* shape);

//! Get index from coordinate array
uint64_t vgl_shape_index_from_coordinate(vgl_shape_t* shape, uint64_t* coord);
//! Get coordinate from index
void vgl_shape_coordinate_from_index(vgl_shape_t* shape, uint64_t* coordinate, uint64_t index);
//! Get number of pixels in shape
uint64_t vgl_shape_pixels(vgl_shape_t* shape);
//! Get number of frames in shape
uint64_t vgl_shape_frames(vgl_shape_t* shape);
//! Get dimension from a shape
uint64_t vgl_shape_dimension(vgl_shape_t* shape, uint8_t dim);
//! Get number of channels in shape
uint64_t vgl_shape_channels(vgl_shape_t* shape);
//! Get width of shape
uint64_t vgl_shape_width(vgl_shape_t* shape);
//! Get height of shape
uint64_t vgl_shape_height(vgl_shape_t* shape);
//! Get length of shape
uint64_t vgl_shape_length(vgl_shape_t* shape);

#endif // VGL_SHAPE_H
