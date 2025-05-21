#ifndef VGL_OPENCV_IO_HPP
#define VGL_OPENCV_IO_HPP

#include "visiongl/common.hpp"
#include <visiongl/image.hpp>
#include <opencv2/core/core.hpp>

namespace vgl {
namespace opencv {

/**
 * @brief Convert an OpenCV cv::Mat to a VglImage
 * 
 * @param mat OpenCV matrix to convert
 * @param ndim Dimensionality of the output image (default: 2)
 * @param has_mipmap Whether to create mipmaps (default: 0)
 * @return VglImage* Newly allocated VglImage containing the converted data
 */
VglImage* mat_to_vgl(const cv::Mat& mat, int ndim = 2, int has_mipmap = 0);

/**
 * @brief Convert a VglImage to an OpenCV cv::Mat
 * 
 * @param image VglImage to convert
 * @return cv::Mat Newly allocated cv::Mat containing the converted data
 */
cv::Mat vgl_to_mat(VglImage* image);

/**
 * @brief Load an image file using OpenCV and convert to VglImage
 * 
 * @param filename Path to the image file
 * @param flags OpenCV loading flags (e.g., CV_LOAD_IMAGE_GRAYSCALE)
 * @param ndim Dimensionality of the output image (default: 2)
 * @param has_mipmap Whether to create mipmaps (default: 0)
 * @return VglImage* Newly allocated VglImage containing the loaded image
 */
VglImage* load_image(const char* filename, int flags = CV_LOAD_IMAGE_UNCHANGED, int ndim = 2, int has_mipmap = 0);

/**
 * @brief Save a VglImage to a file using OpenCV
 * 
 * @param filename Path where the image will be saved
 * @param image VglImage to save
 * @param params Optional parameters for the specific format
 * @return int 0 if successful, error code otherwise
 */
int save_image(const char* filename, VglImage* image, const std::vector<int>& params = std::vector<int>());

} // namespace opencv
} // namespace vgl

#endif // VGL_OPENCV_IO_HPP
