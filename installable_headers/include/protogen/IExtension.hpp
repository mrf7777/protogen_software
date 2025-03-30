#ifndef PROTOGEN_IEXTENSION_HPP
#define PROTOGEN_IEXTENSION_HPP

#include <protogen/IAttributeStore.hpp>
#include <protogen/IInitializable.hpp>

#include <memory>

namespace protogen {

/**
 * Represents an extension to the protogen software.
 * 
 * An extension is a piece of software that can be dynamically loaded
 * into the protogen software. An extension can be a sensor, a render
 * surface, an app, or something else. Extensions are loaded from shared
 * libraries that are dynamically loaded by the core protogen software.
 * 
 * See the documentation for each extension type for more information. Namely:
 * - ISensor
 * - IRenderSurface
 * - IProtogenApp
 */
class IExtension : public attributes::IAttributeStore, public IInitializable {
public:
    virtual ~IExtension() = default;
};

using CreateExtensionFunction = IExtension * (*)();
using DestroyExtensionFunction = void (*)(IExtension *);

} // namespace

#endif