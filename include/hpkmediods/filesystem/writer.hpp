#pragma once

#include <fstream>
#include <hpkmediods/filesystem/file_rotator.hpp>
#include <hpkmediods/types/clusters.hpp>
#include <hpkmediods/types/parallelism.hpp>
#include <iomanip>
#include <matrix/matrix.hpp>
#include <string>

namespace hpkmediods
{
template <typename T>
class AbstractWriter
{
protected:
    int m_digits;
    FileRotator m_fileRotator;
    std::vector<std::string> m_runParams;

public:
    AbstractWriter(Parallelism parallelism, const int digits = 8);

    virtual ~AbstractWriter() = default;

    void writeClusterResults(const Clusters<T>* clusterResults, const int_fast64_t& time, std::string& filepath);

    void writeRunStats(const T& error, const int_fast64_t& time, std::string& filepath);

    void writeError(const T& error, std::ofstream& file);

    void writeTime(const T& time, std::ofstream& file);

    void writeRunParams(std::ofstream& file);

    virtual void writeClusters(const Matrix<T>* clusters, std::string& filepath) = 0;

    virtual void writeClustering(const std::vector<int32_t>* clustering, std::string& filepath) = 0;

protected:
    std::ofstream openFile(const std::string& filepath, const std::ios::openmode mode);
};

template <typename T>
class ClusterResultWriter : public AbstractWriter<T>
{
public:
    ClusterResultWriter(Parallelism parallelism) : AbstractWriter<T>(parallelism) {}

    ~ClusterResultWriter() = default;

    void writeClusters(const Matrix<T>* clusters, std::string& filepath) override;

    void writeClustering(const std::vector<int32_t>* clustering, std::string& filepath) override;
};

template <typename T>
AbstractWriter<T>::AbstractWriter(Parallelism parallelism, const int digits) : m_digits(digits)
{
    m_runParams.push_back(parallelismToString(parallelism));
}

template <typename T>
void AbstractWriter<T>::writeClusterResults(const Clusters<T>* clusterResults, const int_fast64_t& time,
                                            std::string& filepath)
{
    writeClusters(clusterResults->getCentroids(), filepath);
    writeClustering(clusterResults->getClustering(), filepath);
    writeRunStats(clusterResults->getError(), time, filepath);
}

template <typename T>
void AbstractWriter<T>::writeRunStats(const T& error, const int_fast64_t& time, std::string& filepath)
{
    auto file = this->openFile(this->m_fileRotator.getUniqueFileName(filepath, "stats"), std::ios::out);

    writeError(error, file);
    writeTime(time, file);
    writeRunParams(file);

    file.close();
}

template <typename T>
void AbstractWriter<T>::writeError(const T& error, std::ofstream& file)
{
    auto ss = file.precision();
    file << std::fixed << std::setprecision(m_digits) << "Error: " << error << std::endl;
    file << std::defaultfloat << std::setprecision(ss);
}

template <typename T>
void AbstractWriter<T>::writeTime(const T& time, std::ofstream& file)
{
    file << "Time: " << time << std::endl;
}

template <typename T>
void AbstractWriter<T>::writeRunParams(std::ofstream& file)
{
    for (auto& val : m_runParams)
    {
        file << val << std::endl;
    }
}

template <typename T>
std::ofstream AbstractWriter<T>::openFile(const std::string& filepath, const std::ios::openmode mode)
{
    std::ofstream file(filepath, mode);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file: " << filepath << std::endl;
        exit(1);
    }

    return file;
}

template <typename T>
void ClusterResultWriter<T>::writeClusters(const Matrix<T>* clusters, std::string& filepath)
{
    auto file = this->openFile(this->m_fileRotator.getUniqueFileName(filepath, "clusters"), std::ios::binary);
    file.write(clusters->serialize(), clusters->bytes());
    file.close();
}

template <typename T>
void ClusterResultWriter<T>::writeClustering(const std::vector<int32_t>* clustering, std::string& filepath)
{
    auto file = this->openFile(this->m_fileRotator.getUniqueFileName(filepath, "clustering"), std::ios::binary);
    file.write(reinterpret_cast<const char*>(clustering->data()), sizeof(int32_t) * clustering->size());
    file.close();
}
}  // namespace hpkmediods