PROJECT  = visiongl
DEMOS = cuda fractal cam gdcm dcmtk cl3d clnd tiff io bin benchmark_cv benchmark_cvocl benchmark_cl benchmark_cl3d benchmark_clnd benchmark_MM benchmark_clbin benchmark_cl3dbin benchmark_clndbin benchmark_FuzzyTophat colordeconv clinfo cltest image_info
TESTS = core cl

ROOTPATH           = $(shell pwd)
BUILDPATH          = $(ROOTPATH)/build
RUNTIMEPATH        = $(ROOTPATH)/runtime
INCLUDEPATH        = $(ROOTPATH)/include
SOURCEPATH         = $(ROOTPATH)/src
SCRIPTSPATH        = $(ROOTPATH)/scripts

PROJECTPATH        = $(SOURCEPATH)/${PROJECT}
DEMOSPATH          = $(SOURCEPATH)/demo
TESTSPATH          = $(SOURCEPATH)/test

OUTPUT_INCLUDEPATH = $(BUILDPATH)/include/$(PROJECT)
OUTPUT_LIBPATH     = $(BUILDPATH)/lib
OUTPUT_BINPATH     = $(BUILDPATH)/bin
OUTPUT_SHAREPATH   = $(BUILDPATH)/share/$(PROJECT)
OUTPUT_RUNTIMEPATH = $(OUTPUT_SHAREPATH)/runtime

ifndef INSTALLPATH
INSTALLPATH = /usr/local
endif
INSTALL_INCLUDEPATH = $(INSTALLPATH)/include
INSTALL_LIB64PATH   = $(INSTALLPATH)/lib64
INSTALL_LIBPATH     = $(INSTALLPATH)/lib
INSTALL_SHAREPATH   = $(INSTALLPATH)/share

LIB_NAME = lib$(PROJECT).so

CC    = clang++
FLAGS = -Wall -Wextra -pedantic -Wno-narrowing -I$(INCLUDEPATH)
LD    = -lGLEW -lGLU -lGL -lglut
DEF   = -DGL_GLEXT_PROTOTYPES -DGLX_GLXEXT_PROTOTYPES
FPIC  = -fPIC
SRC   = ${PROJECTPATH}/glsl2cpp_shaders.cpp ${PROJECTPATH}/vglContext.cpp ${PROJECTPATH}/vglSimpleBGModel.cpp ${PROJECTPATH}/glsl2cpp_BG.cpp ${PROJECTPATH}/glsl2cpp_Stereo.cpp ${PROJECTPATH}/vglImage.cpp ${PROJECTPATH}/vglLoadShader.cpp ${PROJECTPATH}/vglGdcmIo.cpp ${PROJECTPATH}/vglDcmtkIo.cpp ${PROJECTPATH}/vglTiffIo.cpp ${PROJECTPATH}/vglDeconv.cpp ${PROJECTPATH}/iplImage.cpp ${PROJECTPATH}/vglOpencv.cpp ${PROJECTPATH}/vglShape.cpp ${PROJECTPATH}/vglStrEl.cpp

COMPILE_FLAG_RUNTIME_DEFINITION = -DVGL_RUNTIME_PATH=\"$(ROOTPATH)/runtime\"
INSTALLATION_RUNTIME_DEFINITION = -DVGL_RUNTIME_PATH=\"$(INSTALL_SHAREPATH)/$(PROJECT)/runtime\"

WITH_DEBUG = 0
WITH_CUDA = 0
WITH_OPENCL = 1
WITH_OPENCV = 1
WITH_GDCM = 1
WITH_DCMTK = 1
WITH_TIFF = 1

ifeq ($(WITH_DEBUG), 1)
	DEF   += -D__DEBUG__
	FLAGS += -g -pg
endif

ifeq ($(WITH_OPENCV), 1)
	DEF += -D__OPENCV__
	LD  += -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_imgcodecs
endif

