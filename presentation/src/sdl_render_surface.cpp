#include <protogen/presentation/sdl_render_surface.h>

#include <SDL2/SDL.h>

namespace protogen {

std::optional<std::unique_ptr<SdlRenderSurface>> SdlRenderSurface::make(const Resolution& resolution)
{
    return std::optional<std::unique_ptr<SdlRenderSurface>>(new SdlRenderSurface(resolution));
}

SdlRenderSurface::~SdlRenderSurface()
{
    SDL_Quit();
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

SdlRenderSurface::SdlRenderSurface(const Resolution& resolution)
    : m_resolution(resolution)
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

}   // namespace