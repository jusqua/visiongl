#ifndef VGL_RUNTIME_HPP
#define VGL_RUNTIME_HPP

#include <filesystem>

namespace vgl {

const std::filesystem::path& get_runtime_path(void);
const std::filesystem::path& get_include_path(void);

} // namespace vgl

#endif // VGL_RUNTIME_HPP
