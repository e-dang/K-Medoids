#include <hpkmediods/utils/uniform_selectors.hpp>
#define BOOST_TEST_MODULE test_parallelism
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

namespace bdata = boost::unit_test::data;
using namespace hpkmediods;

const int64_t seed = 9;

struct UniformSelectorFixture
{
    UniformSelectorFixture() : selector(&seed, 0) {}

    ~UniformSelectorFixture() {}

    UniformSelector selector;
};

BOOST_FIXTURE_TEST_CASE(test_serial_uniform_selector, UniformSelectorFixture)
{
    int SELECTION_SIZE = 10;
    int CONTAINER_SIZE = 40;
    auto selections    = selector.select(SELECTION_SIZE, CONTAINER_SIZE);

    BOOST_TEST(selections.size() == 10);
    BOOST_TEST(std::all_of(selections.begin(), selections.end(),
                           [&CONTAINER_SIZE](const int32_t val) { return val <= CONTAINER_SIZE - 1 && val >= 0; }));
    BOOST_TEST(static_cast<int>(selections.size()) == SELECTION_SIZE);
}
