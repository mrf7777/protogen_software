#include <protogen.h>


ProtogenHeadMatrices::ProtogenHeadMatrices()
    : m_whichProtogenFrameBufferIsUsed(0)
{
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

    m_protogenFrameBuffer0 = m_matrix->CreateFrameCanvas();
    m_protogenFrameBuffer1 = m_matrix->CreateFrameCanvas();
};

ProtogenHeadMatrices::~ProtogenHeadMatrices() {
    m_matrix->Clear();
}

rgb_matrix::FrameCanvas * ProtogenHeadMatrices::getNextProtogenFrameBuffer() {
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

void ProtogenHeadMatrices::drawFrame(const std::function<void(rgb_matrix::Canvas&)>& drawer) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto frame = getNextProtogenFrameBuffer();
    frame->Clear();
    drawer(*frame);
    m_matrix->SwapOnVSync(frame);
}
