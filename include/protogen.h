#ifndef PROTOGEN_H
#define PROTOGEN_H

#include <memory>
#include <mutex>
#include <tuple>

#include <canvas.h>
#include <led-matrix.h>

#include <render.h>

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

	void viewRender(const render::Render& render) {
		std::lock_guard<std::mutex> lock(m_mutex);
		auto* frame = getNextProtogenFrameBuffer();
		for(std::size_t y = 0; y < render.height(); ++y) {
			for(std::size_t x = 0; x < render.width(); ++x) {
				const auto color = render.get(x, y);
				frame->SetPixel(x, y, std::get<0>(color), std::get<1>(color), std::get<2>(color));
			}
		}
		m_matrix->SwapOnVSync(frame);
	}

	~ProtogenHeadMatrices() {
		m_matrix->Clear();
	}

	rgb_matrix::FrameCanvas * getNextProtogenFrameBuffer() {
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

private:
	std::unique_ptr<rgb_matrix::RGBMatrix> m_matrix;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer0;
	rgb_matrix::FrameCanvas * m_protogenFrameBuffer1;
	unsigned int m_whichProtogenFrameBufferIsUsed;
	mutable std::mutex m_mutex;
};

#endif
