#ifndef PROTOGEN_H
#define PROTOGEN_H

#include <stdexcept>
#include <memory>
#include <optional>
#include <mutex>
#include <tuple>
#include <functional>

#include <protogen/presentation/render_surface.h>
#include <protogen/ICanvas.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/IAttributeStore.hpp>

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
	ProtogenHeadMatrices();
	~ProtogenHeadMatrices();

	Initialization initialize() override;
	void drawFrame(const std::function<void(ICanvas&)>& drawer) override;
	Resolution resolution() const override;
	std::optional<std::string> getAttribute(const std::string& key) const override;
	std::vector<std::string> listAttributes() const override;
	SetAttributeResult setAttribute(const std::string& key, const std::string& value) override;
	RemoveAttributeResult removeAttribute(const std::string& key) override;
	bool hasAttribute(const std::string& key) const override;
	
private:
	class ConstructorException : public std::runtime_error {
	public:
		ConstructorException(const char * what) : std::runtime_error(what) {}
	};
	
	rgb_matrix::FrameCanvas * getNextProtogenFrameBuffer();

	std::unique_ptr<rgb_matrix::RGBMatrix> m_matrix;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer0;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer1;
	unsigned int m_whichProtogenFrameBufferIsUsed;
	mutable std::mutex m_mutex;

	std::shared_ptr<attributes::IAttributeStore> m_attributes;
};

}	// namespace

#endif
