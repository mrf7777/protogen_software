#ifndef SSE_H
#define SSE_H

#include <iostream>
#include <mutex>
#include <sstream>
#include <optional>
#include <string>

#include <httplib.h>

#include "utils.h"

// Server-Sent Events server implementation.
// For usage, see GitHub project "cpp httplib" and look for the server-sent events example in the README docs.

namespace sse {

class Event final {
public:
	static std::optional<Event> make_event(const std::string& event, const std::string& data) {
		if(event.find("\n") != std::string::npos || data.find("\n") != std::string::npos) {
			return {};
		}
		Event e;
		e.m_event = event;
		e.m_data = data;
		return {e};
	}

	std::string to_sse_string() const {
		std::stringstream ss;
		ss << "event: " << m_event << "\n";
		ss << "data: " << m_data << "\n\n";
		return ss.str();
	}
private:
	std::string m_event;
	std::string m_data;
};

/**
 * Directly from the httplib docs example.
 */
class EventDispatcher final {
public:
	EventDispatcher() : m_id{0}, m_cid{-1}
	{}
	void wait_event(httplib::DataSink* sink) {
		std::unique_lock<std::mutex> lock(m_mutex);
		int id = m_id;
		m_cv.wait(lock, [&]{ return m_cid == m_id; });
		sink->write(m_message.data(), m_message.size());
		std::cout << "written data to sink from event dispatcher" << std::endl;
	}
	void send_event(const Event& event) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_cid = m_id++;
		m_message = event.to_sse_string();
		m_cv.notify_all();
	}
private:
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::atomic_int m_id;
	std::atomic_int m_cid;
	std::string m_message;
};


}

#endif
