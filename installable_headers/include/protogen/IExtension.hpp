#ifndef PROTOGEN_IEXTENSION_HPP
#define PROTOGEN_IEXTENSION_HPP

#include <protogen/IAttributeStore.hpp>

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
class IExtension {
public:
    enum class Initialization {
        Success,
        Failure,
    };

    virtual ~IExtension() = default;

    /**
     * Initialize your extension. This will be called before any other methods.
     */
    virtual Initialization initialize() = 0;

    /**
     * Return attribute store for this extension. This is used to store
     * metadata, configuration, and other information about the extension.
     */
    virtual std::shared_ptr<attributes::IAttributeStore> getAttributeStore() = 0;
};

using CreateExtensionFunction = IExtension * (*)();
using DestroyExtensionFunction = void (*)(IExtension *);

} // namespace

#endif