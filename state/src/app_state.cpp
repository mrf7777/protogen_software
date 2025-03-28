#include <protogen/state/app_state.h>

namespace protogen {

AppState::AppState()
    : m_apps(),
    m_activeAppId({})
{}

bool AppState::addApp(std::shared_ptr<IProtogenApp> app) {
    const auto app_id = app->getAttributeStore()->getAttribute(attributes::A_ID).value_or("");
    if(app_id.empty()) {
        return false;
    }

    m_apps[app_id] = app;
    return true;
}

bool AppState::setActiveApp(const std::string& app_id) {
    if(!m_apps.contains(app_id)) {
        return false;
    }

    // If an app is already selected, set it to inactive.
    if(auto current_app = getActiveApp()) {
        current_app.value()->setActive(false);
    }

    m_activeAppId = {app_id};
    getActiveApp().value()->setActive(true);
    return true;
}

std::optional<std::shared_ptr<IProtogenApp>> AppState::getApp(const std::string &app_id)
{
    if(m_apps.contains(app_id)) {
        return m_apps.at(app_id);
    } else {
        return {};
    }
}

std::optional<std::shared_ptr<IProtogenApp>> AppState::getActiveApp()
{
    if(!m_activeAppId.has_value()) {
        return {};
    }
    
    return getApp(m_activeAppId.value());
}

std::optional<std::string> AppState::getActiveAppId() const {
    return m_activeAppId;
}

std::set<std::string> AppState::appIds() const
{
    std::set<std::string> appIds;
    for(const auto& [appId, _] : m_apps) {
        appIds.insert(appId);
    }
    return appIds;
}

std::string AppState::toString() const
{
    return "AppState{}";
}

}   // namespace