#include <renderer.h>

namespace render {

EmotionDrawer::EmotionDrawer(const std::string& emotions_directory)
    : m_emotionsDirectory(emotions_directory)
{
    for(const auto& emotion : ProtogenHeadState::allEmotions()) {
        // TODO: use std::filesystem to join file system paths.
        const auto emotion_images_dir = emotions_directory + "/" + emotion;
        m_emotionImageSpectrums.insert({
            emotion,
            image::ImageSpectrum(emotion_images_dir)
        });
    }
}

void EmotionDrawer::draw(ICanvas& canvas, ProtogenHeadState::Emotion emotion, Proportion eye_openness) const {
    auto image = m_emotionImageSpectrums.at(emotion).imageForValue(eye_openness);
    writeImageToCanvas(image, &canvas);
}

void EmotionDrawer::configWebServerToHostEmotionImages(
        httplib::Server& srv,
        const std::string& base_url_path) {
    srv.set_mount_point(base_url_path, m_emotionsDirectory);
}

ProtogenHeadFrameProvider::ProtogenHeadFrameProvider() {}

void ProtogenHeadFrameProvider::draw(ICanvas& canvas, ProtogenHeadState::Emotion emotion, Proportion mouth_openness, EmotionDrawer& emotion_drawer, image::ImageSpectrum& mouth_images, image::StaticImageDrawer& static_drawer, bool blank, Proportion eye_openness) {
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

MinecraftDrawer::MinecraftDrawer()
{}

void MinecraftDrawer::draw(ICanvas& canvas, const MinecraftState& state) const {
    drawWorld(canvas, state.blockMatrix(), state.blockColorProfile());
    drawPlayers(canvas, state);
}

void MinecraftDrawer::drawWorld(ICanvas& canvas, const mc::BlockMatrix& block_matrix, const mc::BlockColorProfile& color_profile) {
    for(std::size_t r = 0; r < block_matrix.rows(); r++)
    {
        for(std::size_t c = 0; c < block_matrix.cols(); c++)
        {
            const auto color = color_profile(block_matrix.get(r, c).value());
            canvas.setPixel(c, r, std::get<0>(color), std::get<1>(color), std::get<2>(color));
        }
    }
}

void MinecraftDrawer::drawPlayers(ICanvas& canvas, const MinecraftState& state) {
    const auto players = state.players();
    for(const auto& player_id : players) {
        state.accessPlayer(player_id, [&canvas, &state](const MinecraftPlayerState& player_state){
            drawPlayer(canvas, player_state, state.blockColorProfile());
        });
    }
}

void MinecraftDrawer::drawPlayer(ICanvas& canvas, const MinecraftPlayerState& player_state, const mc::BlockColorProfile& color_profile) {
    const auto color = color_profile(player_state.selectedBlock());
    const auto cursor = player_state.cursor();
    canvas.setPixel(cursor.second, cursor.first, std::get<0>(color), std::get<1>(color), std::get<2>(color));
}

Renderer::Renderer(EmotionDrawer emotion_drawer, MinecraftDrawer minecraft_drawer, const std::string& mouth_images_dir, const std::string& static_image_path)
    : m_emotionDrawer(emotion_drawer),
    m_minecraftDrawer(minecraft_drawer),
    m_staticImageDrawer(static_image_path),
    m_headImages(mouth_images_dir),
    m_protogenHeadFrameProvider()
{
}

void Renderer::render(const AppState& data, ICanvas& canvas) {
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

void Renderer::viewProtogenHeadData(const ProtogenHeadState& data, ICanvas& canvas) {
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

void Renderer::viewMinecraftData(const MinecraftState& data, ICanvas& canvas) {
    m_minecraftDrawer.draw(canvas, data);
}

}