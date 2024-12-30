#include <protogen/apps/protogen_app_loader.h>

#include <dlfcn.h>

namespace protogen
{

ProtogenAppLoader::ProtogenAppLoader(const std::string &apps_directory, std::shared_ptr<IProportionProvider> mouth_proportion_provider, const Resolution& device_resolution)
    : m_appDirectory(apps_directory), m_mouthProportionProvider(mouth_proportion_provider), m_deviceResolution(device_resolution)
{
}

Apps ProtogenAppLoader::apps() const
{
    Apps apps;
    for(const auto& entry : std::filesystem::directory_iterator(m_appDirectory)) {
        if(entry.is_directory()) {
            auto app = loadAppFromDirectory(entry.path());
            if(app.has_value()) {
                app.value()->setMouthProportionProvider(m_mouthProportionProvider);
                app.value()->supportedResolutions(m_deviceResolution);
                const auto app_resources_directory = entry / std::filesystem::path("resources");
                app.value()->receiveResourcesDirectory(app_resources_directory.generic_string());
                const std::string app_id = app.value()->id();
                apps.insert({app_id, std::move(app.value())});
            }
        }
    }
    return apps;
}

std::optional<std::shared_ptr<IProtogenApp>> ProtogenAppLoader::loadAppFromDirectory(const std::filesystem::path &app_directory)
{
    // Search for a `.so` file.
    for(const auto& app_file : std::filesystem::directory_iterator(app_directory)) {
        if(app_file.path().extension() == ".so") {
            // Load library.
            const std::string app_library_path = app_file.path().string();
            void * protogen_app_lib = dlopen(app_library_path.c_str(), RTLD_NOW);
            if(protogen_app_lib == NULL) {
                std::cerr << "Could not load protogen app library file: " << app_file.path() << std::endl;
                return {};
            }

            // Load app create and destroy functions.
            void * protogen_app_create_raw = dlsym(protogen_app_lib, "create_app");
            if(protogen_app_create_raw == NULL) {
                std::cerr << "Could not load symbol 'create_app' from protogen app library file: " << app_file.path() << std::endl;
                dlclose(protogen_app_lib);
                return {};
            }
            void * protogen_app_destroy_raw = dlsym(protogen_app_lib, "destroy_app");
            if(protogen_app_destroy_raw == NULL) {
                std::cerr << "Could not load symbol 'destroy_app' from protogen app library file: " << app_file.path() << std::endl;
                dlclose(protogen_app_lib);
                return {};
            }
            auto protogen_app_create = reinterpret_cast<CreateAppFunction>(protogen_app_create_raw);
            auto protogen_app_destroy = reinterpret_cast<DestroyAppFunction>(protogen_app_destroy_raw);

            auto protogen_app_deleter = ProtogenAppDeleter(protogen_app_destroy, protogen_app_lib);
            auto protogen_app = std::shared_ptr<IProtogenApp>(protogen_app_create(), protogen_app_deleter);
            return protogen_app;
        }
    }
    std::cerr << "Could not find library for protogen app in app directory: " << app_directory << std::endl;
    return {};
}

} // namespace