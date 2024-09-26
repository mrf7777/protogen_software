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
#include <tuple>

#include <graphics.h>
#include <canvas.h>
#include <led-matrix.h>
#include <pixel-mapper.h>

#include <Magick++.h>

#include <httplib.h>

#include "utils.h"
#include "images.h"
#include "minecraft.h"

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

class MinecraftState final {
public:
	MinecraftState() : m_blockMatrix(32, 128) {}
	const mc::BlockMatrix& blockMatrix() const { return m_blockMatrix; }
	mc::BlockMatrix& blockMatrix() { return m_blockMatrix; }
private:
	mc::BlockMatrix m_blockMatrix;
};

class AppState final : public IToString {
public:
	enum class Mode {
		ProtogenHead,
		Minecraft,
	};

	AppState() : m_mode(Mode::ProtogenHead) {}
	int frameRate() const {
		switch(m_mode) {
			case Mode::ProtogenHead:
				return 50;
			case Mode::Minecraft:
				return 10;
			default:
				return 60;
		}
	}
	Mode mode() const { return m_mode; }
	void setMode(Mode m) { m_mode = m; }
	ProtogenHeadState& protogenHeadState() {
		return m_protogenHeadState;
	}
	const ProtogenHeadState& protogenHeadState() const {
		return m_protogenHeadState;
	}
	MinecraftState& minecraftState() { return m_minecraftState; }
	const MinecraftState& minecraftState() const { return m_minecraftState; }

	virtual std::string toString() const override {
		return "AppState{protogenHeadState: " + m_protogenHeadState.toString() + "}";
	}
private:
	Mode m_mode;
	ProtogenHeadState m_protogenHeadState;
	MinecraftState m_minecraftState;
};

class EmotionDrawer final {
public:
	EmotionDrawer(const std::string& emotions_directory = "./protogen_images/eyes")
		: m_images(emotions_directory),
		m_emotionsDirectory(emotions_directory)
	{
	}
	void drawToCanvas(rgb_matrix::Canvas& canvas, ProtogenHeadState::Emotion emotion) {
		auto image = m_images.getImage(ProtogenHeadState::emotionToString(emotion));
		if(image.has_value()) {
			writeImageToCanvas(image.value(), &canvas);
		}
	}
	void configWebServerToHostEmotionImages(
			httplib::Server& srv,
			const std::string& base_url_path) {
		srv.set_mount_point(base_url_path, m_emotionsDirectory);
	}
private:
	image::ImagesDirectoryResource m_images;
	std::string m_emotionsDirectory;
};

class MinecraftDrawer final {
public:
	MinecraftDrawer() {}
	void drawToCanvas(rgb_matrix::Canvas& canvas, const MinecraftState& state) {
		canvas.Clear();
		for(std::size_t r = 0; r < state.blockMatrix().rows(); r++)
		{
			for(std::size_t c = 0; c < state.blockMatrix().cols(); c++)
			{
				const std::tuple<uint8_t, uint8_t, uint8_t> color = std::visit(overloaded{
					[](const mc::AirBlock){ return std::tuple{0, 0, 0}; },
					[](const mc::StoneBlock){ return std::tuple{127, 127, 127}; },
					[](const mc::DirtBlock){ return std::tuple{166, 81, 25}; },
					[](const mc::WoodBlock){ return std::tuple{255, 169, 41}; },
					[](const mc::GrassBlock){ return std::tuple{62, 191, 48}; },
					[](const mc::SandBlock){ return std::tuple{245, 255, 105}; },
					[](const mc::WaterBlock){ return std::tuple{87, 163, 222}; },
				}, state.blockMatrix().get(r, c).value().block());
				canvas.SetPixel(c, r, std::get<0>(color), std::get<1>(color), std::get<2>(color));
			}
		}
	}
};

