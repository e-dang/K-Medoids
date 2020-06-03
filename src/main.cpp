#include <boost/timer/timer.hpp>
#include <hpkmediods/filesystem/reader.hpp>
#include <hpkmediods/filesystem/writer.hpp>
#include <hpkmediods/kmediods.hpp>
#include <random>

using namespace hpkmediods;
typedef double value_t;

constexpr bool strings_equal(char const* a, char const* b)
{
    return *a == *b && (*a == '\0' || strings_equal(a + 1, b + 1));
}

constexpr Parallelism getParallelism()
{
    if constexpr (strings_equal(PARALLELISM, "serial"))
        return Parallelism::Serial;
    else if constexpr (strings_equal(PARALLELISM, "omp"))
        return Parallelism::OMP;
    else if constexpr (strings_equal(PARALLELISM, "mpi"))
        return Parallelism::MPI;
    else
        return Parallelism::Hybrid;
}

constexpr char kmediodsMethod[]   = METHOD;
constexpr Parallelism parallelism = getParallelism();
constexpr int numData             = 20000;
constexpr int dims                = 2;
constexpr int numClusters         = 10;
constexpr int numIters            = 10;
constexpr int repeats             = 1;
constexpr int claraRepeats        = 10;

const Clusters<value_t>* calcClusters(KMediods<value_t, parallelism>* kmediods, const Matrix<value_t>* const data)
{
    const Clusters<value_t>* results;
    for (int i = 0; i < numIters; ++i)
    {
        kmediods->reset();
        results = kmediods->fit(data, numClusters, repeats);
    }

    return results;
}

const Clusters<value_t>* calcClusters(CLARAKMediods<value_t, parallelism>* kmediods, const Matrix<value_t>* const data)
{
    const Clusters<value_t>* results;
    for (int i = 0; i < numIters; ++i)
    {
        kmediods->reset();
        results = kmediods->fit(data, numClusters, repeats, claraRepeats);
    }

    return results;
}

void sharedMemory(std::string& filepath)
{
    MatrixReader<value_t> reader;
    ClusterResultWriter<value_t> writer(parallelism);
    std::conditional<strings_equal(kmediodsMethod, "REG"), KMediods<value_t, parallelism>,
                     CLARAKMediods<value_t, parallelism>>::type kmediods(PAM_INIT, PAM);
    const Clusters<value_t>* results;

    auto data = reader.read(filepath, numData, dims);
    {
        boost::timer::auto_cpu_timer t;
        results = calcClusters(&kmediods, &data);
    }

    std::cout << "Error: " << results->getError() << "\n";
    // writer.writeClusterResults(results, 0, filepath);
}

void distributed(std::string& filepath)
{
    int rank;
    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MatrixReader<value_t> reader;
    ClusterResultWriter<value_t> writer(parallelism);
    std::conditional<strings_equal(kmediodsMethod, "REG"), KMediods<value_t, parallelism>,
                     CLARAKMediods<value_t, parallelism>>::type kmediods(PAM_INIT, PAM);
    const Clusters<value_t>* results;

    Matrix<value_t> data;
    if (rank == 0)
        data = reader.read(filepath, numData, dims);

    {
        boost::timer::auto_cpu_timer t;
        results = calcClusters(&kmediods, &data);
    }

    if (rank == 0)
    {
        std::cout << "Error: " << results->getError() << "\n";
        writer.writeClusterResults(results, 0, filepath);
    }

    MPI_Finalize();
}

int main(int argc, char* argv[])
{
    std::string filepath = "/Users/ericdang/Documents/High_Performance_Computing_Fall_2019/K-Mediods/data/test_" +
                           std::to_string(numData) + "_" + std::to_string(dims) + ".txt";

    std::cout << "Method: " << kmediodsMethod << "\nParallelism: " << parallelismToString(parallelism)
              << "\nData: " << filepath << '\n';
    if (parallelism == Parallelism::Serial || parallelism == Parallelism::OMP)
        sharedMemory(filepath);
    else
        distributed(filepath);
}