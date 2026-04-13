#include <stdint.h>
#include <stdio.h>

#include "vgl_image.h"
#include "vgl_io.h"

// Based on https://en.wikipedia.org/wiki/BMP_file_format

/// Bitmap file detection "magic" bytes
static const uint32_t BMP_TYPE = 0x4d42;

// TODO: Support for 1, 2, 4, and 16 bits per pixel
/// Bitmap bits per pixel format.
typedef enum {
    BMP_FORMAT_UNKNOWN = -1,
    BMP_FORMAT_GRAY = 8,
    BMP_FORMAT_RGB = 24,
    BMP_FORMAT_RGBA = 32,
} bmp_format_e;

// Necessary "shenanigan"
#pragma pack(push, 1)
/// Formerly BITMAPFILEHEADER.
//! Adapted from https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader.
typedef struct {
    uint16_t type;   ///< Formerly bfType.
    uint32_t size;   ///< Formerly bfSize.
    uint32_t dummy;  ///< Formerly bfReserved1 and bfReserved2.
    uint32_t offset; ///< Formerly bfOffBits.
} bmp_file_header_t;

/// Formerly BITMAPINFOHEADER.
//! Adapted from https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader.
typedef struct {
    uint32_t size;             ///< Formerly biSize.
    uint32_t width;            ///< Formerly biWidth.
    uint32_t height;           ///< Formerly biHeight.
    uint16_t planes;           ///< Formerly biPlanes.
    uint16_t bpp;              ///< Formerly biBitCount
    uint32_t compression;      ///< Formerly biCompression.
    uint32_t count;            ///< Formerly biSizeImage.
    uint32_t x_ppm;            ///< Formerly biXPelsPerMeter.
    uint32_t y_ppm;            ///< Formerly biYPelsPerMeter.
    uint32_t num_colors;       ///< Formerly biClrUsed.
    uint32_t important_colors; ///< Formerly biClrImportant.
} bmp_info_header_t;
#pragma pack(pop)

static int bmp_verify(void* fd);
static int bmp_load(void* fd, vgl_image_t* image);
static int bmp_save(void* fd, const vgl_image_t* image);

int register_io_bmp_module(void)
{
    static io_module_t module = {
        .exts   = "bmp:dip",
        .verify = bmp_verify,
        .load   = bmp_load,
        .save   = bmp_save,
    };
    return register_io_module(&module);
}

static int bmp_verify(void* fd)
{
    uint16_t type;
    fseek(fd, 0, SEEK_SET);
    fread(&type, 1, 2, fd);
    return type == BMP_TYPE;
}

static int bmp_load(void* fd, vgl_image_t* image)
{
    // TODO: Add more file load guards and file types.
    bmp_file_header_t bf = {0};
    bmp_info_header_t bi = {0};

    fseek(fd, 0, SEEK_SET);
    fread(&bf, 1, sizeof(bmp_file_header_t), fd);
    fread(&bi, 1, sizeof(bmp_info_header_t), fd);

    // No support for compressed files yet.
    if (bi.compression != 0)
        return 0;

    vgl_format_kind_e format;
    switch (bi.bpp) {
    case BMP_FORMAT_GRAY:
        format = VGL_FORMAT_GRAY8;
        break;
    case BMP_FORMAT_RGB:
        format = VGL_FORMAT_RGB8;
        break;
    case BMP_FORMAT_RGBA:
        format = VGL_FORMAT_RGBA8;
        break;
    default:
        return 0;
    }

    if (vgl_image_deinit(image) != 1)
        return 0;
    if (vgl_image_init_2d(image, bi.width, bi.height, format) != 1)
        return 0;

    const uint32_t padding = (4 - (image->extent[0] * image->channels) % 4) % 4;
    const uint32_t row_size = image->channels * image->extent[0] + padding;
    uint64_t column_offset = 0;
    uint8_t* data = image->data;
    fseek(fd, bf.offset, SEEK_SET);
    for (uint32_t i = 0; i < image->extent[1]; ++i) {
        fread(data + column_offset, image->channels, image->extent[0], fd);
        fseek(fd, padding, SEEK_CUR);
        column_offset += row_size;
    }

    return 1;
}

static int bmp_save(void* fd, const vgl_image_t* image)
{
    uint32_t offset = sizeof(bmp_file_header_t) + sizeof(bmp_info_header_t);
    uint16_t bpp = 0;

    switch (image->format) {
    case VGL_FORMAT_GRAY8:
        bpp = BMP_FORMAT_GRAY;
        break;
    case VGL_FORMAT_RGB8:
        bpp = BMP_FORMAT_RGB;
        break;
    case VGL_FORMAT_RGBA8:
        bpp = BMP_FORMAT_RGBA;
        break;
    default:
        return 0;
    }

    bmp_file_header_t bf = {
        .type = BMP_TYPE,
        .size = offset + image->length,
        .dummy = 0,
        .offset = offset,
    };
    bmp_info_header_t bi = {
        .size = sizeof(bmp_info_header_t),
        .width = image->extent[0],
        .height = image->extent[1],
        .planes = 1,
        .bpp = bpp,
        .compression = 0,
        .count = 0,
        .x_ppm = 0,
        .y_ppm = 0,
        .num_colors = 0,
        .important_colors = 0,
    };

    switch (image->format) {
    case VGL_FORMAT_GRAY8:
        bi.bpp = BMP_FORMAT_GRAY;
        break;
    case VGL_FORMAT_RGB8:
        bi.bpp = BMP_FORMAT_RGB;
        break;
    case VGL_FORMAT_RGBA8:
        bi.bpp = BMP_FORMAT_RGBA;
        break;
    default:
        return 0;
    }

    fseek(fd, 0, SEEK_SET);
    fwrite(&bf, sizeof(bmp_file_header_t), 1, fd);
    fwrite(&bi, sizeof(bmp_info_header_t), 1, fd);

    const uint32_t padding = (4 - (image->extent[0] * image->channels) % 4) % 4;
    const uint32_t row_size = image->channels * image->extent[0] + padding;
    uint64_t column_offset = 0;
    uint8_t* data = image->data;
    for (uint32_t i = 0; i < image->extent[1]; ++i) {
        fwrite(data + column_offset, image->channels, image->extent[0], fd);
        for (uint32_t j = 0; j < padding; ++j) fputc(0x00, fd);
        column_offset += row_size;
    }

    return 1;
}
