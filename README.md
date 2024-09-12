# VisionGL

The objective of this library is to provide an easy way to prototype and implement image and video processing functions in CUDA, GLSL and OpenCL.
The library provides wrapper code generators for all the three languages.

The wrapper functions are generated in C++ from the shader code. Shader codes written in CUDA or GLSL must contain three types of directives in comments:
1. Documentation comment: a multiple line comment between `/** <doc comment> */`. This comment is copied ipsis literis to the output files.
2. Declaration comment: a single line comment describing the parameter list of the wrapper function. The list has variable name, type, and, when it is an image, semantics indicating if it is an input or output image.
3. Attribution comment: single line comments associating variables in C++ to its counterparts in GLSL or CUDA.

## Build

To build the library:
```shell
make all
```

To build and install the library:
```shell
make install
```

To build the library and documentation:
```shell
make dox
```

## Library demo

To build the demo you can use make file with `make <demo name>` and run with `make run<demo name>`:

- demofrac: Probably the most basic demo. Does not require any aditional library besides the minimum. Use the arrows in the numpad to navigate, z/x to zoom in/out, and q to quit.

- democam: Another basic demo. Requires OpenCV to obtain images from the webcam. Captures images from the first webcam found. The capture is done by OpenCV function cvCaptureFromCAM. An OpenGL window shows four versions of the captured image after a few operations.

- demoio: To run these examples, please compile VisionGL with TIFF support.

- demotiff: To run these examples, please compile VisionGL with TIFF support.

- demogdcm: To run this example, please compile VisionGL with GDCM support.

- demodcmtk: To run this example, please compile VisionGL with DCMTK support.

- demobenchmark: This demo is composed by six programs.
  - demobenchmark_cv: The simplest one, requires OpenCV.
  - demobenchmark_cl: To run this example, please compile VisionGL with OpenCL support.
  - demobenchmark_cvocl: To run this example, please compile OpenCV with OpenCL support. Download OpenCV and look for the folder modules/ocl. It may be not present in the master branch from the git repository, but it is present in the 2.4.8 and 2.4.9 tags.
  - demobenchmark_cl3d: To run this example, please compile VisionGL with OpenCL support. Your device must support the extension `cl_khr_3d_image_writes`. This extension is available in most AMD video cards. Please use the function `clGetDeviceInfo` with the option `CL_DEVICE_EXTENSIONS` to check the extension availability. An alternative is to look for the card model in compupench.org and look for the card. details.
  - demobenchmark_mm: To run this example, please compile VisionGL with OpenCL support.
  - demobenchmark_fuzzytophat: To run this example, please compile VisionGL with OpenCL support.

- demobenchmark_clnd: To run this example, please compile VisionGL with OpenCL support.
- demobin: Examples of operations on binary images (1bpp). To run this example, please compile VisionGL with OpenCL support.

## References

1. Daniel Oliveira Dantas, Helton Danilo Passos Leal, Davy Oliveira Barros Sousa. Fast 2D and 3D image processing with OpenCL. International Conference on Image Processing (ICIP), 2015, IEEE, Quebec City.
    - http://dx.doi.org/10.1109/ICIP.2015.7351730
    - https://ieeexplore.ieee.org/document/7351730

2. Daniel Oliveira Dantas, Helton Danilo Passos Leal, Davy Oliveira Barros Sousa. Fast multidimensional image processing with OpenCL. International Conference on Image Processing (ICIP), 2016, IEEE, Phoenix City.
    - http://doi.org/10.1109/ICIP.2016.7532664
    - https://ieeexplore.ieee.org/document/7532664/

3. Daniel Oliveira Dantas, Junior Barrera. Automatic generation of wrapper code for video processing functions. Learning and Nonlinear models, Vol 9, Num 2, 2011.
    - https://www.academia.edu/4367451

## License

Distributed under the Apache 2.0 License. See `LICENSE` for more information.
