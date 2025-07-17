#include <visiongl/cl/image.hpp>
#include <visiongl/context.hpp>
#include <visiongl/cl/cl2cpp_shaders.hpp>
#include <visiongl/glsl2cpp_shaders.hpp>
#include <visiongl/common.hpp>
#include <string>
#include "utils.hpp"

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("\nUsage: demo_benchmark_cl lena_1024.tiff 1000 /tmp\n\n");
        printf("In this example, will run the program for lena_1024.tiff in a \nloop with 1000 iterations. Output images will be stored in /tmp.\n\n");
        printf("Error: Bad number of arguments = %d. 3 arguments required.\n", argc-1);
        exit(1);
    }
    vglClInit();
    vglClInteropSetFalse();

    int nSteps = atoi(argv[2]);
    char* inFilename = argv[1];
    char* outPath = argv[3];
    char* outFilename = (char*) malloc(strlen(outPath) + 200);

    VglImage* img = vglLoadImage(inFilename, CV_LOAD_IMAGE_UNCHANGED, 0);
    vglImage3To4Channels(img);

    if (img == NULL)
    {
        std::string str("Error: File not found: ");
        str.append(inFilename);
        printf("%s", str.c_str());
    }

    VglImage* out = vglCreateImage(img);

    // Convolution kernels
    float kernel33[3][3]    = { {1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f},
                                {2.0f/16.0f, 4.0f/16.0f, 2.0f/16.0f},
                                {1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f}, };
    float kernel55[5][5]    = { {1.0f/256.0f,  4.0f/256.0f,  6.0f/256.0f,  4.0f/256.0f, 1.0f/256.0f},
                                {4.0f/256.0f, 16.0f/256.0f, 24.0f/256.0f, 16.0f/256.0f, 4.0f/256.0f},
                                {6.0f/256.0f, 24.0f/256.0f, 36.0f/256.0f, 24.0f/256.0f, 6.0f/256.0f},
                                {4.0f/256.0f, 16.0f/256.0f, 24.0f/256.0f, 16.0f/256.0f, 4.0f/256.0f},
                                {1.0f/256.0f,  4.0f/256.0f,  6.0f/256.0f,  4.0f/256.0f, 1.0f/256.0f}, };
    float erodeMaskCross[9] = { 0, 1, 0, 1, 1, 1, 0, 1, 0 };
    float erodeMaskSquare[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };

    auto builder = BenchmarkBuilder();

    builder.attach({
        .name = "Blur 3x3",
        .func = [&] { vglClBlurSq3(img, out); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_blur33.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.attach({
        .name = "Convolution Blur 3x3",
        .func = [&] { vglClConvolution(img, out, (float*) kernel33, 3, 3); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_conv33.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.attach({
        .name = "Convolution Blur 5x5",
        .func = [&] { vglClConvolution(img, out, (float*) kernel55, 5, 5); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_conv55.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.attach({
        .name = "Erode Cross 3x3",
        .func = [&] { vglClErode(img, out, erodeMaskCross, 3, 3); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_erosion_cross.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.attach({
        .name = "Erode Square 3x3",
        .func = [&] { vglClErode(img, out, erodeMaskSquare, 3, 3); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_erosion_square.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.attach({
        .name = "Invert",
        .func = [&] { vglClInvert(img, out); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_invert.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.attach({
        .name = "Threshold",
        .func = [&] { vglClThreshold(img, out, 0.5); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_thresh.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.attach({
        .name = "Copy Device to Device",
        .func = [&] { vglClCopy(img, out); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_copy.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.attach({
        .name = "Grayscale",
        .func = [&] { vglClGrayscale(img, out); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_grayscale.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.attach({
        .name = "Copy Device to Device",
        .func = [&] { vglClCopy(img, out); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_copy.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.attach({
        .name = "Copy Host to Device",
        .func = [&] { vglClUpload(img); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_upload.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.attach({
        .name = "Copy Device to Host",
        .func = [&] { vglClDownload(img); },
        .post = [&] {
            vglCheckContext(out, VGL_RAM_CONTEXT);
            sprintf(outFilename, "%s%s", outPath, "/out_cl_download.tif");
            cvSaveImage(outFilename, out->ipl);
        }
    });

    builder.run(nSteps);

    return 0;
}
