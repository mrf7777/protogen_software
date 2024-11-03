#ifndef PROTOGEN_H
#define PROTOGEN_H

#include <stdexcept>
#include <memory>
#include <optional>
#include <mutex>
#include <tuple>
#include <functional>

#include <render_surface.h>
#include <protogen/ICanvas.hpp>

#include <led-matrix.h>

namespace protogen {

/**
 * Adapter for rgb_matrix::Canvas to ICanvas.
 * Does NOT take ownership of rgb_matrix::Canvas.
 */
class RgbMatrixCanvasToICanvasAdapter : public ICanvas {
public:
	RgbMatrixCanvasToICanvasAdapter(rgb_matrix::Canvas * canvas);
	int width() const override;
	int height() const override;
	void setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) override;
	void clear() override;
	void fill(uint8_t red, uint8_t green, uint8_t blue) override;
private:
	rgb_matrix::Canvas * mCanvas;
};

class ProtogenHeadMatrices final : public IRenderSurface {
public:
	static std::optional<std::unique_ptr<ProtogenHeadMatrices>> make();
	~ProtogenHeadMatrices();

	void drawFrame(const std::function<void(ICanvas&)>& drawer) override;
private:
	class ConstructorException : public std::exception {
	public:
		ConstructorException() : std::exception() {}
	};
	ProtogenHeadMatrices();
	rgb_matrix::FrameCanvas * getNextProtogenFrameBuffer();

	std::unique_ptr<rgb_matrix::RGBMatrix> m_matrix;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer0;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer1;
	unsigned int m_whichProtogenFrameBufferIsUsed;
	mutable std::mutex m_mutex;
};

}	// namespace

#endif
