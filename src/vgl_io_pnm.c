#include <ctype.h>
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "vgl_image.h"
#include "vgl_io.h"

typedef enum {
    PNM_UNKNOWN = -1,
    PNM_GRAY_ASCII = '2',
    PNM_RGB_ASCII = '3',
    PNM_GRAY_BINARY = '5',
    PNM_RGB_BINARY = '6',
    __PNM_KIND_LENGTH,
} pnm_kind_e;

static int pnm_verify(void* fd);
static int pnm_load(void* fd, vgl_image_t* image);
static int pnm_save(void* fd, const vgl_image_t* image);

static pnm_kind_e get_pnm_kind(void* fd);
static int skip_unused_data(void* fd, char* buffer, char** cursor, size_t size);
static pnm_kind_e convert_to_pnm_kind(vgl_format_kind_e format);

int register_io_pnm_module(void)
{
    static io_module_t module = {
        .exts   = "pnm:ppm:pgm", // TODO: Support for bitset images and thus PBM images
        .verify = pnm_verify,
        .load   = pnm_load,
        .save   = pnm_save,
    };
    return register_io_module(&module);
}

static int pnm_verify(void* fd)
{
    return get_pnm_kind(fd) != PNM_UNKNOWN;
}

static int pnm_load(void* fd, vgl_image_t* image)
{
    pnm_kind_e pnm_kind = get_pnm_kind(fd);
    struct {
        vgl_format_kind_e format;
        int is_ascii;
        int data[3];
        size_t count;
        size_t capacity;
    } info = {0};

    switch (pnm_kind) {
    case PNM_GRAY_ASCII:
        info.format = VGL_FORMAT_GRAY8;
        info.capacity = 3;
        info.is_ascii = 1;
        break;
    case PNM_RGB_ASCII:
        info.format = VGL_FORMAT_RGB8;
        info.capacity = 3;
        info.is_ascii = 1;
        break;
    case PNM_GRAY_BINARY:
        info.format = VGL_FORMAT_GRAY8;
        info.capacity = 3;
        info.is_ascii = 0;
        break;
    case PNM_RGB_BINARY:
        info.format = VGL_FORMAT_RGB8;
        info.capacity = 3;
        info.is_ascii = 0;
        break;
    default:
        return 0;
    }

    struct {
        char data[MAX_IO_BUFFER_SIZE];
        char* cursor;
        size_t size;
    } buffer = {0};
    buffer.size = MAX_IO_BUFFER_SIZE;
    buffer.cursor = buffer.data + buffer.size;

    while (info.count < info.capacity) {
        if (skip_unused_data(fd, buffer.data, &buffer.cursor, buffer.size) == 0)
            return 0;

        int offset;
        sscanf(buffer.cursor, "%d%n", &info.data[info.count++], &offset);
        buffer.cursor += offset;
    }

    if (vgl_image_deinit(image) != 1)
        return 0;
    if (vgl_image_init_2d(image, info.data[0], info.data[1], info.format) != 1)
        return 0;
    unsigned char maxval = info.data[2];
    unsigned char* data = image->data;

    if (info.is_ascii) {
        while ((size_t)(data - (unsigned char*)image->data) < image->length) {
            if (skip_unused_data(fd, buffer.data, &buffer.cursor, buffer.size) == 0)
                return 0;

            int px;
            int offset;
            sscanf(buffer.cursor, "%d%n", &px, &offset);
            *data = px > maxval ? maxval : px;
            buffer.cursor += offset;
            data++;
        }
    } else {
        while(*buffer.cursor != '\n') {
            buffer.cursor++;
            if ((size_t)(buffer.cursor - buffer.data) >= buffer.size) {
                if (fread(buffer.data, 1, buffer.size, fd) <= 0)
                    return 0;
                buffer.cursor = buffer.data;
            }
        }

        fseek(fd, -(MAX_IO_BUFFER_SIZE - (size_t)(buffer.cursor - buffer.data + 1)), SEEK_CUR);
        fread(data, 1, image->length, fd);
    }

    return 1;
}

static int pnm_save(void* fd, const vgl_image_t* image)
{
    pnm_kind_e kind = convert_to_pnm_kind(image->format);

    char buffer[MAX_IO_BUFFER_SIZE] = {0};
    int size = snprintf(buffer, MAX_IO_BUFFER_SIZE, "P%c\n%zu %zu\n255\n", kind, image->extent[0], image->extent[1]);

    fwrite(buffer, 1, size, fd);
    fwrite(image->data, 1, image->length, fd);
    return 1;
}

static pnm_kind_e get_pnm_kind(void* fd)
{
    unsigned char def[2];
    fseek(fd, 0, SEEK_SET);
    fread(def, 1, 2, fd);

    if (def[0] != 'P')
        return -1;

    int type = def[1];
    switch (def[1]) {
    case PNM_GRAY_BINARY:
    case PNM_GRAY_ASCII:
    case PNM_RGB_BINARY:
    case PNM_RGB_ASCII:
        return type;
    default:
        return PNM_UNKNOWN;
    }
}

static pnm_kind_e convert_to_pnm_kind(vgl_format_kind_e format)
{
    switch (format) {
    case VGL_FORMAT_GRAY8:
        return PNM_GRAY_BINARY;
    case VGL_FORMAT_RGB8:
    case VGL_FORMAT_RGBA8:
        return PNM_RGB_BINARY;
    default:
        return PNM_UNKNOWN;
    }
}

static int skip_unused_data(void* fd, char* buffer, char** cursor, size_t size)
{
    for (;;) {
        if ((size_t)(*cursor - buffer) >= size) {
            if (fread(buffer, 1, size, fd) <= 0)
                return 0;
            *cursor = buffer;
            continue;
        }
        if (isspace(**cursor)) {
            while (isspace(**cursor)) {
                (*cursor)++;
                if ((size_t)(*cursor - buffer) >= size) {
                    if (fread(buffer, 1, size, fd) <= 0)
                        return 0;
                    *cursor = buffer;
                }
            }
            continue;
        }
        if (**cursor == '#') {
            for (;;) {
                char* eol = strchr(*cursor, '\n');
                if (eol != NULL) {
                    *cursor = eol + 1;
                    break;
                }

                if (fread(buffer, 1, size, fd) <= 0)
                    return 0;
                *cursor = buffer;
            }
            continue;
        }
        break;
    }
    if ((size_t)(*cursor - buffer) + MAX_IO_CURSOR_ROLLBACK_SIZE >= size) {
        fseek(fd, -(MAX_IO_BUFFER_SIZE - (size_t)(*cursor - buffer)), SEEK_CUR);
        if (fread(buffer, 1, size, fd) <= 0)
            return 0;
        *cursor = buffer;
    }
    return 1;
}
