PROJECT  = visiongl
DEMOS = cuda fractal cam gdcm dcmtk cl3d clnd tiff io bin benchmark_cv benchmark_cvocl benchmark_cl benchmark_cl3d benchmark_clnd benchmark_MM benchmark_clbin benchmark_cl3dbin benchmark_clndbin benchmark_FuzzyTophat benchmark_MP colordeconv clinfo cltest image_info
TESTS = core cl

ROOTPATH           = $(shell pwd)
BUILDPATH          = $(ROOTPATH)/build
RUNTIMEPATH        = $(ROOTPATH)/runtime
INCLUDEPATH        = $(ROOTPATH)/include
SOURCEPATH         = $(ROOTPATH)/src
SCRIPTSPATH        = $(ROOTPATH)/scripts
DEMOSPATH          = $(ROOTPATH)/demo

OUTPUT_INCLUDEPATH = $(BUILDPATH)/include
OUTPUT_LIBPATH     = $(BUILDPATH)/lib
OUTPUT_BINPATH     = $(BUILDPATH)/bin
OUTPUT_SHAREPATH   = $(BUILDPATH)/share/$(PROJECT)
OUTPUT_RUNTIMEPATH = $(OUTPUT_SHAREPATH)/runtime

INSTALL_INCLUDEPATH = /usr/include/
INSTALL_LIBPATH     = /usr/lib64/
INSTALL_SHAREPATH   = /usr/share/

CC    = clang++
FLAGS = -Wall -Wextra -pedantic -Wno-narrowing -I$(INCLUDEPATH)
LD    = -lGLEW -lGLU -lGL -lglut
DEF   = -DGL_GLEXT_PROTOTYPES -DGLX_GLXEXT_PROTOTYPES
FPIC  = -fPIC
SRC   = src/glsl2cpp_shaders.cpp src/vglContext.cpp src/vglSimpleBGModel.cpp src/glsl2cpp_BG.cpp src/glsl2cpp_Stereo.cpp src/vglImage.cpp src/vglLoadShader.cpp src/vglGdcmIo.cpp src/vglDcmtkIo.cpp src/vglTiffIo.cpp src/vglDeconv.cpp src/iplImage.cpp src/vglOpencv.cpp src/vglShape.cpp src/vglStrEl.cpp

COMPILE_FLAG_RUNTIME_DEFINITION = -DVGL_RUNTIME_PATH=\"$(ROOTPATH)/runtime\"
INSTALLATION_RUNTIME_DEFINITION = -DVGL_RUNTIME_PATH=\"$(INSTALL_SHAREPATH)$(PROJECT)/runtime\"

WITH_DEBUG = 0
WITH_CUDA = 0
WITH_OPENCL = 1
WITH_OPENCV = 0
WITH_GDCM = 1
WITH_DCMTK = 1
WITH_TIFF = 1

ifeq ($(WITH_DEBUG), 1)
	DEF   += -D__DEBUG__
	FLAGS += -g -pg
endif

ifeq ($(WITH_OPENCV), 1)
	DEF += -D__OPENCV__
	LD  += -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_legacy
endif

