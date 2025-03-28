#ifndef PROTOGEN_APP_SAFETY_WRAPPER_H
#define PROTOGEN_APP_SAFETY_WRAPPER_H

#include <memory>

#include <protogen/IProtogenApp.hpp>

namespace protogen {

class AppSafetyWrapper : public IProtogenApp {
public:
    explicit AppSafetyWrapper(std::shared_ptr<IProtogenApp> app);
    Initialization initialize() override;
    std::shared_ptr<attributes::IAttributeStore> getAttributeStore() override;
    void setActive(bool active) override;
    void receiveRenderSurface(std::shared_ptr<IRenderSurface> render_surface) override;
    void receiveSensors(std::vector<std::shared_ptr<sensor::ISensor>> sensors) override;

private:
    std::shared_ptr<IProtogenApp> m_app;
};

} // namespace

#endif