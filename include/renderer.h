#ifndef RENDERER_H
#define RENDERER_H

#include <tuple>
#include <cstdint>
#include <mutex>
#include <unordered_map>

#include <canvas.h>

#include <audio.h>
#include <images.h>
#include <app_state.h>
#include <proportion.h>

namespace render
{

class EmotionDrawer final {
public:
	EmotionDrawer(const std::string& emotions_directory)
		: m_emotionsDirectory(emotions_directory)
	{
		for(const auto& emotion : ProtogenHeadState::allEmotions()) {
			// TODO: use std::filesystem to join file system paths.
			const auto emotion_images_dir = emotions_directory + "/" + ProtogenHeadState::emotionToString(emotion);
			m_emotionImageSpectrums.insert({
				emotion,
				image::ImageSpectrum(emotion_images_dir)
			});
		}
	}
	void draw(rgb_matrix::Canvas& canvas, ProtogenHeadState::Emotion emotion, Proportion eye_openness) const {
		auto image = m_emotionImageSpectrums.at(emotion).imageForValue(eye_openness);
		writeImageToCanvas(image, &canvas);
	}
	void configWebServerToHostEmotionImages(
			httplib::Server& srv,
			const std::string& base_url_path) {
		srv.set_mount_point(base_url_path, m_emotionsDirectory);
	}
private:
	std::unordered_map<ProtogenHeadState::Emotion, image::ImageSpectrum> m_emotionImageSpectrums;
	std::string m_emotionsDirectory;
};

class ProtogenHeadFrameProvider final {
public:
	ProtogenHeadFrameProvider() {}
	void draw(rgb_matrix::Canvas& canvas, ProtogenHeadState::Emotion emotion, Proportion mouth_openness, EmotionDrawer& emotion_drawer, image::ImageSpectrum& mouth_images, image::StaticImageDrawer& static_drawer, bool blank, Proportion eye_openness) {
		if(!blank) {
			// mouth
			auto mouth_image = mouth_images.imageForValue(mouth_openness);
			writeImageToCanvas(mouth_image, &canvas);
			// emotion
			emotion_drawer.draw(canvas, emotion, eye_openness);
			// static
			static_drawer.drawToCanvas(canvas);
		}
	}
};


class MinecraftDrawer final {
public:
	MinecraftDrawer()
	{}
	void draw(rgb_matrix::Canvas& canvas, const MinecraftState& state) const {
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

class IRender {
public:
    virtual ~IRender() = default;
    virtual void render(const AppState& data, rgb_matrix::Canvas& canvas) = 0;
};

class Renderer : public IRender{
public:
    Renderer(EmotionDrawer emotion_drawer, MinecraftDrawer minecraft_drawer, const std::string& mouth_images_dir, const std::string& static_image_path)
        : m_emotionDrawer(emotion_drawer),
        m_minecraftDrawer(minecraft_drawer),
        m_staticImageDrawer(static_image_path),
        m_headImages(mouth_images_dir),
        m_protogenHeadFrameProvider()
    {
    }

    virtual void render(const AppState& data, rgb_matrix::Canvas& canvas) override {
        std::lock_guard<std::mutex> lock(m_mutex);
		switch(data.mode()) {
		case AppState::Mode::ProtogenHead:
			viewProtogenHeadData(data.protogenHeadState(), canvas);
			break;
		case AppState::Mode::Minecraft:
			viewMinecraftData(data.minecraftState(), canvas);
			break;
		}
    }
private:
	void viewProtogenHeadData(const ProtogenHeadState& data, rgb_matrix::Canvas& canvas) {
        m_protogenHeadFrameProvider.draw(
            canvas,
            data.getEmotionConsideringForceBlink(),
            data.mouthOpenness(),
            m_emotionDrawer,
            m_headImages,
            m_staticImageDrawer,
            data.blank(),
			data.eyeOpenness()
        );
	}

	void viewMinecraftData(const MinecraftState& data, rgb_matrix::Canvas& canvas) {
		m_minecraftDrawer.draw(canvas, data);
	}

    EmotionDrawer m_emotionDrawer;
    MinecraftDrawer m_minecraftDrawer;
    image::StaticImageDrawer m_staticImageDrawer;
    image::ImageSpectrum m_headImages;
    ProtogenHeadFrameProvider m_protogenHeadFrameProvider;
    mutable std::mutex m_mutex;
};

}

#endif