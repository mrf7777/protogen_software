#ifndef PROTOGEN_ICANVAS_H
#define PROTOGEN_ICANVAS_H

#include <cstdint>

/**
 * Represents a surface to draw imagery.
 */
class ICanvas {
public:
    virtual ~ICanvas() {}

    /**
     * Width in pixels.
     */
    virtual int width() const = 0;
    /**
     * Height in pixels.
     */
    virtual int height() const = 0;
    
    /**
     * Sets pixel color. 
     */
    virtual void setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) = 0;
    /**
     * Sets entire canvas black.
     */
    virtual void clear() = 0;
    /**
     * Sets entire canvas to provided color.
     */
    virtual void fill(uint8_t red, uint8_t green, uint8_t blue) = 0;
};

#endif