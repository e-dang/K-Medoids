#pragma once

#include <hpkmedoids/kmedoids/clara_impl.hpp>

namespace hpkmedoids
{
template <typename T, Parallelism Level = Parallelism::Serial, class DistanceFunc = L1Norm<T>>
class CLARAKMedoids
{
public:
    typedef typename std::conditional<Level == Parallelism::Serial || Level == Parallelism::OMP,
                                      SharedMemoryCLARAKMedoids<T, Level, DistanceFunc>,
                                      DistributedCLARAKMedoids<T, Level, DistanceFunc>>::type impl_type;

    CLARAKMedoids(const std::string& initializer, const std::string& maximizer,
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
}  // namespace hpkmedoids