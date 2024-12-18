#ifndef PROTOGEN_APP_DEFAULT_DECORATOR
#define PROTOGEN_APP_DEFAULT_DECORATOR

#include <memory>

#include <protogen/ICanvas.hpp>
#include <protogen/IProportionProvider.hpp>
#include <protogen/IProtogenApp.hpp>

namespace protogen
{

class ProtogenAppDefaultDecorator : public IProtogenApp {
public:
    ProtogenAppDefaultDecorator(std::shared_ptr<IProtogenApp> protogen_app);
    std::string name() const override;
    std::string id() const override;
    std::string description() const override;
    bool sanityCheck(std::string& errorMessage) const override;
    void setActive(bool active) override;
    Endpoints serverEndpoints() const override;
    std::string homePage() const override;
    std::string thumbnail() const override;
    std::string staticFilesDirectory() const override;
    void render(ICanvas& canvas) const override;
    float framerate() const override;
    void setMouthProportionProvider(std::shared_ptr<IProportionProvider> provider) override;

private:
    std::shared_ptr<IProtogenApp> m_app;

    static constexpr float FRAMERATE_MIN = 0.0f;
    static constexpr float FRAMERATE_MAX = 60.0f;
    static_assert(FRAMERATE_MIN <= FRAMERATE_MAX, "framerate min and max should be properly ordered");
};

} // namespace

#endif