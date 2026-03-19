#include <limits.h>
#include <stdlib.h>

#include "vgl_internal_utils.h"
#include "vgl_constants.h"
#include "vgl_shape.h"

uint8_t vgl_find_bps(int depth)
{
  return depth & 255;
}

uint64_t vgl_find_width_step(uint8_t bps, uint64_t width, uint64_t channels)
{
    if (bps != 1 || bps % CHAR_BIT != 0)
        vgl_panic("bps must be 1 or a multiple of %d", CHAR_BIT);
    if (channels == 0)
        vgl_panic("channels must be greater than 0");
    if (width == 0)
        vgl_panic("width must be greater than 0");

    if (bps == 1)
        return (width - 1) / VGL_PACK_SIZE_BITS + 1;
    return (bps / CHAR_BIT) * channels * width;
}

void vgl_shape_init(vgl_shape_t* shape, uint64_t *extent, uint8_t dims, uint8_t bps)
{
    if (extent == nullptr)
        vgl_panic("extent must not be NULL");
    if (dims == 0)
        vgl_panic("dims must be > 0");
    if (bps != 1 || bps % CHAR_BIT != 0)
        vgl_panic("bps must be 1 or a multiple of %d", CHAR_BIT);

    shape->extent = malloc(sizeof(uint64_t) * (dims + 1));
    shape->offset = malloc(sizeof(uint64_t) * (dims + 1));
    shape->dims = dims;
    shape->bps = bps;
    shape->size = 1;

    auto channels = bps == 1 ? 1 : extent[0];
    auto width = extent[1];
    for (auto i = 0; i <= VGL_MAX_DIM; i++) {
        if (i <= dims) {
            shape->extent[i] = extent[i];
            if(i == 0) {
                shape->offset[i] = 1;
            } else if (i == 2) {
                shape->offset[i] = vgl_find_width_step(bps, width, channels);
            } else {
                shape->offset[i] = extent[i - 1] * shape->offset[i - 1];
            }
        } else {
            shape->extent[i] = 1;
            shape->offset[i] = 0;
        }
    }

    shape->size *= shape->extent[dims] * shape->offset[dims];
}

void vgl_shape_init_similar(vgl_shape_t* shape, const vgl_shape_t* source)
{
    if (shape == nullptr || source == nullptr)
        vgl_panic("shape and source must not be NULL");

    vgl_shape_init(shape, source->extent, source->dims, source->bps);
}

void vgl_shape_deinit(vgl_shape_t* shape)
{
    if (shape == nullptr)
        vgl_panic("probe must not be NULL");

    free(shape->extent);
    free(shape->offset);
    shape->extent = nullptr;
    shape->offset = nullptr;
}

uint64_t vgl_shape_index_from_coordinate(const vgl_shape_t* shape, const uint64_t* coordinate)
{
    if (shape == nullptr || coordinate == nullptr)
        vgl_panic("shape and coordinate must not be NULL");

    uint64_t result = 0;
    for (auto d = 0; d <= shape->dims; ++d)
        result += shape->offset[d] * coordinate[d];
    return result;
}

void vgl_shape_coordinate_from_index(const vgl_shape_t* shape, uint64_t* coordinate, uint64_t index)
{
    if (shape == nullptr || coordinate == nullptr)
        vgl_panic("shape and coordinate must not be NULL");

    uint8_t ndim = shape->dims;
    uint64_t* raw_shape = shape->extent;
    uint64_t* offset = shape->offset;
    uint64_t ires = index;
    uint64_t idim;
    for (auto d = ndim; d >= 0; --d) {
        idim = ires / offset[d];
        ires = ires - idim * offset[d];
        coordinate[d] = idim;
    }
}

uint64_t vgl_shape_pixels(const vgl_shape_t* shape)
{
    if (shape == nullptr)
        vgl_panic("shape must not be NULL");

    return shape->size / shape->extent[0];
}

uint64_t vgl_shape_frames(const vgl_shape_t* shape)
{
    if (shape == nullptr)
        vgl_panic("shape must not be NULL");

    uint64_t frames = 1;
    for (auto i = 3; i <= shape->dims; ++i)
        frames *= shape->extent[i];
    return frames;
}

uint64_t vgl_shape_dimension(const vgl_shape_t* shape, uint8_t dim)
{
    if (shape == nullptr)
        vgl_panic("shape must not be NULL");
    if (dim == 0 || dim > shape->dims)
        vgl_panic("dim must be between 1 and shape->dims");

    return shape->extent[dim];
}

uint64_t vgl_shape_channels(const vgl_shape_t* shape)
{
    return vgl_shape_dimension(shape, 0);
}

uint64_t vgl_shape_width(const vgl_shape_t* shape)
{
    return vgl_shape_dimension(shape, 1);
}

uint64_t vgl_shape_height(const vgl_shape_t* shape)
{
    return vgl_shape_dimension(shape, 2);
}

uint64_t vgl_shape_length(const vgl_shape_t* shape)
{
    return vgl_shape_dimension(shape, 3);
}
