#include <hpkmediods/distances.hpp>
#include <vector>
#define BOOST_TEST_MODULE test_distances
#include <boost/test/unit_test.hpp>

namespace tt = boost::test_tools;

BOOST_AUTO_TEST_CASE(test_l2norm)
{
    L2Norm<double> distanceFunc;
    std::vector<double> vec1 = { -1.0, 2.0, 3.0, 4.0 };
    std::vector<double> vec2 = { 46.0, -2.0, 1.0, 47.0 };
    BOOST_TEST(distanceFunc(vec1.begin(), vec1.end(), vec2.begin(), vec2.end()) == 63.8592, tt::tolerance(0.01));
}
