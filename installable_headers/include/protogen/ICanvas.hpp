#ifndef PROTOGEN_ICANVAS_H
#define PROTOGEN_ICANVAS_H

#include <cstdint>
#include <vector>
#include <algorithm>

namespace protogen {

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
    virtual void clear() {
        fill(0, 0, 0);
    };
    /**
     * Sets entire canvas to provided color.
     */
    virtual void fill(uint8_t red, uint8_t green, uint8_t blue) {
        fillRegion(0, 0, width(), height(), red, green, blue);
    };
    /**
     * Fills a region with provided color.
     * If the region is out of bounds, it is clipped.
     */
    virtual void fillRegion(int x, int y, int width, int height, uint8_t red, uint8_t green, uint8_t blue) {
        if (x < 0) {
            width += x;
            x = 0;
        }
        if (y < 0) {
            height += y;
            y = 0;
        }
        if (width <= 0 || height <= 0) {
            return;
        }
        if (x + width > this->width()) {
            width = this->width() - x;
        }
        if (y + height > this->height()) {
            height = this->height() - y;
        }

        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < height; ++j) {
                setPixel(x + i, y + j, red, green, blue);
            }
        }
    };
    /**
     * Draws a line between two points with provided color.
     */
    virtual void drawLine(int x1, int y1, int x2, int y2, uint8_t red, uint8_t green, uint8_t blue) {
        int dx = x2 - x1;
        int dy = y2 - y1;
        const int abs_dx = dx < 0 ? -dx : dx;
        const int abs_dy = dy < 0 ? -dy : dy;
        int steps = abs_dx > abs_dy ? abs_dx : abs_dy;
        float xInc = dx / (float)steps;
        float yInc = dy / (float)steps;
        float x = x1;
        float y = y1;
        for (int i = 0; i <= steps; ++i) {
            setPixel(x, y, red, green, blue);
            x += xInc;
            y += yInc;
        }
    };
    /**
     * Draws a polygon with the provided color.
     * If the polygon has less than 2 points, nothing is drawn.
     * If fill is true, the polygon is filled with the color.
     * If fill is false, only the outline is drawn.
     */
    virtual void drawPolygon(const std::vector<std::pair<int, int>>& points, uint8_t red, uint8_t green, uint8_t blue, bool fill) {
        if (points.size() < 2) {
            return;
        }
        for (size_t i = 0; i < points.size() - 1; ++i) {
            drawLine(points.at(i).first, points.at(i).second, points.at(i + 1).first, points.at(i + 1).second, red, green, blue);
        }
        drawLine(points.at(points.size() - 1).first, points.at(points.size() - 1).second, points.at(0).first, points.at(0).second, red, green, blue);

        if (fill) {
            // Filling the polygon using scanline algorithm
            int minY = points.at(0).second, maxY = points.at(0).second;
            for (const auto& point : points) {
                if (point.second < minY) minY = point.second;
                if (point.second > maxY) maxY = point.second;
            }

            for (int y = minY; y <= maxY; ++y) {
                std::vector<int> nodes;
                size_t j = points.size() - 1;
                for (size_t i = 0; i < points.size(); ++i) {
                    if ((points.at(i).second < y && points.at(j).second >= y) || (points.at(j).second < y && points.at(i).second >= y)) {
                        nodes.push_back(points.at(i).first + (y - points.at(i).second) * (points.at(j).first - points.at(i).first) / (points.at(j).second - points.at(i).second));
                    }
                    j = i;
                }
                std::sort(nodes.begin(), nodes.end());
                for (size_t i = 0; i < nodes.size(); i += 2) {
                    if (i + 1 < nodes.size()) {
                        drawLine(nodes.at(i), y, nodes.at(i + 1), y, red, green, blue);
                    }
                }
            }
        }
    };

    /**
     * Draws an ellipse with the provided color.
     *
     * @param x The x-coordinate of the top-left corner of the bounding box of the ellipse.
     * @param y The y-coordinate of the top-left corner of the bounding box of the ellipse.
     * @param width The width of the ellipse.
     * @param height The height of the ellipse.
     * @param red The red component of the color (0-255).
     * @param green The green component of the color (0-255).
     * @param blue The blue component of the color (0-255).
     * @param fill If true, the ellipse will be filled with the color. If false, only the outline will be drawn.
     *
     * The (x, y) coordinates specify the top-left corner of the bounding box that contains the ellipse.
     * The center of the ellipse is calculated as (x + width / 2, y + height / 2).
     */
    virtual void drawEllipse(int x, int y, int width, int height, uint8_t red, uint8_t green, uint8_t blue, bool fill) {
        // This implementation uses the midpoint ellipse algorithm to draw the ellipse.
        int a = width / 2;
        int b = height / 2;
        int a2 = a * a;
        int b2 = b * b;
        int fa2 = 4 * a2;
        int fb2 = 4 * b2;
        int x0 = x + a;
        int y0 = y + b;
        int x1 = 0;
        int y1 = b;
        int sigma = 2 * b2 + a2 * (1 - 2 * b);

        // Draw the outline of the ellipse
        while (b2 * x1 <= a2 * y1) {
            setPixel(x0 + x1, y0 + y1, red, green, blue);
            setPixel(x0 - x1, y0 + y1, red, green, blue);
            setPixel(x0 + x1, y0 - y1, red, green, blue);
            setPixel(x0 - x1, y0 - y1, red, green, blue);
            if (fill) {
                for (int i = x0 - x1; i <= x0 + x1; ++i) {
                    setPixel(i, y0 + y1, red, green, blue);
                    setPixel(i, y0 - y1, red, green, blue);
                }
            }
            if (sigma >= 0) {
                sigma += fa2 * (1 - y1);
                y1--;
            }
            sigma += b2 * ((4 * x1) + 6);
            x1++;
        }

        x1 = a;
        y1 = 0;
        sigma = 2 * a2 + b2 * (1 - 2 * a);

        // Draw the outline of the ellipse
        while (a2 * y1 <= b2 * x1) {
            setPixel(x0 + x1, y0 + y1, red, green, blue);
            setPixel(x0 - x1, y0 + y1, red, green, blue);
            setPixel(x0 + x1, y0 - y1, red, green, blue);
            setPixel(x0 - x1, y0 - y1, red, green, blue);
            if (fill) {
                for (int i = x0 - x1; i <= x0 + x1; ++i) {
                    setPixel(i, y0 + y1, red, green, blue);
                    setPixel(i, y0 - y1, red, green, blue);
                }
            }
            if (sigma >= 0) {
                sigma += fb2 * (1 - x1);
                x1--;
            }
            sigma += a2 * ((4 * y1) + 6);
            y1++;
        }
    }
};

}   // namespace

#endif