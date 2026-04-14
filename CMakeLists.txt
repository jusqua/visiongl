cmake_minimum_required(VERSION 3.10)
project(visiongl LANGUAGES C)

add_library(visiongl)
target_compile_features(visiongl PRIVATE c_std_11)

# TODO: Auto generated modules based on user input
file(WRITE src/vgl_io_modules.generated.c "//! This file is auto generated. Any changes will be overriten.
int register_io_pnm_module(void);
int register_io_bmp_module(void);

void init_io_modules(void)
{
	register_io_pnm_module();
	register_io_bmp_module();
}
")

file(GLOB_RECURSE INCLUDES "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.c")
target_include_directories(visiongl PRIVATE src)
target_sources(visiongl PRIVATE ${SOURCES} ${INCLUDES})
