#ifndef PROTOGEN_EXTENSION_DELETER_H
#define PROTOGEN_EXTENSION_DELETER_H

#include <protogen/IExtension.hpp>

namespace protogen {

class ExtensionDeleter {
public:
    ExtensionDeleter(DestroyExtensionFunction extension_destroyer, void * extension_lib_handle);

    void operator()(IExtension * extension) const;
private:
    DestroyExtensionFunction m_extensionDestroyer;
    void * m_extensionLibHandle;
};

} // namespace

#endif