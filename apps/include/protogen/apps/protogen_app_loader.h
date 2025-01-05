#ifndef PROTOGEN_APP_LOADER_H
#define PROTOGEN_APP_LOADER_H

#include <string>
#include <map>
#include <memory>
#include <filesystem>
#include <optional>
#include <dlfcn.h>

#include <protogen/IProtogenApp.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/apps/IUserDataLocator.h>

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
    explicit ProtogenAppLoader(
        const std::string& apps_directory,
        std::shared_ptr<IProportionProvider> mouth_proportion_provider,
        const Resolution& device_resolution,
        std::shared_ptr<IUserDataLocator> user_data_location_provider);
    Apps apps() const;

private:
    static std::optional<std::shared_ptr<IProtogenApp>> loadAppFromDirectory(const std::filesystem::path& app_directory);

    std::filesystem::path m_appDirectory;
    std::shared_ptr<IProportionProvider> m_mouthProportionProvider;
    Resolution m_deviceResolution;
    std::shared_ptr<IUserDataLocator> m_userDataLocationProvider;
};

}   // namespace

#endif