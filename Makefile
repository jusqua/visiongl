PROJECT  = visiongl
DEMOS = cuda fractal cam gdcm dcmtk cl3d clnd tiff io bin benchmark_cv benchmark_cvocl benchmark_cl benchmark_cl3d benchmark_clnd benchmark_MM benchmark_clbin benchmark_cl3dbin benchmark_clndbin benchmark_FuzzyTophat benchmark_MP colordeconv clinfo cltest image_info
TESTS = core cl

CC    = clang++
FLAGS = -Wall -Wextra -pedantic -Wno-narrowing -I./src
LD    = -lGLEW -lGLU -lGL -lglut
DEF   = -DGL_GLEXT_PROTOTYPES -DGLX_GLXEXT_PROTOTYPES
FPIC  = -fPIC
SRC   = src/glsl2cpp_shaders.cpp src/vglContext.cpp src/vglSimpleBGModel.cpp src/glsl2cpp_BG.cpp src/glsl2cpp_Stereo.cpp src/vglImage.cpp src/vglLoadShader.cpp src/vglGdcmIo.cpp src/vglDcmtkIo.cpp src/vglTiffIo.cpp src/vglDeconv.cpp src/iplImage.cpp src/vglOpencv.cpp src/vglShape.cpp src/vglStrEl.cpp

INSTALL_INCLUDEPATH = /usr/include/
INSTALL_LIBPATH     = /usr/lib64/

ROOTPATH           = $(PWD)
INCLUDE_PATH       = $(ROOTPATH)/src
INCLUDE_DIR        = -I $(INCLUDE_PATH)

BUILDPATH          = $(ROOTPATH)/build
OUTPUT_INCLUDEPATH = $(BUILDPATH)/include
OUTPUT_LIBPATH     = $(BUILDPATH)/lib
OUTPUT_BINPATH     = $(BUILDPATH)/bin
OUTPUT_INCLUDEDIR  = -I $(OUTPUT_INCLUDEPATH)
OUTPUT_LIBDIR      = -L $(OUTPUT_LIBPATH)

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

all: lib
	mkdir -p $(OUTPUT_INCLUDEPATH)
	cat $(INCLUDE_PATH)/vglHead.h $(INCLUDE_PATH)/vglImage.h $(INCLUDE_PATH)/vglCudaImage.h $(INCLUDE_PATH)/vglClImage.h $(INCLUDE_PATH)/vglGdcmIo.h $(INCLUDE_PATH)/vglDcmtkIo.h $(INCLUDE_PATH)/vglTiffIo.h $(INCLUDE_PATH)/vglContext.h $(INCLUDE_PATH)/vglSimpleBGModel.h $(INCLUDE_PATH)/glsl2cpp*.h $(INCLUDE_PATH)/kernel2cu*.h $(INCLUDE_PATH)/cl2cpp*.h $(INCLUDE_PATH)/vglClFunctions*.h $(INCLUDE_PATH)/iplImage*.h $(INCLUDE_PATH)/vglOpencv*.h $(INCLUDE_PATH)/vglTail.h $(INCLUDE_PATH)/vglDeconv.h > /tmp/$(BINARY_NAME).h; grep -v vglImage\.h /tmp/$(BINARY_NAME).h > $(OUTPUT_INCLUDEPATH)/$(BINARY_NAME).h

lib: cuda_wrapper frag_wrapper frag_bg_wrapper frag_stereo_wrapper cl_wrapper cl_nd_wrapper cl_mm_wrapper cl_bin_wrapper
	mkdir -p $(OUTPUT_LIBPATH)
	echo $(LD_LIBRARY_PATH)
	$(CC) $(FLAGS) $(FPIC) $(LD) $(DEF) -shared -o $(OUTPUT_LIBPATH)/lib$(PROJECT).so $(SRC)

install: all
	cp -f $(OUTPUT_INCLUDEPATH)/$(BINARY_NAME).h $(INSTALL_INCLUDEPATH)
	cp -f $(OUTPUT_LIBPATH)/lib$(BINARY_NAME).so $(INSTALL_LIBPATH)

dox: all
	doxygen $(BINARY_NAME).dox
	cd dox/latex; pwd; make; xdvi refman.dvi&

.PHONY: $(DEMOS)
$(DEMOS): binfolder
	$(CC) $(FLAGS) $(FPIC) $(LD) -l$(PROJECT) $(DEF) -o $(OUTPUT_BINPATH)/demo_$@ src/demo/$@.cpp src/demo/timer.cpp

.PHONY: $(TESTS)
$(TESTS): binfolder
	$(CC) $(FLAGS) $(FPIC) $(LD) -l$(PROJECT) $(DEF) -o $(OUTPUT_BINPATH)/test_$@ src/test/$@.cpp

binfolder:
	mkdir -p $(OUTPUT_BINPATH)
	ln -sf $(ROOTPATH)/runtime/CL $(OUTPUT_BINPATH)/
	ln -sf $(ROOTPATH)/runtime/CL_BIN $(OUTPUT_BINPATH)/
	ln -sf $(ROOTPATH)/runtime/CL_MM $(OUTPUT_BINPATH)/
	ln -sf $(ROOTPATH)/runtime/CL_ND $(OUTPUT_BINPATH)/
	ln -sf $(ROOTPATH)/runtime/cloud $(OUTPUT_BINPATH)/
	ln -sf $(ROOTPATH)/runtime/cloud_CL $(OUTPUT_BINPATH)/
	ln -sf $(ROOTPATH)/runtime/CL_UTIL $(OUTPUT_BINPATH)/
	ln -sf $(ROOTPATH)/runtime/CUDA $(OUTPUT_BINPATH)/
	ln -sf $(ROOTPATH)/runtime/FS $(OUTPUT_BINPATH)/
	ln -sf $(ROOTPATH)/runtime/FS_BG $(OUTPUT_BINPATH)/
	ln -sf $(ROOTPATH)/runtime/FS_Stereo $(OUTPUT_BINPATH)/
	ln -sf $(ROOTPATH)/runtime/VS $(OUTPUT_BINPATH)/

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
	./scripts/kernel2cu.pl -o src/kernel2cu_shaders runtime/CUDA/*.kernel

frag_wrapper:
	./scripts/glsl2cpp.pl -o src/glsl2cpp_shaders -p FS runtime/FS/*.frag

frag_bg_wrapper:
	./scripts/glsl2cpp.pl -o src/glsl2cpp_BG -p FS_BG runtime/FS_BG/*.frag

frag_stereo_wrapper:
	./scripts/glsl2cpp.pl -o src/glsl2cpp_Stereo -p FS_Stereo runtime/FS_Stereo/*.frag

cl_wrapper:
	./scripts/cl2cpp.pl -o src/cl2cpp_shaders -p CL runtime/CL/*.cl

cl_nd_wrapper:
	./scripts/cl2cpp.pl -o src/cl2cpp_ND -p CL_ND runtime/CL_ND/*.cl

cloud_cl_wrapper:
	./scripts/cl2cloud.pl -o src/cloud_CL runtime/CL/*.cl

cl_mm_wrapper:
	./scripts/cl2cpp.pl -o src/cl2cpp_MM -p CL_MM runtime/CL_MM/*.cl

cl_bin_wrapper:
	./scripts/cl2cpp.pl -o src/cl2cpp_BIN -p CL_BIN runtime/CL_BIN/*.cl

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
