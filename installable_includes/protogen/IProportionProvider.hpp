#ifndef IPROPORTIONPROVIDER_H
#define IPROPORTIONPROVIDER_H

#include <protogen/Proportion.hpp>

namespace protogen {

class IProportionProvider {
public:
	virtual ~IProportionProvider() = default;
	virtual Proportion proportion() const = 0;
};

}	// namespace

#endif