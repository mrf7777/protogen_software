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
#include <map>
#include <functional>

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

class MinecraftPlayerState final {
public:
	enum class CursorDirection {
		Up, Down, Left, Right,
	};
	static CursorDirection stringToCursorDirection(const std::string& cd) {
		if(cd == "up")
			return CursorDirection::Up;
		else if(cd == "down")
			return CursorDirection::Down;
		else if(cd == "left")
			return CursorDirection::Left;
		else if(cd == "right")
			return CursorDirection::Right;
		else
			return CursorDirection::Up;
	}
	using CursorPos = std::pair<std::size_t, std::size_t>;
	MinecraftPlayerState(std::size_t start_row, std::size_t start_col, std::size_t max_row, std::size_t max_col)
		: m_cursor{start_row, start_col},
		m_maxRow{max_row},
		m_maxCol{max_col},
		m_selectedBlock(mc::Block(mc::DirtBlock()))
	{
		correctCursor();
	}
	mc::Block selectedBlock() const {
		return m_selectedBlock;
	}
	void setSelectedBlock(const mc::Block& b) {
		m_selectedBlock = b;
	}
	CursorPos cursor() const {
		return m_cursor;
	}
	void moveCursor(CursorDirection direction) {
		switch(direction) {
		case CursorDirection::Up:
			if(m_cursor.first > 0)
				m_cursor.first -= 1;
			break;
		case CursorDirection::Down:
			if(m_cursor.first < (m_maxRow - 1))
				m_cursor.first += 1;
			break;
		case CursorDirection::Left:
			if(m_cursor.second > 0)
				m_cursor.second -= 1;
			break;
		case CursorDirection::Right:
			if(m_cursor.second < (m_maxCol - 1))
				m_cursor.second += 1;
			break;
		}
	}
private:
	void correctCursor() {
		// If cursor is out of bounds, put it back in bounds.
		if(m_cursor.first >= m_maxRow)
			m_cursor.first = m_maxRow - 1;
		if(m_cursor.second >= m_maxCol)
			m_cursor.second = m_maxCol - 1;
	}

	CursorPos m_cursor;
	std::size_t m_maxRow;
	std::size_t m_maxCol;
	mc::Block m_selectedBlock;
};

class MinecraftState final {
public:
	using PlayerId = std::string;

	MinecraftState()
		: m_blockMatrix(32, 128), 
		m_players()
	{}
	const mc::BlockMatrix& blockMatrix() const { return m_blockMatrix; }
	mc::BlockMatrix& blockMatrix() { return m_blockMatrix; }
	bool addNewPlayer(const PlayerId& id) {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		if(m_players.contains(id)) {
			return false;
		} else {
			const std::size_t number_players = m_players.size();
			const auto result = m_players.insert(std::make_pair(
				id,
				MinecraftPlayerState(0, number_players, 32, 128)
			));
			return true;
		}
	}
	void removePlayer(const PlayerId& id) {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		m_players.erase(id);
	}
	bool accessPlayer(const PlayerId& id, std::function<void(MinecraftPlayerState&)> accessor) {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		if(m_players.contains(id)) {
			accessor(m_players.at(id));
			return true;
		} else {
			return false;
		}
	}
	bool accessPlayer(const PlayerId& id, std::function<void(const MinecraftPlayerState&)> accessor) const {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		if(m_players.contains(id)) {
			accessor(m_players.at(id));
			return true;
		} else {
			return false;
		}
	}
	std::vector<PlayerId> players() const {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		return _players();
	}
	std::string playersSeperatedByNewline() const {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		std::string players_string;
		for(const auto player : _players()) {
			players_string += player + "\n";
		}
		return players_string;
	}
private:
	std::vector<PlayerId> _players() const {
		std::vector<PlayerId> players;
		for(const auto& player : m_players) {
			players.push_back(player.first);
		}
		return players;
	}

	mc::BlockMatrix m_blockMatrix;
	mutable std::mutex m_playerMutex;
	std::map<PlayerId, MinecraftPlayerState> m_players;
};

class AppState final : public IToString {
public:
	enum class Mode {
		ProtogenHead,
		Minecraft,
	};

	static std::string modeToString(Mode m) {
		switch(m) {
		case Mode::ProtogenHead:
			return "protogen_head";
		case Mode::Minecraft:
			return "minecraft";
		default:
			return "";
		}
	}

	static Mode stringToMode(const std::string& m) {
		if(m == "protogen_head")
			return Mode::ProtogenHead;
		else if(m == "minecraft")
			return Mode::Minecraft;
		else
			return Mode::ProtogenHead;
	}

