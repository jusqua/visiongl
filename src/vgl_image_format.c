#include <stdint.h>

#include "vgl_image.h"

#define CONVERT_TO_BYTE(value)  ((uint8_t)((value) * 255.0f))
#define CONVERT_TO_FLOAT(value) ((float)(value) / 255.0f)

typedef struct {
    float r, g, b, a;
} pixel_t;

typedef void (*pack_fn)(pixel_t* dst, const void* src);
typedef void (*unpack_fn)(void* dst, const pixel_t* src);

static void pack_gray_byte(pixel_t* dst, const void* src);
static void pack_gray_float(pixel_t* dst, const void* src);
static void pack_rgb_byte(pixel_t* dst, const void* src);
static void pack_rgb_float(pixel_t* dst, const void* src);
static void pack_rgba_byte(pixel_t* dst, const void* src);
static void pack_rgba_float(pixel_t* dst, const void* src);

static void unpack_gray_byte(void* dst, const pixel_t* src);
static void unpack_gray_float(void* dst, const pixel_t* src);
static void unpack_rgb_byte(void* dst, const pixel_t* src);
static void unpack_rgb_float(void* dst, const pixel_t* src);
static void unpack_rgba_byte(void* dst, const pixel_t* src);
static void unpack_rgba_float(void* dst, const pixel_t* src);

static pack_fn pack[] = {
    [VGL_FORMAT_GRAY_BYTE] = pack_gray_byte,
    [VGL_FORMAT_GRAY_FLOAT] = pack_gray_float,
    [VGL_FORMAT_RGB_BYTE] = pack_rgb_byte,
    [VGL_FORMAT_RGB_FLOAT] = pack_rgb_float,
    [VGL_FORMAT_RGBA_BYTE] = pack_rgba_byte,
    [VGL_FORMAT_RGBA_FLOAT] = pack_rgba_float,
};

static unpack_fn unpack[] = {
    [VGL_FORMAT_GRAY_BYTE] = unpack_gray_byte,
    [VGL_FORMAT_GRAY_FLOAT] = unpack_gray_float,
    [VGL_FORMAT_RGB_BYTE] = unpack_rgb_byte,
    [VGL_FORMAT_RGB_FLOAT] = unpack_rgb_float,
    [VGL_FORMAT_RGBA_BYTE] = unpack_rgba_byte,
    [VGL_FORMAT_RGBA_FLOAT] = unpack_rgba_float,
};

static void pack_gray_byte(pixel_t* pixel, const void* raw)
{
    float gray = CONVERT_TO_FLOAT(*(uint8_t*)raw);
    pixel->r = gray;
    pixel->g = gray;
    pixel->b = gray;
    pixel->a = 1.0f;
}

static void pack_gray_float(pixel_t* pixel, const void* raw)
{
    float gray = *(float*)raw;
    pixel->r = gray;
    pixel->g = gray;
    pixel->b = gray;
    pixel->a = 1.0f;
}

static void pack_rgb_byte(pixel_t* pixel, const void* raw)
{
    const uint8_t* ptr = raw;
    pixel->r = CONVERT_TO_FLOAT(ptr[0]);
    pixel->g = CONVERT_TO_FLOAT(ptr[1]);
    pixel->b = CONVERT_TO_FLOAT(ptr[2]);
    pixel->a = 1.0f;
}

static void pack_rgb_float(pixel_t* pixel, const void* raw)
{
    const float* ptr = raw;
    pixel->r = ptr[0];
    pixel->g = ptr[1];
    pixel->b = ptr[2];
    pixel->a = 1.0f;
}

static void pack_rgba_byte(pixel_t* pixel, const void* raw)
{
    const uint8_t* ptr = raw;
    pixel->r = CONVERT_TO_FLOAT(ptr[0]);
    pixel->g = CONVERT_TO_FLOAT(ptr[1]);
    pixel->b = CONVERT_TO_FLOAT(ptr[2]);
    pixel->a = CONVERT_TO_FLOAT(ptr[3]);
}

static void pack_rgba_float(pixel_t* pixel, const void* raw)
{
    const float* ptr = raw;
    pixel->r = ptr[0];
    pixel->g = ptr[1];
    pixel->b = ptr[2];
    pixel->a = ptr[3];
}

static void unpack_gray_byte(void* raw, const pixel_t* pixel)
{
    uint8_t* ptr = raw;
    *ptr = CONVERT_TO_BYTE(pixel->r);
}

static void unpack_gray_float(void* raw, const pixel_t* pixel)
{
    float* ptr = raw;
    *ptr = pixel->r;
}

static void unpack_rgb_byte(void* raw, const pixel_t* pixel)
{
    uint8_t* ptr = raw;
    *ptr = CONVERT_TO_BYTE(pixel->r);
    *(ptr + 1) = CONVERT_TO_BYTE(pixel->g);
    *(ptr + 2) = CONVERT_TO_BYTE(pixel->b);
}

static void unpack_rgb_float(void* raw, const pixel_t* pixel)
{
    float* ptr = raw;
    *ptr = pixel->r;
    *(ptr + 1) = pixel->g;
    *(ptr + 2) = pixel->b;
}

static void unpack_rgba_byte(void* raw, const pixel_t* pixel)
{
    uint8_t* ptr = raw;
    *ptr = CONVERT_TO_BYTE(pixel->r);
    *(ptr + 1) = CONVERT_TO_BYTE(pixel->g);
    *(ptr + 2) = CONVERT_TO_BYTE(pixel->b);
    *(ptr + 3) = CONVERT_TO_BYTE(pixel->a);
}

static void unpack_rgba_float(void* raw, const pixel_t* pixel)
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
        pack[current](&pixel, image->data + i * image->bps);
        unpack[target](new_image.data + i * new_image.bps, &pixel);
    }

    if (!vgl_image_move(image, &new_image))
        return 0;
    return 1;
}
