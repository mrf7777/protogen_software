#include <protogen/extensions/ExtensionDeleter.h>

#include <dlfcn.h>

using namespace protogen;

ExtensionDeleter::ExtensionDeleter(DestroyExtensionFunction extension_destroyer, void * extension_lib_handle)
    : m_extensionDestroyer(extension_destroyer), m_extensionLibHandle(extension_lib_handle)
{}

void ExtensionDeleter::operator()(IExtension * extension) const {
    m_extensionDestroyer(extension);
    dlclose(m_extensionLibHandle);
}