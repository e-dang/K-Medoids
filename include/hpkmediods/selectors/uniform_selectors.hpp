#pragma once

#include <set>

namespace hpkmediods
{
class AbstractUniformSelector
{
public:
    AbstractUniformSelector(const int64_t* seed = nullptr, const int min = 0);

    virtual std::set<int32_t> select(const int sampleSize, const int32_t containerSize) const = 0;

protected:
    int64_t m_seed;
    int m_min;
};

class UniformSelector : public AbstractUniformSelector
{
public:
    UniformSelector(const int64_t* seed = nullptr, const int min = 0);

    std::set<int32_t> select(const int sampleSize, const int32_t containerSize) const override;
};
}  // namespace hpkmediods