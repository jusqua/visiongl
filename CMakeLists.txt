cmake_minimum_required(VERSION 3.15)

project(visiongl LANGUAGES CXX VERSION 0.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(VGL_DEBUG_MODE ON CACHE BOOL "Enable debug mode" FORCE)
else()
    set(VGL_DEBUG_MODE OFF CACHE BOOL "Enable debug mode" FORCE)
endif()

option(VGL_USE_CUDA "Enable CUDA support" OFF)
option(VGL_USE_OPENCV "Enable OpenCV support" ON)
option(VGL_USE_OPENCL "Enable OpenCL support" ON)
option(VGL_USE_DCMTK "Enable DCMTK support" OFF) # Unstable, needs futher attention
option(VGL_USE_GDCM "Enable GDCM support" ON)
option(VGL_USE_TIFF "Enable TIFF support" ON)

message(STATUS "${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build configuration:")
message(STATUS "  - Debug mode: ${VGL_DEBUG_MODE}")
message(STATUS "  - CUDA support: ${VGL_USE_CUDA}")
message(STATUS "  - OpenCL support: ${VGL_USE_OPENCL}")
message(STATUS "  - OpenCV support: ${VGL_USE_OPENCV}")
message(STATUS "  - GDCM support: ${VGL_USE_GDCM}")
message(STATUS "  - TIFF support: ${VGL_USE_TIFF}")

# Required default dependencies
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

set(ALL_LIBRARY_SOURCES "")
set(GENERATED_FILES "")

# Define runtime path for wrapper generation
set(RUNTIME_PATH ${CMAKE_CURRENT_SOURCE_DIR}/runtime)
macro(add_wrapper_generation SCRIPT_NAME BASENAME PREFIX INPUT_GLOB SUBDIR)
    set(INTERMEDIATE_PATH ${CMAKE_BINARY_DIR}/wrapper/${PREFIX})
    set(PERL_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${SCRIPT_NAME}.pl)
    set(INCLUDE_PATH_BASE ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME})
    set(SOURCE_PATH_BASE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    set(FINAL_INCLUDE_PATH ${INCLUDE_PATH_BASE})
    set(FINAL_SOURCE_PATH ${SOURCE_PATH_BASE})

    if(NOT ${SUBDIR} STREQUAL "")
        set(FINAL_INCLUDE_PATH ${FINAL_INCLUDE_PATH}/${SUBDIR})
        set(FINAL_SOURCE_PATH ${FINAL_SOURCE_PATH}/${SUBDIR})
    endif()

    file(GLOB INPUT_FILES "${INPUT_GLOB}")

    set(OUTPUT_HEADER ${FINAL_INCLUDE_PATH}/${BASENAME}.hpp)
    if(${PREFIX} STREQUAL "CUDA")
        set(OUTPUT_SOURCE_EXTENSION cu)
    else()
        set(OUTPUT_SOURCE_EXTENSION cpp)
    endif()
    set(OUTPUT_SOURCE ${FINAL_SOURCE_PATH}/${BASENAME}.${OUTPUT_SOURCE_EXTENSION})

    set(SCRIPT_GENERATED_BASENAME ${INTERMEDIATE_PATH}/${BASENAME})
    set(SCRIPT_GENERATED_HEADER ${SCRIPT_GENERATED_BASENAME}.hpp)
    set(SCRIPT_GENERATED_SOURCE ${SCRIPT_GENERATED_BASENAME}.${OUTPUT_SOURCE_EXTENSION})

    add_custom_command(
        OUTPUT ${OUTPUT_HEADER} ${OUTPUT_SOURCE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${INTERMEDIATE_PATH}
        COMMAND perl -I${CMAKE_CURRENT_SOURCE_DIR}/scripts ${PERL_SCRIPT} -o ${SCRIPT_GENERATED_BASENAME} -p ${PREFIX} ${INPUT_FILES}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${FINAL_INCLUDE_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${FINAL_SOURCE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${SCRIPT_GENERATED_HEADER} ${OUTPUT_HEADER}
        COMMAND ${CMAKE_COMMAND} -E copy ${SCRIPT_GENERATED_SOURCE} ${OUTPUT_SOURCE}
        DEPENDS ${INPUT_FILES} ${PERL_SCRIPT}
        COMMENT "Generating ${BASENAME} from ${SCRIPT_NAME}.pl (source files named ${PREFIX})"
        VERBATIM
    )

    list(APPEND GENERATED_FILES ${OUTPUT_HEADER} ${OUTPUT_SOURCE})
endmacro()

set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB COMMON_SOURCES CONFIGURE_DEPENDS ${COMMON_DIR}/*.cpp)
list(APPEND ALL_LIBRARY_SOURCES ${COMMON_SOURCES})

add_wrapper_generation(glsl2cpp glsl2cpp_shaders FS ${RUNTIME_PATH}/FS/*.frag "")
add_wrapper_generation(glsl2cpp glsl2cpp_BG FS_BG ${RUNTIME_PATH}/FS_BG/*.frag "")
add_wrapper_generation(glsl2cpp glsl2cpp_Stereo FS_Stereo ${RUNTIME_PATH}/FS_Stereo/*.frag "")

if(VGL_USE_CUDA)
    find_package(CUDA REQUIRED)
    set(CUDA_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/cuda)
    file(GLOB_RECURSE CUDA_MODULE_SOURCES CONFIGURE_DEPENDS ${CUDA_MODULE_DIR}/*.cu)
    list(APPEND ALL_LIBRARY_SOURCES ${CUDA_MODULE_SOURCES})
    add_wrapper_generation(kernel2cu kernel2cu_shaders CUDA ${RUNTIME_PATH}/CUDA/*.kernel cuda)
endif()

if(VGL_USE_OPENCL)
    find_package(OpenCL REQUIRED)
    set(OPENCL_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/cl)
    file(GLOB_RECURSE OPENCL_MODULE_SOURCES CONFIGURE_DEPENDS ${OPENCL_MODULE_DIR}/*.cpp)
    list(APPEND ALL_LIBRARY_SOURCES ${OPENCL_MODULE_SOURCES})
    add_wrapper_generation(cl2cpp cl2cpp_shaders CL ${RUNTIME_PATH}/CL/*.cl cl)
    add_wrapper_generation(cl2cpp cl2cpp_ND CL_ND ${RUNTIME_PATH}/CL_ND/*.cl cl)
    add_wrapper_generation(cl2cpp cl2cpp_MM CL_MM ${RUNTIME_PATH}/CL_MM/*.cl cl)
    add_wrapper_generation(cl2cpp cl2cpp_BIN CL_BIN ${RUNTIME_PATH}/CL_BIN/*.cl cl)
endif()

if(VGL_USE_OPENCV)
    find_package(OpenCV REQUIRED)
    set(OPENCV_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/opencv)
    file(GLOB_RECURSE OPENCV_MODULE_SOURCES CONFIGURE_DEPENDS ${OPENCV_MODULE_DIR}/*.cpp)
    list(APPEND ALL_LIBRARY_SOURCES ${OPENCV_MODULE_SOURCES})
endif()

if(VGL_USE_DCMTK)
    find_package(DCMTK REQUIRED)
    set(DCMTK_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/dcmtk)
    file(GLOB_RECURSE DCMTK_MODULE_SOURCES CONFIGURE_DEPENDS ${DCMTK_MODULE_DIR}/*.cpp)
    list(APPEND ALL_LIBRARY_SOURCES ${DCMTK_MODULE_SOURCES})
endif()

if(VGL_USE_GDCM)
    find_package(GDCM REQUIRED)
    set(GDCM_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/gdcm)
    file(GLOB_RECURSE GDCM_MODULE_SOURCES CONFIGURE_DEPENDS ${GDCM_MODULE_DIR}/*.cpp)
    list(APPEND ALL_LIBRARY_SOURCES ${GDCM_MODULE_SOURCES})
endif()

if(VGL_USE_TIFF)
    find_package(TIFF REQUIRED)
    set(TIFF_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/tiff)
    file(GLOB_RECURSE TIFF_MODULE_SOURCES CONFIGURE_DEPENDS ${TIFF_MODULE_DIR}/*.cpp)
    list(APPEND ALL_LIBRARY_SOURCES ${TIFF_MODULE_SOURCES})
endif()

add_library(${PROJECT_NAME} SHARED ${ALL_LIBRARY_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link libraries after the target is created
target_link_libraries(${PROJECT_NAME} PUBLIC
    OpenGL::GL
    GLEW::GLEW
    GLUT::GLUT
)

# Add link optional dependencies
if(VGL_USE_CUDA)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CUDA_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_CUDA)
endif()

if(VGL_USE_OPENCL)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenCL::OpenCL)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_OPENCL)
endif()

if(VGL_USE_OPENCV)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_OPENCV)
endif()

if(VGL_USE_DCMTK)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${DCMTK_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_DCMTK)
endif()

if(VGL_USE_GDCM)
    target_link_libraries(${PROJECT_NAME} PUBLIC gdcmCommon gdcmDICT gdcmDSED gdcmIOD gdcmjpeg8 gdcmjpeg12 gdcmjpeg16 gdcmMEXD gdcmMSFF socketxx)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_GDCM)
endif()

if(VGL_USE_TIFF)
    target_link_libraries(${PROJECT_NAME} PUBLIC TIFF::TIFF)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_TIFF)
endif()

add_custom_target(generate_wrappers ALL DEPENDS ${GENERATED_FILES})
add_dependencies(${PROJECT_NAME} generate_wrappers)

# Suppress errors that were warnings in past versions of C++
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/wd4838 /wd4996>
  $<$<OR:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<COMPILE_LANG_AND_ID:CXX,Clang>>:-Wno-narrowing -Wno-deprecated-declarations>
)