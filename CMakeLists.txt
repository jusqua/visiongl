cmake_minimum_required(VERSION 3.16)
project(visiongl VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define paths
set(ROOTPATH ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILDPATH ${CMAKE_BINARY_DIR})
set(RUNTIMEPATH ${ROOTPATH}/runtime)
set(INCLUDEPATH ${ROOTPATH}/include)
set(SOURCEPATH ${ROOTPATH}/src)
set(SCRIPTSPATH ${ROOTPATH}/scripts)
set(TARGETPATH ${SOURCEPATH}/${PROJECT_NAME})
set(BENCHMARKPATH ${SOURCEPATH}/demo)
set(TESTSPATH ${SOURCEPATH}/test)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILDPATH}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILDPATH}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILDPATH}/bin)

# Options (matching Makefile conditionals)
option(WITH_DEBUG "Enable debug mode" ON)
option(WITH_CUDA "Enable CUDA support" OFF)
option(WITH_OPENCL "Enable OpenCL support" ON)
option(WITH_OPENCV "Enable OpenCV support" OFF)
option(WITH_GDCM "Enable GDCM support" OFF)
option(WITH_TIFF "Enable TIFF support" ON)

# Build options
option(BUILD_BENCHMARKS "Build benchmark applications" OFF)

# Print build options
message(STATUS "Build configuration:")
message(STATUS "  - Library: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "  - Debug mode: ${WITH_DEBUG}")
message(STATUS "  - CUDA support: ${WITH_CUDA}")
message(STATUS "  - OpenCL support: ${WITH_OPENCL}")
message(STATUS "  - OpenCV support: ${WITH_OPENCV}")
message(STATUS "  - GDCM support: ${WITH_GDCM}")
message(STATUS "  - TIFF support: ${WITH_TIFF}")
message(STATUS "  - Build demos: ${BUILD_DEMOS}")
message(STATUS "  - Build benchmarks: ${BUILD_BENCHMARKS}")

# Find all source files
file(GLOB SOURCES "${TARGETPATH}/*.cpp")

# Configure compile definitions
add_compile_definitions(GL_GLEXT_PROTOTYPES GLXX_GLXEXT_PROTOTYPES)

# Runtime path definition for compilation
add_compile_definitions(VGL_RUNTIME_PATH="${RUNTIMEPATH}")

# Debug configuration
if(WITH_DEBUG)
    add_compile_definitions(__DEBUG__)
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

# Optional dependencies based on options
if(WITH_OPENCV)
    find_package(OpenCV REQUIRED)
    add_compile_definitions(__OPENCV__)
endif()

if(WITH_CUDA)
    find_package(CUDA REQUIRED)
    add_compile_definitions(__CUDA__)
    file(GLOB CUDA_SOURCES "${TARGETPATH}/*.cu")
    list(APPEND SOURCES ${CUDA_SOURCES})
endif()

if(WITH_OPENCL)
    find_package(OpenCL REQUIRED)
    add_compile_definitions(__OPENCL__)
endif()

if(WITH_GDCM)
    find_package(GDCM REQUIRED)
    add_compile_definitions(__GDCM__)
endif()

if(WITH_TIFF)
    find_package(TIFF REQUIRED)
    add_compile_definitions(__TIFF__)
endif()

# Find fmt (from vcpkg)
find_package(fmt CONFIG REQUIRED)

# Create the library target
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Set include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${INCLUDEPATH}>
        $<INSTALL_INTERFACE:include>
)

# Link dependencies
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    GLEW::GLEW
    GLUT::GLUT
    fmt::fmt
)

# Link optional dependencies
if(WITH_OPENCV)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
endif()

if(WITH_OPENCL)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenCL::OpenCL)
endif()

if(WITH_GDCM)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${GDCM_LIBRARIES})
endif()

if(WITH_TIFF)
    target_link_libraries(${PROJECT_NAME} PUBLIC TIFF::TIFF)
endif()

# Set compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-narrowing)
endif()


# --- Generated Header Files ---

set(GENERATED_HEADERS "") # Initialize list
set(PERL_MODULE_DIR ${ROOTPATH}/scripts/perlModules) # Assuming common.pm is here
set(PERL_COMMON_MODULE ${PERL_MODULE_DIR}/common.pm)

