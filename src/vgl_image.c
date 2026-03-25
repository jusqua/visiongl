#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "vgl_image.h"

static void compute_shape(const uint64_t* extent, uint64_t* offset, uint64_t* length, uint64_t* count, uint32_t dims, uint32_t bps)
{
    offset[0] = 1;
    for (uint32_t i = 1; i < dims; ++i)
        offset[i] = extent[i - 1] * offset[i - 1];
    *count = extent[dims - 1] * offset[dims - 1];
    *length = *count * bps;
}

uint32_t vgl_channels_from_format(vgl_format_kind_e format)
{
    switch (format)
    {
        case VGL_FORMAT_GRAY8:
        case VGL_FORMAT_GRAYF:
            return 1;
        case VGL_FORMAT_RGB8:
        case VGL_FORMAT_RGBF:
            return 3;
        case VGL_FORMAT_RGBA8:
        case VGL_FORMAT_RGBAF:
            return 4;
        default:
            return 0;
    }
}

uint32_t vgl_bps_from_format(vgl_format_kind_e format)
{
    uint32_t channels = vgl_channels_from_format(format);
    switch (format)
    {
        case VGL_FORMAT_GRAY8:
        case VGL_FORMAT_RGB8:
        case VGL_FORMAT_RGBA8:
            return channels;
        case VGL_FORMAT_GRAYF:
        case VGL_FORMAT_RGBF:
        case VGL_FORMAT_RGBAF:
            return channels * sizeof(float);
        default:
            return 0;
    }
}

int vgl_image_init(vgl_image_t* image, uint64_t* extent, uint32_t dims, vgl_format_kind_e format)
{
    if (image == NULL || extent == NULL)
        return 0;

    image->dims = dims;
    image->format = format;
    image->context = VGL_CONTEXT_HOST;

    image->channels = vgl_channels_from_format(format);
    image->bps = vgl_bps_from_format(format);

    image->extent = malloc(dims * sizeof(uint64_t));
    if (image->extent == NULL)
        goto error_malloc_extent;
    image->offset = malloc(dims * sizeof(uint64_t));
    if (image->offset == NULL)
        goto error_malloc_offset;

    memcpy(image->extent, extent, dims * sizeof(uint64_t));
    compute_shape(extent, image->offset, &image->length, &image->count, dims, image->bps);

    image->data = malloc(image->length);
    if (image->data == NULL)
        goto error_malloc_data;

    return 1;

error_malloc_data:
    free(image->offset);
error_malloc_offset:
    free(image->extent);
error_malloc_extent:
    return 0;
}

int vgl_image_move(vgl_image_t* dst, vgl_image_t* src)
{
    if (dst == NULL || src == NULL)
        return 0;

    vgl_image_deinit(dst);

    dst->dims = src->dims;
    dst->format = src->format;
    dst->context = src->context;
    dst->channels = src->channels;
    dst->bps = src->bps;
    dst->length = src->length;
    dst->count = src->count;
    dst->offset = src->offset;
    dst->extent = src->extent;
    dst->data = src->data;

    src->extent = NULL;
    src->offset = NULL;
    src->data = NULL;

    return 1;
}

int vgl_image_init_2d(vgl_image_t* image, uint64_t width, uint64_t height, vgl_format_kind_e format)
{
    return vgl_image_init(image, (uint64_t[]){width, height}, 2, format);
}

int vgl_image_init_3d(vgl_image_t* image, uint64_t width, uint64_t height, uint64_t depth, vgl_format_kind_e format)
{
    return vgl_image_init(image, (uint64_t[]){width, height, depth}, 3, format);
}

int vgl_image_init_similar(vgl_image_t* image, const vgl_image_t* source)
{
    return vgl_image_init(image, source->extent, source->dims, source->format);
}

int vgl_image_deinit(vgl_image_t* image)
{
    if (image == NULL) return 0;

    if (image->extent != NULL)
        free(image->extent);
    if (image->offset != NULL)
        free(image->offset);
    if (image->data != NULL)
        free(image->data);

    image->extent = NULL;
    image->offset = NULL;
    image->data = NULL;

    return 1;
}

int vgl_image_reshape(vgl_image_t* image, const uint64_t* target, uint32_t dims)
{
    if (image == NULL || target == NULL || dims == 0)
        return 0;

    uint64_t* offset = malloc(dims * sizeof(uint64_t));
    if (offset == NULL)
        goto error_malloc_offset;

    uint64_t length = 0;
    uint64_t count = 0;
    compute_shape(target, offset, &length, &count, dims, image->bps);
    if (image->length != length || image->count != count)
        goto error_shape_does_not_match;

    if (image->dims != dims)
    {
        uint64_t* extent = malloc(dims * sizeof(uint64_t));
        if (extent == NULL)
            goto error_malloc_extent;
        if (image->extent != NULL)
            free(image->extent);
        image->extent = extent;
    }

    if (image->offset != NULL)
        free(image->offset);
    image->offset = offset;

    memcpy(image->extent, target, dims * sizeof(uint64_t));
    image->count = count;
    image->length = length;

    return 1;

error_malloc_extent:
error_shape_does_not_match:
    free(offset);
error_malloc_offset:
    return 0;
}
