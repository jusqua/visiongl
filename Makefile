PROJECT = visiongl
TARGET  = lib$(PROJECT).so

ROOTPATH           = $(shell pwd)
BUILDPATH          = $(ROOTPATH)/build
RUNTIMEPATH        = $(ROOTPATH)/runtime
INCLUDEPATH        = $(ROOTPATH)/include
SOURCEPATH         = $(ROOTPATH)/src
SCRIPTSPATH        = $(ROOTPATH)/scripts

TARGETPATH = $(SOURCEPATH)/${PROJECT}
DEMOSPATH  = $(SOURCEPATH)/demo
TESTSPATH  = $(SOURCEPATH)/test

DEMOS   = $(shell find $(DEMOSPATH)/ -type f -name '*.cpp' -exec basename -- {} .cpp \;)
TESTS   = $(shell find $(TESTSPATH)/ -type f -name '*.cpp' -exec basename -- {} .cpp \;)
SOURCES = $(shell find $(TARGETPATH)/ -type f -name '*.cpp')

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

COMPILE_FLAG_RUNTIME_DEFINITION = -DVGL_RUNTIME_PATH=\"$(ROOTPATH)/runtime\"
INSTALLATION_RUNTIME_DEFINITION = -DVGL_RUNTIME_PATH=\"$(INSTALL_SHAREPATH)/$(PROJECT)/runtime\"

WITH_DEBUG = 1
WITH_CUDA = 0
WITH_OPENCL = 1
WITH_OPENCV = 1
WITH_GDCM = 1
WITH_DCMTK = 1
WITH_TIFF = 1

ifeq ($(WITH_DEBUG), 1)
	DEF   += -D__DEBUG__
	FLAGS += -g -pg -O0
else
	FLAGS += -O2
endif

ifeq ($(WITH_OPENCV), 1)
	DEF += -D__OPENCV__
	LD  += -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_imgcodecs
endif

