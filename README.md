        VisionGL

The objective of this library is to provide an easy way to prototype and
implement image and video processing functions in CUDA, GLSL and OpenCL.
The library provides wrapper code generators for all the three languages.

The wrapper functions are generated in C++ from the shader code. Shader
codes written in CUDA or GLSL must contain three types of directives in
comments:
1 - Documentation comment: a multiple line comment between /** and */
    This comment is copied ipsis literis to the output files.
2 - Declaration comment: a single line comment describing the parameter
    list of the wrapper function. The list has variable name, type, and, when
    it is an image, semantics indicating if it is an input or output image.
3 - Attribution comment: single line comments associating variables in C++
    to its counterparts in GLSL or CUDA

Shader code written in OpenCL require only the two first kinds of comments.

The wrapper functions use a common variable type to store image data, called
VglImage. It contains space to store the image data in RAM, and handlers to
store the image in CUDA, OpenGL and OpenCL contexts. These contexts
are used under demand. If the image is used only in OpenGL context, the
CUDA handler will be empty (NULL or -1) and vice-versa.

        Compilation

The library requires GLEW 1.6 or superior and freeglut 3 or superior.
OpenGL support by the hardware is also required!

To install GLEW:
apt-get install libglew1.6 libglew1.6-dev

To install freeglut:
apt-get install freeglut3 freeglut3-dev

To install OpenCV:
apt-get install libopencv-dev

        Compilation

The library can be compiled with support for CUDA, OpenCL, OpenCV, Tiff, GDCM or DCMTK.
Enable compilation by changing the variables inside Makefile.

To build the library:
make all

To install the library to INSTALL_PATH
make install

To build the demos:
make democam
make demofrac
make demobenchmark
make demoio
make demotiff
make demogdcm
make demodcmtk
make demobin

To run the demos:
make rundemocam
make rundemofrac
make rundemobenchmark
make rundemoio
make rundemotiff
make rundemogdcm
make rundemodcmtk
make rundemobin

To build the documentation:
build dox

To generate wrapper code after some shader is modified:
make cl
make frag
make frag_bg
make frag_stereo
make frag_diffeq
make cuda

        About the demos

demofrac: Probably the most basic demo. Does not require any aditional
library besides the minimum. Use the arrows in the numpad to navigate,
 z/x to zoom in/out, and q to quit.

democam: Another basic demo. Requires OpenCV to obtain images from the
webcam. Captures images from the first webcam found. The capture is done by
OpenCV function cvCaptureFromCAM. An OpenGL window shows four versions
of the captured image after a few operations.

demoio and demotiff: To run these examples, please compile VisionGL with
TIFF support.

demogdcm: To run this example, please compile VisionGL with
GDCM support. In a previous section there are instructions about how
to compile GDCM.

demodcmtk: To run this example, please compile VisionGL with
DCMTK support. In a previous section there are instructions about how
to compile DCMTK.

demobenchmark: This demo is composed by six programs.
 - demobenchmark_cv: The simplest one, requires OpenCV.
 - demobenchmark_cl: To run this example, please compile VisionGL with
                     OpenCL support.
 - demobenchmark_cvocl: To run this example, please compile OpenCV with
                     OpenCL support. Download OpenCV and look for the
                     folder modules/ocl. It may be not present in the
                     master branch from the git repository, but it is
                     present in the 2.4.8 and 2.4.9 tags. Get OpenCV
                     with git by running:

                     git clone https://github.com/opencv/opencv.git
                     git checkout 2.4.9

 - demobenchmark_cl3d: To run this example, please compile VisionGL with
                     OpenCL support.
                     Your device must support the extension
                     cl_khr_3d_image_writes. This extension is available
                     in most AMD video cards. Please use the function
                     clGetDeviceInfo with the option CL_DEVICE_EXTENSIONS
                     to check the extension availability. An alternative is
                     to look for the card model in compupench.org and look
                     for the card details.

 - demobenchmark_mm: To run this example, please compile VisionGL with
                     OpenCL support.

 - demobenchmark_fuzzytophat: To run this example, please compile VisionGL with
                     OpenCL support.

demobenchmark_clnd: To run this example, please compile VisionGL with
OpenCL support.

demobin: Examples of operations on binary images (1bpp). To run this
example, please compile VisionGL with OpenCL support.

References

[1] Daniel Oliveira Dantas, Helton Danilo Passos Leal, Davy Oliveira Barros Sousa
Fast 2D and 3D image processing with OpenCL
International Conference on Image Processing (ICIP), 2015, IEEE, Quebec City.
http://dx.doi.org/10.1109/ICIP.2015.7351730
http://ieeexplore.ieee.org/xpl/articleDetails.jsp?reload=true&arnumber=7351730

[2] Daniel Oliveira Dantas, Helton Danilo Passos Leal, Davy Oliveira Barros Sousa
Fast multidimensional image processing with OpenCL
International Conference on Image Processing (ICIP), 2016, IEEE, Phoenix City.
http://doi.org/10.1109/ICIP.2016.7532664
http://ieeexplore.ieee.org/document/7532664/

[3] Daniel Oliveira Dantas, Junior Barrera
Automatic generation of wrapper code for video processing functions
Learning and Nonlinear models, Vol 9, Num 2, 2011
https://www.academia.edu/4367451/Automatic_generation_of_wrapper_code_for_video_processing_functions
http://www.deti.ufc.br/~lnlm/papers/vol9-no2-art5.pdf
