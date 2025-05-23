#include <visiongl/context.hpp>
#include <visiongl/opencv/io.hpp>
#include <opencv2/imgcodecs.hpp>
#include <cstring>

namespace vgl {
namespace opencv {

/**
 * Convert an OpenCV cv::Mat to a VglImage
 */
VglImage* mat_to_vgl(const cv::Mat& mat_img, int ndim, int has_mipmap)
{
    // Get matrix properties
    int width = mat_img.cols;
    int height = mat_img.rows;
    int depth;
    int nChannels = mat_img.channels();
    
    // Map OpenCV depth to VGL depth
    switch (mat_img.depth()) {
        case CV_8U:
            depth = IPL_DEPTH_8U;
            break;
        case CV_8S:
            depth = IPL_DEPTH_8S;
            break;
        case CV_16U:
            depth = IPL_DEPTH_16U;
            break;
        case CV_16S:
            depth = IPL_DEPTH_16S;
            break;
        case CV_32S:
            depth = IPL_DEPTH_32S;
            break;
        case CV_32F:
            depth = IPL_DEPTH_32F;
            break;
        default:
            fprintf(stderr, "%s:%s: Error: Unsupported OpenCV depth.\n", __FILE__, __FUNCTION__);
            return NULL;
    }

    // Create VglImage with the same dimensions and type
    VglImage* vgl_img = NULL;
    if (ndim == 2) {
        // For 2D image
        CvSize size = cvSize(width, height);
        vgl_img = vglCreateImage(size, depth, nChannels, ndim, has_mipmap);
    }
    else if (ndim == 3) {
        // For 3D image (single frame, treat as 3D with length=1)
        CvSize size = cvSize(width, height);
        vgl_img = vglCreate3dImage(size, depth, nChannels, 1, has_mipmap);
    }
    else {
        fprintf(stderr, "%s:%s: Error: Unsupported number of dimensions: %d.\n", __FILE__, __FUNCTION__, ndim);
        return NULL;
    }

    if (vgl_img == NULL) {
        fprintf(stderr, "%s:%s: Error: Could not create VglImage.\n", __FILE__, __FUNCTION__);
        return NULL;
    }

    // Copy data from Mat to VglImage
    size_t mat_data_size = mat_img.elemSize() * mat_img.total();
    size_t vgl_data_size = vgl_img->getTotalSizeInBytes();

    if (mat_data_size == vgl_data_size) {
        // Direct copy if sizes match
        memcpy(vgl_img->getImageData(), mat_img.data, mat_data_size);
    } 
    else {
        // Copy row by row to account for possible padding differences
        for (int y = 0; y < height; y++) {
            memcpy(
                vgl_img->getImageData() + y * vgl_img->getWidthStep(), 
                mat_img.ptr(y), 
                mat_img.elemSize() * width
            );
        }
    }
    
    vglSetContext(vgl_img, VGL_RAM_CONTEXT);
    
    return vgl_img;
}

/**
 * Convert a VglImage to an OpenCV cv::Mat
 */
cv::Mat vgl_to_mat(VglImage* image)
{
    if (image == NULL) {
        fprintf(stderr, "%s:%s: Error: NULL image.\n", __FILE__, __FUNCTION__);
        return cv::Mat();
    }
    
    vglCheckContext(image, VGL_RAM_CONTEXT);

    // Map VGL depth to OpenCV depth
    int cv_depth;
    switch (image->depth) {
        case IPL_DEPTH_8U:
            cv_depth = CV_8U;
            break;
        case IPL_DEPTH_8S:
            cv_depth = CV_8S;
            break;
        case IPL_DEPTH_16U:
            cv_depth = CV_16U;
            break;
        case IPL_DEPTH_16S:
            cv_depth = CV_16S;
            break;
        case IPL_DEPTH_32S:
            cv_depth = CV_32S;
            break;
        case IPL_DEPTH_32F:
            cv_depth = CV_32F;
            break;
        default:
            fprintf(stderr, "%s:%s: Error: Unsupported VGL depth: %d.\n", __FILE__, __FUNCTION__, image->depth);
            return cv::Mat();
    }
    
    // Create output matrix
    cv::Mat mat_img;
    
    if (image->ndim <= 2) {
        // For 2D images
        mat_img = cv::Mat(image->getHeight(), image->getWidth(), CV_MAKETYPE(cv_depth, image->getNChannels()));
        
        // Copy data row by row to account for possible padding differences
        for (int y = 0; y < image->getHeight(); y++) {
            memcpy(
                mat_img.ptr(y),
                image->getImageData() + y * image->getWidthStep(),
                mat_img.elemSize() * image->getWidth()
            );
        }
    } 
    else if (image->ndim == 3) {
        // For 3D images, convert to a multi-channel 2D matrix if length is 1
        // Otherwise, create a 3D matrix
        if (image->getLength() == 1) {
            mat_img = cv::Mat(image->getHeight(), image->getWidth(), CV_MAKETYPE(cv_depth, image->getNChannels()));
            
            // Copy data row by row
            for (int y = 0; y < image->getHeight(); y++) {
                memcpy(
                    mat_img.ptr(y),
                    image->getImageData() + y * image->getWidthStep(),
                    mat_img.elemSize() * image->getWidth()
                );
            }
        } 
        else {
            // Create a 3D matrix using OpenCV's multi-dimensional matrix
            int sizes[] = {image->getLength(), image->getHeight(), image->getWidth()};
            mat_img = cv::Mat(3, sizes, CV_MAKETYPE(cv_depth, image->getNChannels()));
            
            // Calculate total size and copy all data at once if padding matches
            size_t vgl_data_size = image->getTotalSizeInBytes();
            size_t mat_data_size = mat_img.total() * mat_img.elemSize();
            
            if (mat_data_size == vgl_data_size) {
                memcpy(mat_img.data, image->getImageData(), vgl_data_size);
            } 
            else {
                // Copy frame by frame, row by row
                size_t frame_size = image->getHeight() * image->getWidthStep();
                size_t row_size = image->getWidth() * image->getNChannels() * (image->depth >> 3);
                
                for (int z = 0; z < image->getLength(); z++) {
                    for (int y = 0; y < image->getHeight(); y++) {
                        memcpy(
                            mat_img.ptr(z, y),
                            image->getImageData() + z * frame_size + y * image->getWidthStep(),
                            row_size
                        );
                    }
                }
            }
        }
    } 
    else {
        fprintf(stderr, "%s:%s: Error: Unsupported image dimensionality: %d.\n", __FILE__, __FUNCTION__, image->ndim);
        return cv::Mat();
    }
    
    return mat_img;
}

/**
 * Load an image file using OpenCV and convert to VglImage
 */
VglImage* load_image(const char* filename, int flags, int ndim, int has_mipmap)
{
    // Load image using OpenCV
    cv::Mat mat = cv::imread(filename, flags);
    if (mat.empty()) {
        fprintf(stderr, "%s:%s: Error: Could not load image: %s\n", __FILE__, __FUNCTION__, filename);
        return NULL;
    }
    
    // Convert to VglImage
    VglImage* vgl_img = mat_to_vgl(mat, ndim, has_mipmap);
    if (vgl_img) {
        // Store filename in the VglImage
        vgl_img->filename = strdup(filename);
    }
    
    return vgl_img;
}

/**
 * Save a VglImage to a file using OpenCV
 */
int save_image(const char* filename, VglImage* image, const std::vector<int>& params)
{
    if (!image) {
        fprintf(stderr, "%s:%s: Error: NULL image.\n", __FILE__, __FUNCTION__);
        return -1;
    }
    
    // Convert VglImage to cv::Mat
    cv::Mat mat = vgl_to_mat(image);
    if (mat.empty()) {
        fprintf(stderr, "%s:%s: Error: Failed to convert image.\n", __FILE__, __FUNCTION__);
        return -1;
    }
    
    // Save using OpenCV
    bool success = cv::imwrite(filename, mat, params);
    if (!success) {
        fprintf(stderr, "%s:%s: Error: Failed to save image: %s\n", __FILE__, __FUNCTION__, filename);
        return -1;
    }
    
    return 0;
}

} // namespace opencv
} // namespace vgl