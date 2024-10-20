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

	static std::vector<Emotion> allEmotions();
	static Emotion emotionFromString(const std::string& s);
	static std::string emotionToString(Emotion e);
	static std::vector<Brightness> allBrightnessLevels();
	static std::string brightnessLevelsSeparatedByNewline();
	static std::string brightnessToString(Brightness brightness);
	static Brightness stringToBrightness(const std::string& brightness);
	static uint8_t brightnessToPercent(Brightness brightness);
	static std::string emotionsSeparatedByNewline();

	ProtogenHeadState();

	Emotion emotion() const;
	void setEmotion(Emotion emotion);
	bool forceBlink() const;
	void setForceBlink(bool forceBlink);
	void setBlank(bool blank);
	bool blank() const;
	void setBrightness(Brightness brightness);
	Brightness brightness() const;
	Emotion getEmotionConsideringForceBlink() const;
	virtual std::string toString() const override;
private:
	Emotion m_emotion;
	bool m_forceBlink;
	bool m_blank;
	Brightness m_brightness;
	mutable std::mutex m_mutex;

	static const Emotion FORCE_BLINK_EMOTION = Emotion::Blink;
};

#endif