# Helper macro for defining generation commands
macro(add_wrapper_generation SCRIPT_NAME OUTPUT_BASENAME PREFIX INPUT_GLOB)
    set(INTERMEDIATE_HPP ${TARGETPATH}/${OUTPUT_BASENAME}.hpp)
    set(FINAL_HPP ${INCLUDEPATH}/${PROJECT_NAME}/${OUTPUT_BASENAME}.hpp)
    set(PERL_SCRIPT ${SCRIPTSPATH}/${SCRIPT_NAME}.pl)

    file(GLOB INPUT_FILES "${INPUT_GLOB}")

    add_custom_command(
        OUTPUT ${FINAL_HPP}
        COMMAND perl -I${ROOTPATH} ${PERL_SCRIPT} -o ${TARGETPATH}/${OUTPUT_BASENAME} -p ${PREFIX} ${INPUT_FILES}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDEPATH}/${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy ${INTERMEDIATE_HPP} ${FINAL_HPP}
        DEPENDS ${INPUT_FILES} ${PERL_SCRIPT} ${PERL_COMMON_MODULE} # Add dependency on common.pm
        COMMENT "Generating ${FINAL_HPP} from ${SCRIPT_NAME}.pl"
        VERBATIM # Important for multiple commands, especially on Windows
    )
    list(APPEND GENERATED_HEADERS ${FINAL_HPP})
endmacro()

# Define generation for each wrapper type
add_wrapper_generation(kernel2cu kernel2cu_shaders "" ${RUNTIMEPATH}/CUDA/*.kernel)
add_wrapper_generation(glsl2cpp glsl2cpp_shaders FS ${RUNTIMEPATH}/FS/*.frag)
add_wrapper_generation(glsl2cpp glsl2cpp_BG FS_BG ${RUNTIMEPATH}/FS_BG/*.frag)
add_wrapper_generation(glsl2cpp glsl2cpp_Stereo FS_Stereo ${RUNTIMEPATH}/FS_Stereo/*.frag)
add_wrapper_generation(cl2cpp cl2cpp_shaders CL ${RUNTIMEPATH}/CL/*.cl)
add_wrapper_generation(cl2cpp cl2cpp_ND CL_ND ${RUNTIMEPATH}/CL_ND/*.cl)
add_wrapper_generation(cl2cpp cl2cpp_MM CL_MM ${RUNTIMEPATH}/CL_MM/*.cl)
add_wrapper_generation(cl2cpp cl2cpp_BIN CL_BIN ${RUNTIMEPATH}/CL_BIN/*.cl)

# Custom target that depends on the *output files*
add_custom_target(generate_headers ALL DEPENDS ${GENERATED_HEADERS})

# --- End Generated Header Files ---

add_dependencies(${PROJECT_NAME} generate_headers) # Add new dependency

# Copy runtime files to build directory
add_custom_target(runtime_copy
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${BUILDPATH}/share/${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILDPATH}/share/${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RUNTIMEPATH} ${BUILDPATH}/share/${PROJECT_NAME}/runtime
    COMMENT "Copying runtime to build directory"
)

# Make sure runtime is copied before library is built
add_dependencies(${PROJECT_NAME} runtime_copy)

# Add benchmarks if enabled
if(BUILD_BENCHMARKS)
    add_custom_target(benchmarks)
    file(GLOB BENCHMARK_SOURCES "${BENCHMARKPATH}/benchmark_*.cpp")
    foreach(BENCHMARK_SOURCE ${BENCHMARK_SOURCES})
        get_filename_component(BENCHMARK_NAME ${BENCHMARK_SOURCE} NAME_WE)
        add_executable(${BENCHMARK_NAME} ${BENCHMARK_SOURCE})
        target_link_libraries(${BENCHMARK_NAME} PRIVATE ${PROJECT_NAME})
        set_target_properties(${BENCHMARK_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/benchmarks"
        )
        add_dependencies(benchmarks ${BENCHMARK_NAME})
    endforeach()
    message(STATUS "Benchmark applications will be built")
endif()

# Installation
include(GNUInstallDirs)

# Benchmarks installation
if(BUILD_BENCHMARKS)
    install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/benchmarks/
            DESTINATION ${CMAKE_INSTALL_BINDIR}/benchmarks
            FILES_MATCHING PATTERN "benchmark_*"
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ GROUP_EXECUTE
                        WORLD_READ WORLD_EXECUTE
    )
endif()

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${INCLUDEPATH}/${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${RUNTIMEPATH}/
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/runtime
)

# For find_package support
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)

# Generate package configuration files
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)
