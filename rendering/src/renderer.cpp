#include <protogen/rendering/renderer.h>

#include <filesystem>

namespace protogen {

EmotionDrawer::EmotionDrawer(const std::string& emotions_directory)
    : m_emotionsDirectory(emotions_directory)
{
    const auto emotions_dir = std::filesystem::path(emotions_directory);
    for(const auto& emotion_dir : std::filesystem::directory_iterator(emotions_dir)) {
        if(emotion_dir.is_directory()) {
            const auto emotion_name = emotion_dir.path().filename();
            m_emotionImageSpectrums.insert({
                emotion_name,
                ImageSpectrum(emotion_dir.path().string())
            });
        }
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

std::string EmotionDrawer::emotionsSeparatedByNewline() const
{
    std::string s;
    for(const auto& emotion : emotions()) {
        s += emotion;
        s += "\n";
    }
    return s;
}

std::vector<ProtogenHeadState::Emotion> EmotionDrawer::emotions() const
{
    std::vector<ProtogenHeadState::Emotion> emotions;
    for(const auto& entry : m_emotionImageSpectrums) {
        emotions.push_back(entry.first);
    }
    return emotions;
}

ProtogenHeadFrameProvider::ProtogenHeadFrameProvider() {}

void ProtogenHeadFrameProvider::draw(ICanvas& canvas, ProtogenHeadState::Emotion emotion, Proportion mouth_openness, EmotionDrawer& emotion_drawer, ImageSpectrum& mouth_images, StaticImageDrawer& static_drawer, bool blank, Proportion eye_openness) {
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

Renderer::Renderer(EmotionDrawer emotion_drawer, const std::string& mouth_images_dir, const std::string& static_image_path)
    : m_emotionDrawer(emotion_drawer),
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
    case AppState::Mode::App:
        data.getActiveApp()->render(canvas);
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

}   // namespace