#ifndef PROTOGEN_BASE_EXTENSION_INITIALIZER_H
#define PROTOGEN_BASE_EXTENSION_INITIALIZER_H

#include <protogen/extensions/IExtensionInitializer.h>

namespace protogen {

class BaseExtensionInitializer : public IExtensionInitializer {
public:
    BaseExtensionInitializer() = default;
    Initialization initialize(IExtension * extension) = 0;
};

} // namespace

#endif