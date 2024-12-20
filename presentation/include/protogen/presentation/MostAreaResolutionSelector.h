#ifndef MOSTARERESOLUTIONSELECTOR_H
#define MOSTARERESOLUTIONSELECTOR_H

#include <vector>

#include <protogen/presentation/IResolutionSelector.h>

namespace protogen {

class MostAreaResolutionSelector : public IResolutionSelector {
public:
    MostAreaResolutionSelector();
    std::optional<Resolution> selectResolution(const Resolution& device_res, const std::vector<Resolution>& available_res) const override;
};

} // namespace

#endif