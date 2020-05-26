#pragma once

#include <hpkmediods/distances.hpp>
#include <hpkmediods/initializers/initializers.hpp>
#include <hpkmediods/maximizers/maximizers.hpp>
#include <hpkmediods/types/cluster_data.hpp>
#include <hpkmediods/types/parallelism.hpp>
#include <string>

namespace hpkmediods
{
template <typename T, Parallelism Level = Parallelism::Serial, class DistanceFunc = L2Norm<T>>
class KMediods
{
public:
    KMediods(const std::string& initializer, const std::string& maximizer) :
        p_initializer(createInitializer<T, Level, DistanceFunc>(initializer)),
        p_maximizer(createMaximizer<T, Level, DistanceFunc>(maximizer))
    {
    }

    const ClusterData<T>* const fit(const Matrix<T>* const data, const int& numClusters, const int& numRepeats)
    {
        for (int i = 0; i < numRepeats; ++i)
        {
            ClusterData<T> clusterData(data, numClusters);
            clusterData.initialize(p_initializer.get());
            std::cout << "initialized\n";
            clusterData.maximize(p_maximizer.get());
            std::cout << "maximized\n";
            compareResults(clusterData);
        }

        return getResults();
    }

    const ClusterData<T>* const getResults() { return &m_bestClustering; }

private:
    void compareResults(const ClusterData<T>& clusterData)
    {
        if (clusterData < m_bestClustering)
        {
            m_bestClustering = clusterData;
        }
    }

private:
    std::shared_ptr<IInitializer<T>> p_initializer;
    std::shared_ptr<IMaximizer<T>> p_maximizer;
    ClusterData<T> m_bestClustering;
};
}  // namespace hpkmediods