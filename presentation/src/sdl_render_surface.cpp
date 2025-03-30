#include <protogen/presentation/sdl_render_surface.h>

#include <protogen/StandardAttributeStore.hpp>

#include <iostream>
#include <cstdlib>

#include <SDL2/SDL.h>

namespace protogen {

SdlRenderSurface::~SdlRenderSurface()
{
    SDL_Quit();
}

SdlRenderSurface::Initialization SdlRenderSurface::initialize()
{
    try
    {
        {
            const int sdl_init_result = SDL_Init(SDL_INIT_VIDEO);
            if(sdl_init_result) {
                throw ConstructorException(SDL_GetError());
            }
        }

        {
            auto window = SDL_CreateWindow(
                "Protogen Head",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                m_resolution.width() * 8,
                m_resolution.height() * 8,
                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
            );
            if(window == NULL) {
                throw ConstructorException(SDL_GetError());
            }
            m_window = std::unique_ptr<SDL_Window, WindowDestroyer>(window);
        }

        {
            auto renderer = SDL_CreateRenderer(
                m_window.get(),
                -1,
                SDL_RENDERER_ACCELERATED
            );
            if(renderer == NULL) {
                throw ConstructorException(SDL_GetError());
            }
            const auto set_logical_size_result = SDL_RenderSetLogicalSize(renderer, m_resolution.width(), m_resolution.height());
            if(set_logical_size_result) {
                throw ConstructorException(SDL_GetError());
            }
            m_renderer = std::unique_ptr<SDL_Renderer, RendererDestroyer>(renderer);
        }

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

void SdlRenderSurface::drawFrame([[maybe_unused]] const std::function<void(ICanvas &)> &drawer)
{
    SdlRendererToICanvasAdapter canvas(m_renderer.get(), m_resolution.width(), m_resolution.height());
    canvas.clear();
    drawer(canvas);
    SDL_RenderPresent(m_renderer.get());
}

Resolution SdlRenderSurface::resolution() const
{
    return m_resolution;
}

std::optional<std::string> SdlRenderSurface::getAttribute(const std::string &key) const
{
    return m_attributes->getAttribute(key);
}

std::vector<std::string> SdlRenderSurface::listAttributes() const
{
    return m_attributes->listAttributes();
}

attributes::IWritableAttributeStore::SetAttributeResult SdlRenderSurface::setAttribute(const std::string &key, const std::string &value)
{
    return m_attributes->setAttribute(key, value);
}

attributes::IWritableAttributeStore::RemoveAttributeResult SdlRenderSurface::removeAttribute(const std::string &key)
{
    return m_attributes->removeAttribute(key);
}

bool SdlRenderSurface::hasAttribute(const std::string &key) const
{
    return m_attributes->hasAttribute(key);
}

SdlRenderSurface::SdlRenderSurface()
    : m_resolution(0, 0),
      m_attributes(new StandardAttributeStore())
{
    unsigned int width;
    unsigned int height;

    const char * width_string = std::getenv(ENV_VAR_WIDTH);
    if(width_string == NULL) {
        width = DEFAULT_WIDTH;
    } else {
        width = std::atoi(width_string);
        if(width == 0) {
            width = DEFAULT_WIDTH;
        }
    }

    const char * height_string = std::getenv(ENV_VAR_HEIGHT);
    if(height_string == NULL) {
        height = DEFAULT_HEIGHT;
    } else {
        height = std::atoi(height_string);
        if(height == 0) {
            height = DEFAULT_HEIGHT;
        }
    }

    m_resolution = Resolution(width, height);

    m_attributes->setAttribute(attributes::A_ID, "sdl_window");
    m_attributes->setAttribute(attributes::A_NAME, "SDL Window");
    m_attributes->setAttribute(attributes::A_DESCRIPTION, "Implements support for showing imagery with a window using SDL. This allows for development and testing the imagery without the need for dedicated protogen hardware and uses your monitor instead.");
}

SdlRendererToICanvasAdapter::SdlRendererToICanvasAdapter(SDL_Renderer *renderer, int width, int height)
    : m_renderer(renderer), m_width(width), m_height(height)
{}

int SdlRendererToICanvasAdapter::width() const
{
    return m_width;
}

int SdlRendererToICanvasAdapter::height() const
{
    return m_height;
}

void SdlRendererToICanvasAdapter::setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{
    SDL_SetRenderDrawColor(m_renderer, red, green, blue, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawPoint(m_renderer, x, y);
}

void SdlRendererToICanvasAdapter::clear()
{
    fill(0, 0, 0);
}

void SdlRendererToICanvasAdapter::fill(uint8_t red, uint8_t green, uint8_t blue)
{
    SDL_SetRenderDrawColor(m_renderer, red, green, blue, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(m_renderer);
}

} // namespace