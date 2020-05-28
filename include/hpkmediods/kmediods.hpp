#pragma once

#include <functional>
#include <hpkmediods/distances.hpp>
#include <hpkmediods/initializers/initializers.hpp>
#include <hpkmediods/maximizers/maximizers.hpp>
#include <hpkmediods/utils/uniform_selectors.hpp>
#include <string>

namespace hpkmediods
{
template <typename T, Parallelism Level = Parallelism::Serial, class DistanceFunc = L2Norm<T>>
class KMediods
{
public:
    KMediods(const std::string& initializer, const std::string& maximizer) :
        p_initializer(createInitializer<T, Level>(initializer)), p_maximizer(createMaximizer<T, Level>(maximizer))
    {
    }

    const Clusters<T>* const fit(const Matrix<T>* const data, const int& numClusters, const int& numRepeats)
    {
        auto distMat = DistanceMatrix<T>::template create<Level, DistanceFunc>(data, numClusters);

        for (int i = 0; i < numRepeats; ++i)
        {
            Clusters<T> clusters(data, &distMat);
            p_initializer->initialize(data, &clusters, &distMat);
            p_maximizer->maximize(data, &clusters, &distMat);
            compareResults(clusters, m_bestClusters);
        }

        return getResults();
    }

    const Clusters<T>* const getResults() { return &m_bestClusters; }

protected:
    void compareResults(const Clusters<T>& candidateClusters, Clusters<T>& bestClusters)
    {
        if (candidateClusters < bestClusters)
            bestClusters = std::move(candidateClusters);
    }

protected:
    Clusters<T> m_bestClusters;

private:
    std::unique_ptr<IInitializer<T>> p_initializer;
    std::unique_ptr<IMaximizer<T>> p_maximizer;
};

int32_t defaultSampleSize(const int32_t numData, const int32_t numClusters) { return 40 + 2 * numClusters; }

template <typename T, Parallelism Level = Parallelism::Serial, class DistanceFunc = L2Norm<T>>
class CLARAKMediods : public KMediods<T, Level, DistanceFunc>
{
public:
    CLARAKMediods(const std::string& initializer, const std::string& maximizer,
                  std::function<int32_t(const int32_t, const int32_t)> sampleSizeCalc = defaultSampleSize) :
        KMediods<T, Level, DistanceFunc>(initializer, maximizer), m_sampleSizeCalc(sampleSizeCalc)
    {
    }

    const Clusters<T>* const fit(const Matrix<T>* const data, const int& numClusters, const int& numRepeats,
                                 const int numSamplingIters)
    {
        auto sampleSize = m_sampleSizeCalc(data->rows(), numClusters);
        Matrix<T> sampledData(sampleSize, data->cols());
        for (int i = 0; i < numSamplingIters; ++i)
        {
            partitionData(data, &sampledData);
            KMediods<T, Level, DistanceFunc>::fit(&sampledData, numClusters, numRepeats);
            Clusters<T> clusters(data, this->m_bestClusters.getCentroids());
            clusters.template calculateAssignmentsFromCentroids<Level, DistanceFunc>(m_distanceFunc);
            this->compareResults(clusters, m_bestNonSampledClusters);
            sampledData.clear();
        }

        return getResults();
    }

    const Clusters<T>* const getResults() { return &m_bestNonSampledClusters; }

private:
    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI> partitionData(
      const Matrix<T>* const data, Matrix<T>* const sampledData)
    {
        auto selections = m_selector.select(sampledData->rows(), data->rows());
        for (const auto& selection : selections)
        {
            sampledData->append(data->crowBegin(selection), data->crowEnd(selection));
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid> partitionData(
      const Matrix<T>* const data, Matrix<T>* const sampledData)
    {
        sampledData->resize(sampledData->capacity());

        auto selections = m_selector.select(sampledData->rows(), data->rows());
        std::vector<int32_t> selectionVec(selections.cbegin(), selections.cend());

#pragma omp parallel for shared(data, sampledData, selectionVec), schedule(static)
        for (int i = 0; i < static_cast<int>(selectionVec.size()); ++i)
        {
            sampledData->set(selectionVec[i], data->crowBegin(selectionVec[i]), data->crowEnd(selectionVec[i]));
        }
    }

private:
    std::function<int32_t(const int32_t, const int32_t)> m_sampleSizeCalc;
    UniformSelector m_selector;
    Clusters<T> m_bestNonSampledClusters;
    DistanceFunc m_distanceFunc;
};
}  // namespace hpkmediods