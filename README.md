# VisionGL

The objective of this library is to provide an easy way to prototype and implement image and video processing functions in CUDA, GLSL and OpenCL.
The library provides wrapper code generators for all the three languages.

## Usage

### Dependencies

#### Tools

- [CMake](https://cmake.org/)
- [Clang](https://clang.llvm.org/) or other C++17 compiler
- [Ninja](https://ninja-build.org/) or other build system
- [Perl](https://www.perl.org/)

#### Libraries

Required:

- [freeglut](https://freeglut.sourceforge.net/) for OpenGL support
- [GLEW](https://glew.sourceforge.net/) for OpenGL support and window management

Optional:

- [OpenCL](https://github.com/KhronosGroup/OpenCL-SDK) (enabled by default) (unsafe to disable)
- [OpenCV](https://opencv.org/) for image loading and conversion (enabled by default)
- [libtiff](https://libtiff.gitlab.io/libtiff/) for TIFF support
- [GDCM](https://sourceforge.net/projects/gdcm/) for DICOM support
- [DCMTK](https://git.dcmtk.org/) for DICOM support (unsafe to enable)
- [CUDA](https://developer.nvidia.com/cuda-toolkit) for CUDA support, only for NVIDIA (unsafe to enable)

> [!IMPORTANT]
> If OpenCV support is disabled, at least enable TIFF or GDCM support to load images. Otherwise, the library will not be able to load images.

### Build

```shell
cmake -G Ninja -S . -B build -D CMAKE_CXX_COMPILER=clang++ # or other C++17 compiler
cmake --build build
```

### Install

```shell
# Setup install path in the first step, /usr/local is the default
sudo cmake --install build # or without sudo if install path is set to a non-protected folder
```

## Research

VisionGL is a research project with the following publications:

- Daniel Oliveira Dantas, Helton Danilo Passos Leal, Davy Oliveira Barros Sousa Fast 2D and 3D image processing with OpenCL International Conference on Image Processing (ICIP), 2015, IEEE, Quebec City. DOI: http://dx.doi.org/10.1109/ICIP.2015.7351730
- Daniel Oliveira Dantas, Helton Danilo Passos Leal, Davy Oliveira Barros Sousa Fast multidimensional image processing with OpenCL International Conference on Image Processing (ICIP), 2016, IEEE, Phoenix City. DOI: http://doi.org/10.1109/ICIP.2016.7532664
- Daniel Oliveira Dantas, Junior Barrera Automatic generation of wrapper code for video processing functions Learning and Nonlinear models, Vol 9, Num 2, 2011. DOI: https://doi.org/10.21528/LMLN-VOL9-NO2-ART5
