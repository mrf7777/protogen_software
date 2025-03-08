#ifndef PROTOGEN_I_EXTENSION_INITIALIZER_H
#define PROTOGEN_I_EXTENSION_INITIALIZER_H

#include <protogen/IExtension.hpp>

namespace protogen {

class IExtensionInitializer {
public:
    enum class Initialization {
        Success,
        Failure,
    };

    virtual ~IExtensionInitializer() = default;
    virtual Initialization initialize(IExtension * extension) = 0;
};

} // namespace

#endif