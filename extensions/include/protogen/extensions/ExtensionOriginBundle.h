#ifndef PROTOGEN_EXTENSIONORIGINBUNDLE_H
#define PROTOGEN_EXTENSIONORIGINBUNDLE_H

#include <memory>
#include <filesystem>

#include <protogen/IExtension.hpp>

namespace protogen {

/**
 * A bundle that contains an extension and the directory where it is located.
 */
struct ExtensionOriginBundle {
    std::shared_ptr<IExtension> extension;
    std::filesystem::path extension_directory;
};

} // namespace

#endif