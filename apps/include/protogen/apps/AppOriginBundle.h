#ifndef PROTOGEN_APPORIGINBUNDLE_H
#define PROTOGEN_APPORIGINBUNDLE_H

#include <memory>
#include <filesystem>

#include <protogen/IProtogenApp.hpp>

namespace protogen {

    /**
     * A bundle that contains an app and the directory where it is located.
     */
    struct AppOriginBundle {
        std::shared_ptr<IProtogenApp> app;
        std::filesystem::path app_directory;
    };
    
} // namespace    

#endif