#pragma once

#include <hpkmedoids/types/parallelism.hpp>
#include <matrix/matrix.hpp>
#include <type_traits>

namespace hpkmedoids
{
template <typename T, Parallelism Level, class DistanceFunc>
class DistanceCalculator
{
public:
    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI, Matrix<T>> calculateDistanceMatrix(
      const Matrix<T>* const mat1, const Matrix<T>* const mat2) const
    {
        Matrix<T> distanceMat(mat1->rows(), mat2->rows(), true, std::numeric_limits<T>::max());

        for (int i = 0; i < mat1->rows(); ++i)
        {
            for (int j = 0; j < mat2->numRows(); ++j)
            {
                distanceMat.at(i, j) =
                  m_distanceFunc(mat1->crowBegin(i), mat1->crowEnd(i), mat2->crowBegin(j), mat2->crowEnd(j));
            }
        }

        return distanceMat;
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid, Matrix<T>> calculateDistanceMatrix(
      const Matrix<T>* const mat1, const Matrix<T>* const mat2) const
    {
        Matrix<T> distanceMat(mat1->rows(), mat2->rows(), true, std::numeric_limits<T>::max());

#pragma omp parallel for shared(distanceMat), schedule(static)
        for (int i = 0; i < mat1->rows(); ++i)
        {
            for (int j = 0; j < mat2->numRows(); ++j)
            {
                distanceMat.at(i, j) =
                  m_distanceFunc(mat1->crowBegin(i), mat1->crowEnd(i), mat2->crowBegin(j), mat2->crowEnd(j));
            }
        }

        return distanceMat;
    }

private:
    DistanceFunc m_distanceFunc;
};
}  // namespace hpkmedoids