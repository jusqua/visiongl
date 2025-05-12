cmake_minimum_required(VERSION 3.15)

project(visiongl LANGUAGES CXX VERSION 0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
option(VGL_USE_DCMTK "Enable DCMTK support" OFF)
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

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS ${COMMON_DIR}/*.cpp)
list(APPEND ALL_LIBRARY_SOURCES ${COMMON_SOURCES})

if(VGL_USE_CUDA)
    find_package(CUDA CONFIG REQUIRED)
    set(CUDA_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/cuda)
    file(GLOB_RECURSE CUDA_MODULE_SOURCES CONFIGURE_DEPENDS ${CUDA_MODULE_DIR}/*.cu)
    list(APPEND ALL_LIBRARY_SOURCES ${CUDA_MODULE_SOURCES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_CUDA)
endif()

if(VGL_USE_OPENCL)
    find_package(OpenCL CONFIG REQUIRED)
    set(OPENCL_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/cl)
    file(GLOB_RECURSE OPENCL_MODULE_SOURCES CONFIGURE_DEPENDS ${OPENCL_MODULE_DIR}/*.cpp)
    list(APPEND ALL_LIBRARY_SOURCES ${OPENCL_MODULE_SOURCES})
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenCL::OpenCL)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_OPENCL)
endif()

if(VGL_USE_OPENCV)
    find_package(OpenCV CONFIG REQUIRED)
    set(OPENCL_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/cl)
    file(GLOB_RECURSE OPENCL_MODULE_SOURCES CONFIGURE_DEPENDS ${OPENCL_MODULE_DIR}/*.cpp)
    list(APPEND ALL_LIBRARY_SOURCES ${OPENCL_MODULE_SOURCES})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_OPENCV)
endif()

if(VGL_USE_DCMTK)
    find_package(DCMTK CONFIG REQUIRED)
    set(DCMTK_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/dcmtk)
    file(GLOB_RECURSE DCMTK_MODULE_SOURCES CONFIGURE_DEPENDS ${DCMTK_MODULE_DIR}/*.cpp)
    list(APPEND ALL_LIBRARY_SOURCES ${DCMTK_MODULE_SOURCES})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${DCMTK_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_DCMTK)
endif()

if(VGL_USE_GDCM)
    find_package(GDCM CONFIG REQUIRED)
    set(GDCM_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/gdcm)
    file(GLOB_RECURSE GDCM_MODULE_SOURCES CONFIGURE_DEPENDS ${GDCM_MODULE_DIR}/*.cpp)
    list(APPEND ALL_LIBRARY_SOURCES ${GDCM_MODULE_SOURCES})
    target_link_libraries(${PROJECT_NAME} PUBLIC gdcmCommon gdcmDICT gdcmDSED gdcmIOD gdcmjpeg8 gdcmjpeg12 gdcmjpeg16 gdcmMEXD gdcmMSFF socketxx)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_GDCM)
endif()

if(VGL_USE_TIFF)
    find_package(TIFF CONFIG REQUIRED)
    set(TIFF_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/tiff)
    file(GLOB_RECURSE TIFF_MODULE_SOURCES CONFIGURE_DEPENDS ${TIFF_MODULE_DIR}/*.cpp)
    list(APPEND ALL_LIBRARY_SOURCES ${TIFF_MODULE_SOURCES})
    target_link_libraries(${PROJECT_NAME} PUBLIC TIFF::TIFF)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VGL_USE_TIFF)
endif()

add_library(${PROJECT_NAME} SHARED ${ALL_LIBRARY_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
