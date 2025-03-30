#include <protogen/presentation/protogen.h>

#include <protogen/StandardAttributeStore.hpp>
#include <protogen/StandardAttributes.hpp>

#include <iostream>

namespace protogen {

ProtogenHeadMatrices::ProtogenHeadMatrices()
    : m_whichProtogenFrameBufferIsUsed(0),
    m_attributes(new StandardAttributeStore())
{
    m_attributes->setAttribute(attributes::A_ID, "hub75_display");
    m_attributes->setAttribute(attributes::A_NAME, "HUB75 Display");
    m_attributes->setAttribute(attributes::A_DESCRIPTION, "Implements support for HUB75 LED matrices. Many RGB LED matrices use the HUB75 interface.");
};

ProtogenHeadMatrices::~ProtogenHeadMatrices()
{
    if(m_matrix) {
        m_matrix->Clear();
    }
}

IRenderSurface::Initialization ProtogenHeadMatrices::initialize()
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
        return Initialization::Success;
    }
    catch(const std::exception& e)
    {
        const std::string name = m_attributes->getAttribute(attributes::A_NAME).value_or("<no name>");
        std::cerr << "Error initializing render surface \"" << name << "\". Error: " << e.what() << std::endl;
        return Initialization::Failure;
    }
    catch(...)
    {
        const std::string name = m_attributes->getAttribute(attributes::A_NAME).value_or("<no name>");
        std::cerr << "Error initializing render surface \"" << name << "\". Unknown error." << std::endl;
        return Initialization::Failure;
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

std::optional<std::string> ProtogenHeadMatrices::getAttribute(const std::string &key) const
{
    return m_attributes->getAttribute(key);
}

std::vector<std::string> ProtogenHeadMatrices::listAttributes() const
{
    return m_attributes->listAttributes();
}

attributes::IWritableAttributeStore::SetAttributeResult ProtogenHeadMatrices::setAttribute(const std::string &key, const std::string &value)
{
    return m_attributes->setAttribute(key, value);
}

attributes::IWritableAttributeStore::RemoveAttributeResult ProtogenHeadMatrices::removeAttribute(const std::string &key)
{
    return m_attributes->removeAttribute(key);
}

bool ProtogenHeadMatrices::hasAttribute(const std::string &key) const
{
    return m_attributes->hasAttribute(key);
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