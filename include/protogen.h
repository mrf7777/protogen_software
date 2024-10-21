#ifndef PROTOGEN_H
#define PROTOGEN_H

#include <memory>
#include <mutex>
#include <tuple>
#include <functional>

#include <canvas.h>
#include <led-matrix.h>

class ProtogenHeadMatrices final {
public:
	ProtogenHeadMatrices();
	~ProtogenHeadMatrices();

	void drawFrame(const std::function<void(rgb_matrix::Canvas&)>& drawer);
private:
	rgb_matrix::FrameCanvas * getNextProtogenFrameBuffer();

	std::unique_ptr<rgb_matrix::RGBMatrix> m_matrix;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer0;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer1;
	unsigned int m_whichProtogenFrameBufferIsUsed;
	mutable std::mutex m_mutex;
};

#endif
