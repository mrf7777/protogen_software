#include <protogen/state/app_state.h>

namespace protogen {

std::string AppState::modeToString(Mode m) {
    switch(m) {
    case Mode::ProtogenHead:
        return "protogen_head";
    case Mode::Minecraft:
        return "minecraft";
    case Mode::App:
        return "app";
    default:
        return "";
    }
}

AppState::Mode AppState::stringToMode(const std::string& m) {
    if(m == "protogen_head")
        return Mode::ProtogenHead;
    else if(m == "minecraft")
        return Mode::Minecraft;
    else if(m == "app")
        return Mode::App;
    else
        return Mode::ProtogenHead;
}

std::vector<AppState::Mode> AppState::allModes() {
    return {Mode::ProtogenHead, Mode::Minecraft};
}

AppState::AppState(const Apps& apps)
    : m_mode(Mode::ProtogenHead),
    m_apps(apps)
{}

float AppState::frameRate() const {
    switch(m_mode) {
        case Mode::ProtogenHead:
            return 50.0;
        case Mode::Minecraft:
            return 10.0;
        case Mode::App:
            return getActiveApp()->framerate();
        default:
            return 60.0;
    }
}

AppState::Mode AppState::mode() const { return m_mode; }

void AppState::setMode(Mode m) {
    const auto previous_mode = m_mode;
    m_mode = m;

    // If changing mode away from app mode, set the app to inactive.
    if(previous_mode == Mode::App && m_mode != Mode::App) {
        getActiveApp()->setActive(false);
        m_activeAppId = "";
    }
}

void AppState::setActiveApp(const std::string& app_id) {
    if(!m_apps.contains(app_id)) {
        return;
    }

    // If an app is already selected, set it to inactive.
    if(IProtogenApp * current_app = getActiveApp()) {
        current_app->setActive(false);
    }

    getActiveApp()->setActive(true);
    m_activeAppId = app_id;
    setMode(Mode::App);
}

IProtogenApp * AppState::getActiveApp() const {
    if(mode() != Mode::App || !m_apps.contains(m_activeAppId)) {
        return nullptr;
    }

    return m_apps.at(m_activeAppId).get();
}

ProtogenHeadState& AppState::protogenHeadState() {
    return m_protogenHeadState;
}

const Apps &AppState::apps() const
{
    return m_apps;
}

const ProtogenHeadState &AppState::protogenHeadState() const
{
    return m_protogenHeadState;
}

MinecraftState& AppState::minecraftState() { return m_minecraftState; }

const MinecraftState& AppState::minecraftState() const { return m_minecraftState; }

std::string AppState::toString() const {
    return "AppState{protogenHeadState: " + m_protogenHeadState.toString() + "}";
}

}   // namespace