#ifndef PROTOGEN_DIRECTORY_EXTENSION_FINDER_H
#define PROTOGEN_DIRECTORY_EXTENSION_FINDER_H

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <protogen/IExtension.hpp>
#include <protogen/extensions/ExtensionOriginBundle.h>
#include <protogen/extensions/IExtensionFinder.h>

namespace protogen {

class DirectoryExtensionFinder : public IExtensionFinder {
public:
    DirectoryExtensionFinder(const std::filesystem::path& directory);
    std::vector<ExtensionOriginBundle> find() override;
private:
    std::optional<std::shared_ptr<IExtension>> loadExtensionFromDirectory(const std::filesystem::path& extension_directory);

    std::filesystem::path m_directory;
};

} // namespace

#endif