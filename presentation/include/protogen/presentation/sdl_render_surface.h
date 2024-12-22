#ifndef SDLRENDERSURFACE_H
#define SDLRENDERSURFACE_H

#include <optional>
#include <memory>

#include <SDL2/SDL.h>

#include <protogen/presentation/render_surface.h>
#include <protogen/ICanvas.hpp>
#include <protogen/Resolution.hpp>

namespace protogen {

/**
 * Takes an SDL_Renderer and adapts it to the ICanvas interface.
 * Does NOT take ownership of SDL_Renderer.
 */
class SdlRendererToICanvasAdapter : public ICanvas {
public:
    SdlRendererToICanvasAdapter(SDL_Renderer * renderer, int width, int height);
    int width() const override;
    int height() const override;
    void setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) override;
    void clear() override;
    void fill(uint8_t red, uint8_t green, uint8_t blue) override;
private:
    SDL_Renderer * m_renderer;
    int m_width;
    int m_height;
};

class SdlRenderSurface : public IRenderSurface {
public:
    static std::optional<std::unique_ptr<SdlRenderSurface>> make(const Resolution& resolution);
    ~SdlRenderSurface() override;
    void drawFrame(const std::function<void(ICanvas&)>& drawer) override;
    Resolution resolution() const override;
private:
    class ConstructorException : public std::runtime_error {
    public:
        ConstructorException(const char * what) : std::runtime_error(what) {}
    };
    SdlRenderSurface(const Resolution& resolution);

    struct WindowDestroyer {
        void operator()(SDL_Window * window) { SDL_DestroyWindow(window); }
    };
    struct RendererDestroyer {
        void operator()(SDL_Renderer * renderer) { SDL_DestroyRenderer(renderer); }
    };

    Resolution m_resolution;
    std::unique_ptr<SDL_Window, WindowDestroyer> m_window;
    std::unique_ptr<SDL_Renderer, RendererDestroyer> m_renderer;
};

}   // namespace

#endif