#ifndef PROTOGEN_APP_LOADER_H
#define PROTOGEN_APP_LOADER_H

#include <string>
#include <map>
#include <memory>
#include <filesystem>
#include <optional>
#include <dlfcn.h>

#include <protogen/IProtogenApp.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/IExtension.hpp>
#include <protogen/ISensor.hpp>
#include <protogen/IRenderSurface.hpp>
#include <protogen/extensions/IExtensionInitializer.h>

namespace protogen
{

using Apps = std::map<std::string, std::shared_ptr<IProtogenApp>>;

class ProtogenAppInitializer : public IExtensionInitializer
{
public:
    explicit ProtogenAppInitializer(
        std::vector<std::shared_ptr<sensor::ISensor>> sensors,
        std::shared_ptr<IRenderSurface> renderSurface
    );
    Initialization initialize(IExtension * extension) override;

private:
    std::shared_ptr<IProportionProvider> m_mouthProportionProvider;
    Resolution m_deviceResolution;
};

}   // namespace

#endif