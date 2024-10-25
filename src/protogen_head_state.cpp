#include <protogen_head_state.h>

std::vector<ProtogenHeadState::Emotion> ProtogenHeadState::allEmotions() {
    // TODO: scan resources directory and deduce.
    return {
        "normal",
        "angry",
        "flustered",
        "sad",
        "disappointed",
        "flirty",
        "scared",
        "owo",
        "uwu",
        "blink",
    };
}

std::vector<ProtogenHeadState::Brightness> ProtogenHeadState::allBrightnessLevels() {
    return {Brightness::ForBrightRoom, Brightness::ForDarkRoom};
}

std::string ProtogenHeadState::brightnessLevelsSeparatedByNewline() {
    std::string s;
    for(const auto brightness : allBrightnessLevels()) {
        s += brightnessToString(brightness) + "\n";
    }
    return s;
}

std::string ProtogenHeadState::brightnessToString(Brightness brightness) {
    switch(brightness) {
    case Brightness::ForBrightRoom:
        return "for_bright_room";
    case Brightness::ForDarkRoom:
        return "for_dark_room";
    default:
        return "";
    }
}

ProtogenHeadState::Brightness ProtogenHeadState::stringToBrightness(const std::string& brightness) {
    if(brightness == "for_bright_room")
        return Brightness::ForBrightRoom;
    else if(brightness == "for_dark_room")
        return Brightness::ForDarkRoom;
    else
        return Brightness::ForBrightRoom;
}

uint8_t ProtogenHeadState::brightnessToPercent(Brightness brightness) {
    switch(brightness) {
    case Brightness::ForBrightRoom:
        return 100;
    case Brightness::ForDarkRoom:
        return 50;
    default:
        return 0;
    }
}

std::string ProtogenHeadState::emotionsSeparatedByNewline() {
    std::string emotions;
    for(const auto& emotion : allEmotions()) {
        emotions += emotion + "\n";
    }
    return emotions;
}

ProtogenHeadState::ProtogenHeadState()
    : m_emotion("normal"),
        m_forceBlink(false),
        m_blank(false),
        m_brightness(Brightness::ForBrightRoom),
        m_mouthOpenness(Proportion::make(0.0).value()),
        m_eyeOpenness(Proportion::make(1.0).value())
{}

ProtogenHeadState::Emotion ProtogenHeadState::emotion() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_emotion;
}

void ProtogenHeadState::setEmotion(Emotion emotion) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_emotion = emotion;
}

bool ProtogenHeadState::forceBlink() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_forceBlink;
}

void ProtogenHeadState::setForceBlink(bool forceBlink) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_forceBlink = forceBlink;
}

void ProtogenHeadState::setBlank(bool blank) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_blank = blank;	
}

bool ProtogenHeadState::blank() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_blank;
}

void ProtogenHeadState::setBrightness(Brightness brightness) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_brightness = brightness;
}

ProtogenHeadState::Brightness ProtogenHeadState::brightness() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_brightness;
}

ProtogenHeadState::Emotion ProtogenHeadState::getEmotionConsideringForceBlink() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_forceBlink) {
        return FORCE_BLINK_EMOTION;
    } else {
        return m_emotion;
    }
}

void ProtogenHeadState::setMouthOpenness(Proportion openness) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_mouthOpenness = openness;
}

Proportion ProtogenHeadState::mouthOpenness() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_mouthOpenness;
}

void ProtogenHeadState::setEyeOpenness(Proportion openness) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_eyeOpenness = openness;
}

Proportion ProtogenHeadState::eyeOpenness() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_eyeOpenness;
}

std::string ProtogenHeadState::toString() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto force_blink_string = m_forceBlink ? "true" : "false";
    const auto blank_string = m_blank ? "true" : "false";
    return "ProtogenHeadState{emotion: " + m_emotion + ", forceBlink: " + force_blink_string + ", blank: " + blank_string + "}";
}