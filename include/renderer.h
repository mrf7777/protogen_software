#ifndef RENDERER_H
#define RENDERER_H

#include <tuple>
#include <cstdint>
#include <mutex>

#include <audio.h>
#include <images.h>
#include <app_state.h>
#include <render.h>

namespace render
{

class EmotionDrawer final {
public:
	EmotionDrawer(const std::string& emotions_directory)
		: m_images(emotions_directory),
		m_emotionsDirectory(emotions_directory)
	{
	}
	void draw(Render& render, ProtogenHeadState::Emotion emotion) const {
		auto image = m_images.getImage(ProtogenHeadState::emotionToString(emotion));
		if(image.has_value()) {
			writeImageToRender(image.value(), render);
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
	void draw(Render& render, ProtogenHeadState::Emotion emotion, std::size_t mouth_state, EmotionDrawer& emotion_drawer, image::ImageSpectrum& mouth_images, image::StaticImageDrawer& static_drawer, bool blank) {
		if(!blank) {
			// mouth
			auto mouth_image = mouth_images.images().at(mouth_state);
			writeImageToRender(mouth_image, render);
			// emotion
			emotion_drawer.draw(render, emotion);
			// static
			static_drawer.drawToRender(render);
		}
	}
};


class MinecraftDrawer final {
public:
	MinecraftDrawer()
	{}
	void draw(Render& render, const MinecraftState& state) const {
		drawWorld(render, state.blockMatrix(), state.blockColorProfile());
		drawPlayers(render, state);
	}
private:
	static void drawWorld(Render& render, const mc::BlockMatrix& block_matrix, const mc::BlockColorProfile& color_profile) {
		for(std::size_t r = 0; r < block_matrix.rows(); r++)
		{
			for(std::size_t c = 0; c < block_matrix.cols(); c++)
			{
				const auto color = color_profile(block_matrix.get(r, c).value());
                render.set(c, r, {std::get<0>(color), std::get<1>(color), std::get<2>(color)});
			}
		}
	}
	static void drawPlayers(Render& render, const MinecraftState& state) {
		const auto players = state.players();
		for(const auto& player_id : players) {
			state.accessPlayer(player_id, [&render, &state](const MinecraftPlayerState& player_state){
				drawPlayer(render, player_state, state.blockColorProfile());
			});
		}
	}
	static void drawPlayer(Render& render, const MinecraftPlayerState& player_state, const mc::BlockColorProfile& color_profile) {
		const auto color = color_profile(player_state.selectedBlock());
		const auto cursor = player_state.cursor();
        render.set(cursor.second, cursor.first, {std::get<0>(color), std::get<1>(color), std::get<2>(color)});
	}
};

class IRender {
public:
    virtual ~IRender() = default;
    virtual Render render(const AppState& data) = 0;
};

class Renderer : public IRender{
public:
    Renderer(std::unique_ptr<audio::IAudioProvider> audio_provider, EmotionDrawer emotion_drawer, MinecraftDrawer minecraft_drawer, const std::string& mouth_images_dir, const std::string& static_image_path)
        : m_audioProvider(std::move(audio_provider)),
        m_emotionDrawer(emotion_drawer),
        m_minecraftDrawer(minecraft_drawer),
        m_staticImageDrawer(static_image_path),
        m_headImages(mouth_images_dir, m_audioProvider->min(), m_audioProvider->max()),
        m_protogenHeadFrameProvider()
    {
    }

    virtual Render render(const AppState& data) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        Render render(128, 32);
		switch(data.mode()) {
		case AppState::Mode::ProtogenHead:
			viewProtogenHeadData(data.protogenHeadState(), render);
			break;
		case AppState::Mode::Minecraft:
			viewMinecraftData(data.minecraftState(), render);
			break;
		}
        return render;
    }
private:
	void viewProtogenHeadData(const ProtogenHeadState& data, Render& render) {
        m_protogenHeadFrameProvider.draw(
            render,
            data.getEmotionConsideringForceBlink(),
            m_headImages.spectrum().bucket(m_audioProvider->audioLevel()),
            m_emotionDrawer,
            m_headImages,
            m_staticImageDrawer,
            data.blank()
        );
	}

	void viewMinecraftData(const MinecraftState& data, Render& render) {
		m_minecraftDrawer.draw(render, data);
	}

    std::unique_ptr<audio::IAudioProvider> m_audioProvider;
    EmotionDrawer m_emotionDrawer;
    MinecraftDrawer m_minecraftDrawer;
    image::StaticImageDrawer m_staticImageDrawer;
    image::ImageSpectrum m_headImages;
    ProtogenHeadFrameProvider m_protogenHeadFrameProvider;
    mutable std::mutex m_mutex;
};

}

#endif