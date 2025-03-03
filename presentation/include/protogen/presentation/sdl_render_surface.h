#ifndef SDLRENDERSURFACE_H
#define SDLRENDERSURFACE_H

#include <optional>
#include <memory>

#include <SDL2/SDL.h>

#include <protogen/presentation/render_surface.h>
#include <protogen/ICanvas.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/IAttributeStore.hpp>

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

/**
 * Represents an SDL window as a render surface.
 * 
 * The width and height of this render surface can be set by setting the
 * environment variables PROTOGEN_SDL_RENDER_SURFACE_WIDTH and
 * PROTOGEN_SDL_RENDER_SURFACE_HEIGHT, respectively. Otherwise, the default
 * width and height will be used.
 */
class SdlRenderSurface : public IRenderSurface {
public:
    SdlRenderSurface();
    ~SdlRenderSurface() override;
    Initialization initialize() override;
    void drawFrame(const std::function<void(ICanvas&)>& drawer) override;
    Resolution resolution() const override;
    std::shared_ptr<attributes::IAttributeStore> getAttributeStore() override;
private:
    class ConstructorException : public std::runtime_error {
    public:
        ConstructorException(const char * what) : std::runtime_error(what) {}
    };

    struct WindowDestroyer {
        void operator()(SDL_Window * window) { SDL_DestroyWindow(window); }
    };
    struct RendererDestroyer {
        void operator()(SDL_Renderer * renderer) { SDL_DestroyRenderer(renderer); }
    };

    Resolution m_resolution;
    std::unique_ptr<SDL_Window, WindowDestroyer> m_window;
    std::unique_ptr<SDL_Renderer, RendererDestroyer> m_renderer;

    std::shared_ptr<attributes::IAttributeStore> m_attributes;

    static constexpr const char * ENV_VAR_WIDTH = "PROTOGEN_SDL_RENDER_SURFACE_WIDTH";
    static constexpr const char * ENV_VAR_HEIGHT = "PROTOGEN_SDL_RENDER_SURFACE_HEIGHT";
    static const unsigned int DEFAULT_WIDTH = 128;
    static const unsigned int DEFAULT_HEIGHT = 32;
};

}   // namespace

#endif