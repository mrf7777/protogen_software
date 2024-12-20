#ifndef IRESOLUTIONSELECTOR_H
#define IRESOLUTIONSELECTOR_H

#include <optional>

#include <protogen/Resolution.hpp>

namespace protogen {

class IResolutionSelector {
public:
    virtual ~IResolutionSelector() = default;
    virtual std::optional<Resolution> selectResolution(const Resolution& device_res, const std::vector<Resolution>& available_res) const = 0;
};

} // namespace

#endif