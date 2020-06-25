#include <boost/timer/timer.hpp>
#include <hpkmedoids/filesystem/reader.hpp>
#include <hpkmedoids/filesystem/writer.hpp>
#include <hpkmedoids/kmedoids.hpp>
#include <random>

using namespace hpkmedoids;
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

constexpr char kmedoidsMethod[]   = METHOD;
constexpr Parallelism parallelism = getParallelism();
constexpr int numData             = 10000;
constexpr int dims                = 10;
constexpr int numClusters         = 10;
constexpr int numIters            = 10;
constexpr int repeats             = 1;
constexpr int claraRepeats        = 10;
int64_t runTime;

const Clusters<value_t>* calcClusters(KMedoids<value_t, parallelism>* kmedoids, const Matrix<value_t>* const data)
{
    const Clusters<value_t>* results;
    for (int i = 0; i < numIters; ++i)
    {
        kmedoids->reset();
        boost::timer::auto_cpu_timer t;
        results = kmedoids->fit(data, numClusters, repeats);
        runTime += t.elapsed().wall;
    }

    return results;
}

const Clusters<value_t>* calcClusters(CLARAKMedoids<value_t, parallelism>* kmedoids, const Matrix<value_t>* const data)
{
    const Clusters<value_t>* results;
    for (int i = 0; i < numIters; ++i)
    {
        kmedoids->reset();
        boost::timer::auto_cpu_timer t;
        results = kmedoids->fit(data, numClusters, repeats, claraRepeats);
        runTime += t.elapsed().wall;
    }

    return results;
}

void sharedMemory(std::string& filepath)
{
    MatrixReader<value_t> reader;
    ClusterResultWriter<value_t> writer(parallelism);
    std::conditional<strings_equal(kmedoidsMethod, "REG"), KMedoids<value_t, parallelism>,
                     CLARAKMedoids<value_t, parallelism>>::type kmedoids(PAM_INIT, PAM);
    const Clusters<value_t>* results;

    auto data = reader.read(filepath, numData, dims);

    results = calcClusters(&kmedoids, &data);

    std::cout << "Error: " << results->getError() << "\n";
    writer.writeClusterResults(results, runTime, filepath);
}

void distributed(std::string& filepath)
{
    int rank;
    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MatrixReader<value_t> reader;
    ClusterResultWriter<value_t> writer(parallelism);
    std::conditional<strings_equal(kmedoidsMethod, "REG"), KMedoids<value_t, parallelism>,
                     CLARAKMedoids<value_t, parallelism>>::type kmedoids(PAM_INIT, PAM);
    const Clusters<value_t>* results;

    Matrix<value_t> data;
    if (rank == 0)
        data = reader.read(filepath, numData, dims);

    results = calcClusters(&kmedoids, &data);

    if (rank == 0)
    {
        std::cout << "Error: " << results->getError() << "\n";
        writer.writeClusterResults(results, runTime, filepath);
    }

    MPI_Finalize();
}

int main(int argc, char* argv[])
{
    std::string filepath = /* INSERT PATH HERE */ std::to_string(numData) + "_" + std::to_string(dims) + "_" +
                           std::to_string(numClusters) + ".txt";

    std::cout << "Method: " << kmedoidsMethod << "\nParallelism: " << parallelismToString(parallelism)
              << "\nData: " << filepath << '\n';
    if (parallelism == Parallelism::Serial || parallelism == Parallelism::OMP)
        sharedMemory(filepath);
    else
        distributed(filepath);
}