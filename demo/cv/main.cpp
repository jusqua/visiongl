#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "utils.hpp"

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("\nUsage: demo_benchmark_cv lena_1024.tiff 1000 /tmp\n\n");
        printf("In this example, will run the program for lena_1024.tiff in a \nloop with 1000 iterations. Output images will be stored in /tmp.\n\n");
        printf("Error: Bad number of arguments = %d. 3 arguments required.\n", argc-1);
        exit(1);
    }

    int nSteps = atoi(argv[2]);
    char* inFilename = argv[1];
    char* outPath = argv[3];
    char* outFilename = (char*) malloc(strlen(outPath) + 200);
    cv::Mat img = cv::imread(inFilename, 1);

    if (img.data == NULL)
    {
        std::string str("Error: File not found: ");
        str.append(inFilename);
        printf("%s",str.c_str());
    }
    cv::Mat out(img.cols, img.rows, CV_8UC3);

    cv::Mat cvkernel33 = (cv::Mat_<float>(3,3) << 1/9.0, 1/9.0, 1/9.0,
                                                  1/9.0, 1/9.0, 1/9.0,
    	                                          1/9.0, 1/9.0, 1/9.0);
    cv::Mat cvkernel55 = (cv::Mat_<float>(5,5) << 1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                                  1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                                  1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                                  1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                                  1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0);
    cv::Mat cverode33cross = getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    cv::Mat cverode33square = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    auto builder = BenchmarkBuilder();

    builder.attach({
        .name = "Blur 3x3",
        .func = [&] { cv::GaussianBlur(img, out, cv::Size(3, 3), 0); },
        .post = [&] {
            sprintf(outFilename, "%s%s", outPath, "/out_cv_blur33.tif");
            cv::imwrite(outFilename, out);
        }
    });

    builder.attach({
        .name = "Convolution Blur 3x3",
        .func = [&] { cv::filter2D(img, out, -1, cvkernel33); },
        .post = [&] {
            sprintf(outFilename, "%s%s", outPath, "/out_cv_conv33.tif");
            cv::imwrite(outFilename, out);
        }
    });

    builder.attach({
        .name = "Convolution Blur 5x5",
        .func = [&] { cv::filter2D(img, out, -1, cvkernel55); },
        .post = [&] {
            sprintf(outFilename, "%s%s", outPath, "/out_cv_conv55.tif");
            cv::imwrite(outFilename, out);
        }
    });

    builder.attach({
        .name = "Erode Cross 3x3",
        .func = [&] { cv::erode(img, out, cverode33cross); },
        .post = [&] {
            sprintf(outFilename, "%s%s", outPath, "/out_cv_erode33cross.tif");
            cv::imwrite(outFilename, out);
        }
    });

    builder.attach({
        .name = "Erode Square 3x3",
        .func = [&] { cv::erode(img, out, cverode33square); },
        .post = [&] {
            sprintf(outFilename, "%s%s", outPath, "/out_cv_erode33square.tif");
            cv::imwrite(outFilename, out);
        }
    });

    builder.attach({
        .name = "Invert",
        .func = [&] { cv::bitwise_not(img, out); },
        .post = [&] {
            sprintf(outFilename, "%s%s", outPath, "/out_cv_invert.tif");
            cv::imwrite(outFilename, out);
        }
    });

    builder.attach({
        .name = "Grayscale",
        .func = [&] {
            cv::cvtColor(img, out, cv::COLOR_BGR2GRAY);
            cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
        },
        .post = [&] {
            sprintf(outFilename, "%s%s", outPath, "/out_cv_grayscale.tif");
            cv::imwrite(outFilename, out);
        }
    });

    builder.attach({
        .name = "Copy",
        .func = [&] { img.copyTo(out); },
        .post = [&] {
            sprintf(outFilename, "%s%s", outPath, "/out_cv_copy.tif");
            cv::imwrite(outFilename, out);
        }
    });

    builder.run(nSteps);

    return 0;
}
