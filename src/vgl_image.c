#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "vgl_image.h"
#include "vgl_internal_utils.h"
#include "vgl_shape.h"

uint8_t vgl_bps_from_color_space_kind(vgl_color_space_kind_e kind)
{
    switch (kind)
    {
        case VGL_COLOR_SPACE_BINARY:
            return 1;
        case VGL_COLOR_SPACE_GRAYSCALE:
            return CHAR_BIT;
        case VGL_COLOR_SPACE_GRAYSCALE_ALPHA:
            return 2 * CHAR_BIT;
        case VGL_COLOR_SPACE_RGB:
            return 3 * CHAR_BIT;
        case VGL_COLOR_SPACE_RGB_ALPHA:
            return 4 * CHAR_BIT;
        default:
            return 0;
    }
}

vgl_color_space_kind_e vgl_color_space_kind_from_bps(uint8_t bps)
{
    switch (bps)
    {
        case 1:
            return VGL_COLOR_SPACE_BINARY;
        case CHAR_BIT:
            return VGL_COLOR_SPACE_GRAYSCALE;
        case 2 * CHAR_BIT:
            return VGL_COLOR_SPACE_GRAYSCALE_ALPHA;
        case 3 * CHAR_BIT:
            return VGL_COLOR_SPACE_RGB;
        case 4 * CHAR_BIT:
            return VGL_COLOR_SPACE_RGB_ALPHA;
        default:
            return VGL_COLOR_SPACE_INVALID;
    }
}

void vgl_image_init(vgl_image_t* image, const vgl_shape_t* shape)
{
    if (image == nullptr || shape == nullptr)
        vgl_panic("image and shape must not be NULL");

    vgl_shape_init_similar(&image->shape, shape);
    image->kind = VGL_IMAGE_CONTEXT_HOST;
    image->context.host.data = malloc(shape->size * shape->bps / CHAR_BIT);
}

void vgl_image_init_2d(vgl_image_t* image, uint64_t width, uint64_t height, vgl_color_space_kind_e color_space)
{
    if (image == nullptr)
        vgl_panic("image must not be NULL");
    if (width == 0 || height == 0)
        vgl_panic("width and height must be greater than 0");

    auto bps = vgl_bps_from_color_space_kind(color_space);
    vgl_shape_init(&image->shape, (uint64_t[]){bps / CHAR_BIT, width, height}, 2, bps);
    image->kind = VGL_IMAGE_CONTEXT_HOST;
    image->context.host.data = malloc(image->shape.size * bps / CHAR_BIT);
}

void vgl_image_init_3d(vgl_image_t* image, uint64_t width, uint64_t height, uint64_t depth, vgl_color_space_kind_e color_space)
{
    if (image == nullptr)
        vgl_panic("image must not be NULL");
    if (width == 0 || height == 0 || depth == 0)
        vgl_panic("width, height, and depth must be greater than 0");

    auto bps = vgl_bps_from_color_space_kind(color_space);
    vgl_shape_init(&image->shape, (uint64_t[]){bps / CHAR_BIT, width, height, depth}, 3, bps);
    image->kind = VGL_IMAGE_CONTEXT_HOST;
    image->context.host.data = malloc(image->shape.size * bps / CHAR_BIT);
}

void vgl_image_init_similar(vgl_image_t* image, const vgl_image_t* source)
{
    if (image == nullptr || source == nullptr)
        vgl_panic("image and source must not be NULL");

    vgl_image_init(image, &source->shape);
}

void vgl_image_deinit(vgl_image_t* image)
{
    if (image == nullptr)
        vgl_panic("image must not be NULL");

    vgl_shape_deinit(&image->shape);
    free(image->context.host.data);
}

void vgl_image_color_space_to_rgb_alpha(vgl_image_t* image)
{
    auto kind = vgl_color_space_kind_from_bps(image->shape.bps);
    if (kind == VGL_COLOR_SPACE_RGB_ALPHA)
    {
        vgl_log(VGL_LOG_INFO, "bps already matches, no conversion needed");
        return;
    }

    switch (kind)
    {
        case VGL_COLOR_SPACE_BINARY: break;
        case VGL_COLOR_SPACE_GRAYSCALE: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size * 3;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = data[i / 3];
            free(data);
        } break;
        case VGL_COLOR_SPACE_GRAYSCALE_ALPHA: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size * 3 / 2;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = data[i * 2 / 3];
            free(data);
        } break;
        case VGL_COLOR_SPACE_RGB_ALPHA: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size * 3 / 4;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = data[i * 4 / 3];
            free(data);
        } break;
        default:
            vgl_panic("unsupported color space kind for binary conversion");
            break;
    }

    vgl_shape_resample(&image->shape, vgl_bps_from_color_space_kind(VGL_COLOR_SPACE_RGB_ALPHA));
}

void vgl_image_color_space_to_rgb(vgl_image_t* image)
{
    auto kind = vgl_color_space_kind_from_bps(image->shape.bps);
    if (kind == VGL_COLOR_SPACE_RGB)
    {
        vgl_log(VGL_LOG_INFO, "bps already matches, no conversion needed");
        return;
    }

    switch (kind)
    {
        case VGL_COLOR_SPACE_BINARY: break;
        case VGL_COLOR_SPACE_GRAYSCALE: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size * 3;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = data[i / 3];
            free(data);
        } break;
        case VGL_COLOR_SPACE_GRAYSCALE_ALPHA: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size * 3 / 2;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = data[i * 2 / 3];
            free(data);
        } break;
        case VGL_COLOR_SPACE_RGB_ALPHA: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size * 3 / 4;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = data[i * 4 / 3];
            free(data);
        } break;
        default:
            vgl_panic("unsupported color space kind for binary conversion");
            break;
    }

    vgl_shape_resample(&image->shape, vgl_bps_from_color_space_kind(VGL_COLOR_SPACE_RGB));
}

