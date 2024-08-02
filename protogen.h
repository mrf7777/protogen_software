#ifndef PROTOGEN_H
#define PROTOGEN_H

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <thread>
#include <chrono>
#include <mutex>

#include <graphics.h>
#include <canvas.h>
#include <led-matrix.h>
#include <pixel-mapper.h>

#include <Magick++.h>

#include "utils.h"
#include "images.h"

class ProtogenHeadState final : public IToString {
public:
	enum class Emotion {
		Normal,
		Angry,
		Flustered,
		Sad,
	};

	static Emotion emotionFromString(const std::string& s) {
		if(s == "normal")
			return Emotion::Normal;
		else if(s == "angry")
			return Emotion::Angry;
		else if(s == "flustered")
			return Emotion::Flustered;
		else if(s == "sad")
			return Emotion::Sad;
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
		}
	}

	ProtogenHeadState()
		: m_emotion(Emotion::Normal),
		m_mouthColor(RGBColor(0, 255, 0)),
	        m_eyeColor(RGBColor(0, 255, 0))	
	{}

	Emotion emotion() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_emotion;
	}
	void setEmotion(Emotion emotion) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_emotion = emotion;
	}
	RGBColor mouthColor() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_mouthColor;
	}
	void setMouthColor(const RGBColor& color) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_mouthColor = color;
	}
	void setEyeColor(const RGBColor& color) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_eyeColor = color;
	}

	virtual std::string toString() const override {
		std::lock_guard<std::mutex> lock(m_mutex);
		return "ProtogenHeadState{emotion: " + emotionToString(m_emotion) + ", eyeColor: " + m_eyeColor.toString() + ", mouthColor: " + m_mouthColor.toString() + "}";
	}

private:
	Emotion m_emotion;
	RGBColor m_mouthColor;
	RGBColor m_eyeColor;
	mutable std::mutex m_mutex;
};

class AppState final : public IToString {
public:
	AppState() {}
	ProtogenHeadState& protogenHeadState() {
		return m_protogenHeadState;
	}
	const ProtogenHeadState& protogenHeadState() const {
		return m_protogenHeadState;
	}

	virtual std::string toString() const override {
		return "AppState{protogenHeadState: " + m_protogenHeadState.toString() + "}";
	}
private:
	ProtogenHeadState m_protogenHeadState;
};


class EmotionDrawer final {
public:
	EmotionDrawer(const std::string& emotion_images_path = "./protogen_images/eyes") {
		std::vector<std::filesystem::path> files_in_directory;
		for(const auto& file : std::filesystem::directory_iterator(emotion_images_path)) {
			files_in_directory.push_back(file.path());
		}
		// bind image files to the corresponding emotions
		std::vector<ProtogenHeadState::Emotion> emotions{
			ProtogenHeadState::Emotion::Normal,
			ProtogenHeadState::Emotion::Sad,
			ProtogenHeadState::Emotion::Angry,
			ProtogenHeadState::Emotion::Flustered,
		};
		for(const auto& emotion : emotions) {
			auto file_for_emotion = std::find_if(files_in_directory.begin(), files_in_directory.end(), [emotion](std::filesystem::path& p){
				if(p.filename().string().find(ProtogenHeadState::emotionToString(emotion)) != std::string::npos) {
					// found image file for the emotion
					return true;
				}
				else
					return false;
			});
			auto image_for_emotion = image::loadImage(file_for_emotion->string());
			if(image_for_emotion.has_value()) {
				m_emotionToImage.insert({emotion, image_for_emotion.value()});
			}
		}
	}
	void drawToCanvas(rgb_matrix::Canvas& canvas, ProtogenHeadState::Emotion emotion) {
		writeImageToCanvas(m_emotionToImage.at(emotion), &canvas);
	}
private:
	std::map<ProtogenHeadState::Emotion, Magick::Image> m_emotionToImage;
};


class ProtogenHeadMatrices final : public IViewData<AppState> {
public:
	ProtogenHeadMatrices(int argc, char *argv[], std::unique_ptr<audio::IAudioProvider> audio_provider)
       		: m_headImages("./protogen_images/mouth", 0.0, 255.0),
		m_audioProvider(std::move(audio_provider)),
		m_emotionDrawer(),
		m_staticImageDrawer("./protogen_images/static/nose.png")
	{
		rgb_matrix::RGBMatrix::Options options;
		options.rows = 32;
		options.cols = 64;
		options.chain_length = 2;
		options.brightness = 100;
		options.hardware_mapping = "adafruit-hat";
		options.led_rgb_sequence = "RBG";

		rgb_matrix::RuntimeOptions runtime_opts;
		runtime_opts.drop_privileges = -1;

		m_matrix = std::unique_ptr<rgb_matrix::RGBMatrix>(
			rgb_matrix::RGBMatrix::CreateFromOptions(
				options, runtime_opts
			)
		);
		m_matrix->Clear();
	};
	~ProtogenHeadMatrices() {
		m_matrix->Clear();
	}
	virtual void viewData(const AppState& data) override {
		std::lock_guard<std::mutex> lock(m_mutex);
		std::cout << data.toString() << std::endl;
		m_matrix->Clear();
		//m_matrix->SetPixel(0, 0, 255, 255, 255);
		//m_matrix->SetPixel(data.protogenHeadState().mouthColor().r()/2, 31, 255, 255, 255);
		const auto audio_level = m_audioProvider->audioLevel();
		writeImageToCanvas(m_headImages.imageForValue(audio_level), m_matrix.get());
		m_emotionDrawer.drawToCanvas(*m_matrix, data.protogenHeadState().emotion());
		m_staticImageDrawer.drawToCanvas(*m_matrix);
	}
private:
	std::unique_ptr<audio::IAudioProvider> m_audioProvider;
	std::unique_ptr<rgb_matrix::RGBMatrix> m_matrix;
	EmotionDrawer m_emotionDrawer;
	image::ImageSpectrum m_headImages;
	image::StaticImageDrawer m_staticImageDrawer;
	mutable std::mutex m_mutex;
};

std::string read_file_to_str(const std::string& filename) {
	std::ifstream file(filename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

#endif