ifeq ($(WITH_CUDA), 1)
	DEF  += -D__CUDA__
	SRC  += src/*.cu
	FPIC += -Xcompiler -fPIC
endif

ifeq ($(WITH_OPENCL), 1)
	DEF += -D__OPENCL__
	LD  += -lOpenCL
	SRC += src/cl2cpp_shaders.cpp src/vglClFunctions.cpp src/vglClImage.cpp src/cl2cpp_MM.cpp src/cl2cpp_ND.cpp src/cl2cpp_BIN.cpp
endif

ifeq ($(WITH_GDCM), 1)
	DEF += -D__GDCM__
	LD  += -lgdcmCommon -lgdcmDICT -lgdcmDSED -lgdcmIOD -lgdcmjpeg8 -lgdcmjpeg12 -lgdcmjpeg16 -lgdcmMEXD -lgdcmMSFF -lsocketxx
endif

ifeq ($(WITH_DCMTK), 1)
	DEF += -D__DCMTK__
	LD  += -ldcmjpeg -lijg8 -lijg12  -lijg16 -ldcmimage -ldcmpstat -ldcmimgle -ldcmqrdb -ldcmnet -ldcmdata -loflog -lofstd -lz -lpthread
endif

ifeq ($(WITH_TIFF), 1)
	DEF += -D__TIFF__
	LD  += -ltiff
endif

setup:
	rm -f /tmp/compile_flags.txt
	echo $(LD) >> /tmp/compile_flags.txt
	echo $(FLAGS) >> /tmp/compile_flags.txt
	echo $(FPIC) >> /tmp/compile_flags.txt
	echo $(DEF) >> /tmp/compile_flags.txt
	echo $(COMPILE_FLAG_RUNTIME_DEFINITION) >> /tmp/compile_flags.txt
	echo "# Generated compile_flags.txt from Makefile" > $(ROOTPATH)/compile_flags.txt
	sed 's/ /\n/g' /tmp/compile_flags.txt >> $(ROOTPATH)/compile_flags.txt

all: lib
	mkdir -p $(OUTPUT_INCLUDEPATH)
	cat $(INCLUDEPATH)/vglHead.h $(INCLUDEPATH)/vglImage.h $(INCLUDEPATH)/vglCudaImage.h $(INCLUDEPATH)/vglClImage.h $(INCLUDEPATH)/vglGdcmIo.h $(INCLUDEPATH)/vglDcmtkIo.h $(INCLUDEPATH)/vglTiffIo.h $(INCLUDEPATH)/vglContext.h $(INCLUDEPATH)/vglSimpleBGModel.h $(INCLUDEPATH)/glsl2cpp*.h $(INCLUDEPATH)/kernel2cu*.h $(INCLUDEPATH)/cl2cpp*.h $(INCLUDEPATH)/vglClFunctions*.h $(INCLUDEPATH)/iplImage*.h $(INCLUDEPATH)/vglOpencv*.h $(INCLUDEPATH)/vglTail.h $(INCLUDEPATH)/vglDeconv.h > /tmp/$(PROJECT).h; grep -v vglImage\.h /tmp/$(PROJECT).h > $(OUTPUT_INCLUDEPATH)/$(PROJECT).h

runtime: cuda_wrapper frag_wrapper frag_bg_wrapper frag_stereo_wrapper cl_wrapper cl_nd_wrapper cl_mm_wrapper cl_bin_wrapper
	mkdir -p $(OUTPUT_SHAREPATH)
	cp -rf $(RUNTIMEPATH) $(OUTPUT_SHAREPATH)

lib: runtime
	mkdir -p $(OUTPUT_LIBPATH)
	$(CC) $(FLAGS) $(FPIC) $(LD) $(DEF) $(INSTALLATION_RUNTIME_DEFINITION) -shared -o $(OUTPUT_LIBPATH)/lib$(PROJECT).so $(SRC)

install:
	cp -rf $(OUTPUT_SHAREPATH) $(INSTALL_SHAREPATH)
	cp -f $(OUTPUT_INCLUDEPATH)/$(PROJECT).h $(INSTALL_INCLUDEPATH)
	cp -f $(OUTPUT_LIBPATH)/lib$(PROJECT).so $(INSTALL_LIBPATH)

dox: all
	doxygen $(BINARY_NAME).dox
	cd dox/latex; pwd; make; xdvi refman.dvi&

.PHONY: $(DEMOS)
$(DEMOS):
	mkdir -p $(OUTPUT_BINPATH)
	$(CC) $(FLAGS) $(FPIC) $(LD) -l$(PROJECT) $(DEF) -o $(OUTPUT_BINPATH)/demo_$@ $(DEMOSPATH)/$@.cpp

.PHONY: $(TESTS)
$(TESTS):
	mkdir -p $(OUTPUT_BINPATH)
	$(CC) $(FLAGS) $(FPIC) $(LD) -l$(PROJECT) $(DEF) -o $(OUTPUT_BINPATH)/test_$@ src/test/$@.cpp


# runtestcore:
# 	cd $(OUTPUT_BINPATH); ./test_$(CORE_NAME) $(ROOTPATH)/samples/dicom/0003.dcm.%d.PGM 0 16 /tmp

# runtestcl:
# 	cd $(OUTPUT_BINPATH); ./test_$(CL_NAME) $(ROOTPATH)/samples/lena_gray.tif

# rundemocl3d:
# 	cd $(OUTPUT_BINPATH); ./demo_$(CL3D_NAME) $(ROOTPATH)/samples/dicom/0003.dcm.%d.PGM 0 16 /tmp

# rundemoclnd:
# 	cd $(OUTPUT_BINPATH); ./demo_$(CLND_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(CLND_NAME) $(ROOTPATH)/samples/mitosis-5d%04d.tif 0 335 /tmp 256 86016 -1d
# 	cd $(OUTPUT_BINPATH); ./demo_$(CLND_NAME) $(ROOTPATH)/samples/mitosis-5d%04d.tif 0 335 /tmp 256 86016
# 	cd $(OUTPUT_BINPATH); ./demo_$(CLND_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 /tmp 0 0 336
# 	cd $(OUTPUT_BINPATH); ./demo_$(CLND_NAME) $(ROOTPATH)/samples/mitosis-5d%04d.tif 0 335 /tmp 0 0 2 168
# 	cd $(OUTPUT_BINPATH); ./demo_$(CLND_NAME) $(ROOTPATH)/samples/mitosis-5d%04d.tif 0 335 /tmp 0 0 2 24 7

# rundemofrac:
# 	cd $(OUTPUT_BINPATH); $(OUTPUT_BINPATH)/demo_$(FRACTAL_NAME)

# rundemocam:
# 	cd $(OUTPUT_BINPATH); $(OUTPUT_BINPATH)/demo_$(CAM_NAME)

# rundemocuda:
# 	cd $(OUTPUT_BINPATH); $(OUTPUT_BINPATH)/demo_$(CUDA_NAME) $(ROOTPATH)/samples/lena_std.tif /tmp/outputcuda.pgm

# rundemogdcm:
# 	cd $(OUTPUT_BINPATH); ./demo_$(GDCM_NAME) $(ROOTPATH)/samples/dicom/0003.dcm /tmp/outputgdcm.dcm
# 	cd $(OUTPUT_BINPATH); ./demo_$(GDCM_NAME) $(ROOTPATH)/samples/dicom/esopecho.dcm /tmp/outputgdcmrgb.dcm -c
# 	cd $(OUTPUT_BINPATH); ./demo_$(GDCM_NAME) $(ROOTPATH)/samples/dicom/I_000032.dcm /tmp/outputgdcmybr.dcm -c
# 	cd $(OUTPUT_BINPATH); ./demo_$(GDCM_NAME) $(ROOTPATH)/samples/dicom/0003.dcm.%d.PGM /tmp/outputgdcmheader.dcm 0 3
# 	cd $(OUTPUT_BINPATH); ./demo_$(GDCM_NAME) $(ROOTPATH)/samples/dicom/I_000003.dcm.%d.PGM /tmp/outputgdcmheadercor.dcm 0 3 -c
# 	cd $(OUTPUT_BINPATH); ./demo_$(GDCM_NAME) $(ROOTPATH)/samples/dicom/E1154S7I_16bit.dcm /tmp/outputgdcm16_compress.dcm -c
# 	cd $(OUTPUT_BINPATH); ./demo_$(GDCM_NAME) $(ROOTPATH)/samples/dicom/E1154S7I_16bit.dcm /tmp/outputgdcm16.dcm

# rundemodcmtk:
# 	cd $(OUTPUT_BINPATH); ./demo_$(DCMTK_NAME) $(ROOTPATH)/samples/dicom/0003.dcm /tmp/outputdcmtk.dcm -c
# 	cd $(OUTPUT_BINPATH); ./demo_$(DCMTK_NAME) $(ROOTPATH)/samples/dicom/esopecho.dcm /tmp/outputdcmtkrgb.dcm -c
# 	cd $(OUTPUT_BINPATH); ./demo_$(DCMTK_NAME) $(ROOTPATH)/samples/dicom/I_000032.dcm /tmp/outputdcmtkybr.dcm -c
# 	cd $(OUTPUT_BINPATH); ./demo_$(DCMTK_NAME) $(ROOTPATH)/samples/dicom/0003.dcm.%d.PGM /tmp/outputdcmtkheader.dcm 0 3 -c
# 	cd $(OUTPUT_BINPATH); ./demo_$(DCMTK_NAME) $(ROOTPATH)/samples/dicom/I_000003.dcm.%d.PGM /tmp/outputdcmtkheadercor.dcm 0 3 -c
# 	cd $(OUTPUT_BINPATH); ./demo_$(DCMTK_NAME) $(ROOTPATH)/samples/dicom/E1154S7I_16bit.dcm /tmp/outputdcmtk16_compress.dcm -c
# 	cd $(OUTPUT_BINPATH); ./demo_$(DCMTK_NAME) $(ROOTPATH)/samples/dicom/E1154S7I_16bit.dcm /tmp/outputdcmtk16.dcm

# rundemotiff:
# 	cd $(OUTPUT_BINPATH); ./demo_$(TIFF_NAME) $(ROOTPATH)/samples/tif/E1154S7I_3d.tif /tmp/demo_tif_out.tif

# rundemoio:
# 	cd $(OUTPUT_BINPATH); ./demo_$(IO_NAME) $(ROOTPATH)/samples/lena_std.tif /tmp/lena_std.tif -1
# 	cd $(OUTPUT_BINPATH); ./demo_$(IO_NAME) $(ROOTPATH)/samples/lena_gray.tif /tmp/lena_gray.tif -1
# 	cd $(OUTPUT_BINPATH); ./demo_$(IO_NAME) $(ROOTPATH)/samples/tif/E1154S7I_001.tif /tmp/E1154S7I_001.tif -1

# 	cd $(OUTPUT_BINPATH); ./demo_$(IO_NAME) $(ROOTPATH)/samples/lena_std.tif /tmp/lena_std.ppm -1
# 	cd $(OUTPUT_BINPATH); ./demo_$(IO_NAME) $(ROOTPATH)/samples/lena_gray.tif /tmp/lena_gray.pgm -1

# 	cd $(OUTPUT_BINPATH); ./demo_$(IO_NAME) /tmp/lena_std.ppm /tmp/lena_std_UNCHANGED.ppm -1
# 	cd $(OUTPUT_BINPATH); ./demo_$(IO_NAME) /tmp/lena_gray.pgm /tmp/lena_gray_UNCHANGED.pgm -1

# 	cd $(OUTPUT_BINPATH); ./demo_$(IO_NAME) /tmp/lena_std.ppm /tmp/lena_std_GRAYSCALE.pgm 0
# 	cd $(OUTPUT_BINPATH); ./demo_$(IO_NAME) /tmp/lena_gray.pgm /tmp/lena_gray_GRAYSCALE.pgm 0

# 	cd $(OUTPUT_BINPATH); ./demo_$(IO_NAME) /tmp/lena_std.ppm /tmp/lena_std_BGR.ppm 1
# 	cd $(OUTPUT_BINPATH); ./demo_$(IO_NAME) /tmp/lena_gray.pgm /tmp/lena_gray_BGR.ppm 1

# rundemobin:
# 	cd $(OUTPUT_BINPATH); ./demo_$(BIN_NAME) $(ROOTPATH)/samples/test_mm.pgm /tmp 0
# 	cd $(OUTPUT_BINPATH); ./demo_$(BIN_NAME) $(ROOTPATH)/samples/lena_std.ppm /tmp 0

# rundemobinconway:
# 	cd $(OUTPUT_BINPATH); ./demo_$(BIN_NAME) $(ROOTPATH)/samples/test_conway.pgm /tmp 40
# 	cd /tmp; convert -delay 1 -loop 0 demo_bin_conway*.pbm animation.gif
# 	cd /tmp; convert animation.gif -filter box -resize 400% animation4x.gif

# rundemobenchmark:
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CV_NAME) $(ROOTPATH)/samples/lena_std.tif 10 /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CVOCL_NAME) $(ROOTPATH)/samples/lena_std.tif 10 /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CL_NAME) $(ROOTPATH)/samples/lena_std.tif 1 /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CL3D_NAME) $(ROOTPATH)/samples/dicom/0003.dcm.%d.PGM 0 16 1 /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_MM_NAME) $(ROOTPATH)/samples/01_test.tif 10 /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_FUZZYTOPHAT_NAME) $(ROOTPATH)/samples/01_test.tif 10 /tmp

# STEPS=100
# rundemobenchmarkclnd:
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLND_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 2048 10752
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLND_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 256 86016 -1d
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLND_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 256 86016
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLND_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 0 0 336
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLND_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 0 0 2 168
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLND_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 0 0 2 24 7

# rundemobenchmark16:
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CV_NAME) $(ROOTPATH)/samples/E1154S7I_001.tif $(STEPS) /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CVOCL_NAME) $(ROOTPATH)/samples/lena_std.tif $(STEPS) /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CL_NAME) $(ROOTPATH)/samples/E1154S7I_001.tif $(STEPS)  /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CL3D_NAME) $(ROOTPATH)/samples/E1154S7I_00%d.tif 1 7 $(STEPS) /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_MM_NAME) $(ROOTPATH)/samples/E1154S7I_001.tif $(STEPS) /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_FUZZYTOPHAT_NAME) $(ROOTPATH)/samples/E1154S7I_001.tif $(STEPS) /tmp

# rundemobenchmarkclbin:
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLBIN_NAME) $(ROOTPATH)/samples/lena_std.pgm $(STEPS) /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLBIN_NAME) ~/Dropbox/_UFS/_APAGAR/out_00000.pgm $(STEPS) /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLBIN_NAME) $(ROOTPATH)/samples/test_mm.pgm $(STEPS) /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CL3DBIN_NAME) $(ROOTPATH)/samples/dicom/0003.dcm.%d.PGM 0 16 $(STEPS) /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CL3DBIN_NAME) $(ROOTPATH)/samples/test_mm.pgm 0 1 $(STEPS) /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CL3DBIN_NAME) $(ROOTPATH)/samples/lena_std.pgm 0 1 $(STEPS) /tmp
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLNDBIN_NAME) $(ROOTPATH)/samples/lena_gray_500.pgm 0 0 $(STEPS) /tmp 500 512
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLNDBIN_NAME) $(ROOTPATH)/samples/lena_gray.tif 0 0 $(STEPS) /tmp 512 512
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLNDBIN_NAME) $(ROOTPATH)/samples/test_mm.pgm 0 0 $(STEPS) /tmp 0 0
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLNDBIN_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 2048 10752
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLNDBIN_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 256 86016 -1d
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLNDBIN_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 256 86016
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLNDBIN_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 0 0 336
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLNDBIN_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 0 0 2 168
# 	cd $(OUTPUT_BINPATH); ./demo_$(BENCHMARK_CLNDBIN_NAME) $(ROOTPATH)/samples/mitosis/mitosis-5d%04d.tif 0 335 $(STEPS) /tmp 0 0 2 24 7

# rundemocolordeconv:
# 	cd $(OUTPUT_BINPATH); ./demo_$(COLORDECONV_NAME) $(ROOTPATH)/samples/Emphysema_H_and_E.jpg 1 /tmp/image.%d.pgm 0.0 1.0 1.0 1.0 0.0 1.0 1.0 1.0 0.0

# runcltest:
# 	./src/demo/cltest

# runclinfo:
# 	./src/demo/clinfo

# runutils:
# 	./src/demo/image_info images/lena_std.tif -p

# runtestcam:
# 	cd $(OUTPUT_BINPATH); ./$(TESTCAM_NAME)

cuda_wrapper:
	$(SCRIPTSPATH)/kernel2cu.pl -o $(SOURCEPATH)/kernel2cu_shaders $(RUNTIMEPATH)/CUDA/*.kernel
	mv $(SOURCEPATH)/kernel2cu_shaders.h $(INCLUDEPATH)

frag_wrapper:
	$(SCRIPTSPATH)/glsl2cpp.pl -o $(SOURCEPATH)/glsl2cpp_shaders -p FS $(RUNTIMEPATH)/FS/*.frag
	mv $(SOURCEPATH)/glsl2cpp_shaders.h $(INCLUDEPATH)

frag_bg_wrapper:
	$(SCRIPTSPATH)/glsl2cpp.pl -o $(SOURCEPATH)/glsl2cpp_BG -p FS_BG $(RUNTIMEPATH)/FS_BG/*.frag
	mv $(SOURCEPATH)/glsl2cpp_BG.h $(INCLUDEPATH)

frag_stereo_wrapper:
	$(SCRIPTSPATH)/glsl2cpp.pl -o $(SOURCEPATH)/glsl2cpp_Stereo -p FS_Stereo $(RUNTIMEPATH)/FS_Stereo/*.frag
	mv $(SOURCEPATH)/glsl2cpp_Stereo.h $(INCLUDEPATH)

cl_wrapper:
	$(SCRIPTSPATH)/cl2cpp.pl -o $(SOURCEPATH)/cl2cpp_shaders -p CL $(RUNTIMEPATH)/CL/*.cl
	mv $(SOURCEPATH)/cl2cpp_shaders.h $(INCLUDEPATH)

cl_nd_wrapper:
	$(SCRIPTSPATH)/cl2cpp.pl -o $(SOURCEPATH)/cl2cpp_ND -p CL_ND $(RUNTIMEPATH)/CL_ND/*.cl
	mv $(SOURCEPATH)/cl2cpp_ND.h $(INCLUDEPATH)

cl_mm_wrapper:
	$(SCRIPTSPATH)/cl2cpp.pl -o $(SOURCEPATH)/cl2cpp_MM -p CL_MM $(RUNTIMEPATH)/CL_MM/*.cl
	mv $(SOURCEPATH)/cl2cpp_MM.h $(INCLUDEPATH)

cl_bin_wrapper:
	$(SCRIPTSPATH)/cl2cpp.pl -o $(SOURCEPATH)/cl2cpp_BIN -p CL_BIN $(RUNTIMEPATH)/CL_BIN/*.cl
	mv $(SOURCEPATH)/cl2cpp_BIN.h $(INCLUDEPATH)

# cloud_cl_wrapper:
# 	$(SCRIPTSPATH)/cl2cloud.pl -o $(SOURCEPATH)/cloud_CL $(RUNTIMEPATH)/CL/*.cl
# 	mv $(SOURCEPATH)/kernel2cu_shaders.h $(INCLUDEPATH)

# CLOUD_DIRS   := src/cloud
# CLOUD_SOURCE := $(foreach dir, $(CLOUD_DIRS), $(wildcard $(dir)/*))
# CLOUD_BIN     = $(CLOUD_SOURCE:%.cpp=%)
# CLOUD_BASE    = $(notdir $(basename $(CLOUD_SOURCE) ) )

# cloud: $(CLOUD_BASE)

# %: %.cpp
# 	$(CC) $*.cpp -o $* $(OPENCL_DEF) $(OUTPUT_INCLUDEDIR) $(OPENCV_INCLUDEDIR) $(OPENCL_INCLUDEDIR) $(OUTPUT_LIBDIR) $(OPENCV_LIBDIR) $(OPENCV_LIBRARIES) -lvisiongl

# runcloud:
# 	cd src/cloud; ./template_for_cloud --input $(ROOTPATH)/samples/lena_std.tif --output /tmp/lixo.tif --window_convolution "[.2, .2, .2, .2, .2]" --window_size_x 1 --window_size_y 5

# lala:
# 	file := $(foreach file, $(files), echo $(file))
# 	export LINUXAMD64_CLOUD="$(CC) $(COMPILER_FLAGS) \
#                         -o $(OUTPUT_BINPATH)/cloud/$(CLOUD_NAME) \
#                          src/cloud/$(CLOUD_NAME).cpp \
#                          -lvisiongl"
# 	$(LINUXAMD64_CLOUD)
