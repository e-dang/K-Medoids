#pragma once

#include <hpkmediods/kmediods/clara_impl.hpp>

namespace hpkmediods
{
template <typename T, Parallelism Level = Parallelism::Serial, class DistanceFunc = L1Norm<T>>
class CLARAKMediods
{
public:
    typedef typename std::conditional<Level == Parallelism::Serial || Level == Parallelism::OMP,
                                      SharedMemoryCLARAKMediods<T, Level, DistanceFunc>,
                                      DistributedCLARAKMediods<T, Level, DistanceFunc>>::type impl_type;

    CLARAKMediods(const std::string& initializer, const std::string& maximizer,
                  std::function<int32_t(const int32_t, const int32_t)> sampleSizeCalc = defaultSampleSize) :
        p_impl(std::make_unique<impl_type>(initializer, maximizer, sampleSizeCalc))
    {
    }

    const Clusters<T>* const fit(const Matrix<T>* const data, const int& numClusters, const int& numRepeats,
                                 const int numSamplingIters)
    {
        return p_impl->fit(data, numClusters, numRepeats, numSamplingIters);
    }

    const Clusters<T>* const getResults() { return p_impl->getResults(); }

    void reset() { p_impl->reset(); }

private:
    std::unique_ptr<impl_type> p_impl;
};
}  // namespace hpkmediods