#include <protogen_head_state.h>

std::vector<ProtogenHeadState::Emotion> ProtogenHeadState::allEmotions() {
    return {
        Emotion::Normal,
        Emotion::Angry,
        Emotion::Flustered,
        Emotion::Sad,
        Emotion::Disappointed,
        Emotion::Flirty,
        Emotion::Scared,
        Emotion::Owo,
        Emotion::Uwu,
        Emotion::Blink,
    };
}

ProtogenHeadState::Emotion ProtogenHeadState::emotionFromString(const std::string& s) {
    if(s == "normal")
        return Emotion::Normal;
    else if(s == "angry")
        return Emotion::Angry;
    else if(s == "flustered")
        return Emotion::Flustered;
    else if(s == "sad")
        return Emotion::Sad;
    else if(s == "disappointed")
        return Emotion::Disappointed;
    else if(s == "flirty")
        return Emotion::Flirty;
    else if(s == "scared")
        return Emotion::Scared;
    else if(s == "owo")
        return Emotion::Owo;
    else if(s == "uwu")
        return Emotion::Uwu;
    else if(s == "blink")
        return Emotion::Blink;
    else
        return Emotion::Normal;
}

std::string ProtogenHeadState::emotionToString(Emotion e) {
    switch(e) {
    case Emotion::Normal:
        return "normal";
    case Emotion::Angry:
        return "angry";
    case Emotion::Flustered:
        return "flustered";
    case Emotion::Sad:
        return "sad";
    case Emotion::Disappointed:
        return "disappointed";
    case Emotion::Flirty:
        return "flirty";
    case Emotion::Scared:
        return "scared";
    case Emotion::Owo:
        return "owo";
    case Emotion::Uwu:
        return "uwu";
    case Emotion::Blink:
        return "blink";
    default:
        return "";
    }
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
    for(const auto emotion : allEmotions()) {
        emotions += emotionToString(emotion) + "\n";
    }
    return emotions;
}

ProtogenHeadState::ProtogenHeadState()
    : m_emotion(Emotion::Normal),
        m_forceBlink(false),
        m_blank(false),
        m_brightness(Brightness::ForBrightRoom)
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

std::string ProtogenHeadState::toString() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto force_blink_string = m_forceBlink ? "true" : "false";
    const auto blank_string = m_blank ? "true" : "false";
    return "ProtogenHeadState{emotion: " + emotionToString(m_emotion) + ", forceBlink: " + force_blink_string + ", blank: " + blank_string + "}";
}