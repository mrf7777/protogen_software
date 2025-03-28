#include <protogen/apps/ProtogenAppInitializer.h>

#include <protogen/IProtogenApp.hpp>

namespace protogen
{

ProtogenAppInitializer::ProtogenAppInitializer(
    std::shared_ptr<IExtensionInitializer> initialExtensionInitializer,
    std::vector<std::shared_ptr<sensor::ISensor>> sensors,
    std::shared_ptr<IRenderSurface> renderSurface
)
    : m_initialExtensionInitializer(initialExtensionInitializer),
    m_sensors(sensors),
    m_renderSurface(renderSurface)
{
}

IExtensionInitializer::Initialization ProtogenAppInitializer::initialize(ExtensionOriginBundle extension)
{
    if(m_initialExtensionInitializer->initialize(extension) == IExtensionInitializer::Initialization::Failure)
    {
        return IExtensionInitializer::Initialization::Failure;
    }

    IProtogenApp* app = dynamic_cast<IProtogenApp*>(extension.extension.get());
    if (app == nullptr)
    {
        std::cerr << "Tried to initialize app but extension is not an app from directory: `" << extension.extension_directory.generic_string() << "`." << std::endl;
        return IExtensionInitializer::Initialization::Failure;
    }
    app->receiveRenderSurface(m_renderSurface);
    app->receiveSensors(m_sensors);
    return IExtensionInitializer::Initialization::Success;
}

} // namespace