#ifndef RENDERER_H
#define RENDERER_H

#include <tuple>
#include <cstdint>
#include <mutex>

#include <audio.h>
#include <images.h>
#include <app_state.h>

#include <canvas.h>

namespace render
{

class EmotionDrawer final {
public:
	EmotionDrawer(const std::string& emotions_directory)
		: m_images(emotions_directory),
		m_emotionsDirectory(emotions_directory)
	{
	}
	void draw(rgb_matrix::Canvas& canvas, ProtogenHeadState::Emotion emotion) const {
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

class ProtogenHeadFrameProvider final {
public:
	ProtogenHeadFrameProvider() {}
	void draw(rgb_matrix::Canvas& canvas, ProtogenHeadState::Emotion emotion, std::size_t mouth_state, EmotionDrawer& emotion_drawer, image::ImageSpectrum& mouth_images, image::StaticImageDrawer& static_drawer, bool blank) {
		if(!blank) {
			// mouth
			auto mouth_image = mouth_images.images().at(mouth_state);
			writeImageToCanvas(mouth_image, &canvas);
			// emotion
			emotion_drawer.draw(canvas, emotion);
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
        m_headImages(mouth_images_dir, 0.0, 1.0),
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
            m_headImages.spectrum().bucket(data.mouthOpenness()),
            m_emotionDrawer,
            m_headImages,
            m_staticImageDrawer,
            data.blank()
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