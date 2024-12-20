#include <protogen/presentation/MostAreaResolutionSelector.h>

#include <limits>

namespace protogen {

MostAreaResolutionSelector::MostAreaResolutionSelector()
{}

std::optional<Resolution> MostAreaResolutionSelector::selectResolution(const Resolution& device_res, const std::vector<Resolution>& available_res) const
{
    std::optional<Resolution> best_res = {};
    double best_res_score = std::numeric_limits<double>::min();
    for(const auto& r : available_res) {
        if(r.fitsIn(device_res)) {
            const double score = r.width() * r.height();
            if(score > best_res_score) {
                best_res = r;
                best_res_score = score;
            }
        }
    }
    return best_res;
}

} // namespace