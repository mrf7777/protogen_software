#include <protogen/extensions/finders/DirectoryExtensionFinder.h>

#include <protogen/extensions/ExtensionDeleter.h>

#include <iostream>
#include <dlfcn.h>

using namespace protogen;

DirectoryExtensionFinder::DirectoryExtensionFinder(const std::filesystem::path &directory)
    : m_directory(directory)
{
}

std::vector<std::shared_ptr<IExtension>> DirectoryExtensionFinder::find()
{
    std::vector<std::shared_ptr<IExtension>> extensions;
    for(const auto& entry : std::filesystem::directory_iterator(m_directory)) {
        if(entry.is_directory()) {
            auto app = loadExtensionFromDirectory(entry.path());
            if(!app.has_value()) {
                continue;
            }
            extensions.push_back(app.value());
        }
    }
    return extensions;
}

std::optional<std::shared_ptr<IExtension>> protogen::DirectoryExtensionFinder::loadExtensionFromDirectory(const std::filesystem::path &extension_directory)
{
    // Search for a `.so` file.
    for(const auto& app_file : std::filesystem::directory_iterator(extension_directory)) {
        if(app_file.path().extension() == ".so") {
            // Load library.
            const std::string app_library_path = app_file.path().string();
            void * protogen_app_lib = dlopen(app_library_path.c_str(), RTLD_NOW);
            if(protogen_app_lib == NULL) {
                std::cerr << "Could not load protogen app library file: " << app_file.path() << ". Dynamic linker error: " << dlerror() << std::endl;
                return {};
            }

            // Load app create and destroy functions.
            void * protogen_app_create_raw = dlsym(protogen_app_lib, "create_extension");
            if(protogen_app_create_raw == NULL) {
                std::cerr << "Could not load symbol 'create_extension' from protogen app library file: " << app_file.path() << ". Dynamic linker error: " << dlerror() << std::endl;
                dlclose(protogen_app_lib);
                return {};
            }
            void * protogen_app_destroy_raw = dlsym(protogen_app_lib, "destroy_extension");
            if(protogen_app_destroy_raw == NULL) {
                std::cerr << "Could not load symbol 'destroy_extension' from protogen app library file: " << app_file.path() << ". Dynamic linker error: " << dlerror() << std::endl;
                dlclose(protogen_app_lib);
                return {};
            }
            auto protogen_app_create = reinterpret_cast<CreateExtensionFunction>(protogen_app_create_raw);
            auto protogen_app_destroy = reinterpret_cast<DestroyExtensionFunction>(protogen_app_destroy_raw);

            auto protogen_app_instance = protogen_app_create();
            if(protogen_app_instance == nullptr) {
                std::cerr << "Could not create instance of extention from library file due to its `create_extension` function returning a null pointer: " << app_file.path() << std::endl;
                dlclose(protogen_app_lib);
                return {};
            }
            auto protogen_app_deleter = ExtensionDeleter(protogen_app_destroy, protogen_app_lib);
            auto protogen_app = std::shared_ptr<IExtension>(protogen_app_instance, protogen_app_deleter);
            return protogen_app;
        }
    }
    std::cerr << "Could not find library for protogen app in app directory: " << extension_directory << std::endl;
    return {};
}
