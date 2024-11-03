#include <app_state.h>

namespace protogen {

std::string AppState::modeToString(Mode m) {
    switch(m) {
    case Mode::ProtogenHead:
        return "protogen_head";
    case Mode::Minecraft:
        return "minecraft";
    default:
        return "";
    }
}

AppState::Mode AppState::stringToMode(const std::string& m) {
    if(m == "protogen_head")
        return Mode::ProtogenHead;
    else if(m == "minecraft")
        return Mode::Minecraft;
    else
        return Mode::ProtogenHead;
}

std::vector<AppState::Mode> AppState::allModes() {
    return {Mode::ProtogenHead, Mode::Minecraft};
}

AppState::AppState() : m_mode(Mode::ProtogenHead) {}

int AppState::frameRate() const {
    switch(m_mode) {
        case Mode::ProtogenHead:
            return 50;
        case Mode::Minecraft:
            return 10;
        default:
            return 60;
    }
}

AppState::Mode AppState::mode() const { return m_mode; }

void AppState::setMode(Mode m) { m_mode = m; }

ProtogenHeadState& AppState::protogenHeadState() {
    return m_protogenHeadState;
}
const ProtogenHeadState& AppState::protogenHeadState() const {
    return m_protogenHeadState;
}
MinecraftState& AppState::minecraftState() { return m_minecraftState; }
const MinecraftState& AppState::minecraftState() const { return m_minecraftState; }

std::string AppState::toString() const {
    return "AppState{protogenHeadState: " + m_protogenHeadState.toString() + "}";
}

}   // namespace