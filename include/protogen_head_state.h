#ifndef PROTOGEN_HEAD_STATE_H
#define PROTOGEN_HEAD_STATE_H

#include <string>
#include <vector>
#include <mutex>

#include <utils.h>
#include <protogen/Proportion.hpp>

namespace protogen {

class ProtogenHeadState final : public IToString {
public:
	using Emotion = std::string;

	enum class Brightness {
		ForBrightRoom,
		ForDarkRoom,
	};

	static std::vector<Emotion> allEmotions();
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
	void setMouthOpenness(Proportion openness);
	Proportion mouthOpenness() const;
	void setEyeOpenness(Proportion openness);
	Proportion eyeOpenness() const;
	virtual std::string toString() const override;
private:
	Emotion m_emotion;
	bool m_forceBlink;
	bool m_blank;
	Brightness m_brightness;
	Proportion m_mouthOpenness;
	Proportion m_eyeOpenness;
	mutable std::mutex m_mutex;

	// Cant be static, wont compile.
	const Emotion FORCE_BLINK_EMOTION = "blink";
};

}	// namespace

#endif