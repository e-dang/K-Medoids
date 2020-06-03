#pragma once

#include <fstream>
#include <iostream>
#include <matrix/matrix.hpp>
#include <string>

namespace hpkmediods
{
template <typename T>
class IReader
{
public:
    virtual ~IReader() = default;

    virtual Matrix<T> read(const std::string& filepath, const int32_t& numData, const int32_t& numFeatures) = 0;
};

template <typename T>
class MatrixReader : public IReader<T>
{
public:
    std::ifstream openFile(const std::string& filepath);

    Matrix<T> read(const std::string& filepath, const int32_t& numData, const int32_t& numFeatures) override;
};

template <typename T>
std::ifstream MatrixReader<T>::openFile(const std::string& filepath)
{
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file: " << filepath << std::endl;
        exit(1);
    }

    return file;
}

template <typename T>
Matrix<T> MatrixReader<T>::read(const std::string& filepath, const int32_t& numData, const int32_t& numFeatures)
{
    auto file = openFile(filepath);

    Matrix<T> data(numData, numFeatures, true);
    file.read(reinterpret_cast<char*>(data.data()), sizeof(T) * numData * numFeatures);
    file.close();

    return data;
}
}  // namespace hpkmediods