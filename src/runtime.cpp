#include <visiongl/runtime.hpp>

#include <string>
#include <filesystem>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

std::string get_environment_variable(const std::string& var) {
#ifdef _WIN32
    // Windows: Use GetEnvironmentVariableA
    char buffer[MAX_PATH];
    DWORD length = GetEnvironmentVariableA(var.c_str(), buffer, MAX_PATH);
    if (length > 0 && length < sizeof(buffer)) {
        return std::string(buffer, length);
    }
#else
    // Unix: Use getenv
    if (const char* env_p = std::getenv(var.c_str())) {
        return std::string(env_p);
    }
#endif

    return std::string();
}

namespace vgl {
    
const std::filesystem::path& get_root_path(void) {
    static std::filesystem::path root_path;
    
    if (!root_path.empty()) return root_path;
    std::vector<std::filesystem::path> paths(0);
    
    // Get root path environment variable if set
    auto path = get_environment_variable("VISIONGL_ROOT");
    if (!path.empty()) paths.push_back(path);

    #ifdef _WIN32
    // Windows: Check Program Files
    path = get_environment_variable("ProgramFiles(x86)");
    if (!path.empty()) paths.push_back(std::filesystem::path(path) / "visiongl");
    path = get_environment_variable("ProgramW6432");
    if (!path.empty()) paths.push_back(std::filesystem::path(path) / "visiongl");
    #else
    // Unix: Check local and global installation paths
    paths.push_back("/usr/local");
    paths.push_back("/usr");
    #endif
    
    for (const auto& path : paths) {
        if (std::filesystem::exists(path)) {
            root_path = path;
            return root_path;
        }
    }
    
    throw std::runtime_error("VISIONGL_ROOT not set and default installation path not found.");
}

    
const std::filesystem::path& get_runtime_path(void) {
    static std::filesystem::path runtime_path;
    
    if (!runtime_path.empty()) return runtime_path;
    
    runtime_path = get_root_path() / "share" / "visiongl" / "runtime";
    
    return runtime_path;
}

const std::filesystem::path& get_include_path(void) {
    static std::filesystem::path include_path;
    
    if (!include_path.empty()) return include_path;
    
    include_path = get_root_path() / "include";
    
    return include_path;
}

} // namespace vgl