#ifndef PROTOGEN_H
#define PROTOGEN_H

#include <memory>
#include <mutex>
#include <tuple>

#include <canvas.h>
#include <led-matrix.h>

class ProtogenHeadMatrices final{
public:
	ProtogenHeadMatrices()
		: m_whichProtogenFrameBufferIsUsed(0)
	{
		rgb_matrix::RGBMatrix::Options options;
		options.rows = 32;
		options.cols = 64;
		options.chain_length = 2;
		options.brightness = 100;
		options.hardware_mapping = "adafruit-hat";
		options.led_rgb_sequence = "RBG";

		rgb_matrix::RuntimeOptions runtime_opts;

		m_matrix = std::unique_ptr<rgb_matrix::RGBMatrix>(
			rgb_matrix::RGBMatrix::CreateFromOptions(
				options, runtime_opts
			)
		);
		m_matrix->Clear();

		m_protogenFrameBuffer0 = m_matrix->CreateFrameCanvas();
		m_protogenFrameBuffer1 = m_matrix->CreateFrameCanvas();
	};

	~ProtogenHeadMatrices() {
		m_matrix->Clear();
	}

	rgb_matrix::FrameCanvas * getNextProtogenFrameBuffer() {
		std::lock_guard<std::mutex> lock(m_mutex);
		switch(m_whichProtogenFrameBufferIsUsed) {
		case 0:
			m_whichProtogenFrameBufferIsUsed = 1;
			return m_protogenFrameBuffer1;
		case 1:
		default:
			m_whichProtogenFrameBufferIsUsed = 0;
			return m_protogenFrameBuffer0;
		}
	}

	void drawFrame(const std::function<void(rgb_matrix::Canvas&)>& drawer) {
		std::lock_guard<std::mutex> lock(m_mutex);
		auto frame = getNextProtogenFrameBuffer();
		drawer(*frame);
		m_matrix->SwapOnVSync(frame);
	}

private:
	std::unique_ptr<rgb_matrix::RGBMatrix> m_matrix;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer0;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer1;
	unsigned int m_whichProtogenFrameBufferIsUsed;
	mutable std::mutex m_mutex;
};

#endif
