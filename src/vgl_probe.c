#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "vgl_probe.h"
#include "vgl_internal_utils.h"
#include "vgl_shape.h"

vgl_probe_t* vgl_probe_wrap(vgl_shape_t* shape, float* data)
{
    assert(shape != NULL && data != NULL);
    auto new_probe = (vgl_probe_t*)malloc(sizeof(vgl_probe_t));
    new_probe->shape = shape;
    new_probe->data = data;

    return new_probe;
}

vgl_probe_t* vgl_probe_create(vgl_shape_t* shape, float* data)
{
    assert(shape != NULL && data != NULL);

    auto new_shape = vgl_shape_clone(shape);
    auto new_data = (float*)malloc(sizeof(float) * shape->size);
    for (auto i = 0; i < shape->size; i++)
      new_data[i] = data[i];

    return vgl_probe_wrap(new_shape, new_data);
}

vgl_probe_t* vgl_probe_clone(vgl_probe_t* probe)
{
    assert(probe != NULL);
    return vgl_probe_create(probe->shape, probe->data);
}

vgl_probe_t* vgl_probe_from_kind(vgl_probe_kind_t kind, int dims)
{
    uint64_t extent[VGL_SHAPE_DATA_LENGTH] = {1};
    for (auto i = 1; i <= dims; i++) extent[i] = 3;
    auto new_shape = vgl_shape_create(extent, dims, sizeof(float) * CHAR_BIT);
    auto new_data = (float*)malloc(sizeof(float) * new_shape->size);
    auto size = new_shape->size;

    switch(kind) {
    case(VGL_PROBE_CROSS): {
        int index;
        uint64_t coordinates[VGL_SHAPE_DATA_LENGTH] = {0};
        for (auto i = 0; i < size; ++i)
            new_data[i] = 0.0f;

        for (auto d = 1; d <= dims; d++)
            coordinates[d] = 1;

        index = vgl_shape_index_from_coordinate(new_shape, coordinates);
        new_data[index] = 1.0f;

        for (auto d = 1; d <= dims; d++) {
            coordinates[d] = 0;
            index = vgl_shape_index_from_coordinate(new_shape, coordinates);
            new_data[index] = 1.0f;

            coordinates[d] = 2;
            index = vgl_shape_index_from_coordinate(new_shape, coordinates);
            new_data[index] = 1.0f;

            coordinates[d] = 1;
        }
    } break;
    case(VGL_PROBE_GAUSS): {
        int index;
        uint64_t coordinates[VGL_SHAPE_DATA_LENGTH] = {0};
        for (auto i = 0; i < size; i++)
        {
            auto val = 1.0f;
            vgl_shape_coordinate_from_index(new_shape, coordinates, index);
            for (auto d = 1; d <= dims; ++d) {
                if (coordinates[d] == 1) val *= .5;
                else val *= .25;
           	}
            new_data[i] = val;
        }
    } break;
    case(VGL_PROBE_MEAN): {
        for (auto i = 0; i < size; i++) {
            new_data[i] = 1.0f / (float) size;
        }
    } break;
    case(VGL_PROBE_CUBE): {
        for (auto i = 0; i < size; i++)
            new_data[i] = 1.0f;
    } break;
    default:
        vgl_panic("Invalid probe kind");
    }

    return vgl_probe_wrap(new_shape, new_data);
}

void vgl_probe_release(vgl_probe_t* probe)
{
    vgl_shape_release(probe->shape);
    free(probe->data);
    probe = nullptr;
}
