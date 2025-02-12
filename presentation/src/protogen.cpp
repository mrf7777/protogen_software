#include <protogen/presentation/protogen.h>

#include <iostream>

namespace protogen {

ProtogenHeadMatrices::ProtogenHeadMatrices()
    : m_whichProtogenFrameBufferIsUsed(0)
{
};

ProtogenHeadMatrices::~ProtogenHeadMatrices()
{
    if(m_matrix) {
        m_matrix->Clear();
    }
}

std::string ProtogenHeadMatrices::id() const
{
    return "hub75_display";
}

std::string ProtogenHeadMatrices::name() const
{
    return "HUB75 Display";
}

std::string ProtogenHeadMatrices::description() const
{
    return "Implements support for HUB75 LED matrices. Many RGB LED matrices use the HUB75 interface.";
}

IRenderSurface::InitializationStatus ProtogenHeadMatrices::initialize()
{
    // TODO: rename this class to HUB75Display or something similar
    // TODO: parameterize these values and make them configurable
    // TODO: learn what other options are available
    try
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
            throw ConstructorException("Could not create RGBMatrix from provided options.");
        }

        m_matrix->Clear();

        m_protogenFrameBuffer0 = m_matrix->CreateFrameCanvas();
        m_protogenFrameBuffer1 = m_matrix->CreateFrameCanvas();
        return InitializationStatus::Success;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error initializing render surface \"" << name() << "\". Error: " << e.what() << std::endl;
        return InitializationStatus::Failure;
    }
    catch(...)
    {
        std::cerr << "Error initializing render surface \"" << name() << "\". Unknown error." << std::endl;
        return InitializationStatus::Failure;
    }
}

rgb_matrix::FrameCanvas *ProtogenHeadMatrices::getNextProtogenFrameBuffer()
{
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

Resolution ProtogenHeadMatrices::resolution() const
{
    return Resolution(128, 32);
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