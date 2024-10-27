#ifndef PROTOGEN_H
#define PROTOGEN_H

#include <stdexcept>
#include <memory>
#include <optional>
#include <mutex>
#include <tuple>
#include <functional>

#include <render_surface.h>

#include <led-matrix.h>

class ProtogenHeadMatrices final : public IRenderSurface {
public:
	static std::optional<std::unique_ptr<ProtogenHeadMatrices>> make();
	~ProtogenHeadMatrices();

	void drawFrame(const std::function<void(rgb_matrix::Canvas&)>& drawer) override;
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

#endif
