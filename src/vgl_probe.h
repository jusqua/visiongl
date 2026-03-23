#ifndef VGL_PROBE_H
#define VGL_PROBE_H
#pragma once

#include "vgl_shape.h"

/// Probe types for window operations
typedef enum {
    VGL_PROBE_CUBE,
    VGL_PROBE_CROSS,
    VGL_PROBE_GAUSS,
    VGL_PROBE_MEAN,
    __VGL_PROBE_KIND_COUNT
} vgl_probe_kind_e;

/// Probe element for window operations
typedef struct {
    vgl_shape_t shape;    ///< Pointer to the shape of the probe
    float*      data;     ///< Pointer to the data of the probe, size of the data is defined in shape
} vgl_probe_t;

/// Initialize probe from data
void vgl_probe_init(vgl_probe_t* probe, const vgl_shape_t* shape);
/// Initialize probe based on another probe
void vgl_probe_init_similar(vgl_probe_t* probe, const vgl_probe_t* source);
/// Create probe from a probe kind and dimensions
void vgl_probe_init_from_kind(vgl_probe_t* probe, vgl_probe_kind_e kind, int dims);
/// Release probe from memory
void vgl_probe_deinit(vgl_probe_t* probe);

#endif // VGL_PROBE_H
