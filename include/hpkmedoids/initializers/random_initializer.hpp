#pragma once

#include <hpkmedoids/initializers/interface.hpp>
#include <hpkmedoids/utils/uniform_selectors.hpp>

namespace hpkmedoids
{
constexpr char RANDOM_INIT[] = "random";

template <typename T>
class RandomInitializer : public IInitializer<T>
{
public:
    void initialize(const Matrix<T>* const data, Clusters<T>* const clusters,
                    const DistanceMatrix<T>* const distMat) const override
    {
        auto selections = m_selector.select(clusters->maxSize(), data->rows());
        for (const auto& selection : selections)
        {
            clusters->addCentroid(selection);
        }
    }

private:
    UniformSelector m_selector;
};
}  // namespace hpkmedoids