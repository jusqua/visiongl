#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "vgl_internal_utils.h"
#include "vgl_constants.h"
#include "vgl_shape.h"

uint8_t vgl_find_bps(uint8_t depth)
{
  return depth & 255;
}

uint64_t vgl_find_width_step(uint8_t bps, uint64_t width, uint64_t channels)
{
    assert(channels > 0);
    assert(width > 0);
    assert(bps > 0);

    if (bps == 1) {
        return (width - 1) / VGL_PACK_SIZE_BITS + 1;
    }
    else if (bps % CHAR_BIT) {
        vgl_panic("bps is not 1 or multiple of %d", bps, CHAR_BIT);
    }
    return (bps / CHAR_BIT) * channels * width;
}

vgl_shape_t* vgl_shape_create(uint64_t extent[VGL_SHAPE_DATA_LENGTH], uint8_t dims, uint8_t bps)
{
    assert(extent != NULL);
    assert(dims > 0);
    assert(bps > 0);

    if ((bps == 1) && (extent[0] != 1)) {
        vgl_panic("Multi channel is not allowed when bps is 1", extent[0]);
    }

    auto new_shape = (vgl_shape_t*)malloc(sizeof(vgl_shape_t));
    if (new_shape == NULL) {
        vgl_panic("Failed to allocate memory for vgl_shape_t");
    }

    new_shape->dims = dims;
    new_shape->bps = bps;
    new_shape->size = 1;

    auto channels = extent[0];
    auto width = extent[1];
    for (auto i = 0; i <= VGL_MAX_DIM; i++) {
        if (i <= dims) {
            new_shape->extent[i] = extent[i];
            if(i == 0) {
                new_shape->offset[i] = 1;
            } else if (i == 2) {
                new_shape->offset[i] = vgl_find_width_step(bps, width, channels);
            } else {
                new_shape->offset[i] = extent[i - 1] * new_shape->offset[i - 1];
            }
        } else {
            new_shape->extent[i] = 1;
            new_shape->offset[i] = 0;
        }
    }

    new_shape->size *= new_shape->extent[dims] * new_shape->offset[dims];

    return new_shape;
}

vgl_shape_t* vgl_shape_clone(vgl_shape_t* shape) {
    return vgl_shape_create(shape->extent, shape->dims, shape->bps);
}

void vgl_shape_release(vgl_shape_t* shape) {
    free(shape);
    shape = nullptr;
}

uint64_t vgl_shape_index_from_coordinate(vgl_shape_t* shape, uint64_t* coordinate) {
    assert(shape != NULL && coordinate != NULL);

    uint64_t result = 0;
    for (auto d = 0; d <= shape->dims; ++d)
        result += shape->offset[d] * coordinate[d];
    return result;
}

void vgl_shape_coordinate_from_index(vgl_shape_t* shape, uint64_t* coordinate, uint64_t index) {
    assert(shape != NULL && coordinate != NULL);

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

uint64_t vgl_shape_pixels(vgl_shape_t* shape) {
    assert(shape != NULL);
    return shape->size / shape->extent[0];
}

uint64_t vgl_shape_frames(vgl_shape_t* shape)
{
    assert(shape != NULL);

    uint64_t frames = 1;
    for (auto i = 3; i <= shape->dims; ++i)
        frames *= shape->extent[i];
    return frames;
}

uint64_t vgl_shape_dimension(vgl_shape_t* shape, uint8_t dim)
{
    assert(shape != NULL && dim <= shape->dims);
    return shape->extent[dim];
}

uint64_t vgl_shape_channels(vgl_shape_t* shape) {
    return vgl_shape_dimension(shape, 0);
}

uint64_t vgl_shape_width(vgl_shape_t* shape) {
    return vgl_shape_dimension(shape, 1);
}

uint64_t vgl_shape_height(vgl_shape_t* shape) {
    return vgl_shape_dimension(shape, 2);
}

uint64_t vgl_shape_length(vgl_shape_t* shape) {
    return vgl_shape_dimension(shape, 3);
}
