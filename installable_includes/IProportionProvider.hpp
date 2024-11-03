#ifndef IPROPORTIONPROVIDER_H
#define IPROPORTIONPROVIDER_H

#include <Proportion.hpp>

namespace audio {

class IProportionProvider {
public:
	virtual ~IProportionProvider() = default;
	virtual Proportion proportion() const = 0;
};

}

#endif