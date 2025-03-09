#ifndef PROTOGEN_I_EXTENSION_FINDER_H
#define PROTOGEN_I_EXTENSION_FINDER_H

#include <string>
#include <memory>
#include <vector>

#include <protogen/IExtension.hpp>
#include <protogen/extensions/ExtensionOriginBundle.h>

namespace protogen {

class IExtensionFinder {
public:
    virtual ~IExtensionFinder() = default;
    virtual std::vector<ExtensionOriginBundle> find() = 0;
};

} // namespace

#endif