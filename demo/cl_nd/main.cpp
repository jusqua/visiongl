#include <visiongl/cl/cl2cpp_ND.hpp>
#include <visiongl/cl/cl2cpp_shaders.hpp>
#include <visiongl/cl/image.hpp>
#include <visiongl/context.hpp>
#include <visiongl/glsl2cpp_shaders.hpp>
#include <visiongl/image.hpp>

#include <visiongl/constants.hpp>
#include <visiongl/shape.hpp>
#include <visiongl/strel.hpp>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "utils.hpp"

void saveResult(VglImage *out, char *outString, char *outPath, const char *outFolder, int i_0) {
    char *cmd = (char *)malloc(strlen(outPath) + 255);
    char *outFilename = (char *)malloc(strlen(outPath) + 255);

    sprintf(cmd, "%s %s%s%s", MKDIR, outPath, DIRSEP, outFolder);
    int retval = system(cmd);
    if (retval) {
        fprintf(stderr, "%s: %s: Error = %d creating folder %s%s%s\n", __FILE__, __FUNCTION__, retval, outPath, DIRSEP, outFolder);
        exit(1);
    }

    sprintf(outFilename, outString, outPath, outFolder);
    vglSaveNdImage((char *)outFilename, out, i_0);

    free(cmd);
    free(outFilename);
}

