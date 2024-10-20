#ifndef PROTOGEN_HEAD_STATE_H
#define PROTOGEN_HEAD_STATE_H

#include <string>
#include <vector>
#include <mutex>

#include <utils.h>

class ProtogenHeadState final : public IToString {
public:
	enum class Emotion {
		Normal,
		Angry,
		Flustered,
		Sad,
		Disappointed,
		Flirty,
		Scared,
		Owo,
		Uwu,
		Blink,
	};

	enum class Brightness {
		ForBrightRoom,
		ForDarkRoom,
	};

	static std::vector<Emotion> allEmotions() {
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

	static Emotion emotionFromString(const std::string& s) {
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

	static std::string emotionToString(Emotion e) {
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

	static std::vector<Brightness> allBrightnessLevels() {
		return {Brightness::ForBrightRoom, Brightness::ForDarkRoom};
	}

	static std::string brightnessLevelsSeperatedByNewline() {
		std::string s;
		for(const auto brightness : allBrightnessLevels()) {
			s += brightnessToString(brightness) + "\n";
		}
		return s;
	}

	static std::string brightnessToString(Brightness brightness) {
		switch(brightness) {
		case Brightness::ForBrightRoom:
			return "for_bright_room";
		case Brightness::ForDarkRoom:
			return "for_dark_room";
		default:
			return "";
		}
	}

	static Brightness stringToBrightness(const std::string& brightness) {
		if(brightness == "for_bright_room")
			return Brightness::ForBrightRoom;
		else if(brightness == "for_dark_room")
			return Brightness::ForDarkRoom;
		else
			return Brightness::ForBrightRoom;
	}

	static uint8_t brightnessToPercent(Brightness brightness) {
		switch(brightness) {
		case Brightness::ForBrightRoom:
			return 100;
		case Brightness::ForDarkRoom:
			return 50;
		default:
			return 0;
		}
	}

	static std::string emotionsSeperatedByNewline() {
		std::string emotions;
		for(const auto emotion : allEmotions()) {
			emotions += emotionToString(emotion) + "\n";
		}
		return emotions;
	}

	ProtogenHeadState()
		: m_emotion(Emotion::Normal),
		  m_forceBlink(false),
		  m_blank(false),
		  m_brightness(Brightness::ForBrightRoom)
	{}

	Emotion emotion() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_emotion;
	}
	void setEmotion(Emotion emotion) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_emotion = emotion;
	}
	bool forceBlink() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_forceBlink;
	}
	void setForceBlink(bool forceBlink) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_forceBlink = forceBlink;
	}
	void setBlank(bool blank) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_blank = blank;	
	}
	bool blank() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_blank;
	}
	void setBrightness(Brightness brightness) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_brightness = brightness;
	}
	Brightness brightness() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_brightness;
	}
	Emotion getEmotionConsideringForceBlink() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		if(m_forceBlink) {
			return FORCE_BLINK_EMOTION;
		} else {
			return m_emotion;
		}
	}
	virtual std::string toString() const override {
		std::lock_guard<std::mutex> lock(m_mutex);
		const auto force_blink_string = m_forceBlink ? "true" : "false";
		const auto blank_string = m_blank ? "true" : "false";
		return "ProtogenHeadState{emotion: " + emotionToString(m_emotion) + ", forceBlink: " + force_blink_string + ", blank: " + blank_string + "}";
	}

private:
	Emotion m_emotion;
	bool m_forceBlink;
	bool m_blank;
	Brightness m_brightness;
	mutable std::mutex m_mutex;

	static const Emotion FORCE_BLINK_EMOTION = Emotion::Blink;
};

#endif