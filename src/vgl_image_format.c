#include <stdint.h>

#include "vgl_image.h"

#define CONVERT_TO_UINT8(value) ((uint8_t)((value) * 255.0f))
#define CONVERT_TO_FLOAT(value) ((float)(value) / 255.0f)

typedef struct {
    float r, g, b, a;
} pixel_t;

typedef void (*pack_fn)(pixel_t* dst, const void* src);
typedef void (*unpack_fn)(void* dst, const pixel_t* src);

static void pack_gray8(pixel_t* dst, const void* src);
static void pack_grayf(pixel_t* dst, const void* src);
static void pack_rgb8(pixel_t* dst, const void* src);
static void pack_rgbf(pixel_t* dst, const void* src);
static void pack_rgba8(pixel_t* dst, const void* src);
static void pack_rgbaf(pixel_t* dst, const void* src);

static void unpack_gray8(void* dst, const pixel_t* src);
static void unpack_grayf(void* dst, const pixel_t* src);
static void unpack_rgb8(void* dst, const pixel_t* src);
static void unpack_rgbf(void* dst, const pixel_t* src);
static void unpack_rgba8(void* dst, const pixel_t* src);
static void unpack_rgbaf(void* dst, const pixel_t* src);

static pack_fn pack[] = {
    [VGL_FORMAT_GRAY8] = pack_gray8,
    [VGL_FORMAT_GRAYF] = pack_grayf,
    [VGL_FORMAT_RGB8] = pack_rgb8,
    [VGL_FORMAT_RGBF] = pack_rgbf,
    [VGL_FORMAT_RGBA8] = pack_rgba8,
    [VGL_FORMAT_RGBAF] = pack_rgbaf,
};

static unpack_fn unpack[] = {
    [VGL_FORMAT_GRAY8] = unpack_gray8,
    [VGL_FORMAT_GRAYF] = unpack_grayf,
    [VGL_FORMAT_RGB8] = unpack_rgb8,
    [VGL_FORMAT_RGBF] = unpack_rgbf,
    [VGL_FORMAT_RGBA8] = unpack_rgba8,
    [VGL_FORMAT_RGBAF] = unpack_rgbaf,
};

static void pack_gray8(pixel_t* pixel, const void* raw)
{
    float gray = CONVERT_TO_FLOAT(*(uint8_t*)raw);
    pixel->r = gray;
    pixel->g = gray;
    pixel->b = gray;
    pixel->a = 1.0f;
}

static void pack_grayf(pixel_t* pixel, const void* raw)
{
    float gray = *(float*)raw;
    pixel->r = gray;
    pixel->g = gray;
    pixel->b = gray;
    pixel->a = 1.0f;
}

static void pack_rgb8(pixel_t* pixel, const void* raw)
{
    const uint8_t* ptr = raw;
    pixel->r = CONVERT_TO_FLOAT(ptr[0]);
    pixel->g = CONVERT_TO_FLOAT(ptr[1]);
    pixel->b = CONVERT_TO_FLOAT(ptr[2]);
    pixel->a = 1.0f;
}

static void pack_rgbf(pixel_t* pixel, const void* raw)
{
    const float* ptr = raw;
    pixel->r = ptr[0];
    pixel->g = ptr[1];
    pixel->b = ptr[2];
    pixel->a = 1.0f;
}

static void pack_rgba8(pixel_t* pixel, const void* raw)
{
    const uint8_t* ptr = raw;
    pixel->r = CONVERT_TO_FLOAT(ptr[0]);
    pixel->g = CONVERT_TO_FLOAT(ptr[1]);
    pixel->b = CONVERT_TO_FLOAT(ptr[2]);
    pixel->a = CONVERT_TO_FLOAT(ptr[3]);
}

static void pack_rgbaf(pixel_t* pixel, const void* raw)
{
    const float* ptr = raw;
    pixel->r = ptr[0];
    pixel->g = ptr[1];
    pixel->b = ptr[2];
    pixel->a = ptr[3];
}

static void unpack_gray8(void* raw, const pixel_t* pixel)
{
    uint8_t* ptr = raw;
    *ptr = CONVERT_TO_UINT8(pixel->r);
}

static void unpack_grayf(void* raw, const pixel_t* pixel)
{
    float* ptr = raw;
    *ptr = pixel->r;
}

static void unpack_rgb8(void* raw, const pixel_t* pixel)
{
    uint8_t* ptr = raw;
    *ptr = CONVERT_TO_UINT8(pixel->r);
    *(ptr + 1) = CONVERT_TO_UINT8(pixel->g);
    *(ptr + 2) = CONVERT_TO_UINT8(pixel->b);
}

static void unpack_rgbf(void* raw, const pixel_t* pixel)
{
    float* ptr = raw;
    *ptr = pixel->r;
    *(ptr + 1) = pixel->g;
    *(ptr + 2) = pixel->b;
}

static void unpack_rgba8(void* raw, const pixel_t* pixel)
{
    uint8_t* ptr = raw;
    *ptr = CONVERT_TO_UINT8(pixel->r);
    *(ptr + 1) = CONVERT_TO_UINT8(pixel->g);
    *(ptr + 2) = CONVERT_TO_UINT8(pixel->b);
    *(ptr + 3) = CONVERT_TO_UINT8(pixel->a);
}

static void unpack_rgbaf(void* raw, const pixel_t* pixel)
{
    float* ptr = raw;
    *ptr = pixel->r;
    *(ptr + 1) = pixel->g;
    *(ptr + 2) = pixel->b;
    *(ptr + 3) = pixel->a;
}

int vgl_image_convert(vgl_image_t* image, vgl_format_kind_e target)
{
    vgl_format_kind_e current = image->format;
    if (current == target)
        return 1;

    if (VGL_FORMAT_UNKNOWN <= target || target >= __VGL_FORMAT_KIND_LENGTH)
        return 0;

    vgl_image_t new_image = {0};
    if (!vgl_image_init(&new_image, image->extent, image->dims, target))
        return 0;

    pixel_t pixel = {0};
    for (uint64_t i = 0; i < image->count; ++i) {
        pack[current](&pixel, (uint8_t*)image->data + i * image->bps);
        unpack[target]((uint8_t*)new_image.data + i * new_image.bps, &pixel);
    }

    if (!vgl_image_move(image, &new_image))
        return 0;
    return 1;
}