	static std::vector<Mode> allModes() {
		return {Mode::ProtogenHead, Mode::Minecraft};
	}

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
	MinecraftDrawer(const mc::BlockColorProfile& color_profile)
		: m_blockColorProfile(color_profile)
	{}
	void drawToCanvas(rgb_matrix::Canvas& canvas, const MinecraftState& state) {
		drawWorld(canvas, state.blockMatrix(), m_blockColorProfile);
		drawPlayers(canvas, state, m_blockColorProfile);
	}
	mc::BlockColorProfile colorProfile() const {
		return m_blockColorProfile;
	}
private:
	static void drawWorld(rgb_matrix::Canvas& canvas, const mc::BlockMatrix& block_matrix, const mc::BlockColorProfile& color_profile) {
		for(std::size_t r = 0; r < block_matrix.rows(); r++)
		{
			for(std::size_t c = 0; c < block_matrix.cols(); c++)
			{
				const auto color = color_profile(block_matrix.get(r, c).value());
				canvas.SetPixel(c, r, std::get<0>(color), std::get<1>(color), std::get<2>(color));
			}
		}
	}
	static void drawPlayers(rgb_matrix::Canvas& canvas, const MinecraftState& state, const mc::BlockColorProfile& color_profile) {
		const auto players = state.players();
		for(const auto& player_id : players) {
			state.accessPlayer(player_id, [&canvas, &color_profile](const MinecraftPlayerState& player_state){
				drawPlayer(canvas, player_state, color_profile);
			});
		}
	}
	static void drawPlayer(rgb_matrix::Canvas& canvas, const MinecraftPlayerState& player_state, const mc::BlockColorProfile& color_profile) {
		const auto color = color_profile(player_state.selectedBlock());
		const auto cursor = player_state.cursor();
		canvas.SetPixel(cursor.second, cursor.first, std::get<0>(color), std::get<1>(color), std::get<2>(color));
	}

	mc::BlockColorProfile m_blockColorProfile;
};

class ProtogenHeadFrameProvider final {
public:
	ProtogenHeadFrameProvider() {}
	void renderFrame(rgb_matrix::FrameCanvas* frame, ProtogenHeadState::Emotion emotion, std::size_t mouth_state, EmotionDrawer& emotion_drawer, image::ImageSpectrum& mouth_images, image::StaticImageDrawer& static_drawer, ProtogenHeadState::Brightness brightness, bool blank) {
		frame->Clear();
		if(!blank) {
			// brightness
			frame->SetBrightness(ProtogenHeadState::brightnessToPercent(brightness));
			// mouth
			auto mouth_image = mouth_images.images().at(mouth_state);
			writeImageToCanvas(mouth_image, frame);
			// emotion
			emotion_drawer.drawToCanvas(*frame, emotion);
			// static
			static_drawer.drawToCanvas(*frame);
		}
	}
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
		m_minecraftFrameCanvasBuffer(nullptr),
		m_whichProtogenFrameBufferIsUsed(0),
		m_minecraftDrawer(mc::defaultBlockColorProfile)
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

		m_frameProvider = std::unique_ptr<ProtogenHeadFrameProvider>(new ProtogenHeadFrameProvider());

		m_protogenFrameBuffer0 = m_matrix->CreateFrameCanvas();
		m_protogenFrameBuffer1 = m_matrix->CreateFrameCanvas();
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
		std::lock_guard<std::mutex> lock(m_mutex);
		m_matrix->Clear();
	}
	mc::BlockColorProfile minecraftColorProfile() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_minecraftDrawer.colorProfile();
	}
private:
	void viewProtogenHeadData(const ProtogenHeadState& data) {
		const auto audio_level = m_audioProvider->audioLevel();
		const auto mouth_frame_index = m_headImages.spectrum().bucket(audio_level);
		const auto emotion = data.getEmotionConsideringForceBlink();
		const auto blank = data.blank();
		const auto brightness = data.brightness();

		auto current_frame_buffer = getNextProtogenFrameBuffer();
		m_frameProvider->renderFrame(
			current_frame_buffer, 
			data.getEmotionConsideringForceBlink(), 
			m_headImages.spectrum().bucket(m_audioProvider->audioLevel()), 
			m_emotionDrawer, 
			m_headImages, 
			m_staticImageDrawer,
			data.brightness(),
			data.blank());
		m_matrix->SwapOnVSync(current_frame_buffer);
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
	rgb_matrix::FrameCanvas * getNextProtogenFrameBuffer() {
		switch(m_whichProtogenFrameBufferIsUsed) {
		case 0:
			m_whichProtogenFrameBufferIsUsed = 1;
			return m_protogenFrameBuffer1;
		case 1:
			m_whichProtogenFrameBufferIsUsed = 0;
			return m_protogenFrameBuffer0;
		}
	}
	std::unique_ptr<ProtogenHeadFrameProvider> m_frameProvider;
	std::unique_ptr<audio::IAudioProvider> m_audioProvider;
	std::unique_ptr<rgb_matrix::RGBMatrix> m_matrix;
	EmotionDrawer m_emotionDrawer;
	MinecraftDrawer m_minecraftDrawer;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer0;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer1;
	unsigned int m_whichProtogenFrameBufferIsUsed;
	rgb_matrix::FrameCanvas * m_minecraftFrameCanvasBuffer;
	image::ImageSpectrum m_headImages;
	image::StaticImageDrawer m_staticImageDrawer;
	mutable std::mutex m_mutex;
};

#endif
