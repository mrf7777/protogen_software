#include <protogen.h>

namespace protogen {

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
    runtime_opts.drop_privileges = -1;

    m_matrix = std::unique_ptr<rgb_matrix::RGBMatrix>(
        rgb_matrix::RGBMatrix::CreateFromOptions(
            options, runtime_opts
        )
    );
    if(m_matrix.get() == nullptr) {
        throw ConstructorException();
    }

    m_matrix->Clear();

    m_protogenFrameBuffer0 = m_matrix->CreateFrameCanvas();
    m_protogenFrameBuffer1 = m_matrix->CreateFrameCanvas();
};

std::optional<std::unique_ptr<ProtogenHeadMatrices>> ProtogenHeadMatrices::make()
{
    try {
        return {std::unique_ptr<ProtogenHeadMatrices>(new ProtogenHeadMatrices())};
    } catch(const ConstructorException&) {
        return {};
    }
}

ProtogenHeadMatrices::~ProtogenHeadMatrices()
{
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

void ProtogenHeadMatrices::drawFrame(const std::function<void(ICanvas&)>& drawer) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto frame = getNextProtogenFrameBuffer();
    frame->Clear();
    RgbMatrixCanvasToICanvasAdapter canvas(frame);
    drawer(canvas);
    m_matrix->SwapOnVSync(frame);
}

RgbMatrixCanvasToICanvasAdapter::RgbMatrixCanvasToICanvasAdapter(rgb_matrix::Canvas *canvas)
    : mCanvas(canvas)
{
}

int RgbMatrixCanvasToICanvasAdapter::width() const
{
    return mCanvas->width();
}

int RgbMatrixCanvasToICanvasAdapter::height() const
{
    return mCanvas->height();
}

void RgbMatrixCanvasToICanvasAdapter::setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{
    mCanvas->SetPixel(x, y, red, green, blue);
}

void RgbMatrixCanvasToICanvasAdapter::clear()
{
    mCanvas->Clear();
}

void RgbMatrixCanvasToICanvasAdapter::fill(uint8_t red, uint8_t green, uint8_t blue)
{
    mCanvas->Fill(red, green, blue);
}

}   // namespace