class ProtogenHeadFrameProvider final {
public:
	ProtogenHeadFrameProvider(rgb_matrix::RGBMatrix* rgb_matrix, std::size_t mouth_states, EmotionDrawer& emotion_drawer, image::ImageSpectrum& mouth_images, image::StaticImageDrawer& static_drawer) {
		const auto emotions = ProtogenHeadState::allEmotions();
		// pre-render all of the face images per emotion, mouth state, and brightness.
		for(const auto& emotion : emotions) {
			const auto emotion_number = static_cast<int>(emotion);
			m_frameCanvases.push_back(std::vector<std::vector<rgb_matrix::FrameCanvas*>>());
			for(std::size_t mouth_state = 0; mouth_state < mouth_states; mouth_state++) {
				m_frameCanvases.at(emotion_number).push_back(std::vector<rgb_matrix::FrameCanvas*>());
				const auto brightness_levels = ProtogenHeadState::allBrightnessLevels();
				for(const auto& brightness : brightness_levels) {
					const auto brightness_number = static_cast<int>(brightness);
					auto frame = rgb_matrix->CreateFrameCanvas();
					if(frame == nullptr) {
						std::cerr << "Could not allocate a frame canvas during pre-rendering protogen head." << std::endl;
						abort();
					}
					m_frameCanvases.at(emotion_number).at(mouth_state).push_back(frame);
					renderFrame(frame, emotion, mouth_state, emotion_drawer, mouth_images, static_drawer, brightness);
				}
			}
		}
		// create a blank frame
		m_blankFrameCanvas = rgb_matrix->CreateFrameCanvas();
		m_blankFrameCanvas->Clear();
	}
	rgb_matrix::FrameCanvas* getFrame(ProtogenHeadState::Emotion emotion, std::size_t mouth_state, bool blank, ProtogenHeadState::Brightness brightness)  {
		if(blank) {
			return m_blankFrameCanvas;
		} else {
			return m_frameCanvases.at(static_cast<int>(emotion)).at(mouth_state).at(static_cast<int>(brightness));
		}
	}
private:
	void renderFrame(rgb_matrix::FrameCanvas* frame, ProtogenHeadState::Emotion emotion, std::size_t mouth_state, EmotionDrawer& emotion_drawer, image::ImageSpectrum& mouth_images, image::StaticImageDrawer& static_drawer, ProtogenHeadState::Brightness brightness) {
		// brightnes
		frame->SetBrightness(ProtogenHeadState::brightnessToPercent(brightness));
		// mouth
		auto mouth_image = mouth_images.images().at(mouth_state);
		writeImageToCanvas(mouth_image, frame);
		// emotion
		emotion_drawer.drawToCanvas(*frame, emotion);
		// static
		static_drawer.drawToCanvas(*frame);
	}

	// Stores pre-computed frames.
	// `m_frameCanvas[e][m][b]` returns a pre-rendered frame for:
	// emotion `e`, mouth position `m`, and brightness `b`. `e` is the integer version
	// of the emotion. `b` is the integer version of the brightness level.
	std::vector<std::vector<std::vector<rgb_matrix::FrameCanvas*>>> m_frameCanvases;
	rgb_matrix::FrameCanvas* m_blankFrameCanvas;
};

std::string read_file_to_str(const std::string& filename) {
	std::ifstream file(filename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

class ProtogenHeadMatrices final : public IViewData<AppState> {
public:
	ProtogenHeadMatrices(int argc, char *argv[], std::unique_ptr<audio::IAudioProvider> audio_provider, EmotionDrawer emotion_drawer)
		: m_emotionDrawer(emotion_drawer),
		m_staticImageDrawer("./protogen_images/static/nose.png"),
		m_minecraftFrameCanvasBuffer(nullptr)
	{
		m_audioProvider = std::move(audio_provider);
		m_headImages = image::ImageSpectrum("./protogen_images/mouth", m_audioProvider->min(), m_audioProvider->max());

		rgb_matrix::RGBMatrix::Options options;
		options.rows = 32;
		options.cols = 64;
		options.chain_length = 2;
		options.brightness = 100;
		options.hardware_mapping = "adafruit-hat";
		options.led_rgb_sequence = "RBG";

		rgb_matrix::RuntimeOptions runtime_opts;

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
		switch(data.mode()) {
		case AppState::Mode::ProtogenHead:
			viewProtogenHeadData(data.protogenHeadState());
			break;
		case AppState::Mode::Minecraft:
			viewMinecraftData(data.minecraftState());
			break;
		}
	}

	void clear() {
		m_matrix->Clear();
	}
private:
	void viewProtogenHeadData(const ProtogenHeadState& data) {
		const auto audio_level = m_audioProvider->audioLevel();
		const auto mouth_frame_index = m_headImages.spectrum().bucket(audio_level);
		const auto emotion = data.getEmotionConsideringForceBlink();
		const auto blank = data.blank();
		const auto brightness = data.brightness();
		auto frame = m_frameProvider->getFrame(emotion, mouth_frame_index, blank, brightness);
		m_matrix->SwapOnVSync(frame);
	}
	void viewMinecraftData(const MinecraftState& data) {
		// Ensure that there is exactly one frame canvas for drawing minecraft.
		if(m_minecraftFrameCanvasBuffer == nullptr)
		{
			m_minecraftFrameCanvasBuffer = m_matrix->CreateFrameCanvas();
		}
		m_minecraftDrawer.drawToCanvas(*m_minecraftFrameCanvasBuffer, data);
		m_matrix->SwapOnVSync(m_minecraftFrameCanvasBuffer);
	}
	std::unique_ptr<ProtogenHeadFrameProvider> m_frameProvider;
	std::unique_ptr<audio::IAudioProvider> m_audioProvider;
	std::unique_ptr<rgb_matrix::RGBMatrix> m_matrix;
	EmotionDrawer m_emotionDrawer;
	MinecraftDrawer m_minecraftDrawer;
	rgb_matrix::FrameCanvas * m_minecraftFrameCanvasBuffer;
	image::ImageSpectrum m_headImages;
	image::StaticImageDrawer m_staticImageDrawer;
	mutable std::mutex m_mutex;
};

#endif
