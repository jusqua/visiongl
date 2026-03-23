#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "vgl_probe.h"
#include "vgl_internal_utils.h"
#include "vgl_shape.h"

void vgl_probe_init(vgl_probe_t* probe, const vgl_shape_t* shape)
{
    if (probe == nullptr || shape == nullptr)
        vgl_panic("probe and shape must not be NULL");

    vgl_shape_init_similar(&probe->shape, shape);
    probe->data = (float*)malloc(sizeof(float) * shape->size);
}

void vgl_probe_init_similar(vgl_probe_t* probe, const vgl_probe_t* source)
{
    if (probe == nullptr || source == nullptr)
        vgl_panic("probe and source must not be NULL");

    vgl_probe_init(probe, &source->shape);
}

void vgl_probe_init_from_kind(vgl_probe_t* probe, vgl_probe_kind_e kind, int dims)
{
    if (probe == nullptr)
        vgl_panic("probe must not be NULL");
    if (kind < 0 || kind >= __VGL_PROBE_KIND_COUNT)
        vgl_panic("kind must be a valid probe kind");
    if (dims == 0)
        vgl_panic("dims must be greater than 0");

    auto extent = (uint64_t*)malloc(sizeof(uint64_t) * (dims + 1));
    extent[0] = 1; for (auto i = 1; i <= dims; i++) extent[i] = 3;
    vgl_shape_init(&probe->shape, extent, dims, CHAR_BIT);
    free(extent);
    probe->data = (float*)malloc(sizeof(float) * probe->shape.size);

    auto shape = &probe->shape;
    auto size = shape->size;
    auto data = probe->data;

    switch(kind) {
    case(VGL_PROBE_CROSS): {
        auto index = 0;
        auto coordinates = (uint64_t*)calloc(dims + 1, sizeof(uint64_t));
        for (auto i = 0; i < size; ++i)
            data[i] = 0.0f;

        for (auto d = 1; d <= dims; d++)
            coordinates[d] = 1;

        index = vgl_shape_index_from_coordinate(shape, coordinates);
        data[index] = 1.0f;

        for (auto d = 1; d <= dims; d++) {
            coordinates[d] = 0;
            index = vgl_shape_index_from_coordinate(shape, coordinates);
            data[index] = 1.0f;

            coordinates[d] = 2;
            index = vgl_shape_index_from_coordinate(shape, coordinates);
            data[index] = 1.0f;

            coordinates[d] = 1;
        }
        free(coordinates);
    } break;
    case(VGL_PROBE_GAUSS): {
        auto index = 0;
        auto coordinates = (uint64_t*)calloc(dims + 1, sizeof(uint64_t));
        for (auto i = 0; i < size; i++)
        {
            auto val = 1.0f;
            vgl_shape_coordinate_from_index(shape, coordinates, index);
            for (auto d = 1; d <= dims; ++d) {
                if (coordinates[d] == 1) val *= .5;
                else val *= .25f;
           	}
            probe->data[i] = val;
        }
        free(coordinates);
    } break;
    case(VGL_PROBE_MEAN): {
        for (auto i = 0; i < size; i++) {
            probe->data[i] = 1.0f / (float)size;
        }
    } break;
    case(VGL_PROBE_CUBE): {
        for (auto i = 0; i < size; i++)
            probe->data[i] = 1.0f;
    } break;
    default:
        vgl_panic("Invalid probe kind");
    }
}

void vgl_probe_deinit(vgl_probe_t* probe)
{
    if (probe == nullptr)
        vgl_panic("probe must not be NULL");

    vgl_shape_deinit(&probe->shape);
    free(probe->data);
}
