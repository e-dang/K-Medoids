#include <boost/generator_iterator.hpp>
#include <boost/random.hpp>
#include <chrono>
#include <hpkmediods/utils/uniform_selectors.hpp>

namespace hpkmediods
{
typedef boost::random::mt19937 RNGType;

AbstractUniformSelector::AbstractUniformSelector(const int64_t* seed, const int min) : m_seed(0), m_min(min)
{
    if (seed == nullptr)
        m_seed =
          std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    else
        m_seed = *seed;
}

UniformSelector::UniformSelector(const int64_t* seed, const int min) : AbstractUniformSelector(seed, min) {}

std::set<int32_t> UniformSelector::select(const int sampleSize, const int32_t containerSize) const
{
    static RNGType rng(m_seed);
    static boost::random::uniform_int_distribution<> dist(m_min, containerSize - 1);
    static boost::variate_generator<RNGType, boost::random::uniform_int_distribution<>> gen(rng, dist);

    std::set<int32_t> selections;
    while (static_cast<int>(selections.size()) < sampleSize)
    {
        selections.insert(gen());
    }

    return selections;
}
}  // namespace hpkmediods