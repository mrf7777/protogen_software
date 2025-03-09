#ifndef PROTOGEN_I_EXTENSION_INITIALIZER_H
#define PROTOGEN_I_EXTENSION_INITIALIZER_H

#include <protogen/IExtension.hpp>
#include <protogen/extensions/ExtensionOriginBundle.h>

namespace protogen {

class IExtensionInitializer {
public:
    enum class Initialization {
        Success,
        Failure,
    };

    virtual ~IExtensionInitializer() = default;
    virtual Initialization initialize(ExtensionOriginBundle extension) = 0;
};

} // namespace

#endif