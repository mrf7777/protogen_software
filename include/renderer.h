#ifndef RENDERER_H
#define RENDERER_H

#include <tuple>
#include <cstdint>
#include <mutex>
#include <unordered_map>

#include <canvas.h>

#include <ICanvas.hpp>
#include <audio.h>
#include <images.h>
#include <app_state.h>
#include <proportion.h>

namespace render
{

class EmotionDrawer final {
public:
	EmotionDrawer(const std::string& emotions_directory);
	void draw(ICanvas& canvas, ProtogenHeadState::Emotion emotion, Proportion eye_openness) const;
	void configWebServerToHostEmotionImages(httplib::Server& srv, const std::string& base_url_path);
private:
	std::unordered_map<ProtogenHeadState::Emotion, image::ImageSpectrum> m_emotionImageSpectrums;
	std::string m_emotionsDirectory;
};

class ProtogenHeadFrameProvider final {
public:
	ProtogenHeadFrameProvider();
	void draw(ICanvas& canvas, ProtogenHeadState::Emotion emotion, Proportion mouth_openness, EmotionDrawer& emotion_drawer, image::ImageSpectrum& mouth_images, image::StaticImageDrawer& static_drawer, bool blank, Proportion eye_openness);
};

class MinecraftDrawer final {
public:
	MinecraftDrawer();
	void draw(ICanvas& canvas, const MinecraftState& state) const;
private:
	static void drawWorld(ICanvas& canvas, const mc::BlockMatrix& block_matrix, const mc::BlockColorProfile& color_profile);
	static void drawPlayers(ICanvas& canvas, const MinecraftState& state);
	static void drawPlayer(ICanvas& canvas, const MinecraftPlayerState& player_state, const mc::BlockColorProfile& color_profile);
};

class IRender {
public:
    virtual ~IRender() = default;
    virtual void render(const AppState& data, ICanvas& canvas) = 0;
};

class Renderer : public IRender {
public:
    Renderer(EmotionDrawer emotion_drawer, MinecraftDrawer minecraft_drawer, const std::string& mouth_images_dir, const std::string& static_image_path);
    virtual void render(const AppState& data, ICanvas& canvas) override;
private:
	void viewProtogenHeadData(const ProtogenHeadState& data, ICanvas& canvas);

	void viewMinecraftData(const MinecraftState& data, ICanvas& canvas);

    EmotionDrawer m_emotionDrawer;
    MinecraftDrawer m_minecraftDrawer;
    image::StaticImageDrawer m_staticImageDrawer;
    image::ImageSpectrum m_headImages;
    ProtogenHeadFrameProvider m_protogenHeadFrameProvider;
    mutable std::mutex m_mutex;
};

}

#endif