ifeq ($(WITH_CUDA), 1)
	DEF  += -D__CUDA__
	SRC  += ${PROJECTPATH}/*.cu
	FPIC += -Xcompiler -fPIC
endif

ifeq ($(WITH_OPENCL), 1)
	DEF += -D__OPENCL__
	LD  += -lOpenCL
	SRC += ${PROJECTPATH}/cl2cpp_shaders.cpp ${PROJECTPATH}/vglClFunctions.cpp ${PROJECTPATH}/vglClImage.cpp ${PROJECTPATH}/cl2cpp_MM.cpp ${PROJECTPATH}/cl2cpp_ND.cpp ${PROJECTPATH}/cl2cpp_BIN.cpp
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

all: header lib
	@echo "Builds files are located at $(BUILDPATH)"

header:
	rm -rf $(OUTPUT_INCLUDEPATH)
	cp -rf $(INCLUDEPATH)/$(PROJECT) $(OUTPUT_INCLUDEPATH)

runtime: cuda_wrapper frag_wrapper frag_bg_wrapper frag_stereo_wrapper cl_wrapper cl_nd_wrapper cl_mm_wrapper cl_bin_wrapper
	rm -rf $(OUTPUT_SHAREPATH)
	mkdir -p $(OUTPUT_SHAREPATH)
	cp -rf $(RUNTIMEPATH) $(OUTPUT_SHAREPATH)

lib: runtime
	mkdir -p $(OUTPUT_LIBPATH)
	$(CC) $(FLAGS) $(FPIC) $(LD) $(DEF) $(INSTALLATION_RUNTIME_DEFINITION) -shared -o $(OUTPUT_LIBPATH)/$(LIB_NAME) $(SRC)
	
install:
	rm -rf $(INSTALL_SHAREPATH)/$(PROJECT)
	cp -rf $(OUTPUT_SHAREPATH) $(INSTALL_SHAREPATH)
	rm -rf $(INSTALL_INCLUDEPATH)/$(PROJECT)
	cp -rf $(OUTPUT_INCLUDEPATH) $(INSTALL_INCLUDEPATH)
	cp -f  $(OUTPUT_LIBPATH)/$(LIB_NAME) $(INSTALL_LIB64PATH)
	ln -sf $(INSTALL_LIB64PATH)/$(LIB_NAME) $(INSTALL_LIBPATH)/
	ldconfig

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
	$(CC) $(FLAGS) $(FPIC) $(LD) -l$(PROJECT) $(DEF) -o $(OUTPUT_BINPATH)/test_$@ $(TESTSPATH)/$@.cpp

cuda_wrapper:
	$(SCRIPTSPATH)/kernel2cu.pl -o $(PROJECTPATH)/kernel2cu_shaders $(RUNTIMEPATH)/CUDA/*.kernel
	mv $(PROJECTPATH)/kernel2cu_shaders.h $(INCLUDEPATH)/$(PROJECT)

frag_wrapper:
	$(SCRIPTSPATH)/glsl2cpp.pl -o $(PROJECTPATH)/glsl2cpp_shaders -p FS $(RUNTIMEPATH)/FS/*.frag
	mv $(PROJECTPATH)/glsl2cpp_shaders.h $(INCLUDEPATH)/$(PROJECT)

frag_bg_wrapper:
	$(SCRIPTSPATH)/glsl2cpp.pl -o $(PROJECTPATH)/glsl2cpp_BG -p FS_BG $(RUNTIMEPATH)/FS_BG/*.frag
	mv $(PROJECTPATH)/glsl2cpp_BG.h $(INCLUDEPATH)/$(PROJECT)

frag_stereo_wrapper:
	$(SCRIPTSPATH)/glsl2cpp.pl -o $(PROJECTPATH)/glsl2cpp_Stereo -p FS_Stereo $(RUNTIMEPATH)/FS_Stereo/*.frag
	mv $(PROJECTPATH)/glsl2cpp_Stereo.h $(INCLUDEPATH)/$(PROJECT)

cl_wrapper:
	$(SCRIPTSPATH)/cl2cpp.pl -o $(PROJECTPATH)/cl2cpp_shaders -p CL $(RUNTIMEPATH)/CL/*.cl
	mv $(PROJECTPATH)/cl2cpp_shaders.h $(INCLUDEPATH)/$(PROJECT)

cl_nd_wrapper:
	$(SCRIPTSPATH)/cl2cpp.pl -o $(PROJECTPATH)/cl2cpp_ND -p CL_ND $(RUNTIMEPATH)/CL_ND/*.cl
	mv $(PROJECTPATH)/cl2cpp_ND.h $(INCLUDEPATH)/$(PROJECT)

cl_mm_wrapper:
	$(SCRIPTSPATH)/cl2cpp.pl -o $(PROJECTPATH)/cl2cpp_MM -p CL_MM $(RUNTIMEPATH)/CL_MM/*.cl
	mv $(PROJECTPATH)/cl2cpp_MM.h $(INCLUDEPATH)/$(PROJECT)

cl_bin_wrapper:
	$(SCRIPTSPATH)/cl2cpp.pl -o $(PROJECTPATH)/cl2cpp_BIN -p CL_BIN $(RUNTIMEPATH)/CL_BIN/*.cl
	mv $(PROJECTPATH)/cl2cpp_BIN.h $(INCLUDEPATH)/$(PROJECT)
