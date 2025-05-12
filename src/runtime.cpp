#include <visiongl/runtime.hpp>

#include <string>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

static std::string VGL_RUNTIME_PATH;

namespace vgl {

std::string get_runtime_path(void) {
    if (!VGL_RUNTIME_PATH.empty()) return VGL_RUNTIME_PATH;
    
    // 1. First check environment variable
    const char* env_p = std::getenv("VISIONGL_RUNTIME_PATH");
    if (env_p) {
        VGL_RUNTIME_PATH = std::string(env_p);
        return VGL_RUNTIME_PATH;
    }
    
    // 2. Check default installation paths based on OS
#ifdef _WIN32
    // Windows: Check Program Files
    char program_files_path[MAX_PATH];
    if (GetEnvironmentVariableA("ProgramFiles", program_files_path, MAX_PATH)) {
        std::string path = std::string(program_files_path) + "\\visiongl\\share\\visiongl\\runtime";
        if (std::filesystem::exists(path)) {
            VGL_RUNTIME_PATH = path;
            return VGL_RUNTIME_PATH;
        }
    }
    else if (GetEnvironmentVariableA("ProgramFiles(x86)", program_files_path, MAX_PATH)) {
        std::string path = std::string(program_files_path) + "\\visiongl\\share\\visiongl\\runtime";
        if (std::filesystem::exists(path)) {
            VGL_RUNTIME_PATH = path;
            return VGL_RUNTIME_PATH;
        }
    }
#else
    // Unix-like: Check /usr/local
    std::string path = "/usr/local/visiongl/share/visiongl/runtime";
    if (std::filesystem::exists(path)) {
        VGL_RUNTIME_PATH = path;
        return VGL_RUNTIME_PATH;
    }
#endif
    
    throw std::runtime_error("VISIONGL_RUNTIME_PATH not set and default installation path not found.");
}

} // namespace vgl