int main(int argc, char *argv[]) {
    auto usage =
        "Usage: benchmark <input file> <index 0> <index n> <steps> <output folder> <d1> <d2> ... <dn>\n"
        "This program reads a stack of image files and saves the results after benchmarking some operations.\n"
        "\n"
        "Both input and output files require a printf-like integer format specifier (%d) which will be replaced by the integers\n"
        "from index 0 to index n\n"
        "Optionally it's possible to define an alternative geometry to the image by adding the dimension sizes after the output folder as follows.\n\n"
        "benchmark <input file> <index 0> <index n> <steps> <output folder> <d1> <d2> ... <dn>\n\n"
        "Leave d1 and d2 as zero to use instead the image dimensions obtained from the image file.\n";

    constexpr int argD1 = 6;

    if (argc < argD1) {
        std::printf("%s", usage);
        std::exit(EXIT_FAILURE);
    }

    char *inFilename = argv[1]; // name of the input file
    int i_0 = atoi(argv[2]);
    int i_n = atoi(argv[3]);
    int nSteps = atoi(argv[4]);
    char *outPath = argv[5]; // name of the output folder
    char *outFolder;
    char *outString = (char *)"%s/%s/%%05d.tif";

    vglClInit();

    int shape[VGL_ARR_SHAPE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int ndim = 3;
    if (argc == argD1) {
        shape[VGL_SHAPE_D3] = i_n - i_0 + 1;
    } else if ((argc == argD1 + 3) && (strcmp(argv[argD1 + 2], "-1d") == 0)) {
        ndim = 1;
        for (int i = 0; i < 2; i++) {
            shape[1 + i] = atoi(argv[argD1 + i]);
        }
    } else {
        ndim = argc - argD1;
        for (int i = 0; i < ndim; i++) {
            shape[1 + i] = atoi(argv[argD1 + i]);
        }
    }

    // start: Saving original shape
    char *tmpFilename = (char *)malloc(strlen(inFilename) + 256);
    sprintf(tmpFilename, inFilename, i_0);
    VglImage *imgSingle = vglLoadImage((char *)tmpFilename);
    int origShape[VGL_ARR_SHAPE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    origShape[VGL_SHAPE_NCHANNELS] = imgSingle->getNChannels();
    origShape[VGL_SHAPE_WIDTH] = imgSingle->getWidth();
    origShape[VGL_SHAPE_HEIGHT] = imgSingle->getHeight();
    origShape[VGL_SHAPE_LENGTH] = i_n - i_0 + 1;
    VglShape *origVglShape = new VglShape(origShape, 3);
    // end: Saving original shape

    VglImage *img = vglLoadNdImage((char *)inFilename, i_0, i_n, shape, ndim);

    // This call to vglClForceAsBuf makes img to be treated in OpenCL context as
    // buffer instead of image even if it has 2 or 3 dimensions. N-dimensional
    // data must be stored as buffer in order to use vglClNd* functions. Must call
    // vglClForceAsBuf right after img creation, and before creating out image
    // from img in order to propagate the clForceAsBuf property.
    vglClForceAsBuf(img);
    VglImage *out = vglCreateImage(img);
    VglImage *out2 = vglCreateImage(img);

    VglStrEl *seCross = new VglStrEl(VGL_STREL_CROSS, ndim);
    VglStrEl *seCube = new VglStrEl(VGL_STREL_CUBE, ndim);
    VglStrEl *seMean = new VglStrEl(VGL_STREL_MEAN, ndim);
    VglStrEl *seCube1 = new VglStrEl(VGL_STREL_CUBE, 1);
    VglStrEl *seMean1 = new VglStrEl(VGL_STREL_MEAN, 1);
    VglStrEl *seCubeArr[ndim + 1];
    VglStrEl *seMeanArr[ndim + 1];
    int diam = 3;
    float diamf = (float)diam;
    float dataCube[3] = {1.0f, 1.0f, 1.0f};
    float dataMean[3] = {1.0f / diamf, 1.0f / diamf, 1.0f / diamf};

    int strelShape[VGL_ARR_SHAPE_SIZE];
    for (int i = 0; i < VGL_ARR_SHAPE_SIZE; i++) {
        strelShape[i] = 1;
    }
    for (int i = 1; i <= ndim; i++) {
        strelShape[i] = diam;
        VglShape *tmpShape = new VglShape(strelShape, ndim);
        seCubeArr[i] = new VglStrEl(dataCube, tmpShape);
        seMeanArr[i] = new VglStrEl(dataMean, tmpShape);
        delete (tmpShape);
        strelShape[i] = 1;
    }

    auto post = [&] {
        vglCheckContext(out, VGL_RAM_CONTEXT);
        if (ndim <= 2) {
            vglReshape(out, origVglShape);
        }
    };

    auto builder = BenchmarkBuilder();

    builder.attach({
        .name = "Convolution",
        .func = [&] { vglClNdConvolution(img, out, seMean); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            if (ndim <= 2) {
                vglReshape(out, origVglShape);
            }
            saveResult(out, outString, outPath, "clnd_conv_mean", i_0);
        }
    });

    builder.attach({
        .name = "Convolution Separated",
        .func = [&] {
            vglClNdConvolution(img, out2, seMeanArr[1]);
            for (int i = 2; i <= ndim; i++) {
                if (i % 2 == 0)
                vglClNdConvolution(out2, out, seMeanArr[i]);
                else
                vglClNdConvolution(out, out2, seMeanArr[i]);
            }
            if (ndim % 2 == 1)
            vglClNdCopy(out2, out);
        },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            if (ndim <= 2) {
                vglReshape(out, origVglShape);
            }
            saveResult(out, outString, outPath, "clnd_conv_sep", i_0);
        }
    });

    builder.attach({
        .name = "Dilation Cube",
        .func = [&] { vglClNdDilate(img, out, seCube); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            if (ndim <= 2) {
                vglReshape(out, origVglShape);
            }
            saveResult(out, outString, outPath, "clnd_dilate_cube", i_0);
        }
    });

    builder.attach({
        .name = "Dilation Cube Separated",
        .func = [&] {
            vglClNdDilate(img, out2, seCubeArr[1]);
            for (int i = 2; i <= ndim; i++) {
                if (i % 2 == 0)
                vglClNdDilate(out2, out, seCubeArr[i]);
                else
                vglClNdDilate(out, out2, seCubeArr[i]);
            }
            if (ndim % 2 == 1)
            vglClNdCopy(out2, out);
        },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            if (ndim <= 2) {
                vglReshape(out, origVglShape);
            }
            saveResult(out, outString, outPath, "clnd_dilate_sep", i_0);
        }
    });

    builder.attach({
        .name = "Dilation Cross",
        .func = [&] { vglClNdDilate(img, out, seCross); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            if (ndim <= 2) {
                vglReshape(out, origVglShape);
            }
            saveResult(out, outString, outPath, "clnd_dilate_cross", i_0);
        }
    });

    builder.attach({
        .name = "Invertion",
        .func = [&] { vglClNdNot(img, out); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            if (ndim <= 2) {
                vglReshape(out, origVglShape);
            }
            saveResult(out, outString, outPath, "clnd_invert", i_0);
        }
    });

    builder.attach({
        .name = "Threshold",
        .func = [&] { vglClNdThreshold(img, out, 30); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            if (ndim <= 2) {
                vglReshape(out, origVglShape);
            }
            saveResult(out, outString, outPath, "clnd_thresh", i_0);
        }
    });

    builder.attach({
        .name = "Copy",
        .func = [&] {
            vglClNdCopy(img, out);
        },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            if (ndim <= 2) {
                vglReshape(out, origVglShape);
            }
            saveResult(out, outString, outPath, "clnd_copy", i_0);
        }
    });

    builder.attach({
        .name = "Upload",
        .func = [&] {
            vglSetContext(img, VGL_RAM_CONTEXT);
            vglClUpload(img);
        },
        .post = [&] {
            saveResult(out, outString, outPath, "clnd_upload", i_0);
        }
    });
    builder.attach({
        .name = "Download",
        .func = [&] {
            vglSetContext(img, VGL_CL_CONTEXT);
            vglClDownload(img);
        },
        .post = [&] {
            saveResult(out, outString, outPath, "clnd_download", i_0);
        }
    });

    builder.run(nSteps);

    return 0;
}
