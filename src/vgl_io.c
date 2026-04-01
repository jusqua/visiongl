#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vgl_image.h"
#include "vgl_io.h"

typedef struct io_module_node {
    io_module_t*           module;
    struct io_module_node* next;
} io_module_node_t;

static io_module_node_t* modules = NULL;
static int is_modules_loaded = 0;

void init_io_modules(void);

int register_io_module(io_module_t* module)
{
    io_module_node_t* node = malloc(sizeof(io_module_node_t));
    if (node == NULL)
        return 0;
    node->module = module;
    node->next = modules;
    modules = node;
    return 1;
}

static io_module_t* guess_io_module(const char* filename)
{
    if (!is_modules_loaded) {
        init_io_modules();
        is_modules_loaded = 1;
    }

    io_module_node_t* node = modules;
    char* file_ext = strrchr(filename, '.');
    if (file_ext == NULL)
        return NULL;
    file_ext++;

    while (node != NULL)
    {
        char* module_ext = (char*)node->module->exts;
        char* next_module_ext = NULL;
        do {
            next_module_ext = strchr(module_ext, ':');
            if (next_module_ext == NULL) next_module_ext = strchr(module_ext, '\0');

            if (memcmp(module_ext, file_ext, next_module_ext - module_ext) == 0)
                break;

            if (*next_module_ext == '\0') module_ext = NULL;
            else module_ext = next_module_ext + 1;
        } while (module_ext != NULL);

        if (module_ext != NULL)
            return node->module;

        node = node->next;
    }

    return NULL;
}

static io_module_t* find_io_module(void* fd, const char* filename)
{
    if (!is_modules_loaded) {
        init_io_modules();
        is_modules_loaded = 1;
    }

    io_module_node_t* node = modules;
    while (node != NULL) {
        if (node->module->verify(fd)) {
            return node->module;
        }
        node = node->next;
    }

    return filename ? guess_io_module(filename) : NULL;
}

int vgl_image_load(vgl_image_t* image, const char* filename)
{
    FILE* fd = fopen(filename, "rb");
    if (fd == NULL)
        return 0;

    io_module_t* module = find_io_module(fd, filename);
    if (module == NULL)
        return 0;

    int status = module->load(fd, image);

    fclose(fd);
    return status;
}

int vgl_image_save(const vgl_image_t* image, const char* filename)
{
    io_module_t* module = guess_io_module(filename);
    if (module == NULL)
        return 0;

    FILE* fd = fopen(filename, "wb");
    if (fd == NULL)
        return 0;

    int status = module->save(fd, image);

    fclose(fd);
    return status;
}
