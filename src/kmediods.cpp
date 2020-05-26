#include <hpkmediods/kmediods.hpp>

namespace hpkmediods
{
template <typename T, Parallelism Level, class DistanceFunc>
KMediods<T, Level, DistanceFunc>::KMediods(const std::string& initializer, const std::string& maximizer) :
    p_initializer(createInitializer<T, Level>(initializer))
{
}
}  // namespace hpkmediods