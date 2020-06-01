#pragma once

#include <hpkmediods/utils/uniform_selectors.hpp>
#include <matrix/matrix.hpp>
#include <type_traits>

namespace hpkmediods
{
template <typename T>
class Sampler
{
public:
    template <Parallelism Level>
    std::enable_if_t<Level == Parallelism::Serial || Level == Parallelism::MPI, Matrix<T>> sample(
      const int32_t sampleSize, const Matrix<T>* const data)
    {
        Matrix<T> sampledData(sampleSize, data->cols());

        auto selections = m_selector.select(sampleSize, data->rows());
        for (const auto& selection : selections)
        {
            sampledData.append(data->crowBegin(selection), data->crowEnd(selection));
        }

        return sampledData;
    }

    template <Parallelism Level>
    std::enable_if_t<Level == Parallelism::OMP || Level == Parallelism::Hybrid, Matrix<T>> sample(
      const int32_t sampleSize, const Matrix<T>* const data)
    {
        Matrix<T> sampledData(sampleSize, data->cols(), true);

        auto selections = m_selector.select(sampleSize, data->rows());
        std::vector<int32_t> selectionVec(selections.cbegin(), selections.cend());

#pragma omp parallel for shared(sampledData, selectionVec, selections), schedule(static)
        for (int i = 0; i < static_cast<int>(selectionVec.size()); ++i)
        {
            sampledData.set(i, data->crowBegin(selectionVec[i]), data->crowEnd(selectionVec[i]));
        }

        return sampledData;
    }

private:
    UniformSelector m_selector;
};
}  // namespace hpkmediods