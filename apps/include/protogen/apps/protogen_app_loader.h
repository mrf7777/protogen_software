#ifndef PROTOGEN_APP_LOADER_H
#define PROTOGEN_APP_LOADER_H

#include <string>
#include <map>
#include <memory>
#include <filesystem>
#include <optional>
#include <dlfcn.h>

#include <protogen/IProtogenApp.hpp>

namespace protogen
{

class ProtogenAppDeleter {
public:
    ProtogenAppDeleter(DestroyAppFunction app_destroyer, void * app_lib_handle)
        : m_appDestroyer(app_destroyer), m_appLibHandle(app_lib_handle)
    {}
    void operator()(IProtogenApp * app) const {
        m_appDestroyer(app);
        dlclose(m_appLibHandle);
    }
private:
    DestroyAppFunction m_appDestroyer;
    void * m_appLibHandle;
};

using Apps = std::map<std::string, std::shared_ptr<IProtogenApp>>;

class ProtogenAppLoader {
public:
    explicit ProtogenAppLoader(const std::string& apps_directory);
    Apps apps() const;

private:
    static std::optional<std::shared_ptr<IProtogenApp>> loadAppFromDirectory(const std::filesystem::path& app_directory);

    std::filesystem::path m_appDirectory;
};

}   // namespace

#endif