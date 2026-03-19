#ifndef VGL_SHAPE_H
#define VGL_SHAPE_H
#pragma once

#include <stdint.h>

//! Shape of a data array, including number of channels and dimensions
typedef struct {
    uint64_t* extent; //!< Size of each dimension, including number of channels in position 0 and other dimensions in the following positions.
    uint64_t* offset; //!< Offset between array dimensions, position 0 is always 1.
    uint64_t  size;   //!< Number of elements available in the data array based on extent.
    uint8_t   dims;   //!< Number of dimensions in shape.
    uint8_t   bps;    //!< Bits per data sample, for custom image data types.
} vgl_shape_t;

//! Find bits per sample from a depth
uint8_t vgl_find_bps(int depth);
//! Find width step from bits per sample, width and number of channels
uint64_t vgl_find_width_step(uint8_t bps, uint64_t width, uint64_t channels);

//! Initialize shape from shape array
void vgl_shape_init(vgl_shape_t* shape, uint64_t *extent, uint8_t dims, uint8_t bps);
//! Initialize shape based on another shape
void vgl_shape_init_similar(vgl_shape_t* shape, const vgl_shape_t* source);
//! Deinitialize shape internal attributes from memory
void vgl_shape_deinit(vgl_shape_t* shape);

//! Get index from coordinate array
uint64_t vgl_shape_index_from_coordinate(const vgl_shape_t* shape, const uint64_t* coordinate);
//! Get coordinate from index
void vgl_shape_coordinate_from_index(const vgl_shape_t* shape, uint64_t* coordinate, uint64_t index);
//! Get number of pixels in shape
uint64_t vgl_shape_pixels(const vgl_shape_t* shape);
//! Get number of frames in shape
uint64_t vgl_shape_frames(const vgl_shape_t* shape);
//! Get dimension from a shape
uint64_t vgl_shape_dimension(const vgl_shape_t* shape, uint8_t dim);
//! Get number of channels in shape
uint64_t vgl_shape_channels(const vgl_shape_t* shape);
//! Get width of shape
uint64_t vgl_shape_width(const vgl_shape_t* shape);
//! Get height of shape
uint64_t vgl_shape_height(const vgl_shape_t* shape);
//! Get length of shape
uint64_t vgl_shape_length(const vgl_shape_t* shape);

#endif // VGL_SHAPE_H