ifeq ($(WITH_CUDA), 1)
	DEF  += -D__CUDA__
	FPIC += -Xcompiler -fPIC
	SOURCES = $(shell find $(TARGETPATH)/ -type f -name '*.cu)
endif

ifeq ($(WITH_OPENCL), 1)
	DEF += -D__OPENCL__
	LD  += -lOpenCL
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
	@echo "Generating compile flags for development environment"
	@rm -f /tmp/compile_flags.txt
	@echo $(LD) >> /tmp/compile_flags.txt
	@echo $(FLAGS) >> /tmp/compile_flags.txt
	@echo $(FPIC) >> /tmp/compile_flags.txt
	@echo $(DEF) >> /tmp/compile_flags.txt
	@echo $(COMPILE_FLAG_RUNTIME_DEFINITION) >> /tmp/compile_flags.txt
	@echo "# Generated compile_flags.txt from Makefile" > $(ROOTPATH)/compile_flags.txt
	@sed 's/ /\n/g' /tmp/compile_flags.txt >> $(ROOTPATH)/compile_flags.txt

all: wrapper runtime header lib
	@echo "Builds files are located at $(BUILDPATH)"

wrapper: cuda_wrapper frag_wrapper frag_bg_wrapper frag_stereo_wrapper cl_wrapper cl_nd_wrapper cl_mm_wrapper cl_bin_wrapper
	@echo "Every runtime file wrapped into source code"

header:
	@echo "Copying headers to build directory"
	@rm -rf $(OUTPUT_INCLUDEPATH)
	@mkdir -p $(OUTPUT_INCLUDEPATH)
	@cp -rf $(INCLUDEPATH)/$(PROJECT) $(OUTPUT_INCLUDEPATH)

.PHONY: runtime
runtime:
	@echo "Copying runtime to build directory"
	@rm -rf $(OUTPUT_SHAREPATH)
	@mkdir -p $(OUTPUT_SHAREPATH)
	@cp -rf $(RUNTIMEPATH) $(OUTPUT_SHAREPATH)

.PHONY: lib
lib:
	@echo "Compiling library to build directory"
	@mkdir -p $(OUTPUT_LIBPATH)
	@$(CC) $(FLAGS) $(FPIC) $(LD) $(DEF) $(INSTALLATION_RUNTIME_DEFINITION) -shared -o $(OUTPUT_LIBPATH)/$(LIB_NAME) $(SOURCES)

install:
	@echo "Installing library at path $(INSTALLPATH)"
	@rm -rf $(INSTALL_SHAREPATH)/$(PROJECT)
	@cp -rf $(OUTPUT_SHAREPATH) $(INSTALL_SHAREPATH)
	@rm -rf $(INSTALL_INCLUDEPATH)/$(PROJECT)
	@cp -rf $(OUTPUT_INCLUDEPATH) $(INSTALL_INCLUDEPATH)
	@cp -f  $(OUTPUT_LIBPATH)/$(LIB_NAME) $(INSTALL_LIB64PATH)
	@ln -sf $(INSTALL_LIB64PATH)/$(LIB_NAME) $(INSTALL_LIBPATH)/

.PHONY: $(DEMOS)
$(DEMOS):
	@echo "Building demo $@"
	mkdir -p $(OUTPUT_BINPATH)
	$(CC) $(FLAGS) $(FPIC) $(LD) -l$(PROJECT) $(DEF) -o $(OUTPUT_BINPATH)/demo_$@ $(DEMOSPATH)/$@.cpp

.PHONY: $(TESTS)
$(TESTS):
	@echo "Building test $@"
	@mkdir -p $(OUTPUT_BINPATH)
	@$(CC) $(FLAGS) $(FPIC) $(LD) -l$(PROJECT) $(DEF) -o $(OUTPUT_BINPATH)/test_$@ $(TESTSPATH)/$@.cpp

cuda_wrapper:
	@echo "Generating CUDA wrapper"
	@$(SCRIPTSPATH)/kernel2cu.pl -o $(TARGETPATH)/kernel2cu_shaders $(RUNTIMEPATH)/CUDA/*.kernel >> /dev/null 2>&1
	@mv $(TARGETPATH)/kernel2cu_shaders.hpp $(INCLUDEPATH)/$(PROJECT)

frag_wrapper:
	@echo "Generating FS wrapper"
	@$(SCRIPTSPATH)/glsl2cpp.pl -o $(TARGETPATH)/glsl2cpp_shaders -p FS $(RUNTIMEPATH)/FS/*.frag >> /dev/null 2>&1
	@mv $(TARGETPATH)/glsl2cpp_shaders.hpp $(INCLUDEPATH)/$(PROJECT)

frag_bg_wrapper:
	@echo "Generating FS_BG wrapper"
	@$(SCRIPTSPATH)/glsl2cpp.pl -o $(TARGETPATH)/glsl2cpp_BG -p FS_BG $(RUNTIMEPATH)/FS_BG/*.frag >> /dev/null 2>&1
	@mv $(TARGETPATH)/glsl2cpp_BG.hpp $(INCLUDEPATH)/$(PROJECT)

frag_stereo_wrapper:
	@echo "Generating FS_Stereo wrapper"
	@$(SCRIPTSPATH)/glsl2cpp.pl -o $(TARGETPATH)/glsl2cpp_Stereo -p FS_Stereo $(RUNTIMEPATH)/FS_Stereo/*.frag >> /dev/null 2>&1
	@mv $(TARGETPATH)/glsl2cpp_Stereo.hpp $(INCLUDEPATH)/$(PROJECT)

cl_wrapper:
	@echo "Generating CL wrapper"
	@$(SCRIPTSPATH)/cl2cpp.pl -o $(TARGETPATH)/cl2cpp_shaders -p CL $(RUNTIMEPATH)/CL/*.cl >> /dev/null 2>&1
	@mv $(TARGETPATH)/cl2cpp_shaders.hpp $(INCLUDEPATH)/$(PROJECT)

cl_nd_wrapper:
	@echo "Generating CL_ND wrapper"
	@$(SCRIPTSPATH)/cl2cpp.pl -o $(TARGETPATH)/cl2cpp_ND -p CL_ND $(RUNTIMEPATH)/CL_ND/*.cl >> /dev/null 2>&1
	@mv $(TARGETPATH)/cl2cpp_ND.hpp $(INCLUDEPATH)/$(PROJECT)

cl_mm_wrapper:
	@echo "Generating CL_MM wrapper"
	@$(SCRIPTSPATH)/cl2cpp.pl -o $(TARGETPATH)/cl2cpp_MM -p CL_MM $(RUNTIMEPATH)/CL_MM/*.cl >> /dev/null 2>&1
	@mv $(TARGETPATH)/cl2cpp_MM.hpp $(INCLUDEPATH)/$(PROJECT)

cl_bin_wrapper:
	@echo "Generating CL_BIN wrapper"
	@$(SCRIPTSPATH)/cl2cpp.pl -o $(TARGETPATH)/cl2cpp_BIN -p CL_BIN $(RUNTIMEPATH)/CL_BIN/*.cl >> /dev/null 2>&1
	@mv $(TARGETPATH)/cl2cpp_BIN.hpp $(INCLUDEPATH)/$(PROJECT)
