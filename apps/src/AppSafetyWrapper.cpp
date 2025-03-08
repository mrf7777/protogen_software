#include <protogen/apps/AppSafetyWrapper.h>

#include <iostream>

using namespace protogen;

protogen::AppSafetyWrapper::AppSafetyWrapper(std::shared_ptr<IProtogenApp> app)
    : m_app(app)
{
}

IExtension::Initialization protogen::AppSafetyWrapper::initialize()
{
    try {
        return m_app->initialize();
    } catch(...) {
        std::cerr << "An exception occurred during the initialization of an app." << std::endl;
        return IExtension::Initialization::Failure;
    }
}

std::shared_ptr<attributes::IAttributeStore> protogen::AppSafetyWrapper::getAttributeStore()
{
    try{
        return m_app->getAttributeStore();
    } catch(...) {
        std::cerr << "An exception occurred while getting the attribute store of an app." << std::endl;
    }
}

void protogen::AppSafetyWrapper::setActive(bool active)
{
    try {
        m_app->setActive(active);
    } catch(...) {
        std::cerr << "An exception occurred while setting the active state of an app." << std::endl;
    }
}

void protogen::AppSafetyWrapper::render(ICanvas &canvas) const
{
    try {
        m_app->render(canvas);
    } catch(...) {
        std::cerr << "An exception occurred while rendering an app." << std::endl;
    }
}

float protogen::AppSafetyWrapper::framerate() const
{
    try {
        return m_app->framerate();
    } catch(...) {
        std::cerr << "An exception occurred while getting the framerate of an app." << std::endl;
        return 0.0f;
    }
}

void protogen::AppSafetyWrapper::receiveRenderSurface(std::shared_ptr<IRenderSurface> render_surface)
{
    try {
        m_app->receiveRenderSurface(render_surface);
    } catch(...) {
        std::cerr << "An exception occurred while receiving the render surface of an app." << std::endl;
    }
}

std::vector<Resolution> protogen::AppSafetyWrapper::supportedResolutions() const
{
    try {
        return m_app->supportedResolutions();
    } catch(...) {
        std::cerr << "An exception occurred while getting the supported resolutions of an app." << std::endl;
        return {};
    }
}

void protogen::AppSafetyWrapper::receiveSensors(std::vector<std::shared_ptr<sensor::ISensor>> sensors)
{
    try {
        m_app->receiveSensors(sensors);
    } catch(...) {
        std::cerr << "An exception occurred while receiving sensors for an app." << std::endl;
    }
}