void vgl_image_color_space_to_grayscale_alpha(vgl_image_t* image)
{
    auto kind = vgl_color_space_kind_from_bps(image->shape.bps);
    if (kind == VGL_COLOR_SPACE_GRAYSCALE_ALPHA)
    {
        vgl_log(VGL_LOG_INFO, "bps already matches, no conversion needed");
        return;
    }

    switch (kind)
    {
        case VGL_COLOR_SPACE_BINARY: break;
        case VGL_COLOR_SPACE_GRAYSCALE: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size * 2;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = i % 2 == 0 ? data[i / 2] : 255;
            free(data);
        } break;
        case VGL_COLOR_SPACE_RGB: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size * 2 / 3;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = i % 2 == 0 ? data[i * 3] + data[i * 3 + 1] + data[i * 3 + 2] : 255;
            free(data);
        } break;
        case VGL_COLOR_SPACE_RGB_ALPHA: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size / 2;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = i % 2 == 0 ? data[i * 4] + data[i * 4 + 1] + data[i * 4 + 2] : data[i * 4 + 3];
            free(data);
        } break;
        default:
            vgl_panic("unsupported color space kind for binary conversion");
            break;
    }

    vgl_shape_resample(&image->shape, vgl_bps_from_color_space_kind(VGL_COLOR_SPACE_GRAYSCALE_ALPHA));
}

void vgl_image_color_space_to_grayscale(vgl_image_t* image)
{
    auto kind = vgl_color_space_kind_from_bps(image->shape.bps);
    if (kind == VGL_COLOR_SPACE_GRAYSCALE)
    {
        vgl_log(VGL_LOG_INFO, "bps already matches, no conversion needed");
        return;
    }

    switch (kind)
    {
        case VGL_COLOR_SPACE_BINARY: break;
        case VGL_COLOR_SPACE_GRAYSCALE_ALPHA: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size / 2;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = data[i * 2];
            free(data);
        } break;
        case VGL_COLOR_SPACE_RGB: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size / 3;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = data[i * 3] + data[i * 3 + 1] + data[i * 3 + 2];
            free(data);
        } break;
        case VGL_COLOR_SPACE_RGB_ALPHA: {
            auto data = image->context.host.data;
            auto new_size = image->shape.size / 4;
            image->context.host.data = malloc(new_size);
            for (size_t i = 0; i < new_size; ++i)
                image->context.host.data[i] = data[i * 4] + data[i * 4 + 1] + data[i * 4 + 2];
            free(data);
        } break;
        default:
            vgl_panic("unsupported color space kind for binary conversion");
            break;
    }

    vgl_shape_resample(&image->shape, vgl_bps_from_color_space_kind(VGL_COLOR_SPACE_GRAYSCALE));
}

void vgl_image_color_space_to_binary(vgl_image_t* image)
{
    auto kind = vgl_color_space_kind_from_bps(image->shape.bps);
    if (kind == VGL_COLOR_SPACE_BINARY)
    {
        vgl_log(VGL_LOG_INFO, "bps already matches, no conversion needed");
        return;
    }

    switch (kind)
    {
        case VGL_COLOR_SPACE_GRAYSCALE: break;
        case VGL_COLOR_SPACE_GRAYSCALE_ALPHA:
        case VGL_COLOR_SPACE_RGB:
        case VGL_COLOR_SPACE_RGB_ALPHA:
            vgl_image_color_space_to_grayscale(image);
            break;
        default:
            vgl_panic("unsupported color space kind");
            return;
    }

    for (size_t i = 0; i < image->shape.size; i++)
        image->context.host.data[i] = image->context.host.data[i] > 128 ? 255 : 0;

    vgl_shape_resample(&image->shape, vgl_bps_from_color_space_kind(VGL_COLOR_SPACE_BINARY));
}

void vgl_image_resample(vgl_image_t* image, uint8_t bps)
{
    if (bps != 1 || bps % CHAR_BIT != 0)
        vgl_panic("bps must be 1 or a multiple of %d", CHAR_BIT);

    if (image->shape.bps > bps)
        vgl_log(VGL_LOG_WARN, "image->bps is larger than the target bps, data may be lost");

    switch (vgl_color_space_kind_from_bps(bps))
    {
        case VGL_COLOR_SPACE_BINARY: {
            vgl_image_color_space_to_binary(image);
        } break;
        case VGL_COLOR_SPACE_GRAYSCALE: {
            vgl_image_color_space_to_grayscale(image);
        } break;
        case VGL_COLOR_SPACE_GRAYSCALE_ALPHA: {
            vgl_image_color_space_to_grayscale_alpha(image);
        } break;
        case VGL_COLOR_SPACE_RGB: {
            vgl_image_color_space_to_rgb(image);
        } break;
        case VGL_COLOR_SPACE_RGB_ALPHA: {
            vgl_image_color_space_to_rgb_alpha(image);
        } break;
        default:
            vgl_panic("unsupported color space kind");
    }
}

void vgl_image_reshape(vgl_image_t* image, const vgl_shape_t* source)
{
    if (image == nullptr || source == nullptr)
        vgl_panic("image and source must not be NULL");

    if (image->shape.size / image->shape.bps != source->size / source->bps)
        vgl_panic("shape size changes must match bps changes");

    if (image->shape.bps != source->bps)
        vgl_image_resample(image, source->bps);

    vgl_shape_reshape(&image->shape, source);
}
