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
#include <cstdlib>

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
		Disappointed,
		Flirty,
		Scared,
		Owo,
		Uwu,
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
	EmotionDrawer(const std::string& emotions_directory = "./protogen_images/eyes")
		: m_images(emotions_directory)	
	{
	}
	void drawToCanvas(rgb_matrix::Canvas& canvas, ProtogenHeadState::Emotion emotion) {
		auto image = m_images.getImage(ProtogenHeadState::emotionToString(emotion));
		if(image.has_value()) {
			writeImageToCanvas(image.value(), &canvas);
		}
	}
private:
	image::ImagesDirectoryResource m_images;
};

class ProtogenHeadFrameProvider final {
public:
	ProtogenHeadFrameProvider(rgb_matrix::RGBMatrix* rgb_matrix, std::size_t mouth_states, EmotionDrawer& emotion_drawer, image::ImageSpectrum& mouth_images, image::StaticImageDrawer& static_drawer) {
		const auto emotions = ProtogenHeadState::allEmotions();
		// pre-render all of the face images per emotion and per mouth state
		for(const auto& emotion : emotions) {
			const auto emotion_number = static_cast<int>(emotion);
			m_frameCanvases.push_back(std::vector<rgb_matrix::FrameCanvas*>());
			for(std::size_t mouth_state = 0; mouth_state < mouth_states; mouth_state++) {
				auto frame = rgb_matrix->CreateFrameCanvas();
				if(frame == nullptr) {
					std::cerr << "Could not allocate a frame canvas during pre-rendering protogen head." << std::endl;
					abort();
				}
				m_frameCanvases.at(emotion_number).push_back(frame);
				renderFrame(frame, emotion, mouth_state, emotion_drawer, mouth_images, static_drawer);
			}
		}
	}
	rgb_matrix::FrameCanvas* getFrame(ProtogenHeadState::Emotion emotion, std::size_t mouth_state)  {
		return m_frameCanvases.at(static_cast<int>(emotion)).at(mouth_state);
	}
private:
	void renderFrame(rgb_matrix::FrameCanvas* frame, ProtogenHeadState::Emotion emotion, std::size_t mouth_state, EmotionDrawer& emotion_drawer, image::ImageSpectrum& mouth_images, image::StaticImageDrawer& static_drawer) {
		// std::cout << "Rendering frame. Emotion: " << ProtogenHeadState::emotionToString(emotion) << " mouth_state: " << mouth_state << " number of mouths: " << mouth_images.images().size() << std::endl;
		frame->Clear();
		// mouth
		auto mouth_image = mouth_images.images().at(mouth_state);
		writeImageToCanvas(mouth_image, frame);
		// emotion
		emotion_drawer.drawToCanvas(*frame, emotion);
		// static
		static_drawer.drawToCanvas(*frame);
	}

	// Stores pre-computed frames.
	// `m_frameCanvas[e][m]` returns a pre-rendered frame for:
	// emotion `e` and mouth position `m`. `e` is the integer version
	// of the emotion.
	std::vector<std::vector<rgb_matrix::FrameCanvas*>> m_frameCanvases;
};

std::string read_file_to_str(const std::string& filename) {
	std::ifstream file(filename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

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

		m_frameProvider = std::unique_ptr<ProtogenHeadFrameProvider>(new ProtogenHeadFrameProvider(
			m_matrix.get(),
			m_headImages.images().size(),
			m_emotionDrawer,
			m_headImages,
			m_staticImageDrawer
		));
	};
	~ProtogenHeadMatrices() {
		m_matrix->Clear();
	}
	virtual void viewData(const AppState& data) override {
		std::lock_guard<std::mutex> lock(m_mutex);
		std::cout << data.toString() << std::endl;
		const auto audio_level = m_audioProvider->audioLevel();
		const auto mouth_frame_index = m_headImages.spectrum().bucket(audio_level);
		const auto emotion = data.protogenHeadState().emotion();
		auto frame = m_frameProvider->getFrame(emotion, mouth_frame_index);
		m_matrix->SwapOnVSync(frame);
	}
	void clear() {
		m_matrix->Clear();
	}
private:
	std::unique_ptr<ProtogenHeadFrameProvider> m_frameProvider;
	std::unique_ptr<audio::IAudioProvider> m_audioProvider;
	std::unique_ptr<rgb_matrix::RGBMatrix> m_matrix;
	EmotionDrawer m_emotionDrawer;
	image::ImageSpectrum m_headImages;
	image::StaticImageDrawer m_staticImageDrawer;
	mutable std::mutex m_mutex;
};

#endif
