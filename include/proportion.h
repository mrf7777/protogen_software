#ifndef PROPORTION_H
#define PROPORTION_H

#include <optional>

/**
 * A floating value between in the closed interval [0, 1].
 */
class Proportion {
public:
	static std::optional<Proportion> make(double x) {
		if(0 <= x && x <= 1) {
			return {Proportion(x)};
		} else {
			return {};
		}
	}

	operator double() const { return m_value; }
	operator float() const { return m_value; }
private:
	Proportion(double x) : m_value{x} {}
	double m_value;
};


#endif