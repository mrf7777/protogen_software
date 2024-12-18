#include <protogen/apps/ProtogenAppDefaultDecorator.h>

#include <algorithm>

namespace protogen
{

ProtogenAppDefaultDecorator::ProtogenAppDefaultDecorator(std::shared_ptr<IProtogenApp> protogen_app)
    : m_app(protogen_app)
{
}

std::string ProtogenAppDefaultDecorator::name() const
{
    if(m_app->name().empty()) {
        return "(No name)";
    } else {
        return m_app->name();
    }}

std::string ProtogenAppDefaultDecorator::id() const
{
    return m_app->id();
}

std::string ProtogenAppDefaultDecorator::description() const
{
    if(m_app->description().empty()) {
        return "(No description)";
    } else {
        return m_app->description();
    }
}

bool ProtogenAppDefaultDecorator::sanityCheck(std::string &errorMessage) const
{
    return m_app->sanityCheck(errorMessage);
}

void ProtogenAppDefaultDecorator::setActive(bool active)
{
    m_app->setActive(active);
}

IProtogenApp::Endpoints ProtogenAppDefaultDecorator::serverEndpoints() const
{
    return m_app->serverEndpoints();
}

std::string ProtogenAppDefaultDecorator::homePage() const
{
    return m_app->homePage();
}

std::string ProtogenAppDefaultDecorator::thumbnail() const
{
    if(m_app->thumbnail().empty()) {
        // TODO: use proper thumbnail
        return "https://upload.wikimedia.org/wikipedia/en/thumb/8/80/Wikipedia-logo-v2.svg/263px-Wikipedia-logo-v2.svg.png";
    } else {
        return m_app->thumbnail();
    }
}

std::string ProtogenAppDefaultDecorator::staticFilesDirectory() const
{
    return m_app->staticFilesDirectory();
}

void ProtogenAppDefaultDecorator::render(ICanvas &canvas) const
{
    m_app->render(canvas);
}

float ProtogenAppDefaultDecorator::framerate() const
{
    return std::clamp(m_app->framerate(), FRAMERATE_MIN, FRAMERATE_MAX);
}

void ProtogenAppDefaultDecorator::setMouthProportionProvider(std::shared_ptr<IProportionProvider> provider)
{
    m_app->setMouthProportionProvider(provider);
}

} // namespace