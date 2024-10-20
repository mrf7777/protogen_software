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
#include <app_state.h>

class EmotionDrawer final {
public:
	EmotionDrawer(const std::string& emotions_directory = "./resources/protogen_images/eyes")
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
	MinecraftDrawer()
	{}
	void drawToCanvas(rgb_matrix::Canvas& canvas, const MinecraftState& state) {
		drawWorld(canvas, state.blockMatrix(), state.blockColorProfile());
		drawPlayers(canvas, state);
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
	static void drawPlayers(rgb_matrix::Canvas& canvas, const MinecraftState& state) {
		const auto players = state.players();
		for(const auto& player_id : players) {
			state.accessPlayer(player_id, [&canvas, &state](const MinecraftPlayerState& player_state){
				drawPlayer(canvas, player_state, state.blockColorProfile());
			});
		}
	}
	static void drawPlayer(rgb_matrix::Canvas& canvas, const MinecraftPlayerState& player_state, const mc::BlockColorProfile& color_profile) {
		const auto color = color_profile(player_state.selectedBlock());
		const auto cursor = player_state.cursor();
		canvas.SetPixel(cursor.second, cursor.first, std::get<0>(color), std::get<1>(color), std::get<2>(color));
	}
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
	ProtogenHeadMatrices(std::unique_ptr<audio::IAudioProvider> audio_provider, EmotionDrawer emotion_drawer)
		: m_emotionDrawer(emotion_drawer),
		m_minecraftDrawer(),
		m_whichProtogenFrameBufferIsUsed(0),
		m_minecraftFrameCanvasBuffer(nullptr),
		m_staticImageDrawer("./resources/protogen_images/static/nose.png")
	{
		m_audioProvider = std::move(audio_provider);
		m_headImages = image::ImageSpectrum("./resources/protogen_images/mouth", m_audioProvider->min(), m_audioProvider->max());

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
private:
	void viewProtogenHeadData(const ProtogenHeadState& data) {
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
		default:
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
